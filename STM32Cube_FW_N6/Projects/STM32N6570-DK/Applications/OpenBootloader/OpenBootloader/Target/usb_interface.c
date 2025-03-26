/**
  ******************************************************************************
  * @file    usb_interface.c
  * @author  MCD Application Team
  * @brief   Contains USB protocol commands
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
#include "openbl_core.h"
#include "usb_interface.h"
#include "usbd_core.h"
#include "platform.h"
#include "usbd_def.h"
#include "usbd_dfu.h"
#include "usbd_ioreq.h"
#include "usbd_dfu_if.h"
#include "usbd_desc.h"
#include "usb_device.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t UsbDetected = 0U;

/* Exported variables --------------------------------------------------------*/
uint8_t UsbSofDetected = 0U;

/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief  This function is used to configure USB pins and then initialize the used USB instance.
  * @retval None.
  */
void OPENBL_USB_Configuration(void)
{
  /* Initialization USB device Library, add supported class and start the library */
  MX_USB_Device_Init();
}

/**
  * @brief  This function is used to De-initialize the USB pins and instance.
  * @retval None.
  */
void OPENBL_USB_DeInit(void)
{
  /* Only de-initialize the USB if it is not the current detected interface */
  if (UsbDetected == 0U)
  {
    __HAL_RCC_USB1_OTG_HS_CLK_ENABLE();

    HAL_NVIC_DisableIRQ(USB1_OTG_HS_IRQn);

    __HAL_RCC_USB1_OTG_HS_FORCE_RESET();
    __HAL_RCC_USB1_OTG_HS_RELEASE_RESET();
  }
}

/**
  * @brief  This function is used to detect if there is any activity on USB protocol.
  * @retval None.
  */
uint8_t OPENBL_USB_ProtocolDetection(void)
{
  if (UsbSofDetected == 1U)
  {
    UsbDetected = 0U;

    /* Disable the other interfaces */
    OPENBL_InterfacesDeInit();
  }
  else
  {
    UsbDetected = 0U;
  }

  return UsbDetected;
}

/**
  * @brief  This function is used to send a NACK when the address is not valid by changing
  * the state of the USB to dfu_error.
  * @param  pDev Pointer to the USBD_HandleTypeDef structure.
  * @retval Returns USBD_FAIL.
  */
uint16_t OPENBL_USB_SendAddressNack(USBD_HandleTypeDef *pDev)
{
  USBD_DFU_HandleTypeDef *hdfu;

  hdfu = (USBD_DFU_HandleTypeDef *)pDev->pClassData;

  if (hdfu->dev_state == DFU_STATE_DNLOAD_BUSY)
  {
    hdfu->dev_state     = DFU_ERROR_TARGET;
    hdfu->dev_status[0] = DFU_ERROR_TARGET;
    hdfu->dev_status[1] = 0U;
    hdfu->dev_status[2] = 0U;
    hdfu->dev_status[3] = 0U;
    hdfu->dev_status[4] = DFU_STATE_ERROR;
    hdfu->dev_status[5] = 0U;
  }

  return (uint16_t)USBD_FAIL;
}

/**
  * @brief  Gets the sector of a given address
  * @param  Address Address of the FLASH Memory
  * @retval The sector of a given address
  */
uint32_t OPENBL_USB_GetPage(uint32_t Address)
{
  UNUSED(Address);

  return 0;
}
