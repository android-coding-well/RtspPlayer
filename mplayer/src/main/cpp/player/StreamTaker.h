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

#include <pthread.h>

typedef void (*PacketCallback)(void *handle, AVPacket packet);

class StreamTaker {
public :
    StreamTaker();

    ~StreamTaker();

    //设置原始数据包回调函数
    //handle 回调设置者句柄
    //IVideoCallback 回调函数
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

    //取流过程（阻塞、耗时，用户无需调用）
    void takingStream();

    //获得视频编码类型
    AVCodecID getVideoCodeID();

    //获得音频编码类型
    AVCodecID getAudioCodeID();

    //获得原始视频帧的宽度，在prepare后才有效
    int getFrameWidth();

    //获得原始视频帧的高度，在prepare后才有效
    int getFrameHeight();

    int getReceiveVideoPacketCount();

    int getReceiveAudioPacketCount();

private :
    //已经接收到的流的个数
    int hasReceiveVideoPacketCount=0;
    int hasReceiveAudioPacketCount=0;


    //是否取流
    bool isTake = false;

    AVFormatContext *pFormatCtx = NULL;

    int videoStream;
    int audioStream;

    int videoFrameWidth=0;

    int videoFrameHeight=0;

    //数据包回调函数
    PacketCallback videoCallback = NULL;
    PacketCallback audioCallback = NULL;

    //回调设置者句柄
    void *handle;

    //编码类型
    AVCodecID videoCodecID = AV_CODEC_ID_NONE;
    AVCodecID audioCodecID = AV_CODEC_ID_NONE;

    //取流准备是否成功
    bool isPrepareSuccess = false;

    pthread_t tid;
    /*线程标示符*/
    pthread_attr_t attr;
};

#endif //MEDIAAPP_STREAMTAKER_H
