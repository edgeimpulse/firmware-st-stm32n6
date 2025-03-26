/**
  ******************************************************************************
  * @file    HAL/Src/hal_generic_valid_it.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32n6xx_it.h"
#include "stm32n6570_discovery_camera.h"
#include "stm32n6570_discovery_sd.h"

/** @addtogroup STM32N6xx_HAL_Validation
  * @{
  */

/** @addtogroup HAL
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M55 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Secure Fault exception.
  * @param  None
  * @retval None
  */
void SecureFault_Handler(void)
{
  /* Go to infinite loop when Secure Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
  while (1)
  {
  }
}

/******************************************************************************/
/*                 STM32N6xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32n6xx.s).                                               */
/******************************************************************************/



/**
  * @}
  */
/**
  * @brief  This function handles DCMIPP Handler.
  * @param  None
  * @retval None
  */
void CSI_IRQHandler(void)
{
  HAL_DCMIPP_CSI_IRQHandler(&hcamera_dcmipp);
}

/**
  * @brief  This function handles DCMIPP interrupt request.
  * @param  None
  * @retval None
  */
void DCMIPP_IRQHandler(void)
{
  BSP_CAMERA_IRQHandler(0);
}

void SDMMC2_IRQHandler(void)
{
  BSP_SD_IRQHandler(0);
}

/**
  * @}
  */

