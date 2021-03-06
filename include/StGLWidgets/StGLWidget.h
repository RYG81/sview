/**
 * Copyright © 2009-2014 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StGLWidget_h_
#define __StGLWidget_h_

#include <StGL/StGLEnums.h>
#include <StGLStereo/StGLProjCamera.h>
#include <StGLWidgets/StGLCorner.h>
#include <StGLWidgets/StGLWidgetList.h>

#include <StThreads/StTimer.h>
#include <StSlots/StSignal.h>

#include <StCore/StVirtualKeys.h> // mouse keys

class  StGLRootWidget;
class  StGLContext;
struct StKeyEvent;

/**
 * This is abstract class for active 2D elements (buttons, menues, bars...) representation
 * with pixels (pixels -> GL) coordinates to simplify 2D mouse callbacks.
 * Please do NOT use this for native 3D GUI elements.
 */
class StGLWidget {

        public:

    ST_CPPEXPORT StGLWidget(StGLWidget* theParent,
                            const int   theLeft = 32, const int    theTop = 32,
                            const StGLCorner theCorner = StGLCorner(ST_VCORNER_TOP, ST_HCORNER_LEFT),
                            const int  theWidth = 32, const int theHeight = 32);

    ST_CPPEXPORT virtual ~StGLWidget();

        public:

    /**
     * @return root (absolute parent) widget
     */
    inline StGLRootWidget* getRoot() {
        return myRoot;
    }

    /**
     * @return parent widget (one level up)
     */
    inline StGLWidget* getParent() {
        return myParent;
    }

    inline StGLWidgetList* getChildren() {
        return &myChildren;
    }

    /**
     * @param theWidget      widget to search
     * @param theIsRecursive flag to search through children of children
     * @return true if wpecified widget is child of this object
     */
    ST_CPPEXPORT bool isChild(StGLWidget* theWidget,
                              const bool  theIsRecursive);

    /**
     * @return link to previous item in the list
     */
    inline StGLWidget* getPrev() {
        return myPrev;
    }

    /**
     * Override link to the previous item in list.
     */
    ST_CPPEXPORT void setPrev(StGLWidget* thePrev);

    /**
     * @return link to next item in the list
     */
    inline StGLWidget* getNext() {
        return myNext;
    }

    /**
     * Override link to the next item in list.
     */
    ST_CPPEXPORT void setNext(StGLWidget* theNext);

    /**
     * @return true if this item not first in the list
     */
    inline bool hasPrev() const {
        return myPrev != NULL;
    }

    /**
     * @return true if this item not last in the list
     */
    inline bool hasNext() const {
        return myNext != NULL;
    }

    /**
     * @return position relative to parent widget
     */
    ST_LOCAL const StGLCorner& getCorner() const {
        return myCorner;
    }

    /**
     * Setup widget layout.
     * @param theCorner position relative to parent widget
     */
    ST_LOCAL void setCorner(const StGLCorner& theCorner) {
        myCorner = theCorner;
    }

        public:

    /**
     * Return extra margins before the main content of the widget (text, image, etc.).
     */
    ST_LOCAL const StMarginsI& getMargins() const {
        return myMargins;
    }

    /**
     * Return extra margins before the main content of the widget (text, image, etc.).
     */
    ST_LOCAL StMarginsI& changeMargins() {
        return myMargins;
    }

    /**
     * Function returns <i>current</i> area rectangle (in pixels) relative to root area.
     * @return rectangle
     */
    inline const StRectI_t& getRectPx() const {
        return rectPx;
    }

    inline void setRectPx(const StRectI_t& rectPx) {
        isResized = true;
        this->rectPx = rectPx;
    }

    inline StRectI_t& changeRectPx() {
        isResized = true;
        return rectPx;
    }

    /**
     * Function returns <i>global</i> area rectangle (in pixels).
     * @return rectangle
     */
    ST_CPPEXPORT StRectI_t getRectPxAbsolute() const;

    /**
     * Convert coordinates relative to the parent widget
     * into absolute pixel coordinates.
     */
    ST_CPPEXPORT StRectI_t getAbsolute(const StRectI_t& theRectPx) const;

    /**
     * @param theScissorRect rectangle for OpenGL scissor test
     */
    ST_CPPEXPORT void stglScissorRect(StGLBoxPx& theScissorRect) const;

    /**
     * @param thePointZo point in Zero2One coordinates to convert
     * @return converted point in GL coordinates
     */
    ST_CPPEXPORT StPointD_t getPointGl(const StPointD_t& thePointZo) const;

    /**
     * @param thePointZo point in Zero2One coordinates
     * @return point in ActiveArea in Zero2One coordinates
     */
    ST_CPPEXPORT StPointD_t getPointIn(const StPointD_t& thePointZo) const;

    /**
     * @return true if opacity > 0.0
     */
    inline bool isVisible() const {
        return opacityValue > 0.0;
    }

    /**
     * @return true if widget can process input events
     */
    inline bool isTopWidget() const {
        return myIsTopWidget;
    }

    /**
     * Modify opacity due to visibility flag and opacity timers.
     * @param isVisible opacity UP/DOWN
     * @param isForce   change opacity to 0 or 1 instantly
     */
    ST_CPPEXPORT virtual void setVisibility(bool isVisible, bool isForce = false);

    /**
     * Returns clicking state.
     * @param theMouseBtn mouse button id
     * @return isClicked
     */
    ST_CPPEXPORT bool isClicked(const int& theMouseBtn) const;

    /**
     * Change clicking state.
     * @param theMouseBtn mouse button id
     * @param isClicked.
     */
    ST_CPPEXPORT void setClicked(const int& theMouseBtn, bool isClicked);

    /**
     * Function iterate children and self to change clicking state.
     * @param theCursorZo point in Zero2One coordinates
     * @param theMouseBtn mouse button id
     */
    ST_CPPEXPORT virtual bool tryClick(const StPointD_t& theCursorZo,
                                       const int&        theMouseBtn,
                                       bool&             isItemClicked);

    /**
     * Function iterate children and self for unclicking state.
     * @param theCursorZo point in Zero2One coordinates
     * @param theMouseBtn mouse button id
     */
    ST_CPPEXPORT virtual bool tryUnClick(const StPointD_t& theCursorZo,
                                         const int&        theMouseBtn,
                                         bool&             isItemUnclicked);

    /**
     * Process key down event. Default implementation do nothing.
     * @param theEvent key event
     * @return true if event has been processed
     */
    ST_CPPEXPORT virtual bool doKeyDown(const StKeyEvent& theEvent);

    /**
     * Process key hold event. Default implementation do nothing.
     * @param theEvent key event
     * @return true if event has been processed
     */
    ST_CPPEXPORT virtual bool doKeyHold(const StKeyEvent& theEvent);

    /**
     * Process key up event. Default implementation do nothing.
     * @param theEvent key event
     * @return true if event has been processed
     */
    ST_CPPEXPORT virtual bool doKeyUp  (const StKeyEvent& theEvent);

    /**
     * @param pointZo point in Zero2One coordinates
     * @return true if input pointer in area rectangle
     */
    ST_CPPEXPORT virtual bool isPointIn(const StPointD_t& pointZo) const;

    /**
     * Update parameters.
     * @param theCursorZo mouse cursor
     */
    ST_CPPEXPORT virtual void stglUpdate(const StPointD_t& theCursorZo);

    /**
     * Update widget and sub-widgets according to new backing store dimensions.
     */
    ST_CPPEXPORT virtual void stglResize();

    /**
     * Process initialization.
     * @return true on success
     */
    ST_CPPEXPORT virtual bool stglInit();

    /**
     * Draw area.
     */
    ST_CPPEXPORT virtual void stglDraw(unsigned int theView);

    /**
     * @return user-defined data
     */
    inline size_t getUserData() const {
        return userData;
    }

    /**
     * @param userData user-defined data
     */
    inline void setUserData(const size_t userData) {
        this->userData = userData;
    }

    /**
     * Append widget to destroy list.
     * This method should be used to destroy widget within callback processing
     * to prevent corruption during widgets iteration.
     * @param theWidget the widget to destroy
     */
    ST_CPPEXPORT virtual void destroyWithDelay(StGLWidget* theWidget);

    /**
     * Destroy all child widgets. Use carefully.
     * Automatically called on widget destruction.
     */
    ST_CPPEXPORT void destroyChildren();

        public: //! @name signals

    struct {
        /**
         * Emit callback Slot on mouse click over this widget.
         * @param theMouseBtnId (const int ) - clicked mouse button.
         */
        StSignal<void (const int )> onMouseClick;

        /**
         * Emit callback Slot on mouse unclick over this widget.
         * @param theMouseBtnId (const int ) - unclicked mouse button.
         */
        StSignal<void (const int )> onMouseUnclick;
    } signals;

        protected: //! @name methods available to inheritors

    friend class StGLRootWidget;

    /**
     * @return scale (const GLdouble& )
     */
    ST_CPPEXPORT GLdouble getScaleX() const;
    ST_CPPEXPORT GLdouble getScaleY() const;

    /**
     * @return rectGl (StRectD_t ) - widget 2D rectangle with GL coordinates.
     */
    ST_CPPEXPORT StRectD_t getRectGl() const;

    ST_CPPEXPORT void getRectGl(StArray<StGLVec2>& theVertices) const;

    /**
     * Returns link to the projection camera from root widget.
     */
    ST_CPPEXPORT StGLProjCamera* getCamera();

    /**
     * @return OpenGL context from root widget.
     */
    ST_CPPEXPORT StGLContext& getContext();

        protected: //! @name protected fields

    StGLRootWidget* myRoot;          //!< root widget - GL context
    StGLWidget*     myParent;        //!< all elements must have parent widget, NULL only for root

    StGLWidgetList  myChildren;      //!< children widgets
    StGLWidget*     myPrev;          //!< previous item in array
    StGLWidget*     myNext;          //!< next item in array

    size_t          userData;        //!< user-defined data
    StRectI_t       rectPx;          //!< area coordinates in pixels
    StMarginsI      myMargins;       //!< extra margins before main content of the widget (text, image, etc.)
    bool mouseClicked[ST_MOUSE_MAX_ID + 1]; // mouse clicking state

        protected: //! @name fields available to inheritors

    StGLCorner      myCorner;        //!< corner (left / top / right / bottom) - relative to the parent widget
    GLdouble        opacityValue;    // 1.0 means 100% visible
    double          opacityOnMs;     // time to increase opacity
    double          opacityOffMs;    // time to decrease opacity
    StTimer         opacityOnTimer;  // timer
    StTimer         opacityOffTimer; // timer
    bool            isResized;
    bool            myHasFocus;
    bool            myIsTopWidget;

};

#endif //__StGLWidget_h_
