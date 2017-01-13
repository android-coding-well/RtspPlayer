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
    //frameWidth:帧宽度
    //frameHeight:帧高度
    //frameHeight:帧高度
    //pix_fmt:视频数据格式,见AVPixelFormat，一般为AV_PIX_FMT_YUV420P
    //return: 1--success
    int prepareYUV2RGB(int frameWidth, int frameHeight, AVPixelFormat pix_fmt);

    //开始转换(需要prepareYUV2RGB返回成功)
    //pFrame:YUV帧（YUV420P）
    //return :转换后的帧数据,NULL则表示转换失败
    AVFrame *YUV2RGB(AVFrame *pFrame);

    //开始转换（需要prepareYUV2RGB返回成功）
    //data:YUV数据（YUV420P）
    //size:YUV大小
    //return :转换后的帧数据,NULL则表示转换失败
    AVFrame *YUV2RGB(uint8_t ** data,int size[]);

    //将音频数据转为非平面16位数据
    //pFrame（in）:解码后的音频帧
    //audioBuffer（out）: 转换后的数据(缓冲大小一般大于音频帧数据的两倍)
    //dataSize（out）:转换后的实际大小
    //return: 1--success
    int audioToS16(AVFrame *pFrame,uint8_t ** audioBuffer,int& dataSize);

    bool isPreparedYUV2RGBSuccess();

private :

    uint8_t * rgbBuffer=NULL;
    bool isPrepareYUV2RGBSuccess = false;

    struct SwsContext *sws_ctx=NULL;

    int frameWidth=0;
    int frameHeight=0;

    AVFrame *pFrameRGBA=NULL;
};


#endif //RTSPPLAYER_CONVERTOR_H
