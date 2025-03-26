/**
  ******************************************************************************
  * @file    stm32n6xx_hal_msp.c
  * @author  GPM Application Team
  * @brief   HAL MSP module.
  *          This file should be copied to the application folder and renamed
  *          to stm32n6xx_hal_msp.c
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

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP HAL MSP module driver
  * @brief HAL MSP module.
  * @{
  */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initialize the Global MSP.
  * @retval None
  */
void HAL_MspInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually
            modified by the user
   */
}

/**
  * @brief  DeInitialize the Global MSP.
  * @retval None
  */
void HAL_MspDeInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually
            modified by the user
   */
}


void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
  /* Disable interrupts */
  HAL_NVIC_DisableIRQ(RTC_S_IRQn);
  HAL_NVIC_DisableIRQ(TAMP_IRQn);

  /* Disable RTC Sleep, RTCAPBLPEN */
  __HAL_RCC_RTC_CLK_SLEEP_DISABLE();
  __HAL_RCC_RTCAPB_CLK_SLEEP_DISABLE();

  /* Disable RTC Clock, RCC_APB4LENR: RTCEN & RTCAPBEN */
  __HAL_RCC_RTC_CLK_DISABLE();
  __HAL_RCC_RTCAPB_CLK_DISABLE();

}

/**
 * @brief  Init RTC peripheral.
 * @param  RTC_HandleTypeDef* hrtc: RTC Handle.
 * @retval None.
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /*  Select LSI (32kHz) as RTC clock source */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    /* Error */
    while(1);
  }

  /* Enable LSI Clocks */
  __HAL_RCC_LSI_ENABLE();

  /* Enable Backup domain access, PWR_DBPCR_DBP enable */
  HAL_PWR_EnableBkUpAccess();

  /* Enable RTC Sleep, RTCAPBLPEN */
  __HAL_RCC_RTC_CLK_SLEEP_ENABLE();
  __HAL_RCC_RTCAPB_CLK_SLEEP_ENABLE();

  /* Enable RTC Clock, RCC_APB4LENR: RTCEN & RTCAPBEN */
  __HAL_RCC_RTC_CLK_ENABLE();
  __HAL_RCC_RTCAPB_CLK_ENABLE();

  /* Enable RTC interrupt */
  HAL_NVIC_SetPriority(RTC_S_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(RTC_S_IRQn);
}

/* RTC wake up timer callback */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
 /* Deactivate wake up timer counter */
 if(HAL_RTCEx_DeactivateWakeUpTimer(hrtc) != HAL_OK)
 {
   while(1); /* KO */
 }
}
/**
  * @brief  Initialize the PPP MSP.
  * @retval None
  */
/*
void HAL_PPP_MspInit(void)
{
}
*/

/**
  * @brief  DeInitialize the PPP MSP.
  * @retval None
  */
/*
void HAL_PPP_MspDeInit(void)
{
}
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
