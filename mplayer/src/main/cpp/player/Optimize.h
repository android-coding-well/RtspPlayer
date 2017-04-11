//
// 视频渲染优化处理器
// 时间单位为微秒（us）
// Created by hwj on 2017/1/22.
//

#ifndef SAFETY1_1_0_OPTIMIZE_H
#define SAFETY1_1_0_OPTIMIZE_H
typedef enum {
    CACHE_WITH_TIME, //根据缓存时间动态计算缓存数量
            CACHE_WITH_COUNT//直接指定缓存数量
} CacheMode;

class Optimize {
public :
    Optimize();

    virtual ~Optimize();

    //重置相关参数
    void reset();

    //*********************************参数设置*************************************
    //设置缓存模式,默认为CACHE_WITH_TIME
    //mode:缓存模式
    //param:CACHE_WITH_TIME--设置缓存时间，单位us小于等于0则默认为1000000us；CACHE_WITH_COUNT--设置缓存数量，小于等于0则默认为10
    void setCacheMode(CacheMode mode, int param);

    //*********************************数据输入*************************************
    //在每次收到视频帧时都调用
    //在此函数中会计算相邻帧平均间隔时间
    void receiveFrame();


    //*********************************数据输出*************************************
    //获得相邻帧之间的平均间隔时间,us
    int getAverageIntervalTime();

    //计算渲染等待时间
    //actualCacheCount:实际已经缓存的帧数
    //renderFrameTime:渲染一帧所用时间us
    int getRenderWaitTime(int actualCacheCount, int renderFrameTime);

    //计算要缓存的帧数,会根据设置的CacheMode返回对应的帧数
    int getCacheFrameCount();

    //是否需要重连，需要重连表示延时已超过MAX_DELAY_TIME
    //undecodeFrameCount:队列中未解码的帧数
    bool isNeedReconnect(int undecodeFrameCount);

public :
    //最大延时时间（us），如果超过这个则客户端需要发起重连重新请求流
    const int MAX_DELAY_TIME = 5000000;

    //默认缓存帧数,在MAX_CACHE_TIME失效或MAX_CACHE_TIME未设置的情况下
    const int DEFAULT_CACHE_COUNT = 10;

    //最大缓存时间（us）,根据这个时间计算需要缓存的帧数
    int MAX_CACHE_TIME = 800000;

    //用户自定义缓存数
    int MAX_CACHE_COUNT;
private :
    //计算渲染等待时间算法1
    //actualCacheCount:实际已经缓存的帧数
    //renderFrameTime:渲染一帧所用时间us
    int calcWaitTime1(int actualCacheCount, int renderFrameTime);//实际缓存帧大概是需要的2倍

    int calcWaitTime2(int actualCacheCount, int renderFrameTime);

    //
    int calcWaitTime3(int actualCacheCount, int renderFrameTime);

    //无增减，会导致缓存 越来越多
    int calcWaitTime4(int actualCacheCount, int renderFrameTime);

    //曲线增减+梯级增减
    int calcWaitTime5(int actualCacheCount, int renderFrameTime);
private:

    //前一帧的时间，us
    unsigned long preFrameTime;
    //总的间隔时间.us
    unsigned long long sumIntervalTime;
    //间隔个数
    int intervalCount;

    //平均间隔时间，us
    int averageIntervalTime;

    //未解码队列中的个数(实时设置)
    int undecodeFrameQueueSize;
    //已经解码队列中的个数(实时设置)
    int hasDecodeFrameQueueSize;

    //渲染一帧的时间(实时设置)，us
    int renderFrameTime;

    CacheMode cacheMode;
};


#endif //SAFETY1_1_0_OPTIMIZE_H
