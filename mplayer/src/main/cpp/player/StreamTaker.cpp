//
//  取流器
// Created by hwj on 2016/10/28.
//

#include "StreamTaker.h"
#include "log.h"
#include "RetCode.h"

#define  LOG_TAG    "StreamTaker"

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
}

StreamTaker::~StreamTaker() {
    stopTakeStream();
    if (pFormatCtx != NULL) {
        avformat_close_input(&pFormatCtx);
        pFormatCtx = NULL;
    }
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
        LOGI("Didn't find a video stream.");
        return FIND_VIDEO_STREAM_FAILED;
    } else {
        LOGI("Find a video stream success:%d", videoStream);
        // Get a pointer to the codec context for the video stream
        videoCodecID = pFormatCtx->streams[videoStream]->codecpar->codec_id;
        LOGI("The videoCodecID=%d", videoCodecID);
        videoFrameWidth= pFormatCtx->streams[videoStream]->codecpar->width;
        videoFrameHeight= pFormatCtx->streams[videoStream]->codecpar->height;
        LOGI("Find a video stream success:width=%d,height=%d",
             videoFrameWidth,
             videoFrameHeight);
    }

    if (audioStream == -1) {
        LOGI("Didn't find a audio stream.");
        // return FIND_VIDEO_STREAM_FAILED;
    } else {
        LOGI("Find a audio stream success:%d", audioStream);
        audioCodecID = pFormatCtx->streams[audioStream]->codecpar->codec_id;
        LOGI("The audioCodecID=%d", audioCodecID);
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
                videoCallback(handle, packet);
            }
        }
        if (packet.stream_index == audioStream && isTake) {
            if (audioCallback != NULL && isTake) {
                LOGI("取到音频流");
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




























