//
// Created by hwj on 2016/11/7.
//

#include "AudioPlayer.h"

#define  LOG_TAG    "AudioPlayer"

void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    AudioPlayer *player = (AudioPlayer *) context;
    if (player != NULL && player->callback != NULL) {
        player->callback(bq, player->context);
    }
}

AudioPlayer::AudioPlayer() {

}

AudioPlayer::~AudioPlayer() {
    shutdown();
}

int AudioPlayer::createEngine() {
    isCreatedEngineSuccess = false;
    SLresult result;
    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if (result != SL_RESULT_SUCCESS) {
        LOGI("Create engine failed:errorcode=%d", result);
        return -1;
    }

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGI("Realize engine failed:errorcode=%d", result);
        return -1;
    }
    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (result != SL_RESULT_SUCCESS) {
        LOGI("Get engine interface failed:errorcode=%d", result);
        return -1;
    }

    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, 0, 0);
    if (result != SL_RESULT_SUCCESS) {
        LOGI("Create output mix failed:errorcode=%d", result);
        return -1;
    }

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGI("Realize output mix failed:errorcode=%d", result);
        return -1;
    }

    // get the environmental reverb interface
    // this could fail if the environmental reverb effect is not available,
    // either because the feature is not present, excessive CPU load, or
    // the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
    }

    isCreatedEngineSuccess = true;
    return 1;
}

// create buffer queue audio player
int AudioPlayer::createBufferQueueAudioPlayer(int rate, int channel) {
    isCreatedAudioPlayerSuccess = false;
    if (!isCreatedEngineSuccess) {
        return -1;
    }

    LOGI("createBufferQueueAudioPlayer:%d,%d", rate, channel);
    SLresult result;

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM,//formatType
                                   channel, //numChannels
                                   rate * 1000,//samplesPerSec  SL_SAMPLINGRATE_8
                                   SL_PCMSAMPLEFORMAT_FIXED_16, //bitsPerSample
                                   SL_PCMSAMPLEFORMAT_FIXED_16,//containerSize
                                   SL_SPEAKER_FRONT_CENTER, //channelMask
                                   SL_BYTEORDER_LITTLEENDIAN};//endianness
    if (channel == 2) {
        format_pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    } else {
        format_pcm.channelMask = SL_SPEAKER_FRONT_CENTER;
    }
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    // create audio player
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/ SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk,
                                                3, ids, req);
    if (SL_RESULT_SUCCESS != result) {
        LOGI("CreateAudioPlayer failed:errorcode=%d", result);
        return -1;
    }


    // realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        LOGI("Realize player failed:errorcode=%d", result);
        return -1;
    }

    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    if (SL_RESULT_SUCCESS != result) {
        LOGI("Get play interface failed:errorcode=%d", result);
        return -1;
    }

    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
                                             &bqPlayerBufferQueue);
    if (SL_RESULT_SUCCESS != result) {
        LOGI("Get buffer queue interface failed:errorcode=%d", result);
        return -1;
    }

    // register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, this);
    if (SL_RESULT_SUCCESS != result) {
        LOGI("Register callback on the buffer queue failed:errorcode=%d", result);
        return -1;
    }

    // get the effect send interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND,
                                             &bqPlayerEffectSend);
    if (SL_RESULT_SUCCESS != result) {
        LOGI("Get effect send interface failed:errorcode=%d", result);
        return -1;
    }

    // get the volume interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
    if (SL_RESULT_SUCCESS != result) {
        LOGI("Get the volume interface failed:errorcode=%d", result);
        return -1;
    }

    // set the player's state to playing
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    if (SL_RESULT_SUCCESS != result) {
        LOGI("Set the player's state failed:errorcode=%d", result);
        return -1;
    }
    isCreatedAudioPlayerSuccess = true;
    return 1;
}

int AudioPlayer::enqueue(const void *pBuffer, unsigned int size) {
    if (!isCreatedAudioPlayerSuccess) {
        return -1;
    }
    SLresult result;
    // enqueue another buffer
    result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, pBuffer,
                                             size);
    if (SL_RESULT_SUCCESS != result) {
        LOGI("Set the player's state failed:errorcode=%d", result);
        return -1;
    }
    return 1;
}

void AudioPlayer::setCallback(slAndroidSimpleBufferQueueCallback callback, void *context) {
    this->callback = callback;
    this->context = context;
}

void AudioPlayer::shutdown() {

    // destroy buffer queue audio player object, and invalidate all associated interfaces
    if (bqPlayerObject != NULL) {
        (*bqPlayerObject)->Destroy(bqPlayerObject);
        bqPlayerObject = NULL;
        bqPlayerPlay = NULL;
        bqPlayerBufferQueue = NULL;
        bqPlayerEffectSend = NULL;
        bqPlayerVolume = NULL;
    }
    // destroy output mix object, and invalidate all associated interfaces
    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }
}

bool AudioPlayer::isCreateAudioPlayerSuccess() {
    return isCreatedAudioPlayerSuccess;
}

bool AudioPlayer::isCreateEngineSuccess() {
    return isCreatedEngineSuccess;
}

int AudioPlayer::convertToS16(AVFrame *pFrame, uint8_t **audio_buff, int &data_size) {
    if (pFrame == NULL) {
        return -1;
    }

    AVSampleFormat dst_format = AV_SAMPLE_FMT_S16;//av_get_packed_sample_fmt((AVSampleFormat)frame->format);
    int64_t dst_layout = av_get_default_channel_layout(pFrame->channels);
    /*const int MAX_AUDIO_FRAME_SIZE = 8196;//8196 192000
    //uint8_t *audio_buff[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
    uint8_t *audio_buff;

    audio_buff = (uint8_t *) malloc(sizeof(uint8_t) * MAX_AUDIO_FRAME_SIZE);//8196*/
    data_size = 0;
    SwrContext *swr_ctx = swr_alloc_set_opts(NULL,
                                             dst_layout,
                                             dst_format,
                                             pFrame->sample_rate,
                                             pFrame->channel_layout,
                                             (AVSampleFormat) pFrame->format,
                                             pFrame->sample_rate,
                                             0, NULL);
    if (!swr_ctx || swr_init(swr_ctx) < 0) {
        LOGI("swr_init failed.");
        return -1;
    }
   // LOGI("swr_init success.");
    // 计算转换后的sample个数 a * b / c
    int dst_nb_samples = av_rescale_rnd(
            swr_get_delay(swr_ctx, pFrame->sample_rate) + pFrame->nb_samples, pFrame->sample_rate,
            pFrame->sample_rate, AVRounding(1));
    LOGI("dst_nb_samples =%d %d", dst_nb_samples, pFrame->nb_samples);

    // 转换，返回值为转换后的sample个数
    int nb = swr_convert(swr_ctx, audio_buff, dst_nb_samples, (const uint8_t **) pFrame->data,
                         pFrame->nb_samples);
    LOGI("nb =%d nb_samples=%d", nb, pFrame->nb_samples);
    data_size = pFrame->channels * nb * av_get_bytes_per_sample(dst_format);

    return 1;
}















