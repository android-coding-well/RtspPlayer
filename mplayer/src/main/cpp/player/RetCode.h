//
// 返回码
// Created by hwj on 2016/10/27.
//

#ifndef MEDIAAPP_RETCODE_H
#define MEDIAAPP_RETCODE_H

#include <string>
#include <hash_map>
using namespace std;
enum RetCodeEnum{
    FAILED=-1,
    UNKNOW=999,
    SUCCESS=1,
    OPEN_FILE_FAILED,
    FIND_STREAM_INFORMATION_FAILED,
    FIND_VIDEO_STREAM_FAILED,
    FIND_AUDIO_STREAM_FAILED,
    FIND_CODEC_FAILED,
    OPEN_CODEC_FAILED,
    ALLOC_AVCODECCONTEXT_FAILED,
    PARAMS_ERROR,
    CAPTURE_FAILED_WITHOUT_PLAYING,
    CAPTURE_ERROR_IN_FILE

};
class RetCode{
public :
     RetCode();
     string getMessageByCode(int code);

private :
    __gnu_cxx::hash_map<int, string> map;
};

#endif //MEDIAAPP_RETCODE_H
