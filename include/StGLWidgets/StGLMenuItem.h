/**
 * Copyright © 2009-2015 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StGLMenuItem_h_
#define __StGLMenuItem_h_

#include <StGLWidgets/StGLShare.h>
#include <StGLWidgets/StGLTextArea.h>
#include <StGL/StGLVertexBuffer.h>

class StGLMenu;
class StGLMenuProgram;
class StGLIcon;

/**
 * Widget for item in the menu.
 */
class StGLMenuItem : public StGLTextArea {

        public:

    /**
     * Standard arrow icons.
     */
    enum Arrow {
        Arrow_None,
        Arrow_Right,
        Arrow_Bottom,
    };

        public:

    // recursively delete all submenus and that this item itself
    // should be used only for dynamic menu recreation
    ST_CPPEXPORT static void DeleteWithSubMenus(StGLMenuItem* theMenuItem);

    ST_CPPEXPORT StGLMenuItem(StGLMenu* theParent,
                              const int theLeft = 32,
                              const int theTop = 32,
                              StGLMenu* theSubMenu = NULL);

    ST_CPPEXPORT virtual ~StGLMenuItem();

    ST_CPPEXPORT virtual void stglUpdate(const StPointD_t& theCursorZo);
    ST_CPPEXPORT virtual void stglResize();
    ST_CPPEXPORT virtual bool stglInit();
    ST_CPPEXPORT virtual void stglDraw(unsigned int view);
    ST_CPPEXPORT virtual bool tryClick(const StPointD_t& theCursorZo, const int& theMouseBtn, bool& theIsItemClicked);
    ST_CPPEXPORT virtual bool tryUnClick(const StPointD_t& theCursorZo, const int& theMouseBtn, bool& theIsItemUnclicked);

    ST_LOCAL const int computeTextWidth() {
        int aWidth = 0, aHeight = 0;
        StGLTextArea::computeTextWidth(-1.0f, aWidth, aHeight);
        return aWidth;
    }

    ST_LOCAL const int computeTextWidth(const StString& theText) {
        int aWidth = 0, aHeight = 0;
        StGLTextArea::computeTextWidth(theText, -1.0f, aWidth, aHeight);
        return aWidth;
    }

    inline StGLMenu* getParentMenu() {
        return (StGLMenu* )StGLWidget::getParent();
    }

    inline StGLMenu* getSubMenu() {
        return mySubMenu;
    }

    inline bool hasSubMenu() {
       return mySubMenu != NULL;
    }

    inline bool isSelected() const {
        return myIsItemSelected;
    }

    ST_CPPEXPORT void setSelected(bool theToSelect);

    ST_CPPEXPORT void setFocus(const bool theValue);

    ST_CPPEXPORT void setHilightColor(const StGLVec4& theValue);

    ST_CPPEXPORT void resetHilightColor();

    ST_CPPEXPORT void setHilightText();

    /**
     * Setup icon.
     */
    ST_CPPEXPORT void setIcon(const StString* theImgPaths,
                              const size_t    theCount);

    /**
     * Setup icon.
     */
    ST_LOCAL void setIcon(const StString& theImgPath) {
        setIcon(&theImgPath, 1);
    }

    /**
     * Get arrow icon.
     */
    ST_LOCAL StGLMenuItem::Arrow getArrowIcon() const {
        return myArrowIcon;
    }

    /**
     * Setup arrow icon.
     */
    ST_LOCAL void setArrowIcon(const StGLMenuItem::Arrow theArrow) {
        myArrowIcon = theArrow;
    }

        public:  //! @name Signals

    struct {
        /**
         * Emit callback Slot on menu item click.
         * @param theUserData (const size_t ) - user predefined data.
         */
        StSignal<void (const size_t )> onItemClick;
    } signals;

        private: //! @name callback Slots (private overriders)

    ST_LOCAL void doMouseUnclick(const int btnId);

        protected:

    ST_LOCAL void stglUpdateTextArea(const StPointD_t& theCursorZo) {
        StGLTextArea::stglUpdate(theCursorZo);
    }

        private: //! @name private methods

    enum State {
        PASSIVE,
        HIGHLIGHT,
        CLICKED,
    };

    ST_LOCAL void stglDrawArea(const StGLMenuItem::State theState,
                               const bool                theIsOnlyArrow);

        private: //! @name private fields

    StGLMenu*                  mySubMenu;        //!< child menu
    StGLIcon*                  myIcon;           //!< optional icon
    StGLShare<StGLMenuProgram> myProgram;        //!< GLSL program
    StGLVertexBuffer           myBackVertexBuf;  //!< background vertices
    StGLVec4                   myBackColor[3];   //!< background color per state
    Arrow                      myArrowIcon;      //!< draw arrow
    bool                       myIsItemSelected; //!< navigation selection flag
    bool                       myToHilightText;  //!< highlight text instead of its box

};

/**
 * Auxiliary class representing menu item with disabled continuous pressing behavior.
 */
class StGLPassiveMenuItem : public StGLMenuItem {

        public:

    ST_CPPEXPORT StGLPassiveMenuItem(StGLMenu* theParent);
    ST_CPPEXPORT virtual void stglUpdate(const StPointD_t& theCursorZo);

};

#endif // __StGLMenuItem_h_
