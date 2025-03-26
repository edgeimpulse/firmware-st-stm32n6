/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "app_threadx.h"
#include <stdio.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#if defined(__ICCARM__)
#include <LowLevelIOInterface.h>
#endif /* __ICCARM__ */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static UART_HandleTypeDef huart1;
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void SystemPower_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

#if defined(__ICCARM__)
/* New definition from EWARM V9.40 */
#define PUTCHAR_PROTOTYPE size_t __write(int file, unsigned char const *ptr, size_t len)
#elif defined(__ARMCC_VERSION)
/* ARM Compiler 6*/
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif /* __ICCARM__ */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  /* HAL init and setup systick to 1ms timebase */
  HAL_Init();

  /* Configure the System Power */
  SystemPower_Config();

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure GPIO pins */
  MX_GPIO_Init();

  /* Configure UART */
  MX_USART1_UART_Init();

  /* Configure and launch ThreadX*/
  MX_ThreadX_Init();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  while (1)
  {
  }

}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            CPU Clock source               = IC1_CK
  *            System bus Clock source        = IC2_IC6_IC11_CK
  *            CPUCLK (sysa_ck) (Hz)          = 800000000
  *            SYSCLK AXI (sysb_ck) (Hz)      = 400000000
  *            SYSCLK NPU (sysc_ck) (Hz)      = 800000000
  *            SYSCLK AXISRAM3/4/5/6 (sysd_ck) (Hz) = 800000000
  *            HCLKx(Hz)                      = 200000000
  *            PCLKx(Hz)                      = 200000000
  *            AHB Prescaler                  = 2
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            APB4 Prescaler                 = 1
  *            APB5 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 48000000
  *            PLL1 State                     = ON
  *            PLL1 clock source              = HSI
  *            PLL1 M                         = 6
  *            PLL1 N                         = 100
  *            PLL1 P1                        = 1
  *            PLL1 P2                        = 1
  *            PLL1 FRACN                     = 0
  *            PLL2 State                     = BYPASS
  *            PLL2 clock source              = HSI
  *            PLL3 State                     = BYPASS
  *            PLL3 clock source              = HSI
  *            PLL4 State                     = BYPASS
  *            PLL4 clock source              = HSI
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Set back system and CPU clock source to HSI */
  __HAL_RCC_CPUCLK_CONFIG(RCC_CPUCLKSOURCE_HSI);
  __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_HSI);

  /* HSE selected as source (stable clock on Level 0 samples */
  /* PLL1 output = ((HSI/PLLM)*PLLN)/PLLP1/PLLP2             */
  /*             = ((64000000/8)*100)/1/1                    */
  /*             = (8000000*100)/1/1                         */
  /*             = 800000000 (800 MHz)                       */
  /* PLL2 output = HSI (64 MHz)                              */
  /* PLL3 output = HSI (64 MHz)                              */
  /* PLL4 output = HSI (64 MHz)                              */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL1.PLLM = 8;
  RCC_OscInitStruct.PLL1.PLLN = 100;
  RCC_OscInitStruct.PLL1.PLLP1 = 1;
  RCC_OscInitStruct.PLL1.PLLP2 = 1;
  RCC_OscInitStruct.PLL1.PLLFractional = 0;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_BYPASS;
  RCC_OscInitStruct.PLL2.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_BYPASS;
  RCC_OscInitStruct.PLL3.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_BYPASS;
  RCC_OscInitStruct.PLL4.PLLSource = RCC_PLLSOURCE_HSI;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Select PLL1 outputs as CPU and System bus clock source */
  /* CPUCLK (sysa_ck) = ic1_ck = PLL1 output/ic1_divider = 800 MHz */
  /* SYSCLK AXI (sysb_ck) = ic2_ck = PLL1 output/ic2_divider = 400 MHz */
  /* SYSCLK NPU (sysc_ck) = ic6_ck = PLL1 output/ic6_divider = 800 MHz */
  /* SYSCLK AXISRAM3/4/5/6 (sysd_ck) = ic11_ck = PLL1 output/ic11_divider = 800 MHz */
  /* Configure the HCLK, PCLK1, PCLK2, PCLK4 and PCLK5 clocks dividers */
  /* HCLK = ic2_ck = PLL1 output/HCLK divider = 200 MHz */
  /* PCLKx = HCLK / PCLKx divider = 200 MHz */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK  | \
                                 RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK4  | RCC_CLOCKTYPE_PCLK5);
  RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_IC1;
  RCC_ClkInitStruct.IC1Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC1Selection.ClockDivider = 1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_IC2_IC6_IC11;
  RCC_ClkInitStruct.IC2Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC2Selection.ClockDivider = 2;
  RCC_ClkInitStruct.IC6Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC6Selection.ClockDivider = 1;
  RCC_ClkInitStruct.IC11Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC11Selection.ClockDivider = 1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;
  RCC_ClkInitStruct.APB5CLKDivider = RCC_APB5_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

/**
  * @brief Power Configuration
  * @retval None
  */
static void SystemPower_Config(void)
{
  /* Configure the power domain */
  __HAL_RCC_PWR_CLK_ENABLE();
  if (HAL_PWREx_ConfigSupply(PWR_EXTERNAL_SOURCE_SUPPLY) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}
/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct = {0};

  __HAL_RCC_GPIOG_CLK_ENABLE();

  /* Configure default pin levels (LEDs OFF)*/
  HAL_GPIO_WritePin(GPIOG, LED1_Pin | LED2_Pin | LED3_Pin, GPIO_PIN_SET);

  /* Configure the LED pins */
  GPIO_InitStruct.Pin = LED1_Pin | LED2_Pin | LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */


  /* USART1 Init */

  huart1.Instance          = USART1;
  huart1.Init.BaudRate     = 115200;
  huart1.Init.Mode         = UART_MODE_TX_RX;
  huart1.Init.Parity       = UART_PARITY_NONE;
  huart1.Init.WordLength   = UART_WORDLENGTH_8B;
  huart1.Init.StopBits     = UART_STOPBITS_1;
  huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;

  HAL_UART_Init(&huart1);
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, 0xFFFF);

  return len;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User may add here some code to deal with this error */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
  /* Error message */
  printf("** Error occurred ** \n\r");
  while (1)
  {
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    HAL_Delay(1000);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
