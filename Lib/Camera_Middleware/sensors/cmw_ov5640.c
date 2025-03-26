/**
  ******************************************************************************
  * @file    cmw_ov5640.c
  * @author  MDG Application Team
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "cmw_ov5640.h"
#include "cmw_camera.h"
#include "ov5640_reg.h"
#include "ov5640.h"



static int CMW_OV5640_GetResType(uint32_t width, uint32_t height, uint32_t*res)
{
  if (width == 160 && height == 120)
  {
    *res = OV5640_R160x120;
  }
  else if (width == 320 && height == 240)
  {
    *res = OV5640_R320x240;
  }
  else if (width == 480 && height == 272)
  {
    *res = OV5640_R480x272;
  }
  else if (width == 640 && height == 480)
  {
    *res = OV5640_R640x480;
  }
  else if (width == 800 && height == 480)
  {
    *res = OV5640_R800x480;
  }
  else
  {
    return -1;
  }
  return 0;
}

static int32_t CMW_OV5640_getMirrorFlipConfig(uint32_t Config)
{
  switch (Config) {
    case CMW_MIRRORFLIP_NONE:
      return OV5640_MIRROR_FLIP_NONE;
      break;
    case CMW_MIRRORFLIP_FLIP:
      return OV5640_FLIP;
      break;
    case CMW_MIRRORFLIP_MIRROR:
      return OV5640_MIRROR;
      break;
    case CMW_MIRRORFLIP_FLIP_MIRROR:
      return OV5640_MIRROR_FLIP;
      break;
    default:
      return CMW_ERROR_PERIPH_FAILURE;
  }
}

static int32_t CMW_OV5640_DeInit(void *io_ctx)
{
  int ret = CMW_ERROR_NONE;

  ret = OV5640_DeInit(&((CMW_OV5640_t *)io_ctx)->ctx_driver);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }
  return ret;
}

static int32_t CMW_OV5640_ReadID(void *io_ctx, uint32_t *Id)
{
  return OV5640_ReadID(&((CMW_OV5640_t *)io_ctx)->ctx_driver, Id);
}

static int32_t CMW_OV5640_SetMirrorFlip(void *io_ctx, uint32_t config)
{
  int32_t mirrorFlip = CMW_OV5640_getMirrorFlipConfig(config);
  return OV5640_MirrorFlipConfig(&((CMW_OV5640_t *)io_ctx)->ctx_driver, mirrorFlip);
}

static int32_t CMW_OV5640_Init(void *io_ctx, CMW_Sensor_Init_t *initSensor)
{
  int ret = CMW_ERROR_NONE;
  uint32_t resolution;

  ret = CMW_OV5640_GetResType(initSensor->width, initSensor->height, &resolution);
  if (ret)
  {
    return CMW_ERROR_WRONG_PARAM;
  }

  ret = OV5640_Init(&((CMW_OV5640_t *)io_ctx)->ctx_driver, resolution, initSensor->pixel_format);
  if (ret != OV5640_OK)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = CMW_OV5640_SetMirrorFlip(io_ctx, initSensor->mirrorFlip);
  if (ret)
  {
    return CMW_ERROR_WRONG_PARAM;
  }

  return CMW_ERROR_NONE;
}

static int32_t CMW_OV5640_Start(void *io_ctx)
{
  return OV5640_Start(&((CMW_OV5640_t *)io_ctx)->ctx_driver);
}

static void CMW_OV5640_PowerOn(CMW_OV5640_t *io_ctx)
{
  /* Camera sensor Power-On sequence */
  /* Assert the camera  NRST pins */
  io_ctx->EnablePin(1);
  io_ctx->ShutdownPin(0);  /* Disable MB1723 2V8 signal  */
  io_ctx->Delay(200); /* NRST signals asserted during 200ms */
  /* De-assert the camera STANDBY pin (active high) */
  io_ctx->ShutdownPin(1);  /* Disable MB1723 2V8 signal  */
	io_ctx->Delay(20);     /* NRST de-asserted during 20ms */
}

int CMW_OV5640_Probe(CMW_OV5640_t *io_ctx, CMW_Sensor_if_t *ov5640_if)
{
  int ret = CMW_ERROR_NONE;
  uint32_t id;
  io_ctx->ctx_driver.IO.Address = io_ctx->Address;
  io_ctx->ctx_driver.IO.Init = io_ctx->Init;
  io_ctx->ctx_driver.IO.DeInit = io_ctx->DeInit;
  io_ctx->ctx_driver.IO.GetTick = io_ctx->GetTick;
  io_ctx->ctx_driver.IO.ReadReg = io_ctx->ReadReg;
  io_ctx->ctx_driver.IO.WriteReg = io_ctx->WriteReg;
  io_ctx->ctx_driver.Mode = SERIAL_MODE;
  io_ctx->ctx_driver.VirtualChannelID = 0U;

  CMW_OV5640_PowerOn(io_ctx);

  ret = OV5640_RegisterBusIO(&io_ctx->ctx_driver, &io_ctx->ctx_driver.IO);
  if (ret != OV5640_OK)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = OV5640_ReadID(&io_ctx->ctx_driver, &id);
  if (ret != OV5640_OK)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }
  if (id != OV5640_ID)
  {
      ret = CMW_ERROR_UNKNOWN_COMPONENT;
  }

  memset(ov5640_if, 0, sizeof(*ov5640_if));
  ov5640_if->Init = CMW_OV5640_Init;
  ov5640_if->Start = CMW_OV5640_Start;
  ov5640_if->DeInit = CMW_OV5640_DeInit;
  ov5640_if->ReadID = CMW_OV5640_ReadID;
  ov5640_if->SetMirrorFlip = CMW_OV5640_SetMirrorFlip;
  return ret;
}
