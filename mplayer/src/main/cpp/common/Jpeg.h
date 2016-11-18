//
// Created by hwj on 2016/10/28.
//

#ifndef MEDIAAPP_JPEG_H
#define MEDIAAPP_JPEG_H
extern "C" {
#include "jpeglib.h"
#include "libavutil/frame.h"
}

class Jpeg {
public :

    /**
     * 将YUV保存为jpeg
     *  path:保存路径，后缀名为.jpg或.jpeg
     *  frame:解码后的YUV420P视频帧
     */
    int saveYuv2Jpeg(const char *path, AVFrame *frame);

    /**
     * 将YUV保存为jpeg
     * path:保存路径，后缀名为.jpg或.jpeg
     * yData:Y分量
     * uData:U分量
     * vData:V分量
     * quality: 图片质量，100为最佳
     * imageWidth:图像宽度
     * imageHeight:图像高度
     */
    int saveYuv2Jpeg(const char *path, unsigned char *yData, unsigned char *uData,
                     unsigned char *vData, int quality, int imageWidth, int imageHeight);

};

#endif //MEDIAAPP_JPEG_H

