/**
 * Copyright © 2009-2015 Kirill Gavrilov <kirill@sview.ru>
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

#include <StCore/StApplication.h>

#include <StCore/StWindow.h>
#include <StCore/StSearchMonitors.h>
#include <StSettings/StSettings.h>
#include <StGL/StGLEnums.h>
#include <StGL/StGLContext.h>
#include <StGLStereo/StFormatEnum.h>
#include <StFile/StFileNode.h>
#include <StVersion.h>

#include "StEventsBuffer.h"

namespace {
    static const StCString ST_SETTING_RENDERER_AUTO = stCString("rendererPluginAuto");
    static const StCString ST_SETTING_RENDERER      = stCString("rendererPlugin");
    static const StCString ST_SETTING_AUTO_VALUE    = stCString("Auto");
}

void StApplication::doChangeDevice(const int32_t theValue) {
    if(myWindow.isNull() || !myIsOpened
    || theValue < 0
    || size_t(theValue) >= myDevices.size()) {
        return;
    }

    mySwitchTo.nullify();
    const StHandle<StOutDevice>& aDev = myDevices[theValue];
    for(size_t aRendIter = 0; aRendIter < myRenderers.size(); ++aRendIter) {
        StHandle<StWindow>& aRend = myRenderers[aRendIter];
        if(aDev->PluginId == aRend->getRendererId()) {
            if(aRend->setDevice(aDev->DeviceId)
            || aRend != myWindow) {
                mySwitchTo = aRend;
            }
            break;
        }
    }
}

StApplication::StApplication(const StHandle<StResourceManager>& theResMgr,
                             const StNativeWin_t                theParentWin,
                             const StHandle<StOpenInfo>&        theOpenInfo)
: myResMgr(theResMgr),
  myMsgQueue(new StMsgQueue()),
  myEventsBuffer(new StEventsBuffer()),
  myWinParent(theParentWin),
  myRendId(ST_SETTING_AUTO_VALUE),
  myExitCode(0),
  myGlDebug(false),
  myIsOpened(false),
  myToQuit(false) {
    stApplicationInit(theOpenInfo);
}

void StApplication::stApplicationInit(const StHandle<StOpenInfo>& theOpenInfo) {
    if(myResMgr.isNull()) {
        myResMgr = new StResourceManager();
    }
#ifdef ST_DEBUG_GL
    myGlDebug = true;
#endif
    StSettings aGlobalSettings(myResMgr, "sview");
    params.ActiveDevice = new StEnumParam(0, "Change device");
    params.ActiveDevice->signals.onChanged.connect(this, &StApplication::doChangeDevice);

    params.VSyncMode = new StEnumParam(0, "VSync mode");
    params.VSyncMode->changeValues().add("Off");
    params.VSyncMode->changeValues().add("On");
    params.VSyncMode->changeValues().add("Mixed");

    bool isOutModeAuto = true; // AUTO by default
    aGlobalSettings.loadBool(ST_SETTING_RENDERER_AUTO, isOutModeAuto);
    if(!isOutModeAuto) {
        aGlobalSettings.loadString(ST_SETTING_RENDERER, myRendId);
    }

    // add additional paths
#ifdef _WIN32
    // requires Windows XP with SP1 or higher
    StStringUtfWide aStCoreFolder = StProcess::getStCoreFolder().toUtfWide();
    SetDllDirectoryW(aStCoreFolder.toCString());
#endif
    myOpenFileInfo = theOpenInfo;
    if(myOpenFileInfo.isNull()) {
        myOpenFileInfo = parseProcessArguments();
    }
    if(myOpenFileInfo.isNull()) {
        myOpenFileInfo = new StOpenInfo();
    }

    const StArgumentsMap anArgs = myOpenFileInfo->getArgumentsMap();
    const StString ARGUMENT_PLUGIN_OUT        = "out";
    const StString ARGUMENT_PLUGIN_OUT_DEVICE = "outDevice";
    const StString ARGUMENT_GLDEBUG           = "gldebug";
    StArgument anArgRenderer = anArgs[ARGUMENT_PLUGIN_OUT];
    StArgument anArgDevice   = anArgs[ARGUMENT_PLUGIN_OUT_DEVICE];
    StArgument anArgGlDebug  = anArgs[ARGUMENT_GLDEBUG];
    if(anArgRenderer.isValid()) {
        myRendId = anArgRenderer.getValue();
    }
    //if(anArgDevice.isValid()) {
    //    aDevice = anArgDevice.getValue();
    //}
    if(anArgGlDebug.isValid()) {
        myGlDebug = true;
    }
}

StApplication::~StApplication() {
    //
}

StString StApplication::getAboutString() const {
    return "No about information";
}

bool StApplication::closingDown() const {
    return !myIsOpened;
}

bool StApplication::open(const StOpenInfo& theOpenInfo) {
    *myOpenFileInfo = theOpenInfo;
    return open();
}

bool StApplication::open() {
    if(!myWindow.isNull()) {
        return true;
    }

    StSettings aGlobalSettings(myResMgr, "sview");
    if(!mySwitchTo.isNull()) {
        myRendId = mySwitchTo->getRendererId();
        myWindow = mySwitchTo;
        mySwitchTo.nullify();
        aGlobalSettings.saveString(ST_SETTING_RENDERER,      myRendId);
        aGlobalSettings.saveBool  (ST_SETTING_RENDERER_AUTO, false);
    } else {
        if(myRenderers.isEmpty()) {
            myWindow = new StWindow(myResMgr, myWinParent);
            myWindow->setMessagesQueue(myMsgQueue);
            myWindow->params.VSyncMode = params.VSyncMode;
        } else {
            bool isAuto = myRendId.isEqualsIgnoreCase(ST_SETTING_AUTO_VALUE);
            if(!isAuto) {
                for(size_t anIter = 0; anIter < myRenderers.size(); ++anIter) {
                    StHandle<StWindow> aWin = myRenderers[anIter];
                    if(myRendId == aWin->getRendererId()) {
                        myWindow = aWin;
                        aGlobalSettings.saveString(ST_SETTING_RENDERER,      myRendId);
                        aGlobalSettings.saveBool  (ST_SETTING_RENDERER_AUTO, isAuto);
                        break;
                    }
                }

                if(myWindow.isNull()) {
                    stError(StString("Output with id '" + myRendId + "' is not found."));
                    isAuto = true;
                }
            }

            if(isAuto) {
                // autodetection
                aGlobalSettings.saveString(ST_SETTING_RENDERER,      ST_SETTING_AUTO_VALUE);
                aGlobalSettings.saveBool  (ST_SETTING_RENDERER_AUTO, isAuto);
                myWindow = myRenderers[0];
                if(!myDevices.isEmpty()) {
                    StHandle<StOutDevice> aBestDev = myDevices[0];
                    for(size_t aDevIter = 0; aDevIter < myDevices.size(); ++aDevIter) {
                        const StHandle<StOutDevice>& aDev = myDevices[aDevIter];
                        if(aDev->Priority > aBestDev->Priority) {
                            aBestDev = aDev;
                        }
                    }
                    for(size_t anIter = 0; anIter < myRenderers.size(); ++anIter) {
                        const StHandle<StWindow>& aWin = myRenderers[anIter];
                        if(aBestDev->PluginId == aWin->getRendererId()) {
                            myWindow = aWin;
                            myWindow->setDevice(aBestDev->DeviceId);
                            break;
                        }
                    }
                }
            }
        }
        myWindow->setTitle(myTitle);
    }

    // synchronize devices enumeration
    const StString aPluginId = myWindow->getRendererId();
    const StString aDeviceId = myWindow->getDeviceId();
    for(size_t aDevIter = 0; aDevIter < myDevices.size(); ++aDevIter) {
        const StHandle<StOutDevice>& aDev = myDevices[aDevIter];
        if(aPluginId == aDev->PluginId
        && aDeviceId == aDev->DeviceId) {
            params.ActiveDevice->setValue((int32_t )aDevIter);
            break;
        }
    }

    // setup GL options before window creation
    const StWinAttr anAttribs[] = {
        StWinAttr_GlDebug, (StWinAttr )myGlDebug,
        StWinAttr_NULL
    };
    myWindow->setAttributes(anAttribs);

    myIsOpened = myWindow->create();
    if(myIsOpened) {
        // connect slots
        myWindow->signals.onRedraw    = stSlot(this, &StApplication::doDrawProxy);
        myWindow->signals.onClose     = stSlot(this, &StApplication::doClose);
        myWindow->signals.onResize    = stSlot(this, &StApplication::doResize);
        myWindow->signals.onAction    = stSlot(this, &StApplication::doAction);
        myWindow->signals.onKeyDown   = stSlot(this, &StApplication::doKeyDown);
        myWindow->signals.onKeyUp     = stSlot(this, &StApplication::doKeyUp);
        myWindow->signals.onKeyHold   = stSlot(this, &StApplication::doKeyHold);
        myWindow->signals.onMouseDown = stSlot(this, &StApplication::doMouseDown);
        myWindow->signals.onMouseUp   = stSlot(this, &StApplication::doMouseUp);
        myWindow->signals.onFileDrop  = stSlot(this, &StApplication::doFileDrop);
        myWindow->signals.onNavigate  = stSlot(this, &StApplication::doNavigate);
    }

    return myIsOpened;
}

void StApplication::addRenderer(const StHandle<StWindow>& theRenderer) {
    if(theRenderer.isNull()) {
        return;
    }

    StHandle<StWindow> aRenderer = theRenderer;
    aRenderer->params.VSyncMode = params.VSyncMode; // share VSync mode between renderers
    aRenderer->setMessagesQueue(myMsgQueue);
    myRenderers.add(aRenderer);
    size_t aDevIter = myDevices.size();
    aRenderer->getDevices(myDevices);
    for(; aDevIter < myDevices.size(); ++aDevIter) {
        params.ActiveDevice->changeValues().add(myDevices[aDevIter]->Name);
    }
}

void StApplication::beforeDraw() {
    //
}

void StApplication::doDrawProxy(unsigned int theView) {
    stglDraw(!myWindow.isNull() && myWindow->isStereoOutput() ? theView : ST_DRAW_MONO);
}

void StApplication::stglDraw(unsigned int ) {
    //
}

const StHandle<StWindow>& StApplication::getMainWindow() const {
    return myWindow;
}

bool StApplication::isActive() const {
    return !myWindow.isNull()
        && myWindow->isActive();
}

const StHandle<StMsgQueue>& StApplication::getMessagesQueue() const {
    return myMsgQueue;
}

int StApplication::exec() {
    if(!myIsOpened) {
        if(!open()) {
            return 1;
        }
    }

    if(!myWindow.isNull()) {
        // just debug output Monitors' configuration
        const StSearchMonitors& aMonitors = myWindow->getMonitors();
        for(size_t aMonIter = 0; aMonIter < aMonitors.size(); ++aMonIter) {
            ST_DEBUG_LOG(aMonitors[aMonIter].toString());
        }
    }

    for(; !myWindow.isNull() && myIsOpened;) {
        processEvents();
    }
    return myExitCode;
}

void StApplication::exit(const int theExitCode) {
    myExitCode = theExitCode;
    myToQuit   = true;
}

bool StApplication::resetDevice() {
    return false;
}

void StApplication::doClose(const StCloseEvent& ) {
    exit(0);
}

const StHandle<StAction>& StApplication::getAction(const int theActionId) {
    return myActions[theActionId];
    //return myActions.at(theActionId);
}

void StApplication::addAction(const int                 theActionId,
                              const StHandle<StAction>& theAction) {
    myActions[theActionId] = theAction;
}

void StApplication::addAction(const int           theActionId,
                              StHandle<StAction>& theAction,
                              const unsigned int  theHotKey1,
                              const unsigned int  theHotKey2) {
    theAction->setHotKey1(theHotKey1);
    theAction->setHotKey2(theHotKey2);
    addAction(theActionId, theAction);
}

void StApplication::registerHotKeys() {
    myKeyActions.clear();
    for(std::map< int, StHandle<StAction> >::iterator anIter = myActions.begin();
        anIter != myActions.end(); ++anIter) {
        const StHandle<StAction>& anAction = anIter->second;
        if(anAction->getHotKey1() != 0) {
            myKeyActions[anAction->getHotKey1()] = anAction;
        }
        if(anAction->getHotKey2() != 0) {
            myKeyActions[anAction->getHotKey2()] = anAction;
        }
    }
}

void StApplication::invokeAction(const int    theActionId,
                                 const double theProgress) {
    StEvent anEvent;
    anEvent.Type            = stEvent_Action;
    anEvent.Action.ActionId = theActionId;
    anEvent.Action.Progress = theProgress;
    myEventsBuffer->append(anEvent);
}

void StApplication::doAction(const StActionEvent& theEvent) {
    std::map< int, StHandle<StAction> >::iterator anAction = myActions.find(theEvent.ActionId);
    if(anAction != myActions.end()) {
        anAction->second->doTrigger((const StEvent* )&theEvent);
    }
}

void StApplication::doKeyDown(const StKeyEvent& theEvent) {
    std::map< unsigned int, StHandle<StAction> >::iterator anAction = myKeyActions.find(theEvent.VKey | theEvent.Flags);
    if(anAction != myKeyActions.end()
    && !anAction->second->isHoldKey()) {
        anAction->second->doTrigger((const StEvent* )&theEvent);
    }
}

void StApplication::doKeyHold(const StKeyEvent& theEvent) {
    std::map< unsigned int, StHandle<StAction> >::iterator anAction = myKeyActions.find(theEvent.VKey | theEvent.Flags);
    if(anAction != myKeyActions.end()
    && anAction->second->isHoldKey()) {
        anAction->second->doTrigger((const StEvent* )&theEvent);
    }
}

void StApplication::doKeyUp    (const StKeyEvent&    ) {}

void StApplication::doResize   (const StSizeEvent&   ) {}
void StApplication::doMouseDown(const StClickEvent&  ) {}
void StApplication::doMouseUp  (const StClickEvent&  ) {}
void StApplication::doFileDrop (const StDNDropEvent& ) {}
void StApplication::doNavigate (const StNavigEvent&  ) {}

void StApplication::processEvents() {
    if(myWindow.isNull() || !myIsOpened) {
        return; // nothing to do
    }

    if(myToQuit) {
        // force Render to quit
        myWindow->beforeClose();
        myWindow->close();
        myIsOpened = false;
        myToQuit   = false;
        return;
    }

    // common callback call
    myWindow->processEvents();

    // application-specific queued events
    myEventsBuffer->swapBuffers();
    for(size_t anEventIter = 0; anEventIter < myEventsBuffer->getSize(); ++anEventIter) {
        StEvent& anEvent = myEventsBuffer->changeEvent(anEventIter);
        if(anEvent.Type == stEvent_Action) {
            doAction(anEvent.Action);
        }
    }

    // draw iteration
    beforeDraw();
    myWindow->stglDraw();

    const StString aDevice = myWindow->getDeviceId();
    const int32_t  aDevNum = params.ActiveDevice->getValue();
    if(!mySwitchTo.isNull()) {
        if(!resetDevice()) {
            myToQuit = true;
        }
        mySwitchTo.nullify();
    } else if(myWindow->isLostDevice()) {
        mySwitchTo = myWindow;
        if(!resetDevice()) {
            myToQuit = true;
        }
        mySwitchTo.nullify();
    } else if(aDevNum >= 0
           && size_t(aDevNum) < myDevices.size()
           && aDevice != myDevices[aDevNum]->DeviceId) {
        // device was changed by renderer itself - synchronize value
        const StString aPlugin = myWindow->getRendererId();
        for(size_t aDevIter = 0; aDevIter < myDevices.size(); ++aDevIter) {
            const StHandle<StOutDevice>& anOutDev = myDevices[aDevIter];
            if(aPlugin == anOutDev->PluginId
            && aDevice == anOutDev->DeviceId) {
                params.ActiveDevice->setValue((int32_t )aDevIter);
                break;
            }
        }
    }
}

StHandle<StOpenInfo> StApplication::parseProcessArguments() {
    StHandle<StOpenInfo> anInfo = new StOpenInfo();

    StArrayList<StString> anArguments = StProcess::getArguments();
    StArgumentsMap anOpenFileArgs;
    size_t aFilesCount = 0;
    bool isFilesSection = false;
    const StString ARGUMENT_FILES_SECTION     = '-';
    const StString ARGUMENT_ANY               = "--";
    const StString ARGUMENT_HELP              = "help";
    const StString ARGUMENT_FILE              = "file";
    const StString ARGUMENT_LEFT_VIEW         = "left";
    const StString ARGUMENT_RIGHT_VIEW        = "right";
    // parse extra parameters
    for(size_t aParamIter = 1; aParamIter < anArguments.size(); ++aParamIter) {
        StString aParam = anArguments[aParamIter];
        ///ST_DEBUG_LOG("aParam= '" + aParam + "'");
        if(isFilesSection) {
            // file name
            StString aFilePath = StProcess::getAbsolutePath(aParam);
            anOpenFileArgs.add(StArgument(ARGUMENT_FILE + aFilesCount++, aFilePath));
            if(!anInfo->hasPath()) {
                // first file determines MIME type (needed to autoselect Drawer plugin)
                anInfo->setPath(aFilePath);
            }
        } else if(aParam == ARGUMENT_FILES_SECTION) {
            isFilesSection = true;
        } else if(aParam.isStartsWith(ARGUMENT_ANY)) {
            // argument
            StArgument anArg; anArg.parseString(aParam.subString(2, aParam.getLength())); // cut suffix --

            if(anArg.getKey().isEqualsIgnoreCase(ARGUMENT_HELP)) {
                return NULL;
            } else if(anArg.getKey().isEqualsIgnoreCase(ARGUMENT_LEFT_VIEW)) {
                // left view
                anArg.setValue(StProcess::getAbsolutePath(anArg.getValue()));
                anOpenFileArgs.add(anArg);
                anInfo->setPath(anArg.getValue()); // left file always determines MIME type
            } else if(anArg.getKey().isEqualsIgnoreCase(ARGUMENT_RIGHT_VIEW)) {
                // right view
                anArg.setValue(StProcess::getAbsolutePath(anArg.getValue()));
                anOpenFileArgs.add(anArg);
                if(!anInfo->hasPath()) {
                    anInfo->setPath(anArg.getValue());
                }
            } else {
                // pass argument unchanged
                anOpenFileArgs.add(anArg);
            }
        } else {
            // file name
            StString aFilePath = StProcess::getAbsolutePath(aParam);
            anOpenFileArgs.add(StArgument(ARGUMENT_FILE + aFilesCount++, aFilePath));
            if(!anInfo->hasPath()) {
                // first file determines MIME type (needed to autoselect Drawer plugin)
                anInfo->setPath(aFilePath);
            }
        }
    }

    anInfo->setArgumentsMap(anOpenFileArgs);
    return anInfo;
}
