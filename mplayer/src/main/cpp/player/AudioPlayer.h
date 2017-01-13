//
// 音频播放器(基于ffmpeg,播放非平面音频数据)
// 参考ndk-r10e下sample/native-audio/jni/native-audio-jni.c
// Created by hwj on 2016/11/7.
//

#ifndef MEDIAAPP_AUDIOPLAYER_H
#define MEDIAAPP_AUDIOPLAYER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "stdlib.h"
#include "log.h"

extern "C" {
#include "libavutil/frame.h"
#include "libavutil/channel_layout.h"
#include "libswresample/swresample.h"
};

class AudioPlayer {
public :
    AudioPlayer();

    ~AudioPlayer();

    //设置回调
    //callback：每当缓冲音频播放完毕后会回调此函数（可在此函数里实现更新缓冲音频）
    //context：句柄
    void setCallback(slAndroidSimpleBufferQueueCallback callback, void *context);

    //创建engine（关键，必须保证此函数创建成功）
    //return 1--成功 -1--失败
    int createEngine();

    //创建缓冲队列音频播放器（需要在createEngine成功的前提下有效）
    //sampleRate: 采样率
    //channels: 通道数
    //return 1--成功 -1--失败
    int createBufferQueueAudioPlayer(int sampleRate, int channels);

    //添加一帧音频数据进队列，如果播放器创建成功则会立即播放这一帧
    //pBuffer：帧数据
    //size：帧大小（即实际占用的字节数）
    //return 1--成功 -1--失败
    int enqueue(const void *pBuffer, unsigned int size);

    //关闭，释放资源
    void shutdown();

    //缓冲队列回调（缓冲队列音频数据播放完毕时会回调此函数）
    slAndroidSimpleBufferQueueCallback callback;

    //回调接收者
    void *context;

    bool isCreateAudioPlayerSuccess();

    bool isCreateEngineSuccess();

    //将音频数据转为非平面16位数据
    //pFrame（in）:解码后的音频帧
    //audioBuffer（out）: 转换后的数据(缓冲大小一般大于音频帧数据的两倍)
    //dataSize（out）:转换后的实际大小
    //return: 1--success
    int convertToS16(AVFrame *pFrame, uint8_t **audio_buff, int &data_size);

private :

    bool isCreatedEngineSuccess = false;

    bool isCreatedAudioPlayerSuccess = false;
    // engine interfaces
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine;

    // buffer queue player interfaces
    SLObjectItf bqPlayerObject = NULL;
    SLPlayItf bqPlayerPlay;
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
    SLEffectSendItf bqPlayerEffectSend;
    SLVolumeItf bqPlayerVolume;

    // output mix interfaces
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

    // aux effect on the output mix, used by the buffer queue player
    const SLEnvironmentalReverbSettings reverbSettings =
            SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

};

#endif //MEDIAAPP_AUDIOPLAYER_H
