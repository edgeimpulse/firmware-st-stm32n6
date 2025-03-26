/**
  ******************************************************************************
  * @file    low_level_otp.c
  * @author  GPM Application Team
  * @brief   Low Level Interface module to access OTP bits
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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stm32n6xx.h"
#include "boot_hal_cfg.h"
#include "low_level_otp.h"

/* Private defines -----------------------------------------------------------*/
#define S_AUTH_PUB_KEY_HASH_OTP_NUMBER        268
#define NS_AUTH_PUB_KEY_HASH_OTP_NUMBER       268/*(S_AUTH_PUB_KEY_HASH_OTP_NUMBER + 8)*/
#define ENC_PRIV_KEY_OTP_NUMBER               276/*(NS_AUTH_PUB_KEY_HASH_OTP_NUMBER + 8)*/

/* Global variables ----------------------------------------------------------*/
uint8_t Secure_Authentication_Public_Key_HASH[SHA256_LENGTH];
uint8_t NonSecure_Authentication_Public_Key_HASH[SHA256_LENGTH];
uint8_t Encryption_Private_Key[ECDSA_256_PRIV_KEY_LENGTH];

/* Private function prototypes -----------------------------------------------*/
static void rev_memcpy(uint32_t *dest, const uint32_t *src, size_t n);

/* Functions Definition ------------------------------------------------------*/
void OTP_InitKeys(void)
{
  BSEC_HandleTypeDef sBsecHandler = {0};
  uint32_t otp_value[8] = {0};

  sBsecHandler.Instance = BSEC;

  /* Read secure authentication public key hash located in OTP 260-267 */
  for (uint32_t i = 0; i < 8; i++)
  {
    if (HAL_BSEC_OTP_Read(&sBsecHandler, S_AUTH_PUB_KEY_HASH_OTP_NUMBER + i, &(otp_value[i])) != HAL_OK)
    {
      Error_Handler();
    }
  }
  rev_memcpy((uint32_t*)Secure_Authentication_Public_Key_HASH, otp_value, 32);

  /* Read non-secure authentication public key hash located in OTP 268-275 */
  for (uint32_t i = 0; i < 8; i++)
  {
    if (HAL_BSEC_OTP_Read(&sBsecHandler, NS_AUTH_PUB_KEY_HASH_OTP_NUMBER + i, &(otp_value[i])) != HAL_OK)
    {
      Error_Handler();
    }
  }
  rev_memcpy((uint32_t*)NonSecure_Authentication_Public_Key_HASH, otp_value, 32);


  /* Read encryption private key located in OTP 276-283 */
  for (uint32_t i = 0; i < 8; i++)
  {
    if (HAL_BSEC_OTP_Read(&sBsecHandler, ENC_PRIV_KEY_OTP_NUMBER + i, &(otp_value[i])) != HAL_OK)
    {
      Error_Handler();
    }
  }
  rev_memcpy((uint32_t*)Encryption_Private_Key, otp_value, 32);
}

static void rev_memcpy(uint32_t *dest, const uint32_t *src, size_t n)
{
  uint32_t i = 0;
  
  /* Copy n bytes from source to destination by reversing endianness*/
  for (i = 0; i < n; i++)
  {
    dest[i] = __REV(src[i]);
  }
}
