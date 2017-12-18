此库依赖ffmpeg3(用于解码和转换)和jpegturbo（用于截图）相关so

player目录下主要包含了点流器(StreamTaker)，视频解码器(Codec)，YUV渲染器(YUV420PRenderer)，
GRB渲染器(RGBRenderer)，音频播放器(AudioPlayer)，格式转换器(Convertor),优化器（Optimize）
以及对视频解码器，GRB渲染器，音频播放器，格式转换器进行组合封装的RGBHandler(实现简单)和YUVHandler(效率更高),只要往其填充原始帧即可实现视频播放

demo中MediaPlayer.cpp是基于player目录下相关库实现了点流到解码到渲染的完整过程
MediaPlayer2.cpp则是使用了RGBHandler实现了点流到解码到渲染的完整过程
