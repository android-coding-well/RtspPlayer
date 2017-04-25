//
// 编解码器(支持音频与视频)
// 注意：编码暂时未测试过，勿用
// 需要ffmpeg libavcodec库支持
// Created by hwj on 2016/10/28.
//

#ifndef MEDIAAPP_CODEC_H
#define MEDIAAPP_CODEC_H
extern "C"
{
#include <libavcodec/avcodec.h>
};


class Codec {
public :
    Codec();

    ~Codec();

    //关闭，释放资源
    void close();

    //重置参数
    void reset();

    //解码准备
    //codecID 解码id （AVCodecID.AV_CODEC_ID_HEVC:即h265）
    //return 1--success
    int prepareDecode(AVCodecID codecID);


    //设置音频参数,如果没有则会采用默认参数，默认参数有可能使得解码失败
    //需要在prepareDecode之前设置才生效
    //channels[in]:通道数
    //channelLayout[in]:通道设计，即单声道，双声道等（默认DEFAULT_AUDIO_CHANNEL_LAYOUT），详见libavutil/channel_layout.h中宏定义，如AV_CH_LAYOUT_STEREO
    //sampleRate[in]:采样率（默认DEFAULT_AUDIO_SAMPLE_RATE）
    void setAudioCodecParameters(int channels,int channelLayout,int sampleRate);


    //编码准备（未测试，请勿使用）
    //codecID 编码id
    //return 1--success
    int prepareEncode(AVCodecID codecID);

    //解码
    //packet 原始数据包
    //return 解码帧，NULL则表示解码失败
    AVFrame *decode(AVPacket packet);

    //解码
    //data: 原始数据
    //dataSize:数据长度
    //return 解码帧，NULL则表示解码失败
    AVFrame *decode(unsigned char *data, int dataSize);

    //prepareDecode是否成功
    bool isPrepareDecodeSuccess();

    //prepareEncode是否成功（未测试，请勿使用）
    bool isPrepareEncodeSuccess();

    //编码（未测试，请勿使用）
    //packet 帧数据
    //return 解码数据包
    AVPacket *encode(AVFrame *pFrame);

    //获得编解码器相关信息
    AVCodecContext *getAVCodecContext();

private :

    AVCodec *pCodec = NULL;
    AVCodecContext *pCodecCtx = NULL;
    AVFrame *pFrame = NULL;
    AVPacket *packet = NULL;

    //解码准备是否成功，成功才可以进行后续解码
    bool isPrepareDecoderSuccess = false;
    bool isPrepareEncoderSuccess = false;

    //默认的音频通道数
    int DEFAULT_AUDIO_CHANNELS=2;
    //默认的音频channel_layout
    int DEFAULT_AUDIO_CHANNEL_LAYOUT=AV_CH_LAYOUT_STEREO;
    //默认的音频采样率
    int DEFAULT_AUDIO_SAMPLE_RATE=44100;
    //默认的比特率
    int DEFAULT_AUDIO_BIT_RATE=128000;

    int sampleRate;
    int channels;
    int channelLayout;

};

#endif //MEDIAAPP_CODEC_H
