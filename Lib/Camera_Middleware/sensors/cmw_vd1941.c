/**
  ******************************************************************************
  * @file    vd1941_bsp.c
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

#include "cmw_vd1941.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "cmw_camera.h"
#include "vdx941.h"

#define container_of(ptr, type, member) (type *) ((unsigned char *)ptr - offsetof(type,member))

#define MIN(a, b)       ((a) < (b) ?  (a) : (b))

#define VD1941_REG_MODEL_ID                           0x0000

typedef struct {
  float white_balance[3];
  float color_matrix[3][3];
  const char *name;
} wb_info_t;

/* Coeff from fox .... */
 // @Todo Move to app side?
static wb_info_t wbs[] = {
  {
    {1.0, 1.069786404594564, 2.495838300616807},
    {
      {1.4529990199036098,-0.3223384849485778,-0.13063260030360893},
      {-0.34023658845734966,1.5474834806501134,-0.20721309668531973},
      {0.03170546883248714,-0.6476149970433441,1.615492554913695},
    },
    "TL84"
  },
};

const static wb_info_t *wb = &wbs[0];

static int float2expo(float v, uint8_t *shift, uint8_t *multiply)
{
  int shift_tmp = 0;

  if (v >= 256)
    return -1;

  while (v >= 2) {
    shift_tmp++;
    v = v / 2;
  }

  *shift = shift_tmp;
  *multiply = (int) (v * 128);

  return 0;
}

static int VD1941_getBayerPattern(int mirror_settings)
{
  switch (mirror_settings) {
  case CMW_MIRRORFLIP_NONE:
    return DCMIPP_RAWBAYER_GBRG;
    break;
  case CMW_MIRRORFLIP_FLIP:
    return DCMIPP_RAWBAYER_RGGB;
    break;
  case CMW_MIRRORFLIP_MIRROR:
    return DCMIPP_RAWBAYER_BGGR;
    break;
    case CMW_MIRRORFLIP_FLIP_MIRROR:
      return DCMIPP_RAWBAYER_GBRG;
    break;
  default:
    return DCMIPP_RAWBAYER_GBRG;
  }
}

static int DCMIPP_PipeBayerInit(DCMIPP_HandleTypeDef *hdcmipp, uint32_t mirrorFlip)
{
  DCMIPP_RawBayer2RGBConfTypeDef demosac_conf = { 0 };
  int ret = CMW_ERROR_NONE;

  demosac_conf.RawBayerType = VD1941_getBayerPattern(mirrorFlip);
  demosac_conf.VLineStrength = DCMIPP_RAWBAYER_ALGO_STRENGTH_4;
  demosac_conf.HLineStrength = DCMIPP_RAWBAYER_ALGO_STRENGTH_4;
  demosac_conf.PeakStrength = DCMIPP_RAWBAYER_ALGO_STRENGTH_3;
  demosac_conf.EdgeStrength = DCMIPP_RAWBAYER_ALGO_STRENGTH_6;
  ret = HAL_DCMIPP_PIPE_SetISPRawBayer2RGBConfig(hdcmipp, DCMIPP_PIPE1, &demosac_conf);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = HAL_DCMIPP_PIPE_EnableISPRawBayer2RGB(hdcmipp, DCMIPP_PIPE1);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }
  return ret;
}

static int DCMIPP_PipeWhiteBalanceInit(DCMIPP_HandleTypeDef *hdcmipp, const float coeffs[3])
{
  DCMIPP_ExposureConfTypeDef exposure;
  int ret = CMW_ERROR_NONE;

  if (!coeffs)
  {
    return CMW_ERROR_WRONG_PARAM;
  }

  ret = float2expo(coeffs[0], &exposure.ShiftRed, &exposure.MultiplierRed);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }
  ret = float2expo(coeffs[1], &exposure.ShiftGreen, &exposure.MultiplierGreen);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }
  ret = float2expo(coeffs[2], &exposure.ShiftBlue, &exposure.MultiplierBlue);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = HAL_DCMIPP_PIPE_SetISPExposureConfig(hdcmipp, DCMIPP_PIPE1, &exposure);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = HAL_DCMIPP_PIPE_EnableISPExposure(hdcmipp, DCMIPP_PIPE1);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  return ret;
}

static int DCMIPP_PipeColorInit(DCMIPP_HandleTypeDef *hdcmipp, const float in_matrix[3][3])
{
  DCMIPP_ColorConversionConfTypeDef color_conv = { 0 };
  uint16_t matrix[3][3];
  int r, c;
  int ret = CMW_ERROR_NONE;

  if (!in_matrix)
  {
    return CMW_ERROR_WRONG_PARAM;
  }

  for (r = 0; r < 3; r++) {
    for (c = 0; c < 3; c++) {
      uint16_t val;

      if (in_matrix[r][c] > 4)
      {
        return CMW_ERROR_COMPONENT_FAILURE;
      }
      if (in_matrix[r][c] < -4)
      {
        return CMW_ERROR_COMPONENT_FAILURE;
      }

      val = (int) (256 * fabs(in_matrix[r][c]));
      if (in_matrix[r][c] < 0)
        val = ((val ^ 0x7ff) + 1) & 0x7ff;
      matrix[r][c] = val;
    }
  }
  color_conv.RR = matrix[0][0];
  color_conv.RG = matrix[0][1];
  color_conv.RB = matrix[0][2];
  color_conv.GR = matrix[1][0];
  color_conv.GG = matrix[1][1];
  color_conv.GB = matrix[1][2];
  color_conv.BR = matrix[2][0];
  color_conv.BG = matrix[2][1];
  color_conv.BB = matrix[2][2];
  ret = HAL_DCMIPP_PIPE_SetISPColorConversionConfig(hdcmipp, DCMIPP_PIPE1, &color_conv);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = HAL_DCMIPP_PIPE_EnableISPColorConversion(hdcmipp, DCMIPP_PIPE1);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  return ret;
}

static int32_t CMW_VD1941_getMirrorFlipConfig(uint32_t Config)
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


static int CMW_VD1941_Read8(CMW_VD1941_t *pObj, uint16_t addr, uint8_t *value)
{
  return pObj->ReadReg(pObj->Address, addr, value, 1);
}

static int CMW_VD1941_Read16(CMW_VD1941_t *pObj, uint16_t addr, uint16_t *value)
{
  uint8_t data[2];
  int ret;

  ret = pObj->ReadReg(pObj->Address, addr, data, 2);
  if (ret)
    return ret;

  *value = (data[1] << 8) | data[0];

  return CMW_ERROR_NONE;
}

static int CMW_VD1941_Read32(CMW_VD1941_t *pObj, uint16_t addr, uint32_t *value)
{
  uint8_t data[4];
  int ret;

  ret = pObj->ReadReg(pObj->Address, addr, data, 4);
  if (ret)
    return ret;

  *value = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];

  return 0;
}

static int CMW_VD1941_Write8(CMW_VD1941_t *pObj, uint16_t addr, uint8_t value)
{
  return pObj->WriteReg(pObj->Address, addr, &value, 1);
}

static int CMW_VD1941_Write16(CMW_VD1941_t *pObj, uint16_t addr, uint16_t value)
{
  return pObj->WriteReg(pObj->Address, addr, (uint8_t *) &value, 2);
}

static int CMW_VD1941_Write32(CMW_VD1941_t *pObj, uint16_t addr, uint32_t value)
{
  return pObj->WriteReg(pObj->Address, addr, (uint8_t *) &value, 4);
}

static void VD1941_ShutdownPin(struct VDx941_Ctx *ctx, int value)
{
  CMW_VD1941_t *p_ctx = container_of(ctx, CMW_VD1941_t, ctx_driver);

  p_ctx->ShutdownPin(value);
}

static int VD1941_Read8(struct VDx941_Ctx *ctx, uint16_t addr, uint8_t *value)
{
  CMW_VD1941_t *p_ctx = container_of(ctx, CMW_VD1941_t, ctx_driver);

  return CMW_VD1941_Read8(p_ctx, addr, value);
}

static int VD1941_Read16(struct VDx941_Ctx *ctx, uint16_t addr, uint16_t *value)
{
  CMW_VD1941_t *p_ctx = container_of(ctx, CMW_VD1941_t, ctx_driver);

  return CMW_VD1941_Read16(p_ctx, addr, value);
}

static int VD1941_Read32(struct VDx941_Ctx *ctx, uint16_t addr, uint32_t *value)
{
  CMW_VD1941_t *p_ctx = container_of(ctx, CMW_VD1941_t, ctx_driver);

  return CMW_VD1941_Read32(p_ctx, addr, value);
}

static int VD1941_Write8(struct VDx941_Ctx *ctx, uint16_t addr, uint8_t value)
{
  CMW_VD1941_t *p_ctx = container_of(ctx, CMW_VD1941_t, ctx_driver);

  return CMW_VD1941_Write8(p_ctx, addr, value);
}

static int VD1941_Write16(struct VDx941_Ctx *ctx, uint16_t addr, uint16_t value)
{
  CMW_VD1941_t *p_ctx = container_of(ctx, CMW_VD1941_t, ctx_driver);

  return CMW_VD1941_Write16(p_ctx, addr, value);
}

static int VD1941_Write32(struct VDx941_Ctx *ctx, uint16_t addr, uint32_t value)
{
  CMW_VD1941_t *p_ctx = container_of(ctx, CMW_VD1941_t, ctx_driver);

  return CMW_VD1941_Write32(p_ctx, addr, value);
}

static int VD1941_WriteArray(struct VDx941_Ctx *ctx, uint16_t addr, uint8_t *data, int data_len)
{
  CMW_VD1941_t *p_ctx = container_of(ctx, CMW_VD1941_t, ctx_driver);
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

static void VD1941_Delay(struct VDx941_Ctx *ctx, uint32_t delay_in_ms)
{
  CMW_VD1941_t *p_ctx = container_of(ctx, CMW_VD1941_t, ctx_driver);

  p_ctx->Delay(delay_in_ms);
}

static void VD1941_Log(struct VDx941_Ctx *ctx, int lvl, const char *format, va_list ap)
{
#if 1
  const int current_lvl = VDx941_LVL_DBG(2);

  if (lvl > current_lvl)
    return ;

  vprintf(format, ap);
#endif
}

static int CMW_VD1941_GetResType(uint32_t width, uint32_t height, VDx941_Res_t *res)
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


static int32_t CMW_VD1941_Init(void *io_ctx, CMW_Sensor_Init_t *initSensor)
{
  VDx941_Config_t config = { 0 };
  int ret;
  int i;

  if (((CMW_VD1941_t *)io_ctx)->IsInitialized)
  {
    return CMW_ERROR_NONE;
  }


  ret = CMW_VD1941_GetResType(initSensor->width, initSensor->height, &config.resolution);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }
  config.image_processing_mode = VD1941_GS_SS1_RGB_8;
  config.ext_clock_freq_in_hz = 25000000;
  config.flip_mirror_mode = CMW_VD1941_getMirrorFlipConfig(initSensor->mirrorFlip);
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

  ret = VDx941_Init(&((CMW_VD1941_t *)io_ctx)->ctx_driver, &config);
  if (ret)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  /* This gain / expo combo give correct starting point for 50 Hz flicker free and correct
   * illumination in desk environment.
   */
  ret = VDx941_SetDigitalGain(&((CMW_VD1941_t *)io_ctx)->ctx_driver, 0x300);
  if (ret)
  {
     return CMW_ERROR_COMPONENT_FAILURE;
  }
  ret = VDx941_SetExpo(&((CMW_VD1941_t *)io_ctx)->ctx_driver, 10000);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = DCMIPP_PipeBayerInit(((CMW_VD1941_t *)io_ctx)->hdcmipp, initSensor->mirrorFlip);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }
  ret = DCMIPP_PipeWhiteBalanceInit(((CMW_VD1941_t *)io_ctx)->hdcmipp, wb->white_balance);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }
  ret = DCMIPP_PipeColorInit(((CMW_VD1941_t *)io_ctx)->hdcmipp, wb->color_matrix);
  if (ret)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ((CMW_VD1941_t *)io_ctx)->IsInitialized = 1;

  return CMW_ERROR_NONE;
}

static int32_t CMW_VD1941_Start(void *io_ctx)
{
  int ret = CMW_ERROR_NONE;
  ret = VDx941_Start(&((CMW_VD1941_t *)io_ctx)->ctx_driver);
  if (ret) {
    VDx941_DeInit(&((CMW_VD1941_t *)io_ctx)->ctx_driver);
    return CMW_ERROR_PERIPH_FAILURE;
  }
  return CMW_ERROR_NONE;
}


static int32_t CMW_VD1941_Stop(void *io_ctx)
{
  int ret = CMW_ERROR_NONE;

  ret = VDx941_Stop(&((CMW_VD1941_t *)io_ctx)->ctx_driver);
  if (ret)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }
  return CMW_ERROR_NONE;
}

static int32_t CMW_VD1941_DeInit(void *io_ctx)
{
  int ret = CMW_ERROR_NONE;

  ret = VDx941_Stop(&((CMW_VD1941_t *)io_ctx)->ctx_driver);
  if (ret)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  ret = VDx941_DeInit(&((CMW_VD1941_t *)io_ctx)->ctx_driver);
  if (ret)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  ((CMW_VD1941_t *)io_ctx)->IsInitialized = 0;
  return CMW_ERROR_NONE;
}


int32_t VD1941_RegisterBusIO(CMW_VD1941_t *io_ctx)
{
  int ret;

  if (!io_ctx)
    return CMW_ERROR_COMPONENT_FAILURE;

  if (!io_ctx->Init)
    return CMW_ERROR_COMPONENT_FAILURE;

  ret = io_ctx->Init();

  return ret;
}

int32_t VD1941_ReadID(CMW_VD1941_t *io_ctx, uint32_t *Id)
{
  uint32_t reg32;
  int32_t ret;

  ret = CMW_VD1941_Read32(io_ctx, VD1941_REG_MODEL_ID, &reg32);
  if (ret)
    return ret;

  *Id = reg32;

  return CMW_ERROR_NONE;
}

static void CMW_VD1941_PowerOn(CMW_VD1941_t *io_ctx)
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

int CMW_VD1941_Probe(CMW_VD1941_t *io_ctx, CMW_Sensor_if_t *vd1941_if)
{
  int ret = CMW_ERROR_NONE;
  uint32_t id;

  io_ctx->ctx_driver.shutdown_pin = VD1941_ShutdownPin;
  io_ctx->ctx_driver.read8 = VD1941_Read8;
  io_ctx->ctx_driver.read16 = VD1941_Read16;
  io_ctx->ctx_driver.read32 = VD1941_Read32;
  io_ctx->ctx_driver.write8 = VD1941_Write8;
  io_ctx->ctx_driver.write16 = VD1941_Write16;
  io_ctx->ctx_driver.write32 = VD1941_Write32;
  io_ctx->ctx_driver.write_array = VD1941_WriteArray;
  io_ctx->ctx_driver.delay = VD1941_Delay;
  io_ctx->ctx_driver.log = VD1941_Log;

  CMW_VD1941_PowerOn(io_ctx);

  ret = VD1941_RegisterBusIO(io_ctx);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = VD1941_ReadID(io_ctx, &id);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }
  if (id != VD1941_CHIP_ID)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  memset(vd1941_if, 0, sizeof(*vd1941_if));
  vd1941_if->Init = CMW_VD1941_Init;
  vd1941_if->DeInit = CMW_VD1941_DeInit;
  vd1941_if->Start = CMW_VD1941_Start;
  vd1941_if->Stop = CMW_VD1941_Stop;

  return ret;
}
