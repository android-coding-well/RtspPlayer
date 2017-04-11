//
// 格式转换器
// Created by hwj on 2016/11/17.
//

#ifndef RTSPPLAYER_CONVERTOR_H
#define RTSPPLAYER_CONVERTOR_H
extern "C" {
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/frame.h"
#include "libavutil/channel_layout.h"
#include "libswresample/swresample.h"
}

#include "log.h"

class Convertor {

public :
    Convertor();

    ~Convertor();

    //YUV转RGB前准备
    //Note:以下三种prepare任选其一即可
    //frameWidth:帧宽度
    //frameHeight:帧高度
    //yuvFormat:指定输入格式,一般为AV_PIX_FMT_YUV420P
    //rgbFormat:指定输出格式，一般为AV_PIX_FMT_RGBA或AV_PIX_FMT_RGB565LE
    int prepareYUVToRGB(int frameWidth, int frameHeight, AVPixelFormat yuvFormat,AVPixelFormat rgbFormat);

    //YUV转RGBA前准备，默认输出格式为AV_PIX_FMT_RGBA
    //frameWidth:帧宽度
    //frameHeight:帧高度
    //yuvFormat:指定输入格式,一般为AV_PIX_FMT_YUV420P
    int prepareYUVToRGBA(int frameWidth, int frameHeight, AVPixelFormat yuvFormat);

    //YUV转RGB565前准备，默认输出格式为AV_PIX_FMT_RGB565LE
    //经测试，发现转换时间过长，能达到100ms,不推荐使用，用prepareYUVToRGBA代替
    //frameWidth:帧宽度
    //frameHeight:帧高度
    //yuvFormat:指定输入格式,一般为AV_PIX_FMT_YUV420P
    int prepareYUVToRGB565LE(int frameWidth, int frameHeight, AVPixelFormat yuvFormat);


    //开始转换(需要isPreparedYUVToRGBSuccess返回成功)
    //pFrame:YUV帧
    //return :转换后的帧数据
    AVFrame *YUV2RGB(AVFrame *pFrame);

    //开始转换（需要isPreparedYUVToRGBSuccess返回成功）
    //data:YUV数据
    //size:YUV大小
    //return :转换后的帧数据
    AVFrame *YUV2RGB(uint8_t ** data,int size[]);

    //将音频数据转为非平面16位数据
    //pFrame（in）:解码后的音频帧
    //audioBuffer（out）: 转换后的数据(缓冲大小一般大于音频帧数据的两倍)
    //dataSize（out）:转换后的实际大小
    //return: 1--success
    int audioToS16(AVFrame *pFrame,uint8_t ** audioBuffer,int& dataSize);

    bool isPreparedYUVToRGBSuccess();

private :
    bool isPrepareYUVToRGBSuccess = false;

    struct SwsContext *sws_ctx=NULL;

    int frameWidth=0;
    int frameHeight=0;

    AVFrame *pFrameRGBA=NULL;
};


#endif //RTSPPLAYER_CONVERTOR_H
