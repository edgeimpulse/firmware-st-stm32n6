/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32n6xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32n6xx_it.h"
#include "usbpd.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern SD_HandleTypeDef hsd1;
extern PCD_HandleTypeDef hpcd_USB1_OTG_HS;
extern TIM_HandleTypeDef htim6;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers             */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
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
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32N6xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32n6xx.s).                    */
/******************************************************************************/

/**
* @brief  This function handles USB-On-The-Go FS/HS global interrupt request.
* @param  None
* @retval None
*/
void USB1_OTG_HS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_HS_IRQHandler 0 */

  /* USER CODE END OTG_HS_IRQHandler 0 */
  HAL_PCD_IRQHandler(&hpcd_USB1_OTG_HS);
  /* USER CODE BEGIN OTG_HS_IRQHandler 1 */

  /* USER CODE END OTG_HS_IRQHandler 1 */
}

/**
  * @brief This function handles EXTI line2 interrupt.
  */
void EXTI2_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI14_IRQn 0 */

  /* USER CODE END EXTI14_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(SD_DETECT_Pin);
  /* USER CODE BEGIN EXTI14_IRQn 1 */

  /* USER CODE END EXTI14_IRQn 1 */
}

/**
  * @brief This function handles SDMMC2 global interrupt.
  */
void SDMMC2_IRQHandler(void)
{
  /* USER CODE BEGIN SDMMC2_IRQn 0 */

  /* USER CODE END SDMMC2_IRQn 0 */
  HAL_SD_IRQHandler(&hsd1);
  /* USER CODE BEGIN SDMMC2_IRQn 1 */

  /* USER CODE END SDMMC2_IRQn 1 */
}

void TIM6_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */

  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/**
  * @brief This function handles UCPD1 global interrupt.
  */
void UCPD1_IRQHandler(void)
{
  /* USER CODE BEGIN UCPD1_IRQn 0 */

  /* USER CODE END UCPD1_IRQn 0 */
  USBPD_PORT0_IRQHandler();
  /* USER CODE BEGIN UCPD1_IRQn 1 */

  /* USER CODE END UCPD1_IRQn 1 */
}
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
