//
//  取流器
// Created by hwj on 2016/10/28.
//

#include "StreamTaker.h"
#include "log.h"
#include "RetCode.h"

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "StreamTaker"
#endif
/**
 * 取流线程
 */
 void *takingStreamThread(void *userData) {
    StreamTaker *taker = (StreamTaker *) userData;
    taker->takingStream();
    pthread_exit(0);
}

StreamTaker::StreamTaker() {
    av_register_all();
    avformat_network_init();
    //线程初始化
    pthread_attr_init(&attr); /*初始化,得到默认的属性值*/

    codecParameters=new CodecParameters();
}

StreamTaker::~StreamTaker() {
    stopTakeStream();
    if (pFormatCtx != NULL) {
        avformat_close_input(&pFormatCtx);
        pFormatCtx = NULL;
    }
    avformat_network_deinit();
}

void StreamTaker::setVideoPacketCallback(void *handle, PacketCallback callback) {
    this->videoCallback = callback;
    this->handle = handle;
}

void StreamTaker::setAudioPacketCallback(void *handle, PacketCallback callback) {
    this->audioCallback = callback;
    this->handle = handle;
}

int StreamTaker::prepare(const char *url) {
   // LOGI("prepare" );
    isPrepareSuccess = false;
    hasReceiveVideoPacketCount=0;
    hasReceiveAudioPacketCount=0;
    if (url == NULL || strcmp(url, "") == 0) {
        return PARAMS_ERROR;
    }
    pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, url, NULL, NULL) != 0) {
        LOGI("Couldn't open file:%s\n", url);
        return OPEN_FILE_FAILED; // Couldn't open file
    }
    LOGI("%d success open file:%s\n", this, url);

    // Retrieve stream information,it take a long time
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGI("Couldn't find stream information.");
        return FIND_STREAM_INFORMATION_FAILED;
    }
    LOGI("%d success find stream information", this);

    LOGI("finding a video stream...");
    // Find the first video stream
    videoStream = -1;
    audioStream = -1;
    int i;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO
            && videoStream < 0) {
            videoStream = i;
        }
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO
            && audioStream < 0) {
            audioStream = i;
        }
        if (videoStream != -1 && audioStream != -1) {
            break;
        }

    }
    if (videoStream == -1) {
        LOGE("Didn't find a video stream.");
       // return FIND_VIDEO_STREAM_FAILED;
    } else {
        LOGI("Find a video stream success:%d", videoStream);

       videoCodecParameters=pFormatCtx->streams[videoStream]->codecpar;
        // Get a pointer to the codec context for the video stream
        videoCodecID = videoCodecParameters->codec_id;
        LOGI("The videoCodecID=%d", videoCodecID);
        videoFrameWidth= videoCodecParameters->width;
        videoFrameHeight= videoCodecParameters->height;
        LOGI("Find a video stream success:width=%d,height=%d",
             videoFrameWidth,
             videoFrameHeight);
        codecParameters->videoCodecId=videoCodecID;
        codecParameters->width=videoFrameWidth;
        codecParameters->height=videoFrameHeight;
    }

    if (audioStream == -1) {
        LOGE("Didn't find a audio stream.");
         //return FIND_VIDEO_STREAM_FAILED;
    } else {
        LOGI("Find a audio stream success:%d", audioStream);
         audioCodecParameters=pFormatCtx->streams[audioStream]->codecpar;
        audioCodecID =audioCodecParameters->codec_id;
        LOGI("The audioCodecID=%d", audioCodecID);
        LOGI("The audioBitRate=%d", audioCodecParameters->bit_rate);
        codecParameters->audioCodecId=audioCodecID;
        codecParameters->channels=audioCodecParameters->channels;
        codecParameters->channelLayout=audioCodecParameters->channel_layout;
        codecParameters->sampleRate=audioCodecParameters->sample_rate;

       /* //此处获取的channels一直为0，无法使用
        LOGI("codec_type=%d,sample_rate=%d,bit_rate=%d,channels=%d,channel_layout=%d",audioCodecParameters->codec_type,
             audioCodecParameters->sample_rate,audioCodecParameters->bit_rate,audioCodecParameters->channels,audioCodecParameters->channel_layout);*/
        LOGI("sample_rate=%d,channels=%d,channel_layout=%d", codecParameters->sampleRate,codecParameters->channels,codecParameters->channelLayout);

    }

    //
    if(videoStream==-1&&audioStream==-1){
        LOGE("prepare failed!")
        return FIND_STREAM_INFORMATION_FAILED;
    }

    isPrepareSuccess = true;
    return SUCCESS;
}

void StreamTaker::startTakeStream() {
    if (!isPrepareSuccess) {
        return;
    }
    isTake = true;
    //开启取流线程
    pthread_create(&tid, &attr, takingStreamThread, this);
}

void StreamTaker::stopTakeStream() {
    isTake = false;
}

AVCodecID StreamTaker::getVideoCodeID() {
    if (isPrepareSuccess) {
        return videoCodecID;
    }
    return AV_CODEC_ID_NONE;

}

AVCodecID StreamTaker::getAudioCodeID() {
    if (isPrepareSuccess) {
        return audioCodecID;
    }
    return AV_CODEC_ID_NONE;

}

void StreamTaker::takingStream() {
    AVPacket packet;
    av_init_packet(&packet);
    while (av_read_frame(pFormatCtx, &packet) >= 0 && isTake) {
        // LOGI("取到的流格式:%d",packet.stream_index);
        // Is this a packet from the video stream?
        if (packet.stream_index == videoStream && isTake) {
            if (videoCallback != NULL && isTake) {
                LOGI("取到视频流");
                hasReceiveVideoPacketCount++;
                videoCallback(handle, packet);
            }
        }
        if (packet.stream_index == audioStream && isTake) {
            if (audioCallback != NULL && isTake) {
                LOGI("取到音频流");
                hasReceiveAudioPacketCount++;
                audioCallback(handle, packet);
            }
        }
        av_packet_unref(&packet);
    }
    LOGI("takingStream exit ");
}

int StreamTaker::getFrameWidth() {
    return videoFrameWidth;
}

int StreamTaker::getFrameHeight() {
    return videoFrameHeight;
}

int StreamTaker::getReceiveVideoPacketCount() {
    return hasReceiveVideoPacketCount;
}

int StreamTaker::getReceiveAudioPacketCount() {
    return hasReceiveAudioPacketCount;
}

CodecParameters * StreamTaker::getCodecParameters() {
    return codecParameters;
}

AVCodecParameters *StreamTaker::getAudioCodecParameters() {
    return audioCodecParameters;
}

AVCodecParameters *StreamTaker::getVideoCodecParameters() {
    return videoCodecParameters;
}




























