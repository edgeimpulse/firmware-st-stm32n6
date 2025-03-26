/**
  ******************************************************************************
  * @file    Secure_nsclib/secure_nsc.h
  * @author  MCD Application Team
  * @brief   Header for secure non-secure callable APIs list
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
#ifndef SECURE_NSC_H
#define SECURE_NSC_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  non-secure callback ID enumeration definition
  */
typedef enum
{
  SECURE_FAULT_CB_ID     = 0x00U, /*!< System secure fault callback ID */
  IAC_ERROR_CB_ID        = 0x01U  /*!< IAC secure error callback ID */
} SECURE_CallbackIDTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SECURE_RegisterCallback(SECURE_CallbackIDTypeDef CallbackId, void *func);

/* Non-secure callable APIs for RCC validation purposes */
void              SECURE_SWITCH_UNPRIVILEGED(void);
void              SECURE_SWITCH_PRIVILEGED(void);
void              SECURE_HAL_RCC_ConfigAttributes(uint32_t Item, uint32_t Attributes);
void              SECURE_RCC_SECURE_ALL(void);
void              SECURE_RCC_UNSECURE_ALL(void);
void              SECURE_RCC_PRIVILEGED_ALL(void);
void              SECURE_RCC_UNPRIVILEGED_ALL(void);
void              SECURE_RCC_PUBLIC_ALL(void);
void              SECURE_RCC_NONPUBLIC_ALL(void);
void              SECURE_RCC_LOCK_ALL(void);
HAL_StatusTypeDef SECURE_ENABLE_HSI_CLK(void);
HAL_StatusTypeDef SECURE_DISABLE_HSI_CLK(void);
HAL_StatusTypeDef SECURE_ENABLE_HSE_CLK(void);
HAL_StatusTypeDef SECURE_DISABLE_HSE_CLK(void);
HAL_StatusTypeDef SECURE_ENABLE_MSI_CLK(void);
HAL_StatusTypeDef SECURE_DISABLE_MSI_CLK(void);
HAL_StatusTypeDef SECURE_ENABLE_LSI_CLK(void);
HAL_StatusTypeDef SECURE_DISABLE_LSI_CLK(void);
HAL_StatusTypeDef SECURE_ENABLE_LSE_CLK(void);
HAL_StatusTypeDef SECURE_DISABLE_LSE_CLK(void);
void SECURE_SETUP_CLK_ENABLE_TEST(void);
void SECURE_TEARDOWN_CLK_ENABLE_TEST(void);
uint32_t SECURE_CHECK_PERIF_ILLEGAL_ACCESS(void);

#endif /* SECURE_NSC_H */
