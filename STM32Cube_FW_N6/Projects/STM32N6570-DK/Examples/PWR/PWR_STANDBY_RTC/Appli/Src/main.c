/**
  ******************************************************************************
  * @file    PWR_STANDBY_RTC/Appli/Src/main.c
  * @author  GPM Application Team
  * @brief   Main program body through the HAL API
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

/** @addtogroup STM32N6xx_FSBL_Appli_Template
  * @{
  */

/** @addtogroup Template_XIP_Appli
  * @{
  */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

/* Private function prototypes -----------------------------------------------*/
void Error_Handler(void);
uint32_t Generate_RTCWUTimer_Interrupt(uint32_t WakeUpCounter);

static void SystemClock_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @retval None
  */
int main(void)
{
  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* System clock already configured, simply SystemCoreClock init */
  SystemCoreClockUpdate();

  /* MCU Configuration--------------------------------------------------------*/

  /* STM32N6xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */ 
  HAL_Init();

  /* Initialize LED2 */
  BSP_LED_Init(LED_RED);

    /* Check low power flags */
  if ((__HAL_PWR_GET_FLAG(PWR_FLAG_SBF) == 0U))
  {
    /* first run, blink slowly */
    for(int i = 0; i < 10; i++)
    {
      /* Toggle LED2 every 500ms */
      BSP_LED_Toggle(LED_RED);
      HAL_Delay(500);
    }

  } else
  {
    /* If returning from standby, system initialization needs to be done */

    /* clear standby flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SBF);

    /* Configure the system Power Supply */
    if (HAL_PWREx_ConfigSupply(PWR_EXTERNAL_SOURCE_SUPPLY) != HAL_OK)
    {
      /* Initialization Error */
      Error_Handler();
    }
    /* Configure Voltage Scaling */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
    {
      /* Initialization Error */
      Error_Handler();
    }

    /* clocks need to be configured because FSBL does not execute after standby */
    SystemClock_Config();

    for(int i = 0; i<50; i++){
      /* Toggle LED2 every 100ms */
      BSP_LED_Toggle(LED_RED);
      HAL_Delay(100);
    }
  }

  /* enable memory retention top keep application in standby */
  HAL_PWREx_EnableTCMRetention();
  HAL_PWREx_EnableTCMFLXRetention();

  /* wake up the system in 5s (5000ms) */
  Generate_RTCWUTimer_Interrupt(5000);
  /* Enter STANDBY mode */
  HAL_PWR_EnterSTANDBYMode();

  /* should never get here */
  while(1);
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
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
  *            PLL1 State                     = ON
  *            PLL1 clock source              = HSI
  *            PLL1 M                         = 8
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Get current CPU/System buses clocks configuration */
  /* and if necessary switch to intermediate HSI clock */
  /* to ensure target clock can be set                 */
  HAL_RCC_GetClockConfig(&RCC_ClkInitStruct);
  if ((RCC_ClkInitStruct.CPUCLKSource == RCC_CPUCLKSOURCE_IC1) ||
      (RCC_ClkInitStruct.SYSCLKSource == RCC_SYSCLKSOURCE_IC2_IC6_IC11))
  {
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_SYSCLK);
    RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_HSI;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
    {
      /* Initialization Error */
      Error_Handler();
    }
  }

  /* HSI selected as PLL1 source                             */
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
 * @brief  Generate RTC Wake Up Timer Event.
 * @retval
 */
uint32_t Generate_RTCWUTimer_Interrupt(uint32_t TimeToWKUP)
{

  /* Set RTC Instance */
  hrtc.Instance = RTC;

  /* Set parameter to be configured */
  hrtc.Init.HourFormat     = RTC_HOURFORMAT_12;
  hrtc.Init.AsynchPrediv   = 0x7FU;
  hrtc.Init.SynchPrediv    = 0xF9U;
  hrtc.Init.OutPut         = RTC_OUTPUT_WAKEUP;
  hrtc.Init.OutPutRemap    = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType     = RTC_OUTPUT_TYPE_PUSHPULL;

  /* Initialize the RTC peripheral */
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    return 1; /* KO */
  }
  
   /* Set RTC full secure mode */
  RTC_SecureStateTypeDef  secureState = {0};
  secureState.rtcSecureFull = RTC_SECURE_FULL_YES;
  secureState.tampSecureFull = TAMP_SECURE_FULL_YES;
  HAL_RTCEx_SecureModeSet(&hrtc, &secureState);
  /*
      LSI = 32KHz as RTC clock source.
      LSI / RTC_WAKEUPCLOCK_RTCCLK_DIV16 = 2KHz.
      => 0.5 * WakeUpCounter = TimeToWKUP (ms).
      => WakeUpCounter = TimeTOWKUP*2
  */
  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, TimeToWKUP*2, RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0) != HAL_OK)
  {
    return 2; /* KO */
  }

 return 0; /* OK */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */
