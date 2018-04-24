//
// Created by zhangxin on 18-4-24.
//
#include <android/log.h>
#include "PhiMediaScannerClient.h"
#define TAG "phiMediaScannerClient"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)

PhiMediaScannerClient::PhiMediaScannerClient() {}
PhiMediaScannerClient::~PhiMediaScannerClient() {}
void PhiMediaScannerClient::setDuration(long long t) {
    LOGD("PhiMediaScannerClient  setDuration : %ld",t);
}
