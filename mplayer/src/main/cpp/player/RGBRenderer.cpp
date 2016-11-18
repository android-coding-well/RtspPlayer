//
// Created by hwj on 2016/11/17.
//

#include "RGBRenderer.h"
#include <string.h>

extern "C" {
#include "libavutil/imgutils.h"
}
#define  LOG_TAG    "GRBRenderer"

RGBRenderer::RGBRenderer() {

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

int RGBRenderer::prepare(ANativeWindow *nativeWindow, int frameWidth, int frameHeight) {
    isPrepareSuccess = false;
    if (nativeWindow == NULL) {
        return -1;
    }
    this->nativeWindow = nativeWindow;
    if (frameWidth <= 0 || frameHeight <= 0) {
        return -1;
    }
    this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;
    // 设置native window的buffer大小,可自动拉伸
    ANativeWindow_setBuffersGeometry(nativeWindow, frameWidth, frameHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    pBlackFrame = av_frame_alloc();
    if (pBlackFrame != NULL) {
        int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, frameWidth, frameHeight, 1);
        uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
        av_image_fill_arrays(pBlackFrame->data, pBlackFrame->linesize, buffer, AV_PIX_FMT_RGBA,
                             frameWidth, frameHeight, 1);
    }

    isPrepareSuccess = true;
    return 1;
}

void RGBRenderer::render(AVFrame *pFrameRGBA) {
    if (pFrameRGBA == NULL) {
        return;
    }
    render(pFrameRGBA->data[0], pFrameRGBA->linesize[0]);
}

void RGBRenderer::render(uint8_t *data, int size) {
    LOGI("render:size=%d", size);
    if (isPrepareSuccess == false) {
        return;
    }
    if (data == NULL || size <= 0) {
        return;
    }
    // lock native window buffer
    ANativeWindow_lock(nativeWindow, &windowBuffer, 0);

    // 获取stride
    uint8_t *dst = (uint8_t *) windowBuffer.bits;
    int dstStride = windowBuffer.stride * 4;
    uint8_t *src = data;
    int srcStride = size;

    // 由于window的stride和帧的stride不同,因此需要逐行复制
    int h;
    for (h = 0; h < frameHeight; h++) {
        memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
    }

    ANativeWindow_unlockAndPost(nativeWindow);
}

bool RGBRenderer::isPreparedSuccess() {
    return isPrepareSuccess;
}


void RGBRenderer::clearScreen() {
    if (isPrepareSuccess == false) {
        return;
    }
    if (pBlackFrame == NULL) {
        return;
    }
    render(pBlackFrame);
}











