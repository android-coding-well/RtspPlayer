//
// Created by hwj on 2016/10/28.
//

#include "Jpeg.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <log.h>

#define  LOG_TAG    "Jpeg"
extern "C" {

int Jpeg::saveYuv2Jpeg(const char *path, unsigned char *yData, unsigned char *uData,
                       unsigned char *vData, int quality, int imageWidth, int imageHeight) {

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE *outfile;    // target file
    JSAMPIMAGE buffer;
    unsigned char *pSrc, *pDst;
    int band, i, buf_width[3], buf_height[3];
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    if ((outfile = fopen(path, "wb")) == NULL) {
        LOGI("Open file: %s failed.",path);
        return -1;
    }
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = imageWidth;  // image width and height, in pixels
    cinfo.image_height = imageHeight;
    cinfo.input_components = 3;    // # of color components per pixel
    cinfo.in_color_space = JCS_RGB;  //colorspace of input image
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    //////////////////////////////
    cinfo.raw_data_in = TRUE;
    cinfo.jpeg_color_space = JCS_YCbCr;
    cinfo.comp_info[0].h_samp_factor = 2;
    cinfo.comp_info[0].v_samp_factor = 2;
    /////////////////////////
    jpeg_start_compress(&cinfo, TRUE);
    buffer = (JSAMPIMAGE) (*cinfo.mem->alloc_small)((j_common_ptr) &cinfo,
                                                    JPOOL_IMAGE, 3 * sizeof(JSAMPARRAY));
    for (band = 0; band < 3; band++) {
        buf_width[band] = cinfo.comp_info[band].width_in_blocks * DCTSIZE;
        buf_height[band] = cinfo.comp_info[band].v_samp_factor * DCTSIZE;
        buffer[band] = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo,
                                                  JPOOL_IMAGE, buf_width[band], buf_height[band]);
    }
    unsigned char *rawData[3];
    rawData[0] = yData;
    rawData[1] = uData;
    rawData[2] = vData;
    int src_width[3], src_height[3];
    for (i = 0; i < 3; i++) {
        src_width[i] = (i == 0) ? imageWidth : imageWidth / 2;
        src_height[i] = (i == 0) ? imageHeight : imageHeight / 2;
    }
    //max_line一般为16，外循环每次处理16行数据。
    int max_line = cinfo.max_v_samp_factor * DCTSIZE;
    int counter;
    for (counter = 0; cinfo.next_scanline < cinfo.image_height; counter++) {
        //buffer image copy.
        for (band = 0; band < 3; band++)  //每个分量分别处理
        {
            int mem_size = src_width[band];//buf_width[band];
            pDst = (unsigned char *) buffer[band][0];
            pSrc = (unsigned char *) rawData[band] + counter * buf_height[band] *
                                                     src_width[band];//buf_width[band];  //yuv.data[band]分别表示YUV起始地址

            for (i = 0; i < buf_height[band]; i++)  //处理每行数据
            {
                memcpy(pDst, pSrc, mem_size);
                pSrc += src_width[band];//buf_width[band];
                pDst += buf_width[band];
            }
        }
       // LOGI("11");
        jpeg_write_raw_data(&cinfo, buffer, max_line);
    }
    jpeg_finish_compress(&cinfo);
   // LOGI("12");
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
    //LOGI("13");
    return 1;
}

int Jpeg::saveYuv2Jpeg(const char *path, AVFrame *frame) {
    int VIDEO_WIDTH = frame->width;
    int VIDEO_HEIGHT = frame->height;
    int HALF_VIDEO_WIDTH = frame->width / 2;
    /*unsigned char* y;
    unsigned char* u;
    unsigned char* v;
    int j = 0,i = 0;//抽取YUV数据
    for (i = 0; i < VIDEO_HEIGHT; i++) {
        memcpy(&(y[VIDEO_WIDTH*i]), &frame->data[0][frame->linesize[0]*i], VIDEO_WIDTH);
        if (i % 2== 0) {
            memcpy(&(u[(HALF_VIDEO_WIDTH)*j]), &frame->data[1][frame->linesize[1]*j], HALF_VIDEO_WIDTH);
            memcpy(&(v[(HALF_VIDEO_WIDTH)*j]), &frame->data[2][frame->linesize[2]*j], HALF_VIDEO_WIDTH);
            j++;
        }
    }*/
    return saveYuv2Jpeg(path, frame->data[0], frame->data[1], frame->data[2], 100, VIDEO_WIDTH,
                        VIDEO_HEIGHT);
}


}
