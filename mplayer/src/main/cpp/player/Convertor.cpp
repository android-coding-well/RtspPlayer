//
// Created by hwj on 2016/11/17.
//

#include "Convertor.h"

#define  LOG_TAG    "Convertor"

Convertor::Convertor() {

}

Convertor::~Convertor() {
    if(pFrameRGBA!=NULL){
        av_frame_free(&pFrameRGBA);
        pFrameRGBA=NULL;
    }


}

AVFrame * Convertor::YUV2RGB(AVFrame *pFrame) {
   /* if (pFrame == NULL) {
        return NULL;
    }
    if (isPrepareYUV2RGBSuccess == false) {
        return NULL;
    }
    // 格式转换
    sws_scale(sws_ctx, (uint8_t const *const *) pFrame->data,
              pFrame->linesize, 0, frameHeight,
              pFrameRGBA->data, pFrameRGBA->linesize);

    return pFrameRGBA;*/
    return YUV2RGB(pFrame->data, pFrame->linesize);
}

AVFrame * Convertor::YUV2RGB(uint8_t * * data,int size[]) {
    if (data == NULL||size==NULL) {
        return NULL;
    }
    if (isPrepareYUV2RGBSuccess == false) {
        return NULL;
    }
    // 格式转换
    sws_scale(sws_ctx, (uint8_t const *const *)data,
              size, 0, frameHeight,
              pFrameRGBA->data, pFrameRGBA->linesize);

    return pFrameRGBA;
}

int Convertor::prepareYUV2RGB(int frameWidth, int frameHeight, AVPixelFormat pix_fmt) {
    isPrepareYUV2RGBSuccess=false;
    if (frameWidth <= 0 || frameHeight <= 0 || pix_fmt == -1) {
        LOGI("Params is error,please check again.");
        return -1;
    }
    // 用于渲染
    pFrameRGBA = av_frame_alloc();
    if (pFrameRGBA == NULL) {
        LOGI("Could not allocate video frame.");
        return -1;
    }
    this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;

    // Determine required buffer size and allocate buffer
    // buffer中数据就是用于渲染的,且格式为RGBA
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA,  frameWidth, frameHeight,1);
    LOGI("prepare21");
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    LOGI("prepare22");
    av_image_fill_arrays( pFrameRGBA->data,  pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                          frameWidth, frameHeight, 1);
    sws_ctx = sws_getContext(frameWidth,
                             frameHeight,
                             pix_fmt,
                             frameWidth,
                             frameHeight,
                             AV_PIX_FMT_RGBA,
                             SWS_BILINEAR,
                             NULL,
                             NULL,
                             NULL);
    isPrepareYUV2RGBSuccess=true;
    return 1;
}

bool Convertor::isPreparedYUV2RGBSuccess() {
    return isPrepareYUV2RGBSuccess;
}

int Convertor::audioToS16(AVFrame *pFrame, uint8_t **audioBuffer, int &dataSize) {
    if (pFrame == NULL) {
        return -1;
    }

    AVSampleFormat dst_format = AV_SAMPLE_FMT_S16;//av_get_packed_sample_fmt((AVSampleFormat)frame->format);
    int64_t dst_layout = av_get_default_channel_layout(pFrame->channels);
    /*const int MAX_AUDIO_FRAME_SIZE = 8196;//8196 192000
    audioBuffer = (uint8_t *) malloc(sizeof(uint8_t) * MAX_AUDIO_FRAME_SIZE);//8196*/
    dataSize = 0;
    SwrContext *swr_ctx = swr_alloc_set_opts(NULL,
                                             dst_layout,
                                             dst_format,
                                             pFrame->sample_rate,
                                             pFrame->channel_layout,
                                             (AVSampleFormat) pFrame->format,
                                             pFrame->sample_rate,
                                             0, NULL);
    if (!swr_ctx || swr_init(swr_ctx) < 0) {
        LOGI("swr_init failed.");
        return -1;
    }
    LOGI("swr_init success.");
    // 计算转换后的sample个数 a * b / c
    int dst_nb_samples = av_rescale_rnd(
            swr_get_delay(swr_ctx, pFrame->sample_rate) + pFrame->nb_samples, pFrame->sample_rate,
            pFrame->sample_rate, AVRounding(1));
    LOGI("dst_nb_samples =%d %d", dst_nb_samples, pFrame->nb_samples);

    // 转换，返回值为转换后的sample个数
    int nb = swr_convert(swr_ctx, audioBuffer, dst_nb_samples, (const uint8_t **) pFrame->data,
                         pFrame->nb_samples);
    LOGI("nb =%d nb_samples=%d", nb, pFrame->nb_samples);
    dataSize = pFrame->channels * nb * av_get_bytes_per_sample(dst_format);

    return 1;
}













