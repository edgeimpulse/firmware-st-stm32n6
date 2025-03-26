/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32n6xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
  * @brief  Initializes the SD MSP.
  * @param  hsd  SD handle
  * @retval None
  */
void HAL_SD_MspInit(SD_HandleTypeDef* hsd)
{
    GPIO_InitTypeDef gpio_init_structure = {0};

    HAL_PWREx_EnableVddIO5();
    /* Enable SDMMC clock */
    __HAL_RCC_SDMMC2_CLK_ENABLE();

    /* Enable GPIOs clock */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* Common GPIO configuration */
    gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull      = GPIO_PULLUP;
    gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF11_SDMMC2;

    /* D2-CLK-CMD-D0-D1*/
    gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4  | GPIO_PIN_5 ;
    HAL_GPIO_Init(GPIOC, &gpio_init_structure);

    /* D3*/
    gpio_init_structure.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOE, &gpio_init_structure);

    /* NVIC configuration for SDMMC2 interrupts */
    HAL_NVIC_SetPriority(SDMMC2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(SDMMC2_IRQn);
}

/**
  * @brief  DeInitializes the SD MSP.
  * @param  hsd  SD handle
  * @retval None
  */
void HAL_SD_MspDeInit(SD_HandleTypeDef* hsd)
{
  GPIO_InitTypeDef gpio_init_structure;

  if(hsd->Instance==SDMMC2)
  {
    /* Disable NVIC for SDIO interrupts */
    HAL_NVIC_DisableIRQ(SDMMC2_IRQn);

    /* GPIOC configuration */
    gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4  | GPIO_PIN_5 ;
    HAL_GPIO_DeInit(GPIOC, gpio_init_structure.Pin);

    /* GPIOD configuration */
    gpio_init_structure.Pin = GPIO_PIN_4;
    HAL_GPIO_DeInit(GPIOD, gpio_init_structure.Pin);

    /* Disable SDMMC2 clock */
    __HAL_RCC_SDMMC2_CLK_DISABLE();
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
