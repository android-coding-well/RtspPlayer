#include <jni.h>
#include <string>
#include "../demo/MediaPlayer.h"
#include "../player/RetCode.h"
#include "../include/log.h"
#include <unistd.h>
#include <android/native_window_jni.h>

#define  LOG_TAG    "native-lib"


extern "C" {

JNIEXPORT jstring JNICALL
Java_com_gosuncn_mplay_PlayerJNI_getMessageByReturnCode(JNIEnv *env, jobject instance, jint code) {

    RetCode retCode;
    string msg = retCode.getMessageByCode(code);
    if (msg.empty()) {
        msg = "未知错误";
    }
    return env->NewStringUTF(msg.c_str());
}

JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI_prepare(JNIEnv *env, jobject instance, jint handle, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);
    MediaPlayer *mPlayer = (MediaPlayer *) handle;
    if (mPlayer == NULL) {
        env->ReleaseStringUTFChars(url_, url);
        return -1;
    }
    int ret = mPlayer->prepare(url);
    return ret;
}

JNIEXPORT void JNICALL
Java_com_gosuncn_mplay_PlayerJNI_stopPlay(JNIEnv *env, jobject instance, jint handle) {

    MediaPlayer *mPlayer = (MediaPlayer *) handle;
    if (mPlayer == NULL) {
        return;
    }
    mPlayer->stopPlay2(true);
}

JNIEXPORT void JNICALL
Java_com_gosuncn_mplay_PlayerJNI_startPlay(JNIEnv *env, jobject instance, jint handle) {
    MediaPlayer *mPlayer = (MediaPlayer *) handle;
    if (mPlayer == NULL) {
        return;
    }
    mPlayer->startPlay();
}


JNIEXPORT void JNICALL
Java_com_gosuncn_mplay_PlayerJNI_onSurfaceCreated(JNIEnv *env, jobject instance, jint handle) {
    LOGI("onSurfaceCreated");

    MediaPlayer *mPlayer = (MediaPlayer *) handle;
    if (mPlayer == NULL) {
        return;
    }
    mPlayer->createYuvRenderer();
}

JNIEXPORT void JNICALL
Java_com_gosuncn_mplay_PlayerJNI_onSurfaceChanged(JNIEnv *env, jobject instance, jint handle,
                                                  jint width,
                                                  jint height) {

    MediaPlayer *mPlayer = (MediaPlayer *) handle;
    if (mPlayer == NULL) {
        return;
    }
    bool isSuccess = mPlayer->resizeWindows(width, height);
    if (!isSuccess) {
        mPlayer->createYuvRenderer();
        mPlayer->resizeWindows(width, height);
    }


}

JNIEXPORT void JNICALL
Java_com_gosuncn_mplay_PlayerJNI_onSurfaceDestroyed(JNIEnv *env, jobject instance, jint handle) {

    LOGI("onSurfaceDestroyed");
    MediaPlayer *mPlayer = (MediaPlayer *) handle;
    if (mPlayer == NULL) {
        return;
    }
    mPlayer->stopPlay2(false);
    usleep(50000);
    mPlayer->close();
    mPlayer = NULL;
}

JNIEXPORT void JNICALL
Java_com_gosuncn_mplay_PlayerJNI_onDrawFrame(JNIEnv *env, jobject instance, jint handle) {


    MediaPlayer *mPlayer = (MediaPlayer *) handle;
    if (mPlayer == NULL) {
        return;
    }
    mPlayer->onDraw();

}
JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI_createRtspPlayer(JNIEnv *env, jobject instance) {

    MediaPlayer *player = new MediaPlayer();
    return (int) player;
}

/**
 * 抓拍
 */
JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI_capture(JNIEnv *env, jobject instance, jint handle,
                                         jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    int ret = -1;
    MediaPlayer *mPlayer = (MediaPlayer *) handle;
    if (mPlayer == NULL) {
        return -1;
    }
    ret = mPlayer->capture(path);
    env->ReleaseStringUTFChars(path_, path);
    return ret;

}
/**
 * 声音开关
 */
JNIEXPORT void JNICALL
Java_com_gosuncn_mplay_PlayerJNI_sound(JNIEnv *env, jobject instance, jint handle, jboolean on) {

    MediaPlayer *mPlayer = (MediaPlayer *) handle;
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
Java_com_gosuncn_mplay_PlayerJNI_close(JNIEnv *env, jobject instance, jint handle) {

    MediaPlayer *mPlayer = (MediaPlayer *) handle;
    if (mPlayer == NULL) {
        return;
    }
    mPlayer->stopPlay2(false);
    usleep(50000);
    delete mPlayer;
    mPlayer = NULL;
}

JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI_prepare2(JNIEnv *env, jobject instance, jint handle,
                                          jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);
    MediaPlayer *mPlayer = (MediaPlayer *) handle;
    if (mPlayer == NULL) {
        return -1;
    }
    int ret = mPlayer->prepare2(url);
    env->ReleaseStringUTFChars(url_, url);
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_gosuncn_mplay_PlayerJNI_setSurface(JNIEnv *env, jobject instance, jint handle,
                                            jobject surface) {
    // 获取native window
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    if (nativeWindow == NULL) {
        return -1;
    }
    MediaPlayer *mPlayer = (MediaPlayer *) handle;
    if (mPlayer == NULL) {
        return -1;
    }
    mPlayer->setANativeWindow(nativeWindow);
    LOGI("prepare2:nativeWindow=%d", nativeWindow);
    return 1;
}

}

