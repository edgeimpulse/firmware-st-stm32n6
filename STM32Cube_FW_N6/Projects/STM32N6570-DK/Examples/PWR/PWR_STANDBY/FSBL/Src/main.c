/**
  ******************************************************************************
  * @file    PWR_STANDBY/FSBL/Src/main.c
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
#include "extmem.h"

/** @addtogroup STM32N6xx_HAL_Examples
  * @{
  */

/** @addtogroup PWR_STANDBY_FSBL
  * @{
  */

XSPI_HandleTypeDef       hxspi2;

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define XSPI2_MAP_ADDRESS 0x70000000
#define NOR_FLASH_SIZE 0x8000000
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
void Error_Handler(void);
static void MX_XSPI2_Init(void);
static void MPU_config(void);

#ifndef NO_OTP_FUSE
static int32_t OTP_Config(void);
#endif
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Enable and configure the MPU---------------------------------------------*/
  MPU_config();

  /* Enable I-Cache-----------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache-----------------------------------------------------------*/
  SCB_EnableDCache();

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


  /* Configure the system clock */
  SystemClock_Config();

  /* can not be set currently under boot from flash due to bootrom lock */
#ifndef NO_OTP_FUSE
  /* Set OTP fuses for XSPI IO pins speed optimization */
  if(OTP_Config() != 0){
    Error_Handler();
  }
#endif /* NO_OTP_FUSE */

  /* Add your application code here */
  MX_XSPI2_Init();

  /* Initialise the serial memory */
  MX_EXTMEM_Init();

  BOOT_Application();
  /* We should never get here as execution is now from user application */
  while(1)
  {
    __NOP();
  }
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

  /* Configure the system Power Supply */
  if (HAL_PWREx_ConfigSupply(PWR_EXTERNAL_SOURCE_SUPPLY) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

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

#ifndef NO_OTP_FUSE
/**
  * @brief  User OTP fuse Configuration
  *         The User Option Bytes are configured as follows :
  *            VDDIO_HSLV = 1 (enable the configuration of pads below 2.5V,
  *                            I/O speed otpmization at low-voltage allowed)
  *            XSPI1_HSLV = 1 (enable I/O XSPIM Port 1 high-speed option)
  *            XSPI2_HSLV = 1 (enable I/O XSPIM Port 2 high-speed option)
  *            Other User Option Bytes remain unchanged
  * @retval None
  */
static int32_t OTP_Config(void)
{
  #define BSEC_HW_CONFIG_ID        124U
  #define BSEC_HWS_HSLV_VDDIO3     (1U<<15)
  #define BSEC_HWS_HSLV_VDDIO2     (1U<<16)
  
  uint32_t fuse_id, bit_mask, data;
  BSEC_HandleTypeDef sBsecHandler;
  int32_t retr = 0;

  /* Enable BSEC & SYSCFG clocks to ensure BSEC data accesses */
  __HAL_RCC_BSEC_CLK_ENABLE();
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  sBsecHandler.Instance = BSEC;

  /* Read current value of fuse */
  fuse_id = BSEC_HW_CONFIG_ID;
  if (HAL_BSEC_OTP_Read(&sBsecHandler, fuse_id, &data) == HAL_OK)
  {
    /* Check if bit has already been set */
    bit_mask = BSEC_HWS_HSLV_VDDIO3 | BSEC_HWS_HSLV_VDDIO2;
    if ((data & bit_mask) != bit_mask)
    {
      data |= bit_mask;
      /* Bitwise programming of lower bits */
      if (HAL_BSEC_ProgramOTPFuse(&sBsecHandler, fuse_id, data, HAL_BSEC_NORMAL_PROG) == HAL_OK)
      {
        /* Read lower bits to verify the correct programming */
        if (HAL_BSEC_OTP_Read(&sBsecHandler, fuse_id, &data) == HAL_OK)
        {
          if ((data & bit_mask) != bit_mask)
          {
            /* Error : Fuse programming not taken in account */
            retr = -1;
          }
        }
        else
        {
          /* Error : Fuse read unsuccessful */
          retr = -2;
        }
      }
      else
      {
        /* Error : Fuse programming unsuccessful */
        retr = -3;
      }
    }
  }
  else
  {
    /* Error  : Fuse read unsuccessful */
    retr = -4;
  }
  return retr;
}
#endif

static void MPU_config(void){
  MPU_Region_InitTypeDef default_config = {0};
  MPU_Attributes_InitTypeDef attr_config = {0};
  uint32_t primask_bit = __get_PRIMASK();
  __disable_irq();

  /* disable the MPU */
  HAL_MPU_Disable();

  /* create an attribute configuration for the MPU */
  attr_config.Attributes = 0;
  attr_config.Number = MPU_ATTRIBUTES_NUMBER1;

  HAL_MPU_ConfigMemoryAttributes(&attr_config);

  /* Create a region associated with memory address 0x70000000 */
  /* TEX=1, Normal memory type, code execution allowed */
  default_config.Enable = MPU_REGION_ENABLE;
  default_config.Number = MPU_REGION_NUMBER1;
  default_config.BaseAddress = 0x70000000;
  default_config.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  default_config.LimitAddress = 0x70000000;
  default_config.AccessPermission = MPU_REGION_ALL_RO;
  default_config.IsShareable = MPU_ACCESS_INNER_SHAREABLE;
  default_config.AttributesIndex = MPU_ATTRIBUTES_NUMBER1;
  HAL_MPU_ConfigRegion(&default_config);

  /* Create a region associated with memory address 0x70000000 */
  /* TEX=1, Normal memory type, code execution allowed */
  default_config.Enable = MPU_REGION_ENABLE;
  default_config.Number = MPU_REGION_NUMBER2;
  default_config.BaseAddress = 0x34000000;
  default_config.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  default_config.LimitAddress = 0x34030000;
  default_config.AccessPermission = MPU_REGION_ALL_RW;
  default_config.IsShareable = MPU_ACCESS_INNER_SHAREABLE;
  default_config.AttributesIndex = MPU_ATTRIBUTES_NUMBER1;
  HAL_MPU_ConfigRegion(&default_config);

  /* enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

  /* Exit critical section to lock the system and avoid any issue around MPU mechanisme */
  __set_PRIMASK(primask_bit);
}


/**
  * @brief XSPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_XSPI2_Init(void)
{
  hxspi2.Instance = XSPI2;

  /* XSPI initialization */
  hxspi2.Init.FifoThresholdByte       = 4U;
  hxspi2.Init.MemoryMode              = HAL_XSPI_SINGLE_MEM;
  hxspi2.Init.MemoryType              = HAL_XSPI_MEMTYPE_MACRONIX;
  hxspi2.Init.MemorySize              = HAL_XSPI_SIZE_32GB;
  hxspi2.Init.ChipSelectHighTimeCycle = 2U;
  hxspi2.Init.FreeRunningClock        = HAL_XSPI_FREERUNCLK_DISABLE;
  hxspi2.Init.ClockMode               = HAL_XSPI_CLOCK_MODE_0;
  hxspi2.Init.WrapSize                = HAL_XSPI_WRAP_NOT_SUPPORTED;
  hxspi2.Init.ClockPrescaler          = 0U;
  hxspi2.Init.SampleShifting          = HAL_XSPI_SAMPLE_SHIFT_NONE;
  hxspi2.Init.DelayHoldQuarterCycle   = HAL_XSPI_DHQC_ENABLE;
  hxspi2.Init.ChipSelectBoundary      = HAL_XSPI_BONDARYOF_NONE;
  hxspi2.Init.MaxTran                 = 0U;
  hxspi2.Init.Refresh                 = 0U;
  hxspi2.Init.MemorySelect            = HAL_XSPI_CSSEL_NCS1;

  if (HAL_XSPI_Init(&hxspi2) != HAL_OK)
  {
    Error_Handler();
  }
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
