/**
  ******************************************************************************
  * @file    low_level_ext_flash.h
  * @author  MCD Application Team
  * @brief   This file contains device definition for low_level_flash_ext driver
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
#ifndef __LOW_LEVEL_EXT_FLASH_H
#define __LOW_LEVEL_EXT_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32n6xx_hal.h"

struct ext_flash_range
{
  uint32_t base;
  uint32_t limit;
};
struct ext_flash_vect
{
  uint32_t nb;
  struct ext_flash_range *range;
};
struct low_level_ext_flash_device
{
  struct ext_flash_vect erase;
  struct ext_flash_vect write;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) && !defined(LOCAL_LOADER_CONFIG)
  struct ext_flash_vect secure;
#endif /* __ARM_FEATURE_CMSE and (__ARM_FEATURE_CMSE == 3U) and (not LOCAL_LOADER_CONFIG) */
#if  !defined(LOCAL_LOADER_CONFIG)
  uint32_t read_error;
#endif /* not LOCAL_LOADER_CONFIG */
};

extern XSPI_HandleTypeDef XSPI_HANDLE;

extern struct low_level_ext_flash_device EXT_FLASH0_DEV;

void EXTMEM_MemCopy(uint32_t *destination_Address, const uint8_t *ptrData, uint32_t DataSize);

#ifdef __cplusplus
}
#endif

#endif /* __LOW_LEVEL_EXT_FLASH_H */

