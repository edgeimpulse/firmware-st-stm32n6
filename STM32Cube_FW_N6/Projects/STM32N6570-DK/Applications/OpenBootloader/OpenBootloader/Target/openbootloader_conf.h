/**
  ******************************************************************************
  * @file    openbootloader_conf.h
  * @author  MCD Application Team
  * @brief   Contains Open Bootloader configuration
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
#ifndef OPENBOOTLOADER_CONF_H
#define OPENBOOTLOADER_CONF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus*/

/* Includes ------------------------------------------------------------------*/
#include "platform.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* -------------------------------- Device ID --------------------------------*/
#define DEVICE_ID_MSB                     0x04U  /* MSB byte of device ID */
#define DEVICE_ID_LSB                     0x86U  /* LSB byte of device ID */

/*--------------------------- Definitions for Memories -----------------------*/
#define RAM_MEM_START_ADDRESS             SRAM2_AXI_BASE_S + 0xB9000U                 /* Start of RAM space in SRAM2 */
#define RAM_MEM_SIZE                      SRAM2_AXI_SIZE - 0xB9000U                   /* Size of RAM space - 284K */
#define RAM_MEM_END_ADDRESS               (RAM_MEM_START_ADDRESS + RAM_MEM_SIZE - 1U) /* End address of RAM  */

#define FLASHLAYOUT_ADDRESS               RAM_MEM_START_ADDRESS                       /* Reserved for flashlayout */
#define FLASHLAYOUT_SIZE                  1024U                                       /* Size of of flashlayout area */
  
#define RAM_WRITE_ADDRESS                 RAM_MEM_START_ADDRESS + FLASHLAYOUT_SIZE    /* Available space in RAM */

#define OPENBL_DEFAULT_MEM                0xFFFFFFFFU                     /* Used for Erase and Write protect CMDs */

#define UNDEF_ADDRESS                     0xFFFFFFFFU                     /* Undefined Address */

#define RDP_LEVEL_0                       0xEEEEEEEEU
#define RDP_LEVEL_1                       0xEEEEEEEEU

#define AREA_ERROR                        0x0U                            /* Error Address Area */
#define RAM_AREA                          0x2U                            /* RAM Address area */
#define OTP_AREA                          0x6U                            /* OTP Address area */

#define FLASH_MASS_ERASE                  0xFFFFU

#define INTERFACES_SUPPORTED              3U

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENBOOTLOADER_CONF_H */
