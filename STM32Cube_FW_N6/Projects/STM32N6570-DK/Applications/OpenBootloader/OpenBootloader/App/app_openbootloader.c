/**
  ******************************************************************************
  * @file    app_openbootloader.c
  * @author  MCD Application Team
  * @brief   OpenBootloader application entry point
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "openbl_core.h"

#include "openbl_usart_cmd.h"
#include "openbl_usb_cmd.h"

#include "app_openbootloader.h"
#include "usart_interface.h"
#include "usb_interface.h"
#include "iwdg_interface.h"
#include "otp_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static OPENBL_HandleTypeDef USART_Handle;
static OPENBL_HandleTypeDef USB_Handle;
static OPENBL_HandleTypeDef IWDG_Handle;

static OPENBL_OpsTypeDef USART_Ops =
{
  OPENBL_USART_Configuration,
  OPENBL_USART_DeInit,
  OPENBL_USART_ProtocolDetection,
  OPENBL_USART_GetCommandOpcode,
  OPENBL_USART_SendByte
};

static OPENBL_OpsTypeDef USB_Ops =
{
  OPENBL_USB_Configuration,
  OPENBL_USB_DeInit,
  OPENBL_USB_ProtocolDetection,
  NULL,
  NULL
};

static OPENBL_OpsTypeDef IWDG_Ops =
{
  OPENBL_IWDG_Configuration,
  NULL,
  NULL,
  NULL,
  NULL
};

/* Exported variables --------------------------------------------------------*/
uint16_t SpecialCmdList[SPECIAL_CMD_MAX_NUMBER] =
{
  SPECIAL_CMD_DEFAULT
};

uint16_t ExtendedSpecialCmdList[EXTENDED_SPECIAL_CMD_MAX_NUMBER] =
{
  SPECIAL_CMD_DEFAULT
};

static OPENBL_CommandsTypeDef USART_Cmd =
{
  OPENBL_USART_GetCommand,
  OPENBL_USART_GetVersion,
  OPENBL_USART_GetID,
  OPENBL_USART_GetPhase,
  OPENBL_USART_ReadMemory,
  OPENBL_USART_ReadPartition,
  OPENBL_USART_Download,
  OPENBL_USART_Start,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize open Bootloader.
  * @retval None.
  */
void OpenBootloader_Init(void)
{
  /* Register USART interfaces */
  USART_Handle.p_Ops = &USART_Ops;
  USART_Handle.p_Cmd = &USART_Cmd;

  OPENBL_USART_SetCommandsList(&USART_Cmd);

  if (OPENBL_RegisterInterface(&USART_Handle) != SUCCESS)
  {
    Error_Handler();
  }

  /* Register USB interfaces */
  USB_Handle.p_Ops = &USB_Ops;
  USB_Handle.p_Cmd = NULL;

  if (OPENBL_RegisterInterface(&USB_Handle) != SUCCESS)
  {
    Error_Handler();
  }

  /* Register IWDG interfaces */
  IWDG_Handle.p_Ops = &IWDG_Ops;
  IWDG_Handle.p_Cmd = NULL;

  if (OPENBL_RegisterInterface(&IWDG_Handle) != SUCCESS)
  {
    Error_Handler();
  }

  /* Initialize interfaces */
  OPENBL_Init();

  /* Initialize memories */
  if (OPENBL_MEM_RegisterMemory(&RAM_Descriptor) != SUCCESS)
  {
    Error_Handler();
  }

  /* Initialize otp */
  OPENBL_OTP_Init();
}

/**
  * @brief  DeInitialize open Bootloader.
  * @retval None.
  */
void OpenBootloader_DeInit(void)
{
  System_DeInit();
}

/**
  * @brief  This function is used to select which protocol will be used when communicating with the host.
  * @retval None.
  */
void OpenBootloader_ProtocolDetection(void)
{
  static uint32_t interface_detected = 0U;

  if (interface_detected == 0U)
  {
    interface_detected = OPENBL_InterfaceDetection();

    /* De-initialize the interfaces that are not detected */
    if (interface_detected == 1U)
    {
      OPENBL_InterfacesDeInit();
    }
  }

  if (interface_detected == 1U)
  {
    OPENBL_CommandProcess();
  }
}
