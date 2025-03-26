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
#include "stm32n6xx_ll_bus.h"
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
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if (huart->Instance == USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_IC9;
    PeriphClkInitStruct.ICSelection[RCC_IC9].ClockSelection = RCC_ICCLKSOURCE_PLL4;
    PeriphClkInitStruct.ICSelection[RCC_IC9].ClockDivider = 1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PE5     ------> USART1_TX
    PE6     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }

}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PE5     ------> USART1_TX
    PE6     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_5 | GPIO_PIN_6);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }

}

/**
* @brief HCD MSP Initialization
* This function configures the hardware resources used in this example
* @param hhcd: HCD handle pointer
* @retval None
*/
void HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd)
{
  if (hhcd->Instance == USB1_OTG_HS)
  {
    /* USER CODE BEGIN USB_OTG_HS_MspInit 0 */

    /* USER CODE END USB_OTG_HS_MspInit 0 */
    /* Enable VDDUSB */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_EnableVddUSBVMEN();
    while(__HAL_PWR_GET_FLAG(PWR_FLAG_USB33RDY));
    HAL_PWREx_EnableVddUSB();

    /** Initializes the peripherals clock
    */
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USBOTGHS1;
    PeriphClkInitStruct.UsbOtgHs1ClockSelection = RCC_USBPHY1REFCLKSOURCE_HSE_DIRECT;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      /* Initialization Error */
      Error_Handler();
    }

    /** Set USB OTG HS PHY1 Reference Clock Source */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USBPHY1;
    PeriphClkInitStruct.UsbPhy1ClockSelection = RCC_USBPHY1REFCLKSOURCE_HSE_DIRECT;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      /* Initialization Error */
      Error_Handler();
    }

    __HAL_RCC_GPIOA_CLK_ENABLE();

    LL_AHB5_GRP1_ForceReset(0x00800000);
    __HAL_RCC_USB1_OTG_HS_FORCE_RESET();
    __HAL_RCC_USB1_OTG_HS_PHY_FORCE_RESET();

    LL_RCC_HSE_SelectHSEDiv2AsDiv2Clock();
    LL_AHB5_GRP1_ReleaseReset(0x00800000);

    /* Peripheral clock enable */
    __HAL_RCC_USB1_OTG_HS_CLK_ENABLE();

    USB1_HS_PHYC->USBPHYC_CR &= ~(0x7 << 0x4);
    USB1_HS_PHYC->USBPHYC_CR |= (0x2 << 0x4);

    __HAL_RCC_USB1_OTG_HS_PHY_RELEASE_RESET();

    HAL_Delay(1);

    __HAL_RCC_USB1_OTG_HS_RELEASE_RESET();

    /* Peripheral PHY clock enable */
    __HAL_RCC_USB1_OTG_HS_PHY_CLK_ENABLE();

    /* USB_OTG_HS interrupt Init */
    HAL_NVIC_SetPriority(USB1_OTG_HS_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(USB1_OTG_HS_IRQn);

    /* USER CODE BEGIN USB_OTG_HS_MspInit 1 */

    /* USER CODE END USB_OTG_HS_MspInit 1 */
  }
}

/**
* @brief HCD MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hhcd: HCD handle pointer
* @retval None
*/
void HAL_HCD_MspDeInit(HCD_HandleTypeDef *hhcd)
{
  if (hhcd->Instance == USB1_OTG_HS)
  {
    /* USER CODE BEGIN USB_OTG_HS_MspDeInit 0 */

    /* USER CODE END USB_OTG_HS_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USB1_OTG_HS_CLK_DISABLE();
    __HAL_RCC_USB1_OTG_HS_PHY_CLK_DISABLE();

    /* Disable VDDUSB */
    if(__HAL_RCC_PWR_IS_CLK_ENABLED())
    {
      HAL_PWREx_DisableVddUSB();
    }
    else
    {
      __HAL_RCC_PWR_CLK_ENABLE();
      HAL_PWREx_DisableVddUSB();
      __HAL_RCC_PWR_CLK_DISABLE();
    }
    /* USB_OTG_HS interrupt DeInit */
    HAL_NVIC_DisableIRQ(USB1_OTG_HS_IRQn);
  /* USER CODE BEGIN USB_OTG_HS_MspDeInit 1 */

  /* USER CODE END USB_OTG_HS_MspDeInit 1 */
  }
}
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
