#include <string>

#include "MediaPlayer2.h"
#include "../player/RetCode.h"

#define  LOG_TAG    "MediaPlayer2"
using namespace std;

MediaPlayer2::MediaPlayer2() {
   // audioQueue = deque<AVFrame *>();
    pthread_mutex_init(&mutex, NULL);
    av_register_all();
    avformat_network_init();
    av_lockmgr_register(lockmgr);

}

MediaPlayer2::~MediaPlayer2() {
    LOGI("~MediaPlayer2 is called");
    stopPlay2();
    pthread_mutex_destroy(&mutex);
    close();
    LOGI("~MediaPlayer2 call end");
}

int MediaPlayer2::lockmgr(void **mtx, enum AVLockOp op) {
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

void MediaPlayer2::close() {
    stopPlay2();
    pthread_mutex_destroy(&mutex);

}


//视频码流回调
void videoPacketCallback3(void *handle, AVPacket packet) {
    LOGI("videoPacketCallback3");
    MediaPlayer2 *player = (MediaPlayer2 *) handle;
    player->rgbHandler.addUndecodeVideoData(packet.data,packet.size);
}


//音频码流回调
void audioPacketCallback2(void *handle, AVPacket packet) {
    LOGI("audioPacketCallback2");
    MediaPlayer2 *player = (MediaPlayer2 *) handle;
   // player->rgbHandler.addUndecodeAudioData(packet);
    player->rgbHandler.addUndecodeAudioData(packet.data,packet.size);
}

/**
 * 播放前准备，如果返回1则表示准备成功
 */
int MediaPlayer2::prepare(const char *url) {
    stopPlay2(true);
    rgbHandler.reset();
    int ret = streamTaker.prepare(url);
    if (ret != SUCCESS) {
       return ret;
    }
    streamTaker.setVideoPacketCallback(this, videoPacketCallback3);
    streamTaker.setAudioPacketCallback(this, audioPacketCallback2);
    rgbHandler.setVideoFrameSize(streamTaker.getFrameWidth(),streamTaker.getFrameHeight());

    rgbHandler.setAudioDecodecParameters(streamTaker.getCodecParameters()->channels,streamTaker.getCodecParameters()->channelLayout
            ,streamTaker.getCodecParameters()->sampleRate);
    rgbHandler.setVideoDecodecID(streamTaker.getVideoCodeID());
    rgbHandler.setAudioDecodecID(streamTaker.getAudioCodeID());

    return SUCCESS;
}

void MediaPlayer2::startPlay() {
    LOGI("%d startPlay", this);
    rgbHandler.clearVideoQueue();
    streamTaker.startTakeStream();
    rgbHandler.startPlay();

}

void MediaPlayer2::stopPlay2() {
    LOGI("%d stopPlay2", this);
    streamTaker.stopTakeStream();
    rgbHandler.stopPlay();
}
void MediaPlayer2::stopPlay2(bool keepSoundStatus) {
    LOGI("%d stopPlay2:%d", this,keepSoundStatus);
    streamTaker.stopTakeStream();
    rgbHandler.stopPlay(keepSoundStatus);
}

int MediaPlayer2::capture(const char *savePath) {
    return rgbHandler.capture(savePath);
}

void MediaPlayer2::soundOn() {
    rgbHandler.clearAudioDecodeQueue();
    rgbHandler.soundOn();
}

void MediaPlayer2::soundOff() {
    rgbHandler.soundOff();
}

bool MediaPlayer2::isVideoPlaying() {
    return rgbHandler.isVideoPlaying();
}

bool MediaPlayer2::isAudioSoundOn() {
    return rgbHandler.isAudioSoundOn();
}

void MediaPlayer2::setANativeWindow(ANativeWindow *nativeWindow) {
    rgbHandler.setANativeWindow(nativeWindow);
}








