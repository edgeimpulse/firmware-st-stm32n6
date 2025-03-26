/**
  ******************************************************************************
  * @file    otp_interface.c
  * @author  MCD Application Team
  * @brief   Contains One-time programmable access functions
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
#include "platform.h"
#include "common_interface.h"
#include "openbl_core.h"

#include "openbl_mem.h"

#include "app_openbootloader.h"
#include "otp_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize the OTP driver.
  * @retval None.
  */
void OPENBL_OTP_Init(void)
{
  OTP_Util_Init();
}

/**
  * @brief  De-initialize the OTP driver.
  * @retval None.
  */
void OPENBL_OTP_DeInit(void)
{
  OTP_Util_DeInit();
}

/**
  * @brief  Read the OTP area.
  * @retval Returns the OTP structure that contains the read values.
  */
Otp_Partition_t OPENBL_OTP_Read(void)
{
  Otp_Partition_t ret = OTP_Util_Read();

  return ret;
}

/**
  * @brief  Write in OTP area.
  * @param  Otp the otp structure that contains the values to be written.
  * @retval Returns OTP_OK if no error else returns OTP_ERROR.
  */
int OPENBL_OTP_Write(Otp_Partition_t Otp)
{
  int ret = OTP_Util_Write(Otp);

  return ret;
}
