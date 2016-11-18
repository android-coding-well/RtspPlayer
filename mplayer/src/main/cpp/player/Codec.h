//
// 编解码器(支持音频与视频)
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

    //解码准备
    //codecID 解码id
    //return 1--success
    int prepareDecode(AVCodecID codecID);

    //编码准备（未测试，请勿使用）
    //codecID 编码id
    //return 1--success
    int prepareEncode(AVCodecID codecID);

    //解码
    //packet 原始数据包
    //return 解码帧
    AVFrame *decode(AVPacket packet);

    //prepareDecode是否成功
    bool isPrepareDecodeSuccess();

    //prepareEncode（未测试，请勿使用）
    bool isPrepareEncodeSuccess();

    //编码（未测试，请勿使用）
    //packet 帧数据
    //return 解码数据包
    AVPacket* encode( AVFrame * pFrame);

    //关闭，是否资源
    void close();

    //获得编解码器相关信息
    AVCodecContext * getAVCodecContext();

private :
    AVCodec *pCodec = NULL;
    AVCodecContext *pCodecCtx = NULL;
    AVFrame *pFrame = NULL;
    AVPacket *packet = NULL;

    //解码准备是否成功，成功才可以进行后续解码
    bool isPrepareDecoderSuccess = false;
    bool isPrepareEncoderSuccess = false;

};

#endif //MEDIAAPP_CODEC_H
