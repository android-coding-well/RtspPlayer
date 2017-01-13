//
// rtsp播放器，结合了RtspPlayer使用
// Created by hwj on 2016/10/24.
//

#ifndef MEDIAAPP_MEDIAPLAYER2_H
#define MEDIAAPP_MEDIAPLAYER2_H

#include <jni.h>
#include <deque>
#include "../common/Jpeg.h"
#include <android/native_window.h>
#include <pthread.h>

#include "../player/AudioPlayer.h"
#include "../player/StreamTaker.h"
#include "../player/Convertor.h"
#include "../player/Codec.h"
#include "../player/RtspPlayer.h"

class MediaPlayer2 {
public :

    MediaPlayer2();

    virtual ~MediaPlayer2();

    //播放前准备工作
    //url:rtsp Url
    int prepare(const char *url);


    //开始播放
    void startPlay();


    //停止播放(针对播放器2)
    void stopPlay2();

    //keepSoundStatus:是否保持原来的声音开关状态
    void stopPlay2(bool keepSoundStatus);

    //关闭
    void close();

    //抓拍
    //savePath:抓拍图片保存路径（包含图片名称），目前只支持jpeg格式
    int capture(const char *savePath);

    //打开声音
    void soundOn();

    //关闭声音
    void soundOff();

    //视频是否正在播放
    bool isVideoPlaying();

    //声音是否打开
    bool isAudioSoundOn();



    // AudioPlayer audioPlayer;

    //取流器
    StreamTaker streamTaker;

    // Convertor convertor;

    //音频解码器
    //Codec audioCodec;

    //视频帧队列，存放解码后的视频数据
    //std::deque<AVFrame *> videoQueue;
    //音频队列，存放解码后的音频数据
    //std::deque<AVFrame *> audioQueue;

    //视频帧宽高
    int frameWidth = 0;
    int frameHeight = 0;


    //设置应用层获得的nativeWindow
    void setANativeWindow(ANativeWindow *nativeWindow);

    //获得GRB渲染器需要使用的nativeWindow
    // ANativeWindow *getANativeWindow();


    //是否抓图
    //bool isCapture = true;
    //即将保存的抓图
    // AVFrame *saveFrame = NULL;

    RtspPlayer rtspPlayer;
private :
    //ffmpeg锁管理器，防止多路播放崩溃
    static int lockmgr(void **mtx, enum AVLockOp op);

private :

    //锁
    pthread_mutex_t mutex;

};

#endif //MEDIAAPP_MEDIAPLAYER_H
