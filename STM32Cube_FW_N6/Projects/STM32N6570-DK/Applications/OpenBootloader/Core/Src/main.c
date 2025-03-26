/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main program body
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
#include "main.h"
#include "app_openbootloader.h"
#include "interfaces_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

/* Private functions ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/

/**
  * @brief  Main program.
  * @retval None.
  */
int main(void)
{
  /* Reset of all peripherals, initializes the systick. */
  HAL_Init();

  /* Configure the System clock */
  SystemClock_Config();

  /* Initialize the OpenBootloader */
  OpenBootloader_Init();

  while (true)
  {
    OpenBootloader_ProtocolDetection();
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            CPU Clock source               = IC1_CK
  *            System bus Clock source        = IC2_IC6_IC11_CK
  *            CPUCLK (sysa_ck) (Hz)          = 400000000
  *            SYSCLK AXI (sysb_ck) (Hz)      = 400000000
  *            SYSCLK NPU (sysc_ck) (Hz)      = 400000000
  *            AHB Prescaler                  = 2
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            APB4 Prescaler                 = 1
  *            APB5 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 48000000
  *            PLL1 State                     = ON
  *            PLL1 clock source              = HSE
  *            PLL2 State                     = OFF
  *            PLL3 State                     = OFF
  *            PLL4 State                     = OFF
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Ensure a system de-initialization before configuring the RCC to avoid any initialization issues */
  System_DeInit();

  /* HSE selected as source (stable clock on Level 0 samples) */
  /* PLL1 output = ((HSE/PLLM)*PLLN)/PLLP1/PLLP2              */
  /*             = ((48000000/3)*50)/1/1                      */
  /*             = (16000000*50)/1/1                          */
  /*             = 800000000 (800 MHz)                        */
  /* PLL2 off                                                 */
  /* PLL3 off                                                 */
  /* PLL4 off                                                 */

  /* Enable HSE && HSI */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv              = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSEState            = RCC_HSE_ON;     /* 48 MHz */

  /* PLL1 configuration - 800MHz */
  RCC_OscInitStruct.PLL1.PLLState      = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource     = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL1.PLLM          = 3;
  RCC_OscInitStruct.PLL1.PLLN          = 50;    /* PLL1 VCO = 48/3 * 50 = 800MHz */
  RCC_OscInitStruct.PLL1.PLLP1         = 1;     /* PLL output = PLL VCO frequency / (PLLP1 * PLLP2) */
  RCC_OscInitStruct.PLL1.PLLP2         = 1;     /* PLL output = 800 MHz */
  RCC_OscInitStruct.PLL1.PLLFractional = 0;

  /* PLL2 & PLL3 & PLL4 set to off */
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_OFF;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_OFF;
  RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_OFF;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization error */
    Error_Handler();
  }

  /* Select PLL1 outputs as CPU and System bus clock source */
  /* CPUCLK = ic1_ck = PLL1 output/ic1_divider = 400 MHz */
  /* SYSCLK AXI (sysb_ck) = ic2_ck = PLL1 output/ic2_divider = 400 MHz */
  /* SYSCLK NPU (sysc_ck) = ic6_ck = PLL1 output/ic6_divider = 400 MHz */
  /* SYSCLK AXISRAM3/4/5/6 (sysd_ck) = ic11_ck = PLL1 output/ic11_divider = 400 MHz */
  /* Configure the HCLK, PCLK1, PCLK2, PCLK4 and PCLK5 clocks dividers */
  /* HCLK = ic2_ck = PLL1 output/HCLK divider = 200 MHz */
  /* PCLKx = HCLK / PCLKx divider = 200 MHz */
  RCC_ClkInitStruct.ClockType                    = (RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
                                                    RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK4 |  \
                                                    RCC_CLOCKTYPE_PCLK5);
  RCC_ClkInitStruct.CPUCLKSource                 = RCC_CPUCLKSOURCE_IC1;
  RCC_ClkInitStruct.SYSCLKSource                 = RCC_SYSCLKSOURCE_IC2_IC6_IC11;

  RCC_ClkInitStruct.IC1Selection.ClockSelection  = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC1Selection.ClockDivider    = 2;

  RCC_ClkInitStruct.IC11Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC11Selection.ClockDivider   = 2;

  RCC_ClkInitStruct.IC2Selection.ClockSelection  = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC2Selection.ClockDivider    = 2;

  RCC_ClkInitStruct.IC6Selection.ClockSelection  = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC6Selection.ClockDivider    = 2;

  RCC_ClkInitStruct.AHBCLKDivider                = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider               = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider               = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider               = RCC_APB4_DIV1;
  RCC_ClkInitStruct.APB5CLKDivider               = RCC_APB5_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

/**
  * @brief  This function is used to de-initialized the clock.
  * @retval None.
  */
void System_DeInit(void)
{
  HAL_RCC_DeInit();

  /* Disable interfaces */
  USARTx_DEINIT();

  /* Disable USB interrupt */
  HAL_NVIC_DisableIRQ(USB1_OTG_HS_IRQn);

  __HAL_RCC_USB1_OTG_HS_CLK_DISABLE();

  __HAL_RCC_USB1_OTG_HS_FORCE_RESET();
  __HAL_RCC_USB1_OTG_HS_RELEASE_RESET();

  HAL_PWREx_DisableVddUSBVMEN();
  HAL_PWREx_DisableVddUSB();

  __HAL_RCC_PWR_CLK_DISABLE();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None.
  */
void Error_Handler(void)
{
  /* Handle the error by performing a System Reset.or an Infinite loop
      The user can add his own error handler.
   */
  while (true);
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (true)
  {
  }
}
#endif /* USE_FULL_ASSERT */
