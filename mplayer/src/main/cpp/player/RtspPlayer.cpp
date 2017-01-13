#include <unistd.h>
#include "RtspPlayer.h"
#include "RetCode.h"

#define  LOG_TAG    "RtspPlayer"

/**
 * 渲染线程
 */
void *renderThread(void *userData) {
    RtspPlayer *player = (RtspPlayer *) userData;
    player->render();
    pthread_exit(0);
}

/**
 * 解码视频帧线程
 */
void *decodeThread(void *userData) {
    RtspPlayer *player = (RtspPlayer *) userData;
    player->doDecodeWork();
    pthread_exit(0);
}

/**
 * 播放音频线程
 */
void *playAudioThread(void *userData) {
    RtspPlayer *player = (RtspPlayer *) userData;
    player->doPlayAudioWork();
    pthread_exit(0);
}


/*
 * 每当缓冲音频播放完毕后会回调此函数（可在此函数里实现更新缓冲音频）
 */
void audioPlayerCallback3(SLAndroidSimpleBufferQueueItf caller,
                          void *pContext) {
    LOGI("audioPlayerCallback3");
    RtspPlayer *player = (RtspPlayer *) pContext;
    player->playAudioCount++;
}


RtspPlayer::RtspPlayer() {
    //线程初始化
    pthread_attr_init(&attr); /*初始化,得到默认的属性值*/
    pthread_attr_init(&attr2); /*初始化,得到默认的属性值*/
    pthread_attr_init(&attr3); /*初始化,得到默认的属性值*/

    audioPlayer.createEngine();
    audioPlayer.setCallback(audioPlayerCallback3, this);
}

RtspPlayer::~RtspPlayer() {
    pthread_attr_destroy(&attr);
    pthread_attr_destroy(&attr2);
    pthread_attr_destroy(&attr3);
    //pthread_exit(0);
    close();
}

bool RtspPlayer::addHasDecodedVideoData(AVFrame *frame) {
    if (frame != NULL) {
        // LOGI("%d addDecodeVideoData:width=%d,height=%d", this,frame->width, frame->height);
        if (this->frameWidth == 0 || this->frameHeight == 0) {
            setVideoFrameSize(frame->width, frame->height);
        }
        videoDecodeQueue.push_back(frame);
        decodeVideoFrameCount++;
        return true;
    }
    decodeFailedVideoFrameCount++;
    return false;
}

bool RtspPlayer::addUndecodeVideoData(unsigned char *data, int dataSize) {
    if (data != NULL && dataSize > 0) {
        AVPacket packet;
        //  av_init_packet(&packet);
        unsigned char *temp = (unsigned char *) malloc(dataSize);
        memcpy(temp, (const void *) data, dataSize);
        packet.data = temp;
        packet.size = dataSize;
        videoUndecodeQueue.push_back(packet);
        LOGI("videoUndecodeQueue=%d", videoUndecodeQueue.size());
        receiveVideoPacketCount++;

        return true;
    }
    return false;
}

void RtspPlayer::setANativeWindow(ANativeWindow *nativeWindow) {
    this->nativeWindow = nativeWindow;
}


ANativeWindow *RtspPlayer::getANativeWindow() {
    return this->nativeWindow;
}

void RtspPlayer::setVideoDecodecID(AVCodecID codecID) {
    if (codecID != AV_CODEC_ID_NONE) {
        this->videoCodecID = codecID;
        videoCodec.prepareDecode(codecID);
    }
}

void RtspPlayer::setVideoFrameSize(int frameWidth, int frameHeight) {
    if (frameWidth > 0 && frameHeight > 0) {
        this->frameWidth = frameWidth;
        this->frameHeight = frameHeight;
        rgbRenderer.prepare(nativeWindow, frameWidth, frameHeight);
    }
}

/**
 * 在开始播放前要确保已经设置了ANativeWindow，解码类型，原始视频大小，解码后的数据
 */
void RtspPlayer::startPlay() {
    LOGI("startPlay");
    isPlaying = true;
    //开启渲染线程
    pthread_create(&tid, &attr, renderThread, this);
    pthread_create(&tid2, &attr2, decodeThread, this);

}

void RtspPlayer::stopPlay() {
    isSoundOn = false;
    isPlaying = false;
    isCapture = false;

}
void RtspPlayer::stopPlay(boolean keepSoundStatus) {
    if(!keepSoundStatus){
        isSoundOn = false;
    }
    isPlaying = false;
    isCapture = false;

}

void RtspPlayer::doDecodeWork() {
    while (isPlaying) {
        if (videoUndecodeQueue.size() != 0) {
            AVPacket avPacket = videoUndecodeQueue.front();//get first without deleting
            videoUndecodeQueue.pop_front();//delete first
            LOGI("doDecodeWork:%d--%d", avPacket.data, avPacket.size);
            addHasDecodedVideoData(videoCodec.decode(avPacket.data, avPacket.size));
            if (avPacket.data != NULL) {
                free(avPacket.data);
                avPacket.data = NULL;
            }

        } else {
            //休眠一段时间,避免空转引发CPU100%被占用
            usleep(10000);
        }
    }
    videoUndecodeQueue.clear();
    LOGI("doDecodeWork exit ");
}

void RtspPlayer::doPlayAudioWork() {
    while (isSoundOn) {
        if (audioDecodeQueue.size() > 0) {
            AVFrame *pFrame = audioDecodeQueue.front();
            audioDecodeQueue.pop_front();
            playingAudio(pFrame);
        } else {
            //休眠一段时间,避免空转引发CPU100%被占用
            usleep(10000);
        }
    }
    LOGI("audio exit");
}

void RtspPlayer::render() {
    while (isPlaying) {
        if (videoDecodeQueue.size() != 0) {
            AVFrame *pFrame = videoDecodeQueue.front();//get first without deleting
            videoDecodeQueue.pop_front();//delete first
            if (pFrame == NULL) {
                LOGI("the decode frame is null");
                continue;
            }
            if (pFrame->width > 0 && pFrame->height > 0 && frameWidth == 0 && frameHeight == 0) {
                this->frameWidth = pFrame->width;
                this->frameHeight = pFrame->height;
            }
            if (isCapture) {
                saveFrame = pFrame;
            }
            if (!convertor.isPreparedYUV2RGBSuccess()) {
                LOGI("PreparedYUV2RGB failed,trying again:frameWidth=%d--frameHeight=%d--pix_fmt=%d",
                     frameWidth, frameHeight, videoCodec.getAVCodecContext()->pix_fmt);
                convertor.prepareYUV2RGB(frameWidth, frameHeight,
                                         videoCodec.getAVCodecContext()->pix_fmt);
            }
            // LOGI("isPreparedYUV2RGBSuccess");
            if (!rgbRenderer.isPreparedSuccess()) {
                LOGI("RGBRenderer prepare failed,trying again:frameWidth=%d--frameHeight=%d",
                     frameWidth, frameHeight);
                rgbRenderer.prepare(getANativeWindow(), frameWidth,
                                    frameHeight);
            }
            //LOGI("isPreparedSuccess");
            if (isPlaying) {
                //LOGI("render");
                if (rgbRenderer.render(convertor.YUV2RGB(pFrame)) == 1) {
                    renderFrameCount++;
                };
            }
        } else {
            //休眠一段时间,避免空转引发CPU100%被占用
            usleep(10000);
        }
    }
    rgbRenderer.clearScreen();
    videoDecodeQueue.clear();
    LOGI("render exit ");
}

void RtspPlayer::close() {
    stopPlay();

    if (saveFrame != NULL) {
        av_free(saveFrame);
        saveFrame = NULL;
    }
}

void RtspPlayer::clearVideoDecodeQueue() {
    videoDecodeQueue.clear();
    videoUndecodeQueue.clear();
}

int RtspPlayer::capture(const char *savePath) {
    if (!isPlaying) {
        LOGI("视频未播放，无法抓拍");
        return CAPTURE_FAILED_WITHOUT_PLAYING;
    }
    LOGI("%d 抓拍开始...", this);
    isCapture = true;
    saveFrame = NULL;
    clock_t start, ends;//1秒clock1000000,相当于1ms=1000
    start = clock();
    while (isCapture) {
        ends = clock();
        if (saveFrame != NULL) {
            isCapture = false;
            break;
        }
        if (ends - start > 100 * 1000) {//100ms后退出，保证不会一直阻塞在这里
            LOGI("超时获取不到帧，无法抓拍");
            break;
        }
    }
    isCapture = false;
    if (saveFrame == NULL) {
        return FAILED;
    }
    int ret = jpeg.saveYuv2Jpeg(savePath, saveFrame);
    LOGI("%d 保存抓拍图片结果：返回码=%d 路径=%s", this, ret, savePath);
    if (ret != 1) {
        return CAPTURE_ERROR_IN_FILE;
    }
    return SUCCESS;

}

void RtspPlayer::soundOn() {
    isSoundOn = true;
    resetAudioCount();
    pthread_create(&tid3, &attr3, playAudioThread, this);

}

void RtspPlayer::soundOff() {
    isSoundOn = false;
    //关闭声音时清除音频队列
    audioDecodeQueue.clear();
}

bool RtspPlayer::isVideoPlaying() {
    return isPlaying;
}

bool RtspPlayer::isAudioSoundOn() {
    return isSoundOn;
}

void RtspPlayer::setAudioDecodecID(AVCodecID codecID) {
    if (codecID != AV_CODEC_ID_NONE) {
        this->audioCodecID = codecID;
        // LOGI("setAudioDecodecID=%d",codecID);
        audioCodec.prepareDecode(codecID);
    }
}

bool RtspPlayer::addHasDecodedAudioData(AVFrame *frame) {
    //LOGI("addHasDecodedAudioData:%d",frame);
    if (isSoundOn) {
        if(frame != NULL){
            decodeAudioFrameCount++;
            audioDecodeQueue.push_back(frame);
            return true;
        }else{
            decodeFailedAudioFrameCount++;
            return false;
        }
    }
    return false;
}

bool RtspPlayer::addUndecodeAudioData(AVPacket packet) {
    if (isSoundOn&&&packet != NULL && audioCodec.isPrepareDecodeSuccess()) {
        receiveAudioPacketCount++;
        addHasDecodedAudioData(audioCodec.decode(packet));
        return true;
    }

    return false;
}

bool RtspPlayer::addUndecodeAudioData(unsigned char *data, int dataSize) {
    if (data != NULL && dataSize > 0) {

        addHasDecodedAudioData(audioCodec.decode(data, dataSize));
        receiveAudioPacketCount++;

        return true;
    }
    return false;
}

void RtspPlayer::playingAudio(AVFrame *pFrame) {
    if (pFrame == NULL) {
        LOGI("audio frame is null");
        return;
    }
    const int MAX_AUDIO_FRAME_SIZE = 8196;//8196 192000
    int data_size = 0;
    uint8_t *audio_buff;
    int audio_buffer_size = av_samples_get_buffer_size(&data_size,
                                                       audioCodec.getAVCodecContext()->channels,
                                                       audioCodec.getAVCodecContext()->frame_size,
                                                       audioCodec.getAVCodecContext()->sample_fmt,
                                                       1);
    LOGI("计算得到音频输出buffer大小=%d", audio_buffer_size);
    if (audio_buffer_size > 0) {
        audio_buff = (uint8_t *) malloc(sizeof(uint8_t) * audio_buffer_size);
    } else {
        audio_buff = (uint8_t *) malloc(sizeof(uint8_t) * MAX_AUDIO_FRAME_SIZE);//8196
    }

    if (!audioPlayer.isCreateAudioPlayerSuccess()) {
        LOGI("audioPlayer create failed,trying again...");
        audioPlayer.createBufferQueueAudioPlayer(pFrame->sample_rate, pFrame->channels);
    }

    if (audioPlayer.convertToS16(pFrame, &audio_buff, data_size) == 1) {
        LOGI("audioPlayer enqueue");
        audioPlayer.enqueue(audio_buff, data_size);

    } else {
        LOGI("audioPlayer enqueue failed");
    }
    // 释放
    av_free(audio_buff);
}

void RtspPlayer::clearAudioDecodeQueue() {
    audioDecodeQueue.clear();
}

boolean RtspPlayer::isPrepareVideoDecoderSuccess() {
    return videoCodec.isPrepareDecodeSuccess();
}

boolean RtspPlayer::isPrepareAudioDecoderSuccess() {
    return audioCodec.isPrepareDecodeSuccess();
}

boolean RtspPlayer::isPrepareRGBRenderSuccess() {
    return rgbRenderer.isPreparedSuccess();
}

void RtspPlayer::resetVideoCount() {
    //收到的音视频
    receiveVideoPacketCount = 0;
    //解码的帧数
    decodeVideoFrameCount = 0;

    //解码失败
    decodeFailedVideoFrameCount = 0;

    //渲染的帧数
    renderFrameCount = 0;
}

void RtspPlayer::resetAudioCount() {
    //收到的音视频
    receiveAudioPacketCount = 0;
    //解码的帧数
    decodeAudioFrameCount = 0;

    //解码失败
    decodeFailedAudioFrameCount = 0;

    //渲染的帧数
    playAudioCount = 0;
}

void RtspPlayer::reset() {
    resetVideoCount();
    resetAudioCount();
    videoCodec.reset();
    audioCodec.reset();
    rgbRenderer.reset();
    frameWidth = 0;
    frameHeight = 0;
}



























































