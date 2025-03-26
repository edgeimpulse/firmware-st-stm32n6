/**
 ******************************************************************************
 * @file    isp_conf_template.h
 * @author  AIS Application Team
 * @brief   Header configuration file - template
 * This file must be copied into the application folder and modified as follows:
 * - add includes of the HAL / BSP header files of the target.
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
#ifndef __ISP_CONF_H
#define __ISP_CONF_H


/* Includes ------------------------------------------------------------------*/
#if defined (STM32N657xx)
#include "stm32n6xx_hal.h"
#elif defined (STM32MP257Fxx)
#include "stm32mp2xx_hal.h"
#elif defined (LINUX)
#include "iqtune-linux-wrapper.h"
#else
#error Add header files for your specific board
#endif

/* DCMIPP version defines */
/* TODO : Remove these definitions when the final implementation is available */
#if defined (STM32N657xx)
#define DCMIPP_MAJ_REV                       2U
#define DCMIPP_MIN_REV                       1U
#elif defined (STM32MP257Fxx)
#define DCMIPP_MAJ_REV                       2U
#define DCMIPP_MIN_REV                       0U
#endif

#endif /* __ISP_CONF_H */
