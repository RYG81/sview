/**
 * Copyright © 2009-2012 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StFPSMeter_h_
#define __StFPSMeter_h_

#include "StTimer.h"

/**
 * Simple class for FPS measurements, based on StTimer.
 */
class ST_LOCAL StFPSMeter {

        public:

    StFPSMeter()
    : myTimer(true),
      myFramesCount(0.0),
      myMeasureStep(1.0),
      myAverageFPS(0.0),
      myIsUpdated(true) {
        //
    }

    virtual ~StFPSMeter() {
        //
    }

    bool isUpdated() {
        if(myIsUpdated) {
            myIsUpdated = false;
            return true;
        } else {
            return false;
        }
    }

    /**
     * Increment frames counter.
     */
    virtual bool nextFrame() {
        myFramesCount += 1.0;
        double elapsedSec = myTimer.getElapsedTimeInSec();
        if(elapsedSec >= myMeasureStep) {
            myIsUpdated  = true;
            myAverageFPS = myFramesCount / elapsedSec;

            // restart the timer for next measurements
            myTimer.restart(); myFramesCount = 0.0;
            return true;
        }
        return false;
    }

    /**
     * Increment frames counter.
     */
    StFPSMeter& operator++() {
        nextFrame();
        return (*this);
    }

    /**
     * @return fps (double ) - average FPS.
     */
    double getAverage() const {
        return myAverageFPS;
    }

    /**
     * @return delay (double ) - average delay.
     */
    double getDelayMs() const {
        return 1000.0 / myAverageFPS;
    }

        private:

    StTimer myTimer;       //!< main timer
    double  myFramesCount; //!< frames counter
    double  myMeasureStep; //!< time interval to compute average FPS (in seconds)
    double  myAverageFPS;  //!< last computed average FPS
    bool    myIsUpdated;

};

#endif // __StFPSMeter_h_
