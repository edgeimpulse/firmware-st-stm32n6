/**
  ******************************************************************************
  * @file    Template/Inc/main.h 
  * @author  GPM Application Team
  * @brief   Header for main.c module
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
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
/* LL drivers common to all LL examples */
#include "stm32n6xx_ll_bus.h"
#include "stm32n6xx_ll_rcc.h"
#include "stm32n6xx_ll_system.h"
#include "stm32n6xx_ll_utils.h"
#include "stm32n6xx_ll_pwr.h"
#include "stm32n6xx_ll_exti.h"
#include "stm32n6xx_ll_gpio.h"

/* LL drivers specific to LL examples IPs */
#include "stm32n6xx_ll_adc.h"
#include "stm32n6xx_ll_cortex.h"
#include "stm32n6xx_ll_crc.h"
#include "stm32n6xx_ll_dma.h"
#include "stm32n6xx_ll_dma2d.h"
#include "stm32n6xx_ll_fmc.h"
#include "stm32n6xx_ll_i2c.h"
#include "stm32n6xx_ll_i3c.h"
#include "stm32n6xx_ll_iwdg.h"
#include "stm32n6xx_ll_lptim.h"
#include "stm32n6xx_ll_lpuart.h"
#include "stm32n6xx_ll_pka.h"
#include "stm32n6xx_ll_rng.h"
#include "stm32n6xx_ll_rtc.h"
#include "stm32n6xx_ll_spi.h"
#include "stm32n6xx_ll_tim.h"
#include "stm32n6xx_ll_usart.h"
#include "stm32n6xx_ll_wwdg.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* MAIN_H */

