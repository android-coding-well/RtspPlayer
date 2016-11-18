//
// Created by hwj on 2016/10/27.
//

#ifndef MEDIAAPP_LOG_H
#define MEDIAAPP_LOG_H
#include <android/log.h>

#define  LOG_TAG    "RtspPlayer"
#define isDebug true
#define  LOGV(...)  if(isDebug){__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);}
#define  LOGD(...)  if(isDebug){__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__);}
#define  LOGI(...)  if(isDebug){__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__);}
#define  LOGW(...)  if(isDebug){__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__);}
#define  LOGE(...)  if(isDebug){__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__);}
#endif //MEDIAAPP_LOG_H
