#include <unistd.h>
#include "RGBHandler.h"
#include "RetCode.h"

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "RGBHandler"
#endif

/**
 * 渲染线程
 */
void *renderThread(void *userData) {
    RGBHandler *player = (RGBHandler *) userData;
    player->doRenderWork();
    pthread_exit(0);
}

/**
 * 解码视频帧线程
 */
void *decodeThread(void *userData) {
    RGBHandler *player = (RGBHandler *) userData;
    player->doDecodeWork();
    pthread_exit(0);
}

/**
 * 播放音频线程
 */
void *playAudioThread(void *userData) {
    RGBHandler *player = (RGBHandler *) userData;
    player->doPlayAudioWork();
    pthread_exit(0);
}


/*
 * 每当缓冲音频播放完毕后会回调此函数（可在此函数里实现更新缓冲音频）
 */
void audioPlayerCallback3(SLAndroidSimpleBufferQueueItf caller,
                          void *pContext) {
    LOGI("audioPlayerCallback3");
    RGBHandler *player = (RGBHandler *) pContext;
    player->playAudioCount++;
}


RGBHandler::RGBHandler() {
    //线程初始化
    pthread_attr_init(&renderAttr); /*初始化,得到默认的属性值*/
    pthread_attr_init(&decodeAttr); /*初始化,得到默认的属性值*/
    pthread_attr_init(&audioAttr); /*初始化,得到默认的属性值*/

    audioPlayer.createEngine();
    audioPlayer.setCallback(audioPlayerCallback3, this);
}

RGBHandler::~RGBHandler() {
    pthread_attr_destroy(&renderAttr);
    pthread_attr_destroy(&decodeAttr);
    pthread_attr_destroy(&audioAttr);
    //pthread_exit(0);
    close();
}

bool RGBHandler::addHasDecodedVideoData(AVFrame *frame) {
    if (frame != NULL) {
        // LOGI("%d addDecodeVideoData:width=%d,height=%d", this,frame->width, frame->height);
        if (this->frameWidth == 0 || this->frameHeight == 0) {
            setVideoFrameSize(frame->width, frame->height);
        }
        videoDecodeQueue.push_back(frame);
        decodeVideoFrameCount++;
        if(!rendering&&decodeVideoFrameCount>=optimize.getCacheFrameCount()){
            rendering=true;
            startRenderVideoThread();
        }

        return true;
    }
    decodeFailedVideoFrameCount++;
    return false;
}

bool RGBHandler::addUndecodeVideoData(unsigned char *data, int dataSize) {
    if (data != NULL && dataSize > 0) {
        optimize.receiveFrame();
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

void RGBHandler::setANativeWindow(ANativeWindow *nativeWindow) {
    this->nativeWindow = nativeWindow;
}


ANativeWindow *RGBHandler::getANativeWindow() {
    return this->nativeWindow;
}

void RGBHandler::setVideoDecodecID(AVCodecID codecID) {
    if (codecID != AV_CODEC_ID_NONE) {
        this->videoCodecID = codecID;
        videoCodec.prepareDecode(codecID);
    }
}

void RGBHandler::setVideoFrameSize(int frameWidth, int frameHeight) {
    if (frameWidth > 0 && frameHeight > 0) {
        this->frameWidth = frameWidth;
        this->frameHeight = frameHeight;
        rgbRenderer.prepareRGBA(nativeWindow, frameWidth, frameHeight);
    }
}

/**
 * 在开始播放前要确保已经设置了ANativeWindow，解码类型，原始视频大小，解码后的数据
 */
void RGBHandler::startPlay() {
    LOGI("startPlay");
    isPlaying = true;
    rendering=false;
    optimize.reset();
    //开启解码线程
    startDecodeVideoThread();

}

void RGBHandler::startDecodeVideoThread(){
    //开启解码线程
    pthread_create(&decodeTid, &decodeAttr, decodeThread, this);
}

void RGBHandler::startRenderVideoThread(){
    //开启渲染线程
    pthread_create(&renderTid, &renderAttr, renderThread, this);
}

bool RGBHandler::isRendering(){
    return rendering;
}


void RGBHandler::stopPlay() {
    isSoundOn = false;
    isPlaying = false;
    isCapture = false;

}
void RGBHandler::stopPlay(bool keepSoundStatus) {
    if(!keepSoundStatus){
        isSoundOn = false;
    }
    isPlaying = false;
    isCapture = false;

}

void RGBHandler::doDecodeWork() {
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
            usleep(5000);
        }
    }
    videoUndecodeQueue.clear();
    LOGI("doDecodeWork exit ");
}

void RGBHandler::doPlayAudioWork() {
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
void RGBHandler::doRenderWork() {
    while (isPlaying) {
        if (videoDecodeQueue.size() >0) {
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
            if (!convertor.isPreparedYUVToRGBSuccess()) {
                LOGI("PreparedYUV2RGB failed,trying again:frameWidth=%d--frameHeight=%d--pix_fmt=%d",
                     frameWidth, frameHeight, videoCodec.getAVCodecContext()->pix_fmt);
                convertor.prepareYUVToRGBA(frameWidth, frameHeight,
                                         videoCodec.getAVCodecContext()->pix_fmt);
            }
            // LOGI("isPreparedYUV2RGBSuccess");
            if (!rgbRenderer.isPreparedSuccess()) {
                LOGI("RGBRenderer prepare failed,trying again:frameWidth=%d--frameHeight=%d",
                     frameWidth, frameHeight);
                rgbRenderer.prepareRGBA(getANativeWindow(), frameWidth,
                                    frameHeight);
            }
            //LOGI("isPreparedSuccess");
            if (isPlaying) {
                //LOGI("render");
                long pre=clock();
                AVFrame * frame=convertor.YUV2RGB(pFrame);
                LOGI("YUV2RGB time=%d us",(clock()-pre));
                if (rgbRenderer.render(frame) == 1) {
                    int renderFrameTime=(clock()-pre);
                    //LOGI("render time=%d",(clock()-pre));
                    renderFrameCount++;
                    usleep(optimize.getRenderWaitTime(videoDecodeQueue.size(),renderFrameTime));
                };
            }
        } else {
            //休眠一段时间,避免空转引发CPU100%被占用
            usleep(5000);
        }
    }
    rgbRenderer.clearScreen();
    videoDecodeQueue.clear();
    LOGI("render exit ");
}

void RGBHandler::close() {
    stopPlay();
    if (saveFrame != NULL) {
        av_free(saveFrame);
        saveFrame = NULL;
    }
}

void RGBHandler::clearVideoQueue() {
    videoDecodeQueue.clear();
    videoUndecodeQueue.clear();
}

int RGBHandler::capture(const char *savePath) {
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

void RGBHandler::soundOn() {
    isSoundOn = true;
    resetAudioCount();
    pthread_create(&audioTid, &audioAttr, playAudioThread, this);

}

void RGBHandler::soundOff() {
    isSoundOn = false;
    //关闭声音时清除音频队列
    audioDecodeQueue.clear();
}

bool RGBHandler::isVideoPlaying() {
    return isPlaying;
}

bool RGBHandler::isAudioSoundOn() {
    return isSoundOn;
}

void RGBHandler::setAudioDecodecID(AVCodecID codecID) {
    if (codecID != AV_CODEC_ID_NONE) {
        this->audioCodecID = codecID;
         LOGI("setAudioDecodecID=%d",codecID);
        audioCodec.prepareDecode(codecID);
    }
}

bool RGBHandler::addHasDecodedAudioData(AVFrame *frame) {
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

bool RGBHandler::addUndecodeAudioData(AVPacket packet) {

    if (isSoundOn&&&packet != NULL && audioCodec.isPrepareDecodeSuccess()) {
        receiveAudioPacketCount++;
        addHasDecodedAudioData(audioCodec.decode(packet));
        return true;
    }

    return false;
}

bool RGBHandler::addUndecodeAudioData(unsigned char *data, int dataSize) {

    if (isSoundOn && audioCodec.isPrepareDecodeSuccess()&&data != NULL && dataSize > 0) {
        AVPacket packet;
        //av_init_packet(&packet);
        unsigned char *temp = (unsigned char *) malloc(dataSize);
        memcpy(temp, (const void *) data, dataSize);
        packet.data = temp;
        packet.size = dataSize;
        addHasDecodedAudioData(audioCodec.decode(packet));
        receiveAudioPacketCount++;

        return true;
    }
    return false;
}

void RGBHandler::playingAudio(AVFrame *pFrame) {
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

void RGBHandler::clearAudioDecodeQueue() {
    audioDecodeQueue.clear();
}

bool RGBHandler::isPrepareVideoDecoderSuccess() {
    return videoCodec.isPrepareDecodeSuccess();
}

bool RGBHandler::isPrepareAudioDecoderSuccess() {
    return audioCodec.isPrepareDecodeSuccess();
}

bool RGBHandler::isPrepareRGBRenderSuccess() {
    return rgbRenderer.isPreparedSuccess();
}

void RGBHandler::resetVideoCount() {
    //收到的音视频
    receiveVideoPacketCount = 0;
    //解码的帧数
    decodeVideoFrameCount = 0;

    //解码失败
    decodeFailedVideoFrameCount = 0;

    //渲染的帧数
    renderFrameCount = 0;
}

void RGBHandler::resetAudioCount() {
    //收到的音视频
    receiveAudioPacketCount = 0;
    //解码的帧数
    decodeAudioFrameCount = 0;

    //解码失败
    decodeFailedAudioFrameCount = 0;

    //渲染的帧数
    playAudioCount = 0;
}

void RGBHandler::reset() {
    resetVideoCount();
    resetAudioCount();
    videoCodec.reset();
    audioCodec.reset();
    rgbRenderer.reset();
    frameWidth = 0;
    frameHeight = 0;
}

void RGBHandler::setAudioDecodecParameters(int channels,int channelLayout, int sampleRate) {
    audioCodec.setAudioCodecParameters(channels,channelLayout,sampleRate);
}
