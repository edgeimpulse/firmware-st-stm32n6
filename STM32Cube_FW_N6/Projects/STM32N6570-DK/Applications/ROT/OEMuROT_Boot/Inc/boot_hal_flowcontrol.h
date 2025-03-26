/**
  ******************************************************************************
  * @file    boot_hal_flowcontrol.h
  * @author  MCD Application Team
  * @brief   Header for flow control in boot_hal.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BOOT_HAL_FLOWCONTROL_H
#define BOOT_HAL_FLOWCONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "region_defs.h"
#include "boot_hal_cfg.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/**
  * @brief  Flow Control : Initial value
  */
#define FLOW_STAGE_CFG              0xfffffe00U
#define FLOW_STAGE_CHK              0xffffe01eU

/**
  * @brief  Flow Control : Initial value
  */
#define FLOW_CTRL_INIT_VALUE        0x00005776U        /*!< Init value definition */

/**
  * @brief  Flow Control : Steps definition
  */
#ifdef OEMIROT_MPU_PROTECTION
#define FLOW_STEP_MPU_I_EN_R0       0x00006787U        /*!< Step MPU  Region 0 Init enable value */
#if !defined(MCUBOOT_OVERWRITE_ONLY)
#define FLOW_STEP_MPU_I_EN_R1       0x00007999U        /*!< Step MPU  Region 1 Init enable value */
#else
#define FLOW_STEP_MPU_I_EN_R1       0x00000000U        /*!< No effect on control flow */
#endif
#define FLOW_STEP_MPU_I_EN_R2       0x0000aab5U        /*!< Step MPU  Region 2 Init enable value */
#define FLOW_STEP_MPU_I_EN_R4       0x0000cccdU        /*!< Step MPU  Region 4 Init enable value */
#define FLOW_STEP_MPU_I_EN_R5       0x0000d2d3U        /*!< Step MPU  Region 5 Init enable value */
#define FLOW_STEP_MPU_I_EN_R6       0x00043d7aU        /*!< Step MPU  Region 6 Init enable value */
#define FLOW_STEP_MPU_I_EN_R7       0x0001cf0eU        /*!< Step MPU  Region 7 Init enable value */
#define FLOW_STEP_MPU_I_EN_R8       0x0051445dU        /*!< Step MPU  Region 8 Init enable value */
#define FLOW_STEP_MPU_I_EN_R9       0x00517d08U        /*!< Step MPU Secure Region 9 Init enable value */
#define FLOW_STEP_MPU_I_EN_R10      0x0000b4abU        /*!< Step MPU Secure Region 10 Init enable value */
#define FLOW_STEP_MPU_I_EN          0x0000e3dcU        /*!< Step MPU Secure Init enable value */
#define FLOW_STEP_MPU_NS_I_EN_R0    0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_I_EN       0x00000000U        /*!< No effect on control flow */
#else /* OEMIROT_MPU_PROTECTION */
#define FLOW_STEP_MPU_I_EN_R0       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R1       0x00000000U        /*!< No effect on control flow */

#define FLOW_STEP_MPU_I_EN_R2       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R3       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R4       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R5       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R6       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R7       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R8       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R9       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN_R10      0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_EN          0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_I_EN_R0    0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_I_EN       0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_MPU_PROTECTION */

#define FLOW_STEP_SAU_I_EN_R0       0x000165baU        /*!< Step SAU Region 0 Init enable value */
#define FLOW_STEP_SAU_I_EN_R1       0x00017ba4U        /*!< Step SAU Region 1 Init enable value */
#define FLOW_STEP_SAU_I_EN_R2       0x00018799U        /*!< Step SAU Region 2 Init enable value */
#define FLOW_STEP_SAU_I_EN_R3       0x00019987U        /*!< Step SAU Region 3 Init enable value */
#define FLOW_STEP_SAU_I_EN_R4       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_SAU_I_EN_R5       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_SAU_I_EN          0x0001b696U        /*!< Step SAU enable Init value */

#if  (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
#define FLOW_STEP_TAMP_ACT_EN       0x000673eaU        /*!< Step Tamper active enable value */
#else
#define FLOW_STEP_TAMP_ACT_EN       0x00000000U        /*!< No effect on control flow */
#endif /* (OEMIROT_TAMPER_ENABLE == ALL_TAMPER) */
#if (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
#define FLOW_STEP_TAMP_INT_EN       0x00067f4dU        /*!< Step Tamper internal enable value */
#define FLOW_STEP_TAMP_SEC_EN       0x0006ad9fU        /*!< Step Tamper secure enable value */
#define FLOW_STEP_TAMP_PRIV_EN      0x0006b381U        /*!< Step Tamper privilege enable value */
#define FLOW_STEP_TAMP_CFG_EN       0x0006bf26U        /*!< Step Tamper configurable enable value */
#else
#define FLOW_STEP_TAMP_INT_EN       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_TAMP_SEC_EN       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_TAMP_PRIV_EN      0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_TAMP_CFG_EN       0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_TAMPER_ENABLE != NO_TAMPER) */

#ifdef OEMIROT_FLASH_PRIVONLY_ENABLE
#define FLOW_STEP_FLASH_P_EN        0x0001cef0U        /*!< Step FLASH Priv enable value */
#else
#define FLOW_STEP_FLASH_P_EN        0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_FLASH_PRIVONLY_ENABLE */

#if defined(OEMIROT_SECURE_USER_SRAM2_ECC) || defined(OEMIROT_SECURE_USER_SRAM1_ECC)
#define FLOW_STEP_RAMCFG_I_EN1      0x000a6e24U        /*!< Step RAM Double ECC errors enable value */
#else
#define FLOW_STEP_RAMCFG_I_EN1      0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_SECURE_USER_SRAM2_ECC || OEMIROT_SECURE_USER_SRAM1_ECC */

#ifdef OEMIROT_MPU_PROTECTION
#define FLOW_STEP_MPU_I_CH_R0       0x0001d0eeU        /*!< Step MPU  Region 0 Init check value */
#if !defined(MCUBOOT_OVERWRITE_ONLY)
#define FLOW_STEP_MPU_I_CH_R1       0x0001e1e1U        /*!< Step MPU  Region 1 Init check value */
#else
#define FLOW_STEP_MPU_I_CH_R1       0x00000000U        /*!< No effect on control flow */
#endif
#define FLOW_STEP_MPU_I_CH_R2       0x0001ffffU        /*!< Step MPU  Region 2 Init check value */
#define FLOW_STEP_MPU_I_CH_R3       0x00023bd7U        /*!< Step MPU  Region 3 Init check value */
#define FLOW_STEP_MPU_I_CH_R4       0x00025dafU        /*!< Step MPU  Region 4 Init check value */
#define FLOW_STEP_MPU_I_CH_R5       0x00029cf6U        /*!< Step MPU  Region 5 Init check value */
#define FLOW_STEP_MPU_I_CH_R6       0x0003da37U        /*!< Step MPU  Region 6 Init check value */
#define FLOW_STEP_MPU_I_CH_R7       0x0001fe01U        /*!< Step MPU  Region 7 Init check value */
#define FLOW_STEP_MPU_I_CH_R8       0x0052a4e2U        /*!< Step MPU  Region 8 Init check value */
#define FLOW_STEP_MPU_I_CH_R9       0x0052fd77U        /*!< Step MPU  Region 9 Init check value */
#define FLOW_STEP_MPU_I_CH_R10      0x00023bd7U        /*!< Step MPU Region 10 Init enable value */
#define FLOW_STEP_MPU_I_CH          0x0002e8fbU        /*!< Step MPU  Init check value */
#define FLOW_STEP_MPU_NS_I_CH_R0    0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_I_CH       0x00000000U        /*!< No effect on control flow */
#else /* OEMIROT_MPU_PROTECTION */
#define FLOW_STEP_MPU_I_CH_R0       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R1       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R2       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R4       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R5       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R6       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R7       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R8       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R9       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH_R10      0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_I_CH          0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_I_CH_R0    0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_NS_I_CH       0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_MPU_PROTECTION */

#define FLOW_STEP_SAU_I_CH_R0       0x00037d16U        /*!< Step SAU Region 0 Init check value */
#define FLOW_STEP_SAU_I_CH_R1       0x0003ae3aU        /*!< Step SAU Region 1 Init check value */
#define FLOW_STEP_SAU_I_CH_R2       0x0003b1daU        /*!< Step SAU Region 2 Init check value */
#define FLOW_STEP_SAU_I_CH_R3       0x0003bdb1U        /*!< Step SAU Region 3 Init check value */
#define FLOW_STEP_SAU_I_CH_R4       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_SAU_I_CH_R5       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_SAU_I_CH          0x0003c9bcU        /*!< Step SAU Init check value */

#if  (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
#define FLOW_STEP_TAMP_ACT_CH       0x0006ca19U        /*!< Step Tamper active check value */
#else
#define FLOW_STEP_TAMP_ACT_CH       0x00000000U        /*!< No effect on control flow */
#endif /*(OEMIROT_TAMPER_ENABLE == ALL_TAMPER) */
#if  (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
#define FLOW_STEP_TAMP_INT_CH       0x0006d407U        /*!< Step Tamper internal check value */
#define FLOW_STEP_TAMP_SEC_CH       0x0006d95eU        /*!< Step Tamper secure check value */
#define FLOW_STEP_TAMP_PRIV_CH      0x00072a07U        /*!< Step Tamper privilege check value */
#define FLOW_STEP_TAMP_CFG_CH       0x00073419U        /*!< Step Tamper configurable check value */
#else
#define FLOW_STEP_TAMP_INT_CH       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_TAMP_SEC_CH       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_TAMP_PRIV_CH      0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_TAMP_CFG_CH       0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_TAMPER_ENABLE != NO_TAMPER) */

#ifdef OEMIROT_FLASH_PRIVONLY_ENABLE
#define FLOW_STEP_FLASH_P_CH        0x0003d7a2U        /*!< Step FLASH Priv check value */
#else
#define FLOW_STEP_FLASH_P_CH        0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_FLASH_PRIVONLY_ENABLE */

#if defined(OEMIROT_SECURE_USER_SRAM2_ECC) || defined(OEMIROT_SECURE_USER_SRAM1_ECC)
#define FLOW_STEP_RAMCFG_I_CH1      0x000ab163U        /*!< Step RAM Double ECC errors check value */
#else
#define FLOW_STEP_RAMCFG_I_CH1      0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_SECURE_USER_SRAM2_ECC || OEMIROT_SECURE_USER_SRAM1_ECC */

#ifdef OEMIROT_MPU_PROTECTION
#define FLOW_STEP_MPU_A_EN_R1       0x000ac741U        /*!< Step MPU Region 1 Appli enable value */
#define FLOW_STEP_MPU_A_CH_R1       0x000b3642U        /*!< Step MPU Region 1 Appli check value */
#define FLOW_STEP_MPU_A_EN_R2       0x000b2437U        /*!< Step MPU Region 2 Appli enable value */
#define FLOW_STEP_MPU_A_CH_R2       0x000b448eU        /*!< Step MPU Region 2 Appli check value */
#if  MCUBOOT_S_DATA_IMAGE_NUMBER == 1
#define FLOW_STEP_MPU_A_EN_R3       0x000b285cU        /*!< Step MPU Region 3 Appli enable value */
#define FLOW_STEP_MPU_A_CH_R3       0x000c0cb9U        /*!< Step MPU Region 3 Appli check value */
#else
#define FLOW_STEP_MPU_A_EN_R3       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_A_CH_R3       0x00000000U        /*!< No effect on control flow */
#endif
#else
#define FLOW_STEP_MPU_A_EN_R1       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_A_CH_R1       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_A_EN_R2       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_A_CH_R2       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_A_EN_R3       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MPU_A_CH_R3       0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_MPU_PROTECTION */

#if  defined(OEMIROT_MCE_PROTECTION)
#define FLOW_STEP_MCE_I_CH1         0x001750d8U        /*!< Step MCE Init enable value */
#define FLOW_STEP_MCE_I_CH2         0x00196309U        /*!< Step MCE Init enable value */

#define FLOW_STEP_MCE_L_EN          0x000d8b31U        /*!< MCE key External Loader enable value */
#define FLOW_STEP_MCE_L_EN_R0       0x002983c5U        /*!< MCE Region 0 External Loader enable value */
#define FLOW_STEP_MCE_L_EN_R1       0x0031f8b2U        /*!< MCE Region 1 External Loader enable value */
#if defined(MCUBOOT_OVERWRITE_ONLY)
#define FLOW_STEP_MCE_L_EN_R2       0x00000000U        /*!< No effect on control flow */
#else /* not MCUBOOT_OVERWRITE_ONLY */
#define FLOW_STEP_MCE_L_EN_R2       0x0029b1acU        /*!< MCE Region 2 External Loader enable value */
#endif /* MCUBOOT_OVERWRITE_ONLY */
#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
#define FLOW_STEP_MCE_L_EN_R3       0x0018cb52U        /*!< MCE Region 3 External Loader enable value */
#else /* not (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM) */
#define FLOW_STEP_MCE_L_EN_R3       0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */
#define FLOW_STEP_MCE_L_EN_RLCK     0x000e845aU        /*!< MCE External Loader global lock enable value */
#define FLOW_STEP_MCE_L_CH          0x003a6074U        /*!< MCE key External Loader check value */
#define FLOW_STEP_MCE_L_CH_R0       0x0006ca19U        /*!< MCE Region 0 External Loader check value */
#define FLOW_STEP_MCE_L_CH_R1       0x0006d407U        /*!< MCE Region 1 External Loader check value */
#if defined(MCUBOOT_OVERWRITE_ONLY)
#define FLOW_STEP_MCE_L_CH_R2       0x00000000U        /*!< No effect on control flow */
#else /* not MCUBOOT_OVERWRITE_ONLY */
#define FLOW_STEP_MCE_L_CH_R2       0x0006d95eU        /*!< MCE Region 2 External Loader check value */
#endif /* MCUBOOT_OVERWRITE_ONLY */
#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
#define FLOW_STEP_MCE_L_CH_R3       0x0018d2b4U        /*!< MCE Region 3 External Loader check value */
#else /* not (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM) */
#define FLOW_STEP_MCE_L_CH_R3       0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */
#define FLOW_STEP_MCE_L_CH_RLCK     0x0006d5f9U        /*!< MCE External Loader global lock check value */

#define FLOW_STEP_MCE_O_EN          0x00043d7aU        /*!< Step MCE key OEMiROT_Boot enable value */
#define FLOW_STEP_MCE_O_EN_R0       0x00325d90U        /*!< MCE Region 0 OEMiROT_Boot enable value */
#define FLOW_STEP_MCE_O_EN_R1       0x0005c37aU        /*!< MCE Region 1 OEMiROT_Boot enable value */
#if defined(MCUBOOT_OVERWRITE_ONLY)
#define FLOW_STEP_MCE_O_EN_R2       0x00000000U        /*!< No effect on control flow */
#else /* not MCUBOOT_OVERWRITE_ONLY */
#define FLOW_STEP_MCE_O_EN_R2       0x0027a250U        /*!< MCE Region 2 OEMiROT_Boot enable value */
#endif /* MCUBOOT_OVERWRITE_ONLY */
#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
#define FLOW_STEP_MCE_O_EN_R3       0x0018d60aU        /*!< MCE Region 3 OEMiROT_Boot enable value */
#else /* not (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM) */
#define FLOW_STEP_MCE_O_EN_R3       0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */
#define FLOW_STEP_MCE_O_CH          0x003a6074U        /*!< Step MCE key OEMiROT_Boot check value */
#define FLOW_STEP_MCE_O_CH_R0       0x0006bf26U        /*!< MCE Region 0 OEMiROT_Boot check value */
#define FLOW_STEP_MCE_O_CH_R1       0x000c0cb9U        /*!< MCE Region 1 OEMiROT_Boot check value */
#if defined(MCUBOOT_OVERWRITE_ONLY)
#define FLOW_STEP_MCE_O_CH_R2       0x00000000U        /*!< No effect on control flow */
#else /* not MCUBOOT_OVERWRITE_ONLY */
#define FLOW_STEP_MCE_O_CH_R2       0xfff9f92aU        /*!< MCE Region 2 OEMiROT_Boot check value */
#endif /* MCUBOOT_OVERWRITE_ONLY */
#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
#define FLOW_STEP_MCE_O_CH_R3       0x00191c23U        /*!< MCE Region 3 OEMiROT_Boot check value */
#else /* not (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM) */
#define FLOW_STEP_MCE_O_CH_R3       0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

#define FLOW_STEP_MCE_A_EN          0x00302257U        /*!< Step MCE key Application enable value */
#define FLOW_STEP_MCE_A_EN_R0       0x00066153U        /*!< MCE Region 0 Application enable value */
#define FLOW_STEP_MCE_A_EN_R1       0x000738beU        /*!< MCE Region 1 Application enable value */
#if defined(MCUBOOT_OVERWRITE_ONLY)
#define FLOW_STEP_MCE_A_EN_R2       0x00000000U        /*!< No effect on control flow */
#else /* not MCUBOOT_OVERWRITE_ONLY */
#define FLOW_STEP_MCE_A_EN_R2       0x00067f4dU        /*!< MCE Region 2 Application enable value */
#endif /* MCUBOOT_OVERWRITE_ONLY */
#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
#define FLOW_STEP_MCE_A_EN_R3       0x0019288fU        /*!< MCE Region 3 Application enable value */
#else /* not (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM) */
#define FLOW_STEP_MCE_A_EN_R3       0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */
#define FLOW_STEP_MCE_A_EN_RLCK     0x000e4968U        /*!< MCE Application global lock enable value */
#define FLOW_STEP_MCE_A_CH          0x000ab163U        /*!< Step MCE key Application check value */
#define FLOW_STEP_MCE_A_CH_R0       0x000ca62dU        /*!< MCE Region 0 Application check value */
#define FLOW_STEP_MCE_A_CH_R1       0x0012e223U        /*!< MCE Region 1 Application check value */
#if defined(MCUBOOT_OVERWRITE_ONLY)
#define FLOW_STEP_MCE_A_CH_R2       0x00000000U        /*!< No effect on control flow */
#else /* not MCUBOOT_OVERWRITE_ONLY */
#define FLOW_STEP_MCE_A_CH_R2       0x0018079eU        /*!< MCE Region 2 Application check value */
#endif /* MCUBOOT_OVERWRITE_ONLY */
#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
#define FLOW_STEP_MCE_A_CH_R3       0x00196309U        /*!< MCE Region 3 Application check value */
#else /* not (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM) */
#define FLOW_STEP_MCE_A_CH_R3       0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */
#define FLOW_STEP_MCE_A_CH_RLCK     0x003016adU        /*!< MCE Application global lock check value */
#else /* not OEMIROT_MCE_PROTECTION */
#define FLOW_STEP_MCE_I_CH1         0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_I_CH2         0x00000000U        /*!< No effect on control flow */

#define FLOW_STEP_MCE_L_EN          0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_L_EN_R0       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_L_EN_R1       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_L_EN_R2       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_L_EN_R3       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_L_EN_RLCK     0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_L_CH          0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_L_CH_R0       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_L_CH_R1       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_L_CH_R2       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_L_CH_R3       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_L_CH_RLCK     0x00000000U        /*!< No effect on control flow */

#define FLOW_STEP_MCE_O_EN          0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_O_EN_R0       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_O_EN_R1       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_O_EN_R2       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_O_EN_R3       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_O_CH          0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_O_CH_R0       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_O_CH_R1       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_O_CH_R2       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_O_CH_R3       0x00000000U        /*!< No effect on control flow */

#define FLOW_STEP_MCE_A_EN          0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_A_EN_R0       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_A_EN_R1       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_A_EN_R2       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_A_EN_R3       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_A_EN_RLCK     0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_A_CH          0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_A_CH_R0       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_A_CH_R1       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_A_CH_R2       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_A_CH_R3       0x00000000U        /*!< No effect on control flow */
#define FLOW_STEP_MCE_A_CH_RLCK     0x00000000U        /*!< No effect on control flow */
#endif /* OEMIROT_MCE_PROTECTION */

  
/**
  * @brief  SFU_BOOT Flow Control : Control values runtime protections
  */
/* Flow control Stage 1 */
#define FLOW_CTRL_MPU_I_EN_R0       (FLOW_CTRL_INIT_VALUE ^      FLOW_STEP_MPU_I_EN_R0)
#define FLOW_CTRL_MPU_I_EN_R1       (FLOW_CTRL_MPU_I_EN_R0 ^     FLOW_STEP_MPU_I_EN_R1)
#define FLOW_CTRL_MPU_I_EN_R2       (FLOW_CTRL_MPU_I_EN_R1 ^     FLOW_STEP_MPU_I_EN_R2)
#define FLOW_CTRL_MPU_I_EN_R4       (FLOW_CTRL_MPU_I_EN_R2 ^     FLOW_STEP_MPU_I_EN_R4)
#define FLOW_CTRL_MPU_I_EN_R5       (FLOW_CTRL_MPU_I_EN_R4 ^     FLOW_STEP_MPU_I_EN_R5)
#define FLOW_CTRL_MPU_I_EN_R6       (FLOW_CTRL_MPU_I_EN_R5 ^     FLOW_STEP_MPU_I_EN_R6)
#define FLOW_CTRL_MPU_I_EN_R7       (FLOW_CTRL_MPU_I_EN_R6 ^     FLOW_STEP_MPU_I_EN_R7)
#define FLOW_CTRL_MPU_I_EN_R8       (FLOW_CTRL_MPU_I_EN_R7 ^     FLOW_STEP_MPU_I_EN_R8)
#define FLOW_CTRL_MPU_I_EN_R9       (FLOW_CTRL_MPU_I_EN_R8 ^     FLOW_STEP_MPU_I_EN_R9)
#define FLOW_CTRL_MPU_I_EN_R10      (FLOW_CTRL_MPU_I_EN_R9 ^     FLOW_STEP_MPU_I_EN_R10)
#define FLOW_CTRL_MPU_I_EN          (FLOW_CTRL_MPU_I_EN_R10 ^    FLOW_STEP_MPU_I_EN)

#define FLOW_CTRL_MPU_NS_I_EN_R0    (FLOW_CTRL_MPU_I_EN ^        FLOW_STEP_MPU_NS_I_EN_R0)
#define FLOW_CTRL_MPU_NS_I_EN       (FLOW_CTRL_MPU_NS_I_EN_R0 ^  FLOW_STEP_MPU_NS_I_EN)

#define FLOW_CTRL_MCE_O_EN          (FLOW_CTRL_MPU_NS_I_EN ^     FLOW_STEP_MCE_O_EN)
#define FLOW_CTRL_MCE_O_EN_R0       (FLOW_CTRL_MCE_O_EN ^        FLOW_STEP_MCE_O_EN_R0)
#define FLOW_CTRL_MCE_O_EN_R1       (FLOW_CTRL_MCE_O_EN_R0 ^     FLOW_STEP_MCE_O_EN_R1)
#define FLOW_CTRL_MCE_O_EN_R2       (FLOW_CTRL_MCE_O_EN_R1 ^     FLOW_STEP_MCE_O_EN_R2)
#define FLOW_CTRL_MCE_O_EN_R3       (FLOW_CTRL_MCE_O_EN_R2 ^     FLOW_STEP_MCE_O_EN_R3)  
  
#define FLOW_CTRL_SAU_I_EN_R0       (FLOW_CTRL_MPU_NS_I_EN ^     FLOW_STEP_SAU_I_EN_R0)
#define FLOW_CTRL_SAU_I_EN_R1       (FLOW_CTRL_SAU_I_EN_R0 ^     FLOW_STEP_SAU_I_EN_R1)
#define FLOW_CTRL_SAU_I_EN_R2       (FLOW_CTRL_SAU_I_EN_R1 ^     FLOW_STEP_SAU_I_EN_R2)
#define FLOW_CTRL_SAU_I_EN_R3       (FLOW_CTRL_SAU_I_EN_R2 ^     FLOW_STEP_SAU_I_EN_R3)
#define FLOW_CTRL_SAU_I_EN_R4       (FLOW_CTRL_SAU_I_EN_R3 ^     FLOW_STEP_SAU_I_EN_R4)
#define FLOW_CTRL_SAU_I_EN_R5       (FLOW_CTRL_SAU_I_EN_R4 ^     FLOW_STEP_SAU_I_EN_R5)
#define FLOW_CTRL_SAU_I_EN          (FLOW_CTRL_SAU_I_EN_R5 ^     FLOW_STEP_SAU_I_EN)

#define FLOW_CTRL_TAMP_ACT_EN       (FLOW_CTRL_SAU_I_EN ^        FLOW_STEP_TAMP_ACT_EN)
#define FLOW_CTRL_TAMP_INT_EN       (FLOW_CTRL_TAMP_ACT_EN ^     FLOW_STEP_TAMP_INT_EN)
#define FLOW_CTRL_TAMP_SEC_EN       (FLOW_CTRL_TAMP_INT_EN ^     FLOW_STEP_TAMP_SEC_EN)
#define FLOW_CTRL_TAMP_PRIV_EN      (FLOW_CTRL_TAMP_SEC_EN ^     FLOW_STEP_TAMP_PRIV_EN)
#define FLOW_CTRL_TAMP_CFG_EN       (FLOW_CTRL_TAMP_PRIV_EN ^    FLOW_STEP_TAMP_CFG_EN)

#define FLOW_CTRL_FLASH_P_EN        (FLOW_CTRL_TAMP_CFG_EN ^     FLOW_STEP_FLASH_P_EN)

#define FLOW_CTRL_RAMCFG_I_EN1      (FLOW_CTRL_FLASH_P_EN ^      FLOW_STEP_RAMCFG_I_EN1)

#define FLOW_CTRL_STAGE_1           FLOW_CTRL_RAMCFG_I_EN1

/* Flow control Stage 2 */
#define FLOW_CTRL_MPU_I_CH_R0       (FLOW_CTRL_STAGE_1 ^         FLOW_STEP_MPU_I_CH_R0)
#define FLOW_CTRL_MPU_I_CH_R1       (FLOW_CTRL_MPU_I_CH_R0 ^     FLOW_STEP_MPU_I_CH_R1)
#define FLOW_CTRL_MPU_I_CH_R2       (FLOW_CTRL_MPU_I_CH_R1 ^     FLOW_STEP_MPU_I_CH_R2)
#define FLOW_CTRL_MPU_I_CH_R4       (FLOW_CTRL_MPU_I_CH_R2 ^     FLOW_STEP_MPU_I_CH_R4)
#define FLOW_CTRL_MPU_I_CH_R5       (FLOW_CTRL_MPU_I_CH_R4 ^     FLOW_STEP_MPU_I_CH_R5)
#define FLOW_CTRL_MPU_I_CH_R6       (FLOW_CTRL_MPU_I_CH_R5 ^     FLOW_STEP_MPU_I_CH_R6)
#define FLOW_CTRL_MPU_I_CH_R7       (FLOW_CTRL_MPU_I_CH_R6 ^     FLOW_STEP_MPU_I_CH_R7)
#define FLOW_CTRL_MPU_I_CH_R8       (FLOW_CTRL_MPU_I_CH_R7 ^     FLOW_STEP_MPU_I_CH_R8)
#define FLOW_CTRL_MPU_I_CH_R9       (FLOW_CTRL_MPU_I_CH_R8 ^     FLOW_STEP_MPU_I_CH_R9)
#define FLOW_CTRL_MPU_I_CH_R10      (FLOW_CTRL_MPU_I_CH_R9 ^     FLOW_STEP_MPU_I_CH_R10)
#define FLOW_CTRL_MPU_I_CH          (FLOW_CTRL_MPU_I_CH_R10 ^    FLOW_STEP_MPU_I_CH)

#define FLOW_CTRL_MPU_NS_I_CH_R0    (FLOW_CTRL_MPU_I_CH ^        FLOW_STEP_MPU_NS_I_CH_R0)
#define FLOW_CTRL_MPU_NS_I_CH       (FLOW_CTRL_MPU_NS_I_CH_R0 ^  FLOW_STEP_MPU_NS_I_CH)

#define FLOW_CTRL_MCE_O_CH          (FLOW_CTRL_MPU_NS_I_CH ^     FLOW_STEP_MCE_O_CH)
#define FLOW_CTRL_MCE_O_CH_R0       (FLOW_CTRL_MCE_O_CH ^        FLOW_STEP_MCE_O_CH_R0)
#define FLOW_CTRL_MCE_O_CH_R1       (FLOW_CTRL_MCE_O_CH_R0 ^     FLOW_STEP_MCE_O_CH_R1)
#define FLOW_CTRL_MCE_O_CH_R2       (FLOW_CTRL_MCE_O_CH_R1 ^     FLOW_STEP_MCE_O_CH_R2)
#define FLOW_CTRL_MCE_O_CH_R3       (FLOW_CTRL_MCE_O_CH_R2 ^     FLOW_STEP_MCE_O_CH_R3)
  
#define FLOW_CTRL_SAU_I_CH_R0       (FLOW_CTRL_MPU_NS_I_CH ^     FLOW_STEP_SAU_I_CH_R0)
#define FLOW_CTRL_SAU_I_CH_R1       (FLOW_CTRL_SAU_I_CH_R0 ^     FLOW_STEP_SAU_I_CH_R1)
#define FLOW_CTRL_SAU_I_CH_R2       (FLOW_CTRL_SAU_I_CH_R1 ^     FLOW_STEP_SAU_I_CH_R2)
#define FLOW_CTRL_SAU_I_CH_R3       (FLOW_CTRL_SAU_I_CH_R2 ^     FLOW_STEP_SAU_I_CH_R3)
#define FLOW_CTRL_SAU_I_CH_R4       (FLOW_CTRL_SAU_I_CH_R3 ^     FLOW_STEP_SAU_I_CH_R4)
#define FLOW_CTRL_SAU_I_CH_R5       (FLOW_CTRL_SAU_I_CH_R4 ^     FLOW_STEP_SAU_I_CH_R5)
#define FLOW_CTRL_SAU_I_CH          (FLOW_CTRL_SAU_I_CH_R5 ^     FLOW_STEP_SAU_I_CH)

#define FLOW_CTRL_TAMP_ACT_CH       (FLOW_CTRL_SAU_I_CH ^        FLOW_STEP_TAMP_ACT_CH)
#define FLOW_CTRL_TAMP_INT_CH       (FLOW_CTRL_TAMP_ACT_CH ^     FLOW_STEP_TAMP_INT_CH)
#define FLOW_CTRL_TAMP_SEC_CH       (FLOW_CTRL_TAMP_INT_CH ^     FLOW_STEP_TAMP_SEC_CH)
#define FLOW_CTRL_TAMP_PRIV_CH      (FLOW_CTRL_TAMP_SEC_CH ^     FLOW_STEP_TAMP_PRIV_CH)
#define FLOW_CTRL_TAMP_CFG_CH       (FLOW_CTRL_TAMP_PRIV_CH ^    FLOW_STEP_TAMP_CFG_CH)

#define FLOW_CTRL_FLASH_P_CH        (FLOW_CTRL_TAMP_CFG_CH ^     FLOW_STEP_FLASH_P_CH)

#define FLOW_CTRL_RAMCFG_I_CH1      (FLOW_CTRL_FLASH_P_CH ^      FLOW_STEP_RAMCFG_I_CH1)

#define FLOW_CTRL_STAGE_2            FLOW_CTRL_RAMCFG_I_CH1

/* Flow control Stage 3 Appli */
#define FLOW_CTRL_MPU_A_EN_R1       (FLOW_CTRL_STAGE_2 ^         FLOW_STEP_MPU_A_EN_R1)
#define FLOW_CTRL_MPU_A_EN_R2       (FLOW_CTRL_MPU_A_EN_R1 ^     FLOW_STEP_MPU_A_EN_R2)
#define FLOW_CTRL_MPU_A_EN_R3       (FLOW_CTRL_MPU_A_EN_R2 ^     FLOW_STEP_MPU_A_EN_R3)

#define FLOW_CTRL_STAGE_3_A         FLOW_CTRL_MPU_A_EN_R3

/* Flow control Stage 4 Appli */
#define FLOW_CTRL_MPU_A_CH_R1       (FLOW_CTRL_STAGE_3_A ^       FLOW_STEP_MPU_A_CH_R1)
#define FLOW_CTRL_MPU_A_CH_R2       (FLOW_CTRL_MPU_A_CH_R1 ^     FLOW_STEP_MPU_A_CH_R2)
#define FLOW_CTRL_MPU_A_CH_R3       (FLOW_CTRL_MPU_A_CH_R2 ^     FLOW_STEP_MPU_A_CH_R3)

#define FLOW_CTRL_STAGE_4_A         FLOW_CTRL_MPU_A_CH_R3

/* Flow control Stage 3 Loader */
#define FLOW_CTRL_GTZC_L_EN_MPCBB1  (FLOW_CTRL_STAGE_2 ^         FLOW_STEP_GTZC_L_EN_MPCBB1)
#define FLOW_CTRL_GTZC_L_EN_TZSC    (FLOW_CTRL_GTZC_L_EN_MPCBB1 ^ FLOW_STEP_GTZC_L_EN_TZSC)

#define FLOW_CTRL_GPIO_L_EN         (FLOW_CTRL_GTZC_L_EN_TZSC ^  FLOW_STEP_GPIO_L_EN)

#define FLOW_CTRL_MPU_L_EN_R7       (FLOW_CTRL_GPIO_L_EN ^       FLOW_STEP_MPU_L_EN_R7)

#define FLOW_CTRL_SAU_L_EN_R0       (FLOW_CTRL_MPU_L_EN_R7  ^    FLOW_STEP_SAU_L_EN_R0)
#define FLOW_CTRL_SAU_L_EN_R1       (FLOW_CTRL_SAU_L_EN_R0 ^     FLOW_STEP_SAU_L_EN_R1)
#define FLOW_CTRL_SAU_L_EN_R2       (FLOW_CTRL_SAU_L_EN_R1 ^     FLOW_STEP_SAU_L_EN_R2)
#define FLOW_CTRL_SAU_L_EN_R3       (FLOW_CTRL_SAU_L_EN_R2 ^     FLOW_STEP_SAU_L_EN_R3)
#define FLOW_CTRL_SAU_L_EN_R4       (FLOW_CTRL_SAU_L_EN_R3 ^     FLOW_STEP_SAU_L_EN_R4)
#define FLOW_CTRL_SAU_L_EN_R5       (FLOW_CTRL_SAU_L_EN_R4 ^     FLOW_STEP_SAU_L_EN_R5)
#define FLOW_CTRL_SAU_L_EN          (FLOW_CTRL_SAU_L_EN_R5 ^     FLOW_STEP_SAU_L_EN)

#define FLOW_CTRL_NVIC_L_EN         (FLOW_CTRL_SAU_L_EN ^        FLOW_STEP_NVIC_L_EN)

#define FLOW_CTRL_HDPEXT_L_EN_B1    (FLOW_CTRL_NVIC_L_EN ^       FLOW_STEP_HDPEXT_L_EN_B1)
#define FLOW_CTRL_HDPEXT_L_EN_B2    (FLOW_CTRL_HDPEXT_L_EN_B1 ^  FLOW_STEP_HDPEXT_L_EN_B2)

#define FLOW_CTRL_STAGE_3_L          FLOW_CTRL_HDPEXT_L_EN_B2

/* Flow control Stage 4 Loader */
#define FLOW_CTRL_GTZC_L_CH_MPCBB1  (FLOW_CTRL_STAGE_3_L ^       FLOW_STEP_GTZC_L_CH_MPCBB1)
#define FLOW_CTRL_GTZC_L_CH_TZSC    (FLOW_CTRL_GTZC_L_CH_MPCBB1 ^ FLOW_STEP_GTZC_L_CH_TZSC)

#define FLOW_CTRL_GPIO_L_CH         (FLOW_CTRL_GTZC_L_CH_TZSC ^  FLOW_STEP_GPIO_L_CH)

#define FLOW_CTRL_MPU_L_CH_R7       (FLOW_CTRL_GPIO_L_CH ^       FLOW_STEP_MPU_L_CH_R7)
#define FLOW_CTRL_MPU_L_LCK         (FLOW_CTRL_MPU_L_CH_R7 ^     FLOW_STEP_MPU_L_LCK)
#define FLOW_CTRL_MPU_L_LCK_CH      (FLOW_CTRL_MPU_L_LCK ^       FLOW_STEP_MPU_L_LCK_CH)

#define FLOW_CTRL_SAU_L_CH_R0       (FLOW_CTRL_MPU_L_LCK_CH ^    FLOW_STEP_SAU_L_CH_R0)
#define FLOW_CTRL_SAU_L_CH_R1       (FLOW_CTRL_SAU_L_CH_R0 ^     FLOW_STEP_SAU_L_CH_R1)
#define FLOW_CTRL_SAU_L_CH_R2       (FLOW_CTRL_SAU_L_CH_R1 ^     FLOW_STEP_SAU_L_CH_R2)
#define FLOW_CTRL_SAU_L_CH_R3       (FLOW_CTRL_SAU_L_CH_R2 ^     FLOW_STEP_SAU_L_CH_R3)
#define FLOW_CTRL_SAU_L_CH_R4       (FLOW_CTRL_SAU_L_CH_R3 ^     FLOW_STEP_SAU_L_CH_R4)
#define FLOW_CTRL_SAU_L_CH_R5       (FLOW_CTRL_SAU_L_CH_R4 ^     FLOW_STEP_SAU_L_CH_R5)
#define FLOW_CTRL_SAU_L_CH          (FLOW_CTRL_SAU_L_CH_R5 ^     FLOW_STEP_SAU_L_CH)
#define FLOW_CTRL_SAU_L_LCK         (FLOW_CTRL_SAU_L_CH ^        FLOW_STEP_SAU_L_LCK)
#define FLOW_CTRL_SAU_L_LCK_CH      (FLOW_CTRL_SAU_L_LCK ^       FLOW_STEP_SAU_L_LCK_CH)

#define FLOW_CTRL_NVIC_L_CH         (FLOW_CTRL_SAU_L_LCK_CH ^    FLOW_STEP_NVIC_L_CH)

#define FLOW_CTRL_HDPEXT_L_CH_B1    (FLOW_CTRL_NVIC_L_CH ^       FLOW_STEP_HDPEXT_L_CH_B1)
#define FLOW_CTRL_HDPEXT_L_CH_B2    (FLOW_CTRL_HDPEXT_L_CH_B1 ^  FLOW_STEP_HDPEXT_L_CH_B2)

#define FLOW_CTRL_STAGE_4_L          FLOW_CTRL_HDPEXT_L_CH_B2


/* External variables --------------------------------------------------------*/
/**
  *  Flow control protection values
  *  Flow control stage
  */
extern volatile uint32_t uFlowProtectValue;
extern volatile uint32_t uFlowStage;

/* Exported macros -----------------------------------------------------------*/
/** Control with STEP operation :
  * (uFlowValue XOR STEP_VALUE) should be equal to CTRL_VALUE ==> execution stopped if failed !
  */
#if defined(FLOW_CONTROL)
#define FLOW_CONTROL_STEP(C,B,A) \
  do{ \
    (C) ^= (B);\
    if ((C) != (A))\
    { \
      Error_Handler();\
    } \
  }while(0)
#else
#define FLOW_CONTROL_STEP(C,B,A) ((void)0)
#endif

/** Control without STEP operation :
  * uFlowValue should be equal to CTRL_VALUE ==> execution stopped if failed !
  */
#if defined(FLOW_CONTROL)
#define FLOW_CONTROL_CHECK(B,A) \
  do{ \
    if ((B) != (A))\
    { \
      Error_Handler();\
    } \
  }while(0)
#else
#define FLOW_CONTROL_CHECK(B,A) ((void)0)
#endif

/** Control flow initialization
  */
#if defined(FLOW_CONTROL)
#define FLOW_CONTROL_INIT(B,A) \
  do{ \
    (B) = (A);\
  }while(0)
#else
#define FLOW_CONTROL_INIT(B,A) ((void)0)
#endif

/** STEP update only :
  * (uFlowValue XOR STEP_VALUE)
  */
#if defined(FLOW_CONTROL)
#define FLOW_STEP(B,A) \
  do{ \
    (B) ^= (A);\
  }while(0)
#else
#define FLOW_STEP(B,A) ((void)0)
#endif

/* Exported functions ------------------------------------------------------- */

/** @defgroup BOOT_FLOWCONTROL_Exported_Functions Exported Functions
  * @{
  */


/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* BOOT_HAL_FLOWCONTROL_H */
