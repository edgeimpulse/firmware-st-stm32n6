/**
  ******************************************************************************
  * @file           usbd_desc.c
  * @author         MCD Application Team
  * @brief          This file implements the USB device descriptors.
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
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define USBD_VID                      0x483
#define USBD_LANGID_STRING            1033
#define USBD_MANUFACTURER_STRING      "STMicroelectronics"
#define USBD_PID                      0xdf11
#define USBD_PRODUCT_STRING           "STM32OpenBootloader DFU@Device ID /0x486, @Revision ID /0x1000"
#define USBD_CONFIGURATION_STRING     "DFU Config"
#define USBD_INTERFACE_STRING         "DFU Interface"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static void Get_SerialNum(void);
uint8_t USBD_StrDesc[USBD_MAX_STR_DESC_SIZ];
static void Get_SerialNum(void);

uint8_t *USBD_DFU_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_DFU_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_DFU_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_DFU_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_DFU_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_DFU_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_DFU_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);

USBD_DescriptorsTypeDef DFU_Desc =
{
  USBD_DFU_DeviceDescriptor,
  USBD_DFU_LangIDStrDescriptor,
  USBD_DFU_ManufacturerStrDescriptor,
  USBD_DFU_ProductStrDescriptor,
  USBD_DFU_SerialStrDescriptor,
  USBD_DFU_ConfigStrDescriptor,
  USBD_DFU_InterfaceStrDescriptor
};

#if defined (__ICCARM__) /* IAR Compiler */
#pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */

/** USB standard device descriptor. */
__ALIGN_BEGIN uint8_t USBD_DFU_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END =
{
  0x12,                       /*bLength */
  USB_DESC_TYPE_DEVICE,       /*bDescriptorType*/
  0x10,                       /*bcdUSB */
  0x01,
  0x00,                       /*bDeviceClass*/
  0x00,                       /*bDeviceSubClass*/
  0x00,                       /*bDeviceProtocol*/
  USB_MAX_EP0_SIZE,           /*bMaxPacketSize*/
  LOBYTE(USBD_VID),           /*idVendor*/
  HIBYTE(USBD_VID),           /*idVendor*/
  LOBYTE(USBD_PID),           /*idProduct*/
  HIBYTE(USBD_PID),           /*idProduct*/
  0x00,                       /*bcdDevice rel. 2.00*/
  0x02,
  USBD_IDX_MFC_STR,           /*Index of manufacturer  string*/
  USBD_IDX_PRODUCT_STR,       /*Index of product string*/
  USBD_IDX_SERIAL_STR,        /*Index of serial number string*/
  USBD_MAX_NUM_CONFIGURATION  /*bNumConfigurations*/
};

#if defined ( __ICCARM__ ) /* IAR Compiler */
#pragma data_alignment=4U
#endif /* defined ( __ICCARM__ ) */

/** USB lang identifier descriptor. */
__ALIGN_BEGIN uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END =
{
  USB_LEN_LANGID_STR_DESC,
  USB_DESC_TYPE_STRING,
  LOBYTE(USBD_LANGID_STRING),
  HIBYTE(USBD_LANGID_STRING)
};

#if defined ( __ICCARM__ ) /* IAR Compiler */
#pragma data_alignment=4U
#endif /* defined ( __ICCARM__ ) */
/* Internal string descriptor. */
__ALIGN_BEGIN uint8_t USBD_StrDesc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4U
#endif
__ALIGN_BEGIN uint8_t USBD_StringSerial[USB_SIZ_STRING_SERIAL] __ALIGN_END =
{
  USB_SIZ_STRING_SERIAL,
  USB_DESC_TYPE_STRING,
};

/**
  * @brief  Return the device descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_DFU_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  UNUSED(speed);
  *length = sizeof(USBD_DFU_DeviceDesc);

  return USBD_DFU_DeviceDesc;
}

/**
  * @brief  Return the LangID string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_DFU_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  *length = sizeof(USBD_LangIDDesc);

  return USBD_LangIDDesc;
}

/**
  * @brief  Return the product string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_DFU_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  if (speed == 0U)
  {
    USBD_GetString((uint8_t *)USBD_PRODUCT_STRING, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString((uint8_t *)USBD_PRODUCT_STRING, USBD_StrDesc, length);
  }

  return USBD_StrDesc;
}

/**
  * @brief  Return the manufacturer string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_DFU_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  USBD_GetString((uint8_t *)USBD_MANUFACTURER_STRING, USBD_StrDesc, length);

  return USBD_StrDesc;
}

/**
  * @brief  Return the serial number string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_DFU_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  UNUSED(speed);
  *length = USB_SIZ_STRING_SERIAL;

  /* Update the serial number string descriptor with the data from the unique
   * ID */
  Get_SerialNum();

  return (uint8_t *) USBD_StringSerial;
}

/**
  * @brief  Return the configuration string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_DFU_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  if (speed == USBD_SPEED_HIGH)
  {
    USBD_GetString((uint8_t *)USBD_CONFIGURATION_STRING, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString((uint8_t *)USBD_CONFIGURATION_STRING, USBD_StrDesc, length);
  }

  return USBD_StrDesc;
}

/**
  * @brief  Return the interface string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_DFU_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  if (speed == 0U)
  {
    USBD_GetString((uint8_t *)USBD_INTERFACE_STRING, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString((uint8_t *)USBD_INTERFACE_STRING, USBD_StrDesc, length);
  }

  return USBD_StrDesc;
}

/**
  * @brief  Create the serial number string descriptor
  * @param  None
  * @retval None
  */
static void Get_SerialNum(void)
{
  uint32_t deviceserial0, deviceserial1, deviceserial2;
  char serial_string[SIZ_STRING_SERIAL + 2U];
  uint8_t idx = 2U;

  deviceserial0 = *(uint32_t *) DEVICE_ID1;
  deviceserial1 = *(uint32_t *) DEVICE_ID2;
  deviceserial2 = *(uint32_t *) DEVICE_ID3;

  snprintf(serial_string, sizeof(serial_string), "%08lX%08lX%08lX",
           deviceserial0, deviceserial1, deviceserial2);

  const char *desc = serial_string;

  while (*desc != '\0')
  {
    USBD_StringSerial[idx++] = *desc++;
    USBD_StringSerial[idx++] =  0x00U;
  }
}
