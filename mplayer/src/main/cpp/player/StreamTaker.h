//
// rtsp取流器
// Created by hwj on 2016/10/28.
//

#ifndef MEDIAAPP_STREAMTAKER_H
#define MEDIAAPP_STREAMTAKER_H


extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
};

typedef void (*PacketCallback)(void *handle, AVPacket packet);

class StreamTaker {
public :
    StreamTaker();

    ~StreamTaker();

    //设置原始数据包回调函数
    //handle 回调设置者句柄
    //callback 回调函数
    void setVideoPacketCallback(void *handle, PacketCallback callback);
    void setAudioPacketCallback(void *handle, PacketCallback callback);

    //取流准备
    //url rtsp地址
    //return:1--success
    int prepare(const char *url);

    //开始取流
    void startTakeStream();

    //停止取流
    void stopTakeStream();

    //获得编码类型
    AVCodecID getVideoCodeID();
    //获得编码类型
    AVCodecID getAudioCodeID();
private :

    //数据包回调函数
    PacketCallback videoCallback = NULL;
    PacketCallback audioCallback = NULL;

    int videoStream;
    int audioStream;

    AVFormatContext *pFormatCtx = NULL;

    //是否取流
    bool isTake = false;

    //编码类型
    AVCodecID videoCodecID=AV_CODEC_ID_NONE;
    AVCodecID audioCodecID=AV_CODEC_ID_NONE;

    //回调设置者句柄
    void *handle;

    //取流准备是否成功
    bool isPrepareSuccess = false;
};

#endif //MEDIAAPP_STREAMTAKER_H
