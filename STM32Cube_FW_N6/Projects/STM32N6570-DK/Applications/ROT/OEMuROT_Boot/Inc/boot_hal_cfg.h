/**
  ******************************************************************************
  * @file    boot_hal_cfg.h
  * @author  MCD Application Team
  * @brief   File fixing configuration flag specific for STM32N6xx platform
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
#ifndef BOOT_HAL_CFG_H
#define BOOT_HAL_CFG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"
#include "flash_layout.h"

/* IWDG */
#define KR_RELOAD          (uint16_t) 0xAAAA

/* RTC clock */
#define RTC_CLOCK_SOURCE_LSI
#ifdef RTC_CLOCK_SOURCE_LSI
#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00F9
#endif
#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00FF
#endif

/* ICache */
//#define OEMIROT_ICACHE_ENABLE /*!< Instruction cache enable */

/* Static protections */
#define OEMIROT_SECURE_USER_SRAM2_ERASE_AT_RESET /*!< SRAM2 clear at Reset  */
#define OEMIROT_SECURE_USER_SRAM2_ECC /*!< SRAM2 ECC */

#define NO_TAMPER            (0)                /*!< No tamper activated */
#define INTERNAL_TAMPER_ONLY (1)                /*!< Only Internal tamper activated */
#define ALL_TAMPER           (2)                /*!< Internal and External tamper activated */
#define OEMIROT_TAMPER_ENABLE NO_TAMPER            /*!< TAMPER configuration flag  */

#ifdef OEMIROT_DEV_MODE
#define OEMIROT_ERROR_HANDLER_STOP_EXEC /*!< Error handler stops execution (else it resets) */
#endif /* OEMIROT_DEV_MODE */

/* Run time protections */
#define OEMIROT_FLASH_PRIVONLY_ENABLE  /*!< Flash Command in Privileged only  */
#define OEMIROT_MPU_PROTECTION    /*!< OEMiROT_Boot uses MPU to prevent execution outside of OEMiROT_Boot code  */
#define OEMIROT_MCE_PROTECTION    /*!< OEMiROT_Boot uses MCE to prevent accesses to not encrypted code in external flash  */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  OEMIROT_SUCCESS = 0U,
  OEMIROT_FAILED
} OEMIROT_ErrorStatus;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Error_Handler(void) __NO_RETURN;
#endif /* BOOT_HAL_CFG_H */
