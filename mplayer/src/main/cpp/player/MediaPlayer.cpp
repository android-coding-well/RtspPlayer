//
// Created by hwj on 2016/10/24.
//

#include <string>

#include "MediaPlayer.h"
#include "RetCode.h"

using namespace std;


void audioPlayerCallback(SLAndroidSimpleBufferQueueItf caller,
                         void *pContext) {
    LOGI("audioPlayerCallback");
    MediaPlayer *player = (MediaPlayer *) pContext;
    LOGI("audioPlayerCallback:audioQueue size=%d", player->audioQueue.size());
    if (player->audioQueue.size() > 0) {
        AVFrame *pFrame = player->audioQueue.front();
        player->audioQueue.pop_front();
        AVSampleFormat dst_format = AV_SAMPLE_FMT_S16;//av_get_packed_sample_fmt((AVSampleFormat)frame->format);
        int64_t dst_layout = av_get_default_channel_layout(pFrame->channels);
        const int MAX_AUDIO_FRAME_SIZE = 8196;//8196 192000
        uint8_t *audio_buff;
        audio_buff = (uint8_t *) malloc(sizeof(uint8_t) * MAX_AUDIO_FRAME_SIZE);//8196
        int data_size = 0;

        if (!player->audioPlayer.isCreateAudioPlayerSuccess()) {
            player->audioPlayer.createBufferQueueAudioPlayer(pFrame->sample_rate, pFrame->channels);
        }

        if (player->convertToS16(pFrame, &audio_buff, data_size) == 1) {
            player->audioPlayer.enqueue(audio_buff, data_size);
        };
    }

}

MediaPlayer::MediaPlayer() {
    videoQueue = deque<AVFrame *>();
    audioQueue = deque<AVFrame *>();
    pthread_mutex_init(&mutex, NULL);
    av_register_all();
    avformat_network_init();
    av_lockmgr_register(lockmgr);

    audioPlayer.createEngine();
    audioPlayer.setCallback(audioPlayerCallback, this);
}

MediaPlayer::~MediaPlayer() {
    LOGI("~MediaPlayer is called");
    stopPlay();
    pthread_mutex_destroy(&mutex);
    close();
    LOGI("~MediaPlayer call end");
}

void MediaPlayer::close() {
    stopPlay();
    pthread_mutex_destroy(&mutex);
    // Free the YUV frame
    if (saveFrame != NULL) {
        av_free(saveFrame);
        saveFrame = NULL;
    }

    if (yuvRenderer != NULL) {
        delete yuvRenderer;
        yuvRenderer = NULL;
    }

}


//视频码流回调
void videoPacketCallback(void *handle, AVPacket packet) {
    MediaPlayer *player = (MediaPlayer *) handle;
    if (player->isVideoPlaying()) {
        AVFrame *pFrame = player->videoCodec.decode(packet);
        if (pFrame != NULL) {
            player->videoQueue.push_back(pFrame);
            player->frameWidth = pFrame->width;
            player->frameHeight = pFrame->height;
            if (player->yuvRenderer != NULL) {
                player->yuvRenderer->setFrameSize(player->frameWidth, player->frameHeight);
            }
        }
    }
}

//视频码流回调
void videoPacketCallback2(void *handle, AVPacket packet) {
    LOGI("videoPacketCallback2");
    MediaPlayer *player = (MediaPlayer *) handle;
    if (player->isVideoPlaying()) {
        LOGI("videoPacketCallback3");
        AVFrame *pFrame = player->videoCodec.decode(packet);
        LOGI("videoPacketCallback4");
        if (pFrame != NULL) {
            if (player->isCapture) {
                player->saveFrame = pFrame;
                player->isCapture = false;
            }
            LOGI("videoPacketCallback5");
            player->frameWidth = pFrame->width;
            player->frameHeight = pFrame->height;
            if (!player->convertor.isPreparedYUV2RGBSuccess()) {
                player->convertor.prepareYUV2RGB(pFrame->width, pFrame->height,
                                                 player->videoCodec.getAVCodecContext()->pix_fmt);
            }
            if (!player->rgbRenderer.isPreparedSuccess()) {
                player->rgbRenderer.prepare(player->getANativeWindow(), pFrame->width,
                                            pFrame->height);
            }
            if (player->isVideoPlaying()) {
                player->rgbRenderer.render(player->convertor.YUV2RGB(pFrame));
            }
        }
    }

}


//音频码流回调
void audioPacketCallback(void *handle, AVPacket packet) {

    MediaPlayer *player = (MediaPlayer *) handle;
    if (player->isVideoPlaying() && player->isAudioSoundOn()) {
        AVFrame *pFrame = player->audioCodec.decode(packet);
        if (pFrame != NULL) {
            player->audioQueue.push_back(pFrame);
            //平面格式将不同的声道的数据分开存储，每种数据对应一个data[i]；压缩格式是左右声道交替存储的，
            // 它只占用了AVFrame结构体中的data[0]
            int isPlanar = av_sample_fmt_is_planar((AVSampleFormat) pFrame->format);
            LOGI("音频解码回调:是否平面格式= %d ,sample_fmt_name=%s", isPlanar,
                 av_get_sample_fmt_name((AVSampleFormat) pFrame->format));


            const int MAX_AUDIO_FRAME_SIZE = 8196;//8196 192000 一般要大于一帧的两倍
            uint8_t *audioBuff;
            audioBuff = (uint8_t *) malloc(sizeof(uint8_t) * MAX_AUDIO_FRAME_SIZE);//8196
            int dataSize = 0;

            if (!player->audioPlayer.isCreateAudioPlayerSuccess()) {
                player->audioPlayer.createBufferQueueAudioPlayer(pFrame->sample_rate,
                                                                 pFrame->channels);
            }

            if (player->audioQueue.size() == 1 &&
                player->convertToS16(pFrame, &audioBuff, dataSize) == 1) {
                player->audioPlayer.enqueue(audioBuff, dataSize);
                player->audioQueue.pop_front();
            };

        }
    } else {
        player->audioQueue.clear();
    }

}


void MediaPlayer::startPlay() {
    LOGI("%d startPlay", this);
    isCapture = false;
    isPlaying = true;
    videoQueue.clear();
    streamTaker.startTakeStream();
}


void MediaPlayer::stopPlay() {
    LOGI("%d stopPlay", this);
    streamTaker.stopTakeStream();
    isCapture = false;
    isPlaying = false;
    isSoundOn = false;
    videoQueue.clear();
}

void MediaPlayer::stopPlay2(bool isClearScreen) {
    LOGI("%d stopPlay2", this);
    streamTaker.stopTakeStream();
    isCapture = false;
    isPlaying = false;
    isSoundOn = false;
    videoQueue.clear();
    if (isClearScreen) {
        rgbRenderer.clearScreen();
    }
}

int MediaPlayer::lockmgr(void **mtx, enum AVLockOp op) {
    LOGE("lockmgr %d", op);
    switch (op) {
        case AV_LOCK_CREATE: {
            int ret = pthread_mutex_init((pthread_mutex_t *)
                                                 mtx, NULL);
            LOGE("AV_LOCK_CREATE %d %d", ret, !*mtx);

            if (ret != 0)
                return 1;
            return 0;
        }
        case AV_LOCK_OBTAIN: {
            int ret = !!pthread_mutex_lock((pthread_mutex_t *)
                                                   mtx);
            LOGE("AV_LOCK_OBTAIN %d", ret);
            return ret;
        }

        case AV_LOCK_RELEASE: {
            int ret = !!pthread_mutex_unlock((pthread_mutex_t *)
                                                     mtx);
            LOGE("AV_LOCK_OBTAIN %d", ret);
            return ret;
        }
        case AV_LOCK_DESTROY:
            pthread_mutex_destroy((pthread_mutex_t *)
                                          mtx);
            return 0;
    }
    return 1;
}

/**
 * 播放前准备，如果返回1则表示准备成功
 */
int MediaPlayer::prepare(const char *url) {
    stopPlay();
    int ret = streamTaker.prepare(url);
    if (ret != SUCCESS) {
        return ret;
    }
    streamTaker.setVideoPacketCallback(this, videoPacketCallback);
    streamTaker.setAudioPacketCallback(this, audioPacketCallback);

    //视频解码准备
    int retCode = videoCodec.prepareDecode(streamTaker.getVideoCodeID());
    if (retCode != SUCCESS) {
        return retCode;
    }
    //音频解码准备
    int retCode2 = audioCodec.prepareDecode(streamTaker.getAudioCodeID());
    if (retCode2 != SUCCESS) {
        // return retCode;
        LOGI("audio prepareDecode failed");
    } else {
    }

    return SUCCESS;
}


int MediaPlayer::prepare2(const char *url) {

    int ret = prepare(url);
    if (ret != SUCCESS) {
        return ret;
    }
    streamTaker.setVideoPacketCallback(this, videoPacketCallback2);

    return SUCCESS;

}

void MediaPlayer::onDraw() {
    if (yuvRenderer != NULL) {
        if (isPlaying) {
            if (videoQueue.size() > 0) {
                // LOGI("帧队列不为空");
                AVFrame *avFrame = videoQueue.front();//get first without deleting
                videoQueue.pop_front();//delete first
                // LOGI("%d 正在渲染此帧大小：%d %d", (int) this, yuvRenderer->iFrameWidth,
                //      yuvRenderer->iFrameHeight);
                if (avFrame != NULL && isPlaying) {
                    yuvRenderer->renderFrame(avFrame->data);
                }
                if (isCapture) {
                    saveFrame = avFrame;
                    isCapture = false;
                }
            } else {
                yuvRenderer->renderFrame();
                //  LOGI("帧队列为空");
            }
        } else {
            yuvRenderer->clearFrame();
            isCapture = false;
        }


    } else {
        isCapture = false;
    }

}


bool MediaPlayer::resizeWindows(int width, int height) {
    LOGI("%d 正在重置窗体大小:%d %d", this, width, height);
    if (yuvRenderer != NULL) {
        yuvRenderer->resetWindow(width, height);
        LOGI("%d 重置窗体大小成功", this);
        return true;
    }
    LOGI("%d 重置窗体大小失败:没有创建YUV播放器", this);
    return false;
}

bool MediaPlayer::createYuvRenderer() {

    LOGI("%d 正在创建YUV播放器", this);
    yuvRenderer = new YUV420PRenderer();
    LOGI("%d 创建YUV播放器成功", this);
    return true;
}

int MediaPlayer::capture(const char *savePath) {
    if (!isPlaying) {
        LOGI("%d 视频未播放，无法抓拍", this);
        return CAPTURE_FAILED_WITHOUT_PLAYING;
    }

    LOGI("%d 抓拍开始...", this);
    isCapture = true;
    saveFrame = NULL;
    clock_t start, ends;//1秒clock1000000,相当于1ms=1000
    start = clock();
    while (isCapture) {
        ends = clock();
        if (ends - start > 250 * 1000) {//250ms后退出，保证不会一直阻塞在这里
            LOGI("%d 超时，无法抓拍", this);
            break;
        }
    }
    isCapture = false;
    if (saveFrame == NULL) {
        return FAILED;
    }

    LOGI("%d 正在保存抓拍图片...", this);
    Jpeg jpeg;
    int ret = jpeg.saveYuv2Jpeg(savePath, saveFrame);
    LOGI("%d 保存抓拍图片结果：返回码=%d 路径=%s", this, ret, savePath);
    if (ret != 1) {
        return CAPTURE_ERROR_IN_FILE;
    }
    return SUCCESS;
}

int MediaPlayer::convertToS16(AVFrame *pFrame, uint8_t **audio_buff, int &data_size) {
    if (pFrame == NULL) {
        return -1;
    }

    AVSampleFormat dst_format = AV_SAMPLE_FMT_S16;//av_get_packed_sample_fmt((AVSampleFormat)frame->format);
    int64_t dst_layout = av_get_default_channel_layout(pFrame->channels);
    /*const int MAX_AUDIO_FRAME_SIZE = 8196;//8196 192000
    //uint8_t *audio_buff[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
    uint8_t *audio_buff;

    audio_buff = (uint8_t *) malloc(sizeof(uint8_t) * MAX_AUDIO_FRAME_SIZE);//8196*/
    data_size = 0;
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
    int nb = swr_convert(swr_ctx, audio_buff, dst_nb_samples, (const uint8_t **) pFrame->data,
                         pFrame->nb_samples);
    LOGI("nb =%d nb_samples=%d", nb, pFrame->nb_samples);
    data_size = pFrame->channels * nb * av_get_bytes_per_sample(dst_format);

    return 1;
}

void MediaPlayer::soundOn() {
    isSoundOn = true;
}

void MediaPlayer::soundOff() {
    isSoundOn = false;
}

bool MediaPlayer::isVideoPlaying() {
    return isPlaying;
}

bool MediaPlayer::isAudioSoundOn() {
    return isSoundOn;
}

void MediaPlayer::setANativeWindow(ANativeWindow *nativeWindow) {
    this->nativeWindow = nativeWindow;
}

ANativeWindow *MediaPlayer::getANativeWindow() {
    return this->nativeWindow;
}































