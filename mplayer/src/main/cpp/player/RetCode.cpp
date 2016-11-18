//
// Created by hwj on 2016/10/27.
//

#include "Retcode.h"

RetCode::RetCode (){
    map.insert(pair<int, string>(SUCCESS, "成功"));
    map.insert(pair<int, string>(FAILED, "失败"));
    map.insert(pair<int, string>(UNKNOW, "未知错误"));
    map.insert(pair<int, string>(OPEN_FILE_FAILED, "打开文件失败"));
    map.insert(pair<int, string>(FIND_STREAM_INFORMATION_FAILED, "找不到流信息"));
    map.insert(pair<int, string>(FIND_VIDEO_STREAM_FAILED, "找不到视频流"));
    map.insert(pair<int, string>(FIND_AUDIO_STREAM_FAILED, "找不到音频流"));
    map.insert(pair<int, string>(FIND_CODEC_FAILED, "找不到编解码器"));
    map.insert(pair<int, string>(OPEN_CODEC_FAILED, "打开编解码器失败"));
    map.insert(pair<int, string>(ALLOC_AVCODECCONTEXT_FAILED, "分配编解码器失败"));
    map.insert(pair<int, string>(PARAMS_ERROR, "参数错误"));
    map.insert(pair<int, string>(CAPTURE_FAILED_WITHOUT_PLAYING, "抓拍失败，视频未播放"));
    map.insert(pair<int, string>(CAPTURE_ERROR_IN_FILE, "抓拍失败，请检查文件路径是否存在或正确"));
}
string RetCode::getMessageByCode(int code) {

    return map[code];
}



