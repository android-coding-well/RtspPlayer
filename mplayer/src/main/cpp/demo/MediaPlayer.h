//
// rtsp播放器，采用RGB和YUV两种渲染方式
// Created by hwj on 2016/10/24.
//

#ifndef MEDIAAPP_MEDIAPLAYER_H
#define MEDIAAPP_MEDIAPLAYER_H

#include <jni.h>
#include <deque>
#include "../common/Jpeg.h"
#include <android/native_window.h>

#include "../player/AudioPlayer.h"
#include "../player/StreamTaker.h"
#include "../player/YUV420PRenderer.h"
#include "../player/Codec.h"
#include "../player/Convertor.h"
#include "../player/RGBRenderer.h"

class MediaPlayer {
public :

    MediaPlayer();

    ~MediaPlayer();

    //播放前准备工作
    //url:rtsp Url
    int prepare(const char *url);

    int prepare2(const char *url);

    //开始播放
    void startPlay();

    //停止播放
    void stopPlay();

    //停止播放(针对播放器2)
    void stopPlay2(bool isClearScreen);

    //关闭
    void close();

    //创建YUV渲染器
    bool createYuvRenderer();

    //渲染帧
    void onDraw();

    //重置窗口大小
    bool resizeWindows(int width, int height);

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

    AudioPlayer audioPlayer;
    //取流器
    StreamTaker streamTaker;

    //YUV渲染器
    YUV420PRenderer *yuvRenderer = NULL;

    //视频解码器
    Codec videoCodec;
    //音频解码器
    Codec audioCodec;

    //视频帧队列，存放解码后的视频数据
    std::deque<AVFrame *> videoQueue;
    //音频队列，存放解码后的音频数据
    std::deque<AVFrame *> audioQueue;

    //视频帧宽高
    int frameWidth = 0;
    int frameHeight = 0;

    //将音频数据转为非平面16位数据
    //pFrame（in）:解码后的音频帧
    //audioBuffer（out）: 转换后的数据(缓冲大小一般大于音频帧数据的两倍)
    //dataSize（out）:转换后的大小
    //return: 1--success
    int convertToS16(AVFrame *pFrame, uint8_t **audioBuffer, int &dataSize);

    //设置应用层获得的nativeWindow
    void setANativeWindow(ANativeWindow *nativeWindow);

    //获得GRB渲染器需要使用的nativeWindow
    ANativeWindow *getANativeWindow();

    //转换器
    Convertor convertor;

    //GRB渲染器
    RGBRenderer rgbRenderer;

    //是否抓图
    bool isCapture = true;
    //即将保存的抓图
    AVFrame *saveFrame = NULL;

private :
    //ffmpeg锁管理器，防止多路播放崩溃
    static int lockmgr(void **mtx, enum AVLockOp op);

    ANativeWindow *nativeWindow = NULL;
private :

    //是否正在播放
    bool isPlaying = false;

    bool isSoundOn = false;

    //锁
    pthread_mutex_t mutex;

};

#endif //MEDIAAPP_MEDIAPLAYER_H
