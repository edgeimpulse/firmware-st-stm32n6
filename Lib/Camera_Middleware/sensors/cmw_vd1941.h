/**
  ******************************************************************************
  * @file    cmw_vd1941.h
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

#ifndef CMW_VD1941_H
#define CMW_VD1941_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "cmw_sensors_if.h"
#include "cmw_errno.h"
#include "vdx941.h"
#include "stm32n6xx_hal_dcmipp.h"

#define VD1941_CHIP_ID 0x53393430

typedef struct
{
  uint16_t Address;
  VDx941_Ctx_t  ctx_driver;
  DCMIPP_HandleTypeDef *hdcmipp;
  uint8_t IsInitialized;
  int32_t (*Init)(void);
  int32_t (*DeInit)(void);
  int32_t (*WriteReg)(uint16_t, uint16_t, uint8_t*, uint16_t);
  int32_t (*ReadReg) (uint16_t, uint16_t, uint8_t*, uint16_t);
  int32_t (*GetTick) (void);
  void (*Delay)(uint32_t delay_in_ms);
  void (*ShutdownPin)(int value);
  void (*EnablePin)(int value);
} CMW_VD1941_t;

int CMW_VD1941_Probe(CMW_VD1941_t *io_ctx, CMW_Sensor_if_t *vd1941_if);

#ifdef __cplusplus
}
#endif

#endif
