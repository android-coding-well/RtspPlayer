//
// Created by hwj on 2016/10/28.
//

#include "Codec.h"
#include "RetCode.h"
#include "log.h"
#define  LOG_TAG    "Codec"

Codec::Codec() {
    avcodec_register_all();
    isPrepareDecoderSuccess=false;
    isPrepareEncoderSuccess=false;
}

Codec::~Codec() {
    close();
}


AVFrame * Codec::decode(AVPacket  packet) {
    LOGI("enter function decode");
    if(!isPrepareDecoderSuccess){
        return NULL;
    }
    int ret=0;
    char msg[512];
    int len=512;
    if( ret=avcodec_send_packet(pCodecCtx,&packet)){
        av_strerror(ret,msg,len);
        LOGI("%d send packet failed:%d--%s",this,ret,msg);
        return NULL;
    }
    pFrame = av_frame_alloc();
    if(ret=avcodec_receive_frame(pCodecCtx,pFrame)){
        av_strerror(ret,msg,len);
        LOGI("%d receive frame failed:%d--%s",this,ret,msg);
        return NULL;
    };

    if (AVPixelFormat::AV_PIX_FMT_YUV420P == pFrame->format) {
        LOGI("%d the format of frame is YUV420P",this );
    } else {
        LOGI("%d the format of frame is %d(details in AVPixelFormat or AVSampleFormat)",this , pFrame->format);
    }
    return pFrame;
}

AVPacket* Codec::encode(AVFrame *  pFrame) {
    LOGI("enter function encode");
    if(!isPrepareEncoderSuccess){
        return NULL;
    }
    if(avcodec_send_frame(pCodecCtx,pFrame)){
        LOGI("send frame failed");
        return NULL;
    }
     packet=av_packet_alloc();
    if(avcodec_receive_packet(pCodecCtx,packet)){
        LOGI("receive packet failed");
        return NULL;
    };
    return packet;
}

int Codec::prepareDecode(AVCodecID codecID) {
    isPrepareDecoderSuccess=false;
    pCodec= avcodec_find_decoder(codecID);
    if (pCodec == NULL) {
        return FIND_CODEC_FAILED; // Codec not found
    }
    LOGI("%d find decoder：name=%s AVCodecID=%d",  this, pCodec->name,pCodec->id);
    pCodecCtx=avcodec_alloc_context3(pCodec);
    if (pCodecCtx == NULL) {
        LOGI("Alloc AVCodecContext failed");
        return ALLOC_AVCODECCONTEXT_FAILED; // Codec not found
    }
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGI("Could not open codec.");
        return OPEN_CODEC_FAILED; // Could not open codec
    }
    LOGI("%d open codec success",  this);
    isPrepareDecoderSuccess=true;

    return SUCCESS;
}

int Codec::prepareEncode(AVCodecID codecID) {
    isPrepareEncoderSuccess=false;
    pCodec= avcodec_find_encoder(codecID);
    if (pCodec == NULL) {
        return FIND_CODEC_FAILED; // Codec not found
    }
    LOGI("%d find decoder：name=%s AVCodecID=%d", this, pCodec->name,pCodec->id);
    pCodecCtx=avcodec_alloc_context3(pCodec);
    if (pCodecCtx == NULL) {
        LOGI("Alloc AVCodecContext failed");
        return ALLOC_AVCODECCONTEXT_FAILED; // Codec not found
    }
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGI("Could not open codec.");
        return OPEN_CODEC_FAILED; // Could not open codec
    }
    LOGI("%d open codec success",  this);
    isPrepareEncoderSuccess=true;

    return SUCCESS;
}

bool Codec::isPrepareDecodeSuccess() {
    return isPrepareDecoderSuccess;
}
bool Codec::isPrepareEncodeSuccess() {
    return isPrepareEncoderSuccess;
}

void Codec::close() {
    if (pFrame!=NULL) {
        av_frame_free(&pFrame);
        pFrame = NULL;
    }
    if (packet!=NULL) {
        av_packet_free(&packet);
        packet = NULL;
    }
    if (pCodecCtx!=NULL) {
        avcodec_free_context(&pCodecCtx);
        pCodecCtx=NULL;
    }
}

AVCodecContext *Codec::getAVCodecContext() {
    return pCodecCtx;
}


















