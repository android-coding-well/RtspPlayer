
#include<stdlib.h>
#include<jni.h>
#include<string.h>
#include "CharUtil.h"

/**
 * 将jstring类型转为char类型指针
 */
const char* CharUtil::jstringToPChar(JNIEnv* env,jstring  jstr,int &strlen, char* encode) {
	char* rtn = NULL;
	jclass clsstring = env->FindClass("java/lang/String");
	jstring strencode = env->NewStringUTF(encode);
	jmethodID mid = env->GetMethodID(clsstring, "getBytes",
									 "(Ljava/lang/String;)[B");
	jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
	jsize alen = env->GetArrayLength(barr);
	jbyte* ba = env->GetByteArrayElements(barr, 0);
	if (alen > 0) {
		strlen = alen + 1;
		rtn = (char*) malloc(strlen);
		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}
	env->ReleaseByteArrayElements(barr, ba, 0);
	env->DeleteLocalRef(strencode);
	return rtn;
}


const char *CharUtil::jstringToPChar(JNIEnv *env, jstring jstr, char *encode) {
	int len;
	return jstringToPChar(env,jstr,len,encode);
}


/**
 *将char类型指针转为jstring
 */
jstring CharUtil::pCharToJstring(JNIEnv* env, const char* pChar, const char* encode) {
	jclass strClass = env->FindClass("java/lang/String");
	jmethodID mID = env->GetMethodID(strClass, "<init>",
									 "([BLjava/lang/String;)V");//[B表示byte[],Ljava/lang/String;表示String类
	jbyteArray bytes = env->NewByteArray((jsize) strlen(pChar));
	env->SetByteArrayRegion(bytes, 0, (jsize) strlen(pChar), (jbyte*) pChar); //将char* 转换为byte数组
	jstring encoding = env->NewStringUTF(encode);
	return (jstring) env->NewObject(strClass, mID, bytes, encoding);
}









