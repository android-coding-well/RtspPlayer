//
// YUV视频处理器，是对解码器和渲染器的综合使用
// Created by hwj on 2017/2/6.
//

#ifndef SAFETY1_1_0_YUVHANDLER_H
#define SAFETY1_1_0_YUVHANDLER_H

#include <deque>
#include <android/native_window.h>
#include <pthread.h>
#include "Codec.h"
#include "Convertor.h"
#include "RGBRenderer.h"
#include "AudioPlayer.h"
#include "Optimize.h"
#include "YUV420PRenderer.h"
#include "../common/Jpeg.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

class YUVHandler{
public :

    YUVHandler();

    virtual ~YUVHandler();

    //重置相关参数
    void  reset();

    //视频解码器是否已准备成功
    bool isPrepareVideoDecoderSuccess();

    //设置视频解码类型
    void setVideoDecodecID(AVCodecID codecID);

    //添加解码好的视频帧
    //return 是否添加成功
    bool addHasDecodedVideoData(AVFrame *frame);

    //添加未解码的视频帧
    //data:（in）原始数据
    //dataSize:(in)数据长度
    //return 是否添加成功
    bool addUndecodeVideoData(unsigned char *data, int dataSize);

    //清除视频队列里的数据
    void clearVideoQueue();

    //启动解码线程
    void startDecodeVideoThread();

    //视频解码过程（阻塞、耗时，用户无需调用）
    void doDecodeWork();


    //创建YUV渲染器
    bool createYuvRender();

    //渲染帧，从已经解码好的队列中取出一帧数据进行渲染绘制
    //note：需要YUV渲染器创建成功
    void onDraw();

    //重置窗口大小，在显示窗体宽高发生变化时调用
    //note：需要YUV渲染器创建成功
    //winWidth:实际显示的宽度
    //winHeight:实际显示的高度
    bool resizeWindows(int winWidth, int winHeight);

    //开始播放
    //开始播放只是开启了解码线程，待队列达到缓存帧数时才会开启渲染线程
    //在开启渲染线程后isRendering()会返回true
    //在开始播放前要确保已经设置了ANativeWindow，解码类型，原始视频大小，解码后的数据
    void startPlay();

    //停止播放
    //默认关闭声音开关
    void stopPlay();

    //停止播放
    //keepSoundStatus：是否保持原来的声音开关状态
    void stopPlay(bool keepSoundStatus);

    //抓拍
    //savePath:抓拍图片保存路径（包含图片名称），目前只支持jpeg格式和png格式
    int capture(const char *savePath);

public :
    //转换器
    Convertor convertor;

    //编解码器
    Codec videoCodec;

    //音频解码器
    Codec audioCodec;

    //音频播放器
    AudioPlayer audioPlayer;

    //收到的帧数
    int receiveVideoPacketCount = 0;
    int receiveAudioPacketCount = 0;

    //解码的帧数
    int decodeVideoFrameCount = 0;
    int decodeAudioFrameCount = 0;

    //解码失败的帧数
    int decodeFailedVideoFrameCount = 0;
    int decodeFailedAudioFrameCount = 0;

    //渲染的帧数
    int renderFrameCount = 0;
    //播放的音频数
    int playAudioCount = 0;

    //视频帧队列，存放解码后的视频数据
    std::deque<AVFrame *> videoDecodeQueue = std::deque<AVFrame *>();
    //视频帧队列，存放未解码的视频数据
    std::deque<AVPacket> videoUndecodeQueue = std::deque<AVPacket>();
private :

    //yuv渲染器
    YUV420PRenderer *yuvRenderer;

    //音视频解码类型
    AVCodecID videoCodecID;
    AVCodecID audioCodecID;

    //流畅度优化器
    Optimize optimize;

    pthread_t renderTid;
    pthread_t decodeTid;
    pthread_t audioTid;
    /*线程标示符*/
    pthread_attr_t renderAttr;
    pthread_attr_t decodeAttr;
    pthread_attr_t audioAttr;


    Jpeg jpeg;
    //即将保存的抓图
    AVFrame *saveFrame = NULL;
    //是否抓图
    bool isCapture = false;

    //是否启动播放工作
    bool isPlaying = false;

    bool isSoundOn = false;


private:
    //重置视频相关数量
    void resetVideoCount();

    //重置音频相关数量
    void resetAudioCount();
};

#endif //SAFETY1_1_0_YUVHANDLER_H
