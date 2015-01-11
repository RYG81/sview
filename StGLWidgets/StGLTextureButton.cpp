/**
 * Copyright © 2009-2015 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#include <StGLWidgets/StGLTextureButton.h>
#include <StGLWidgets/StGLRootWidget.h>

#include <StGL/StGLProgramMatrix.h>
#include <StGL/StGLResources.h>
#include <StGL/StGLContext.h>
#include <StGLCore/StGLCore20.h>

#include <StImage/StImageFile.h>
#include <StThreads/StProcess.h>

/**
 * This class defines GLSL program for textured button widget.
 */
class StGLTextureButton::Program : public StGLProgram {

        public:

    Program(const StString& theTitle)
    : StGLProgram(theTitle),
      myDispX(0.0f) {}

    StGLVarLocation getVVertexLoc()   const { return StGLVarLocation(0); }
    StGLVarLocation getVTexCoordLoc() const { return StGLVarLocation(1); }

    void setProjMat(StGLContext&      theCtx,
                    const StGLMatrix& theProjMat) {
        theCtx.core20fwd->glUniformMatrix4fv(uniProjMatLoc, 1, GL_FALSE, theProjMat);
    }

    virtual bool link(StGLContext& theCtx) {
        if(!isValid()) {
            return false;
        }

        bindAttribLocation(theCtx, "vVertex",   getVVertexLoc());
        bindAttribLocation(theCtx, "vTexCoord", getVTexCoordLoc());
        if(!StGLProgram::link(theCtx)) {
            return false;
        }

        uniProjMatLoc   = StGLProgram::getUniformLocation(theCtx, "uProjMat");
        uniDispLoc      = StGLProgram::getUniformLocation(theCtx, "uDisp");
        uniTimeLoc      = StGLProgram::getUniformLocation(theCtx, "uTime");
        uniClickedLoc   = StGLProgram::getUniformLocation(theCtx, "uClicked");
        uniParamsLoc    = StGLProgram::getUniformLocation(theCtx, "uParams");
        uniColorLoc     = StGLProgram::getUniformLocation(theCtx, "uColor");

        StGLVarLocation uniTextureLoc = StGLProgram::getUniformLocation(theCtx, "uTexture");
        if(uniTextureLoc.isValid()) {
            StGLProgram::use(theCtx);
            theCtx.core20fwd->glUniform1i(uniTextureLoc, StGLProgram::TEXTURE_SAMPLE_0);
            StGLProgram::unuse(theCtx);
        }

        return uniProjMatLoc.isValid()
            && uniTimeLoc.isValid()
            && uniClickedLoc.isValid()
            && uniParamsLoc.isValid()
            && uniTextureLoc.isValid();
    }

    void useTemp(StGLContext& theCtx) {
        StGLProgram::use(theCtx);
    }

    void use(StGLContext&    theCtx,
             const StGLVec4& theColor,
             const double    theTime,
             const double    theLightX,
             const double    theLightY,
             const double    theOpacity,
             const bool      theIsClicked,
             const GLfloat   theDispX) {
        StGLProgram::use(theCtx);
        theCtx.core20fwd->glUniform4fv(uniColorLoc, 1, theColor);
        theCtx.core20fwd->glUniform1f(uniTimeLoc, GLfloat(theTime));
        theCtx.core20fwd->glUniform1i(uniClickedLoc, (theIsClicked ? 20 : 2));
        theCtx.core20fwd->glUniform3f(uniParamsLoc, GLfloat(theLightX), GLfloat(theLightY), GLfloat(theOpacity));
        if(!stAreEqual(myDispX, theDispX, 0.0001f)) {
            myDispX = theDispX;
            theCtx.core20fwd->glUniform4fv(uniDispLoc,  1, StGLVec4(theDispX, 0.0f, 0.0f, 0.0f));
        }
    }

        private:

    GLfloat         myDispX;

    StGLVarLocation uniProjMatLoc;
    StGLVarLocation uniDispLoc;

    StGLVarLocation uniTimeLoc;
    StGLVarLocation uniClickedLoc;
    StGLVarLocation uniParamsLoc;

    StGLVarLocation uniColorLoc;

};

/**
 * This class defines matrix of GLSL programs for textured button widget.
 */
class StGLTextureButton::ButtonPrograms : public StGLProgramMatrix<1, 2, StGLTextureButton::Program> {

        public:

    /**
     * Color conversion options in GLSL Fragment Shader.
     */
    enum FragSection {
        FragSection_Main = 0, //!< section with main() function
        FragSection_GetColor, //!< read color values from textures
        FragSection_NB
    };

    /**
     * Color getter options in GLSL Fragment Shader.
     */
    enum FragGetColor {
        FragGetColor_RGB = 0,
        FragGetColor_Alpha,
        FragGetColor_NB
    };

        public:

    /**
     * Access program.
     */
    StHandle<StGLTextureButton::Program>& getProgram(const StGLTextureButton::ProgramIndex theIndex) { return myPrograms[theIndex]; }

    /**
     * Main constructor.
     */
    ButtonPrograms() {
        myTitle = "StGLTextureButton";
        registerFragmentShaderPart(FragSection_GetColor, FragGetColor_RGB,
            "uniform sampler2D uTexture;\n"
            "vec4 getColor(in vec2 theTexCoord) {\n"
            "    return texture2D(uTexture, theTexCoord);\n"
            "}\n\n");

        const char VERT_SHADER[] =
           "uniform mat4  uProjMat;\n"
           "uniform vec4  uDisp;\n"
           "uniform float uTime;\n"
           "uniform int   uClicked;\n"
            // per-vertex input
           "attribute vec4 vVertex;\n"
           "attribute vec2 vTexCoord;\n"
            // outs to fragment shader
           "varying vec2 fTexCoord;\n"

           "void main(void) {\n"
           "    fTexCoord = vTexCoord;\n"
           "    vec4 v = vVertex + uDisp;\n"
           "    if(uClicked > 10) {\n"
           "        v.z = v.z - 0.25;\n"
           "    } else {\n"
           "        v.z = v.z + sin(uTime * v.x + uTime) * cos(v.y + uTime) * 0.25;\n"
           "    }\n"
           "    gl_Position = uProjMat * v;\n"
           "}\n";

        const char FRAG_SHADER[] =
           "uniform vec3 uParams;\n"
           "varying vec2 fTexCoord;\n"
           "vec4 getColor(in vec2 theTexCoord);\n"
           "void main(void) {\n"
           "    vec4 aColor = getColor(fTexCoord);\n"
           "    float ups = 0.0;\n"
           "        float upsx = (uParams.x - fTexCoord.x);\n"
           "        upsx *= upsx;\n"
           "        float upsy = (uParams.y - fTexCoord.y);\n"
           "        upsy *= upsy;\n"
           "        ups = upsx + upsy;\n"
           "        ups = -ups * 0.2;\n"
           "        if(ups < -0.1) {\n"
           "            ups = -0.1;\n"
           "        }\n"
           "    aColor.rgb += 0.1 + ups;\n"
           "    aColor.a *= uParams.z;\n"
           "    gl_FragColor = aColor;\n"
           "}\n";

        registerVertexShaderPart  (0,                0, VERT_SHADER);
        registerFragmentShaderPart(FragSection_Main, 0, FRAG_SHADER);
    }

    /**
     * Release OpenGL resources.
     */
    ST_LOCAL virtual void release(StGLContext& theCtx) {
        StGLProgramMatrix<1, 2, StGLTextureButton::Program>::release(theCtx);
        for(int aProgIter = 0; aProgIter < StGLTextureButton::ProgramIndex_NB; ++aProgIter) {
            StHandle<StGLTextureButton::Program>& aProgram = myPrograms[aProgIter];
            if(!aProgram.isNull()) {
                aProgram->release(theCtx);
                aProgram.nullify();
            }
        }
    }

    /**
     * Initialize all programs.
     */
    bool init(StGLContext& theCtx) {
        /**const char FRAGMENT_GET_RED[] =
            "uniform sampler2D uTexture;\n"
            "uniform vec4      uColor;\n"
            "vec4 getColor(in vec2 theTexCoord) {\n"
            "     vec4 aColor = uColor;\n"
            "     aColor.a *= 1.0 - texture2D(uTexture, theTexCoord).r;\n"
            "     return aColor;\n"
            "}\n\n";*/

        const char FRAGMENT_GET_ALPHA[] =
            "uniform sampler2D uTexture;\n"
            "uniform vec4      uColor;\n"
            "vec4 getColor(in vec2 theTexCoord) {\n"
            "     vec4 aColor = uColor;\n"
            "     aColor.a *= 1.0 - texture2D(uTexture, theTexCoord).a;\n"
            "     return aColor;\n"
            "}\n\n";
        registerFragmentShaderPart(FragSection_GetColor, FragGetColor_Alpha,
                                   ///theCtx.arbTexRG ? FRAGMENT_GET_RED :
                                     FRAGMENT_GET_ALPHA);

        setFragmentShaderPart(theCtx, FragSection_GetColor, FragGetColor_RGB);
        if(!initProgram(theCtx)) {
            release(theCtx);
            return false;
        }
        myPrograms[StGLTextureButton::ProgramIndex_WaveRGB] = myActiveProgram;
        myActiveProgram.nullify();

        setFragmentShaderPart(theCtx, FragSection_GetColor, FragGetColor_Alpha);
        if(!initProgram(theCtx)) {
            release(theCtx);
            return false;
        }
        myPrograms[StGLTextureButton::ProgramIndex_WaveAlpha] = myActiveProgram;
        return true;
    }

        private:

    StHandle<StGLTextureButton::Program> myPrograms[StGLTextureButton::ProgramIndex_NB];

};

namespace {
    static const size_t SHARE_PROGRAM_ID = StGLRootWidget::generateShareId();
}

StGLTextureButton::StGLTextureButton(StGLWidget*      theParent,
                                     const int        theLeft,
                                     const int        theTop,
                                     const StGLCorner theCorner,
                                     const size_t     theFacesCount)
: StGLWidget(theParent, theLeft, theTop, theCorner),
  myColor(getRoot()->getColorForElement(StGLRootWidget::Color_IconActive)),
  myFaceId(0),
  myFacesCount(theFacesCount),
  myAnim(Anim_Wave),
  myTextures(theFacesCount),
  myTexturesPaths(theFacesCount),
  myProgram(getRoot()->getShare(SHARE_PROGRAM_ID)),
  myProgramIndex(StGLTextureButton::ProgramIndex_WaveRGB),
  myWaveTimer(false) {
    StGLWidget::signals.onMouseUnclick = stSlot(this, &StGLTextureButton::doMouseUnclick);
}

StGLTextureButton::~StGLTextureButton() {
    StGLContext& aCtx = getContext();
    myVertBuf.release(aCtx);
    myTCrdBuf.release(aCtx);
    for(size_t anIter = 0; anIter < myTextures.size(); ++anIter) {
        myTextures[anIter].release(aCtx);
    }
}

void StGLTextureButton::glWaveTimerControl() {
    if(isPointIn(getRoot()->getCursorZo())) {
        if(!myWaveTimer.isOn()) {
            myWaveTimer.restart();
        }
    } else {
        myWaveTimer.stop();
    }
}

void StGLTextureButton::setTexturePath(const StString* theTexturesPaths,
                                       const size_t    theCount) {
    size_t minCount = (theCount > myFacesCount) ? myFacesCount : theCount;
#ifdef ST_DEBUG
    if(theCount != myFacesCount) {
        ST_DEBUG_LOG_AT("WARNING, Not enough textures paths for StGLTextureButton!");
    }
#endif
    for(size_t t = 0; t < minCount; ++t) {
        myTexturesPaths.changeValue(t) = theTexturesPaths[t];
    }
}

void StGLTextureButton::setFaceId(const size_t theId) {
    if(myFaceId == theId) {
        return;
    }

    myFaceId = theId;
    myProgramIndex = StGLTexture::isAlphaFormat(myTextures[myFaceId].getTextureFormat())
                   ? StGLTextureButton::ProgramIndex_WaveAlpha
                   : StGLTextureButton::ProgramIndex_WaveRGB;
}

void StGLTextureButton::stglResize() {
    StGLWidget::stglResize();
    StGLContext& aCtx = getContext();

    // update vertices
    StArray<StGLVec2> aVertices(4);
    StRectI_t aRect = getRectPxAbsolute();
    aRect.left()   += myMargins.left;
    aRect.right()  -= myMargins.right;
    aRect.top()    += myMargins.top;
    aRect.bottom() -= myMargins.bottom;
    myRoot->getRectGl(aRect, aVertices);
    myVertBuf.init(aCtx, aVertices);

    // update projection matrix
    if(myProgram.isNull()) {
        return;
    }

    StHandle<StGLTextureButton::Program>& aProgram = myProgram->getProgram(myProgramIndex);
    if(aProgram.isNull()) {
        return;
    }

    aProgram->useTemp(aCtx);
    aProgram->setProjMat(aCtx, getRoot()->getScreenProjection());
    aProgram->unuse(aCtx);
}

bool StGLTextureButton::stglInit() {
    StHandle<StImageFile> anImage = StImageFile::create();
    StGLContext& aCtx = getContext();
    if(!anImage.isNull()) {
        const StHandle<StResourceManager>& aResMgr = getRoot()->getResourceManager();
        for(size_t aFaceIter = 0; aFaceIter < myFacesCount; ++aFaceIter) {
            const StString& aName = myTexturesPaths[aFaceIter];
            if(aName.isEmpty()) {
                ST_DEBUG_LOG("StGLTextureButton, texture for face " + aFaceIter + " not set");
                continue;
            }

            StHandle<StResource> aRes = aResMgr->getResource(aName);
            if(aRes.isNull()) {
                ST_DEBUG_LOG("StGLTextureButton, texture '" + aName + "' not found");
                continue;
            }

            uint8_t* aData     = NULL;
            int      aDataSize = 0;
            if(!aRes->isFile()
             && aRes->read()) {
                aData     = (uint8_t* )aRes->getData();
                aDataSize = aRes->getSize();
            }
            if(!anImage->load(aRes->getPath(), StImageFile::ST_TYPE_PNG, aData, aDataSize)) {
                ST_DEBUG_LOG(anImage->getState());
                continue;
            }
            changeRectPx().right()  = getRectPx().left() + (int )anImage->getSizeX() + myMargins.left + myMargins.right;
            changeRectPx().bottom() = getRectPx().top()  + (int )anImage->getSizeY() + myMargins.top  + myMargins.bottom;

            GLint anInternalFormat = GL_RGB;
            if(!StGLTexture::getInternalFormat(aCtx, anImage->getPlane(), anInternalFormat)) {
                ST_ERROR_LOG("StGLTextureButton, texture '" + aName + "' has unsupported format!");
                continue;
            }

            myTextures[aFaceIter].setTextureFormat(anInternalFormat);
            myTextures[aFaceIter].init(aCtx, anImage->getPlane());
        }
        anImage.nullify();
    }
    myProgramIndex = StGLTexture::isAlphaFormat(myTextures[myFaceId].getTextureFormat())
                   ? StGLTextureButton::ProgramIndex_WaveAlpha
                   : StGLTextureButton::ProgramIndex_WaveRGB;

    if(myProgram.isNull()) {
        myProgram.create(getRoot()->getContextHandle(), new ButtonPrograms());
        myProgram->init(aCtx);
    }

    if(!myProgram->isValid()) {
        return false;
    }

    StArray<StGLVec2> aDummyVert(4);
    StArray<StGLVec2> aTexCoords(4);
    aTexCoords[0] = StGLVec2(1.0f, 0.0f);
    aTexCoords[1] = StGLVec2(1.0f, 1.0f);
    aTexCoords[2] = StGLVec2(0.0f, 0.0f);
    aTexCoords[3] = StGLVec2(0.0f, 1.0f);

    myVertBuf.init(aCtx, aDummyVert);
    myTCrdBuf.init(aCtx, aTexCoords);
    return true;
}

void StGLTextureButton::stglDraw(unsigned int ) {
    if(!isVisible()) {
        return;
    }

    StHandle<StGLTextureButton::Program>& aProgram = myProgram->getProgram(myProgramIndex);
    if(aProgram.isNull()) {
        return;
    }

    StGLContext& aCtx = getContext();
    aCtx.core20fwd->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    aCtx.core20fwd->glEnable(GL_BLEND);
    myTextures[myFaceId].bind(aCtx);

    StRectD_t butRectGl = getRectGl();
    GLdouble butWGl = butRectGl.right() - butRectGl.left();
    GLdouble butHGl = butRectGl.top() - butRectGl.bottom();

    const StPointD_t aMouseGl = getPointGl(getRoot()->getCursorZo());;
    if(myAnim == Anim_Wave) {
        glWaveTimerControl();
    }

    aProgram->use( aCtx,
                   myColor,
                   myWaveTimer.getElapsedTimeInSec(),
                  (aMouseGl.x() - butRectGl.left()) / butWGl,
                  (butRectGl.top()  - aMouseGl.y()) / butHGl,
                   opacityValue,
                   isClicked(ST_MOUSE_LEFT),
                   getRoot()->getScreenDispX());

    myVertBuf.bindVertexAttrib(aCtx, aProgram->getVVertexLoc());
    myTCrdBuf.bindVertexAttrib(aCtx, aProgram->getVTexCoordLoc());

    aCtx.core20fwd->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    myTCrdBuf.unBindVertexAttrib(aCtx, aProgram->getVTexCoordLoc());
    myVertBuf.unBindVertexAttrib(aCtx, aProgram->getVVertexLoc());

    aProgram->unuse(aCtx);
    myTextures[myFaceId].unbind(aCtx);
    aCtx.core20fwd->glDisable(GL_BLEND);
}

bool StGLTextureButton::tryClick(const StPointD_t& cursorZo, const int& mouseBtn, bool& isItemClicked) {
    if(StGLWidget::tryClick(cursorZo, mouseBtn, isItemClicked)) {
        isItemClicked = true;
        return true;
    }
    return false;
}

bool StGLTextureButton::tryUnClick(const StPointD_t& cursorZo, const int& mouseBtn, bool& isItemUnclicked) {
    if(StGLWidget::tryUnClick(cursorZo, mouseBtn, isItemUnclicked)) {
        isItemUnclicked = true;
        return true;
    }
    return false;
}

void StGLTextureButton::doMouseUnclick(const int theBtnId) {
    if(theBtnId == ST_MOUSE_LEFT) {
        signals.onBtnClick(getUserData());
    }
}

StGLIcon::StGLIcon(StGLWidget*      theParent,
                   const int        theLeft,
                   const int        theTop,
                   const StGLCorner theCorner,
                   const size_t     theFacesCount)
: StGLTextureButton(theParent, theLeft, theTop, theCorner, theFacesCount) {
    myAnim = Anim_None;
}

bool StGLIcon::tryClick(const StPointD_t& , const int& , bool& ) {
    return false;
}

bool StGLIcon::tryUnClick(const StPointD_t& , const int& , bool& ) {
    return false;
}
