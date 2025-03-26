/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples/ADC/ADC_FixedTriggerLatency/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
#include "stm32n6xx_nucleo.h"
#include "stm32n6xx_ll_tim.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* Function pointer declaration in non-secure*/
#if defined(__ICCARM__)
#pragma section=".noncacheable"
#define __NON_CACHEABLE_SECTION_BEGIN ((uint32_t) __sfb(".noncacheable"))
#define __NON_CACHEABLE_SECTION_END		((uint32_t) __sfe(".noncacheable"))
#elif defined(__GNUC__)
extern uint32_t __snoncacheable;
extern uint32_t __enoncacheable;
#define __NON_CACHEABLE_SECTION_BEGIN ((uint32_t) &__snoncacheable)
#define __NON_CACHEABLE_SECTION_END		((uint32_t) &__enoncacheable)
#endif

/* typedef for non-secure callback functions */
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

/**
  * @brief Toggle periods for various blinking modes
  */
#define LED_BLINK_FAST       100  /* Toggle period fast (unit: ms) */
#define LED_BLINK_SLOW       500  /* Toggle period slow (unit: ms) */
#define LED_BLINK_ERROR     1000  /* Toggle period very slow for error case (unit: ms) */

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
