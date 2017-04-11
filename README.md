# 实时流媒体播放器RtspPlayer

一款基于FFmpeg的支持多路的实时流媒体播放器
---
## 环境
* Windows 10
* 基于FFmpeg3.2.2(ndk-r10e编译)
* AndroidStudio2.2.3

---
 至于编译FFMPEG的细节详看：
### [windows10下编译ffmpeg for android](http://blog.csdn.net/huweijian5/article/details/52840177)

 在androidstudio上使用的细节详看：
### [在androidstudio2.2上使用ffmpeg动态库](http://blog.csdn.net/huweijian5/article/details/52883969)

 搭建简单的测试服务
### [利用live555搭建最简单的流媒体服务](http://blog.csdn.net/huweijian5/article/details/53928521)

音视频相关知识
### [音视频相关知识记录](http://blog.csdn.net/huweijian5/article/details/54099197)

---
![一路视频](https://github.com/huweijian5/RtspPlayer/blob/master/screenshots/snapshot1.png)

![一路视频](https://github.com/huweijian5/RtspPlayer/blob/master/screenshots/snapshot3.png)

![多路视频](https://github.com/huweijian5/RtspPlayer/blob/master/screenshots/snapshot2.gif)
---
## 使用说明
* 本工程最主要的作用是作为demo供大家参考，里面对于点流，解码，渲染等都作了一定封装，是可以参考使用的
* 增加了NdkMediaCodec用来硬解码，使得ANDROID_PLATFORM达到了21，如果想在低于21的编译通过，请直接注释掉NativeCodec相关代码即可
待写...

