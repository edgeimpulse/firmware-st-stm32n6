/**
  ******************************************************************************
  * @file    low_level_device.c
  * @author  MCD Application Team
  * @brief   This file contains device definition for low_level_device
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
#include "flash_layout.h"
#include "low_level_flash.h"
static struct flash_range erase_vect[] =
{
#if !defined(MCUBOOT_OVERWRITE_ONLY)
  { FLASH_AREA_SCRATCH_OFFSET, FLASH_AREA_SCRATCH_OFFSET + FLASH_AREA_SCRATCH_SIZE - 1},
#endif /* !MCUBOOT_OVERWRITE_ONLY */
  { FLASH_AREA_BEGIN_OFFSET, FLASH_AREA_END_OFFSET - 1}
};
static struct flash_range write_vect[] =
{
#if !defined(MCUBOOT_OVERWRITE_ONLY)
  { FLASH_AREA_SCRATCH_OFFSET, FLASH_AREA_SCRATCH_OFFSET + FLASH_AREA_SCRATCH_SIZE - 1},
#endif /* !MCUBOOT_OVERWRITE_ONLY */
  { FLASH_AREA_BEGIN_OFFSET, FLASH_AREA_END_OFFSET - 1}
};
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
static struct flash_range secure_vect[] =
{
#if !defined(MCUBOOT_OVERWRITE_ONLY)
  { FLASH_AREA_SCRATCH_OFFSET, FLASH_AREA_SCRATCH_OFFSET + FLASH_AREA_SCRATCH_SIZE - 1},
#endif /* !MCUBOOT_OVERWRITE_ONLY */
  { FLASH_AREA_BEGIN_OFFSET, FLASH_AREA_0_OFFSET + FLASH_S_PARTITION_SIZE - 1}
};
#endif

struct low_level_device FLASH0_DEV =
{
  .erase = { .nb = sizeof(erase_vect) / sizeof(struct flash_range), .range = erase_vect},
  .write = { .nb = sizeof(write_vect) / sizeof(struct flash_range), .range = write_vect},
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  .secure = { .nb = sizeof(secure_vect) / sizeof(struct flash_range), .range = secure_vect},
#endif
  .read_error = 1
};
