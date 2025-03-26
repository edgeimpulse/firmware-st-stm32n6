/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PSSI/PSSI_Slave_Single_Com/FSBL/Src/stm32n6xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   This file provides code for the MSP Initialization
  *          and de-Initialization codes.
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

  /* System interrupt init*/

  HAL_PWREx_EnableVddIO4();
  HAL_PWREx_EnableVddIO5();

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
* @brief PSSI MSP Initialization
* This function configures the hardware resources used in this example
* @param hpssi: PSSI handle pointer
* @retval None
*/
void HAL_PSSI_MspInit(PSSI_HandleTypeDef* hpssi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hpssi->Instance==PSSI)
  {
  /* USER CODE BEGIN PSSI_MspInit 0 */

  /* USER CODE END PSSI_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_PSSI;
    PeriphClkInitStruct.PssiClockSelection = RCC_PSSICLKSOURCE_HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_DCMI_PSSI_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**PSSI GPIO Configuration
    PC1     ------> PSSI_D7
    PC11     ------> PSSI_D4
    PC0     ------> PSSI_DE
    PE4     ------> PSSI_D5
    PE6     ------> PSSI_D1
    PB9     ------> PSSI_D3
    PB8     ------> PSSI_RDY
    PC3     ------> PSSI_D2
    PA1     ------> PSSI_D0
    PG1     ------> PSSI_PDCK
    PG2     ------> PSSI_D6
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_0|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_PSSI;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_PSSI;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF10_PSSI;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_PSSI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_PSSI;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_PSSI;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN PSSI_MspInit 1 */

  /* USER CODE END PSSI_MspInit 1 */
  }

}

/**
* @brief PSSI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hpssi: PSSI handle pointer
* @retval None
*/
void HAL_PSSI_MspDeInit(PSSI_HandleTypeDef* hpssi)
{
  if(hpssi->Instance==PSSI)
  {
  /* USER CODE BEGIN PSSI_MspDeInit 0 */

  /* USER CODE END PSSI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DCMI_PSSI_CLK_DISABLE();

    /**PSSI GPIO Configuration
    PC1     ------> PSSI_D7
    PC11     ------> PSSI_D4
    PC0     ------> PSSI_DE
    PE4     ------> PSSI_D5
    PE6     ------> PSSI_D1
    PB9     ------> PSSI_D3
    PB8     ------> PSSI_RDY
    PC3     ------> PSSI_D2
    PA1     ------> PSSI_D0
    PG1     ------> PSSI_PDCK
    PG2     ------> PSSI_D6
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_0|GPIO_PIN_3);

    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_4|GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9|GPIO_PIN_8);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_1|GPIO_PIN_2);

  /* USER CODE BEGIN PSSI_MspDeInit 1 */

  /* USER CODE END PSSI_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
