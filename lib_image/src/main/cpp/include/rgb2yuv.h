/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef ORG_TENSORFLOW_JNI_IMAGEUTILS_RGB2YUV_H_
#define ORG_TENSORFLOW_JNI_IMAGEUTILS_RGB2YUV_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
   * Converts 32-bit ARGB8888 image data to YUV420SP data.  This is useful, for
   * instance, in creating data to feed the classes that rely on raw camera
   * preview frames.
   *
   * @param input An array of input pixels in ARGB8888 format.
   * @param output A pre-allocated array for the YUV420SP output data.
   * @param width The width of the input image.
   * @param height The height of the input image.
   */
void ConvertARGB8888ToYUV420SP(const uint32_t *const input,
                               uint8_t *const output, int width, int height);
/**
   * Converts 16-bit RGB565 image data to YUV420SP data.  This is useful, for
   * instance, in creating data to feed the classes that rely on raw camera
   * preview frames.
   *
   * @param input An array of input pixels in RGB565 format.
   * @param output A pre-allocated array for the YUV420SP output data.
   * @param width The width of the input image.
   * @param height The height of the input image.
   */
void ConvertRGB565ToYUV420SP(const uint16_t *const input, uint8_t *const output,
                             const int width, const int height);

#ifdef __cplusplus
}
#endif

#endif  // ORG_TENSORFLOW_JNI_IMAGEUTILS_RGB2YUV_H_
