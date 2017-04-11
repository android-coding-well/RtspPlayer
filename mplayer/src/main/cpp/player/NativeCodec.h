//
// -DANDROID_PLATFORM=android-21
// Created by hwj on 2017/2/20.
//

#ifndef SAFETY1_1_0_NATIVECODEC_H
#define SAFETY1_1_0_NATIVECODEC_H
#include <media/NdkMediaCodec.h>
class NativeCodec{
public :
    NativeCodec();
    virtual ~NativeCodec();

    int configDecoder(int frameWidth,int frameHeight,ANativeWindow* aNativeWindow);

    //输入未解码数据
    int inputData(uint8_t* data,int size);
    //获得解码数据
    uint8_t* getDecodeData(size_t &size);

    void doDecodeWork(uint8_t * data,size_t size);
private :

    int frameWidth;
    int frameHeight;

    AMediaCodec *codec = NULL;
    ANativeWindow *aNativeWindow = NULL;
};

#endif //SAFETY1_1_0_NATIVECODEC_H
