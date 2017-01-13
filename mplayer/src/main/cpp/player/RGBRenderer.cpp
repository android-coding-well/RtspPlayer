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
    if (rgbBuffer != NULL) {
        av_free(rgbBuffer);
        rgbBuffer == NULL;
    }
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
    LOGI("prepare：frameWidth and frameHeight:%d-%d", frameWidth, frameHeight);
    isPrepareSuccess = false;
    if (nativeWindow == NULL) {
        LOGI("prepare：nativeWindow is null");
        return -1;
    }
    this->nativeWindow = nativeWindow;
    if (frameWidth <= 0 || frameHeight <= 0) {
        LOGI("prepare：frameWidth and frameHeight is error:%d-%d", frameWidth, frameHeight);
        return -1;
    }
    this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;
    // 设置native window的buffer大小,可自动拉伸
    ANativeWindow_setBuffersGeometry(nativeWindow, frameWidth, frameHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    if (pBlackFrame == NULL) {
        pBlackFrame = av_frame_alloc();
        int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, frameWidth, frameHeight, 1);
        rgbBuffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
        av_image_fill_arrays(pBlackFrame->data, pBlackFrame->linesize, rgbBuffer, AV_PIX_FMT_RGBA,
                             frameWidth, frameHeight, 1);
    }
    isPrepareSuccess = true;
    LOGI("prepare success");
    return 1;
}

int RGBRenderer::render(AVFrame *pFrameRGBA) {
    if (pFrameRGBA == NULL) {
        return -1;
    }
    return render(pFrameRGBA->data[0], pFrameRGBA->linesize[0]);
}

int RGBRenderer::render(uint8_t *data, int size) {

    if (isPrepareSuccess == false) {
        LOGI("render：prepare do not success");
        return -1;
    }
    if (data == NULL || size <= 0) {
        LOGI("render：data or size is empty");
        return -1;
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
    LOGI("render success");
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
    frameWidth = 0;
    frameHeight = 0;
}

















