/**
 * Copyright © 2009-2015 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StGLMessageBox_h_
#define __StGLMessageBox_h_

#include <StGLWidgets/StGLTextArea.h>
#include <StGLWidgets/StGLMenuProgram.h>

class StGLButton;
class StGLScrollArea;

/**
 * Simple widget to show message text.
 */
class StGLMessageBox : public StGLWidget {

        public:

    ST_CPPEXPORT StGLMessageBox(StGLWidget*     theParent,
                                const StString& theTitle,
                                const StString& theText);
    ST_CPPEXPORT StGLMessageBox(StGLWidget*     theParent,
                                const StString& theTitle,
                                const StString& theText,
                                const int       theWidth,
                                const int       theHeight);
    ST_CPPEXPORT virtual ~StGLMessageBox();
    ST_CPPEXPORT virtual bool stglInit();
    ST_CPPEXPORT virtual void stglResize();
    ST_CPPEXPORT virtual void stglDraw(unsigned int theView);
    ST_CPPEXPORT virtual void setVisibility(bool isVisible, bool isForce);
    ST_CPPEXPORT virtual bool tryClick(const StPointD_t& theCursorZo, const int& theMouseBtn, bool& isItemClicked);
    ST_CPPEXPORT virtual bool tryUnClick(const StPointD_t& theCursorZo, const int& theMouseBtn, bool& isItemUnclicked);

        public:

    /**
     * @return child widget which holds content of this message box
     */
    ST_LOCAL StGLScrollArea* getContent() const {
        return myContent;
    }

    /**
     * Return true if message box should be destroyed on first mouse click.
     */
    ST_LOCAL bool isContextual() const {
        return myIsContextual;
    }

    /**
     * Setup flag to automatically destroy the message box on first mouse click.
     */
    ST_LOCAL void setContextual(const bool theIsContextual) {
        myIsContextual = theIsContextual;
    }

    /**
     * Set message box title.
     */
    ST_CPPEXPORT void setTitle(const StString& theTitle);

    /**
     * Set content to the plain text.
     */
    ST_CPPEXPORT void setText(const StString& theText);

    /**
     * Append button to this message box.
     */
    ST_CPPEXPORT StGLButton* addButton(const StString& theTitle,
                                       const bool      theIsDefault = false,
                                       const int       theWidth = 0);

    ST_CPPEXPORT virtual bool doKeyDown(const StKeyEvent& theEvent);

    ST_LOCAL int getMarginLeft()   const { return myMarginLeft; }
    ST_LOCAL int getMarginRight()  const { return myMarginRight; }
    ST_LOCAL int getMarginTop()    const { return myMarginTop; }
    ST_LOCAL int getMarginBottom() const { return myMarginBottom; }

        private:   //! @name callback Slots (private overriders)

    ST_LOCAL void doMouseUnclick(const int theBtnId);

    /**
     * Move focus to the next button.
     * @param theDir determine direction (1 forward, -1 backward)
     * @return true if next button is available
     */
    ST_LOCAL bool doNextButton(const int theDir);

    ST_LOCAL void create(const StString& theTitle,
                         const StString& theText,
                         const int       theWidth,
                         const int       theHeight);

        public:    //! @name Signals

    struct {
        /**
         * @param theUserData (const size_t ) - user predefined data.
         */
        StSignal<void (const size_t )> onClickLeft;
        StSignal<void (const size_t )> onClickRight;
    } signals;

        public:    //! @name callback Slots

    ST_CPPEXPORT void doKillSelf(const size_t );

        protected:   //! @name private fields

    StGLScrollArea*   myContent;      //!< content widget
    StGLTextArea*     myTitle;        //!< window title
    StGLWidget*       myBtnPanel;     //!< panel for buttons
    StGLButton*       myDefaultBtn;   //!< default button to redirect Enter
    StGLMenuProgram   myProgram;      //!< GLSL program
    StGLVertexBuffer  myVertexBuf;    //!< vertices VBO
    int               myButtonsNb;    //!< number of buttons added to this message box


    int               myMarginLeft;   //!< margins to content
    int               myMarginRight;
    int               myMarginTop;
    int               myMarginBottom;
    int               myMinSizeY;     //!< minimal height of the message box
    bool              myToAdjustY;    //!< flag to automatically adjust height to fit content / window
    bool              myIsContextual; //!< flag to automatically destroy the message box on first mouse click

};

#endif // __StGLMessageBox_h_
