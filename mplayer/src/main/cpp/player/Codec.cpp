//
// Created by hwj on 2016/10/28.
//

#include "Codec.h"
#include "RetCode.h"
#include "log.h"

#define  LOG_TAG    "Codec"

Codec::Codec() {
    avcodec_register_all();
    pFrame = av_frame_alloc();
    packet = av_packet_alloc();

    sampleRate=DEFAULT_AUDIO_SAMPLE_RATE;
    channels=DEFAULT_AUDIO_CHANNELS;
    channelLayout=DEFAULT_AUDIO_CHANNEL_LAYOUT;
}

Codec::~Codec() {
    close();
}


AVFrame *Codec::decode(AVPacket packet) {
    // LOGI("enter function decode");
    if (!isPrepareDecoderSuccess) {
        return NULL;
    }
    int ret = 0;
    char msg[32];
    int len = 32;
    LOGI("%d send packet :%d--%d", this, packet.data, packet.size);

    if (ret = avcodec_send_packet(pCodecCtx, &packet)) {
        av_strerror(ret, msg, len);
        LOGE("%d send packet failed:%d--%s--%d", this, ret, msg, packet.size);
        return NULL;
    }
    int i = 0;
    const int TRY_COUNT = 3;
    while (ret = avcodec_receive_frame(pCodecCtx, pFrame) != 0) {
        i++;
        av_strerror(ret, msg, len);
        LOGE("%d receive frame failed:%d--%s", this, ret, msg);
        if (ret != -11) {//直接认为失败，丢弃
            return NULL;
        } else if (i > TRY_COUNT) {//第一次收到-11后如果连续TRY_COUNT次还是-11，则直接丢弃
            return NULL;
        }
    }

    /*if (AVPixelFormat::AV_PIX_FMT_YUV420P == pFrame->format) {
        LOGI("%d the format of frame is YUV420P",this );
    } else {
        LOGI("%d the format of frame is %d(details in AVPixelFormat or AVSampleFormat)",this , pFrame->format);
    }*/
    //LOGI("帧类型=%d",pFrame->pict_type);
    //LOGI("(AVPixelFormat or AVSampleFormat)=%d",pFrame->format);
    return pFrame;
}

AVFrame *Codec::decode(unsigned char *data, int dataSize) {
    if (data == NULL || dataSize <= 0) {
        return NULL;
    }
    AVPacket packet;
    packet.data = data;
    packet.size = dataSize;
    av_init_packet(&packet);
    return decode(packet);
}

AVPacket *Codec::encode(AVFrame *pFrame) {
    // LOGI("enter function encode");
    if (!isPrepareEncoderSuccess) {
        return NULL;
    }
    if (avcodec_send_frame(pCodecCtx, pFrame)) {
        LOGE("send frame failed");
        return NULL;
    }
    if (avcodec_receive_packet(pCodecCtx, packet)) {
        LOGE("receive packet failed");
        return NULL;
    };
    return packet;
}

int Codec::prepareDecode(AVCodecID codecID) {
    isPrepareDecoderSuccess = false;

    pCodec = avcodec_find_decoder(codecID);
    if (pCodec == NULL) {
        return FIND_CODEC_FAILED; // Codec not found
    }
    LOGI("%d find decoder：name=%s AVCodecID=%d", this, pCodec->name, pCodec->id);
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == NULL) {
        LOGE("Alloc AVCodecContext failed");
        return ALLOC_AVCODECCONTEXT_FAILED; // Codec not found
    }

    LOGI("%d pCodecCtx params before:sample_rate=%d, channels=%d, bit_rate=%d,channel_layout=%d", this, pCodecCtx->sample_rate,
         pCodecCtx->channels, pCodecCtx->bit_rate,pCodecCtx->channel_layout);

    if (codecID > 0x15000) {//表示是音频类型,需要设置相关参数，否则可能解码不了
        LOGI("音频解码,必须设置sample_rate和channels");
        pCodecCtx->sample_rate =sampleRate;
        LOGI("channelLayout=%d",channelLayout);
        //发现 channel_layout即使设置了也无效，仍然会被改为0
        pCodecCtx->channel_layout = channelLayout;
        pCodecCtx->channels =channels;

    }
    LOGI("%d pCodecCtx params after:sample_rate=%d,channels=%d,bit_rate=%d,channel_layout=%d", this, pCodecCtx->sample_rate,
         pCodecCtx->channels, pCodecCtx->bit_rate,pCodecCtx->channel_layout);

    //pCodecCtx->pix_fmt=AV_PIX_FMT_YUV420P;
    //开启多线程（根据cpu核数）
    pCodecCtx->thread_count = av_cpu_count();
    LOGI("the thread count=%d", pCodecCtx->thread_count);

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Could not open codec.");
        return OPEN_CODEC_FAILED; // Could not open codec
    }
    LOGI("%d open codec success", this);
    isPrepareDecoderSuccess = true;

    return SUCCESS;
}

int Codec::prepareEncode(AVCodecID codecID) {
    isPrepareEncoderSuccess = false;
    pCodec = avcodec_find_encoder(codecID);
    if (pCodec == NULL) {
        return FIND_CODEC_FAILED; // Codec not found
    }
    LOGI("%d find encoder：name=%s AVCodecID=%d", this, pCodec->name, pCodec->id);
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == NULL) {
        LOGE("Alloc AVCodecContext failed");
        return ALLOC_AVCODECCONTEXT_FAILED; // Codec not found
    }
    //开启多线程（根据cpu核数）
    pCodecCtx->thread_count = av_cpu_count();
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Could not open codec.");
        return OPEN_CODEC_FAILED; // Could not open codec
    }
    LOGI("%d open codec success", this);
    isPrepareEncoderSuccess = true;

    return SUCCESS;
}

bool Codec::isPrepareDecodeSuccess() {
    return isPrepareDecoderSuccess;
}

bool Codec::isPrepareEncodeSuccess() {
    return isPrepareEncoderSuccess;
}

void Codec::close() {
    if (pFrame != NULL) {
        av_frame_free(&pFrame);
        pFrame = NULL;
    }
    if (packet != NULL) {
        av_packet_free(&packet);
        packet = NULL;
    }
    if (pCodecCtx != NULL) {
        avcodec_close(pCodecCtx);
        avcodec_free_context(&pCodecCtx);
        pCodecCtx = NULL;
    }
}

AVCodecContext *Codec::getAVCodecContext() {
    return pCodecCtx;
}

void Codec::reset() {
    isPrepareDecoderSuccess = false;
    isPrepareEncoderSuccess = false;
     sampleRate=DEFAULT_AUDIO_SAMPLE_RATE;
     channels=DEFAULT_AUDIO_CHANNELS;
     channelLayout=DEFAULT_AUDIO_CHANNEL_LAYOUT;
}


void Codec::setAudioCodecParameters(int channels,int channelLayout, int sampleRate) {
    LOGI("receive channels=%d,channelLayout=%d,sampleRate=%d",channels, channelLayout, sampleRate);
    if(channels==0){
        //通道数可以根据通道设计channellayout获得
        channels=av_get_channel_layout_nb_channels(channelLayout);
    }
    this->channels=channels<=0?DEFAULT_AUDIO_CHANNELS:channels;
    this->channelLayout=channelLayout<=0?DEFAULT_AUDIO_CHANNEL_LAYOUT:channelLayout;
    this->sampleRate=sampleRate<=0?DEFAULT_AUDIO_SAMPLE_RATE:sampleRate;
    LOGI("channels=%d,channelLayout=%d,sampleRate=%d", this->channels, this->channelLayout, this->sampleRate);

}


























