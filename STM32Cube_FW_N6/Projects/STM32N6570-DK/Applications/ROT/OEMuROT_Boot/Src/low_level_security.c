/**
  ******************************************************************************
  * @file    low_level_security.c
  * @author  MCD Application Team
  * @brief   security protection implementation for secure boot on STM32N6xx
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
#include <string.h>
#include "boot_hal_cfg.h"
#include "boot_hal_flowcontrol.h"
#include "region_defs.h"
#include "mcuboot_config/mcuboot_config.h"
#include "bootutil/bootutil_log.h"
#include "target_cfg.h"
#include "bootutil_priv.h"

#include "low_level_rng.h"
#include "low_level_security.h"
#include "low_level_mce.h"

#define MPU_ATTRIBUTE_DEVICE      MPU_ATTRIBUTES_NUMBER0
#define MPU_ATTRIBUTE_CODE        MPU_ATTRIBUTES_NUMBER1
#define MPU_ATTRIBUTE_DATA        MPU_ATTRIBUTES_NUMBER2
#define MPU_ATTRIBUTE_DATANOCACHE MPU_ATTRIBUTES_NUMBER3

typedef struct
{
  RISAF_TypeDef *RISAFx;
  uint32_t Region;
  RISAF_BaseRegionConfig_t BaseRegionConfig;
} RISAF_BaseRegionCfg_t;

/** @defgroup OEMIROT_SECURITY_Private_Defines  Private Defines
  * @{
  */

/**************************
  * Initial configuration *
  *************************/

/* MPU configuration
  ================== */
const MPU_Attributes_InitTypeDef mpu_attributes[] =
{
  {
    MPU_ATTRIBUTES_NUMBER0,
    0x04 /* Device */
  },
  {
    MPU_ATTRIBUTES_NUMBER1,
    0xAA /* Code */
  },
  {
    MPU_ATTRIBUTES_NUMBER2,
    0xFF /* Data */
  },
  {
    MPU_ATTRIBUTES_NUMBER3,
    0x44 /* Data no cache */
  },
};

const MPU_Region_InitTypeDef mpu_region_cfg[] = {
  /* Region 0: Allows RW access to all slots areas */
  {
    MPU_REGION_ENABLE,
    MPU_REGION_NUMBER0,
    FLASH_AREA_BEGIN_ADDRESS,
    FLASH_AREA_END_ADDRESS - 1,
    MPU_ATTRIBUTE_DATA,
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
  },
#if !defined(MCUBOOT_OVERWRITE_ONLY)
  /* Region 1: Allows RW access on SCRATCH area */
  {
    MPU_REGION_ENABLE,
    MPU_REGION_NUMBER1,
    SCRATCH_AREA_START,
    SCRATCH_AREA_LIMIT,
    MPU_ATTRIBUTE_DATA,
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
  },
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) */
#ifdef MCUBOOT_USE_HASH_REF
  /* Region 2: Allows RW access on HASH REF area */
  {
    MPU_REGION_ENABLE,
    MPU_REGION_NUMBER2,
    HASH_REF_AREA_START,
    HASH_REF_AREA_LIMIT,
    MPU_ATTRIBUTE_DATA,
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
  },
#endif /* MCUBOOT_USE_HASH_REF */
  /* Region 3: Allows RW access to peripherals */
  {
    MPU_REGION_ENABLE,
    MPU_REGION_NUMBER3,
    PERIPH_BASE_S,
    PERIPH_BASE_S + 0xFFFFFFF,
    MPU_ATTRIBUTE_DEVICE,
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
  },
  /* Region 4: Allows execution of boot */
  {
    MPU_REGION_ENABLE,
    MPU_REGION_NUMBER4,
    BL2_CODE_START,
    BL2_CODE_LIMIT,
    MPU_ATTRIBUTE_CODE,
    MPU_REGION_PRIV_RO,
    MPU_INSTRUCTION_ACCESS_ENABLE,
    MPU_ACCESS_NOT_SHAREABLE,
  },
  /* Region 5: Allows RW access on boot DATA area */
  {
    MPU_REGION_ENABLE,
    MPU_REGION_NUMBER5,
    BL2_DATA_START,
    BL2_DATA_LIMIT,
    MPU_ATTRIBUTE_DATA,
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
  },
  /* Region 6: Allows reading access on revision ID area */
  {
    MPU_REGION_ENABLE,
    MPU_REGION_NUMBER6,
    REVID_BASE_S,
    REVID_BASE_S + 0x100,
    MPU_ATTRIBUTE_DATA,
    MPU_REGION_PRIV_RO,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
  }
};

const MPU_Region_InitTypeDef mpu_region_cfg_appli_s[] = {
  /* Region 0: Allows RW access to secondary slots areas */
  {
    MPU_REGION_DISABLE,
    MPU_REGION_NUMBER0,
    SECONDARY_PARTITION_START,
    SECONDARY_PARTITION_LIMIT,
    MPU_ATTRIBUTE_DATA,
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
  },
  /* Region 4: Allows execution of application secure */
  {
    MPU_REGION_ENABLE,
    MPU_REGION_NUMBER7,
    S_CODE_START,
    S_CODE_LIMIT,
    MPU_ATTRIBUTE_DATA,
    MPU_REGION_PRIV_RO,
    MPU_INSTRUCTION_ACCESS_ENABLE,
    MPU_ACCESS_NOT_SHAREABLE,
  },
  /* Region 5: Allows RW access on application secure DATA area */
  {
    MPU_REGION_ENABLE,
    MPU_REGION_NUMBER8,
    S_DATA_START,
    S_DATA_LIMIT,
    MPU_ATTRIBUTE_DATA,
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
  }
};

const MPU_Region_InitTypeDef mpu_region_cfg_appli_ns[] = {
  /* Region 0: Allows execution of application non secure */
  {
    MPU_REGION_ENABLE,
    MPU_REGION_NUMBER0,
    NS_CODE_START,
    NS_CODE_LIMIT,
    MPU_ATTRIBUTE_CODE,
    MPU_REGION_PRIV_RO,
    MPU_INSTRUCTION_ACCESS_ENABLE,
    MPU_ACCESS_NOT_SHAREABLE,
  },
  /* Region 1: Allows RW access on application non secure DATA area */
  {
    MPU_REGION_ENABLE,
    MPU_REGION_NUMBER1,
    NS_DATA_START,
    NS_DATA_LIMIT,
    MPU_ATTRIBUTE_DATA,
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
  },
  /* Region 2: Allows RW access to peripherals */
  {
    MPU_REGION_ENABLE,
    MPU_REGION_NUMBER2,
    PERIPH_BASE_NS,
    PERIPH_BASE_NS + 0xFFFFFFF,
    MPU_ATTRIBUTE_DEVICE,
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
  },
};

const struct sau_cfg_t region_sau_init_cfg[] = {
  /* Region 0: Allows external flash access */
  {
    0,
    /*NS_CODE_START*/0x70000000,
    /*NS_CODE_LIMIT*/0x7004FFFF,
    1,
#ifdef FLOW_CONTROL
    FLOW_STEP_SAU_I_EN_R1,
    FLOW_CTRL_SAU_I_EN_R1,
    FLOW_STEP_SAU_I_CH_R1,
    FLOW_CTRL_SAU_I_CH_R1,
#endif /* FLOW_CONTROL */
  },
  /* Region 1: Allows external flash access */
  {
    1,
    /*NS_CODE_START*/0x70050000,
    /*NS_CODE_LIMIT*/0x70100000,
    0,
#ifdef FLOW_CONTROL
    FLOW_STEP_SAU_I_EN_R1,
    FLOW_CTRL_SAU_I_EN_R1,
    FLOW_STEP_SAU_I_CH_R1,
    FLOW_CTRL_SAU_I_CH_R1,
#endif /* FLOW_CONTROL */
  },
  /* Region 2: Allows SRAM access */
  {
    2,
    NS_DATA_START,
    NS_DATA_LIMIT,
    0,
#ifdef FLOW_CONTROL
    FLOW_STEP_SAU_I_EN_R2,
    FLOW_CTRL_SAU_I_EN_R2,
    FLOW_STEP_SAU_I_CH_R2,
    FLOW_CTRL_SAU_I_CH_R2,
#endif /* FLOW_CONTROL */
  },
  /* Region 3: Allows peripheral access */
  {
    3,
    PERIPH_BASE_NS,
    PERIPH_BASE_NS + 0xFFFFFFF,
    0,
#ifdef FLOW_CONTROL
    FLOW_STEP_SAU_I_EN_R0,
    FLOW_CTRL_SAU_I_EN_R0,
    FLOW_STEP_SAU_I_CH_R0,
    FLOW_CTRL_SAU_I_CH_R0,
#endif /* FLOW_CONTROL */
  },
};



RISAF_BaseRegionCfg_t risaf_cfg[] =
{
  /* TCM */
  {
    RISAF1,
    RISAF_REGION_1,
    {
       RISAF_FILTER_ENABLE,
       RIF_ATTRIBUTE_NSEC,
       0,
       RIF_CID_MASK,
       RIF_CID_MASK,
       0x00000000,
       0x000FFFFF,
    }
  },
  /* FLEXRAM */
  {
    RISAF7,
    RISAF_REGION_1,
    {
       RISAF_FILTER_ENABLE,
       RIF_ATTRIBUTE_NSEC,
       0,
       RIF_CID_MASK,
       RIF_CID_MASK,
       0x00000000,
       0x00063FFF,
    }
  },
  /* AXISRAM1 */
  {
    RISAF2,
    RISAF_REGION_1,
    {
       RISAF_FILTER_ENABLE,
       RIF_ATTRIBUTE_NSEC,
       0,
       RIF_CID_MASK,
       RIF_CID_MASK,
       0x00000000,
       0x000FFFFF,
    }
  },
  /* AXISRAM2 */
  {
    RISAF3,
    RISAF_REGION_1,
    {
       RISAF_FILTER_ENABLE,
       RIF_ATTRIBUTE_SEC,
       0,
       RIF_CID_MASK,
       RIF_CID_MASK,
       0x00000000,
       0x000FFFFF,
    }
  },
  /* Application and data secure */
  {
    RISAF12,
    RISAF_REGION_1,
    {
       RISAF_FILTER_ENABLE,
       RIF_ATTRIBUTE_SEC,
       0,
       RIF_CID_MASK,
       RIF_CID_MASK,
       0x00000000,
       0x0004FFFF,
    }
  },
  /* Non secure flash areas */
  {
    RISAF12,
    RISAF_REGION_2,
    {
       RISAF_FILTER_ENABLE,
       RIF_ATTRIBUTE_NSEC,
       0,
       RIF_CID_MASK,
       RIF_CID_MASK,
       0x00050000,
       0x001FFFFF,
    }
  }
};

/**
  * @}
  */
/* Private function prototypes -----------------------------------------------*/
/** @defgroup OEMIROT_SECURITY_Private_Functions  Private Functions
  * @{
  */
static void sau_init_cfg(void);
static void rif_init_cfg(void);
static void mpu_init_cfg(void);
static void mpu_appli_cfg(void);
/**
  * @}
  */

/** @defgroup OEMIROT_SECURITY_Exported_Functions Exported Functions
  * @{
  */
/**
  * @brief  Apply the runtime security  protections to
  *
  * @param  None
  * @note   By default, the best security protections are applied
  * @retval None
  */
void LL_SECU_ApplyRunTimeProtections(void)
{
  /* Enable BusFault and SecureFault handlers (HardFault is default) */
  SCB->SHCSR |= (SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_SECUREFAULTENA_Msk);

  /* Configure and enable SAU */
  sau_init_cfg();

  /* Configure and enable RIF */
  rif_init_cfg();

  /* Set MPU to forbid execution outside of immutable code  */
  mpu_init_cfg();

  /* Configure and enable MCE */
  mcu_init_cfg();
}

/**
  * @brief  Update the runtime security protections for application start
  *
  * @param  None
  * @retval None
  */
void LL_SECU_UpdateRunTimeProtections(void)
{
  /* Update MPU config for application execeution */
  mpu_appli_cfg();
}

/**
  * @brief  RIF initialization
  * @param  None
  * @retval None
  */
static void rif_init_cfg(void)
{
  uint32_t i = 0U;

  /* Configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* RISAF Config */
    __HAL_RCC_RISAF_CLK_ENABLE();

    for (i = 0; i < ARRAY_SIZE(risaf_cfg); i++)
    {
      HAL_RIF_RISAF_ConfigBaseRegion(risaf_cfg[i].RISAFx, risaf_cfg[i].Region, &risaf_cfg[i].BaseRegionConfig);
    }

    __HAL_RCC_RIFSC_CLK_ENABLE();

    /* Set GPIOO as configurable by non-secure */
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RCC_PERIPH_INDEX_GPIOO, RIF_ATTRIBUTE_NSEC);
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RCC_PERIPH_INDEX_GPDMA1, RIF_ATTRIBUTE_SEC);
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_XSPI1, RIF_ATTRIBUTE_NSEC);
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_XSPI2, RIF_ATTRIBUTE_NSEC);
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_MCE1, RIF_ATTRIBUTE_SEC);
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_MCE2, RIF_ATTRIBUTE_SEC);
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RCC_PERIPH_INDEX_AXISRAM1, RIF_ATTRIBUTE_NSEC);
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_USART1, RIF_ATTRIBUTE_NSEC);

    /* Enable GPIOO clock and ensure GPIOO bank is powered on */
    __HAL_RCC_GPIOO_CLK_ENABLE();

    /* Configure PO1 as non-secure to be used for non-secure led toggling */
    HAL_GPIO_ConfigPinAttributes(GPIOO, GPIO_PIN_1, GPIO_PIN_NSEC);
  }
  /* Verification stage */
  else
  {

  }
}

/**
  * @brief  SAU configuration
  * @param  None
  * @retval None
  */
static void sau_init_cfg(void)
{
  uint32_t i = 0U;

  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* Disable SAU */
    SAU->CTRL &= ~(SAU_CTRL_ENABLE_Msk);

    for (i = 0; i < ARRAY_SIZE(region_sau_init_cfg); i++)
    {
      SAU->RNR = region_sau_init_cfg[i].RNR & SAU_RNR_REGION_Msk;
      SAU->RBAR = region_sau_init_cfg[i].RBAR & SAU_RBAR_BADDR_Msk;
      SAU->RLAR = (region_sau_init_cfg[i].RLAR & SAU_RLAR_LADDR_Msk) |
                  ((region_sau_init_cfg[i].nsc << SAU_RLAR_NSC_Pos)  & SAU_RLAR_NSC_Msk) |
                  SAU_RLAR_ENABLE_Msk;

      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, region_sau_init_cfg[i].flow_step_enable,
                                           region_sau_init_cfg[i].flow_ctrl_enable);
    }

    /* Enable SAU */
    SAU->CTRL |= (SAU_CTRL_ENABLE_Msk);

    SCB->NSACR = (SCB->NSACR & ~(SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk)) |
                   ((SCB_NSACR_CP10_11_VAL << SCB_NSACR_CP10_Pos) & (SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk));

    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_SAU_I_EN, FLOW_CTRL_SAU_I_EN);
  }
  /* verification stage */
  else
  {
  }
}

void LL_SECU_DisableCleanMpu(void)
{
  uint8_t i;

  MPU->CTRL = 0;

  for(i = MPU_REGION_NUMBER0; i <= MPU_REGION_NUMBER15; i++)
  {
    HAL_MPU_DisableRegion(i);
  }
}

/**
  * @brief  mpu init
  * @param  None
  * @retval None
  */
static void mpu_init_cfg(void)
{
#ifdef OEMIROT_MPU_PROTECTION
  uint32_t i;

  /* Configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* Configure secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(mpu_region_cfg); i++)
    {
      HAL_MPU_ConfigRegion(&mpu_region_cfg[i]);
      FLOW_CONTROL_STEP(uFlowProtectValue, mpu_region_cfg_s[i].flow_step_enable,
                                           mpu_region_cfg_s[i].flow_ctrl_enable);
    }

    for (i = 0; i < ARRAY_SIZE(mpu_attributes); i++)
    {
      HAL_MPU_ConfigMemoryAttributes(&mpu_attributes[i]);
    }

    /* Enable secure MPU */
    HAL_MPU_Enable(MPU_HARDFAULT_NMI);
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_I_EN, FLOW_CTRL_MPU_I_EN);

    /* Enable non secure MPU */
    HAL_MPU_Enable_NS(MPU_HARDFAULT_NMI);
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_NS_I_EN, FLOW_CTRL_MPU_NS_I_EN);
  }
  /* Verification stage */
  else
  {

  }
#endif /* OEMIROT_MPU_PROTECTION */
}

static void mpu_appli_cfg(void)
{
#ifdef OEMIROT_MPU_PROTECTION
  uint32_t i;

  /* Configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    HAL_MPU_DisableRegion(MPU_REGION_NUMBER0);

    /* Configure secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(mpu_region_cfg_appli_s); i++)
    {
      HAL_MPU_ConfigRegion(&mpu_region_cfg_appli_s[i]);
      FLOW_CONTROL_STEP(uFlowProtectValue, mpu_region_cfg_appli_s[i].flow_step_enable,
                                           mpu_region_cfg_appli_s[i].flow_ctrl_enable);
    }
    /* Configure non secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(mpu_region_cfg_appli_ns); i++)
    {
      HAL_MPU_ConfigRegion_NS(&mpu_region_cfg_appli_ns[i]);
      FLOW_CONTROL_STEP(uFlowProtectValue, mpu_region_cfg_appli_ns[i].flow_step_enable,
                                           mpu_region_cfg_appli_ns[i].flow_ctrl_enable);
    }
  }
  else
  {

  }
#endif /* OEMIROT_MPU_PROTECTION */
}

/**
  * @}
  */
