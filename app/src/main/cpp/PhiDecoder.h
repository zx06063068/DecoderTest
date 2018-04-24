//
// Created by zhangxin on 18-4-24.
//

#ifndef DECODERTEST_PHIDECODER_H
#define DECODERTEST_PHIDECODER_H

#include "PhiMediaScannerClient.h"
#ifdef __cplusplus
extern "C" {
#endif
int processFile(const char* inputpath,const char* outputpath,PhiMediaScannerClient &phiMediaScannerClient);
#ifdef __cplusplus
}
#endif
#endif //DECODERTEST_PHIDECODER_H
