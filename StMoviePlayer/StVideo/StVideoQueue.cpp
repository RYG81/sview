/**
 * Copyright © 2009-2014 Kirill Gavrilov <kirill@sview.ru>
 *
 * StMoviePlayer program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StMoviePlayer program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "StVideoQueue.h"

#include <StStrings/StStringStream.h>
#include <StThreads/StThread.h>

#if (defined(_WIN64) || defined(__WIN64__))\
 || (defined(_LP64)  || defined(__LP64__))
    #define ST_USE64PTR
#endif

namespace {

#ifdef  __APPLE__
    /**
     * Override pixel format.
     */
    AVPixelFormat stGetFormatYUV420P(AVCodecContext*      /*theCtx*/,
                                     const AVPixelFormat* /*theFmt*/) {
        return stAV::PIX_FMT::YUV420P;
    }
#endif

#ifdef ST_AV_OLDSYNC
    /**
     * These are called whenever we allocate a frame
     * buffer. We use this to store the global_pts in
     * a frame at the time it is allocated.
     */
    static int ourGetBuffer(AVCodecContext* theCodecCtx,
                            AVFrame*        theFrame) {
        StVideoQueue* aVideoQueue = (StVideoQueue* )theCodecCtx->opaque;
        const int aResult = avcodec_default_get_buffer(theCodecCtx, theFrame);
    #ifdef ST_USE64PTR
        theFrame->opaque = (void* )aVideoQueue->getVideoPktPts();
    #else
        int64_t* aPts = new int64_t();
        *aPts = aVideoQueue->getVideoPktPts();
        theFrame->opaque = aPts;
    #endif
        return aResult;
    }

    static void ourReleaseBuffer(AVCodecContext* theCodecCtx,
                                 AVFrame*        theFrame) {
        if(theFrame != NULL) {
        #ifdef ST_USE64PTR
            theFrame->opaque = (void* )stAV::NOPTS_VALUE;
        #else
            delete (int64_t* )theFrame->opaque;
            theFrame->opaque = NULL;
        #endif
        }
        avcodec_default_release_buffer(theCodecCtx, theFrame);
    }
#endif

    /**
     * Thread function just call decodeLoop() function.
     */
    static SV_THREAD_FUNCTION threadFunction(void* theVideoQueue) {
        StVideoQueue* aVideoQueue = (StVideoQueue* )theVideoQueue;
        aVideoQueue->decodeLoop();
        return SV_THREAD_RETURN 0;
    }

};

StVideoQueue::StVideoQueue(const StHandle<StGLTextureQueue>& theTextureQueue,
                           const StHandle<StVideoQueue>&     theMaster)
: StAVPacketQueue(512),
  myDowntimeState(true),
  myTextureQueue(theTextureQueue),
  myHasDataState(false),
  myMaster(theMaster),
#if defined(_WIN32)
  myCodecDxva264(avcodec_find_decoder_by_name("h264_dxva2")),
  myCodecDxvaWmv(avcodec_find_decoder_by_name("wmv3_dxva2")),
  myCodecDxvaVc1(avcodec_find_decoder_by_name("vc1_dxva2")),
#elif defined(__APPLE__)
  myCodecVda(avcodec_find_decoder_by_name("h264_vda")),
#endif
  myUseGpu(false),
  //
  myToRgbCtx(NULL),
  myToRgbPixFmt(stAV::PIX_FMT::NONE),
  myToRgbIsBroken(false),
  //
  myAvDiscard(AVDISCARD_DEFAULT),
  myFramePts(0.0),
  myPixelRatio(1.0f),
  //
  myVideoClock(0.0),
  //
  myVideoPktPts(stAV::NOPTS_VALUE),
  //
  myAudioClock(0.0),
  myAudioDelayMSec(0),
  myFramesCounter(1),
  myWasFlushed(false),
  myStFormatByUser(StFormat_AUTO),
  myStFormatByName(StFormat_AUTO),
  myStFormatInStream(StFormat_AUTO) {

#ifdef ST_USE64PTR
    myFrame.Frame->opaque = (void* )stAV::NOPTS_VALUE;
#else
    myFrame.Frame->opaque = NULL;
#endif
    myThread = new StThread(threadFunction, (void* )this, theMaster.isNull() ? "StVideoQueueM" : "StVideoQueueS");
}

StVideoQueue::~StVideoQueue() {
    myTextureQueue->clear();

    myToQuit = true;
    pushQuit();

    myThread->wait();
    myThread.nullify();

    deinit();
}

namespace {

    struct StFFmpegStereoFormat {
        StFormat    stID;
        const char* name;
    };

    static const StCString THE_SRC_MODE_KEY     = stCString("STEREO_MODE");
    static const StCString THE_SRC_MODE_KEY_WMV = stCString("StereoscopicLayout");

    static const StFFmpegStereoFormat STEREOFLAGS[] = {
        // MKV stereoscopic mode decoded by FFmpeg into STEREO_MODE metadata tag
        {StFormat_Mono,                 "mono"},
        {StFormat_SideBySide_RL,        "right_left"},
        {StFormat_SideBySide_LR,        "left_right"},
        {StFormat_TopBottom_RL,         "bottom_top"},
        {StFormat_TopBottom_LR,         "top_bottom"},
        {StFormat_Rows,                 "row_interleaved_rl"},
        {StFormat_Rows,                 "row_interleaved_lr"},
        {StFormat_Columns,              "col_interleaved_rl"},
        {StFormat_Columns,              "col_interleaved_lr"},
        {StFormat_FrameSequence,        "block_lr"}, /// ???
        {StFormat_FrameSequence,        "block_rl"},
        {StFormat_AnaglyphRedCyan,      "anaglyph_cyan_red"},
        {StFormat_AnaglyphGreenMagenta, "anaglyph_green_magenta"},
        // values in WMV StereoscopicLayout tag
        {StFormat_SideBySide_RL,        "SideBySideRF"}, // Right First
        {StFormat_SideBySide_LR,        "SideBySideLF"},
        {StFormat_TopBottom_LR,         "OverUnderLT"},  // Left Top
        {StFormat_TopBottom_RL,         "OverUnderRT"},
        // NULL-terminate array
        {StFormat_AUTO, NULL}
    };

};

bool StVideoQueue::initCodec(AVCodec* theCodec) {
    // configure the codec
    myCodecCtx->codec_id = theCodec->id;
    //myCodecCtx->debug_mv = debug_mv;
    //myCodecCtx->debug = debug;
    //myCodecCtx->workaround_bugs = workaround_bugs;
    //myCodecCtx->lowres = 1;
    //if(lowres) myCodecCtx->flags |= CODEC_FLAG_EMU_EDGE;
    //myCodecCtx->idct_algo= idct;
    //if(fast) myCodecCtx->flags2 |= CODEC_FLAG2_FAST;
    //myCodecCtx->skip_idct= skip_idct;
    //myCodecCtx->skip_loop_filter= skip_loop_filter;
    //myCodecCtx->error_recognition= error_recognition;
    //myCodecCtx->error_concealment= error_concealment;
    int threadsCount = StThread::countLogicalProcessors();
    myCodecCtx->thread_count = threadsCount;
#if(LIBAVCODEC_VERSION_INT < AV_VERSION_INT(52, 112, 0))
    avcodec_thread_init(myCodecCtx, threadsCount);
#endif

    // open codec
#if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53, 8, 0))
    if(avcodec_open2(myCodecCtx, theCodec, NULL) < 0) {
#else
    if(avcodec_open(myCodecCtx, theCodec) < 0) {
#endif
        return false;
    }

    myCodec = theCodec;
    fillCodecInfo(theCodec);
    ST_DEBUG_LOG("FFmpeg: Setup AVcodec to use " + threadsCount + " threads");
    return true;
}

bool StVideoQueue::init(AVFormatContext*   theFormatCtx,
                        const unsigned int theStreamId,
                        const StString&    theFileName) {
    if(!StAVPacketQueue::init(theFormatCtx, theStreamId, theFileName)
    || myCodecCtx->codec_type != AVMEDIA_TYPE_VIDEO) {
        signals.onError(stCString("FFmpeg: invalid stream"));
        deinit();
        return false;
    }

    // detect 720in1080 streams with cropping information
    const bool is720in1080 = (sizeX() == 1280) && (sizeY() == 720)
                          && (getCodedSizeX() == 1920)
                          && (getCodedSizeY() == 1080 || getCodedSizeY() == 1088);
#if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55, 0, 0))
    if(is720in1080) {
        myCodecCtx->flags2 |= CODEC_FLAG2_IGNORE_CROP;
    }
#endif

    // find the decoder for the video stream
    myCodecAuto = avcodec_find_decoder(myCodecCtx->codec_id);
    if(myCodecAuto == NULL) {
        signals.onError(stCString("FFmpeg: Video codec not found"));
        deinit();
        return false;
    }

    bool isCodecOverridden = false;
    AVCodec* aCodecGpu = NULL;
    if(myUseGpu) {
    #if defined(_WIN32)
        if(StString(myCodecAuto->name).isEquals(stCString("h264"))) {
            aCodecGpu = myCodecDxva264;
        } else if(StString(myCodecAuto->name).isEquals(stCString("wmv3"))) {
            aCodecGpu = myCodecDxvaWmv;
        } else if(StString(myCodecAuto->name).isEquals(stCString("vc1"))) {
            aCodecGpu = myCodecDxvaVc1;
        }
    #elif defined(__APPLE__)
        if(StString(myCodecAuto->name).isEquals(stCString("h264"))
        && myCodecCtx->pix_fmt == stAV::PIX_FMT::YUV420P) {
            aCodecGpu = myCodecVda;
        }
    #endif
    }

    if(aCodecGpu != NULL) {
    #if defined(__APPLE__)
        myCodecCtx->get_format = stGetFormatYUV420P;
    #endif
        isCodecOverridden = initCodec(aCodecGpu);
    #if defined(__APPLE__)
        if(!isCodecOverridden) {
            myCodecCtx->get_format = myGetFrmtInit;
        }
    #endif
    }

    // open VIDEO codec
    if(!isCodecOverridden && !initCodec(myCodecAuto)) {
        signals.onError(stCString("FFmpeg: Could not open video codec"));
        deinit();
        return false;
    }

    // determine required myFrameRGB size and allocate it
    if(sizeX() == 0 || sizeY() == 0) {
        signals.onError(stCString("FFmpeg: Codec return wrong frame size"));
        deinit();
        return false;
    }

    // reset AVFrame structure
    myFrame.reset();

    // compute PAR
    if(myStream->sample_aspect_ratio.num && av_cmp_q(myStream->sample_aspect_ratio, myStream->codec->sample_aspect_ratio)) {
        myPixelRatio = GLfloat(myStream->sample_aspect_ratio.num) / GLfloat(myStream->sample_aspect_ratio.den);
    } else {
        if(myCodecCtx->sample_aspect_ratio.num == 0 ||
           myCodecCtx->sample_aspect_ratio.den == 0) {
            myPixelRatio = 1.0f;
        } else {
            myPixelRatio = GLfloat(myCodecCtx->sample_aspect_ratio.num) / GLfloat(myCodecCtx->sample_aspect_ratio.den);
        }
    }

    // special WMV tags
    StString aValue;
    const StString aHalfHeightKeyWMV  = "StereoscopicHalfHeight";
    const StString aHalfWidthKeyWMV   = "StereoscopicHalfWidth";
    const StString aHorParallaxKeyWMV = "StereoscopicHorizontalParallax";
    if(stAV::meta::readTag(myFormatCtx, aHalfHeightKeyWMV, aValue)) {
        if(aValue == "1") {
            myPixelRatio *= 0.5;
        }
    } else if(stAV::meta::readTag(myFormatCtx, aHalfWidthKeyWMV, aValue)) {
        if(aValue == "1") {
            myPixelRatio *= 2.0;
        }
    }
    myHParallax = 0;
    if(stAV::meta::readTag(myFormatCtx, aHorParallaxKeyWMV, aValue)) {
        StCLocale aCLocale;
        myHParallax = (int )stStringToLong(aValue.toCString(), 10, aCLocale);
    }

    // stereoscopic mode tags
    myStFormatInStream = is720in1080 ? StFormat_Tiled4x : StFormat_AUTO;
    if(stAV::meta::readTag(myFormatCtx, THE_SRC_MODE_KEY,     aValue)
    || stAV::meta::readTag(myStream,    THE_SRC_MODE_KEY,     aValue)
    || stAV::meta::readTag(myFormatCtx, THE_SRC_MODE_KEY_WMV, aValue)) {
        for(size_t aSrcId = 0;; ++aSrcId) {
            const StFFmpegStereoFormat& aFlag = STEREOFLAGS[aSrcId];
            if(aFlag.stID == StFormat_AUTO || aFlag.name == NULL) {
                break;
            } else if(aValue == aFlag.name) {
                myStFormatInStream = aFlag.stID;
                //ST_DEBUG_LOG("  read srcFormat from tags= " + myStFormatInStream);
                break;
            }
        }
    }

    // detect information from file name
    bool isAnamorphByName = false;
    myStFormatByName = st::formatFromName(myFileName, isAnamorphByName);
    if(myStFormatInStream == StFormat_AUTO
    && isAnamorphByName) {
        if(myStFormatByName == StFormat_SideBySide_LR
        || myStFormatByName == StFormat_SideBySide_RL) {
            myPixelRatio *= 2.0;
        } else if(myStFormatByName == StFormat_TopBottom_LR
               || myStFormatByName == StFormat_TopBottom_RL) {
            myPixelRatio *= 0.5;
        }
    }

#ifdef ST_AV_OLDSYNC
    // override buffers' functions for getting true PTS rootines
    myCodecCtx->opaque = this;
    myCodecCtx->get_buffer = ourGetBuffer;
    myCodecCtx->release_buffer = ourReleaseBuffer;
#endif
    return true;
}

void StVideoQueue::deinit() {
    if(myMaster.isNull()) {
        myTextureQueue->clear();
        myTextureQueue->setConnectedStream(false);
    }
    mySlave.nullify();
    myPixelRatio = 1.0f;
    myDataAdp.nullify();

    myDataRGB.nullify();
    sws_freeContext(myToRgbCtx);
    myToRgbCtx      = NULL;
    myToRgbPixFmt   = stAV::PIX_FMT::NONE;
    myToRgbIsBroken = false;

    myFramesCounter = 1;
    myCachedFrame.nullify();

    if(myCodecCtx  != NULL
    && myCodecAuto != NULL) {
        myCodecCtx->codec_id = myCodecAuto->id;
    }
    StAVPacketQueue::deinit();
}

#ifdef ST_AV_OLDSYNC
void StVideoQueue::syncVideo(AVFrame* theSrcFrame,
                             double*  thePts) {
    if(*thePts != 0.0) {
        // if we have pts, set video clock to it
        myVideoClock = *thePts;
    } else {
        // if we aren't given a pts, set it to the clock
        *thePts = myVideoClock;
    }

    // update the video clock
    double aFrameDelay = av_q2d(myCodecCtx->time_base);
    // if we are repeating a frame, adjust clock accordingly
    aFrameDelay  += theSrcFrame->repeat_pict * (aFrameDelay * 0.5);
    myVideoClock += aFrameDelay;
}
#endif

void StVideoQueue::prepareFrame(const StFormat theSrcFormat) {
    int          aFrameSizeX = 0;
    int          aFrameSizeY = 0;
    PixelFormat  aPixFmt     = stAV::PIX_FMT::NONE;
    stAV::dimYUV aDimsYUV;
    myFrame.getImageInfo(myCodecCtx, aFrameSizeX, aFrameSizeY, aPixFmt);
    if(aPixFmt == stAV::PIX_FMT::RGB24) {
        myDataAdp.setColorModel(StImage::ImgColor_RGB);
        myDataAdp.setColorScale(StImage::ImgScale_Full);
        myDataAdp.setPixelRatio(getPixelRatio());
        myDataAdp.changePlane(0).initWrapper(StImagePlane::ImgRGB, myFrame.getPlane(0),
                                             size_t(aFrameSizeX), size_t(aFrameSizeY),
                                             myFrame.getLineSize(0));
#if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53, 5, 0))
    } else if(stAV::isFormatYUVPlanar(myFrame.Frame,
#else
    } else if(stAV::isFormatYUVPlanar(myCodecCtx,
#endif
                                      aDimsYUV)) {

        /// TODO (Kirill Gavrilov#5) remove hack
        // workaround for incorrect frame dimensions information in some files
        // critical for tiled source format that should be 1080p
        if(theSrcFormat == StFormat_Tiled4x
        && aPixFmt      == stAV::PIX_FMT::YUV420P
        && aFrameSizeX >= 1906 && aFrameSizeX <= 1920
        && myFrame.getLineSize(0) >= 1920
        && aFrameSizeY >= 1074) {
            aDimsYUV.widthY  = 1920;
            aDimsYUV.heightY = 1080;
            aDimsYUV.widthU  = aDimsYUV.widthV  = aDimsYUV.widthY  / 2;
            aDimsYUV.heightU = aDimsYUV.heightV = aDimsYUV.heightY / 2;
        }

        StImagePlane::ImgFormat aPlaneFrmt = StImagePlane::ImgGray;
    #if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 29, 0))
        if(myCodecCtx->color_range == AVCOL_RANGE_JPEG) {
            // there no color range information in the AVframe (yet)
            aDimsYUV.isFullScale = true;
        }
    #endif
        myDataAdp.setColorScale(aDimsYUV.isFullScale ? StImage::ImgScale_Full : StImage::ImgScale_Mpeg);
        if(aDimsYUV.bitsPerComp == 9) {
            aPlaneFrmt = StImagePlane::ImgGray16;
            myDataAdp.setColorScale(aDimsYUV.isFullScale ? StImage::ImgScale_Jpeg9  : StImage::ImgScale_Mpeg9);
        } else if(aDimsYUV.bitsPerComp == 10) {
            aPlaneFrmt = StImagePlane::ImgGray16;
            myDataAdp.setColorScale(aDimsYUV.isFullScale ? StImage::ImgScale_Jpeg10 : StImage::ImgScale_Mpeg10);
        } else if(aDimsYUV.bitsPerComp == 16) {
            aPlaneFrmt = StImagePlane::ImgGray16;
        }
        myDataAdp.setColorModel(StImage::ImgColor_YUV);
        myDataAdp.setPixelRatio(getPixelRatio());
        myDataAdp.changePlane(0).initWrapper(aPlaneFrmt, myFrame.getPlane(0),
                                             size_t(aDimsYUV.widthY), size_t(aDimsYUV.heightY), myFrame.getLineSize(0));
        myDataAdp.changePlane(1).initWrapper(aPlaneFrmt, myFrame.getPlane(1),
                                             size_t(aDimsYUV.widthU), size_t(aDimsYUV.heightU), myFrame.getLineSize(1));
        myDataAdp.changePlane(2).initWrapper(aPlaneFrmt, myFrame.getPlane(2),
                                             size_t(aDimsYUV.widthV), size_t(aDimsYUV.heightV), myFrame.getLineSize(2));
    } else if(!myToRgbIsBroken) {
        if(myToRgbCtx    == NULL
        || myToRgbPixFmt != aPixFmt
        || size_t(aFrameSizeX) != myDataRGB.getSizeX()
        || size_t(aFrameSizeY) != myDataRGB.getSizeY()) {
            // initialize software scaler/converter
        //#if LIBSWSCALE_VERSION_MAJOR >= 3
            //myToRgbCtx = sws_getCachedContext(myToRgbCtx,
            //                                  aFrameSizeX, aFrameSizeY, aPixFmt,
            //                                  aFrameSizeX, aFrameSizeY, stAV::PIX_FMT::RGB24
            //                                  SWS_BICUBIC, NULL, NULL, NULL);
        //#else
            sws_freeContext(myToRgbCtx);
            myToRgbCtx = sws_getContext(aFrameSizeX, aFrameSizeY, aPixFmt,              // source
                                        aFrameSizeX, aFrameSizeY, stAV::PIX_FMT::RGB24, // destination
                                        SWS_BICUBIC, NULL, NULL, NULL);
        //#endif
            myToRgbPixFmt = aPixFmt;
            if(myToRgbCtx == NULL
            || aFrameSizeX <= 0
            || aFrameSizeY <= 0) {
                signals.onError(stCString("FFmpeg: Failed to create SWScaler context"));
                myToRgbIsBroken = true;
            } else {
                // assign appropriate parts of RGB buffer to image planes in myFrameRGB
                const size_t aBufferSize = avpicture_get_size(stAV::PIX_FMT::RGB24, aFrameSizeX, aFrameSizeY);
                if(aBufferSize == 0
                || !myDataRGB.initTrash(StImagePlane::ImgRGB, size_t(aFrameSizeX), size_t(aFrameSizeY),
                                        aBufferSize / size_t(aFrameSizeY))) {
                    signals.onError(stCString("FFmpeg: Failed allocation of RGB frame (out of memory)"));
                    myToRgbIsBroken = true;
                } else {
                    avpicture_fill((AVPicture* )myFrameRGB.Frame, myDataRGB.changeData(), stAV::PIX_FMT::RGB24,
                                   aFrameSizeX, aFrameSizeY);
                }
            }
        }

        if(!myToRgbIsBroken) {
            sws_scale(myToRgbCtx,
                      myFrame.Frame->data, myFrame.Frame->linesize,
                      0, aFrameSizeY,
                      myFrameRGB.Frame->data, myFrameRGB.Frame->linesize);

            myDataAdp.setColorModel(StImage::ImgColor_RGB);
            myDataAdp.setColorScale(StImage::ImgScale_Full);
            myDataAdp.setPixelRatio(getPixelRatio());
            myDataAdp.changePlane(0).initWrapper(StImagePlane::ImgRGB, myDataRGB.changeData(),
                                                 size_t(aFrameSizeX), size_t(aFrameSizeY));
        }
    } else {
        //ST_DEBUG_LOG("Frame skipped - unsupported pixel format!");
    }
}

void StVideoQueue::pushFrame(const StImage&     theSrcDataLeft,
                             const StImage&     theSrcDataRight,
                             const StHandle<StStereoParams>& theStParams,
                             const StFormat     theSrcFormat,
                             const double       theSrcPTS) {
    while(!myToFlush && myTextureQueue->isFull()) {
        StThread::sleep(10);
    }

    if(myToFlush) {
        myToFlush = false;
        return;
    }

    myTextureQueue->push(theSrcDataLeft, theSrcDataRight, theStParams, theSrcFormat, theSrcPTS);
    myTextureQueue->setConnectedStream(true);
    if(myWasFlushed) {
        // force frame update after seeking regardless playback timer
        myTextureQueue->stglSwapFB(0);
        myWasFlushed = false;
    }
}

void StVideoQueue::decodeLoop() {
    int isFrameFinished = 0;
    double anAverageDelaySec = 40.0;
    double aPrevPts  = 0.0;
    double aSlavePts = 0.0;
    myFramePts = 0.0;
    StImage* aSlaveData = NULL;
    StHandle<StAVPacket> aPacket;
    StImage anEmptyImg;
    StString aTagValue;
    bool isStarted = false;
    for(;;) {
        if(isEmpty()) {
            myDowntimeState.set();
            StThread::sleep(10);
            continue;
        }
        myDowntimeState.reset();

        aPacket = pop();
        if(aPacket.isNull()) {
            continue;
        }
        switch(aPacket->getType()) {
            case StAVPacket::FLUSH_PACKET: {
                // got the special FLUSH packet - flush FFMPEG codec buffers
                if(myCodecCtx != NULL && myCodec != NULL) {
                    avcodec_flush_buffers(myCodecCtx);
                }
                // now we clear our sttextures buffer
                if(myMaster.isNull()) {
                    myTextureQueue->clear();
                }
                myAudioClock = 0.0;
                myVideoClock = 0.0;
                myToFlush    = false;
                myWasFlushed = true;
                continue;
            }
            case StAVPacket::START_PACKET: {
                myAudioClock = 0.0;
                myVideoClock = 0.0;
                myHasDataState.reset();
                isStarted = true;
                continue;
            }
            case StAVPacket::END_PACKET: {
                if(!myMaster.isNull()) {
                    while(myHasDataState.check() && !myMaster->isInDowntime()) {
                        StThread::sleep(10);
                    }
                    // wake up Master
                    myDataAdp.nullify();
                    myHasDataState.set();
                } else {
                    if(!mySlave.isNull()) {
                        mySlave->unlockData();
                    }
                    StTimer stTimerWaitEmpty(true);
                    double waitTime = anAverageDelaySec * myTextureQueue->getSize() + 0.1;
                    while(!myTextureQueue->isEmpty() && stTimerWaitEmpty.getElapsedTimeInSec() < waitTime && !myToQuit) {
                        StThread::sleep(2);
                    }
                }
                if(myToQuit) {
                    return;
                }
                continue;
            }
            case StAVPacket::QUIT_PACKET: {
                return;
            }
        }

        // wait master retrieve previous data
        while(!myMaster.isNull() && myHasDataState.check()) {
            //
        }

        // decode video frame
    #if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 23, 0))
        avcodec_decode_video2(myCodecCtx, myFrame.Frame, &isFrameFinished, aPacket->getAVpkt());
    #else
        avcodec_decode_video(myCodecCtx, myFrame.Frame, &isFrameFinished,
                             aPacket->getData(), aPacket->getSize());
    #endif
        if(isFrameFinished == 0) {
            // need more packets to decode whole frame
            aPacket.nullify();
            continue;
        }

        if(aPacket->isKeyFrame()) {
            myFramesCounter = 1;
        }

    #ifndef ST_AV_OLDSYNC
        myVideoPktPts = av_frame_get_best_effort_timestamp(myFrame.Frame);
        if(myVideoPktPts == stAV::NOPTS_VALUE) {
            myVideoPktPts = 0;
        }

        myFramePts  = double(myVideoPktPts) * av_q2d(myStream->time_base);
        myFramePts -= myPtsStartBase; // normalize PTS
    #else
        // Save global pts to be stored in pFrame in first call
        myVideoPktPts = aPacket->getPts();

    #ifdef ST_USE64PTR
        if(aPacket->getDts() == stAV::NOPTS_VALUE
        && (int64_t )myFrame.Frame->opaque != stAV::NOPTS_VALUE) {
            myFramePts = double((int64_t )myFrame.Frame->opaque);
    #else
        if(aPacket->getDts() == stAV::NOPTS_VALUE
        && myFrame.Frame->opaque != NULL
        && *(int64_t* )myFrame.Frame->opaque != stAV::NOPTS_VALUE) {
            myFramePts = double(*(int64_t* )myFrame.Frame->opaque);
    #endif
        } else if(aPacket->getDts() != stAV::NOPTS_VALUE) {
            myFramePts = double(aPacket->getDts());
        } else {
            myFramePts = 0.0;
        }
        myFramePts *= av_q2d(myStream->time_base);
        myFramePts -= myPtsStartBase; // normalize PTS

        syncVideo(myFrame.Frame, &myFramePts);
    #endif

        const double aDelay = myFramePts - aPrevPts;
        if(aDelay > 0.0 && aDelay < 1.0) {
            anAverageDelaySec = aDelay;
        }
        aPrevPts = myFramePts;

        // do we need to skip frames or not
        static const double OVERR_LIMIT = 0.2;
        static const double GREATER_LIMIT = 100.0;
        if(myMaster.isNull()) {
            const double anAudioClock = getAClock() + double(myAudioDelayMSec) * 0.001;
            double diff = anAudioClock - myFramePts;
            if(diff > OVERR_LIMIT && diff < GREATER_LIMIT) {
                if(myAvDiscard != AVDISCARD_NONREF) {
                    ST_DEBUG_LOG("skip frames: AVDISCARD_NONREF (on)"
                        + " (aClock " + anAudioClock
                        + " vClock " + myFramePts
                        + " diff " + diff + ")"
                    );
                    myAvDiscard = AVDISCARD_NONREF;
                    ///myCodecCtx->skip_frame = AVDISCARD_NONKEY;
                    myCodecCtx->skip_frame = myAvDiscard;
                    if(!mySlave.isNull()) {
                        mySlave->myCodecCtx->skip_frame = myAvDiscard;
                    }
                }
            } else {
                if(myAvDiscard != AVDISCARD_DEFAULT) {
                    ST_DEBUG_LOG("skip frames: AVDISCARD_DEFAULT (off)");
                    myAvDiscard = AVDISCARD_DEFAULT;
                    myCodecCtx->skip_frame = myAvDiscard;
                    if(!mySlave.isNull()) {
                        mySlave->myCodecCtx->skip_frame = myAvDiscard;
                    }
                }
            }
        }

        // we currently allow to override source format stored in metadata
    #ifdef ST_AV_NEWSTEREO
        AVFrameSideData* aSideData = av_frame_get_side_data(myFrame.Frame, AV_FRAME_DATA_STEREO3D);
        if(aSideData != NULL) {
            AVStereo3D* aStereo = (AVStereo3D* )aSideData->data;
            myStFormatInStream = stAV::stereo3dAvToSt(aStereo->type);
            if(aStereo->flags & AV_STEREO3D_FLAG_INVERT) {
                myStFormatInStream = st::formatReversed(myStFormatInStream);
            }
        }
    #endif
        if(stAV::meta::readTag(myFrame.Frame, THE_SRC_MODE_KEY, aTagValue)) {
            for(size_t aSrcId = 0;; ++aSrcId) {
                const StFFmpegStereoFormat& aFlag = STEREOFLAGS[aSrcId];
                if(aFlag.stID == StFormat_AUTO || aFlag.name == NULL) {
                    break;
                } else if(aTagValue == aFlag.name) {
                    myStFormatInStream = aFlag.stID;
                    //ST_DEBUG_LOG("  read srcFormat from tags= " + myStFormatInStream);
                    break;
                }
            }
        }
        // override source format stored in metadata
        StFormat aSrcFormat = myStFormatByUser;
        if(aSrcFormat == StFormat_AUTO) {
            // prefer info stored in the stream itself
            aSrcFormat = myStFormatInStream;
        }
        if(aSrcFormat == StFormat_AUTO) {
            // try using format detected from file name
            aSrcFormat = myStFormatByName;
        }
        /*if(aSrcFormat == StFormat_AUTO
        && sizeY() != 0) {
            // try detection based on aspect ratio
            aSrcFormat = st::formatFromRatio(GLfloat(sizeX()) / GLfloat(sizeY()));
        }*/

        prepareFrame(aSrcFormat);

        if(!mySlave.isNull()) {
            if(isStarted) {
                StHandle<StStereoParams> aParams = aPacket->getSource();
                if(!aParams.isNull()) {
                    aParams->setSeparationNeutral(myHParallax);
                }
                isStarted = false;
            }

            for(;;) {
                // wait data from Slave
                if(aSlaveData == NULL) {
                    aSlaveData = mySlave->waitData(aSlavePts);
                }
                if(aSlaveData != NULL) {
                    const double aPtsDiff = myFramePts - aSlavePts;
                    if(aPtsDiff > 0.5 * anAverageDelaySec) {
                        // wait for more recent frame from slave thread
                        mySlave->unlockData();
                        aSlaveData = NULL;
                        StThread::sleep(10);
                        continue;
                    } else if(aPtsDiff < -0.5 * anAverageDelaySec) {
                        // too far...
                        if(aPtsDiff < -6.0) {
                            // result of seeking?
                            mySlave->unlockData();
                            aSlaveData = NULL;
                        }
                        break;
                    }

                    pushFrame(myDataAdp, *aSlaveData, aPacket->getSource(), StFormat_SeparateFrames, myFramePts);

                    aSlaveData = NULL;
                    mySlave->unlockData();
                } else {
                    pushFrame(myDataAdp, anEmptyImg, aPacket->getSource(), aSrcFormat, myFramePts);
                }
                break;
            }
        } else if(!myMaster.isNull()) {
            // push data to Master
            myHasDataState.set();
        } else {
            if(isStarted) {
                StHandle<StStereoParams> aParams = aPacket->getSource();
                if(!aParams.isNull()) {
                    aParams->setSeparationNeutral(myHParallax);
                }
                isStarted = false;
            }

            // simple one-stream case
            if(aSrcFormat == StFormat_FrameSequence) {
                if(isOddNumber(myFramesCounter)) {
                    myCachedFrame.fill(myDataAdp);
                } else {
                    pushFrame(myCachedFrame, myDataAdp, aPacket->getSource(), StFormat_FrameSequence, myFramePts);
                }
                ++myFramesCounter;
            } else {
                pushFrame(myDataAdp, anEmptyImg, aPacket->getSource(), aSrcFormat, myFramePts);
            }
        }

        aPacket.nullify(); // and now packet finished
    }
}
