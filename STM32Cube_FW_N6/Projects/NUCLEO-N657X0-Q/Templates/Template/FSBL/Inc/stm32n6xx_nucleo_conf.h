/**
  ******************************************************************************
  * @file    Template/Inc/stm32n6xx_nucleo_conf.h
  * @author  GPM Application Team
  * @brief   STM32N6XX nucleo board configuration file.
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
#ifndef STM32N6XX_NUCLEO_CONF_H
#define STM32N6XX_NUCLEO_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/* Usage of nucleo board */
#define USE_NUCLEO_144      1U

/* Usage of COM feature */
#define USE_BSP_COM_FEATURE 1U
#define USE_COM_LOG         0U

/* Button user interrupt priority */
#define BSP_BUTTON_USER_IT_PRIORITY 0x0FUL  /* Default is lowest priority level */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6XX_NUCLEO_CONF_H */

