/**
  ******************************************************************************
  * @file    interfaces_conf.h
  * @author  MCD Application Team
  * @brief   Contains Interfaces configuration
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
#ifndef INTERFACES_CONF_H
#define INTERFACES_CONF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_ll_usart.h"
#include "stm32n6xx_ll_gpio.h"
#include "stm32n6xx_ll_rcc.h"

#define MEMORIES_SUPPORTED                3U

/*-------------------------- Definitions for USART -------------------------- */
#define USARTx                            USART1
#define USARTx_CLK_ENABLE()               __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_CLK_DISABLE()              __HAL_RCC_USART1_CLK_DISABLE()
#define USARTx_GPIO_CLK_TX_ENABLE()       __HAL_RCC_GPIOE_CLK_ENABLE()
#define USARTx_GPIO_CLK_RX_ENABLE()       __HAL_RCC_GPIOE_CLK_ENABLE()
#define USARTx_DEINIT()                   LL_USART_DeInit(USARTx)

#define USARTx_TX_PIN                     GPIO_PIN_5
#define USARTx_TX_GPIO_PORT               GPIOE
#define USARTx_RX_PIN                     GPIO_PIN_6
#define USARTx_RX_GPIO_PORT               GPIOE
#define USARTx_ALTERNATE                  GPIO_AF7_USART1

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* INTERFACES_CONF_H */
