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
typedef struct CodecParameters{

    //视频编码类型
    int videoCodecId=0;
    //视频宽高
    int width=0;
    int height=0;

    //音频编码类型
    int audioCodecId=0;
    //音频通道数
    int channels=0;
    //音频通道格式
    int channelLayout=0;
    //音频采样率
    int sampleRate=0;
}CodecParameters,*PCodecParameters;

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

    //获得已经接收到的原始视频帧数
    int getReceiveVideoPacketCount();

    //获得已经接收到的原始音频帧数
    int getReceiveAudioPacketCount();

    //获得编解码器参数
    //通过此参数可以获得视频的编码类型,视频宽高,音频采样率等等
    CodecParameters * getCodecParameters();

    //获得音频编解码器参数
    //通过此参数可以获得音频的编码类型，采样率（sample_rate），帧率（bit_rate），通道数（sample_rate），channel_layout等信息
    AVCodecParameters * getAudioCodecParameters();

    //获得视频编解码器参数
    AVCodecParameters * getVideoCodecParameters();

private :

    //视频编解码器参数
    AVCodecParameters * videoCodecParameters=NULL;

    //视频编解码器参数
    AVCodecParameters * audioCodecParameters=NULL;

    CodecParameters * codecParameters=NULL;

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

    int audioChannels;
    int audioChannelLayout;
    int audioSampleRate;
    int audioBitRate;

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
