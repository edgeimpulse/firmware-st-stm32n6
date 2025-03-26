/**
  ******************************************************************************
  * @file    openbl_util.h
  * @author  MCD Application Team
  * @brief   Header for openbl_util.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef OPENBL_UTIL_H
#define OPENBL_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "string.h"
#include "stdlib.h"
#include <stdbool.h>
#include <limits.h>
#include <errno.h>
/* Exported types ------------------------------------------------------------*/
typedef struct
{
  char     *opt[0xF];
  uint32_t  id[0xF];
  char     *name[0xF];
  char     *type[0xF];
  char     *ip[0xF];
  uint32_t  offset[0xF];
  uint32_t  partsize;
} OPENBL_Flashlayout_TypeDef;


/* Exported constants --------------------------------------------------------*/
#define PHASE_FLASHLAYOUT                    0x00                /* Flashlayout phase */
#define PHASE_OTP                            0xF2                /* OTP partion phase */
#define PHASE_0x1                            0x01                /* Phase ID 0x1  */
#define PHASE_0x3                            0x03                /* Phase ID 0x3 */
#define PHASE_0x4                            0x04                /* Phase ID 0x4 */
#define PHASE_END                            0xFE                /* End phasse */
#define PHASE_FIRST_USER                     0x01                /* Phase first user */
#define PHASE_LAST_USER                      0x0F                /* Phase last user */
#define PARSE_ERROR                          -1
#define PARSE_OK                             0
#define GETPHASE_SIZE                        9
#define PHASE_CMD                            0xF1

#define BOOT_INTERFACE_SEL_SERIAL_UART       0x5U                /* Boot occurred on UART */
#define BOOT_INTERFACE_SEL_SERIAL_USB        0x6U                /* Boot occurred on USB */
#define UART_ID                              0                   /* USB registered as interface ID 0 */
#define USB_ID                               1                   /* USB registered as interface ID 1 */
#define UNDEF_ID                             -2

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
int OPENBL_FlashLayout_Parse_Layout(uint32_t address, uint32_t size);
int OPENBL_Flashlayout_Parse_ID(char *p_string_id, uint32_t idx);
int OPENBL_Flashlayout_Parse_Offset(char *p_string_offset, uint32_t idx);
int OPENBL_Flashlayout_Parse_Name(char *p_string_name, uint32_t idx);
int OPENBL_Flashlayout_Parse_Type(char *p_string_type, uint32_t idx);
int OPENBL_Flashlayout_Parse_IP(char *p_string_ip, uint32_t idx);
int OPENBL_Flashlayout_Parse_Option(char *p_string_option, uint32_t idx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENBL_UTIL_H */
