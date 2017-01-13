//
// Created by hwj on 2016/11/17.
//

#ifndef RTSPPLAYER_GRBRENDERER_H
#define RTSPPLAYER_GRBRENDERER_H

#include <android/native_window.h>
#include "log.h"

extern "C" {
#include "libavutil/frame.h"
}

class RGBRenderer {

public :
    RGBRenderer();

    ~RGBRenderer();

    //重置参数
    void reset();

    //渲染前准备相关资源
    //nativeWindow：渲染窗体
    //frameWidth:视频帧宽度（非窗体宽度）
    //frameHeight:视频帧高度（非窗体高度）
    //return 1--success
    int prepare(ANativeWindow *nativeWindow, int frameWidth, int frameHeight);

    //渲染rgb数据
    //pFrameRGBA:转换后的rgb帧
    //return 1--success
    int render(AVFrame *pFrameRGBA);

    //渲染rgb数据
    //rgbData:rgb数据
    //size:大小
    //return 1--success
    int render(uint8_t *rgbData, int size);

    //清除画面，即黑屏
    void clearScreen();

    bool isPreparedSuccess();


private :

    bool isPrepareSuccess = false;
    ANativeWindow *nativeWindow=NULL;
    ANativeWindow_Buffer windowBuffer;

    int frameWidth=0;
    int frameHeight=0;

    //黑屏帧
    AVFrame *pBlackFrame = NULL;
    uint8_t * rgbBuffer=NULL;
};


#endif //RTSPPLAYER_GRBRENDERER_H
