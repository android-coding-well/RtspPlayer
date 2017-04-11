//
// Created by hwj on 2017/2/6.
//

#include <unistd.h>
#include "YUVHandler.h"
#include "RetCode.h"

#define  LOG_TAG    "YUVHandler"

YUVHandler::YUVHandler() {
    //线程初始化
    pthread_attr_init(&renderAttr); /*初始化,得到默认的属性值*/
    pthread_attr_init(&decodeAttr); /*初始化,得到默认的属性值*/
    pthread_attr_init(&audioAttr); /*初始化,得到默认的属性值*/
}

YUVHandler::~YUVHandler() {
    pthread_attr_destroy(&renderAttr);
    pthread_attr_destroy(&decodeAttr);
    pthread_attr_destroy(&audioAttr);

}

/**
 * 解码视频帧线程
 */
void *decodeThread2(void *userData) {
    YUVHandler *player = (YUVHandler *) userData;
    player->doDecodeWork();
    pthread_exit(0);
}


bool YUVHandler::isPrepareVideoDecoderSuccess() {
    return videoCodec.isPrepareDecodeSuccess();
}

void YUVHandler::setVideoDecodecID(AVCodecID codecID) {
    if (codecID != AV_CODEC_ID_NONE) {
        this->videoCodecID = codecID;
        videoCodec.prepareDecode(codecID);
    }
}

bool YUVHandler::addHasDecodedVideoData(AVFrame *frame) {
    if (frame != NULL) {
        // LOGI("%d addDecodeVideoData:width=%d,height=%d", this,frame->width, frame->height);
        /* if (this->frameWidth == 0 || this->frameHeight == 0) {
             setVideoFrameSize(frame->width, frame->height);
         }*/
        videoDecodeQueue.push_back(frame);
        decodeVideoFrameCount++;
        return true;
    }
    decodeFailedVideoFrameCount++;
    return false;
}

bool YUVHandler::addUndecodeVideoData(unsigned char *data, int dataSize) {
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

void YUVHandler::clearVideoQueue() {
    videoDecodeQueue.clear();
    videoUndecodeQueue.clear();
}

void YUVHandler::startPlay() {
    isPlaying = true;
    optimize.reset();
    //开启解码线程
    startDecodeVideoThread();
}

void YUVHandler::stopPlay() {
    isSoundOn = false;
    isPlaying = false;
    isCapture = false;
}

void YUVHandler::stopPlay(bool keepSoundStatus) {
    if (!keepSoundStatus) {
        isSoundOn = false;
    }
    isPlaying = false;
    isCapture = false;
}

int YUVHandler::capture(const char *savePath) {
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

void YUVHandler::startDecodeVideoThread() {
    //开启解码线程
    pthread_create(&decodeTid, &decodeAttr, decodeThread2, this);
}

void YUVHandler::doDecodeWork() {
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

void YUVHandler::onDraw() {
    //LOGI("onDraw");

    if (isPlaying) {

        if (videoDecodeQueue.size() > 0) {
            LOGI("videoDecodeQueue size:%d", videoDecodeQueue.size());
            AVFrame *avFrame = videoDecodeQueue.front();//get first without deleting
            videoDecodeQueue.pop_front();//delete first
            yuvRenderer->setFrameSize(avFrame->width, avFrame->height);
            LOGI("avFrame size=%d--%d", yuvRenderer->iFrameWidth, yuvRenderer->iFrameHeight);
            if (avFrame != NULL && isPlaying) {
                if (isCapture) {
                    saveFrame = avFrame;
                }
                renderFrameCount++;
                long pre = clock();
                yuvRenderer->renderFrame(avFrame->data);
                LOGI("renderFrame = %d us", (clock() - pre));
            }

        } else {
            yuvRenderer->renderFrame();
           // LOGI("帧队列为空");
        }
    } else {
        yuvRenderer->clearFrame();
    }
}

bool YUVHandler::resizeWindows(int width, int height) {
    yuvRenderer->resetWindow(width, height);
    return true;
}

void YUVHandler::reset() {
    resetVideoCount();
    resetAudioCount();
    videoCodec.reset();
    audioCodec.reset();

}

bool YUVHandler::createYuvRender() {
    yuvRenderer=new YUV420PRenderer();
    if(yuvRenderer!=NULL){
        return true;
    }
    return false;
}

void YUVHandler::resetVideoCount() {
    //收到的音视频
    receiveVideoPacketCount = 0;
    //解码的帧数
    decodeVideoFrameCount = 0;

    //解码失败
    decodeFailedVideoFrameCount = 0;

    //渲染的帧数
    renderFrameCount = 0;
}

void YUVHandler::resetAudioCount() {
    //收到的音视频
    receiveAudioPacketCount = 0;
    //解码的帧数
    decodeAudioFrameCount = 0;

    //解码失败
    decodeFailedAudioFrameCount = 0;

    //播放的帧数
    playAudioCount = 0;
}







































