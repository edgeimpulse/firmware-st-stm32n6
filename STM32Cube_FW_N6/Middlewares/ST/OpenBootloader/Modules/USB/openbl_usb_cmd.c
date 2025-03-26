/**
  ******************************************************************************
  * @file    openbl_usb_cmd.c
  * @author  MCD Application Team
  * @brief   Contains USB protocol commands
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "openbl_usb_cmd.h"
#include "openbl_mem.h"
#include "openbootloader_conf.h"
#include "usb_interface.h"
#include "common_interface.h"
#include "openbl_util.h"
#include "otp_interface.h"

/* External variables --------------------------------------------------------*/
extern OPENBL_Flashlayout_TypeDef FlashlayoutStruct;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USB_RAM_BUFFER_SIZE             20U  /* Size of USB buffer used to store received data from the host */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t OtpIndex;
static uint8_t PhaseId       = PHASE_FLASHLAYOUT;
static uint8_t PartitionNum  = PHASE_FLASHLAYOUT;
static bool IsStartOperation = false;
static uint32_t DestinationAddress;
static Otp_Partition_t Otp;

/* Private function prototypes -----------------------------------------------*/
uint32_t OPENBL_USB_GetAddress(uint8_t Phase);
uint8_t OPENBL_USB_GetPhase(uint32_t Alt);

/* Exported functions---------------------------------------------------------*/
/**
  * @brief  Erase sector.
  * @param  Address Address of sector to be erased.
  * @retval 0 if operation is successful, MAL_FAIL else.
  */
uint16_t OPENBL_USB_EraseMemory(uint32_t Address)
{
  ErrorStatus error_value;
  uint8_t status;
  uint32_t numpage;
  uint32_t page;
  uint8_t usb_ram_buffer[USB_RAM_BUFFER_SIZE];
  uint8_t *ramaddress;

  ramaddress = (uint8_t *) usb_ram_buffer;
  numpage = 1;

  *ramaddress = (uint8_t)(numpage & 0x00FFU);
  ramaddress++;

  *ramaddress = (uint8_t)((numpage & 0xFF00U) >> 8);
  ramaddress++;

  page = OPENBL_USB_GetPage(Address);

  *ramaddress = (uint8_t)(page & 0x00FFU);
  ramaddress++;

  *ramaddress = (uint8_t)((page & 0xFF00U) >> 8);
  ramaddress++;

  error_value = OPENBL_MEM_Erase(OPENBL_DEFAULT_MEM, (uint8_t *) usb_ram_buffer, USB_RAM_BUFFER_SIZE);

  if (error_value != SUCCESS)
  {
    status = 1U;
  }
  else
  {
    status = 0U;
  }

  return status;
}

/**
  * @brief  Memory write routine.
  * @param  pSrc Pointer to the source buffer. Address to be written to.
  * @param  Alt USB Alternate.
  * @param  Length Number of data to be written (in bytes).
  * @retval None.
  */
void OPENBL_USB_Download(uint8_t *pSrc, uint32_t Alt, uint32_t Length, uint32_t BlockNumber)
{
  uint32_t i;

  switch (PhaseId)
  {
    case PHASE_OTP:
      /* Set OTP version */
      Otp.Version = (((uint32_t)pSrc[3] << 24U)
                     | ((uint32_t)pSrc[2] << 16U)
                     | ((uint32_t)pSrc[1] << 8U)
                     | (uint32_t)pSrc[0]);

      /* Set OTP global state */
      Otp.GlobalState = (((uint32_t)pSrc[7] << 24U)
                         | ((uint32_t)pSrc[6] << 16U)
                         | ((uint32_t)pSrc[5] << 8U)
                         | (uint32_t)pSrc[4]);

      if (BlockNumber == 0U)
      {
        /* Set OTP values and status */
        for (i = 8U, OtpIndex = 0U; (i < Length && (OtpIndex < OTP_PART_SIZE)); i += 4U, OtpIndex++)
        {
          Otp.OtpPart[OtpIndex] = (((uint32_t)pSrc[i + 3] << 24U)
                                   | ((uint32_t)pSrc[i + 2] << 16U)
                                   | ((uint32_t)pSrc[i + 1] << 8U)
                                   | (uint32_t)pSrc[i]);
        }
      }
      else
      {
        for (i = 0U, OtpIndex = (254U + ((BlockNumber - 1U) * 256U)); (i < Length && (OtpIndex < OTP_PART_SIZE)); i += 4U, OtpIndex++)
        {
          Otp.OtpPart[OtpIndex] = (((uint32_t)pSrc[i + 3] << 24U)
                                   | ((uint32_t)pSrc[i + 2] << 16U)
                                   | ((uint32_t)pSrc[i + 1] << 8U)
                                   | (uint32_t)pSrc[i]);
        }
      }

      /* Write OTP */
      if (OtpIndex == OTP_PART_SIZE)
      {
        OPENBL_OTP_Write(Otp);
      }

      break;

    case PHASE_0x3:
      OPENBL_MEM_Write(DestinationAddress, pSrc, Length);

      break;

    case PHASE_FLASHLAYOUT:
      /* First 256 is reserved for binary signature info */
      DestinationAddress = DestinationAddress + 256U;

      /* Parse the flash layout */
      if (OPENBL_FlashLayout_Parse_Layout((uint32_t)pSrc + 256U, (Length - 256U)) == PARSE_ERROR)
      {
        while (1) {};
      }

      break;

    default:
      break;
  }
}

/**
  * @brief  Memory read routine.
  * @param  USB Alternate
  * @param  pDest Pointer to the destination buffer.
  * @param  Length Number of data to be read (in bytes).
  * @retval Pointer to the physical address where data should be read.
  */
uint8_t *OPENBL_USB_ReadMemory(uint32_t Alt, uint8_t *pDest, uint32_t Length, uint32_t BlockNumber)
{
  uint32_t i;

  PhaseId = OPENBL_USB_GetPhase(Alt);

  switch (PhaseId)
  {
    case PHASE_CMD:
      /* Phase flash layout */
      if (PartitionNum == PHASE_FLASHLAYOUT)
      {
        PhaseId = PHASE_FLASHLAYOUT;
      }
      else if (PartitionNum < FlashlayoutStruct.partsize)
      {
        PhaseId = FlashlayoutStruct.id[PartitionNum];
      }
      else
      {
        PhaseId = PHASE_END;
      }

      /* Get the phase address */
      DestinationAddress = OPENBL_USB_GetAddress(PhaseId);

      /* Get phase command response */
      pDest[0] = PhaseId;
      pDest[1] = (uint8_t)(DestinationAddress >> 0U);
      pDest[2] = (uint8_t)(DestinationAddress >> 8U);
      pDest[3] = (uint8_t)(DestinationAddress >> 16U);
      pDest[4] = (uint8_t)(DestinationAddress >> 24U);
      pDest[5] = 0x00U;
      pDest[6] = 0x00U;
      pDest[7] = 0x00U;
      pDest[8] = 0x00U;

      /* Start operation call after each phase operation */
      /* If current operation is phase operation, next operation is start operation */
      if (IsStartOperation) /* Start operation */
      {
        /* Next operation is phase operation */
        IsStartOperation = false;

        /* Update current partition */
        PartitionNum++;
      }
      else /* Phase operation */
      {
        /* Next operation is start operation */
        IsStartOperation = true;
      }
      break;

    case PHASE_OTP:
      /* Read OTP */
      if (BlockNumber == 0)
      {
        Otp = OPENBL_OTP_Read();

        /* Get OTP version */
        pDest[0] = (uint8_t)Otp.Version;
        pDest[1] = (uint8_t)(Otp.Version >> 8U);
        pDest[2] = (uint8_t)(Otp.Version >> 16U);
        pDest[3] = (uint8_t)(Otp.Version >> 24U);

        /* Get OTP global state */
        pDest[4] = (uint8_t)Otp.GlobalState;
        pDest[5] = (uint8_t)(Otp.GlobalState >> 8U);
        pDest[6] = (uint8_t)(Otp.GlobalState >> 16U);
        pDest[7] = (uint8_t)(Otp.GlobalState >> 24U);

        /* Get OTP values and status */
        for (i = 8U, OtpIndex = 0U; (i < Length && (OtpIndex < OTP_PART_SIZE)); i += 4U, OtpIndex++)
        {
          /* 127 OTP sent for the first 1024 bytes block */
          /* BlockNumber == 0 : OtpIndex = 0..251 - 127 OTP x 8 bytes */
          pDest[i]     = (uint8_t)(Otp.OtpPart[OtpIndex]);
          pDest[i + 1] = (uint8_t)(Otp.OtpPart[OtpIndex] >> 8U);
          pDest[i + 2] = (uint8_t)(Otp.OtpPart[OtpIndex] >> 16U);
          pDest[i + 3] = (uint8_t)(Otp.OtpPart[OtpIndex] >> 24U);
        }

      }
      else
      {
        for (i = 0U, OtpIndex = (254U + ((BlockNumber - 1) * 256U)); (i < Length && (OtpIndex < OTP_PART_SIZE)); i += 4U, OtpIndex++)
        {
          /*
           * BlockNumber == 1 : OtpIndex = 254..505 - 128 OTP x 8 bytes = 1024 bytes - 8 bytes = 4 byes for OTP value & 4 bytes for status
           * BlockNumber == 2 : OtpIndex = 506..761 - 1024 bytes
           * BlockNumber == 3 : OtpIndex = 762..767 - 1024 bytes
           */

          pDest[i]     = (uint8_t)(Otp.OtpPart[OtpIndex]);
          pDest[i + 1] = (uint8_t)(Otp.OtpPart[OtpIndex] >> 8U);
          pDest[i + 2] = (uint8_t)(Otp.OtpPart[OtpIndex] >> 16U);
          pDest[i + 3] = (uint8_t)(Otp.OtpPart[OtpIndex] >> 24U);
        }
      }
      break;

    default:
      break;
  }

  /* Return a valid address to avoid HardFault */
  return pDest;
}

/**
  * @brief  This function is used to jump to the user application.
  * @param  Address The jump address.
  * @retval None.
  */
void OPENBL_USB_Jump(uint32_t Address)
{
  uint8_t status;

  /* Check if received address is valid or not */
  status = OPENBL_MEM_CheckJumpAddress(Address);

  if (status == 1U)
  {
    OPENBL_MEM_JumpToAddress(Address);
  }
}

/**
  * @brief  Write protect.
  * @param  pBuffer A buffer that contains the list of sectors or pages to be protected.
  * @param  Length Contains the length of the pBuffer.
  * @retval None.
  */
void OPENBL_USB_WriteProtect(uint8_t *pBuffer, uint32_t Length)
{
  ErrorStatus error_value;

  error_value = OPENBL_MEM_SetWriteProtection(ENABLE, OPENBL_DEFAULT_MEM, pBuffer, Length);

  if (error_value == SUCCESS)
  {
    /* Start post processing task if needed */
    Common_StartPostProcessing();
  }
}

/**
  * @brief  Write unprotect.
  * @retval None.
  */
void OPENBL_USB_WriteUnprotect(void)
{
  ErrorStatus error_value;

  error_value = OPENBL_MEM_SetWriteProtection(DISABLE, OPENBL_DEFAULT_MEM, NULL, 0U);

  if (error_value == SUCCESS)
  {
    /* Start post processing task if needed */
    Common_StartPostProcessing();
  }
}

/**
  * @brief  Read protect.
  * @retval None.
  */
void OPENBL_USB_ReadProtect(void)
{
  /* Enable the read protection */
  OPENBL_MEM_SetReadOutProtection(OPENBL_DEFAULT_MEM, ENABLE);

  /* Start post processing task if needed */
  Common_StartPostProcessing();
}

/**
  * @brief  Read unprotect.
  * @retval None.
  */
void OPENBL_USB_ReadUnprotect(void)
{
  /* Disable the read protection */
  OPENBL_MEM_SetReadOutProtection(OPENBL_DEFAULT_MEM, DISABLE);

  /* Start post processing task if needed */
  Common_StartPostProcessing();
}

/**
  * @brief  Link between USB Alternate and STM32CubeProgrammer phase
  * @param  Alt USB Alternate.
  * @retval STM32CubeProgramer Phase.
  */
uint8_t OPENBL_USB_GetPhase(uint32_t Alt)
{
  uint8_t ret;

  switch (Alt)
  {
    case 0U:
      ret = PHASE_FLASHLAYOUT;
      break;

    case 1U:
      ret = PHASE_0x3;
      break;

    case 2U:
      ret = PHASE_0x4;
      break;

    case 3U:
      ret = PHASE_CMD;
      break;

    case 4U:
      ret = PHASE_OTP;
      break;

    default:
      ret = PHASE_END;
      break;
  }

  return ret;
}

/**
  * @brief  Get Address of STM32CubeProgrammer phase.
  * @param  Phase USB Alternate.
  * @retval STM32CubeProgrammer Phase.
  */
uint32_t OPENBL_USB_GetAddress(uint8_t Phase)
{
  uint32_t ret;

  switch (Phase)
  {
    case PHASE_FLASHLAYOUT:
      ret = FLASHLAYOUT_ADDRESS;
      break;

    case PHASE_0x3:
      ret = RAM_WRITE_ADDRESS;
      break;

    default:
      ret = UNDEF_ADDRESS;
      break;
  }

  return ret;
}
