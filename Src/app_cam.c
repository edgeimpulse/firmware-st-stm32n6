 /**
 ******************************************************************************
 * @file    app_cam.c
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
#include <assert.h>
#include "cmw_camera.h"
#include "app_cam.h"
#include "app_config.h"
#include "utils.h"

static int CAM_GetDecimationRatio(float ratio)
{
  int dec_ratio = 1;

  assert(ratio <= 64);
  while (ratio >= 8) {
    dec_ratio *= 2;
    ratio /= 2;
  }

  return dec_ratio;
}

/* Keep display output aspect ratio using crop area */
static void CAM_InitCropConfig(CMW_Manual_Configuration_t *conf)
{
  const float ratiox = (float)CAMERA_WIDTH / LCD_BG_WIDTH;
  const float ratioy = (float)CAMERA_HEIGHT / LCD_BG_HEIGHT;
  const float ratio = MIN(ratiox, ratioy);
  CMW_Manual_Crop_t *crop = &conf->crop;

  assert(ratio >= 1);
  assert(ratio < 64);

  crop->width = (uint32_t) MIN(LCD_BG_WIDTH * ratio, CAMERA_WIDTH);
  crop->height = (uint32_t) MIN(LCD_BG_HEIGHT * ratio, CAMERA_HEIGHT);
  crop->offset_x = (CAMERA_WIDTH - crop->width + 1) / 2;
  crop->offset_y = (CAMERA_HEIGHT - crop->height + 1) / 2;
}

static void CAM_InitDecimationConfig(CMW_Manual_Configuration_t *conf)
{
  float ratio_x = (float)conf->crop.width / conf->downsize.width;
  float ratio_y = (float)conf->crop.height / conf->downsize.height;

  conf->decimation.horizontal_ratio = CAM_GetDecimationRatio(ratio_x);
  conf->decimation.vertical_ratio = CAM_GetDecimationRatio(ratio_y);
}

static void CAM_InitDisplayManualConf(CMW_Manual_Configuration_t *conf)
{
  conf->downsize.width = LCD_BG_WIDTH;
  conf->downsize.height = LCD_BG_HEIGHT;
  CAM_InitCropConfig(conf);
  CAM_InitDecimationConfig(conf);
}

static void CAM_InitNnManualConf(CMW_Manual_Configuration_t *conf)
{
  conf->downsize.width = NN_WIDTH;
  conf->downsize.height = NN_HEIGHT;
  CAM_InitCropConfig(conf);
  CAM_InitDecimationConfig(conf);
}

static void DCMIPP_PipeInitDisplay()
{
  DCMIPP_Conf_t dcmipp_conf;
  int ret;

  assert(LCD_BG_WIDTH >= LCD_BG_HEIGHT);

  dcmipp_conf.output_width = LCD_BG_WIDTH;
  dcmipp_conf.output_height = LCD_BG_HEIGHT;
  dcmipp_conf.output_format = CAPTURE_FORMAT;
  dcmipp_conf.output_bpp = CAPTURE_BPP;
  dcmipp_conf.mode = CAM_Aspect_ratio_manual;
  dcmipp_conf.enable_swap = 0;
  dcmipp_conf.enable_gamma_conversion = 0;
  CAM_InitDisplayManualConf(&dcmipp_conf.manual_conf);
  ret = CMW_CAMERA_SetPipeConfig(DCMIPP_PIPE1, &dcmipp_conf);
  assert(ret == HAL_OK);
}

static void DCMIPP_PipeInitNn()
{
  DCMIPP_Conf_t dcmipp_conf;
  int ret;

  dcmipp_conf.output_width = NN_WIDTH,
  dcmipp_conf.output_height = NN_HEIGHT,
  dcmipp_conf.output_format = NN_FORMAT,
  dcmipp_conf.output_bpp = NN_BPP,
  dcmipp_conf.mode = CAM_Aspect_ratio_manual;
  dcmipp_conf.enable_swap = 1;
  dcmipp_conf.enable_gamma_conversion = 0;
  CAM_InitNnManualConf(&dcmipp_conf.manual_conf);
  ret = CMW_CAMERA_SetPipeConfig(DCMIPP_PIPE2, &dcmipp_conf);
  assert(ret == HAL_OK);
}

void CAM_ei_PipeInitNn(int width, int height)
{
  DCMIPP_Conf_t dcmipp_conf;
  int ret;

  dcmipp_conf.output_width = width,
  dcmipp_conf.output_height = height,
  dcmipp_conf.output_format = NN_FORMAT,
  dcmipp_conf.output_bpp = NN_BPP,
  dcmipp_conf.mode = CAM_Aspect_ratio_manual;
  dcmipp_conf.enable_swap = 1;
  dcmipp_conf.enable_gamma_conversion = 0;
  CAM_InitNnManualConf(&dcmipp_conf.manual_conf);
  ret = CMW_CAMERA_SetPipeConfig(DCMIPP_PIPE2, &dcmipp_conf);
  assert(ret == HAL_OK);
}

static uint8_t camera_buffer[NN_WIDTH * NN_HEIGHT * NN_BPP + 1024] ALIGN_32 IN_PSRAM;
uint8_t *CAM_ei_capture_frame(void)
{
    extern volatile int cameraFrameReceived;
    CAM_IspUpdate();

    /* Start NN camera single capture Snapshot */
    CAM_NNPipe_Start((uint8_t *)camera_buffer, CAMERA_MODE_SNAPSHOT);

    while (cameraFrameReceived == 0) {};
    cameraFrameReceived = 0;

    return camera_buffer;
}

void CAM_Init(void)
{
  CMW_CameraInit_t cam_conf;
  int ret;

  cam_conf.width = CAMERA_WIDTH;
  cam_conf.height = CAMERA_HEIGHT;
  cam_conf.fps = CAMERA_FPS;
  cam_conf.pixel_format = 0; /* Default; Not implemented yet */
  cam_conf.anti_flicker = 0;
  cam_conf.mirror_flip = CAMERA_FLIP;

  ret = CMW_CAMERA_Init(&cam_conf);
  assert(ret == CMW_ERROR_NONE);

  DCMIPP_PipeInitDisplay();
  DCMIPP_PipeInitNn();
}

void CAM_DisplayPipe_Start(uint8_t *display_pipe_dst, uint32_t cam_mode)
{
  int ret;

  ret = CMW_CAMERA_Start(DCMIPP_PIPE1, display_pipe_dst, cam_mode);
  assert(ret == CMW_ERROR_NONE);
}

void CAM_NNPipe_Start(uint8_t *nn_pipe_dst, uint32_t cam_mode)
{
  int ret;

  ret = CMW_CAMERA_Start(DCMIPP_PIPE2, nn_pipe_dst, cam_mode);
  assert(ret == CMW_ERROR_NONE);
}

void CAM_IspUpdate(void)
{
  int ret;

  ret = CMW_CAMERA_Run();
  assert(ret == CMW_ERROR_NONE);
}
