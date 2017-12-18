#include <jni.h>
#include <string>
#include "../player/RetCode.h"
#include "../demo/MediaPlayer2.h"
#include "../include/log.h"
#include <unistd.h>
#include <android/native_window_jni.h>

#define  LOG_TAG    "StreamHandler"


extern "C" {

JNIEXPORT jstring JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_getMessageByReturnCode(JNIEnv *env, jobject instance, jint code) {

    RetCode retCode;
    string msg = retCode.getMessageByCode(code);
    if (msg.empty()) {
        msg = "未知错误";
    }
    return env->NewStringUTF(msg.c_str());
}

JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_prepare(JNIEnv *env, jobject instance, jint handle,
                                          jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);
    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        env->ReleaseStringUTFChars(url_, url);
        return -1;
    }
    int ret = mPlayer->prepare(url);
    return ret;
}

JNIEXPORT void JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_stopPlay(JNIEnv *env, jobject instance, jint handle) {

    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        return;
    }
    mPlayer->stopPlay2();
}

JNIEXPORT void JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_startPlay(JNIEnv *env, jobject instance, jint handle) {

    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        return;
    }
    mPlayer->startPlay();
}


JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_createRtspPlayer(JNIEnv *env, jobject instance) {

    MediaPlayer2 *player = new MediaPlayer2();
    return (int) player;
}
JNIEXPORT void JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_onSurfaceDestroyed(JNIEnv *env, jobject instance, jint handle) {
    LOGI("onSurfaceDestroyed");
    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        return;
    }
    mPlayer->stopPlay2();
    usleep(50000);
    mPlayer->close();
    mPlayer = NULL;
}
/**
 * 抓拍
 */
JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_capture(JNIEnv *env, jobject instance, jint handle,
                                          jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    int ret = -1;
    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        env->ReleaseStringUTFChars(path_, path);
        return -1;
    }
    ret = mPlayer->capture(path);
    return ret;

}
/**
 * 声音开关
 */
JNIEXPORT void JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_sound(JNIEnv *env, jobject instance, jint handle, jboolean on) {

    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        return;
    }
    if (on) {
        mPlayer->soundOn();
    } else {
        mPlayer->soundOff();
    }
}

JNIEXPORT void JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_close(JNIEnv *env, jobject instance, jint handle) {

    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        return;
    }
    mPlayer->stopPlay2();
    usleep(50000);
    delete mPlayer;
    mPlayer = NULL;
}


JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_setSurface(JNIEnv *env, jobject instance, jint handle,
                                             jobject surface) {

    // 获取native window
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    if (nativeWindow == NULL) {
        return -1;
    }
    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        return -1;
    }
    mPlayer->setANativeWindow(nativeWindow);
    LOGI("prepare2:nativeWindow=%d", nativeWindow);
    return 1;
}

JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_getRenderCount(JNIEnv *env, jobject instance, jint handle
                                            ) {

    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        return -1;
    }
    return mPlayer->rgbHandler.renderFrameCount;
}
JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_getReceiveVideoCount(JNIEnv *env, jobject instance, jint handle
) {

    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        return -1;
    }
    return mPlayer->rgbHandler.receiveVideoPacketCount;
}
JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_getDecodeVideoCount(JNIEnv *env, jobject instance, jint handle
) {

    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        return -1;
    }
    return mPlayer->rgbHandler.decodeVideoFrameCount;
}
JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_getDecodeFailedVideoCount(JNIEnv *env, jobject instance, jint handle
) {

    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        return -1;
    }
    return mPlayer->rgbHandler.decodeFailedVideoFrameCount;
}
JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_getReceiveAudioCount(JNIEnv *env, jobject instance, jint handle
) {

    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        return -1;
    }
    return mPlayer->rgbHandler.receiveAudioPacketCount;
}
JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_getDecodeAudioCount(JNIEnv *env, jobject instance, jint handle
) {

    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        return -1;
    }
    return mPlayer->rgbHandler.decodeAudioFrameCount;
}

JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_getPlayAudioCount(JNIEnv *env, jobject instance, jint handle
) {

    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        return -1;
    }
    return mPlayer->rgbHandler.playAudioCount;
}

JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI3_getDecodeFailedAudioCount(JNIEnv *env, jobject instance, jint handle
) {

    MediaPlayer2 *mPlayer = (MediaPlayer2 *) handle;
    if (mPlayer == NULL) {
        return -1;
    }
    return mPlayer->rgbHandler.decodeFailedAudioFrameCount;
}


}
