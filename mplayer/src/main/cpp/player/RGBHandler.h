/**
 * 此播放器结合优化器Optimize使用
 */
#ifndef RTSPPLAYER_RTSPPLAYER_H
#define RTSPPLAYER_RTSPPLAYER_H

#include <deque>
#include <android/native_window.h>
#include <pthread.h>
#include "Codec.h"
#include "Convertor.h"
#include "RGBRenderer.h"
#include "AudioPlayer.h"
#include "Optimize.h"
#include "../common/Jpeg.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

class RGBHandler {
public :
    RGBHandler();

    virtual ~RGBHandler();

    //释放资源
    void close();

    //重置为初始状态
    void reset();

    //视频解码器是否已准备成功
    bool isPrepareVideoDecoderSuccess();

    //音频解码器是否已准备成功
    bool isPrepareAudioDecoderSuccess();

    //RGB渲染器是否已准备成功
    bool isPrepareRGBRenderSuccess();

    //*********************************************************************
    //***************************视频接口**********************************
    //*********************************************************************

    //设置应用层获得的nativeWindow
    void setANativeWindow(ANativeWindow *nativeWindow);

    //获得GRB渲染器需要使用的nativeWindow
    ANativeWindow *getANativeWindow();

    //设置视频解码类型
    void setVideoDecodecID(AVCodecID codecID);

    //设置视频原始大小
    void setVideoFrameSize(int frameWidth, int frameHeight);

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

    //是否正在渲染
    bool isRendering();

    //视频播放工作是否启动
    bool isVideoPlaying();

    //渲染过程（阻塞、耗时，用户无需调用）
    void doRenderWork();

    //视频解码过程（阻塞、耗时，用户无需调用）
    void doDecodeWork();

    //音频播放过程（阻塞、耗时，用户无需调用）
    void doPlayAudioWork();

    //*********************************************************************
    //***************************音频接口**********************************
    //*********************************************************************

    void playingAudio(AVFrame *pFrame);

    //清除存放解码后的音频队列里的数据
    void clearAudioDecodeQueue();

    //添加解码好的音频帧
    bool addHasDecodedAudioData(AVFrame *frame);

    //添加未解码的音频帧
    bool addUndecodeAudioData(AVPacket packet);

    //添加未解码的音频帧
    bool addUndecodeAudioData(unsigned char *data, int dataSize);

    //设置音频解码类型
    void setAudioDecodecID(AVCodecID codecID);

    //打开声音,在打开声音之前请确保已经添加了音频数据
    void soundOn();

    //关闭声音
    void soundOff();

    //声音是否打开
    bool isAudioSoundOn();

public :
    //转换器
    Convertor convertor;

    //GRB渲染器
    RGBRenderer rgbRenderer;

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

private:
    //优化器
    Optimize optimize;

    //是否正在渲染
    bool rendering = false;

    //是否抓图
    bool isCapture = false;

    //即将保存的抓图
    AVFrame *saveFrame = NULL;

    bool isSoundOn = false;
    Jpeg jpeg;

    //是否启动播放工作
    bool isPlaying = false;

    //视频帧队列，存放解码后的视频数据
    std::deque<AVFrame *> videoDecodeQueue = std::deque<AVFrame *>();
    //视频帧队列，存放未解码的视频数据
    std::deque<AVPacket> videoUndecodeQueue = std::deque<AVPacket>();
    //音频帧队列，存放解码后的音频数据
    std::deque<AVFrame *> audioDecodeQueue = std::deque<AVFrame *>();

    int frameWidth = 0;
    int frameHeight = 0;

    ANativeWindow *nativeWindow = NULL;

    AVCodecID videoCodecID;
    AVCodecID audioCodecID;

    pthread_t renderTid;
    pthread_t decodeTid;
    pthread_t audioTid;
    /*线程标示符*/
    pthread_attr_t renderAttr;
    pthread_attr_t decodeAttr;
    pthread_attr_t audioAttr;

private :
    //启动解码线程
    void startDecodeVideoThread();

    //启动渲染线程
    void startRenderVideoThread();

    //重置视频相关数量
    void resetVideoCount();

    //重置音频相关数量
    void resetAudioCount();

};

#endif //RTSPPLAYER_RTSPPLAYER_H
