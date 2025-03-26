/**
  ******************************************************************************
  * @file    low_level_security.h
  * @author  MCD Application Team
  * @brief   Header for low_level_security.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LOW_LEVEL_SECURITY_H
#define LOW_LEVEL_SECURITY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "boot_hal_cfg.h"
#include "bootutil/bootutil_log.h"
#include "flash_layout.h"
/** @addtogroup Secure Secure Boot / Firmware Update
  * @{
  */

/** @addtogroup LOW_LEVEL
  * @{
  */
/** @defgroup LOW_LEVEL_SECURITY Security Low Level define
  * @{
  */
#define SRAM2_PAGE_SIZE (0x400)
/**
  * @}
  */
/** @defgroup LOW_LEVEL_SECURITY Security Low Level Interface
  * @{
  */
/** @defgroup SECURITY_Configuration Security Configuration
  * @{
  */

/**
  * @}
  */

/** @defgroup SECURITY_Exported_Constants Exported Constants
  * @{
  */

/**
  * @}
  */

/** @defgroup SECURITY_Exported_Functions Exported Functions
  * @{
  */
void    LL_SECU_ApplyRunTimeProtections(void);
void    LL_SECU_UpdateRunTimeProtections(void);
void    LL_SECU_DisableCleanMpu(void);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* LOW_LEVEL_SECURITY_H */
