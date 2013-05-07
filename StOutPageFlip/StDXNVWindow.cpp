/**
 * Copyright © 2009-2012 Kirill Gavrilov <kirill@sview.ru>
 *
 * StOutPageFlip library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StOutPageFlip library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef _WIN32
#include "StDXNVWindow.h"

#include <StThreads/StFPSMeter.h>

namespace {

    /**
     * This is absolutely hacking function
     * that retrieves slave window handle for current StWindow instance.
     * Code may become broken in future!
     */
    static bool getStNativeWin(StWindow* theStWin,
                               HWND&     theMaster,
                               HWND&     theSlave) {
        DWORD aPid = 0;
        DWORD aTid = 0;
        DWORD aMyPid = (DWORD )StProcess::getPID();
        LONG_PTR aStWinPtr = (LONG_PTR )theStWin;

        // at first iteration we search for master window (to detect StWindow message thread ID)
        for(HWND aGlWin = GetTopWindow(NULL); aGlWin != NULL; aGlWin = GetNextWindow(aGlWin, GW_HWNDNEXT)) {
            aTid = GetWindowThreadProcessId(aGlWin, &aPid);
            if(aPid == aMyPid
            && GetWindowLongPtr(aGlWin, int(GWLP_USERDATA)) == aStWinPtr) {
                theMaster = aGlWin;
                break;
            }
        }
        if(aPid != aMyPid) {
            return false;
        }

        // at second iteration we detect the slave window (the only window in thread with 0 user data)
        for(HWND aGlWin = GetTopWindow(NULL); aGlWin != NULL; aGlWin = GetNextWindow(aGlWin, GW_HWNDNEXT)) {
            if(aTid == GetWindowThreadProcessId(aGlWin, NULL)
            && GetWindowLongPtr(aGlWin, int(GWLP_USERDATA)) == 0) {
                theSlave = aGlWin;
                return true;
            }
        }
        return false;
    }

    static const wchar_t ST_D3DWIN_CLASSNAME[] = L"StDirect3D";
    static StAtomic<int32_t> ST_D3DWIN_CLASSCOUNTER(0);

};

StDXNVWindow::StDXNVWindow(const size_t     theFboSizeX,
                           const size_t     theFboSizeY,
                           const StMonitor& theMonitor,
                           StWindow*        theStWin)
: myBufferL(NULL),
  myBufferR(NULL),
  myFboSizeX(theFboSizeX),
  myFboSizeY(theFboSizeY),
  myWinD3d(NULL),
  myWinMaster(NULL),
  myDxManager(),
  myDxSurface(),
  myMonitor(theMonitor),
  myStWin(theStWin),
  myMutex(),
  hEventReady(NULL),
  hEventQuit(NULL),
  hEventShow(NULL),
  hEventHide(NULL),
  hEventUpdate(NULL),
  myIsOwnWin(false) {
    //
    stMemSet(myMouseState, 0, sizeof(myMouseState));
    stMemSet(myVKeyState,  0, sizeof(myVKeyState));
    // we create Win32 event directly (not StCondition) to use it with MsgWaitForMultipleObjects()
    hEventReady  = CreateEvent(0, true, false, NULL);
    hEventQuit   = CreateEvent(0, true, false, NULL);
    hEventShow   = CreateEvent(0, true, false, NULL);
    hEventHide   = CreateEvent(0, true, false, NULL);
    hEventUpdate = CreateEvent(0, true, false, NULL);

    getStNativeWin(myStWin, myWinMaster, myWinD3d);
    myIsOwnWin  = (myWinD3d == NULL);
}

bool StDXNVWindow::allocateBuffers() {
    if(myBufferL == NULL) {
        myBufferL = stMemAllocAligned<unsigned char*>(myFboSizeX * myFboSizeY * 4, 16);
    }
    if(myBufferR == NULL) {
        myBufferR = stMemAllocAligned<unsigned char*>(myFboSizeX * myFboSizeY * 4, 16);
    }
    return myBufferL != NULL
        && myBufferR != NULL;
}

void StDXNVWindow::releaseBuffers() {
    stMemFreeAligned(myBufferL);
    stMemFreeAligned(myBufferR);
    myBufferL = myBufferR = NULL;
}

StDXNVWindow::~StDXNVWindow() {
    CloseHandle(hEventReady);
    CloseHandle(hEventQuit);
    CloseHandle(hEventShow);
    CloseHandle(hEventHide);
    CloseHandle(hEventUpdate);
    releaseBuffers();

    if(!myWinClass.isEmpty() && UnregisterClassW(myWinClass.toCString(), GetModuleHandle(NULL)) == 0) {
        ST_DEBUG_LOG("StDXNVWindow, FAILED to unregister Class= '" + myWinClass.toUtf8() + "'");
    }
}

// static winproc function - used for callback on windows
LRESULT CALLBACK StDXNVWindow::wndProcWrapper(HWND   theWnd,
                                              UINT   theMsg,
                                              WPARAM theParamW,
                                              LPARAM theParamL) {
    if(theMsg == WM_CREATE) {
        // save pointer to our class instance (sended on window create) to window storage
        LPCREATESTRUCT pCS = (LPCREATESTRUCT )theParamL;
        SetWindowLongPtr(theWnd, (int )(GWLP_USERDATA), (LONG_PTR )pCS->lpCreateParams);
    }
    // get pointer to our class instance
    StDXNVWindow* aThis = (StDXNVWindow* )GetWindowLongPtr(theWnd, (int )(GWLP_USERDATA));
    if(aThis != NULL) {
        return aThis->wndProcFunction(theWnd, theMsg, theParamW, theParamL);
    } else {
        return DefWindowProc(theWnd, theMsg, theParamW, theParamL);
    }
}

LRESULT StDXNVWindow::wndProcFunction(HWND   theWnd,
                                      UINT   theMsg,
                                      WPARAM theParamW,
                                      LPARAM theParamL) {
    // we do stupid checks here...
    if(myStWin->isFullScreen() && myStWin->isStereoOutput()) {
        if(theMsg == WM_MOUSEWHEEL) {
            int zDelta = GET_WHEEL_DELTA_WPARAM(theParamW);
            int mbtn = (zDelta > 0) ? ST_MOUSE_SCROLL_V_UP : ST_MOUSE_SCROLL_V_DOWN;
            updateMouseBtn(mbtn, true);  // emulate down
            updateMouseBtn(mbtn, false); // emulate up
        }

        updateMouseBtn(ST_MOUSE_LEFT,   GetAsyncKeyState(GetSystemMetrics(SM_SWAPBUTTON) == 0 ? VK_LBUTTON : VK_RBUTTON) != 0);
        updateMouseBtn(ST_MOUSE_RIGHT,  GetAsyncKeyState(GetSystemMetrics(SM_SWAPBUTTON) != 0 ? VK_LBUTTON : VK_RBUTTON) != 0);
        updateMouseBtn(ST_MOUSE_MIDDLE, GetAsyncKeyState(VK_MBUTTON) != 0);
        for(int vkeyId = 0; vkeyId < 255; ++vkeyId) {
            updateKeyState(vkeyId, GetAsyncKeyState(vkeyId) != 0);
        }
    }
    return DefWindowProc(theWnd, theMsg, theParamW, theParamL);
}

void StDXNVWindow::updateMouseBtn(const int theBtnId, bool theNewState) {
    if(myMouseState[theBtnId] != theNewState) {
        myMouseState[theBtnId] = theNewState;
        StMessage_t stMsg;
        StMouseMessage_t mouseMsg;
        stMsg.data = (void* )&mouseMsg;
        mouseMsg.point  = myStWin->getMousePos();
        mouseMsg.button = theBtnId;
        stMsg.uin = theNewState ? StMessageList::MSG_MOUSE_DOWN_APPEND : StMessageList::MSG_MOUSE_UP_APPEND;
        myStWin->appendMessage(stMsg);
    }
}

void StDXNVWindow::updateKeyState(const int theVKeyId, bool theNewState) {
    if(myVKeyState[theVKeyId] != theNewState) {
        myVKeyState[theVKeyId] = theNewState;
        StMessage_t stMsg;
        stMsg.data = (void* )theVKeyId;
        stMsg.uin  = theNewState ? StMessageList::MSG_KEY_DOWN_APPEND : StMessageList::MSG_KEY_UP_APPEND;
        myStWin->appendMessage(stMsg);
    }
}

static StString stLastError() {
    wchar_t* aMsgBuff = NULL;
    DWORD anErrorCode = GetLastError();
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, anErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (wchar_t* )&aMsgBuff, 0, NULL);

    StString aResult;
    if(aMsgBuff != NULL) {
        aResult = StString(aMsgBuff) + " (" + int(anErrorCode) + ")";
        LocalFree(aMsgBuff);
    } else {
        aResult = StString("Error code #") + int(anErrorCode);
    }
    return aResult;
}

bool StDXNVWindow::initWinAPIWindow() {
    HINSTANCE anAppInstance = GetModuleHandle(NULL);
    if(myWinClass.isEmpty()) {
        myWinClass = StStringUtfWide(ST_D3DWIN_CLASSNAME) + StStringUtfWide(ST_D3DWIN_CLASSCOUNTER.increment());
        WNDCLASSW aWinClass;  // Windows Class Structure
        aWinClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        aWinClass.lpfnWndProc   = (WNDPROC )wndProcWrapper;
        aWinClass.cbClsExtra    = 0;
        aWinClass.cbWndExtra    = 0;
        aWinClass.hInstance     = anAppInstance;
        aWinClass.hIcon         = LoadIcon(anAppInstance, L"A");
        aWinClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
        aWinClass.hbrBackground = NULL;
        aWinClass.lpszMenuName  = NULL;
        aWinClass.lpszClassName = myWinClass.toCString();
        if(RegisterClassW(&aWinClass) == 0) {
            stError("StDXWindow, Failed to RegisterClass '" + myWinClass.toUtf8() + "' (" + stLastError() + ")");
            myWinClass.clear();
            return false;
        }
    }
    StRectI_t aMonRect = myMonitor.getVRect();
    myWinD3d = CreateWindowExW(
        WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
        myWinClass.toCString(),             // Class Name
        L"sView Direct3D output",           // Window Title
        WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        aMonRect.left(), aMonRect.top(), aMonRect.width(), aMonRect.height(),
        NULL,                               // Parent Window
        NULL,                               // No Menu
        anAppInstance,
        this                                // Put pointer to class, getted on WM_CREATE (to use class-member function)
    );
    if(myWinD3d == NULL) {
        stError("StDXWindow, Failed to CreateWindow (" + stLastError() + ")");
        return false;
    }
    ST_DEBUG_LOG_AT("StDXWindow, Created help window");
    myIsOwnWin = true;
    return true;
}

void StDXNVWindow::dxLoop() {
    if(myWinD3d == NULL && !initWinAPIWindow()) {
        return;
    }
    myDxManager = new StDXManager(); //create our direct Manager
    if(!myDxManager->init(myWinD3d, int(getD3dSizeX()), int(getD3dSizeY()), false, myMonitor)) {
        ST_ERROR_LOG_AT("Failed to INIT StDXManager");
        return;
    }

    bool aShowState = false;
    POINT aCursorPos;

    HANDLE waitEvents[4] = {hEventQuit, hEventShow, hEventHide, hEventUpdate};

    MSG aMsg;
    stMemSet(&aMsg, 0, sizeof(aMsg));
    SetEvent(hEventReady);
    for(;;) {
        switch(MsgWaitForMultipleObjects(4, waitEvents, FALSE, INFINITE, QS_ALLINPUT)) {
            case WAIT_OBJECT_0: {
                // Event 1 (hEventQuit) has been set. If the event was created as autoreset, it has also been reset
                ST_DEBUG_LOG_AT("releaseDXWindow()");
                myDxSurface.nullify();
                /// TODO (Kirill Gavrilov#9) do we need this call here?
                myDxManager->reset(myWinD3d, int(2), int(2), false);
                myDxManager.nullify();
                if(myIsOwnWin) {
                    PostQuitMessage(0);
                    DestroyWindow(myWinD3d);
                }
                return;
            }
            case WAIT_OBJECT_0 + 1: {
                // Event 2 (hEventShow) has been set. If the event was created as autoreset, it has also been reset
                aShowState = true;
                myDxSurface.nullify();
                GetCursorPos(&aCursorPos); // backup cursor position

                if(myDxManager->reset(myWinD3d, int(getD3dSizeX()), int(getD3dSizeY()), aShowState)) {
                    myDxSurface = new StDXNVSurface(getD3dSizeX() * 2, getD3dSizeY());
                    if(!myDxSurface->create(myDxManager->getD3DDevice())) {
                        //stError(ST_TEXT("Output plugin, Failed to create Direct3D surface"));
                        ST_ERROR_LOG("StDXNVWindow, Failed to create Direct3D surface");
                        ///return;
                    }
                } else {
                    myDxManager->reset(myWinD3d, int(100), int(100), false);
                    // workaround to ensure something show on the screen
                    myStWin->setFullScreen(false);
                    aShowState = false;
                    ST_ERROR_LOG("StDXNVWindow, Failed to reset Direct3D device into FULLSCREEN state");
                }
                if(myIsOwnWin) {
                    ShowWindow(myWinD3d, SW_SHOWMAXIMIZED);
                    UpdateWindow(myWinD3d); // debug staff
                } else {
                    // set keyboard focus
                    SetForegroundWindow(myWinMaster);
                }
                ResetEvent(hEventShow);
                SetCursorPos(aCursorPos.x, aCursorPos.y); // restore cursor position
                break;
            }
            case WAIT_OBJECT_0 + 2: {
                // Event 3 (hEventHide) has been set. If the event was created as autoreset, it has also been reset
                aShowState = false;
                myDxSurface.nullify();
                GetCursorPos(&aCursorPos); // backup cursor position

                // release unused memory
                myMutex.lock();
                releaseBuffers();
                myMutex.unlock();

                if(!myDxManager->reset(myWinD3d, int(getD3dSizeX()), int(getD3dSizeY()), aShowState)) {
                    ST_ERROR_LOG("StDXNVWindow, Failed to reset Direct3D device into WINDOWED state");
                }
                if(myIsOwnWin) {
                    ShowWindow(myWinD3d, SW_HIDE);
                    UpdateWindow(myWinD3d);
                }
                ResetEvent(hEventHide);
                SetCursorPos(aCursorPos.x, aCursorPos.y); // restore cursor position
                break;
            }
            case WAIT_OBJECT_0 + 3: {
                // Event 4 (hEventUpdate) has been set. If the event was created as autoreset, it has also been reset
                if(!aShowState) {
                    ResetEvent(hEventUpdate);
                    break;
                }

                myMutex.lock();
                if(myBufferL != NULL && myBufferR != NULL) {
                    myDxSurface->update(myFboSizeX, myFboSizeY, myBufferR, false);
                    myDxSurface->update(myFboSizeX, myFboSizeY, myBufferL, true);
                }
                myMutex.unlock();
                myDxManager->beginRender();
                myDxSurface->render(myDxManager->getD3DDevice());
                ResetEvent(hEventUpdate);
                myDxManager->endRender();

                if(!myDxManager->swap()) {
                    SetEvent(hEventShow);
                }

                //static StFPSMeter dxFPSMeter;
                //if((++dxFPSMeter).isUpdated()) { ST_DEBUG_LOG("DX FPS= " + dxFPSMeter.getAverage()); }
                break;
            }
            case WAIT_OBJECT_0 + 4: {
                // a windows message has arrived
                while(PeekMessage(&aMsg, NULL, 0U, 0U, PM_REMOVE)) {
                    TranslateMessage(&aMsg); // Translate The Message
                    DispatchMessage(&aMsg);  // Dispatch The Message
                }
                break; // break from switch
            }
        }
    }
}

#endif // _WIN32
