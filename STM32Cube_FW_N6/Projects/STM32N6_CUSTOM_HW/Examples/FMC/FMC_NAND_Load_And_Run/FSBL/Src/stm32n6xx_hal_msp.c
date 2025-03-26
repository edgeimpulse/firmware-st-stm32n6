/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : stm32n6xx_hal_msp.c
  * @author         : GPM Application Team
  * @brief          : This file provides code for the MSP Initialization
  *                   and de-Initialization codes.
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

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

static uint32_t FMC_Initialized = 0;

static void HAL_FMC_MspInit(void){
  /* USER CODE BEGIN FMC_MspInit 0 */

  /* USER CODE END FMC_MspInit 0 */
  GPIO_InitTypeDef GPIO_InitStruct ={0};
  if (FMC_Initialized) {
    return;
  }
  FMC_Initialized = 1;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FMC;
    PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_ENABLE();

  /** FMC GPIO Configuration
  PC10   ------> FMC_CLK
  PE13   ------> FMC_SDNCAS
  PE15   ------> FMC_SDCKE0
  PD10   ------> FMC_A3
  PD1   ------> FMC_A7
  PE8   ------> FMC_A12
  PE9   ------> FMC_A14
  PC5   ------> FMC_NWE
  PE14   ------> FMC_SDNE0
  PE12   ------> FMC_SDNRAS
  PE7   ------> FMC_A4
  PD15   ------> FMC_A8
  PD4   ------> FMC_A11
  PE10   ------> FMC_A15
  PE5   ------> FMC_SDNE1
  PD2   ------> FMC_A0
  PD12   ------> FMC_A5
  PD14   ------> FMC_A9
  PE6   ------> FMC_SDCKE1
  PD6   ------> FMC_A1
  PD0   ------> FMC_A6
  PC3   ------> FMC_D8
  PE11   ------> FMC_SDNWE
  PD7   ------> FMC_A2
  PD3   ------> FMC_A10
  PD8   ------> FMC_NBL0
  PB7   ------> FMC_D15
  PB6   ------> FMC_D14
  PB3 (JTDO/TRACESWO)   ------> FMC_NBL1
  PP7   ------> FMC_D23
  PP6   ------> FMC_D22
  PP0   ------> FMC_D16
  PD9   ------> FMC_SDCLK
  PP4   ------> FMC_D20
  PP1   ------> FMC_D17
  PP15   ------> FMC_D31
  PP5   ------> FMC_D21
  PP12   ------> FMC_D28
  PP3   ------> FMC_D19
  PP2   ------> FMC_D18
  PP13   ------> FMC_D29
  PP11   ------> FMC_D27
  PP8   ------> FMC_D24
  PP14   ------> FMC_D30
  PF2   ------> FMC_NWAIT
  PP9   ------> FMC_D25
  PP10   ------> FMC_D26
  PG14   ------> FMC_NCE
  PA9   ------> FMC_D3
  PA1   ------> FMC_D6
  PB11   ------> FMC_D10
  PA10   ------> FMC_D2
  PA5   ------> FMC_NOE
  PB12   ------> FMC_D9
  PA3   ------> FMC_A17
  PA3   ------> FMC_ALE
  PA11   ------> FMC_D1
  PA2   ------> FMC_D5
  PG10   ------> FMC_CLE
  PB4(NJTRST)   ------> FMC_D13
  PA12   ------> FMC_D0
  PA8   ------> FMC_D4
  PA0   ------> FMC_D7
  PB10   ------> FMC_D11
  PB5(JTDO/TRACESWO)   ------> FMC_D12
  PA4   ------> FMC_A13
  */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_5|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_15|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_14|GPIO_PIN_12|GPIO_PIN_7|GPIO_PIN_10
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_1|GPIO_PIN_15|GPIO_PIN_4
                          |GPIO_PIN_2|GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_6
                          |GPIO_PIN_0|GPIO_PIN_7|GPIO_PIN_3|GPIO_PIN_8
                          |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_3|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_4|GPIO_PIN_10|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_0|GPIO_PIN_4
                          |GPIO_PIN_1|GPIO_PIN_15|GPIO_PIN_5|GPIO_PIN_12
                          |GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_13|GPIO_PIN_11
                          |GPIO_PIN_8|GPIO_PIN_14|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOP, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_FMC;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_5
                          |GPIO_PIN_3|GPIO_PIN_11|GPIO_PIN_2|GPIO_PIN_12
                          |GPIO_PIN_8|GPIO_PIN_0|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN FMC_MspInit 1 */

  /* USER CODE END FMC_MspInit 1 */
}

void HAL_NAND_MspInit(NAND_HandleTypeDef* hnand){
  /* USER CODE BEGIN NAND_MspInit 0 */

  /* USER CODE END NAND_MspInit 0 */
  HAL_FMC_MspInit();
  /* USER CODE BEGIN NAND_MspInit 1 */

  /* USER CODE END NAND_MspInit 1 */
}

static uint32_t FMC_DeInitialized = 0;

static void HAL_FMC_MspDeInit(void){
  /* USER CODE BEGIN FMC_MspDeInit 0 */

  /* USER CODE END FMC_MspDeInit 0 */
  if (FMC_DeInitialized) {
    return;
  }
  FMC_DeInitialized = 1;
  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_DISABLE();

  /** FMC GPIO Configuration
  PC10   ------> FMC_CLK
  PE13   ------> FMC_SDNCAS
  PE15   ------> FMC_SDCKE0
  PD10   ------> FMC_A3
  PD1   ------> FMC_A7
  PE8   ------> FMC_A12
  PE9   ------> FMC_A14
  PC5   ------> FMC_NWE
  PE14   ------> FMC_SDNE0
  PE12   ------> FMC_SDNRAS
  PE7   ------> FMC_A4
  PD15   ------> FMC_A8
  PD4   ------> FMC_A11
  PE10   ------> FMC_A15
  PE5   ------> FMC_SDNE1
  PD2   ------> FMC_A0
  PD12   ------> FMC_A5
  PD14   ------> FMC_A9
  PE6   ------> FMC_SDCKE1
  PD6   ------> FMC_A1
  PD0   ------> FMC_A6
  PC3   ------> FMC_D8
  PE11   ------> FMC_SDNWE
  PD7   ------> FMC_A2
  PD3   ------> FMC_A10
  PD8   ------> FMC_NBL0
  PB7   ------> FMC_D15
  PB6   ------> FMC_D14
  PB3 (JTDO/TRACESWO)   ------> FMC_NBL1
  PP7   ------> FMC_D23
  PP6   ------> FMC_D22
  PP0   ------> FMC_D16
  PD9   ------> FMC_SDCLK
  PP4   ------> FMC_D20
  PP1   ------> FMC_D17
  PP15   ------> FMC_D31
  PP5   ------> FMC_D21
  PP12   ------> FMC_D28
  PP3   ------> FMC_D19
  PP2   ------> FMC_D18
  PP13   ------> FMC_D29
  PP11   ------> FMC_D27
  PP8   ------> FMC_D24
  PP14   ------> FMC_D30
  PF2   ------> FMC_NWAIT
  PP9   ------> FMC_D25
  PP10   ------> FMC_D26
  PG14   ------> FMC_NCE
  PA9   ------> FMC_D3
  PA1   ------> FMC_D6
  PB11   ------> FMC_D10
  PA10   ------> FMC_D2
  PA5   ------> FMC_NOE
  PB12   ------> FMC_D9
  PA3   ------> FMC_A17
  PA3   ------> FMC_ALE
  PA11   ------> FMC_D1
  PA2   ------> FMC_D5
  PG10   ------> FMC_CLE
  PB4(NJTRST)   ------> FMC_D13
  PA12   ------> FMC_D0
  PA8   ------> FMC_D4
  PA0   ------> FMC_D7
  PB10   ------> FMC_D11
  PB5(JTDO/TRACESWO)   ------> FMC_D12
  PA4   ------> FMC_A13
  */
  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_5|GPIO_PIN_3);

  HAL_GPIO_DeInit(GPIOE, GPIO_PIN_13|GPIO_PIN_15|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_14|GPIO_PIN_12|GPIO_PIN_7|GPIO_PIN_10
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_11);

  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_10|GPIO_PIN_1|GPIO_PIN_15|GPIO_PIN_4
                          |GPIO_PIN_2|GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_6
                          |GPIO_PIN_0|GPIO_PIN_7|GPIO_PIN_3|GPIO_PIN_8
                          |GPIO_PIN_9);

  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_3|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_4|GPIO_PIN_10|GPIO_PIN_5);

  HAL_GPIO_DeInit(GPIOP, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_0|GPIO_PIN_4
                          |GPIO_PIN_1|GPIO_PIN_15|GPIO_PIN_5|GPIO_PIN_12
                          |GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_13|GPIO_PIN_11
                          |GPIO_PIN_8|GPIO_PIN_14|GPIO_PIN_9|GPIO_PIN_10);

  HAL_GPIO_DeInit(GPIOF, GPIO_PIN_2);

  HAL_GPIO_DeInit(GPIOG, GPIO_PIN_14|GPIO_PIN_10);

  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_5
                          |GPIO_PIN_3|GPIO_PIN_11|GPIO_PIN_2|GPIO_PIN_12
                          |GPIO_PIN_8|GPIO_PIN_0|GPIO_PIN_4);

  /* USER CODE BEGIN FMC_MspDeInit 1 */

  /* USER CODE END FMC_MspDeInit 1 */
}

void HAL_NAND_MspDeInit(NAND_HandleTypeDef* hnand){
  /* USER CODE BEGIN NAND_MspDeInit 0 */

  /* USER CODE END NAND_MspDeInit 0 */
  HAL_FMC_MspDeInit();
  /* USER CODE BEGIN NAND_MspDeInit 1 */

  /* USER CODE END NAND_MspDeInit 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
