/**
******************************************************************************
* @file    xspi_s26ks512.c
* @author  MCD Application Team
* @brief   This file includes a standard driver for the S26KS512
*          XSPI memory.
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
@verbatim
==============================================================================
##### How to use this driver #####
==============================================================================
[..]
(#) This driver is used to drive the S26KS512DPBHI02 Octal HYPERFLASH.

(#) S26KS512DPBHI02 Initialization steps:
(++) Initialize the XSPI external memory using the XSPI_S26KS512_Init() function.
This function includes the MSP layer hardware resources initialization and the
XSPI interface with the external memory.

(#) S26KS512DPBHI02 Octal HYPERFLASH memory operations
(++) XSPI memory can be accessed with read/write operations once it is
initialized.
Read/write operation can be performed with AXI access using the functions
XSPI_S26KS512_Read()/XSPI_S26KS512_Write().

@endverbatim
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "xspi_s26ks512.h"

/** @addtogroup STM32N6570_EV
* @{
*/

/** @defgroup XSPI_S26KS512 XSPI
* @{
*/

/* Exported variables --------------------------------------------------------*/
/* Private constants --------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup XSPI_S26KS512_Exported_Variables XSPI_HYPERFLASH Exported Variables
* @{
*/
uint16_t Status_Reg_Read_Cycles_Addr[STATUS_REG_READ_CYCLES_NB] = {ADDR_PATTERN_1};
uint8_t  Status_Reg_Read_Cycles_Data[(2*STATUS_REG_READ_CYCLES_NB)] = {STATUS_REG_READ_CMD, 0x00};

uint32_t Write_To_Buffer_Cycles_Addr[WRITE_TO_BUFFER_CYCLES_NB] = {ADDR_PATTERN_1, ADDR_PATTERN_2, 0x00, 0x00};
uint8_t  Write_To_Buffer_Cycles_Data[(2*WRITE_TO_BUFFER_CYCLES_NB)] = {DATA_PATTERN_1, 0x00, DATA_PATTERN_2, 0x00, WRITE_TO_BUFFER_CMD, 0x00, 0x00, 0x00};

uint16_t Chip_Erase_Cycles_Addr[SECTOR_ERASE_CYCLES_NB] = {ADDR_PATTERN_1, ADDR_PATTERN_2, ADDR_PATTERN_1, ADDR_PATTERN_1, ADDR_PATTERN_2, ADDR_PATTERN_1};
uint8_t  Chip_Erase_Cycles_Data[(2*SECTOR_ERASE_CYCLES_NB)] = {DATA_PATTERN_1, 0x00, DATA_PATTERN_2, 0x00, ERASE_CMD, 0x00, DATA_PATTERN_1, 0x00, DATA_PATTERN_2, 0x00, CHIP_ERASE_CMD, 0x00};

uint32_t Sector_Erase_Cycles_Addr[SECTOR_ERASE_CYCLES_NB] = {ADDR_PATTERN_1, ADDR_PATTERN_2, ADDR_PATTERN_1, ADDR_PATTERN_1, ADDR_PATTERN_2, 0x00};
uint8_t  Sector_Erase_Cycles_Data[(2*SECTOR_ERASE_CYCLES_NB)] = {DATA_PATTERN_1, 0x00, DATA_PATTERN_2, 0x00, ERASE_CMD, 0x00, DATA_PATTERN_1, 0x00, DATA_PATTERN_2, 0x00, SECTOR_ERASE_CMD, 0x00};

uint16_t Reset_Cycles_Addr[RESET_CYCLES_NB] = {0x00};
uint8_t  Reset_Cycles_Data[(2*RESET_CYCLES_NB)] = {RESET_CMD, 0x00};
/**
* @}
*/
/* Private functions ---------------------------------------------------------*/

/* Exported functions ---------------------------------------------------------*/
/** @addtogroup XSPI_S26KS512_Exported_Functions
* @{
*/

/**
* @brief  Initializes the XSPI interface.
* @param  None
* @retval status
*/
uint32_t XSPI_S26KS512_Init(XSPI_HandleTypeDef *hxspi)
{
  uint32_t XSPI_state = XSPI_OK;
  uint32_t index;
//  XSPI_HyperbusCfgTypeDef sHyperbusCfg = {0};
  XSPI_HyperbusCmdTypeDef sCommand = {0};

#if 0
  XSPIHandle.Instance = XSPI2;

  if (HAL_XSPI_DeInit(hxspi) != HAL_OK)
  {
    XSPI_state =  XSPI_ERROR;
  }

  XSPIHandle.Init.ChipSelectBoundary      = HAL_XSPI_BONDARYOF_NONE;
  XSPIHandle.Init.ChipSelectHighTimeCycle = 1;
  XSPIHandle.Init.ClockMode               = HAL_XSPI_CLOCK_MODE_0;
  XSPIHandle.Init.ClockPrescaler          = 0x03;
  XSPIHandle.Init.DelayHoldQuarterCycle   = HAL_XSPI_DHQC_ENABLE;
  XSPIHandle.Init.FifoThresholdByte       = 2;
  XSPIHandle.Init.FreeRunningClock        = HAL_XSPI_FREERUNCLK_DISABLE;
  XSPIHandle.Init.MemoryMode              = HAL_XSPI_SINGLE_MEM;
  XSPIHandle.Init.MemorySize              = HAL_XSPI_SIZE_512MB;
  XSPIHandle.Init.MemoryType              = HAL_XSPI_MEMTYPE_HYPERBUS;
  XSPIHandle.Init.SampleShifting          = HAL_XSPI_SAMPLE_SHIFT_NONE;
  XSPIHandle.Init.WrapSize                = HAL_XSPI_WRAP_NOT_SUPPORTED;

  if (HAL_XSPI_Init(hxspi) != HAL_OK)
  {
    XSPI_state =  XSPI_ERROR;
  }

  sHyperbusCfg.RWRecoveryTimeCycle   = 0;
  sHyperbusCfg.AccessTimeCycle       = 16;
  sHyperbusCfg.WriteZeroLatency      = HAL_XSPI_NO_LATENCY_ON_WRITE;
  sHyperbusCfg.LatencyMode           = HAL_XSPI_VARIABLE_LATENCY;

  if (HAL_XSPI_HyperbusCfg(hxspi, &sHyperbusCfg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    XSPI_state =  XSPI_ERROR;
  }
#endif

  sCommand.AddressSpace = HAL_XSPI_MEMORY_ADDRESS_SPACE;
  sCommand.AddressWidth = HAL_XSPI_ADDRESS_16_BITS;
  sCommand.DQSMode      = HAL_XSPI_DQS_ENABLE;
  sCommand.DataLength   = 2;

  for (index = 0; index < RESET_CYCLES_NB; index++)
  {
    sCommand.Address = Reset_Cycles_Addr[index];

    if (HAL_XSPI_HyperbusCmd(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      XSPI_state =  XSPI_ERROR;
    }

    if (HAL_XSPI_Transmit(hxspi, &(Reset_Cycles_Data[(2*index)]), HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      XSPI_state =  XSPI_ERROR;
    }
  }
  return  XSPI_state;

}

/**
* @brief  Erase a sector of XSPI memory.
* @param  SecAdd     Address of Sector to erase
* @retval status
*/
uint32_t XSPI_S26KS512_Erase_Sector(XSPI_HandleTypeDef *hxspi, uint32_t SecAdd)
{
  uint32_t XSPI_state = XSPI_OK;
  uint8_t data[2];
  uint32_t index =0;
  XSPI_HyperbusCmdTypeDef sCommand = {0};

  sCommand.AddressSpace = HAL_XSPI_MEMORY_ADDRESS_SPACE;
  sCommand.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
  sCommand.DQSMode      = HAL_XSPI_DQS_ENABLE;
  sCommand.DataLength   = 2;
  sCommand.DataMode     = HAL_XSPI_DATA_8_LINES;

  /* 1- Erasing Sequence ------------------------------------------------ */
  Sector_Erase_Cycles_Addr[SECTOR_ERASE_CYCLES_NB-1] = SecAdd;

  for (index = 0; index < SECTOR_ERASE_CYCLES_NB; index++)
  {
    sCommand.Address = Sector_Erase_Cycles_Addr[index];

    if (HAL_XSPI_HyperbusCmd(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      XSPI_state =  XSPI_ERROR;
    }

    if (HAL_XSPI_Transmit(hxspi, &(Sector_Erase_Cycles_Data[(2*index)]), HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      XSPI_state =  XSPI_ERROR;
    }
  }

  /* Reconfigure XSPI to automatic polling mode to wait for end of erase */
  do
  {
    for (index = 0; index < STATUS_REG_READ_CYCLES_NB; index++)
    {
      sCommand.Address = Status_Reg_Read_Cycles_Addr[index];

      if (HAL_XSPI_HyperbusCmd(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        XSPI_state =  XSPI_ERROR;
      }

      if (HAL_XSPI_Transmit(hxspi, &(Status_Reg_Read_Cycles_Data[(2*index)]), HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        XSPI_state =  XSPI_ERROR;
      }
    }

    if (HAL_XSPI_HyperbusCmd(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      XSPI_state =  XSPI_ERROR;
    }

    if (HAL_XSPI_Receive(hxspi, data, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      XSPI_state =  XSPI_ERROR;
    }
  } while ((data[0] & 0x80) == 0);

  if ((data[0] & 0x11) != 0 )
  {
    return XSPI_ERROR;
  }

  return  XSPI_state;

}

/**
* @brief  Erase full XSPI memory.
* @param  None
* @retval status
*/
uint32_t XSPI_S26KS512_Erase_Chip(XSPI_HandleTypeDef *hxspi)
{
  uint32_t XSPI_state = XSPI_OK;
  uint8_t data[2];
  uint32_t index =0;
  XSPI_HyperbusCmdTypeDef sCommand = {0};

  sCommand.AddressSpace = HAL_XSPI_MEMORY_ADDRESS_SPACE;
  sCommand.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
  sCommand.DQSMode      = HAL_XSPI_DQS_ENABLE;
  sCommand.DataLength   = 2;
  sCommand.DataMode     = HAL_XSPI_DATA_8_LINES;

  /* 1- Erasing Sequence ------------------------------------------------ */
  for (index = 0; index < SECTOR_ERASE_CYCLES_NB; index++)
  {
    sCommand.Address = Chip_Erase_Cycles_Addr[index];

    if (HAL_XSPI_HyperbusCmd(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      XSPI_state =  XSPI_ERROR;
    }

    if (HAL_XSPI_Transmit(hxspi, &(Chip_Erase_Cycles_Data[(2*index)]), HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      XSPI_state =  XSPI_ERROR;
    }
  }

  /* Reconfigure XSPI to automatic polling mode to wait for end of erase */
  do
  {
    for (index = 0; index < STATUS_REG_READ_CYCLES_NB; index++)
    {
      sCommand.Address = Status_Reg_Read_Cycles_Addr[index];

      if (HAL_XSPI_HyperbusCmd(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        XSPI_state =  XSPI_ERROR;
      }

      if (HAL_XSPI_Transmit(hxspi, &(Status_Reg_Read_Cycles_Data[(2*index)]), HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        XSPI_state =  XSPI_ERROR;
      }
    }

    if (HAL_XSPI_HyperbusCmd(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      XSPI_state =  XSPI_ERROR;
    }

    if (HAL_XSPI_Receive(hxspi, data, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      XSPI_state =  XSPI_ERROR;
    }
  } while ((data[0] & 0x80) == 0);

  if ((data[0] & 0x10) != 0 )
  {
    return XSPI_ERROR;
  }

  return  XSPI_state;

}

/**
* @brief  Reads an amount of data from the XSPI memory.
* @param  pData     Pointer to data to be read
* @param  ReadAddr  Read start address
* @param  Size      Size of data to read
* @retval status
*/
uint32_t XSPI_S26KS512_Read(XSPI_HandleTypeDef *hxspi, uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
  uint32_t ret = XSPI_OK;
  XSPI_HyperbusCmdTypeDef sCommand = {0};

  /* Reading Sequence ------------------------------------------------ */
  sCommand.AddressSpace = HAL_XSPI_MEMORY_ADDRESS_SPACE;
  sCommand.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
  sCommand.DQSMode      = HAL_XSPI_DQS_ENABLE;
  sCommand.DataMode     = HAL_XSPI_DATA_8_LINES;
  sCommand.Address     = ReadAddr;
  sCommand.DataLength  = Size;

  if (HAL_XSPI_HyperbusCmd(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    ret =  XSPI_ERROR;
  }

  if (HAL_XSPI_Receive(hxspi, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    ret =  XSPI_ERROR;
  }

  /* Return status */
  return ret;
}

/**
* @brief  Writes an amount of data to the XSPI memory.
* @param  pData     Pointer to data to be written
* @param  WriteAddr Write start address
* @param  Size      Size of data to write
* @retval status
*/
uint32_t XSPI_S26KS512_Write(XSPI_HandleTypeDef *hxspi, uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
  uint32_t ret = XSPI_OK;
  uint8_t data[2];
  uint32_t index;
  XSPI_HyperbusCmdTypeDef sCommand = {0};

  /* Enable write operations ----------------------------------------- */
  sCommand.AddressSpace = HAL_XSPI_MEMORY_ADDRESS_SPACE;
  sCommand.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
  sCommand.DQSMode      = HAL_XSPI_DQS_ENABLE;
  sCommand.DataLength   = 2;

  Write_To_Buffer_Cycles_Addr[WRITE_TO_BUFFER_CYCLES_NB - 2] = WriteAddr;
  Write_To_Buffer_Cycles_Addr[WRITE_TO_BUFFER_CYCLES_NB - 1] = WriteAddr+2;

  /* Convert byte size to word size */
  Write_To_Buffer_Cycles_Data[2*(WRITE_TO_BUFFER_CYCLES_NB - 1)] = ((Size >> 1) - 1);

  for (index = 0; index < WRITE_TO_BUFFER_CYCLES_NB; index++)
  {
    sCommand.Address = Write_To_Buffer_Cycles_Addr[index];

    if (HAL_XSPI_HyperbusCmd(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      ret =  XSPI_ERROR;
    }

    if (HAL_XSPI_Transmit(hxspi, &(Write_To_Buffer_Cycles_Data[(2*index)]), HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      ret =  XSPI_ERROR;
    }
  }

  for (index = 0; index < Size; index = index + 2, WriteAddr = WriteAddr+2)
  {
    sCommand.Address = WriteAddr;

    if (HAL_XSPI_HyperbusCmd(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      ret =  XSPI_ERROR;
    }

    if (HAL_XSPI_Transmit(hxspi, &(pData[index]), HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      ret =  XSPI_ERROR;
    }
  }

  sCommand.DataLength  = 2;
  data[0] = PROG_BUFFER_TO_FLASH_CMD;
  data[1] = 0x00;

  if (HAL_XSPI_HyperbusCmd(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    ret =  XSPI_ERROR;
  }

  if (HAL_XSPI_Transmit(hxspi, data, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    ret =  XSPI_ERROR;
  }

  /* Reconfigure XSPI to automatic polling mode to wait for end of program */
  do
  {
    for (index = 0; index < STATUS_REG_READ_CYCLES_NB; index++)
    {
      sCommand.Address = Status_Reg_Read_Cycles_Addr[index];

      if (HAL_XSPI_HyperbusCmd(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        ret =  XSPI_ERROR;
      }

      if (HAL_XSPI_Transmit(hxspi, &(Status_Reg_Read_Cycles_Data[(2*index)]), HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        ret =  XSPI_ERROR;
      }
    }

    /* Reset Address to retrieve Status register */
    sCommand.Address = 0;

    if (HAL_XSPI_HyperbusCmd(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      ret =  XSPI_ERROR;
    }

    if (HAL_XSPI_Receive(hxspi, data, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      ret =  XSPI_ERROR;
    }
  } while ((data[0] & 0x80) == 0);

  if ((data[0] & 0x10) != 0 )
  {
    return XSPI_ERROR;
  }
  /* Return status */
  return ret;
}

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/