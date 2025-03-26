/**
 ******************************************************************************
 * @file    app_config.h
 * @author  GPM Application Team
 *
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#ifndef APP_CONFIG
#define APP_CONFIG

#define USE_DCACHE

#if defined(USE_IMX335_SENSOR)
  #define CAMERA_WIDTH 2592
  #define CAMERA_HEIGHT 1944

  #define CAPTURE_FORMAT DCMIPP_PIXEL_PACKER_FORMAT_RGB565_1
  #define CAPTURE_BPP 2

  #define CAMERA_FLIP CMW_MIRRORFLIP_NONE
#elif defined(USE_VD66GY_SENSOR)
  #define CAMERA_WIDTH 1120
  #define CAMERA_HEIGHT 720

  #define CAPTURE_FORMAT DCMIPP_PIXEL_PACKER_FORMAT_RGB565_1
  #define CAPTURE_BPP 2

  #define CAMERA_FLIP CMW_MIRRORFLIP_FLIP
#elif defined(USE_OV5640_SENSOR)
  #define CAMERA_WIDTH 800
  #define CAMERA_HEIGHT 480

  #define CAPTURE_FORMAT DCMIPP_PIXEL_PACKER_FORMAT_RGB565_1
  #define CAPTURE_BPP 2
#elif defined(USE_VD55G1_SENSOR)
  #define CAMERA_WIDTH 800
  #define CAMERA_HEIGHT 600

  #define CAPTURE_FORMAT DCMIPP_PIXEL_PACKER_FORMAT_RGB565_1
  #define CAPTURE_BPP 2

  #define CAMERA_FLIP CMW_MIRRORFLIP_FLIP
#elif defined(USE_VD1941_SENSOR)
  #define CAMERA_WIDTH 2560
  #define CAMERA_HEIGHT 1984

  #define CAPTURE_FORMAT DCMIPP_PIXEL_PACKER_FORMAT_RGB565_1
  #define CAPTURE_BPP 2
#else
#error "define sensor"
#endif

#ifndef CAMERA_FLIP
#define CAMERA_FLIP CMW_MIRRORFLIP_NONE
#endif

#define LCD_BG_WIDTH 800
#define LCD_BG_HEIGHT 480
#define LCD_FG_WIDTH 800
#define LCD_FG_HEIGHT 480
#define LCD_FG_FRAMEBUFFER_SIZE (LCD_FG_WIDTH * LCD_FG_HEIGHT * 2)

/* Delay display by DISPLAY_DELAY frame number */
#define DISPLAY_DELAY 1


#define NN_WIDTH 640
#define NN_HEIGHT 640
#define NN_FORMAT DCMIPP_PIXEL_PACKER_FORMAT_RGB888_YUV444_1
#define NN_BPP 3

#endif
