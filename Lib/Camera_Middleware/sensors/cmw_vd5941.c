/**
  ******************************************************************************
  * @file    vd5941_bsp.c
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

#include "cmw_vd5941.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "cmw_camera.h"
#include "vdx941.h"

#define container_of(ptr, type, member) (type *) ((unsigned char *)ptr - offsetof(type,member))

#define MIN(a, b)       ((a) < (b) ?  (a) : (b))

#define VD5941_REG_MODEL_ID                           0x0000

static int CMW_VD5941_Read8(CMW_VD5941_t *pObj, uint16_t addr, uint8_t *value)
{
  return pObj->ReadReg(pObj->Address, addr, value, 1);
}

static int CMW_VD5941_Read16(CMW_VD5941_t *pObj, uint16_t addr, uint16_t *value)
{
  uint8_t data[2];
  int ret;

  ret = pObj->ReadReg(pObj->Address, addr, data, 2);
  if (ret)
    return ret;

  *value = (data[1] << 8) | data[0];

  return CMW_ERROR_NONE;
}

static int CMW_VD5941_Read32(CMW_VD5941_t *pObj, uint16_t addr, uint32_t *value)
{
  uint8_t data[4];
  int ret;

  ret = pObj->ReadReg(pObj->Address, addr, data, 4);
  if (ret)
    return ret;

  *value = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];

  return 0;
}

static int CMW_VD5941_Write8(CMW_VD5941_t *pObj, uint16_t addr, uint8_t value)
{
  return pObj->WriteReg(pObj->Address, addr, &value, 1);
}

static int CMW_VD5941_Write16(CMW_VD5941_t *pObj, uint16_t addr, uint16_t value)
{
  return pObj->WriteReg(pObj->Address, addr, (uint8_t *) &value, 2);
}

static int CMW_VD5941_Write32(CMW_VD5941_t *pObj, uint16_t addr, uint32_t value)
{
  return pObj->WriteReg(pObj->Address, addr, (uint8_t *) &value, 4);
}

static void VD5941_ShutdownPin(struct VDx941_Ctx *ctx, int value)
{
  CMW_VD5941_t *p_ctx = container_of(ctx, CMW_VD5941_t, ctx_driver);

  p_ctx->ShutdownPin(value);
}

static int VD5941_Read8(struct VDx941_Ctx *ctx, uint16_t addr, uint8_t *value)
{
  CMW_VD5941_t *p_ctx = container_of(ctx, CMW_VD5941_t, ctx_driver);

  return CMW_VD5941_Read8(p_ctx, addr, value);
}

static int VD5941_Read16(struct VDx941_Ctx *ctx, uint16_t addr, uint16_t *value)
{
  CMW_VD5941_t *p_ctx = container_of(ctx, CMW_VD5941_t, ctx_driver);

  return CMW_VD5941_Read16(p_ctx, addr, value);
}

static int VD5941_Read32(struct VDx941_Ctx *ctx, uint16_t addr, uint32_t *value)
{
  CMW_VD5941_t *p_ctx = container_of(ctx, CMW_VD5941_t, ctx_driver);

  return CMW_VD5941_Read32(p_ctx, addr, value);
}

static int VD5941_Write8(struct VDx941_Ctx *ctx, uint16_t addr, uint8_t value)
{
  CMW_VD5941_t *p_ctx = container_of(ctx, CMW_VD5941_t, ctx_driver);

  return CMW_VD5941_Write8(p_ctx, addr, value);
}

static int VD5941_Write16(struct VDx941_Ctx *ctx, uint16_t addr, uint16_t value)
{
  CMW_VD5941_t *p_ctx = container_of(ctx, CMW_VD5941_t, ctx_driver);

  return CMW_VD5941_Write16(p_ctx, addr, value);
}

static int VD5941_Write32(struct VDx941_Ctx *ctx, uint16_t addr, uint32_t value)
{
  CMW_VD5941_t *p_ctx = container_of(ctx, CMW_VD5941_t, ctx_driver);

  return CMW_VD5941_Write32(p_ctx, addr, value);
}

static int VD5941_WriteArray(struct VDx941_Ctx *ctx, uint16_t addr, uint8_t *data, int data_len)
{
  CMW_VD5941_t *p_ctx = container_of(ctx, CMW_VD5941_t, ctx_driver);
  const unsigned int chunk_size = 128;
  uint16_t sz;
  int ret;

  while (data_len) {
    sz = MIN(data_len, chunk_size);
    ret = p_ctx->WriteReg(p_ctx->Address, addr, data, sz);
    if (ret)
      return ret;
    data_len -= sz;
    addr += sz;
    data += sz;
  }

  return 0;
}

static void VD5941_Delay(struct VDx941_Ctx *ctx, uint32_t delay_in_ms)
{
  CMW_VD5941_t *p_ctx = container_of(ctx, CMW_VD5941_t, ctx_driver);

  p_ctx->Delay(delay_in_ms);
}

static void VD5941_Log(struct VDx941_Ctx *ctx, int lvl, const char *format, va_list ap)
{
#if 1
  const int current_lvl = VDx941_LVL_DBG(2);

  if (lvl > current_lvl)
    return ;

  vprintf(format, ap);
#endif
}

static int CMW_VD5941_GetResType(uint32_t width, uint32_t height, VDx941_Res_t *res)
{
  if (width == 320 && height == 240)
  {
    *res = VDx941_RES_QVGA_320_240;
  }
  else if (width == 640 && height == 480)
  {
    *res = VDx941_RES_VGA_640_480;
  }
  else if (width == 800 && height == 600)
  {
    *res = VDx941_RES_SVGA_800_600;
  }
  else if (width == 1280 && height == 720)
  {
    *res = VDx941_RES_720P_1280_720;
  }
  else if (width == 1920 && height == 1080)
  {
    *res = VDx941_RES_1080P_1920_1080;
  }
  else if (width == 2560 && height == 1984)
  {
    *res = VDx941_RES_FULL_2560_1984;
  }
  else
  {
    return CMW_ERROR_WRONG_PARAM;
  }
  return 0;
}

static int32_t CMW_VD5941_getMirrorFlipConfig(uint32_t Config)
{
  switch (Config) {
    case CMW_MIRRORFLIP_NONE:
      return VDx941_MIRROR_FLIP_NONE;
      break;
    case CMW_MIRRORFLIP_FLIP:
      return VDx941_FLIP;
      break;
    case CMW_MIRRORFLIP_MIRROR:
      return VDx941_MIRROR;
      break;
    case CMW_MIRRORFLIP_FLIP_MIRROR:
      return VDx941_MIRROR_FLIP;
      break;
    default:
      return CMW_ERROR_PERIPH_FAILURE;
  }
}

static int32_t CMW_VD5941_Init(void *io_ctx, CMW_Sensor_Init_t *initSensor)
{
  VDx941_Config_t config = { 0 };
  int ret;
  int i;

  if (((CMW_VD5941_t *)io_ctx)->IsInitialized)
  {
    return CMW_ERROR_NONE;
  }

  ret = CMW_VD5941_GetResType(initSensor->width, initSensor->height, &config.resolution);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }
  config.image_processing_mode = VDx941_GS_SS1_NATIVE_8;
  config.ext_clock_freq_in_hz = 25000000;
  config.flip_mirror_mode = CMW_VD5941_getMirrorFlipConfig(initSensor->mirrorFlip);
  config.patgen = VDx941_PATGEN_DISABLE;
  /* setup csi2 itf */
  config.out_itf.data_rate_in_mps = VDx941_DEFAULT_DATARATE;
  config.out_itf.datalane_nb = 2;
  config.out_itf.logic_lane_mapping[0] = 0;
  config.out_itf.logic_lane_mapping[1] = 1;
  config.out_itf.logic_lane_mapping[2] = 2;
  config.out_itf.logic_lane_mapping[3] = 3;
  config.out_itf.clock_lane_swap_enable = 1;
  config.out_itf.physical_lane_swap_enable[0] = 1;
  config.out_itf.physical_lane_swap_enable[1] = 1;
  /* gpios as input */
  for (i = 0; i < VDx941_GPIO_NB; i++)
  {
    config.gpios[i].gpio_ctrl = VDx941_GPIO_IN;
    config.gpios[i].enable = 0;
  }
  /* Default VT mode*/
  config.sync_mode = VDx941_MASTER;

  ret = VDx941_Init(&((CMW_VD5941_t *)io_ctx)->ctx_driver, &config);
  if (ret)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  /* This gain / expo combo give correct starting point for 50 Hz flicker free and correct
   * illumination in desk environment.
   */
  ret = VDx941_SetDigitalGain(&((CMW_VD5941_t *)io_ctx)->ctx_driver, 0x100);
  if (ret)
  {
     return CMW_ERROR_COMPONENT_FAILURE;
  }
  ret = VDx941_SetExpo(&((CMW_VD5941_t *)io_ctx)->ctx_driver, 10000);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ((CMW_VD5941_t *)io_ctx)->IsInitialized = 1;

  return CMW_ERROR_NONE;
}

static int32_t CMW_VD5941_Start(void *io_ctx)
{
  int ret = CMW_ERROR_NONE;
  ret = VDx941_Start(&((CMW_VD5941_t *)io_ctx)->ctx_driver);
  if (ret) {
    VDx941_DeInit(&((CMW_VD5941_t *)io_ctx)->ctx_driver);
    return CMW_ERROR_PERIPH_FAILURE;
  }
  return CMW_ERROR_NONE;
}


static int32_t CMW_VD5941_Stop(void *io_ctx)
{
  int ret = CMW_ERROR_NONE;

  ret = VDx941_Stop(&((CMW_VD5941_t *)io_ctx)->ctx_driver);
  if (ret)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }
  return CMW_ERROR_NONE;
}

static int32_t CMW_VD5941_DeInit(void *io_ctx)
{
  int ret = CMW_ERROR_NONE;

  ret = VDx941_Stop(&((CMW_VD5941_t *)io_ctx)->ctx_driver);
  if (ret)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  ret = VDx941_DeInit(&((CMW_VD5941_t *)io_ctx)->ctx_driver);
  if (ret)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  ((CMW_VD5941_t *)io_ctx)->IsInitialized = 0;
  return CMW_ERROR_NONE;
}


int32_t VD5941_RegisterBusIO(CMW_VD5941_t *io_ctx)
{
  int ret;

  if (!io_ctx)
    return CMW_ERROR_COMPONENT_FAILURE;

  if (!io_ctx->Init)
    return CMW_ERROR_COMPONENT_FAILURE;

  ret = io_ctx->Init();

  return ret;
}

int32_t VD5941_ReadID(CMW_VD5941_t *io_ctx, uint32_t *Id)
{
  uint32_t reg32;
  int32_t ret;

  ret = CMW_VD5941_Read32(io_ctx, VD5941_REG_MODEL_ID, &reg32);
  if (ret)
    return ret;

  *Id = reg32;

  return CMW_ERROR_NONE;
}

static void CMW_VD5941_PowerOn(CMW_VD5941_t *io_ctx)
{
  /* Camera sensor Power-On sequence */
  /* Assert the camera  NRST pins */
  io_ctx->EnablePin(1);
  io_ctx->ShutdownPin(0);  /* Disable MB1723 2V8 signal  */
  HAL_Delay(200);   /* NRST signals asserted during 200ms */
  /* De-assert the camera STANDBY pin (active high) */
  io_ctx->ShutdownPin(1);  /* Disable MB1723 2V8 signal  */
	HAL_Delay(20);     /* NRST de-asserted during 20ms */
}

int CMW_VD5941_Probe(CMW_VD5941_t *io_ctx, CMW_Sensor_if_t *vd5941_if)
{
  int ret = CMW_ERROR_NONE;
  uint32_t id;

  io_ctx->ctx_driver.shutdown_pin = VD5941_ShutdownPin;
  io_ctx->ctx_driver.read8 = VD5941_Read8;
  io_ctx->ctx_driver.read16 = VD5941_Read16;
  io_ctx->ctx_driver.read32 = VD5941_Read32;
  io_ctx->ctx_driver.write8 = VD5941_Write8;
  io_ctx->ctx_driver.write16 = VD5941_Write16;
  io_ctx->ctx_driver.write32 = VD5941_Write32;
  io_ctx->ctx_driver.write_array = VD5941_WriteArray;
  io_ctx->ctx_driver.delay = VD5941_Delay;
  io_ctx->ctx_driver.log = VD5941_Log;

  CMW_VD5941_PowerOn(io_ctx);

  ret = VD5941_RegisterBusIO(io_ctx);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = VD5941_ReadID(io_ctx, &id);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }
  if (id != VD5941_CHIP_ID)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  memset(vd5941_if, 0, sizeof(*vd5941_if));
  vd5941_if->Init = CMW_VD5941_Init;
  vd5941_if->DeInit = CMW_VD5941_DeInit;
  vd5941_if->Start = CMW_VD5941_Start;
  vd5941_if->Stop = CMW_VD5941_Stop;

  return ret;
}
