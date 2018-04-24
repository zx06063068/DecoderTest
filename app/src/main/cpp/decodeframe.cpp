//
// Created by zhangxin on 18-4-24.
//
#include <jni.h>
#include <android/log.h>
#include "PhiDecoder.h"
#include "PhiMediaScannerClient.h"

#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(^_^)", format, ##__VA_ARGS__)

extern "C"
JNIEXPORT jint JNICALL
Java_com_phicomm_decoder_DecodeFrameUtil_randomscreen(JNIEnv *env, jobject thiz,
                                                      jstring inputurl_,
                                                      jstring outputurl_) {
    const char *inputurl = env->GetStringUTFChars(inputurl_, 0);
    const char *outputurl = env->GetStringUTFChars(outputurl_, 0);
    PhiMediaScannerClient phiMediaScannerClient=PhiMediaScannerClient();
    processFile(inputurl,outputurl,phiMediaScannerClient);
    env->ReleaseStringUTFChars(inputurl_, inputurl);
    env->ReleaseStringUTFChars(outputurl_, outputurl);
    return  0;
}



