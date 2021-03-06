/**
 * Copyright © 2011-2015 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StGLCheckbox_h_
#define __StGLCheckbox_h_

#include <StGLWidgets/StGLTextureButton.h>
#include <StSettings/StParam.h>

// forward declarations
class StGLMenuProgram;

/**
 * Widget represents classical checkbox.
 * Initialized with handle to tracked boolean property
 * thus multiple widgets can show one property without complex sync routines.
 */
class StGLCheckbox : public StGLTextureButton {

        public: //! @name overriders

    /**
     * Main constructor.
     * @param theParent       parent widget
     * @param theTrackedValue tracked boolean value
     */
    ST_CPPEXPORT StGLCheckbox(StGLWidget* theParent,
                              const StHandle<StBoolParam>& theTrackedValue,
                              const int theLeft = 32, const int theTop = 32,
                              const StGLCorner theCorner = StGLCorner(ST_VCORNER_TOP, ST_HCORNER_LEFT));

    ST_CPPEXPORT virtual ~StGLCheckbox();

    ST_CPPEXPORT virtual void stglResize();
    ST_CPPEXPORT virtual bool stglInit();
    ST_CPPEXPORT virtual void stglDraw(unsigned int view);

        public:

    /**
     * Switch this checkbox.
     */
    ST_CPPEXPORT void reverseValue();

        private: //! @name callback Slots (private overriders)

    ST_LOCAL void doMouseUnclick(const int theBtnId);

        private:

    StHandle<StBoolParam>      myTrackValue; //!< handle to tracked value
    StGLShare<StGLMenuProgram> myProgram;    //!< shared program
    StGLVertexBuffer           myVertBuf;    //!< vertices buffer

};

#endif //__StGLCheckbox_h_
