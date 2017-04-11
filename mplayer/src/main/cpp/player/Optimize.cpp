//
// Created by hwj on 2017/1/22.
//

#include <time.h>
#include "Optimize.h"
#include "log.h"
#include <math.h>

#define  LOG_TAG    "Optimize"

Optimize::Optimize() {
    reset();
    cacheMode = CACHE_WITH_TIME;
    MAX_CACHE_COUNT = DEFAULT_CACHE_COUNT;
}

Optimize::~Optimize() {

}

void Optimize::receiveFrame() {
    if (preFrameTime == 0) {
        preFrameTime = clock();
    } else {

        //第一种计算平均值
        intervalCount++;
        averageIntervalTime = (clock() - preFrameTime) / intervalCount;
        /*if(intervalCount>1000){//已经收到1000帧时重新计算相邻帧平均间隔
            LOGE("has receive 1000 frame and now the averageIntervalTime is %d us",averageIntervalTime);
            intervalCount=0;
            preFrameTime=0;
        }*/

        //第二种计算平均值
        /*int interval = clock() - preFrameTime;
        preFrameTime = clock();
        LOGI("frame interval time=%d", interval);

        if (interval < averageIntervalTime / 5 || interval > averageIntervalTime * 5) { //在此范围内的微秒数则丢弃
            LOGE("unnoraml frame interval time=%d", interval);
            //return;
        }

        intervalCount++;
        sumIntervalTime += interval;
        averageIntervalTime = sumIntervalTime / intervalCount;

        if(intervalCount>800){//已经收到800帧时重新计算相邻帧平均间隔
            LOGE("has receive 1000 frame and now the averageIntervalTime is %d us",averageIntervalTime);
            intervalCount=1;
            sumIntervalTime=averageIntervalTime;
        }*/



    }
}

void Optimize::setCacheMode(CacheMode mode, int param) {
    switch (mode) {
        case CACHE_WITH_TIME:
            if (param > 0) {
                MAX_CACHE_TIME = param;
            }
            break;
        case CACHE_WITH_COUNT:
            if (param > 0) {
                MAX_CACHE_COUNT = param;
            }
            break;
    }
}

void Optimize::reset() {
    //前一帧的时间，us
    preFrameTime = 0;
    //总的间隔时间.us
    sumIntervalTime = 0;
    //间隔个数
    intervalCount = 0;

    //平均间隔时间，us
    averageIntervalTime = 0;

    //未解码队列中的个数(实时设置)
    undecodeFrameQueueSize = 0;
    //已经解码队列中的个数(实时设置)
    hasDecodeFrameQueueSize = 0;

    //渲染一帧的时间(实时设置)，ms
    renderFrameTime = 0;

    //cacheMode = CACHE_WITH_COUNT;

    //MAX_CACHE_COUNT = DEFAULT_CACHE_COUNT;
}

int Optimize::getAverageIntervalTime() {
    //return averageIntervalTime;
    if (averageIntervalTime > 50000) {//一开始相邻帧的平均间隔时间还未稳定，因此默认每秒20帧的速度播放
        return averageIntervalTime;
    }
    return 50000;
}

int Optimize::getRenderWaitTime(int actualCacheCount, int renderFrameTime) {

    int sleepTime = 0;
    int aveIntervalTime = getAverageIntervalTime();

    if (actualCacheCount == 0) { //如果缓存帧数为0，且流不足，此时视频可能会产生卡顿
        LOGE("video cache frame count is 0 ,video nonfluency!!!");
    }
    //LOGI("frame average interval time = %d,queue count = %d ,render time = %d us", aveIntervalTime,hasDecodeFrameCount,renderFrameTime);
    sleepTime = calcWaitTime5(actualCacheCount, renderFrameTime);

    LOGI(
            "queue count = %d ,need cache count= %d ,render time = %d us,frame average interval time = %d us,render need sleep time= %d us",
            actualCacheCount, getCacheFrameCount(), renderFrameTime, aveIntervalTime, sleepTime);
    return sleepTime;
}

int Optimize::calcWaitTime2(int actualCacheCount, int renderFrameTime) {
    int aveIntervalTime = getAverageIntervalTime(); //相邻帧平均间隔时间
    int waitTime = aveIntervalTime - renderFrameTime; //理论上需要等待的时间
    int calcWaitTime = 0;
    int needCacheCount = getCacheFrameCount(); //需要的缓存数
    if (waitTime < 0) { //如果渲染时间大于平均帧间隔时间，则直接返回0
        calcWaitTime = 0;
        return calcWaitTime;
    }

    //实际缓存数大于需要的缓存数情况下，相差越大，则减少的时间越大
    if (actualCacheCount > round(needCacheCount * 10 / 4.0f)) {
        calcWaitTime = 0;
    } else if (actualCacheCount > round(needCacheCount * 9 / 4.0f)) {
        calcWaitTime = waitTime * 3 / 16;
    } else if (actualCacheCount > round(needCacheCount * 7 / 4.0f)) {
        calcWaitTime = waitTime * 7 / 16;
    } else if (actualCacheCount > round(needCacheCount * 5 / 4.0f)) {
        calcWaitTime = waitTime * 12 / 16;
    } else {
        calcWaitTime = waitTime;
    }

    //实际缓存数小于需要的缓存数情况下，相差越大，则增加的时间越大
    if (actualCacheCount < round(needCacheCount / 4.0f)) {
        calcWaitTime = waitTime * 10 / 4;
    } else if (actualCacheCount < round(needCacheCount * 2 / 4.0f)) {
        calcWaitTime = waitTime * 9 / 4;
    } else if (actualCacheCount < round(needCacheCount * 3 / 4.0f)) {
        calcWaitTime = waitTime * 7 / 4;
    } else if (actualCacheCount < needCacheCount) {
        calcWaitTime = waitTime * 5 / 4;
    }

    LOGI("wait delat time = %d us", calcWaitTime - waitTime);

    return calcWaitTime;
}

int Optimize::calcWaitTime1(int actualCacheCount, int renderFrameTime) {
    int aveIntervalTime = getAverageIntervalTime(); //相邻帧平均间隔时间
    int waitTime = aveIntervalTime - renderFrameTime; //理论上需要等待的时间
    int calcWaitTime = 0;
    int needCacheCount = getCacheFrameCount(); //需要的缓存数
    if (waitTime < 0) { //如果渲染时间大于平均帧间隔时间，则直接返回0
        calcWaitTime = 0;
        return calcWaitTime;
    }

    //实际缓存数大于需要的缓存数情况下，相差越大，则减少的时间越大
    if (actualCacheCount > round(needCacheCount * 10 / 4.0f)) {
        calcWaitTime = 0;
    } else if (actualCacheCount > round(needCacheCount * 9 / 4.0f)) {
        calcWaitTime = waitTime / 4;
    } else if (actualCacheCount > round(needCacheCount * 7 / 4.0f)) {
        calcWaitTime = waitTime * 2 / 4;
    } else if (actualCacheCount > round(needCacheCount * 5 / 4.0f)) {
        calcWaitTime = waitTime * 3 / 4;
    } else {
        calcWaitTime = waitTime;
    }

    //实际缓存数小于需要的缓存数情况下，相差越大，则增加的时间越大
    if (actualCacheCount < round(needCacheCount / 4.0f)) {
        calcWaitTime = waitTime * 8 / 4;
    } else if (actualCacheCount < round(needCacheCount * 2 / 4.0f)) {
        calcWaitTime = waitTime * 7 / 4;
    } else if (actualCacheCount < round(needCacheCount * 3 / 4.0f)) {
        calcWaitTime = waitTime * 6 / 4;
    } else if (actualCacheCount < needCacheCount) {
        calcWaitTime = waitTime * 5 / 4;
    }

    LOGI("wait delat time = %d us", calcWaitTime - waitTime);

    return calcWaitTime;
}

int Optimize::calcWaitTime3(int actualCacheCount, int renderFrameTime) {
    int aveIntervalTime = getAverageIntervalTime(); //相邻帧平均间隔时间
    int waitTime = aveIntervalTime - renderFrameTime; //理论上需要等待的时间
    int calcWaitTime = 0;
    if (waitTime < 0) { //如果渲染时间大于平均帧间隔时间，则直接返回0
        calcWaitTime = 0;
        return calcWaitTime;
    }
    calcWaitTime = waitTime - 10000;
    return calcWaitTime;
}

int Optimize::calcWaitTime4(int actualCacheCount, int renderFrameTime) {
    int aveIntervalTime = getAverageIntervalTime(); //相邻帧平均间隔时间
    int waitTime = aveIntervalTime - renderFrameTime; //理论上需要等待的时间
    int needCacheCount = getCacheFrameCount(); //需要的缓存数
    int calcWaitTime = 0;
    if (waitTime < 0) { //如果渲染时间大于平均帧间隔时间，则直接返回0
        calcWaitTime = 0;
        return calcWaitTime;
    }
    //实际缓存数大于需要的缓存数情况下，相差越大，则减少的时间越大
    if (actualCacheCount > round(needCacheCount * 8 / 4.0f)) {
        calcWaitTime = 0;
    } else if (actualCacheCount > round(needCacheCount * 7 / 4.0f)) {
        calcWaitTime = waitTime - 20000;
    } else if (actualCacheCount > round(needCacheCount * 6 / 4.0f)) {
        calcWaitTime = waitTime - 13000;
    } else if (actualCacheCount > round(needCacheCount * 5 / 4.0f)) {
        calcWaitTime = waitTime - 5000;
    } else {
        calcWaitTime = waitTime;
    }

    //实际缓存数小于需要的缓存数情况下，相差越大，则增加的时间越大
    if (actualCacheCount < round(needCacheCount / 4.0f)) {
        calcWaitTime = waitTime + 20000;
    } else if (actualCacheCount < round(needCacheCount * 2 / 4.0f)) {
        calcWaitTime = waitTime + 15000;
    } else if (actualCacheCount < round(needCacheCount * 3 / 4.0f)) {
        calcWaitTime = waitTime + 10000;
    } else if (actualCacheCount < needCacheCount) {
        calcWaitTime = waitTime + 5000;
    }
    if (calcWaitTime < 0) {
        calcWaitTime = 0;
    }

    LOGI("wait delat time = %d us", calcWaitTime - waitTime);
    return calcWaitTime;
}


int Optimize::calcWaitTime5(int actualCacheCount, int renderFrameTime) {
    int aveIntervalTime = getAverageIntervalTime(); //相邻帧平均间隔时间
    int waitTime = aveIntervalTime - renderFrameTime - 10000; //理论上需要等待的时间
    int needCacheCount = getCacheFrameCount(); //需要的缓存数
    int calcWaitTime = 0;
    if (waitTime < 0) { //如果渲染时间大于平均帧间隔时间，则直接返回0
        calcWaitTime = 0;
        return calcWaitTime;
    }
    int m = 10000;//us
    //int delat=(m-actualCacheCount*m/needCacheCount);
    int delat;
    if (actualCacheCount <= 2 * needCacheCount) {
        delat = (m * cosf(M_PI * actualCacheCount / 2.0f / needCacheCount));
    } else {
        delat = (m - actualCacheCount * m / needCacheCount);
    }

    LOGI("wait delat time = %d us", delat);
    calcWaitTime = waitTime + delat;
    if (calcWaitTime < 0) {
        calcWaitTime = 0;
    }
    return calcWaitTime;
}

bool Optimize::isNeedReconnect(int undecodeFrameCount) {
    if (undecodeFrameCount * getAverageIntervalTime() >
        MAX_DELAY_TIME) { //表示已经超过可接受的延时时间，客户端需要做重连机制
        return true;
    }
    return false;
}


int Optimize::getCacheFrameCount() {
    int count = DEFAULT_CACHE_COUNT;
    switch (cacheMode) {
        case CACHE_WITH_TIME:
            if (getAverageIntervalTime() != 0) {
                count = MAX_CACHE_TIME / getAverageIntervalTime();
            }

            break;
        case CACHE_WITH_COUNT:
            count = MAX_CACHE_COUNT;
            break;
    }
    //LOGI("getCacheFrameCount=%d", count);
    return count;
};

