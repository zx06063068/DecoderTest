//
// Created by zhangxin on 18-4-24.
//

#ifndef DECODERTEST_PHIMEDIASCANNERCLIENT_H
#define DECODERTEST_PHIMEDIASCANNERCLIENT_H
class PhiMediaScannerClient
{
public:
    PhiMediaScannerClient();
    virtual ~PhiMediaScannerClient();
    virtual void setDuration(long long t);
    //virtual  handleStringTag(const char* name, const char* value) = 0;
    //virtual  setMimeType(const char* mimeType) = 0;

};
#endif //DECODERTEST_PHIMEDIASCANNERCLIENT_H
