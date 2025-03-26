/**
  ******************************************************************************
  * @file    low_level_mce.h
  * @author  MCD Application Team
  * @brief   Header for low_level_mce.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LOW_LEVEL_MCE_H
#define LOW_LEVEL_MCE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "stm32n6xx_hal.h"
#include "boot_hal_cfg.h"

/* Types of MCE regions (also used as indexes) */
#define LL_MCE_REGION_CODE_PRIMARY                    (0U)
#define LL_MCE_REGION_CODE_SECONDARY                  (LL_MCE_REGION_CODE_PRIMARY + 1U)
#if defined(MCUBOOT_OVERWRITE_ONLY)
#define LL_MCE_REGION_NUMBER                          (LL_MCE_REGION_CODE_SECONDARY + 1U)
#else /* not MCUBOOT_OVERWRITE_ONLY */
#define LL_MCE_REGION_SCRATCH                         (LL_MCE_REGION_CODE_SECONDARY + 1U)
#define LL_MCE_REGION_NUMBER                          (LL_MCE_REGION_SCRATCH + 1U)
#endif /* MCUBOOT_OVERWRITE_ONLY */

#define LL_MCE_REGION_CFG_INSTALL                     (0U)                             /* 0x0 cfg for install mode */
#define LL_MCE_REGION_CFG_BL                          (LL_MCE_REGION_CFG_INSTALL + 1U) /* 0x1 cfg for BL */
#define LL_MCE_REGION_CFG_APPLI                       (LL_MCE_REGION_CFG_BL + 1U)      /* 0x2 cfg for Appli */
#define LL_MCE_REGION_CFG_END                         (LL_MCE_REGION_CFG_APPLI + 1U)

/* Exported functions ------------------------------------------------------- */
void mcu_init_cfg(void);
bool LL_MCE_IsCodePrimaryRegionEnabled(void);
HAL_StatusTypeDef LL_MCE_DisableCodePrimaryRegion(void);
HAL_StatusTypeDef LL_MCE_EnableCodePrimaryRegion(void);
bool LL_MCE_IsBufferInCodePrimaryRegion(const uint8_t *pBuffer, size_t Length);

#ifdef __cplusplus
}
#endif

#endif /* LOW_LEVEL_MCE_H */

