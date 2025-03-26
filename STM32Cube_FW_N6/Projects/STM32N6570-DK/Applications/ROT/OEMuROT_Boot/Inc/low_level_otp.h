/**
  ******************************************************************************
  * @file    low_level_otp.h
  * @author  GPM Application Team
  * @brief   Header for low_level_otp.c module
  *
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LOW_LEVEL_OTP_H
#define LOW_LEVEL_OTP_H

#ifdef __cplusplus
extern "C" {
#endif
/* Exported constants --------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
#define ECDSA_256_PRIV_KEY_LENGTH (32U)
#define SHA256_LENGTH             (32U)

/* External variables --------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */
void OTP_InitKeys(void);

#ifdef __cplusplus
}
#endif

#endif /* LOW_LEVEL_OTP_H */
