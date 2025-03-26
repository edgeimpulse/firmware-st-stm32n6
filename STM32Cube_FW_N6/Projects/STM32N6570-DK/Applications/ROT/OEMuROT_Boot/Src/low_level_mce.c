/**
  ******************************************************************************
  * @file    low_level_mce.c
  * @author  MCD Application Team
  * @brief   Low Level Interface module to use STM32 MCE Ip
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
#include "low_level_rng.h"
#include "low_level_mce.h"
#include "boot_hal_flowcontrol.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/* Private defines -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static MCE_HandleTypeDef hmce_primary = {.Instance = MCE2};

static MCE_RegionConfigTypeDef aRegion[] =
{
  /* Primary slots */
  {
    .ContextID = MCE_AES_CONTEXT_ID,
    .StartAddress = PRIMARY_AREA_START,
    .EndAddress = PRIMARY_AREA_LIMIT,
    .Mode = MCE_BLOCK_CIPHER,
  },
#if defined(MCUBOOT_USE_HASH_REF)
  /* Hash ref area */
 {
    .ContextID = MCE_AES_CONTEXT_ID,
    .StartAddress = HASH_REF_AREA_START,
    .EndAddress = HASH_REF_AREA_LIMIT,
    .Mode = MCE_NO_CIPHER,
  },
#endif /* MCUBOOT_USE_HASH_REF */
#if !defined(MCUBOOT_OVERWRITE_ONLY)
  /* Scratch area */
  {
    .ContextID = MCE_AES_CONTEXT_ID,
    .StartAddress = SCRATCH_AREA_START,
    .EndAddress = SCRATCH_AREA_LIMIT,
    .Mode = MCE_BLOCK_CIPHER,
  }
#endif /* not MCUBOOT_OVERWRITE_ONLY */
};

/* Private function prototypes -----------------------------------------------*/
static void LL_SAES_Get_Keys(uint32_t *p_derived_key, uint32_t *p_random_key);

/* Functions Definition ------------------------------------------------------*/
/**
  * @brief  Configure MCE IP and regions
  * @retval None
  */
void mcu_init_cfg(void)
{
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    MCE_RegionConfigTypeDef pRegionConfig = { 0U };
    MCE_AESConfigTypeDef primary_config = { 0U };
    uint32_t a_dkey[4U] = {0}, a_rkey[4U] = {0};

    /* Initialize MCE */
    if (HAL_MCE_Init(&hmce_primary) != HAL_OK)
    {
      Error_Handler();
    }

    pRegionConfig.ContextID = MCE_CONTEXT1;
    pRegionConfig.Mode = MCE_STREAM_CIPHER;
    pRegionConfig.StartAddress = 0x70020000;
    pRegionConfig.EndAddress = 0x7007FFFF;

    /* Configure regions */
    if (HAL_MCE_ConfigRegion(&hmce_primary, MCE_REGION1, &pRegionConfig) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_MCE_SetRegionAESContext(&hmce_primary, MCE_CONTEXT1, MCE_REGION1) != HAL_OK)
    {
      Error_Handler();
    }

    LL_SAES_Get_Keys(a_dkey, a_rkey);

    primary_config.Nonce [0] = 0xA3BB3AAB;
    primary_config.Nonce [1] = 0x8A87A1A8;
    primary_config.Version = 0x1234;
    primary_config.pKey = a_dkey;

    /* Configure AES context */
    if (HAL_MCE_ConfigAESContext(&hmce_primary, &primary_config, MCE_CONTEXT1) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_MCE_EnableAESContext(&hmce_primary, MCE_CONTEXT1) != HAL_OK)
    {
      Error_Handler();
    }
  }
  else
  {

  }
}

static void LL_SAES_Get_Keys(uint32_t *p_derived_key, uint32_t *p_random_key)
{
  CRYP_HandleTypeDef hcryp = { 0U };
  uint32_t a_Magic[4U] = {0xA3B21441U, 0x9B345FFEU, 0xC03655FBU, 0x87AB0F67U};
  size_t magic_length;

  /* Set the SAES parameters */
  hcryp.Instance            = SAES;
  hcryp.Init.DataType       = CRYP_NO_SWAP;
  hcryp.Init.Algorithm      = CRYP_AES_ECB;
  hcryp.Init.KeySelect      = CRYP_KEYSEL_HW;
  hcryp.Init.KeyMode        = CRYP_KEYMODE_NORMAL;
  hcryp.Init.KeySize        = CRYP_KEYSIZE_256B;

  /* Enable RNG clock */
  __HAL_RCC_RNG_CLK_ENABLE();
  /* Enable SAES clock */
  __HAL_RCC_SAES_CLK_ENABLE();

  /* Initialize SAES */
  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
    Error_Handler();
  }

  /* Generate key by encrypting magic with DHUK */
  if (HAL_CRYP_Encrypt(&hcryp, (uint32_t *)a_Magic, 4, p_derived_key, 100) != HAL_OK)
  {
    Error_Handler();
  }

  /* Generate random magic */
  RNG_GetBytes((uint8_t *)a_Magic, sizeof(a_Magic), &magic_length);
  if (magic_length != sizeof(a_Magic))
  {
    Error_Handler();
  }

  /* Generate key by encrypting random magic with DHUK */
  if (HAL_CRYP_Encrypt(&hcryp, (uint32_t *)a_Magic, 4, p_random_key, 100) != HAL_OK)
  {
    Error_Handler();
  }

  /* Deinitialize SAES */
  if (HAL_CRYP_DeInit(&hcryp) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Check the status of the code primary region
  * @retval true if enabled, false otherwise.
  */
bool LL_MCE_IsCodePrimaryRegionEnabled(void)
{
  MCE_Region_TypeDef *p_region;
  __IO uint32_t address;

  address = (__IO uint32_t)((uint32_t)hmce_primary.Instance + 0x40U + (0x10U * MCE_REGION1));
  p_region = (MCE_Region_TypeDef *)address;

  if ((p_region->REGCR & MCE_REGCR_BREN) == MCE_REGCR_BREN)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**
  * @brief  Disable the MCE protection of the code primary region
  * @retval HAL_OK if successful, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LL_MCE_DisableCodePrimaryRegion(void)
{
  return HAL_MCE_DisableRegion(&hmce_primary, MCE_REGION1);
}

/**
  * @brief  Enable the MCE protection of the code primary region
  * @retval HAL_OK if successful, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LL_MCE_EnableCodePrimaryRegion(void)
{
  return HAL_MCE_EnableRegion(&hmce_primary, MCE_REGION1);
}

/**
  * @brief  Check whether the buffer is in the code primary region
  * @param  pBuffer address of the buffer
  * @param  Length length of the buffer
  * @retval 1 if enabled, 0 otherwise.
  */
bool LL_MCE_IsBufferInCodePrimaryRegion(const uint8_t *pBuffer, size_t Length)
{
  if ((((uint32_t)pBuffer) >= aRegion[MCE_REGION1].StartAddress)
      && (((uint32_t)pBuffer) <= aRegion[MCE_REGION1].EndAddress))
  {
    return true;
  }
  if ((((uint32_t)(pBuffer + Length)) >= aRegion[MCE_REGION1].StartAddress)
      && (((uint32_t)(pBuffer + Length)) <= aRegion[MCE_REGION1].EndAddress))
  {
    return true;
  }
  return false;
}