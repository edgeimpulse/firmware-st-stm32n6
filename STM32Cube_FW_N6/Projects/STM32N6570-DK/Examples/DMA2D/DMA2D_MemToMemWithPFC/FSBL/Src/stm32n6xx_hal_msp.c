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

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
* @brief DMA2D MSP Initialization
* This function configures the hardware resources used in this example
* @param hdma2d: DMA2D handle pointer
* @retval None
*/
void HAL_DMA2D_MspInit(DMA2D_HandleTypeDef* hdma2d)
{
  if(hdma2d->Instance==DMA2D)
  {
  /* USER CODE BEGIN DMA2D_MspInit 0 */

  /* USER CODE END DMA2D_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_DMA2D_CLK_ENABLE();
    /* DMA2D interrupt Init */
    HAL_NVIC_SetPriority(DMA2D_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2D_IRQn);
  /* USER CODE BEGIN DMA2D_MspInit 1 */
    __HAL_RCC_RIFSC_CLK_ENABLE();

    /* DMA2D access */
    RIMC_MasterConfig_t RIMC_master = {0};
    RIMC_master.MasterCID = RIF_CID_1;
    RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;
    HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_DMA2D, &RIMC_master);
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_DMA2D, RIF_ATTRIBUTE_SEC|RIF_ATTRIBUTE_PRIV);

  /* USER CODE END DMA2D_MspInit 1 */
  }

}

/**
* @brief DMA2D MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hdma2d: DMA2D handle pointer
* @retval None
*/
void HAL_DMA2D_MspDeInit(DMA2D_HandleTypeDef* hdma2d)
{
  if(hdma2d->Instance==DMA2D)
  {
  /* USER CODE BEGIN DMA2D_MspDeInit 0 */
  /*## Reset peripherals ##################################################*/
  /* Enable DMA2D reset state */
  __HAL_RCC_DMA2D_FORCE_RESET();

  /* Release DMA2D from reset state */
  __HAL_RCC_DMA2D_RELEASE_RESET();

  /* USER CODE END DMA2D_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DMA2D_CLK_DISABLE();

    /* DMA2D interrupt DeInit */
    HAL_NVIC_DisableIRQ(DMA2D_IRQn);
    /* USER CODE BEGIN DMA2D_MspDeInit 1 */
    RIMC_MasterConfig_t RIMC_master = {0};

    RIMC_master.MasterCID = RIF_CID_1;

    RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;

    HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_DMA2D, &RIMC_master);

    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_DMA2D, RIF_ATTRIBUTE_SEC|RIF_ATTRIBUTE_PRIV);

  /* USER CODE END DMA2D_MspDeInit 1 */
  }

}

/**
* @brief LTDC MSP Initialization
* This function configures the hardware resources used in this example
* @param hltdc: LTDC handle pointer
* @retval None
*/
void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
  GPIO_InitTypeDef  gpio_init_structure = {0};
  RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct = {0};
  RIMC_MasterConfig_t RIMC_master = {0};

  if (hltdc->Instance == LTDC)
  {
    __HAL_RCC_LTDC_CLK_ENABLE();

    __HAL_RCC_LTDC_FORCE_RESET();
    __HAL_RCC_LTDC_RELEASE_RESET();

    /* STM32N6570-DK MB1860A RK050HR18 */
    /* PG0  R0          */
    /* PD9  R1          */
    /* PD15 R2          */
    /* PB4  R3          */
    /* PH4  R4          */
    /* PA15 R5          */
    /* PG11 R6          */
    /* PD8  R7          */
    /* PG12 G0          */
    /* PG1  G1          */
    /* PA1  G2          */
    /* PA0  G3          */
    /* PB15 G4          */
    /* PB12 G5          */
    /* PB11 G6          */
    /* PG8  G7          */
    /* P15  B0          */
    /* PA7  B1          */
    /* PB2  B2          */
    /* PG6  B3          */
    /* PH3  B4          */
    /* PH6  B5          */
    /* PA8  B6          */
    /* PA2  B7          */
    /*                  */
    /* PG13 LCD_DE      */
    /* PQ3  LCD_ONOFF   */
    /* PB14 LCD_HSYNC   */
    /* PE11 PCD_VSYNC   */
    /* PB13 LCD_CLK     */
    /* PQ4  LCD_INT     */
    /* PQ6  LCD_BL_CTRL */
    /* PE1  NRST        */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOQ_CLK_ENABLE();

    gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull      = GPIO_NOPULL;
    gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;

    /* G3, G2, B7, B1, B6, R5 */
    gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_15;
    gpio_init_structure.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOA, &gpio_init_structure);

    /* LCD_CLK, LCD_HSYNC B2, R3, G6, G5, G4 */
    gpio_init_structure.Pin       = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_15;
    gpio_init_structure.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOB, &gpio_init_structure);

    /* R7, R1, R2 */
    gpio_init_structure.Pin       = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_15;
    gpio_init_structure.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOD, &gpio_init_structure);

    /* LCD_VSYNC */
    gpio_init_structure.Pin       = GPIO_PIN_11;
    gpio_init_structure.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOE, &gpio_init_structure);

    /* R0, G1, B3, G7, R6, G0 */
    gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_11 | GPIO_PIN_12 ;
    gpio_init_structure.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOG, &gpio_init_structure);

    /* B4, R4, B5 */
    gpio_init_structure.Pin       = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6;
    gpio_init_structure.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOH, &gpio_init_structure);

    /* NRST */
    gpio_init_structure.Pin       = GPIO_PIN_1;
    gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOE, &gpio_init_structure);

    /* LCD_ONOFF, LCD_BL_CTRL */
    gpio_init_structure.Pin       = GPIO_PIN_3 | GPIO_PIN_6;
    gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOQ, &gpio_init_structure);

    /* LCD_DE */
    gpio_init_structure.Pin       = GPIO_PIN_13;
    gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOG, &gpio_init_structure);

    HAL_GPIO_WritePin(GPIOQ, GPIO_PIN_3, GPIO_PIN_SET); /* LCD On */ /* PQ3  LCD_ONOFF   */
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET); /* Display Enable */ /* PG13 LCD_DE      */
    HAL_GPIO_WritePin(GPIOQ, GPIO_PIN_6, GPIO_PIN_SET); /* 100% Brightness */ /* PQ6  LCD_BL_CTRL */

    /* LCD clock configuration */
    /* Typical PCLK is 25 MHz so the PLL1 is configured to provide this clock */
    /* LTDC - PLL1 */
    /* Configure LTDC clock to IC16 with PLL1  */

    /* LCD clock configuration */
    /* Typical PCLK is 25 MHz so the PLL1 is configured to provide this clock */
    /* LCD clock configuration */
    /* PLL1_VCO Input = HSI_VALUE/PLLM = 64 Mhz / 4 = 16 */
    /* PLL1_VCO Output = PLL3_VCO Input * PLLN = 16 Mhz * 75 = 1200 */
    /* PLLLCDCLK = PLL3_VCO Output/(PLLP1 * PLLP2) = 1200/48 = 25Mhz */
    /* LTDC clock frequency = PLLLCDCLK = 25 Mhz */
    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    RCC_PeriphCLKInitStruct.LtdcClockSelection = RCC_LTDCCLKSOURCE_IC16;
    RCC_PeriphCLKInitStruct.ICSelection[RCC_IC16].ClockSelection = RCC_ICCLKSOURCE_PLL1;
    RCC_PeriphCLKInitStruct.ICSelection[RCC_IC16].ClockDivider = 48;
    if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    RIMC_master.MasterCID = RIF_CID_1;
    RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;

    HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_LTDC1 , &RIMC_master);
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDCL1 , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  }

  /* USER CODE BEGIN LTDC_MspInit 1 */

  /* USER CODE END LTDC_MspInit 1 */
}

/**
* @brief LTDC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hltdc: LTDC handle pointer
* @retval None
*/
void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef* hltdc)
{
  if(hltdc->Instance==LTDC)
  {
    /* USER CODE BEGIN LTDC_MspDeInit 0 */

    /* USER CODE END LTDC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LTDC_CLK_DISABLE();

    /**LTDC GPIO Configuration
    PC4     ------> LTDC_DE
    PH3     ------> LTDC_B4
    PH6     ------> LTDC_B5
    PD15     ------> LTDC_R2
    PB14     ------> LTDC_HSYNC
    PC9     ------> LTDC_B3
    PC0     ------> LTDC_R4
    PB13     ------> LTDC_CLK
    PB2     ------> LTDC_B2
    PD13     ------> LTDC_R6
    PC8     ------> LTDC_B0
    PE4     ------> LTDC_G1
    PB15     ------> LTDC_G4
    PE11     ------> LTDC_VSYNC
    PD8     ------> LTDC_R7
    PD9     ------> LTDC_R1
    PO2     ------> LTDC_B7
    PF4     ------> LTDC_R3
    PF5     ------> LTDC_G0
    PG5     ------> LTDC_B1
    PO3     ------> LTDC_G3
    PA1     ------> LTDC_G2
    PB11     ------> LTDC_G6
    PA15(JTDI)     ------> LTDC_R5
    PB12     ------> LTDC_G5
    PG0     ------> LTDC_R0
    PG8     ------> LTDC_G7
    PA8     ------> LTDC_B6
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_4|GPIO_PIN_9|GPIO_PIN_0|GPIO_PIN_8);

    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_3|GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_15|GPIO_PIN_13|GPIO_PIN_8|GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_14|GPIO_PIN_13|GPIO_PIN_2|GPIO_PIN_15
                    |GPIO_PIN_11|GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_4|GPIO_PIN_11);

    HAL_GPIO_DeInit(GPIOO, GPIO_PIN_2|GPIO_PIN_3);

    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_4|GPIO_PIN_5);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_5|GPIO_PIN_0|GPIO_PIN_8);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_15|GPIO_PIN_8);

    /* USER CODE BEGIN LTDC_MspDeInit 1 */

    /* USER CODE END LTDC_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
