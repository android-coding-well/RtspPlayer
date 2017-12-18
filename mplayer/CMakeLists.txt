# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)


# 定义源文件目录
get_filename_component(CPP_SRC_DIR  ${CMAKE_SOURCE_DIR}/src/main/cpp  ABSOLUTE)
# 定义源文件目录下的源文件
file(GLOB_RECURSE cpp_sources ${CPP_SRC_DIR}/*.*)

if (ANDROID_ABI MATCHES "^armeabi-v7a$")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfloat-abi=softfp -mfpu=neon")
elseif(ANDROID_ABI MATCHES "^arm64-v8a")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -ftree-vectorize")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSTANDALONE_DEMO_LIB \
                    -std=c++11 -fno-exceptions -fno-rtti -O2 -Wno-narrowing \
                    -fPIE")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} \
                              -Wl,--allow-multiple-definition \
                              -Wl,--whole-archive -fPIE -v")




# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             streamhandler

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
              ${cpp_sources}
             )


#Add other prebuilt libraries
 add_library(avcodec-lib  SHARED IMPORTED )
 set_target_properties(avcodec-lib PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libavcodec.so )
 add_library(avdevice-lib  SHARED IMPORTED )
 set_target_properties(avdevice-lib PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libavdevice.so )
 add_library(avfilter-lib  SHARED IMPORTED )
 set_target_properties(avfilter-lib PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libavfilter.so )
 add_library(avformat-lib  SHARED IMPORTED )
 set_target_properties(avformat-lib PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libavformat.so )
 add_library(avutil-lib  SHARED IMPORTED )
 set_target_properties(avutil-lib PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libavutil.so )
 add_library(swresample-lib  SHARED IMPORTED )
 set_target_properties(swresample-lib PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libswresample.so )
 add_library(swscale-lib  SHARED IMPORTED )
 set_target_properties(swscale-lib PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libswscale.so )

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
                       streamhandler
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
                       mediandk#硬解码

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

#升级到gradle4之后会报错误：More than one file was found with OS independent path 'lib/armeabi/libstreamhandler.so'
#解决办法有两个：一是删除jniLibs/armeabi/libstreamhandler.so,同时注释掉下面生成so输出路径的语句即可
#二是在当前build.gradle中添加 android{ packagingOptions { pickFirst 'lib/armeabi/libstreamhandler.so' }}

#在指定目录生成so文件，注意目录区分大小写，如jniLibs_DIR的“jniLibs”必须和后面build.gradle指定的sourceSet目录中指定的“jniLibs”完全一致
set(jniLibs_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs)
set_target_properties( streamhandler
                             PROPERTIES
                             LIBRARY_OUTPUT_DIRECTORY
                             "${jniLibs_DIR}/${ANDROID_ABI}")