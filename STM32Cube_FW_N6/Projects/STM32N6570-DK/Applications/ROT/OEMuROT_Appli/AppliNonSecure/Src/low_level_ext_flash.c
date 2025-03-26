/*
 * Copyright (c) 2013-2018 Arm Limited
 * Copyright (c) 2023 STMicroelectronics
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "low_level_ext_flash.h"
#include "stm32_extmem_conf.h"

#include "appli_flash_layout.h"

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif /* ARG_UNUSED */

/* Size of the DMA shared memory to perform memory-mapped writing with MCE */
#define BL2_DMA_SHARED_MEM_SIZE         (0x100)               /* 256 bytes */

/* config for flash driver */
#if defined(OEMIROT_MCE_PROTECTION)
#define EXT_FLASH0_PROG_UNIT  0x10 /* Cryptographic constraint for MCE writing */
#else /* not OEMIROT_MCE_PROTECTION */
#define EXT_FLASH0_PROG_UNIT  0x04 /* STM32_ExtMem_Manager constraint for mapped-mode writing */
#endif /* OEMIROT_MCE_PROTECTION */
#define EXT_FLASH0_ERASED_VAL 0xff
/*
#define DEBUG_EXT_FLASH_ACCESS
#define CHECK_ERASE
#define CHECK_WRITE
*/
#if !defined(LOCAL_LOADER_CONFIG)
/* Driver version */
#define ARM_EXT_FLASH_DRV_VERSION   ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

static const ARM_DRIVER_VERSION DriverVersion =
{
  ARM_FLASH_API_VERSION,  /* Defined in the CMSIS Flash Driver header file */
  ARM_EXT_FLASH_DRV_VERSION
};

/**
  * \brief Flash driver capability macro definitions \ref ARM_FLASH_CAPABILITIES
  */
/* Flash Ready event generation capability values */
#define EVENT_READY_NOT_AVAILABLE   (0u)
#define EVENT_READY_AVAILABLE       (1u)
/* Data access size values */
#define DATA_WIDTH_8BIT             (0u)
#define DATA_WIDTH_16BIT            (1u)
#define DATA_WIDTH_32BIT            (2u)
/* Chip erase capability values */
#define CHIP_ERASE_NOT_SUPPORTED    (0u)
#define CHIP_ERASE_SUPPORTED        (1u)

/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES DriverCapabilities =
{
  EVENT_READY_NOT_AVAILABLE,
  DATA_WIDTH_32BIT,
  CHIP_ERASE_SUPPORTED
};
#endif /* not LOCAL_LOADER_CONFIG */
/**
  * \brief Flash status macro definitions \ref ARM_FLASH_STATUS
  */
/* Busy status values of the Flash driver  */
#define DRIVER_STATUS_IDLE      (0u)
#define DRIVER_STATUS_BUSY      (1u)
/* Error status values of the Flash driver */
#define DRIVER_STATUS_NO_ERROR  (0u)
#define DRIVER_STATUS_ERROR     (1u)

/**
  * \brief Arm Flash device structure.
  */
struct arm_ext_flash_dev_t
{
  struct low_level_ext_flash_device *dev;
  ARM_FLASH_INFO *data;                   /*!< EXT FLASH memory device data */
  EXTMEM_NOR_SFDP_FlashInfoTypeDef *info; /* Dynamic information on the external flash */
};

DMA_HandleTypeDef handle_GPDMA1_Channel12;
DMA_HandleTypeDef handle_GPDMA1_Channel13;
uint8_t memory_mapped_write_buffer[BL2_DMA_SHARED_MEM_SIZE];
/**
  * @}
  */

/**
  * \brief      Check if the Flash memory boundaries are not violated.
  * \param[in]  flash_dev  Flash device structure \ref arm_ext_flash_dev_t
  * \param[in]  offset     Highest Flash memory address which would be accessed.
  * \return     Returns true if Flash memory boundaries are not violated, false
  *             otherwise.
  */

static bool is_range_valid(struct arm_ext_flash_dev_t *flash_dev,
                           uint32_t offset)
{
  uint32_t flash_limit = 0;

  /* Calculating the highest address of the Flash memory address range */
  flash_limit = (0x1UL << flash_dev->info->FlashSize) - 1;

  return (offset > flash_limit) ? (false) : (true) ;
}
/**
  * \brief        Check if the parameter is an erasable page.
  * \param[in]    flash_dev  Flash device structure \ref arm_ext_flash_dev_t
  * \param[in]    param      Any number that can be checked against the
  *                          program_unit, e.g. Flash memory address or
  *                          data length in bytes.
  * \return       Returns true if param is a sector eraseable, false
  *               otherwise.
  */
static bool is_erase_allow(struct arm_ext_flash_dev_t *flash_dev,
                           uint32_t param)
{
  /*  allow erase in range provided by device info */
  struct ext_flash_vect *vect = &flash_dev->dev->erase;
  uint32_t nb;
  for (nb = 0; nb < vect->nb; nb++)
    if ((param >= vect->range[nb].base) && (param <= vect->range[nb].limit))
    {
      return true;
    }
  return false;
}
/**
  * \brief        Check if the parameter is writeable area.
  * \param[in]    flash_dev  Flash device structure \ref arm_ext_flash_dev_t
  * \param[in]    param      Any number that can be checked against the
  *                          program_unit, e.g. Flash memory address or
  *                          data length in bytes.
  * \return       Returns true if param is aligned to program_unit, false
  *               otherwise.
  */
static bool is_write_allow(struct arm_ext_flash_dev_t *flash_dev,
                           uint32_t start, uint32_t len)
{
  /*  allow write access in area provided by device info */
  struct ext_flash_vect *vect = &flash_dev->dev->write;
  uint32_t nb;
  for (nb = 0; nb < vect->nb; nb++)
    if ((start >= vect->range[nb].base) && ((start + len - 1) <= vect->range[nb].limit))
    {
      return true;
    }
  return false;
}

/**
  * \brief        Check if the parameter is aligned to program_unit.
  * \param[in]    flash_dev  Flash device structure \ref arm_ext_flash_dev_t
  * \param[in]    param      Any number that can be checked against the
  *                          program_unit, e.g. Flash memory address or
  *                          data length in bytes.
  * \return       Returns true if param is aligned to program_unit, false
  *               otherwise.
  */

static bool is_write_aligned(struct arm_ext_flash_dev_t *flash_dev,
                             uint32_t param)
{
  return ((param % flash_dev->data->program_unit) != 0) ? (false) : (true);
}
/**
  * \brief        Check if the parameter is aligned to page_unit.
  * \param[in]    flash_dev  Flash device structure \ref arm_ext_flash_dev_t
  * \param[in]    param      Any number that can be checked against the
  *                          program_unit, e.g. Flash memory address or
  *                          data length in bytes.
  * \return       Returns true if param is aligned to sector_unit, false
  *               otherwise.
  */
static bool is_erase_aligned(struct arm_ext_flash_dev_t *flash_dev,
                             uint32_t param)
{
  /*  2 pages */
  return ((param % (flash_dev->data->sector_size)) != 0) ? (false) : (true);
}

/**
  * @brief  Initializes the DMA used for memory-mapped data writing
  * @param  None
  * @retval None
  */
static void Ext_Flash_XspiDmaInit(void)
{
  /* Enable DMAs clock */
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  handle_GPDMA1_Channel12.Instance = GPDMA1_Channel12;
  handle_GPDMA1_Channel12.Init.Request = DMA_REQUEST_SW;
  handle_GPDMA1_Channel12.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
  handle_GPDMA1_Channel12.Init.Direction = DMA_MEMORY_TO_MEMORY;
  handle_GPDMA1_Channel12.Init.SrcInc = DMA_SINC_INCREMENTED;
  handle_GPDMA1_Channel12.Init.DestInc = DMA_DINC_INCREMENTED;
  handle_GPDMA1_Channel12.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
  handle_GPDMA1_Channel12.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
  handle_GPDMA1_Channel12.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
  handle_GPDMA1_Channel12.Init.SrcBurstLength = 1;
  handle_GPDMA1_Channel12.Init.DestBurstLength = 1;
  handle_GPDMA1_Channel12.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT1;
  handle_GPDMA1_Channel12.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
  handle_GPDMA1_Channel12.Init.Mode = DMA_NORMAL;

  HAL_DMA_Init(&handle_GPDMA1_Channel12);
  HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel12, DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC | DMA_CHANNEL_SRC_SEC | DMA_CHANNEL_DEST_SEC);

  handle_GPDMA1_Channel13.Instance = GPDMA1_Channel13;
  handle_GPDMA1_Channel13.Init.Request = DMA_REQUEST_SW;
  handle_GPDMA1_Channel13.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
  handle_GPDMA1_Channel13.Init.Direction = DMA_MEMORY_TO_MEMORY;
  handle_GPDMA1_Channel13.Init.SrcInc = DMA_SINC_INCREMENTED;
  handle_GPDMA1_Channel13.Init.DestInc = DMA_DINC_INCREMENTED;
  handle_GPDMA1_Channel13.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
  handle_GPDMA1_Channel13.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
  handle_GPDMA1_Channel13.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
  handle_GPDMA1_Channel13.Init.SrcBurstLength = 1;
  handle_GPDMA1_Channel13.Init.DestBurstLength = 1;
  handle_GPDMA1_Channel13.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT1;
  handle_GPDMA1_Channel13.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
  handle_GPDMA1_Channel13.Init.Mode = DMA_NORMAL;

  HAL_DMA_Init(&handle_GPDMA1_Channel13);
  HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel13, DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC | DMA_CHANNEL_SRC_SEC | DMA_CHANNEL_DEST_NSEC);
}

/**
  * @brief  De-initializes the DMA used for memory-mapped data writing
  * @param  None
  * @retval None
  */
static void Ext_Flash_XspiDmaDeInit(void)
{
  /* Disable DMAs clock */
  __HAL_RCC_GPDMA1_CLK_DISABLE();
}

/**
  * @brief  Implements the weak function from STM32ExtMem
  * \param[in]    destination_Address Destination address
  * \param[in]    ptrData             Data to write
  * \param[in]    DataSize            Data length in bytes
  * @param  None
  * @retval None
  */
void EXTMEM_MemCopy(uint32_t *destination_Address, const uint8_t *ptrData, uint32_t DataSize)
{
  /* Copy the data in a buffer accessible by the DMA */
  if (DataSize <= BL2_DMA_SHARED_MEM_SIZE)
  {
    memcpy((void *)memory_mapped_write_buffer, (void *)ptrData, DataSize);

    if ((uint32_t)destination_Address < 0x90050000)       /* Secure */
    {
      HAL_DMA_Start(&handle_GPDMA1_Channel12, (uint32_t)memory_mapped_write_buffer, (uint32_t)destination_Address, DataSize);
      HAL_DMA_PollForTransfer(&handle_GPDMA1_Channel12, HAL_DMA_FULL_TRANSFER, 100U);
    }
    else  /* Non Secure */
    {
      HAL_DMA_Start(&handle_GPDMA1_Channel13, (uint32_t)memory_mapped_write_buffer, (uint32_t)destination_Address, DataSize);
      HAL_DMA_PollForTransfer(&handle_GPDMA1_Channel13, HAL_DMA_FULL_TRANSFER, 100U);
    }
  }
}

static ARM_FLASH_INFO ARM_EXT_FLASH0_DEV_DATA =
{
  .sector_info    = NULL,     /* Uniform sector layout */
  .sector_count   = 0x0UL,    /* To be updated dynamically */
  .sector_size    = 0x0UL,    /* To be updated dynamically */
  .page_size      = 0x0UL,    /* To be updated dynamically */
  .program_unit   = EXT_FLASH0_PROG_UNIT,
  .erased_value   = EXT_FLASH0_ERASED_VAL,
};

static EXTMEM_NOR_SFDP_FlashInfoTypeDef EXTMEM_FLASH_INFO =
{
  .FlashSize      = 0UL,
  .PageSize       = 0UL,
  .EraseType1Size = 0UL,
  .EraseType2Size = 0UL,
  .EraseType3Size = 0UL,
  .EraseType4Size = 0UL
};

static struct arm_ext_flash_dev_t ARM_EXT_FLASH0_DEV =
{
  .dev  = &(EXT_FLASH0_DEV),
  .data = &(ARM_EXT_FLASH0_DEV_DATA),
  .info = &(EXTMEM_FLASH_INFO)
};

/* Flash Status */
static ARM_FLASH_STATUS ARM_EXT_FLASH0_STATUS = {0, 0, 0};

#if !defined(LOCAL_LOADER_CONFIG)
static ARM_DRIVER_VERSION Ext_Flash_GetVersion(void)
{
  return DriverVersion;
}

static ARM_FLASH_CAPABILITIES Ext_Flash_GetCapabilities(void)
{
  return DriverCapabilities;
}
#endif /* not LOCAL_LOADER_CONFIG */

static int32_t Ext_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
  ARG_UNUSED(cb_event);
  uint32_t flash_size_in_bytes;
  XSPIM_CfgTypeDef xspi_manager_cfg = {0};

  XSPI_HANDLE.Instance = EXT_FLASH_XSPI_INSTANCE;

  /* XSPI initialization */
  XSPI_HANDLE.Init.FifoThresholdByte       = 4U;
  XSPI_HANDLE.Init.MemoryMode              = HAL_XSPI_SINGLE_MEM;
  XSPI_HANDLE.Init.MemoryType              = HAL_XSPI_MEMTYPE_MACRONIX;
  XSPI_HANDLE.Init.MemorySize              = HAL_XSPI_SIZE_32GB;
  XSPI_HANDLE.Init.ChipSelectHighTimeCycle = 1U;
  XSPI_HANDLE.Init.FreeRunningClock        = HAL_XSPI_FREERUNCLK_DISABLE;
  XSPI_HANDLE.Init.ClockMode               = HAL_XSPI_CLOCK_MODE_0;
  XSPI_HANDLE.Init.WrapSize                = HAL_XSPI_WRAP_NOT_SUPPORTED;
  XSPI_HANDLE.Init.ClockPrescaler          = 1U;
  XSPI_HANDLE.Init.SampleShifting          = HAL_XSPI_SAMPLE_SHIFT_NONE;
  XSPI_HANDLE.Init.DelayHoldQuarterCycle   = HAL_XSPI_DHQC_ENABLE;
  XSPI_HANDLE.Init.ChipSelectBoundary      = HAL_XSPI_BONDARYOF_NONE;
  XSPI_HANDLE.Init.MaxTran                 = 0U;
  XSPI_HANDLE.Init.Refresh                 = 0U;
  XSPI_HANDLE.Init.MemorySelect            = HAL_XSPI_CSSEL_NCS1;
  if (HAL_XSPI_Init(&XSPI_HANDLE) != HAL_OK)
  {
    return ARM_DRIVER_ERROR;
  }

  /* Connect Instance1(XSPI1/MCE1) to port2 */
  xspi_manager_cfg.IOPort = HAL_XSPIM_IOPORT_2;
  xspi_manager_cfg.nCSOverride = HAL_XSPI_CSSEL_OVR_NCS1;
  xspi_manager_cfg.Req2AckTime = 100;
  if (HAL_XSPIM_Config(&XSPI_HANDLE, &xspi_manager_cfg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return ARM_DRIVER_ERROR;
  }

  /* Initialize DMA for the external memory */
  Ext_Flash_XspiDmaInit();

  /* Initialize the EXTMEM */
  if (EXT_FLASH_XSPI_INSTANCE == XSPI1)
  {
    if (EXTMEM_Init(MEMORY_SERIAL_0, HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI1)) != EXTMEM_OK)
    {
      return ARM_DRIVER_ERROR;
    }
  }
  else if (EXT_FLASH_XSPI_INSTANCE == XSPI2)
  {
    if (EXTMEM_Init(MEMORY_SERIAL_0, HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI2)) != EXTMEM_OK)
    {
      return ARM_DRIVER_ERROR;
    }
  }
  else
  {
    return ARM_DRIVER_ERROR;
  }

  /* Get flash information */
  EXTMEM_GetInfo(MEMORY_SERIAL_0, ARM_EXT_FLASH0_DEV.info);
  flash_size_in_bytes = (0x1UL << (ARM_EXT_FLASH0_DEV.info)->FlashSize);
  (ARM_EXT_FLASH0_DEV.data)->sector_count = flash_size_in_bytes / ((ARM_EXT_FLASH0_DEV.info)->EraseType1Size);
  (ARM_EXT_FLASH0_DEV.data)->sector_size = (ARM_EXT_FLASH0_DEV.info)->EraseType1Size;
  (ARM_EXT_FLASH0_DEV.data)->page_size = (ARM_EXT_FLASH0_DEV.info)->PageSize;

  /* Enable default memory mapped mode */
  if (EXTMEM_MemoryMappedMode(MEMORY_SERIAL_0, EXTMEM_ENABLE) != EXTMEM_OK)
  {
    return ARM_DRIVER_ERROR;
  }

  return ARM_DRIVER_OK;
}
#if !defined(LOCAL_LOADER_CONFIG)
static int32_t Ext_Flash_Uninitialize(void)
{
  /* Disable default memory mapped mode */
  if (EXTMEM_MemoryMappedMode(MEMORY_SERIAL_0, EXTMEM_DISABLE) != EXTMEM_OK)
  {
    return ARM_DRIVER_ERROR;
  }

  /* De-initialize the EXTMEM */
  if (EXTMEM_DeInit(MEMORY_SERIAL_0) != EXTMEM_OK)
  {
    return ARM_DRIVER_ERROR;
  }

  /* De-initialize DMA for the external memory */
  Ext_Flash_XspiDmaDeInit();

  return HAL_XSPI_DeInit(&XSPI_HANDLE);
}

static int32_t Ext_Flash_PowerControl(ARM_POWER_STATE state)
{
  switch (state)
  {
    case ARM_POWER_FULL:
      /* Nothing to be done */
      return ARM_DRIVER_OK;
    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
    default:
      return ARM_DRIVER_ERROR_PARAMETER;
  }
}

static int32_t Ext_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
  bool is_valid = true;

  ARM_EXT_FLASH0_STATUS.error = DRIVER_STATUS_NO_ERROR;

  /* Check Flash memory boundaries */
  is_valid = is_range_valid(&ARM_EXT_FLASH0_DEV, addr + cnt - 1);
  if (is_valid != true)
  {
    if (ARM_EXT_FLASH0_DEV.dev->read_error)
    {
      ARM_EXT_FLASH0_STATUS.error = DRIVER_STATUS_ERROR;
      return ARM_DRIVER_ERROR_PARAMETER;
    }
    memset(data, 0xff, cnt);
    return ARM_DRIVER_OK;
  }
  /*  ECC to implement with NMI */
  /*  do a memcpy */
#ifdef DEBUG_EXT_FLASH_ACCESS
  printf("read %lx n=%x \r\n", (addr + EXT_FLASH_BASE_ADDRESS), cnt);
#endif /*  DEBUG_EXT_FLASH_ACCESS */

  memcpy(data, (void *)((uint32_t)addr + EXT_FLASH_BASE_ADDRESS), cnt);

  return ARM_DRIVER_OK;
}
#endif /* not LOCAL_LOADER_CONFIG */
static int32_t Ext_Flash_ProgramData(uint32_t addr,
                                     const void *data, uint32_t cnt)
{
  uint32_t flash_base = (uint32_t)EXT_FLASH_BASE_ADDRESS;
  EXTMEM_StatusTypeDef err;
  EXTMEM_StatusTypeDef err_mapped_mem;
#if defined(CHECK_WRITE) || defined(DEBUG_EXT_FLASH_ACCESS)
  void *dest;
#endif /* CHECK_WRITE or DEBUG_EXT_FLASH_ACCESS */
  ARM_EXT_FLASH0_STATUS.error = DRIVER_STATUS_NO_ERROR;

#if defined(CHECK_WRITE) || defined(DEBUG_EXT_FLASH_ACCESS)
  dest = (void *)(flash_base + addr);
#endif /* CHECK_WRITE or DEBUG_EXT_FLASH_ACCESS */
#ifdef DEBUG_EXT_FLASH_ACCESS
  printf("write %x n=%x \r\n", (uint32_t) dest, cnt);
#endif /* DEBUG_EXT_FLASH_ACCESS */
  /* Check Flash memory boundaries and alignment with minimum write size
    * (program_unit), data size also needs to be a multiple of program_unit.
  */
  if ((!is_range_valid(&ARM_EXT_FLASH0_DEV, addr + cnt - 1)) ||
      (!is_write_aligned(&ARM_EXT_FLASH0_DEV, addr))     ||
      (!is_write_aligned(&ARM_EXT_FLASH0_DEV, cnt))      ||
      (!is_write_allow(&ARM_EXT_FLASH0_DEV, addr, cnt))
     )
  {
    ARM_EXT_FLASH0_STATUS.error = DRIVER_STATUS_ERROR;
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  /* Disable memory mapped mode */
  if (EXTMEM_MemoryMappedMode(MEMORY_SERIAL_0, EXTMEM_DISABLE) != EXTMEM_OK)
  {
    return ARM_DRIVER_ERROR;
  }

  ARM_EXT_FLASH0_STATUS.busy = DRIVER_STATUS_BUSY;

  /* Manage the write of the application in mapped mode */
  err = EXTMEM_WriteInMappedMode(MEMORY_SERIAL_0, (flash_base + addr), (uint8_t *) data, cnt);

  /* Enable back memory mapped mode (even in case of writing error) */
  err_mapped_mem = EXTMEM_MemoryMappedMode(MEMORY_SERIAL_0, EXTMEM_ENABLE);
  if ((err == EXTMEM_OK) && (err_mapped_mem != EXTMEM_OK))
  {
    err = err_mapped_mem;
  }

  ARM_EXT_FLASH0_STATUS.busy = DRIVER_STATUS_IDLE;

  /* compare data written */
#ifdef CHECK_WRITE
  if ((err == EXTMEM_OK) && memcmp(dest, data, cnt))
  {
    err = EXTMEM_ERROR_DRIVER;
#ifdef DEBUG_EXT_FLASH_ACCESS
    printf("write %x n=%x (cmp failed)\r\n", (uint32_t)(dest), cnt);
#endif /* DEBUG_EXT_FLASH_ACCESS */
  }
#endif /* CHECK_WRITE */
#ifdef DEBUG_EXT_FLASH_ACCESS
  if (err != EXTMEM_OK)
  {
    printf("failed write %x n=%x \r\n", (uint32_t)(dest), cnt);
  }
#endif /* DEBUG_EXT_FLASH_ACCESS */
  return (err == EXTMEM_OK) ? ARM_DRIVER_OK : ARM_DRIVER_ERROR;
}

static int32_t Ext_Flash_EraseSector(uint32_t addr)
{
  EXTMEM_StatusTypeDef err;
  EXTMEM_StatusTypeDef err_mapped_mem;
#ifdef CHECK_ERASE
  uint32_t i;
  uint32_t *pt;
#endif /* CHECK_ERASE */
#ifdef DEBUG_EXT_FLASH_ACCESS
  printf("erase %x\r\n", addr);
#endif /* DEBUG_EXT_FLASH_ACCESS */
  if (!(is_range_valid(&ARM_EXT_FLASH0_DEV, addr)) ||
      !(is_erase_aligned(&ARM_EXT_FLASH0_DEV, addr)) ||
      !(is_erase_allow(&ARM_EXT_FLASH0_DEV, addr)))
  {
    ARM_EXT_FLASH0_STATUS.error = DRIVER_STATUS_ERROR;
#ifdef DEBUG_EXT_FLASH_ACCESS
#if defined(__ARMCC_VERSION)
    printf("failed erase %x\r\n", addr);
#else
    printf("failed erase %lx\r\n", addr);
#endif /* __ARMCC_VERSION */
#endif /* DEBUG_EXT_FLASH_ACCESS */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  ARM_EXT_FLASH0_STATUS.error = DRIVER_STATUS_NO_ERROR;

  /* Disable memory mapped mode */
  if (EXTMEM_MemoryMappedMode(MEMORY_SERIAL_0, EXTMEM_DISABLE) != EXTMEM_OK)
  {
    return ARM_DRIVER_ERROR;
  }

  ARM_EXT_FLASH0_STATUS.busy = DRIVER_STATUS_BUSY;

  err = EXTMEM_EraseSector(MEMORY_SERIAL_0, addr, (ARM_EXT_FLASH0_DEV.data)->sector_size);

  /* Enable back memory mapped mode (even in case of erasing error) */
  err_mapped_mem = EXTMEM_MemoryMappedMode(MEMORY_SERIAL_0, EXTMEM_ENABLE);
  if ((err == EXTMEM_OK) && (err_mapped_mem != EXTMEM_OK))
  {
    err = err_mapped_mem;
  }

  ARM_EXT_FLASH0_STATUS.busy = DRIVER_STATUS_IDLE;

#ifdef DEBUG_EXT_FLASH_ACCESS
  if (err != EXTMEM_OK)
  {
    printf("erase failed \r\n");
  }
#endif /* DEBUG_EXT_FLASH_ACCESS */
#ifdef CHECK_ERASE
  /* addr is an offset */
  pt = (uint32_t *)((uint32_t)EXT_FLASH_BASE_ADDRESS + addr);
  for (i = 0; i > 0x400; i++)
  {
    if (pt[i] != 0xffffffff)
    {
#ifdef DEBUG_EXT_FLASH_ACCESS
      printf("erase failed off %x %x %x\r\n", addr, &pt[i], pt[i]);
#endif /* DEBUG_EXT_FLASH_ACCESS */
      err = EXTMEM_ERROR_DRIVER;
      break;
    }
  }
#endif /* CHECK_ERASE */
  return (err == EXTMEM_OK) ? ARM_DRIVER_OK : ARM_DRIVER_ERROR;
}
#if !defined(LOCAL_LOADER_CONFIG)
static int32_t Ext_Flash_EraseChip(void)
{
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static ARM_FLASH_STATUS Ext_Flash_GetStatus(void)
{
  return ARM_EXT_FLASH0_STATUS;
}
#endif /* not LOCAL_LOADER_CONFIG */
static ARM_FLASH_INFO *Ext_Flash_GetInfo(void)
{
  return ARM_EXT_FLASH0_DEV.data;
}

#if !defined(LOCAL_LOADER_CONFIG)

ARM_DRIVER_FLASH Driver_EXT_FLASH0 =
{
  /* Get Version */
  Ext_Flash_GetVersion,
  /* Get Capability */
  Ext_Flash_GetCapabilities,
  /* Initialize */
  Ext_Flash_Initialize,
  /* UnInitialize */
  Ext_Flash_Uninitialize,
  /* power control */
  Ext_Flash_PowerControl,
  /* Read data */
  Ext_Flash_ReadData,
  /* Program data */
  Ext_Flash_ProgramData,
  /* Erase Sector */
  Ext_Flash_EraseSector,
  /* Erase chip */
  Ext_Flash_EraseChip,
  /* Get Status */
  Ext_Flash_GetStatus,
  /* Get Info */
  Ext_Flash_GetInfo
};
#else /* LOCAL_LOADER_CONFIG */
ARM_DRIVER_FLASH Driver_EXT_FLASH0 =
{
  /* Get Version */
  NULL,
  /* Get Capability */
  NULL,
  /* Initialize */
  Flash_Initialize,
  /* UnInitialize */
  NULL,
  /* power control */
  NULL,
  /* Read data */
  NULL,
  /* Program data */
  Ext_Flash_ProgramData,
  /* Erase Sector */
  Ext_Flash_EraseSector,
  /* Erase chip */
  NULL,
  /* Get Status */
  NULL,
  /* Get Info */
  Ext_Flash_GetInfo
};
#endif /* not LOCAL_LOADER_CONFIG */
