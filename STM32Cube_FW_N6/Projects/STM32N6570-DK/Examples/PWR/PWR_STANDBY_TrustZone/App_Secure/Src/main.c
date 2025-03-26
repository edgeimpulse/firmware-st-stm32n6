/**
  ******************************************************************************
  * @file    PWR/PWR_STANDBY_TrustZone/App_Secure/Src/main.c
  * @author  GPM Application Team
  * @brief   Secure main program.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2023 STMicroelectronics</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32n6xx_it.h"

/** @addtogroup STM32N6xx_HAL_Template
  * @{
  */
/** @addtogroup HAL
  * @{
  */
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Non-secure Vector table to jump to                                         */
/* Caution: address must correspond to non-secure address as it is mapped in  */
/*          the non-secure vector table                                       */

/* Memory partitioning on STM32N6xx uses */
/* Secure code is stored from 0x7010'0000 to 0x7017ffff */
/* Non-Secure code is stored from 0x7018'0000 to 0x701fffff */
/* - AXI SRAM1 for Secure  data */
/* - AXI SRAM2 for Non-secure data */

#define VECT_TAB_NS_OFFSET  0x00180400
#define VTOR_TABLE_NS_START_ADDR (XSPI2_BASE|VECT_TAB_NS_OFFSET)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemIsolation_Config(void);
/* Global variables ----------------------------------------------------------*/
RTC_HandleTypeDef hrtc;
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  funcptr_NS NonSecure_ResetHandler;

  /* Enable I-Cache-----------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache-----------------------------------------------------------*/
  SCB_EnableDCache();

  /* Enable BusFault and SecureFault handlers (HardFault is default) */
  SCB->SHCSR |= (SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_SECUREFAULTENA_Msk);

  /* Reset of all peripherals, Initializes the Flash interface and the systick. */
  HAL_Init();

  /* Secure/Non-secure Memory and Peripheral isolation configuration */
  SystemIsolation_Config();

  /*************** Setup and jump to non-secure *******************************/

  /* Set non-secure vector table location */
  SCB_NS->VTOR = VTOR_TABLE_NS_START_ADDR;

  /* Set non-secure main stack (MSP_NS) */
  __TZ_set_MSP_NS((*(uint32_t *)VTOR_TABLE_NS_START_ADDR));

  /* Get non-secure reset handler */
  NonSecure_ResetHandler = (funcptr_NS)(*((uint32_t *)((VTOR_TABLE_NS_START_ADDR) + 4U)));

  /* Start non-secure state software application */
  NonSecure_ResetHandler();

  /* Non-secure software does not return, this code is not executed */
  while (1) {
    __NOP();
  }
}

/**
  * @brief  System Isolation Configuration
  *         This function is responsible for Memory and Peripheral isolation
  *         for secure and non-secure application parts
  * @retval None
  */
static void SystemIsolation_Config(void)
{
  RISAF_BaseRegionConfig_t risaf_conf;
  /* RISAF Config */
  __HAL_RCC_RISAF_CLK_ENABLE();

  /* set up base region configuration for AXISRAM1 and 2 */
  risaf_conf.StartAddress = 0;

  risaf_conf.Filtering = RISAF_FILTER_ENABLE;
  risaf_conf.PrivWhitelist = RIF_CID_MASK;
  risaf_conf.ReadWhitelist = RIF_CID_MASK;
  risaf_conf.WriteWhitelist = RIF_CID_MASK;

  /* FLEXRAM is secure */
  risaf_conf.Secure = RIF_ATTRIBUTE_SEC;
  risaf_conf.EndAddress = 0x00063FFF;
  HAL_RIF_RISAF_ConfigBaseRegion(RISAF7, RISAF_REGION_1, &risaf_conf);
  
  /* AXISRAM1 is secure */
  risaf_conf.Secure = RIF_ATTRIBUTE_SEC;
  risaf_conf.EndAddress = 0x0009BFFF;
  HAL_RIF_RISAF_ConfigBaseRegion(RISAF2, RISAF_REGION_1, &risaf_conf);

  /* AXISRAM2 is non-secure */
  risaf_conf.Secure = RIF_ATTRIBUTE_NSEC;
  risaf_conf.EndAddress = 0x000FFFFF;
  HAL_RIF_RISAF_ConfigBaseRegion(RISAF3, RISAF_REGION_1, &risaf_conf);

  /* set up RISAF ROM regions for XSPI2 */
  risaf_conf.StartAddress = 0x00100000;
  risaf_conf.EndAddress = 0x0017FFFF;
  risaf_conf.Secure = RIF_ATTRIBUTE_SEC;
  HAL_RIF_RISAF_ConfigBaseRegion(RISAF12, RISAF_REGION_1, &risaf_conf);
  
  risaf_conf.StartAddress = 0x00180000;
  risaf_conf.EndAddress = 0x001FFFFF;
  risaf_conf.Secure = RIF_ATTRIBUTE_NSEC;
  HAL_RIF_RISAF_ConfigBaseRegion(RISAF12, RISAF_REGION_2, &risaf_conf);
  
  __HAL_RCC_RIFSC_CLK_ENABLE();
  /* Set GPIOO as configurable by non-secure */
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RCC_PERIPH_INDEX_GPIOO, RIF_ATTRIBUTE_NSEC);

  /* Enable GPIOO clock and ensure GPIOO bank is powered on */
  __HAL_RCC_GPIOO_CLK_ENABLE();
  /* Configure PO1 as non-secure to be used for non-secure led toggling */
  HAL_GPIO_ConfigPinAttributes(GPIOO, GPIO_PIN_1, GPIO_PIN_NSEC);
  
  /* Configure PWR_CPUCR as non-secure to be R/W for NSEC application*/
  HAL_PWR_ConfigAttributes(PWR_ITEM_6, PWR_NSEC_PRIV);
  
  /* Set RTC in secure mode except Init and Wake-up timer*/
  __HAL_RCC_RTC_CLK_ENABLE();
  RTC_SecureStateTypeDef  secureState = {0};
  secureState.rtcSecureFull = RTC_SECURE_FULL_NO;
  secureState.tampSecureFull = TAMP_SECURE_FULL_YES;
  secureState.rtcNonSecureFeatures = RTC_NONSECURE_FEATURE_INIT | RTC_NONSECURE_FEATURE_WUT;
  HAL_RTCEx_SecureModeSet(&hrtc, &secureState);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(file);
  UNUSED(line);

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
