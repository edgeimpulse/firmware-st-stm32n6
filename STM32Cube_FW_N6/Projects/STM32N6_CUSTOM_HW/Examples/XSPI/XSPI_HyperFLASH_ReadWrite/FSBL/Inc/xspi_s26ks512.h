/**
  ******************************************************************************
  * @file    xspi_s26ks512.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the xspi_s26ks512.c driver.
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
#ifndef XSPI_S26KS512_H
#define XSPI_S26KS512_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"


/** @addtogroup STM32N6570_EV
  * @{
  */

/** @addtogroup XSPI_S26KS512
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define STATUS_REG_READ_CYCLES_NB              1
#define WRITE_TO_BUFFER_CYCLES_NB              4
#define SECTOR_ERASE_CYCLES_NB                 6
#define RESET_CYCLES_NB                        1

#define ADDR_PATTERN_1                         (0x555 << 1) /* Address reference 16-bit words */
#define ADDR_PATTERN_2                         (0x2AA << 1) /* Address reference 16-bit words */
#define DATA_PATTERN_1                         0xAA
#define DATA_PATTERN_2                         0x55

/* Register operations */
#define STATUS_REG_READ_CMD                    0x70

/* Program Operations */
#define WRITE_TO_BUFFER_CMD                    0x25
#define PROG_BUFFER_TO_FLASH_CMD               0x29

/* Erase Operations */
#define ERASE_CMD                              0x80
#define CHIP_ERASE_CMD                         0x10
#define SECTOR_ERASE_CMD                       0x30

/* Reset operations */
#define RESET_CMD                              0xF0


/* Max programming buffer size */
#define MAX_DATA_WRITE_BYTE                    512 /* Maw Write Buffer: 256 words / 512 Bytes */
#define MAX_DATA_WRITE_WORD                    256 /* Maw Write Buffer: 256 words / 512 Bytes */

/* Sector Address example */
#define ADDRESS_SECTOR_0                       0x00000000
#define ADDRESS_SECTOR_1                       0x00020000
#define ADDRESS_SECTOR_2                       0x00040000
#define ADDRESS_SECTOR_63                      0x007E0000
#define ADDRESS_SECTOR_127                     0x00FE0000
#define ADDRESS_SECTOR_255                     0x01FE0000
#define ADDRESS_MEMORY_MAX                     0x01FFFFFF
#define SECTOR_SIZE                            0x20000

/** @defgroup XSPI_S26KS512_Exported_Constants XSPI Exported Constants
  * @{
  */

#define XSPI_OK                                ((uint8_t)0x00)
#define XSPI_ERROR                             ((uint8_t)0x01)

/**
  * @}
  */

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup XSPI_S26KS512_Exported_Functions XSPI_HYPERFLASH Exported Functions
  * @{
  */
uint32_t XSPI_S26KS512_Init(XSPI_HandleTypeDef *hxspi);
uint32_t XSPI_S26KS512_Erase_Sector(XSPI_HandleTypeDef *hxspi, uint32_t SecAdd);
uint32_t XSPI_S26KS512_Erase_Chip(XSPI_HandleTypeDef *hxspi);
uint32_t XSPI_S26KS512_Read(XSPI_HandleTypeDef *hxspi, uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
uint32_t XSPI_S26KS512_Write(XSPI_HandleTypeDef *hxspi, uint8_t *pData, uint32_t WriteAddr, uint32_t Size);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* XSPI_S26KS512_H */
