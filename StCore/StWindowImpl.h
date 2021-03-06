/**
 * Copyright © 2007-2014 Kirill Gavrilov <kirill@sview.ru>
 *
 * StCore library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StCore library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __StWindowImpl_h_
#define __StWindowImpl_h_

#include <StCore/StWindow.h>
#include <StCore/StSearchMonitors.h>
#include <StCore/StKeysState.h>

#include "StWinHandles.h"
#include "StEventsBuffer.h"

#if defined(__APPLE__)
    #include <StCocoa/StCocoaCoords.h>
    #include <IOKit/pwr_mgt/IOPMLib.h>
#elif !defined(_WIN32)
    #include <sys/sysinfo.h>
#endif

#if defined(__ANDROID__)
    struct AInputEvent;
#endif

#ifdef __OBJC__
    @class NSOpenGLContext;
#else
    class NSOpenGLContext;
#endif

class StGLContext;
class StThread;

/**
 * This class represents implementation of
 * all main routines for GLwindow managment.
 */
class StWindowImpl {

        public: //! @name main interface

    ST_LOCAL StWindowImpl(const StHandle<StResourceManager>& theResMgr,
                          const StNativeWin_t                theParentWindow);
    ST_LOCAL ~StWindowImpl();
    ST_LOCAL void close();
    ST_LOCAL void setTitle(const StString& theTitle);
    ST_LOCAL bool hasDepthBuffer() const { return attribs.GlDepthSize != 0; }
    ST_LOCAL void getAttributes(StWinAttr* theAttributes) const;
    ST_LOCAL void setAttributes(const StWinAttr* theAttributes);
    ST_LOCAL bool isActive() const { return myIsActive; }
    ST_LOCAL bool isStereoOutput() { return attribs.IsStereoOutput; }
    ST_LOCAL void setStereoOutput(bool theStereoState) { attribs.IsStereoOutput = theStereoState; }
    ST_LOCAL void show(const int );
    ST_LOCAL void hide(const int );
    ST_LOCAL void showCursor(bool toShow);
    ST_LOCAL bool isFullScreen() { return attribs.IsFullScreen; }
    ST_LOCAL void setFullScreen(bool theFullscreen);
    ST_LOCAL StRectI_t getPlacement() const { return attribs.IsFullScreen ? myRectFull : myRectNorm; }
    ST_LOCAL void setPlacement(const StRectI_t& theRect,
                               const bool       theMoveToScreen);
    ST_LOCAL StPointD_t getMousePos();
    ST_LOCAL bool create();
    ST_LOCAL void stglSwap(const int& theWinId);
    ST_LOCAL bool stglMakeCurrent(const int theWinId);
    ST_LOCAL StGLBoxPx stglViewport(const int& theWinId) const;
    ST_LOCAL void processEvents();
    ST_LOCAL void post(StEvent& theEvent);
    ST_LOCAL GLfloat getScaleFactor() const {
        return myMonitors[myWinOnMonitorId].getScale();
    }
    ST_LOCAL const StSearchMonitors& getMonitors() const {
        return myMonitors;
    }

    ST_LOCAL bool isParentOnScreen() const;

        public: //! @name clipboard

    ST_LOCAL bool toClipboard(const StString& theText);

#if defined(__linux__)
    StString myTextToCopy;
#endif

        public: //! @name additional

    ST_LOCAL void updateChildRect();
#ifdef _WIN32
    ST_LOCAL bool wndCreateWindows(); // called from non-main thread
    ST_LOCAL LRESULT stWndProc(HWND , UINT , WPARAM , LPARAM );
#elif defined(__APPLE__)
    ST_LOCAL void doCreateWindows(NSOpenGLContext* theGLContextMaster,
                                  NSOpenGLContext* theGLContextSlave);
#endif

        public:

    ST_LOCAL void updateMonitors();
    ST_LOCAL void updateWindowPos();
    ST_LOCAL void updateActiveState();
    ST_LOCAL void updateBlockSleep();
#if defined(__ANDROID__)
    ST_LOCAL void onAndroidInput(const AInputEvent* theEvent, bool& theIsProcessed);
    ST_LOCAL void onAndroidCommand(int32_t theCommand);
    ST_LOCAL bool onAndroidInitWindow();
#elif defined(__linux__)
    ST_LOCAL void parseXDNDClientMsg();
    ST_LOCAL void parseXDNDSelectionMsg();

    ST_LOCAL static Bool stXWaitMapped(Display* theDisplay,
                                       XEvent*  theEvent,
                                       char*    theArg);
#endif

    /**
     * Swap events read/write buffers
     * and pop cached events from read buffer.
     */
    ST_LOCAL void swapEventsBuffers();

    /**
     * @return uptime in seconds for event
     */
    ST_LOCAL double getEventTime() const;

    /**
     * @return uptime in seconds (convert from 32-bit integer)
     */
    ST_LOCAL double getEventTime(const uint32_t theTime) const;

    /**
     * Setup common fields of event structure (Type, Flags)
     * and post key down event and perform post-processing.
     */
    ST_LOCAL void postKeyDown(StEvent& theEvent);

    /**
     * Setup common fields of event structure (Type, Flags)
     * and post key up event and perform post-processing.
     */
    ST_LOCAL void postKeyUp(StEvent& theEvent);

    /**
     * Tiles configuration (multiple viewports within the same window).
     */
    enum TiledCfg {
        TiledCfg_Separate,     //!< dedicated windows - default
        TiledCfg_MasterSlaveX, //!< Master at left   / Slave at right
        TiledCfg_SlaveMasterX, //!< Master at right  / Slave at left
        TiledCfg_MasterSlaveY, //!< Master at top    / Slave at bottom
        TiledCfg_SlaveMasterY, //!< Master at bottom / Slave at top
    };

    ST_LOCAL void getTiledWinRect(StRectI_t& theRect) const;
    ST_LOCAL void correctTiledCursor(int& theLeft, int& theTop) const;

    ST_LOCAL void convertRectToBacking(StGLBoxPx& theRect,
                                       const int  theWinId) const;

    ST_LOCAL void updateSlaveConfig() {
        myMonSlave.idSlave = int(attribs.SlaveMonId);
        if(attribs.Slave == StWinSlave_slaveFlipX) {
            myMonSlave.xAdd = 0; myMonSlave.xSub = 1;
            myMonSlave.yAdd = 1; myMonSlave.ySub = 0;
        } else if(attribs.Slave == StWinSlave_slaveFlipY) {
            myMonSlave.xAdd = 1; myMonSlave.xSub = 0;
            myMonSlave.yAdd = 0; myMonSlave.ySub = 1;
        } else {
            myMonSlave.xAdd = 1; myMonSlave.xSub = 0;
            myMonSlave.yAdd = 1; myMonSlave.ySub = 0;
        }
    }

    ST_LOCAL int getMasterLeft() const {
        return myMonitors[myMonSlave.idMaster].getVRect().left();
    }

    ST_LOCAL int getMasterTop() const {
        return myMonitors[myMonSlave.idMaster].getVRect().top();
    }

    /**
     * @return true if slave window should be displayed on independent monitor.
     */
    ST_LOCAL bool isSlaveIndependent() const {
        return attribs.Slave == StWinSlave_slaveSync
            || attribs.Slave == StWinSlave_slaveFlipX
            || attribs.Slave == StWinSlave_slaveFlipY;
    }

    ST_LOCAL int getSlaveLeft() const {
        if(!isSlaveIndependent()) {
            return myMonitors[getPlacement().center()].getVRect().left();
        } else if(attribs.IsFullScreen) {
            return myMonitors[myMonSlave.idSlave].getVRect().left();
        } else {
            const StMonitor& aMonMaster = myMonitors[getPlacement().center()]; // detect from current location
            return myMonSlave.xAdd * (myMonitors[myMonSlave.idSlave].getVRect().left()  + myRectNorm.left()  - aMonMaster.getVRect().left())
                 + myMonSlave.xSub * (myMonitors[myMonSlave.idSlave].getVRect().right() - myRectNorm.right() + aMonMaster.getVRect().left());
        }
    }

    ST_LOCAL int getSlaveWidth() const {
        if(attribs.Slave == StWinSlave_slaveHTop2Px) {
            return 2;
        } else if(attribs.Slave == StWinSlave_slaveHLineTop
               || attribs.Slave == StWinSlave_slaveHLineBottom) {
            return myMonitors[getPlacement().center()].getVRect().width();
        } else if(attribs.IsFullScreen) {
            return myMonitors[myMonSlave.idSlave].getVRect().width();
        } else {
            return myRectNorm.width();
        }
    }

    ST_LOCAL int getSlaveTop() const {
        if(attribs.Slave == StWinSlave_slaveHLineBottom) {
            return myMonitors[getPlacement().center()].getVRect().bottom() - 1;
        } else if(attribs.Slave == StWinSlave_slaveHLineTop
               || attribs.Slave == StWinSlave_slaveHTop2Px) {
            return myMonitors[getPlacement().center()].getVRect().top();
        } else if(attribs.IsFullScreen) {
            return myMonitors[myMonSlave.idSlave].getVRect().top();
        } else {
            const StMonitor& aMonMaster = myMonitors[getPlacement().center()]; // detect from current location
            return myMonSlave.yAdd * (myMonitors[myMonSlave.idSlave].getVRect().top()    + myRectNorm.top()    - aMonMaster.getVRect().top())
                 + myMonSlave.ySub * (myMonitors[myMonSlave.idSlave].getVRect().bottom() - myRectNorm.bottom() + aMonMaster.getVRect().top());
        }
    }

    ST_LOCAL int getSlaveHeight() const {
        if(attribs.Slave == StWinSlave_slaveHLineBottom
        || attribs.Slave == StWinSlave_slaveHTop2Px) {
            return 1;
        } else if(attribs.Slave == StWinSlave_slaveHLineTop) {
            return 10;
        } else if(attribs.IsFullScreen) {
            return myMonitors[myMonSlave.idSlave].getVRect().height();
        } else {
            return myRectNorm.height();
        }
    }

        public: //! @name fields

    enum BlockSleep {
        BlockSleep_OFF,     //!< do not block sleeping
        BlockSleep_SYSTEM,  //!< block system to sleep but not display
        BlockSleep_DISPLAY, //!< block display to sleep
    };

    static StAtomic<int32_t> myFullScreenWinNb; //!< shared counter for fullscreen windows to detect inactive state

    StHandle<StResourceManager> myResMgr; //!< file resources manager
    StHandle<StGLContext> myGlContext;
    StWinHandles       myMaster;          //!< master window
    StWinHandles       mySlave;           //!< slave  window (optional)
    StNativeWin_t      myParentWin;       //!< parent window (optional, for embedding)

    StString           myWindowTitle;     //!< window caption
    int                myInitState;       //!< initialization error code

    StPointD_t         myMousePt;         //!< mouse coordinates to track activity
    StRectI_t          myRectNorm;        //!< master window coordinates in normal     state
    StRectI_t          myRectFull;        //!< master window coordinates in fullscreen state
    StRectI_t          myRectNormPrev;    //!< window rectangle to track changes
    StMarginsI         myDecMargins;      //!< decoration margins

    StSearchMonitors   myMonitors;        //!< available monitors
    int                myMonMasterFull;   //!< monitor for fullscreen (-1 - use monitor where window currently placed)
    StSlaveWindowCfg_t myMonSlave;        //!< slave window options
    size_t             mySyncCounter;
    int                myWinOnMonitorId;  //!< monitor id where window is placed
    int                myWinMonScaleId;   //!< monitor id from which scale factor is applied
    TiledCfg           myTiledCfg;        //!< tiles configuration (multiple viewports within the same window)

#ifdef _WIN32
    StSearchMonitors   myMsgMonitors;     //!< available monitors, accessed from message thread
    POINT              myPointTest;       //!< temporary point object to verify cached window position
    StHandle<StThread> myMsgThread;       //!< dedicated thread for window message loop
    StCondition        myEventInitWin;    //!< special event waited from createWindows() thread
    StCondition        myEventInitGl;
    StCondition        myEventQuit;       //!< quit message thread event
    StCondition        myEventCursorShow;
    StCondition        myEventCursorHide;
    MSG                myEvent;           //!< message for windows' message loop
    bool               myIsVistaPlus;     //!< system is Vista+
#elif defined(__APPLE__)
    StCocoaCoords      myCocoaCoords;
    IOPMAssertionLevel mySleepAssert;     //!< prevent system going to sleep
#elif defined(__ANDROID__)
    //
#else
    XEvent             myXEvent;
    char               myXInputBuff[32];
#endif

    bool               myToResetDevice;   //!< indicate device lost state
    bool               myIsUpdated;       //!< helper flag on window movements updates
    bool               myIsActive;        //!< window visible state
    BlockSleep         myBlockSleep;      //!< indicates that display sleep was blocked or not
    volatile bool      myIsDispChanged;   //!< monitors reconfiguration event

    /**
     * Window attributes structure for internal use.
     * Notice that some options could not be changed after window was created!
     */
    struct {
        bool       IsNoDecor;          //!< to decorate master window or not (will be ignored in case of embedded and fullscreen)
        bool       IsStereoOutput;     //!< indicate stereoscopic output on / off (used for interconnection between modules)
        bool       IsGlStereo;         //!< request OpenGL hardware accelerated QuadBuffer
        bool       IsGlDebug;          //!< request OpenGL debug context
        int8_t     GlDepthSize;        //!< OpenGL Depth Buffer size
        bool       IsFullScreen;       //!< to show in fullscreen mode
        bool       IsExclusiveFullScr; //!< use exclusive fullscreen mode (improve performance, prevent other applications)
        bool       IsHidden;           //!< to hide the window
        bool       IsSlaveHidden;      //!< to hide the only slave window
        bool       ToHideCursor;       //!< to hide cursor
        bool       ToBlockSleepSystem; //!< prevent system  going to sleep (display could be turned off)
        bool       ToBlockSleepDisplay;//!< prevent display going to sleep
        bool       AreGlobalMediaKeys; //!< register system hot-key to capture multimedia even without window focus
        StWinSlave Slave;              //!< slave configuration
        int8_t     SlaveMonId;         //!< on which monitor show slave window (1 by default)
        StWinSplit Split;              //!< split window configuration
        bool       ToAlignEven;        //!< align window position to even numbers
    } attribs;

    struct {
        StSignal<void (const StCloseEvent&  )>* onClose;
        StSignal<void (const StSizeEvent&   )>* onResize;
        StSignal<void (const StSizeEvent&   )>* onAnotherMonitor;
        StSignal<void (const StKeyEvent&    )>* onKeyUp;
        StSignal<void (const StKeyEvent&    )>* onKeyDown;
        StSignal<void (const StKeyEvent&    )>* onKeyHold;
        StSignal<void (const StClickEvent&  )>* onMouseUp;
        StSignal<void (const StClickEvent&  )>* onMouseDown;
        StSignal<void (const StDNDropEvent& )>* onFileDrop;
        StSignal<void (const StNavigEvent&  )>* onNavigate;
        StSignal<void (const StActionEvent& )>* onAction;
    } signals;

    class StSyncTimer : public StTimer {

            public:

        /**
         * Constructor.
         */
        StSyncTimer()
        : StTimer(),
          myLastSyncMicroSec(0.0),
          mySyncMicroSec(0.0f) {
        #if defined(_WIN32)
            myGetTick64 = NULL;
        #endif
        }

        /**
         * Initialize timer from current UpTime using system API.
         */
        ST_LOCAL void initUpTime();

        /**
         * @return UpTime computed with this timer
         */
        ST_LOCAL double getUpTime() const {
            return getElapsedTime() + double(mySyncMicroSec) * 0.000001;
        }

        /**
         * @return true each 2 minutes
         */
        ST_LOCAL bool isResyncNeeded() const {
            return (getElapsedTimeInMicroSec() - myLastSyncMicroSec) > 120000000.0;
        }

        /**
         * Compute correction for high-performance timer value
         * relative to real system UpTime.
         */
        ST_LOCAL void resyncUpTime();

        /**
         * Retrieve UpTime using system API.
         */
        ST_LOCAL double getUpTimeFromSystem() const {
        #ifdef _WIN32
            const uint64_t anUptime = (myGetTick64 != NULL) ? myGetTick64() : (uint64_t )GetTickCount();
            return double(anUptime) * 0.001;
        #elif defined(__APPLE__)
            // use function from CoreServices to retrieve system uptime
            const Nanoseconds anUpTimeNano = AbsoluteToNanoseconds(UpTime());
            return double((*(uint64_t* )&anUpTimeNano) / 1000) * 0.000001;
        #else
            // read system uptime (in seconds)
            struct sysinfo aSysInfo;
            ::sysinfo(&aSysInfo);
            return double(aSysInfo.uptime);
        #endif
        }

    #ifdef _WIN32
        typedef ULONGLONG (WINAPI *GetTickCount64_t)();
        GetTickCount64_t   myGetTick64;
    #endif
        double             myLastSyncMicroSec; //!< timestamp of last synchronization
        float              mySyncMicroSec;     //!< should be replaced by double with atomic accessors

    };

    StKeysState    myKeysState;        //!< cached keyboard state
    StSyncTimer    myEventsTimer;
    StEventsBuffer myEventsBuffer;     //!< window events double buffer
    StEvent        myStEvent;          //!< temporary event object (to be used in message loop thread)
    StEvent        myStEventAux;       //!< extra temporary event object (to be used in StWindow creation thread)
    double         myLastEventsTime;   //!< time when processEvents() was last called
    bool           myEventsThreaded;
    bool           myIsMouseMoved;

};

#endif // __StWindowImpl_h_
