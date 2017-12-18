//
// Created by hwj on 2016/11/17.
//

#include "RGBRenderer.h"
#include <string.h>

extern "C" {
#include "libavutil/imgutils.h"
}
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "GRBRenderer"
#endif

RGBRenderer::RGBRenderer() {
    rgbSize=2;//默认WINDOW_FORMAT_RGB_565
}

RGBRenderer::~RGBRenderer() {
    if (nativeWindow != NULL) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = NULL;
    }
    if (pBlackFrame != NULL) {
        av_frame_free(&pBlackFrame);
        pBlackFrame == NULL;
    }

}


int RGBRenderer::prepareRGBA(ANativeWindow *nativeWindow, int frameWidth, int frameHeight) {
    isPrepareSuccess = false;
    if (nativeWindow == NULL) {
        LOGE("prepare：nativeWindow is null");
        return -1;
    }
    this->nativeWindow = nativeWindow;
    LOGI("prepare：ANativeWindow_getFormat=%d",ANativeWindow_getFormat(nativeWindow));
    if (frameWidth <= 0 || frameHeight <= 0) {
        LOGE("prepare：frameWidth and frameHeight is error:%d-%d",frameWidth,frameHeight);
        return -1;
    }
    this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;
    // 设置native window的buffer大小,可自动拉伸
    ANativeWindow_setBuffersGeometry(nativeWindow, frameWidth, frameHeight,WINDOW_FORMAT_RGBA_8888);
    rgbSize=4;
    if (pBlackFrame == NULL) {
        pBlackFrame = av_frame_alloc();
        int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, frameWidth, frameHeight, 1);
        uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
        av_image_fill_arrays(pBlackFrame->data, pBlackFrame->linesize, buffer, AV_PIX_FMT_RGBA,
                             frameWidth, frameHeight, 1);
    }
    isPrepareSuccess = true;
    return 1;
}

int RGBRenderer::prepareRGB565(ANativeWindow *nativeWindow, int frameWidth, int frameHeight) {
    isPrepareSuccess = false;
    if (nativeWindow == NULL) {
        LOGE("prepare：nativeWindow is null");
        return -1;
    }
    this->nativeWindow = nativeWindow;
    LOGI("prepare：ANativeWindow_getFormat=%d",ANativeWindow_getFormat(nativeWindow));
    if (frameWidth <= 0 || frameHeight <= 0) {
        LOGE("prepare：frameWidth and frameHeight is error:%d-%d",frameWidth,frameHeight);
        return -1;
    }
    this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;
    // 设置native window的buffer大小,可自动拉伸
    ANativeWindow_setBuffersGeometry(nativeWindow, frameWidth, frameHeight,WINDOW_FORMAT_RGB_565);
    rgbSize=2;
    if (pBlackFrame == NULL) {
        pBlackFrame = av_frame_alloc();
        int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB565LE, frameWidth, frameHeight, 1);
        uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
        av_image_fill_arrays(pBlackFrame->data, pBlackFrame->linesize, buffer, AV_PIX_FMT_RGB565LE,
                             frameWidth, frameHeight, 1);
    }
    isPrepareSuccess = true;
    return 1;
}

int RGBRenderer::render(AVFrame *pFrameRGBA) {
    if (pFrameRGBA == NULL) {
        return -1;
    }
    if(pFrameRGBA->data[0]==NULL||pFrameRGBA->linesize[0]<=0){
        return -1;
    }
    return render(pFrameRGBA->data[0], pFrameRGBA->linesize[0]);
}

int RGBRenderer::render(uint8_t *data, int size) {

    if (isPrepareSuccess == false) {
        LOGE("render：prepare do not success");
        return -1;
    }
    if (data == NULL || size <= 0) {
        LOGE("render：data or size is empty");
        return -1;
    }

    // lock native window buffer
    ANativeWindow_lock(nativeWindow, &windowBuffer, 0);
    LOGI("render:the size of rgbSize is %d",rgbSize);
    LOGI("render:frameWidth=%d,frameHeight=%d,size=%d",frameWidth,frameHeight,size);
    LOGI("render:windowBuffer.width=%d,windowBuffer.height=%d,windowBuffer.stride=%d",windowBuffer.width,windowBuffer.height,windowBuffer.stride);


    if(windowBuffer.width >= windowBuffer.stride){
        memcpy(windowBuffer.bits, data, frameWidth * frameHeight * rgbSize);
    }else{//由于window的stride和帧的stride不同,因此需要逐行复制
        uint8_t *dst = (uint8_t *) windowBuffer.bits;
        int h;
        //其中size=frameWidth* rgbSize；
        for (h = 0; h < frameHeight; h++) {
            memcpy(dst + h *  windowBuffer.stride *rgbSize, data + h * size, size);
        }
    }
    ANativeWindow_unlockAndPost(nativeWindow);
    return 1;
}

bool RGBRenderer::isPreparedSuccess() {
    return isPrepareSuccess;
}

void RGBRenderer::clearScreen() {
    render(pBlackFrame);
    render(pBlackFrame);
}

void RGBRenderer::reset() {
     isPrepareSuccess = false;
     frameWidth=0;
     frameHeight=0;
}

















