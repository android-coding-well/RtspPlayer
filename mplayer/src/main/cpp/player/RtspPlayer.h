//
// Created by hwj on 2016/12/28.
//

#ifndef RTSPPLAYER_RTSPPLAYER_H
#define RTSPPLAYER_RTSPPLAYER_H

#include <deque>
#include <android/native_window.h>
#include <pthread.h>
#include "Codec.h"
#include "Convertor.h"
#include "RGBRenderer.h"
#include "../common/Jpeg.h"
#include "AudioPlayer.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

class RtspPlayer {
public :
    RtspPlayer();

    virtual ~RtspPlayer();

    void close();

    //视频解码器是否已准备成功
    boolean isPrepareVideoDecoderSuccess();

    //音频解码器是否已准备成功
    boolean isPrepareAudioDecoderSuccess();

    //RGB渲染器是否已准备成功
    boolean isPrepareRGBRenderSuccess();

    //*********************************************************************
    //***************************视频接口**********************************
    //*********************************************************************
    //开始播放
    void startPlay();

    //渲染过程（阻塞、耗时，用户无需调用）
    void render();

    void doDecodeWork();
    void doPlayAudioWork();
    //停止播放
    //默认关闭声音开关
    void stopPlay();

    //停止播放
    //keepSoundStatus：是否保持原来的声音开关状态
    void stopPlay( boolean keepSoundStatus);


    //添加解码好的视频帧
    //return 是否添加成功
    bool addHasDecodedVideoData(AVFrame *frame);

    //添加未解码的视频帧
    //data:（in）原始数据
    //dataSize:(in)数据长度
    //return 是否添加成功
    bool addUndecodeVideoData(unsigned char* data,int dataSize);

    //设置应用层获得的nativeWindow
    void setANativeWindow(ANativeWindow *nativeWindow);

    //获得GRB渲染器需要使用的nativeWindow
    ANativeWindow *getANativeWindow();

    //设置视频解码类型
    void setVideoDecodecID(AVCodecID codecID);

    //设置视频原始大小
    void setVideoFrameSize(int frameWidth, int frameHeight);

    //清除存放解码后的视频队列里的数据
    void clearVideoDecodeQueue();

    //抓拍
    //savePath:抓拍图片保存路径（包含图片名称），目前只支持jpeg格式
    int capture(const char *savePath);

    //视频是否正在播放
    bool isVideoPlaying();

    //*********************************************************************
    //***************************音频接口**********************************
    //*********************************************************************

    void playingAudio( AVFrame * pFrame);

    //清除存放解码后的音频队列里的数据
    void clearAudioDecodeQueue();

    //添加解码好的音频帧
    bool addHasDecodedAudioData(AVFrame *frame);

    //添加未解码的音频帧
    bool addUndecodeAudioData(AVPacket packet);
    bool addUndecodeAudioData(unsigned char* data,int dataSize);
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

    Codec videoCodec;

    //音频解码器
    Codec audioCodec;

    AudioPlayer audioPlayer;

    //收到的帧数
    int receiveVideoPacketCount=0;
    int receiveAudioPacketCount=0;
    //解码的帧数
    int decodeVideoFrameCount=0;
    //解码失败的帧数
    int decodeFailedVideoFrameCount=0;
    int decodeAudioFrameCount=0;
    int decodeFailedAudioFrameCount=0;
    //渲染的帧数
    int renderFrameCount=0;
    //播放的音频数
    int playAudioCount=0;
private:
    //是否抓图
    bool isCapture = false;
    //即将保存的抓图
    AVFrame *saveFrame = NULL;
    bool isSoundOn = false;
    Jpeg jpeg;
    //是否正在播放
    bool isPlaying = false;

    //视频帧队列，存放解码后的视频数据
    std::deque<AVFrame *> videoDecodeQueue=std::deque<AVFrame *>();

    std::deque<AVPacket > videoUndecodeQueue=std::deque<AVPacket >();

    //音频帧队列，存放解码后的音频数据
    std::deque<AVFrame *> audioDecodeQueue=std::deque<AVFrame *>();

    int frameWidth = 0;
    int frameHeight = 0;

    ANativeWindow *nativeWindow = NULL;
    AVCodecID videoCodecID;
    AVCodecID audioCodecID;


    pthread_t tid;
    pthread_t tid2;
    pthread_t tid3;
    /*线程标示符*/
    pthread_attr_t attr;
    pthread_attr_t attr2;
    pthread_attr_t attr3;

private :
    void resetVideoCount();
    void resetAudioCount();
public:
    void reset();


};

#endif //RTSPPLAYER_RTSPPLAYER_H
