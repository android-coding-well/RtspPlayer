//
// Created by hwj on 2017/2/20.
//

#include <memory.h>
#include <time.h>
#include "NativeCodec.h"
#include "log.h"

NativeCodec::NativeCodec() {
    codec = AMediaCodec_createDecoderByType("video/avc");
    if (codec == NULL) {
        return;
    }

    AMediaFormat *format = AMediaFormat_new();
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_WIDTH, 1920);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_HEIGHT, 1080);
    //AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_WIDTH, 480);
    // AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_HEIGHT, 320);
    AMediaFormat_setString(format, AMEDIAFORMAT_KEY_MIME, "video/avc");
    media_status_t ret = AMediaCodec_configure(codec, format, aNativeWindow, NULL, 0);
    LOGI("AMediaCodec_configure=%d", ret);
    AMediaCodec_start(codec);
    AMediaFormat_delete(format);
}

NativeCodec::~NativeCodec() {

}

void NativeCodec::doDecodeWork(uint8_t *data, size_t size) {
    ssize_t bufidx = -1;
    bufidx = AMediaCodec_dequeueInputBuffer(codec, 0);//-1 一直等待
    LOGI("input buffer %zd", bufidx);
    if (bufidx >= 0) {
        size_t bufsize;
        uint8_t *buf = AMediaCodec_getInputBuffer(codec, bufidx,
                                                  &bufsize);//第三个参数获得buffer的capacity(容量)大小
        LOGI("bufsize %d", bufsize);
        //填充数据进队列
        memcpy(buf, (const void *) data, size);
        AMediaCodec_queueInputBuffer(codec, bufidx, 0, size, clock(),
                                     0);
    }

    AMediaCodecBufferInfo info;
    ssize_t status = AMediaCodec_dequeueOutputBuffer(codec, &info, 0);
    LOGI("AMediaCodec_dequeueOutputBuffer %d", status);
    while (status >= 0) {
        LOGI("AMediaCodec_dequeueOutputBuffer %d  %d  %d  %d", info.flags, info.offset,
             info.presentationTimeUs, info.size);//info.size即为解码后数据的长度
        size_t outsize;
        //获得解码后数据
        uint8_t *outbuf = AMediaCodec_getOutputBuffer(codec, status, &outsize);
        LOGI("AMediaCodec_getOutputBuffer %d  %d", outbuf, outsize);
        //将解码后的缓存数据释放回编解码器
        AMediaCodec_releaseOutputBuffer(codec, status, true);
        status = AMediaCodec_dequeueOutputBuffer(codec, &info, 0);
    }
}

int NativeCodec::configDecoder(int frameWidth, int frameHeight, ANativeWindow *aNativeWindow) {
    if (frameWidth <= 0 || frameHeight <= 0) {
        return -1;
    }
    codec = AMediaCodec_createDecoderByType("video/avc");
    if (codec == NULL) {
        return -1;
    }

    AMediaFormat *format = AMediaFormat_new();
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_WIDTH, frameWidth);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_HEIGHT, frameHeight);
    //AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_WIDTH, 480);
    // AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_HEIGHT, 320);
    AMediaFormat_setString(format, AMEDIAFORMAT_KEY_MIME, "video/avc");
    media_status_t ret = AMediaCodec_configure(codec, format, aNativeWindow, NULL, 0);
    LOGI("AMediaCodec_configure=%d", ret);
    AMediaCodec_start(codec);
    AMediaFormat_delete(format);
    return 1;
}

int NativeCodec::inputData(uint8_t *data, int size) {
    if (codec == NULL) {
        return -1;
    }
    ssize_t bufidx = -1;
    //1 出队得到输入缓存的id
    bufidx = AMediaCodec_dequeueInputBuffer(codec, 0);//第二个参数为-1时表示一直等待
    LOGI("input buffer %zd", bufidx);
    if (bufidx >= 0) {
        size_t bufsize;
        //2 根据id获得输入缓存
        uint8_t *buf = AMediaCodec_getInputBuffer(codec, bufidx,
                                                  &bufsize);//第三个参数获得buffer的capacity(容量)大小
        LOGI("bufsize %d", bufsize);
        //3 将数据填充进缓存
        memcpy(buf, (const void *) data, size);
        //4 入列
        AMediaCodec_queueInputBuffer(codec, bufidx, 0, size,
                                     clock(),//时间戳
                                     0);
        return 1;
    }
    return -1;
}

uint8_t *NativeCodec::getDecodeData(size_t &outSize) {
    AMediaCodecBufferInfo info;
    //
    ssize_t bufidx = AMediaCodec_dequeueOutputBuffer(codec, &info, 0);
    LOGI("AMediaCodec_dequeueOutputBuffer %d", bufidx);
    uint8_t *outbuf = NULL;
    if (bufidx >= 0) {
        LOGI("AMediaCodec_dequeueOutputBuffer %d  %d  %d  %d", info.flags, info.offset,
             info.presentationTimeUs, info.size);//info.size即为解码后数据的长度
        //size_t outsize;
        //获得解码后数据
        outbuf = AMediaCodec_getOutputBuffer(codec, bufidx, &outSize);
        LOGI("AMediaCodec_getOutputBuffer %d  %d", outbuf, outSize);
        //将解码后的缓存数据释放回编解码器
        AMediaCodec_releaseOutputBuffer(codec, bufidx, aNativeWindow == NULL ? true : false);
    }
    return outbuf;
}












