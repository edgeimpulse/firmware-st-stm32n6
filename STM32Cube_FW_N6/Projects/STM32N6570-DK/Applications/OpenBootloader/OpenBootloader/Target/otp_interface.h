/**
  ******************************************************************************
  * @file    otp_interface.h
  * @author  MCD Application Team
  * @brief   Header for otp_interface.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef OTP_INTERFACE_H
#define OTP_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "otp_util.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void OPENBL_OTP_Init(void);
void OPENBL_OTP_DeInit(void);
int OPENBL_OTP_Write(Otp_Partition_t Otp);
Otp_Partition_t OPENBL_OTP_Read(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPTIONBYTES_INTERFACE_H */
