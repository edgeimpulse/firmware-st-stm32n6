/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined ( __ICCARM__ )
#  define CMSE_NS_CALL  __cmse_nonsecure_call
#  define CMSE_NS_ENTRY __cmse_nonsecure_entry
#else
#  define CMSE_NS_CALL  __attribute((cmse_nonsecure_call))
#  define CMSE_NS_ENTRY __attribute((cmse_nonsecure_entry))
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* Function pointer declaration in non-secure*/
#if defined ( __ICCARM__ )
typedef void (CMSE_NS_CALL *funcptr)(void);
#else
typedef void CMSE_NS_CALL (*funcptr)(void);
#endif

/* typedef for non-secure callback functions */
typedef funcptr funcptr_NS;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* --- Fuse : hardware configuration --- */
#define BSEC_HW_CONFIG_ID        124U

#define BSEC_HWS_IWDG1_HW        0U
#define BSEC_HWS_IWDG_FZ_STOP    1U
#define BSEC_HWS_IWDG_FZ_STANDBY 2U
#define BSEC_HWS_RST_STOP        10U
#define BSEC_HWS_RST_STDBY       11U
#define BSEC_HWS_SELINBORH       12U
#define BSEC_HWS_HSLV_VDDIO5     13U
#define BSEC_HWS_HSLV_VDDIO4     14U
#define BSEC_HWS_HSLV_VDDIO3     15U
#define BSEC_HWS_HSLV_VDDIO2     16U
#define BSEC_HWS_HSLV_VDD        17U
#define BSEC_HWS_DFT_DISABLE     20U
/* ------------------------------------- */

/******************************************************/
/* --- Customize address and mask according needs --- */
/* OTP word to be configured */
#define BSEC_FUSE_ADDRESS        BSEC_HW_CONFIG_ID
/* OTP bits to be configured */
#define BSEC_FUSE_MASK           ((uint32_t)(1U << BSEC_HWS_HSLV_VDDIO3))
/* -------------------------------------------------- */
/******************************************************/

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
