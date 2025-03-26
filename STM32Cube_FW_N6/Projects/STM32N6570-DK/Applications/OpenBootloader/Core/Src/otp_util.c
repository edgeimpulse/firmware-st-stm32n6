/**
  ******************************************************************************
  * @file    otp_util.c
  * @author  MCD Application Team
  * @brief   OTP utility functions
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "otp_util.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
BSEC_HandleTypeDef handleBsec;
BSEC_HandleTypeDef *hbsec = &handleBsec;
static uint32_t otp_val_idx;
static uint32_t otp_stat_idx;
static uint32_t otp_idx;
static HAL_StatusTypeDef status;

/* Exported variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Set BSEC instance and enable needed clocks.
  * @retval None.
  */
void OTP_Util_Init(void)
{
  hbsec->Instance = BSEC;

  HAL_PWR_EnableBkUpAccess();

  __HAL_RCC_BSEC_CLK_ENABLE();
  __HAL_RCC_RTCAPB_CLK_ENABLE();
}

/**
  * @brief  Disable BSEC clock.
  * @retval None
  */
void OTP_Util_DeInit(void)
{
  HAL_PWR_DisableBkUpAccess();

  __HAL_RCC_BSEC_CLK_DISABLE();
  __HAL_RCC_RTCAPB_CLK_DISABLE();
}

/**
  * @brief  Write in the OTP area.
  * @param  Otp the otp structure.
  * @retval OTP_OK if no error else returns OTP_ERROR.
  */
int OTP_Util_Write(Otp_Partition_t Otp)
{
  uint32_t valueW;
  uint32_t otpPermWLockValue;
  uint32_t stickyLockValue;
  uint32_t requestUpdateValue;

  for (otp_val_idx = 0, otp_stat_idx = otp_val_idx + 1, otp_idx = (otp_val_idx / 2);
       otp_val_idx < OTP_PART_SIZE;
       otp_val_idx += 2, otp_stat_idx = otp_val_idx + 1, otp_idx = (otp_val_idx / 2))
  {
    /* Get the request update bit */
    requestUpdateValue = (uint8_t)((Otp.OtpPart[otp_stat_idx] & OTP_REQUEST_UPDATE_MASK) >> OTP_REQUEST_UPDATE_POS);

    /* Check if there is request update */
    if (requestUpdateValue != OTP_REQUEST_UPDATE)
    {
      /* Continue to the next otp as no update on this otp */
      continue;
    }

    /*### Program the otp value ###*/
    /* Get the otp value */
    valueW = Otp.OtpPart[otp_val_idx];

    /* Skip write value if value = 0 */
    if (valueW != 0U)
    {
      /* Get the permanent lock value */
      otpPermWLockValue = (uint8_t)((Otp.OtpPart[otp_stat_idx] & OTP_PERM_LOCK_MASK) >> OTP_PERM_LOCK_POS);

      if (otpPermWLockValue == OTP_PERM_LOCK)
      {
        /* Write the otp value with permanent lock */
        status = HAL_BSEC_OTP_Program(hbsec, otp_idx, valueW, HAL_BSEC_LOCK_PROG);
      }
      else if ((otpPermWLockValue != OTP_PERM_LOCK))
      {
        /* Write the otp value */
        status = HAL_BSEC_OTP_Program(hbsec, otp_idx, valueW, HAL_BSEC_NORMAL_PROG);
      }

      if (status != HAL_OK)
      {
        return OTP_ERROR;
      }
    }

    /* ### Program the otp status ### */
    /* Status frame useful values */
    /* bit 0  = read error detected, 1 => invalid value */
    /* bit 26 = lock error */
    /* bit 27 = sticky programming lock */
    /* bit 28 = shadow write sticky lock */
    /* bit 29 = shadow read sticky lock */
    /* bit 30 = permanent write lock */
    /* bit 31 = request update */
    /* ############################## */

    /* Get the sticky lock value (1 = bit 27, 2 = bit 28, 4 = bit 29) */
    stickyLockValue = (uint8_t)((Otp.OtpPart[otp_stat_idx] & OTP_STICKY_LOCK_MASK) >> OTP_STICKY_LOCK_POS);

    /* Check if there is a sticky lock */
    if ((stickyLockValue & OTP_STICKY_LOCK_ALL) != 0U)
    {
      /* Program the sticky lock */
      status = HAL_BSEC_OTP_Lock(hbsec, otp_idx, stickyLockValue);

      /* Check the status */
      if (status != HAL_OK)
      {
        return OTP_ERROR;
      }
    }
  }

  return OTP_OK;
}

/**
  * @brief  Read the OTP area.
  * @retval Returns Otp structure.
  */
Otp_Partition_t OTP_Util_Read(void)
{
  uint32_t valueR;
  uint32_t stickyLockR;
  uint32_t otpPermWLockR;
  uint32_t state;
  uint32_t statusR;
  uint32_t secR = 0U;
  Otp_Partition_t Otp = {0};

  /* Get the otp version */
  Otp.Version = OPENBL_OTP_VERSION;

  /* Get security status */
  status = HAL_BSEC_GetDeviceLifeCycleState(hbsec, &secR);

  /* Check the status */
  if (status != HAL_OK)
  {
    /* Save otp security as invalid */
    secR = HAL_BSEC_INVALID_STATE;
  }

  /* Set the otp security */
  if (secR == HAL_BSEC_OPEN_STATE)
  {
    Otp.GlobalState = BSEC_SEC_OTP_OPEN;
  }
  else if (secR == HAL_BSEC_CLOSED_STATE)
  {
    Otp.GlobalState = BSEC_SEC_OTP_CLOSED;
  }
  else
  {
    Otp.GlobalState = BSEC_SEC_OTP_INVALID;
  }

  /* Get the otp values */
  for (otp_val_idx = 0, otp_stat_idx = otp_val_idx + 1, otp_idx = (otp_val_idx / 2);
       otp_val_idx < OTP_PART_SIZE;
       otp_val_idx += 2, otp_stat_idx = otp_val_idx + 1, otp_idx = (otp_val_idx / 2))
  {
    /* Reset the read status variable */
    statusR = 0;

    /* Read the otp word */
    status = HAL_BSEC_OTP_Read(hbsec, otp_idx, &valueR);

    /* Save the otp value */
    Otp.OtpPart[otp_val_idx] = valueR;

    /* Check the status */
    if (status != HAL_OK)
    {
      /* Save status as read error */
      statusR += OTP_READ_ERROR;

      /* Save the otp value */
      Otp.OtpPart[otp_stat_idx] = statusR;

      /* Update the otp value to 0 as read error */
      Otp.OtpPart[otp_val_idx] = 0;

      /* Continue to the next otp */
      continue;
    }

    /* Get the fuse lock status */
    status = HAL_BSEC_OTP_GetState(hbsec, otp_idx, &state);

    /* Check the status */
    if (status != HAL_OK)
    {
      /* Save status as lock error */
      statusR += OTP_LOCK_ERROR;

      /* Save the otp value */
      Otp.OtpPart[otp_stat_idx] = statusR;

      /* Continue to the next otp */
      continue;
    }

    /* Check the sticky lock status */
    if ((state & HAL_BSEC_FUSE_PROG_LOCKED) == HAL_BSEC_FUSE_PROG_LOCKED)
    {
      stickyLockR = 1U;
    }
    else
    {
      stickyLockR = 0U;
    }

    /* Check the permanent lock status */
    if ((state & HAL_BSEC_FUSE_LOCKED) == HAL_BSEC_FUSE_LOCKED)
    {
      otpPermWLockR = 1U;
    }
    else
    {
      otpPermWLockR = 0U;
    }

    /* Set the otp status */
    Otp.OtpPart[otp_stat_idx] = (stickyLockR << OTP_STICKY_LOCK_POS) + (otpPermWLockR << OTP_PERM_LOCK_POS);
  }

  return Otp;
}
