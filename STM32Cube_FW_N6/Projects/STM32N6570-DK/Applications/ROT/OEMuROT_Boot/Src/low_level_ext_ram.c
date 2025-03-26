/**
  ******************************************************************************
  * @file    low_level_ext_ram.c
  * @author  MCD Application Team
  * @brief   Low Level Interface module to access the XSPI external RAM
  *
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

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx.h"
#include "stm32_extmem_conf.h"
#include "low_level_ext_ram.h"

/* Global variables ----------------------------------------------------------*/
XSPI_HandleTypeDef XSPI_RUN_HANDLE;

/* Functions Definition ------------------------------------------------------*/
/**
  * @brief Initialize the external RAM
  * @retval HAL status
  */
HAL_StatusTypeDef Ext_Ram_Initialize(void)
{
  HAL_StatusTypeDef status;
  XSPIM_CfgTypeDef xspi_manager_cfg = {0};

  XSPI_RUN_HANDLE.Instance = EXT_RAM_XSPI_INSTANCE;

  XSPI_RUN_HANDLE.Init.ChipSelectBoundary      = HAL_XSPI_BONDARYOF_NONE;
  XSPI_RUN_HANDLE.Init.ChipSelectHighTimeCycle = 1U;
  XSPI_RUN_HANDLE.Init.ClockMode               = HAL_XSPI_CLOCK_MODE_0;
  XSPI_RUN_HANDLE.Init.ClockPrescaler          = 4U;
  XSPI_RUN_HANDLE.Init.DelayHoldQuarterCycle   = HAL_XSPI_DHQC_DISABLE;
  XSPI_RUN_HANDLE.Init.FifoThresholdByte       = 4U;
  XSPI_RUN_HANDLE.Init.FreeRunningClock        = HAL_XSPI_FREERUNCLK_DISABLE;
  XSPI_RUN_HANDLE.Init.MemoryMode              = HAL_XSPI_SINGLE_MEM;
  XSPI_RUN_HANDLE.Init.MemorySize              = HAL_XSPI_SIZE_256MB;
  XSPI_RUN_HANDLE.Init.MemoryType              = HAL_XSPI_MEMTYPE_APMEM;
  XSPI_RUN_HANDLE.Init.SampleShifting          = HAL_XSPI_SAMPLE_SHIFT_NONE;
  XSPI_RUN_HANDLE.Init.WrapSize                = HAL_XSPI_WRAP_NOT_SUPPORTED;
  XSPI_RUN_HANDLE.Init.MaxTran                 = 0U;
  XSPI_RUN_HANDLE.Init.Refresh                 = 0U;
  XSPI_RUN_HANDLE.Init.MemorySelect            = HAL_XSPI_CSSEL_NCS1;
  status = HAL_XSPI_Init(&XSPI_RUN_HANDLE);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Connect Instance2(XSPI2/MCE2) to port1 */
  xspi_manager_cfg.IOPort = HAL_XSPIM_IOPORT_2;
  xspi_manager_cfg.nCSOverride = HAL_XSPI_CSSEL_OVR_NCS1;
  xspi_manager_cfg.Req2AckTime = 100;
  if (HAL_XSPIM_Config(&XSPI_RUN_HANDLE, &xspi_manager_cfg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Initialize the EXTMEM */
  if (EXT_RAM_XSPI_INSTANCE == XSPI1)
  {
    if (EXTMEM_Init(MEMORY_PSRAM_0, 50000000) != EXTMEM_OK)
    {
      return HAL_ERROR;
    }
  }
  else if (EXT_RAM_XSPI_INSTANCE == XSPI2)
  {
    if (EXTMEM_Init(MEMORY_PSRAM_0, 50000000) != EXTMEM_OK)
    {
      return HAL_ERROR;
    }
  }
  else
  {
    return HAL_ERROR;
  }

  /* Enable default memory mapped mode */
  if (EXTMEM_MemoryMappedMode(MEMORY_PSRAM_0, EXTMEM_ENABLE) != EXTMEM_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}
