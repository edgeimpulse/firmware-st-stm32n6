/**
  ******************************************************************************
  * @file    bl2_nv_service.c
  * @author  MCD Application Team
  * @brief   This file provides all the Non volatile firmware functions.
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
#include "boot_hal_cfg.h"
#include "string.h"
#include "platform/include/plat_nv_counters.h"
#include "bootutil/bootutil_log.h"

#define NV_CNT_ID_MAX_VALUE 3
#define NV_CNT_MAX_VALUE 127
#define NV_CNT_OTP_START_VALUE 60

/* Private configuration  -----------------------------------------------*/
HAL_StatusTypeDef plat_init_nv_counter(void)
{
  /* Enable BSEC & SYSCFG clocks to ensure BSEC data accesses */
  __HAL_RCC_BSEC_CLK_ENABLE();
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  return HAL_OK;
}

HAL_StatusTypeDef plat_set_nv_counter(enum nv_counter_t CounterId,
                                      uint32_t Data, uint32_t *Updated)
{
  BSEC_HandleTypeDef sBsecHandler = {0};
  uint32_t otp_value[4] = {0};
  int32_t i;

  /* Reset Updated flag */
  *Updated = 0U;

  if ((CounterId > NV_CNT_ID_MAX_VALUE) || (Data > NV_CNT_MAX_VALUE))
  {
    return HAL_ERROR;
  }

  otp_value[3] = (1 << (Data - 96));
  otp_value[2] = (1 << (Data - 65));
  otp_value[1] = (1 << (Data - 33));
  otp_value[0] = (1 << (Data - 1));

  sBsecHandler.Instance = BSEC;

  for (i = 0; i < 4; i++)
  {
    if (HAL_BSEC_OTP_Program(&sBsecHandler, NV_CNT_OTP_START_VALUE + 4 * CounterId + i, otp_value[i], HAL_BSEC_NORMAL_PROG) != HAL_OK)
    {
      return HAL_ERROR;
    }
  }

  /* Set updated flag */
  *Updated = 1U;

  BOOT_LOG_INF("Counter %d set to 0x%x", CounterId, Data);

  return HAL_OK;
}

HAL_StatusTypeDef plat_read_nv_counter(enum nv_counter_t CounterId,
                                             uint32_t size, uint8_t *val)
{
  BSEC_HandleTypeDef sBsecHandler = {0};
  uint32_t data = 0;
  uint32_t otp_value[4] = {0};
  int32_t i;
  uint64_t val1 = 0, val2 = 0;
  uint32_t val3 = 0;

  if (CounterId > NV_CNT_ID_MAX_VALUE)
  {
    return HAL_ERROR;
  }

  sBsecHandler.Instance = BSEC;

  for (i = 0; i < 4; i++)
  {
    if (HAL_BSEC_OTP_Read(&sBsecHandler, NV_CNT_OTP_START_VALUE + 4 * CounterId + i, &(otp_value[i])) != HAL_OK)
    {
      return HAL_ERROR;
    }
  }

  val1 = otp_value[1];
  val1 = val1 << 32;
  val1 |= otp_value[0];

  val2 = otp_value[3];
  val2 = val2 << 32;
  val2 |= otp_value[2];

  for (i = 63; i >= 0; i--)
  {
    val3 = (i > 0) ? (val1 >> i) : val1;
    if (val3)
    {
      data = i + 1;
      break;
    }
  }

  for (i = 63; i >= 0; i--)
  {
    val3 = (i > 0) ? (val2 >> i) : val2;
    if (val3)
    {
      data = i + 64;
      break;
    }
  }

  memcpy(val, &data, sizeof(uint32_t));

  BOOT_LOG_INF("Counter %d get 0x%x", CounterId, data);

  return HAL_OK;
}
