# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             src/main/cpp/native-lib.cpp
             src/main/cpp/player/MediaPlayer.cpp
             src/main/cpp/player/YUV420PRenderer.cpp
             src/main/cpp/player/RetCode.cpp
             src/main/cpp/player/Codec.cpp
             src/main/cpp/common/Jpeg.cpp
             src/main/cpp/player/StreamTaker.cpp
             src/main/cpp/player/AudioPlayer.cpp
             src/main/cpp/player/Convertor.cpp
             src/main/cpp/player/RGBRenderer.cpp
             src/main/cpp/common/GThread.cpp)


#Add other prebuilt libraries
 add_library(avcodec-lib  SHARED IMPORTED )
 set_target_properties(avcodec-lib PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libavcodec-57.so )
 add_library(avdevice-lib  SHARED IMPORTED )
 set_target_properties(avdevice-lib PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libavdevice-57.so )
 add_library(avfilter-lib  SHARED IMPORTED )
 set_target_properties(avfilter-lib PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libavfilter-6.so )
 add_library(avformat-lib  SHARED IMPORTED )
 set_target_properties(avformat-lib PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libavformat-57.so )
 add_library(avutil-lib  SHARED IMPORTED )
 set_target_properties(avutil-lib PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libavutil-55.so )
 add_library(swresample-lib  SHARED IMPORTED )
 set_target_properties(swresample-lib PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libswresample-2.so )
 add_library(swscale-lib  SHARED IMPORTED )
 set_target_properties(swscale-lib PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libswscale-4.so )

add_library(jpeg-lib  SHARED IMPORTED )
 set_target_properties(jpeg-lib PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libjpeg.so)
#add_library(turbojpeg-lib  SHARED IMPORTED )
 #set_target_properties(turbojpeg-lib PROPERTIES IMPORTED_LOCATION
 #                      ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libturbojpeg.so)

# Specifies a path to native header files.
include_directories(src/main/cpp/include/)
include_directories(src/main/cpp/ffmpeg3/include/)
include_directories(src/main/cpp/libjpegturbo/include/)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib
                       avcodec-lib
                       avdevice-lib
                       avfilter-lib
                       avformat-lib
                       avutil-lib
                       swresample-lib
                       swscale-lib
                       jpeg-lib
                       android#使用ANativeWindow_fromSurface将surface转化为ANativeWindow,该接口的声明在android/native_window_jni.h中，实现在libandroid.so(位置：ndk-bundle\platforms\android-18\arch-arm\usr\lib)中
                       GLESv2#渲染时使用
                       OpenSLES#音频使用

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

