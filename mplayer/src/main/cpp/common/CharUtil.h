
#ifndef VGIS_ANDROID_COMMONUTIL_H_
#define VGIS_ANDROID_COMMONUTIL_H_

#include <jni.h>

/**
 * 通用工具类
 */
class CharUtil {
public:

    //将jstring类型转化为char*,注意使用完必须手动free内存
    //[in] env
    //[in] jstr
    //[out] strlen jstr的长度
    //[in] encode 输出的编码格式，默认为utf-8
    //return
    static const char *jstringToPChar(JNIEnv *env, jstring jstr, int &strlen, char* encode="utf-8");

    //将jstring类型转化为char*,注意使用完必须手动free内存
    //[in] env
    //[in] jstr
    //[in] encode 输出的编码格式，默认为utf-8
    //return
    static const char *jstringToPChar(JNIEnv *env, jstring jstr, char* encode="utf-8");

    //将char*类型转化为jstring
    //[in] env
    //[in] pChar
    //[in] encode pChar的编码格式，默认为GB2312
    //return
    static jstring pCharToJstring(JNIEnv *env, const char *pChar, const char* encode="GB2312");


};


#endif
