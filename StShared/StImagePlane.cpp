/**
 * Copyright © 2010-2013 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#include <StImage/StImagePlane.h>

StString StImagePlane::formatImgFormat(ImgFormat theImgFormat) {
    switch(theImgFormat) {
        case ImgGray:    return "ImgGray";
        case ImgGray16:  return "ImgGray16";
        case ImgRGB:     return "ImgRGB";
        case ImgBGR:     return "ImgBGR";
        case ImgRGB32:   return "ImgRGB32";
        case ImgBGR32:   return "ImgBGR32";
        case ImgRGBA:    return "ImgRGBA";
        case ImgBGRA:    return "ImgBGRA";
        case ImgGrayF:   return "ImgGrayF";
        case ImgRGBF:    return "ImgRGBF";
        case ImgBGRF:    return "ImgBGRF";
        case ImgRGBAF:   return "ImgRGBAF";
        case ImgBGRAF:   return "ImgBGRAF";
        case ImgUNKNOWN:
        default:         return "ImgUNKNOWN";
    }
}

StImagePlane::StImagePlane()
: myDataPtr(NULL),
  mySizeBPP(1),
  mySizeX(0),
  mySizeY(0),
  mySizeRowBytes(0),
  myImgFormat(StImagePlane::ImgGray),
  myIsOwnPointer(true),
  myIsTopBottom(true) {
    //
    setFormat(myImgFormat);
}

StImagePlane::~StImagePlane() {
    nullify();
}

void StImagePlane::setFormat(StImagePlane::ImgFormat thePixelFormat) {
    myImgFormat = thePixelFormat;
    switch(myImgFormat) {
        case ImgGrayF:
            mySizeBPP = sizeof(GLfloat);
            break;
        case ImgRGBAF:
        case ImgBGRAF:
            mySizeBPP = sizeof(GLfloat) * 4;
            break;
        case ImgRGBF:
        case ImgBGRF:
            mySizeBPP = sizeof(GLfloat) * 3;
            break;
        case ImgRGBA:
        case ImgBGRA:
            mySizeBPP = 4;
            break;
        case ImgRGB32:
        case ImgBGR32:
            mySizeBPP = 4;
            break;
        case ImgRGB:
        case ImgBGR:
            mySizeBPP = 3;
            break;
        case ImgGray16:
            mySizeBPP = 2;
            break;
        case ImgGray:
        default:
            mySizeBPP = 1;
    }
}

bool StImagePlane::initWrapper(StImagePlane::ImgFormat thePixelFormat,
                               GLubyte*      theDataPtr,
                               const size_t  theSizeX,
                               const size_t  theSizeY,
                               const size_t  theSizeRowBytes) {
    nullify(thePixelFormat);
    if((theSizeX == 0) || (theSizeY == 0) || (theDataPtr == NULL)) {
        return false;
    }
    mySizeX = theSizeX;
    mySizeY = theSizeY;
    mySizeRowBytes = (theSizeRowBytes != 0) ? theSizeRowBytes : (theSizeX * mySizeBPP);
    myDataPtr = theDataPtr;
    myIsOwnPointer = false;
    return true;
}

bool StImagePlane::initTrash(StImagePlane::ImgFormat thePixelFormat,
                             const size_t theSizeX,
                             const size_t theSizeY,
                             const size_t theSizeRowBytes) {
    nullify(thePixelFormat);
    if((theSizeX == 0) || (theSizeY == 0)) {
        return false;
    }
    mySizeX = theSizeX;
    mySizeY = theSizeY;
    mySizeRowBytes = mySizeX * mySizeBPP;
    if(theSizeRowBytes > mySizeRowBytes) {
        // use argument only if it greater
        mySizeRowBytes = theSizeRowBytes;
    }
    myDataPtr = stMemAllocAligned<GLubyte*>(getSizeBytes());
    myIsOwnPointer = true;
    return myDataPtr != NULL;
}

bool StImagePlane::initZero(StImagePlane::ImgFormat thePixelFormat,
                            const size_t theSizeX,
                            const size_t theSizeY,
                            const size_t theSizeRowBytes,
                            const int theValue) {
    if(!initTrash(thePixelFormat, theSizeX, theSizeY, theSizeRowBytes)) {
        return false;
    }
    stMemSet(myDataPtr, theValue, getSizeBytes());
    return true;
}

bool StImagePlane::initCopy(const StImagePlane& theCopy) {
    if(initTrash(theCopy.getFormat(), theCopy.getSizeX(), theCopy.getSizeY(), theCopy.getSizeRowBytes())) {
        stMemCpy(changeData(), theCopy.getData(), theCopy.getSizeBytes());
        return true;
    }
    return false;
}

bool StImagePlane::initWrapper(const StImagePlane& theCopy) {
    return initWrapper(theCopy.getFormat(), theCopy.myDataPtr,
                       theCopy.getSizeX(), theCopy.getSizeY(), theCopy.getSizeRowBytes());
}

bool StImagePlane::initSideBySide(const StImagePlane& theImageL,
                                  const StImagePlane& theImageR,
                                  const int theSeparationDx,
                                  const int theSeparationDy,
                                  const int theValue) {
    if(theImageL.isNull() || theImageR.isNull()) {
        // just ignore
        return true;
    }
    if(theImageL.getSizeX() != theImageR.getSizeX() ||
       theImageL.getSizeY() != theImageR.getSizeY()) {
        // currently unsupported operation
        return false;
    }
    size_t dxAbsPx = size_t(abs(theSeparationDx));
    size_t dxLeftRPx  = (theSeparationDx > 0) ?     dxAbsPx : 0;
    size_t dxLeftLPx  = (theSeparationDx < 0) ? 2 * dxAbsPx : 0;

    size_t dyAbsPx = size_t(abs(theSeparationDy));
    size_t dyTopLPx  = (theSeparationDy > 0) ? dyAbsPx : 0;
    size_t dyTopRPx  = (theSeparationDy < 0) ? dyAbsPx : 0;

    size_t outSizeX = (theImageL.getSizeX() + dxAbsPx) * 2;
    size_t outSizeY =  theImageL.getSizeY() + dyAbsPx  * 2;

    setFormat(theImageL.getFormat());
    if(!initZero(theImageL.getFormat(), outSizeX, outSizeY, outSizeX * theImageL.getSizePixelBytes(), theValue)) {
        return false;
    }

    // save cross-eyed
    for(size_t row = 0; row < theImageR.getSizeY(); ++row) {
        stMemCpy(changeData(dyTopRPx + row, dxLeftRPx),
                 theImageR.getData(row, 0),
                 theImageR.getSizeRowBytes());
    }
    for(size_t row = 0; row < theImageR.getSizeY(); ++row) {
        stMemCpy(changeData(dyTopLPx + row, theImageR.getSizeX() + dxLeftLPx + dxLeftRPx),
                 theImageL.getData(row, 0),
                 theImageL.getSizeRowBytes());
    }
    return true;
}

bool StImagePlane::fill(const StImagePlane& theCopy) {
    if(getSizeY()        != theCopy.getSizeY()
    || getSizeRowBytes() != theCopy.getSizeRowBytes()
    || getFormat()       != theCopy.getFormat()) {
        return initCopy(theCopy);
    }
    for(size_t row = 0; row < theCopy.getSizeY(); ++row) {
        stMemCpy(changeData(row, 0),
                 theCopy.getData(row, 0),
                 theCopy.getSizeRowBytes());
    }
    return true;
}

void StImagePlane::nullify(StImagePlane::ImgFormat thePixelFormat) {
    if(myIsOwnPointer && (myDataPtr != NULL)) {
        stMemFreeAligned(myDataPtr);
    }
    myDataPtr = NULL;
    myIsOwnPointer = true;
    mySizeX = mySizeY = mySizeRowBytes = 0;
    setFormat(thePixelFormat);
    myIsTopBottom = true;
}
