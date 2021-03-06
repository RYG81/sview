/**
 * Copyright © 2007-2014 Kirill Gavrilov
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#include <StSettings/StSettings.h>

/*#if defined(__ANDROID__)

bool StSettings::load() { return true; }
bool StSettings::save() { return false; }
StSettings::StSettings(const StString& theSettingsSet) {}
StSettings::~StSettings() {}
bool StSettings::loadInt32 (const StString& ,
                            int32_t&        ) { return false; }
bool StSettings::saveInt32 (const StString& ,
                            const int32_t&  ) { return false; }
bool StSettings::loadString(const StString& ,
                            StString&       ) { return false; }
bool StSettings::saveString(const StString& ,
                            const StString& ) { return false; }

#elif !defined(_WIN32) && !defined(__APPLE__)*/
#if !defined(_WIN32) && !defined(__APPLE__)

#include <StStrings/StLogger.h>
#include <StThreads/StProcess.h>

#include <sys/stat.h>    // mkdir under Linux
#include <sys/types.h>   // mkdir under Linux

#include <libconfig.h++>
using namespace libconfig;

bool StSettings::load() {
    if(myIsLoaded) {
        return true;
    }

    try {
        myConfig->readFile(myFullFileName.toCString()); // load the configuration
    } catch(...) {
        ST_DEBUG_LOG("StSettings, failed to parse " + myFullFileName);
        return false;
    }
    return true;
}

bool StSettings::save() {
    try {
        myConfig->writeFile(myFullFileName.toCString());
    } catch(...) {
        ST_DEBUG_LOG("StSettings, failed write to " + myFullFileName);
        return false;
    }
    return true;
}

StSettings::StSettings(const StHandle<StResourceManager>& theResMgr,
                       const StString&                    theSettingsSet)
: myConfig(new Config()),
  myIsLoaded(false) {
    myFullFileName = theResMgr->getSettingsFolder() + theSettingsSet + ".cfg";
}

StSettings::~StSettings() {
    delete myConfig;
}

bool StSettings::loadInt32(const StString& theParamPath,
                           int32_t&        theValue) {
    if(!load()) {
        return false;
    }
    try {
        if(!myConfig->lookupValue(theParamPath.toCString(), theValue)) {
            ST_DEBUG_LOG("StSettings, parameter \"" + theParamPath + "\" not founded");
            return false;
        }
    } catch(...) {
        ST_DEBUG_LOG(myFullFileName + " failed");
        return false;
    }
    return true;
}

/**
 * Split parameter path into group and parameter name (theParamPath = 'group.paramName')
 */
inline bool groupName(const StString& theParamPath,
                      StString&       theGroup,
                      StString&       theParam) {
    for(StUtf8Iter anIter = theParamPath.iterator(); *anIter != 0; ++anIter) {
        if(*anIter == stUtf32_t('.')) {
            theGroup = theParamPath.subString(0, anIter.getIndex());
            theParam = theParamPath.subString(anIter.getIndex() + 1, theParamPath.getLength());
            return true;
        }
    }
    return false;
}

bool StSettings::saveInt32(const StString& theParamPath,
                           const int32_t&  theValue) {
    bool isLoaded = load(); // try to load config firstly
    try {
        if(!isLoaded || !myConfig->exists(theParamPath.toCString())) {
            Setting& aRoot = myConfig->getRoot();
            StString aParamGroup, aParamName;
            if(groupName(theParamPath, aParamGroup, aParamName)) {
                if(!myConfig->exists(aParamGroup.toCString())) {
                    Setting& aRootGrp = aRoot.add(aParamGroup.toCString(), Setting::TypeGroup);
                    aRootGrp.add(aParamName.toCString(), Setting::TypeInt);
                } else {
                    myConfig->lookup(aParamGroup.toCString()).add(aParamName.toCString(), Setting::TypeInt);
                }
            } else {
                aRoot.add(theParamPath.toCString(), Setting::TypeInt);
            }
            /**ST_DEBUG_LOG(
                "aRoot.add, " + theParamPath.toCString()
                + "= " + theValue
            );*/
        }
        myConfig->lookup(theParamPath.toCString()) = theValue;
        save();
    } catch(ParseException& ex) {
        ST_DEBUG_LOG("StSettings, error on line " + ex.getLine() + ": " + ex.getError());
        return false;
    } catch(SettingNotFoundException nfex) {
        ST_DEBUG_LOG("StSettings, setting not found: " + nfex.getPath());
        return false;
    } catch(ConfigException& cex) {
        ST_DEBUG_LOG("StSettings[" + theParamPath + "], config exception!");
        return false;
    }
    return true;
}

bool StSettings::loadString(const StString& theParamPath,
                            StString&       theValue) {
    if(!load()) {
        return false;
    }
    try {
        std::string aBuff;
        if(!myConfig->lookupValue(theParamPath.toCString(), aBuff)) {
            ST_DEBUG_LOG("StSettings, parameter \"" + theParamPath + "\" not founded");
            return false;
        }
        theValue = StString(aBuff.c_str());
    } catch(...) {
        ST_DEBUG_LOG("StSettings, " + myFullFileName + " failed");
        return false;
    }
    return true;
}

bool StSettings::saveString(const StString& theParamPath,
                            const StString& theValue) {
    bool isLoaded = load(); // try to load config firstly
    try {
        if(!isLoaded || !myConfig->exists(theParamPath.toCString())) {
            Setting& aRoot = myConfig->getRoot();
            StString aParamGroup, aParamName;
            if(groupName(theParamPath, aParamGroup, aParamName)) {
                if(!myConfig->exists(aParamGroup.toCString())) {
                    Setting& rootGrp = aRoot.add(aParamGroup.toCString(), Setting::TypeGroup);
                    rootGrp.add(aParamName.toCString(), Setting::TypeString);
                } else {
                    myConfig->lookup(aParamGroup.toCString()).add(aParamName.toCString(), Setting::TypeString);
                }
            } else {
                aRoot.add(theParamPath.toCString(), Setting::TypeString);
            }
            /**ST_DEBUG_LOG(
                "aRoot.add, " + theParamPath
                + "= " + theValue
            );*/
        }
        myConfig->lookup(theParamPath.toCString()) = theValue.toCString();
        save();
    } catch(ParseException& ex) {
        ST_DEBUG_LOG("StSettings, error on line " + ex.getLine() + ": " + ex.getError());
        return false;
    } catch(SettingNotFoundException nfex) {
        ST_DEBUG_LOG("StSettings, setting not found: " + nfex.getPath());
        return false;
    } catch(ConfigException& cex) {
        ST_DEBUG_LOG("StSettings[" + theParamPath + "], config exception!");
        return false;
    }
    return true;
}

#endif // !defined(_WIN32) && !defined(__APPLE__)
