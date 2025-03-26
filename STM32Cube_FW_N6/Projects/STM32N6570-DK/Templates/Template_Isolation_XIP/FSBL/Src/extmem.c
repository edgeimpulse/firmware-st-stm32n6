/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : Template_Isolation_XIP/FSBL/Src/extmem.c
  * @brief          : This file implements the external memory manager
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "extmem.h"
#include "stm32_boot_xip.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* Headers 2.1 and 2.3 information for ST firmware */
#define HEADER_V2_1_IMG_SIZE_OFFSET 76
#define HEADER_V2_3_IMG_SIZE_OFFSET 108
#define HEADER_V2_1_SIZE 1024
#define HEADER_V2_3_SIZE 1024

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init Secure Manager API
  * @retval None
  */
void MX_EXTMEM_Init(void)
{

  /* USER CODE BEGIN MX_EXTMEM_Init_PreTreatment */
    
  /* USER CODE END MX_EXTMEM_Init_PreTreatment */

  EXTMEM_Init(EXTMEMORY_1, HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI2));
//  EXTMEM_Init(EXTMEMORY_2, HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI1));

  /* USER CODE BEGIN MX_EXTMEM_Init_PostTreatment */

  /* USER CODE END MX_EXTMEM_Init_PostTreatment */
}
