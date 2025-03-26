/*
 * Copyright (c) 2018 Arm Limited. All rights reserved.
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

#ifndef __FLASH_LAYOUT_H__
#define __FLASH_LAYOUT_H__

/* This header file is included from linker scatter file as well, where only a
 * limited C constructs are allowed. Therefore it is not possible to include
 * here the platform_retarget.h to access flash related defines. To resolve this
 * some of the values are redefined here with different names, these are marked
 * with comment.
 */

/* Flash layout configuration : begin ****************************************/

#define NO_LOAD_AND_RUN      (0)                          /* OEMuROT_Boot uses "Execute In Place" */
#define LOAD_AND_RUN_INT_RAM (1)                          /* OEMuROT_Boot uses "Load & Run" with internal RAM memory */
#define LOAD_AND_RUN_EXT_RAM (2)                          /* OEMuROT_Boot uses "Load & Run" with external RAM memory */
#define OEMIROT_LOAD_AND_RUN NO_LOAD_AND_RUN              /* Flag configuring the feature "Load and Run" */

#define MCUBOOT_OVERWRITE_ONLY     /* Defined: the FW installation uses overwrite method.
                                      Undefined: The FW installation uses swap mode. */

#define MCUBOOT_USE_MCE            /* Defined: The external flash can be protected by MCE.
                                      Undefined: The MCE isn't supported. */

#define MCUBOOT_S_DATA_IMAGE_NUMBER 1   /* 1: S data image for S application.
                                           0: No S data image. */

#define MCUBOOT_NS_DATA_IMAGE_NUMBER 1  /* 1: NS data image for NS application.
                                           0: No NS data image. */


/* Flash layout configuration : end ******************************************/

/* Use image hash reference to reduce boot time (signature check bypass) */
//#define MCUBOOT_USE_HASH_REF

/* Total number of images */
#define MCUBOOT_APP_IMAGE_NUMBER 2 /* Two separated images for S and NS application binaries */
#define MCUBOOT_IMAGE_NUMBER (MCUBOOT_APP_IMAGE_NUMBER + MCUBOOT_S_DATA_IMAGE_NUMBER + MCUBOOT_NS_DATA_IMAGE_NUMBER)

/* Flash layout info for BL2 bootloader */
#define EXT_FLASH_SECTOR_SIZE           (0x10000)             /* External Flash: 64 KB */
#define EXT_FLASH_TOTAL_SIZE            (0x08000000)          /* External Flash: 128 MBytes */
#define EXT_FLASH_BASE_ADDRESS          (0x70000000)          /* External Flash (XSPI1 + MCE1 - AES) */
#define EXT_RAM_BASE_ADDRESS            (0x90000000)          /* External Flash (XSPI2 + MCE2 - Noekeon) */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    EXT_FLASH_SECTOR_SIZE /* External Flash: 4 KB */
#define FLASH_BASE_ADDRESS              EXT_FLASH_BASE_ADDRESS

#if (OEMIROT_LOAD_AND_RUN == NO_LOAD_AND_RUN)
#define BASE_ADDRESS_APP_S    (EXT_FLASH_BASE_ADDRESS)
#define BASE_ADDRESS_APP_NS   (EXT_FLASH_BASE_ADDRESS)
#elif (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
#define BASE_ADDRESS_APP_S    (EXT_RAM_BASE_ADDRESS)
#define BASE_ADDRESS_APP_NS   (EXT_RAM_BASE_ADDRESS)
#elif (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM)
#define BASE_ADDRESS_APP_S    (SRAM1_AXI_BASE_S)
#define BASE_ADDRESS_APP_NS   (SRAM1_AXI_BASE_NS)
#endif /* OEMIROT_LOAD_AND_RUN == NO_LOAD_AND_RUN */

/* XSPI instances used by the external memories:
 *  - XSPI1 + MCE1 (AES) must be used with the external flash.
 *  - XSPI2 + MCE2 (Noekeon) must be used with the external RAM.
 *
 * On STM32N6S570-DK,
 *  - the external flash is available via XSPI-PHY2 (Port2).
 *  - the external RAM is available via XSPI-PHY1 (Port1).
 * The XSPI I/O Manager (XSPIM) must be used in swapped mode.
 */
#define EXT_RAM_XSPI_INSTANCE           XSPI1
#define EXT_FLASH_XSPI_INSTANCE         XSPI2
#define MCE_AES_CONTEXT                 MCE1_CONTEXT1
#define MCE_AES_CONTEXT_ID              MCE_CONTEXT1


/* Flash device ID */
#define EXT_FLASH_DEVICE_ID             (FLASH_DEVICE_ID + 1) /* External Flash */

/* Flash area IDs */
#define FLASH_AREA_0_ID                 (1)
#define FLASH_AREA_1_ID                 (2)
#define FLASH_AREA_2_ID                 (3)
#define FLASH_AREA_3_ID                 (4)
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define FLASH_AREA_4_ID                 (5)
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLASH_AREA_5_ID                 (6)
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define FLASH_AREA_6_ID                 (7)
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLASH_AREA_7_ID                 (8)
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
#define FLASH_AREA_SCRATCH_ID           (9)

/* Offset and size definitions of the flash partitions that are handled by the
 * bootloader. The image swapping is done between IMAGE_0 and IMAGE_1, SCRATCH
 * is used as a temporary storage during image swapping.
 */

/* area for BL2 code protected by hdp */
#define FLASH_AREA_BL2_OFFSET           (0x0)
#define FLASH_AREA_BL2_SIZE             (0x20000)

#define FLASH_AREA_BL2_ADDRESS          (SRAM2_AXI_BASE_S + FLASH_AREA_BL2_OFFSET)

/* scratch area */
#if defined(FLASH_AREA_SCRATCH_ID)
#define FLASH_AREA_SCRATCH_DEVICE_ID    (EXT_FLASH_DEVICE_ID - FLASH_DEVICE_ID)
#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM)
#define FLASH_AREA_SCRATCH_OFFSET       (0x00000000)
#else /* not (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM) */
#define FLASH_AREA_SCRATCH_OFFSET       (EXT_FLASH_TOTAL_SIZE - FLASH_HASH_REF_AREA_SIZE - \
                                         FLASH_AREA_SCRATCH_SIZE)
#define SCRATCH_AREA_START              (FLASH_BASE_ADDRESS + FLASH_AREA_SCRATCH_OFFSET)
#define SCRATCH_AREA_LIMIT              (SCRATCH_AREA_START + FLASH_AREA_SCRATCH_SIZE - 1)
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM */
#if defined(MCUBOOT_OVERWRITE_ONLY)
#define FLASH_AREA_SCRATCH_SIZE         (0x0000) /* Not used in MCUBOOT_OVERWRITE_ONLY mode */
#else /* not MCUBOOT_OVERWRITE_ONLY*/
#define FLASH_AREA_SCRATCH_SIZE         (0x20000) /* 128 KB */
#endif /* MCUBOOT_OVERWRITE_ONLY */
/* control scratch area */
#if (FLASH_AREA_SCRATCH_OFFSET % EXT_FLASH_SECTOR_SIZE) != 0
#error "FLASH_AREA_SCRATCH_OFFSET not aligned on EXT_FLASH_SECTOR_SIZE"
#endif /* (FLASH_AREA_SCRATCH_OFFSET % EXT_FLASH_SECTOR_SIZE) != 0*/
#else /* FLASH_AREA_SCRATCH_ID */
#define FLASH_AREA_SCRATCH_SIZE         (0x0)
#endif /* FLASH_AREA_SCRATCH_ID */

/* area for image HASH references */
#if defined(MCUBOOT_USE_HASH_REF)
#define FLASH_HASH_REF_AREA_OFFSET      (EXT_FLASH_TOTAL_SIZE - FLASH_HASH_REF_AREA_SIZE)
#define FLASH_HASH_REF_AREA_SIZE        (FLASH_AREA_IMAGE_SECTOR_SIZE)
#define HASH_REF_AREA_START             (FLASH_BASE_ADDRESS + FLASH_HASH_REF_AREA_OFFSET)
#define HASH_REF_AREA_LIMIT             (HASH_REF_AREA_START + FLASH_HASH_REF_AREA_SIZE - 1)
#else
#define FLASH_HASH_REF_AREA_SIZE        (0x0000)
#endif /* MCUBOOT_USE_HASH_REF */

/* BL2 partitions size */
#define FLASH_S_PARTITION_SIZE          (0x20000) /* 128 KB for S partition */
#define FLASH_NS_PARTITION_SIZE         (0x20000) /* 128 KB for NS partition */
#define FLASH_PARTITION_SIZE            (FLASH_S_PARTITION_SIZE + FLASH_NS_PARTITION_SIZE)

#define FLASH_MAX_APP_PARTITION_SIZE    ((FLASH_S_PARTITION_SIZE >   \
                                         FLASH_NS_PARTITION_SIZE) ? \
                                         FLASH_S_PARTITION_SIZE : \
                                         FLASH_NS_PARTITION_SIZE)
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define FLASH_S_DATA_PARTITION_SIZE     (FLASH_AREA_IMAGE_SECTOR_SIZE)
#else
#define FLASH_S_DATA_PARTITION_SIZE     (0x0)
#endif /* (MCUBOOT_S_DATA_IMAGE_NUMBER == 1) */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLASH_NS_DATA_PARTITION_SIZE    (FLASH_AREA_IMAGE_SECTOR_SIZE)
#else
#define FLASH_NS_DATA_PARTITION_SIZE    (0x0)
#endif /* (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1) */

#define FLASH_MAX_DATA_PARTITION_SIZE   ((FLASH_S_DATA_PARTITION_SIZE >   \
                                         FLASH_NS_DATA_PARTITION_SIZE) ? \
                                         FLASH_S_DATA_PARTITION_SIZE : \
                                         FLASH_NS_DATA_PARTITION_SIZE)
#define FLASH_MAX_PARTITION_SIZE        ((FLASH_MAX_APP_PARTITION_SIZE >   \
                                         FLASH_MAX_DATA_PARTITION_SIZE) ? \
                                         FLASH_MAX_APP_PARTITION_SIZE : \
                                         FLASH_MAX_DATA_PARTITION_SIZE)

/* BL2 flash areas */
#define FLASH_AREA_BEGIN_OFFSET         (FLASH_AREA_BL2_OFFSET + FLASH_AREA_BL2_SIZE)
#define FLASH_AREA_BEGIN_ADDRESS        (FLASH_BASE_ADDRESS + FLASH_AREA_BEGIN_OFFSET)
#define FLASH_AREAS_DEVICE_ID           (FLASH_DEVICE_ID - FLASH_DEVICE_ID)

/* Secure app image primary slot */
#if defined(FLASH_AREA_0_ID)
#define FLASH_AREA_0_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_0_OFFSET             (FLASH_AREA_BEGIN_OFFSET)
#define FLASH_AREA_0_SIZE               (FLASH_S_PARTITION_SIZE)
/* Control Secure app image primary slot */
#if (FLASH_AREA_0_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_0_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_0_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#else /* FLASH_AREA_0_ID */
#define FLASH_AREA_0_OFFSET             (0x0)
#define FLASH_AREA_0_SIZE               (0x0)
#endif /* FLASH_AREA_0_ID */

/* Secure data image primary slot */
#if defined (FLASH_AREA_4_ID)
#define FLASH_AREA_4_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_4_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_0_SIZE)
#define FLASH_AREA_4_SIZE               (FLASH_S_DATA_PARTITION_SIZE)
/* Control Secure data image primary slot */
#if (FLASH_AREA_4_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_4_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_AREA_4_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#else /* FLASH_AREA_4_ID */
#define FLASH_AREA_4_OFFSET             (0x0)
#define FLASH_AREA_4_SIZE               (0x0)
#endif /* FLASH_AREA_4_ID */

/* Non-secure app image primary slot */
#if defined(FLASH_AREA_1_ID)
#define FLASH_AREA_1_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_1_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_0_SIZE + \
                                         FLASH_AREA_4_SIZE)
#define FLASH_AREA_1_SIZE               (FLASH_NS_PARTITION_SIZE)
/* Control Non-secure app image primary slot */
#if (FLASH_AREA_1_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_1_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_AREA_1_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0  */
#else /* FLASH_AREA_1_ID */
#define FLASH_AREA_1_OFFSET             (0x0)
#define FLASH_AREA_1_SIZE               (0x0)
#endif /* FLASH_AREA_1_ID */

/* Non-secure data image primary slot */
#if defined(FLASH_AREA_5_ID)
#define FLASH_AREA_5_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_5_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_0_SIZE + \
                                         FLASH_AREA_4_SIZE + FLASH_AREA_1_SIZE)
#define FLASH_AREA_5_SIZE               (FLASH_NS_DATA_PARTITION_SIZE)
/* Control Non-secure data image primary slot */
#if (FLASH_AREA_5_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_5_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_AREA_5_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0  */
#else /* FLASH_AREA_5_ID */
#define FLASH_AREA_5_OFFSET             (0x0)
#define FLASH_AREA_5_SIZE               (0x0)
#endif /* FLASH_AREA_5_ID */

/* Secure app image secondary slot */
#if defined(FLASH_AREA_2_ID)
#define FLASH_AREA_2_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_2_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_0_SIZE + \
                                         FLASH_AREA_4_SIZE + FLASH_AREA_1_SIZE + \
                                         FLASH_AREA_5_SIZE)
#define FLASH_AREA_2_SIZE               (FLASH_S_PARTITION_SIZE)
/* Control Secure app image secondary slot */
#if (FLASH_AREA_2_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_2_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*   (FLASH_AREA_2_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#else /* FLASH_AREA_2_ID */
#define FLASH_AREA_2_OFFSET             (0x0)
#define FLASH_AREA_2_SIZE               (0x0)
#endif /* FLASH_AREA_2_ID */

/* Secure data image secondary slot */
#if defined(FLASH_AREA_6_ID)
#define FLASH_AREA_6_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_6_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_0_SIZE + \
                                         FLASH_AREA_4_SIZE + FLASH_AREA_1_SIZE + \
                                         FLASH_AREA_5_SIZE + FLASH_AREA_2_SIZE)
#define FLASH_AREA_6_SIZE               (FLASH_S_DATA_PARTITION_SIZE)
/* Control Secure data image secondary slot */
#if (FLASH_AREA_6_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_6_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_6_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#else /* FLASH_AREA_6_ID */
#define FLASH_AREA_6_OFFSET             (0x0)
#define FLASH_AREA_6_SIZE               (0x0)
#endif /* FLASH_AREA_6_ID */

/* Non-secure app image secondary slot */
#if defined(FLASH_AREA_3_ID)
#define FLASH_AREA_3_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_3_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_0_SIZE + \
                                         FLASH_AREA_4_SIZE + FLASH_AREA_1_SIZE + \
                                         FLASH_AREA_5_SIZE + FLASH_AREA_2_SIZE + \
                                         FLASH_AREA_6_SIZE)
#define FLASH_AREA_3_SIZE               (FLASH_NS_PARTITION_SIZE)
/* Control Non-Secure app image secondary slot */
#if (FLASH_AREA_3_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_3_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_3_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#else /* FLASH_AREA_3_ID */
#define FLASH_AREA_3_OFFSET             (0x0)
#define FLASH_AREA_3_SIZE               (0x0)
#endif /* FLASH_AREA_3_ID */

/* Non-Secure data image secondary slot */
#if defined(FLASH_AREA_7_ID)
#define FLASH_AREA_7_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_7_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_0_SIZE + \
                                         FLASH_AREA_4_SIZE + FLASH_AREA_1_SIZE + \
                                         FLASH_AREA_5_SIZE + FLASH_AREA_2_SIZE + \
                                         FLASH_AREA_6_SIZE + FLASH_AREA_3_SIZE)
#define FLASH_AREA_7_SIZE               (FLASH_NS_DATA_PARTITION_SIZE)
/* Control Non-Secure data image secondary slot */
#if (FLASH_AREA_7_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_7_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_7_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#else /* FLASH_AREA_7_ID */
#define FLASH_AREA_7_OFFSET             (0x0)
#define FLASH_AREA_7_SIZE               (0x0)
#endif /* FLASH_AREA_7_ID */

/* flash areas end offset */
#define FLASH_AREA_END_OFFSET           (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_4_SIZE + \
                                         FLASH_AREA_0_SIZE + FLASH_AREA_1_SIZE + \
                                         FLASH_AREA_5_SIZE + FLASH_AREA_2_SIZE + \
                                         FLASH_AREA_3_SIZE + FLASH_AREA_6_SIZE + \
                                         FLASH_AREA_7_SIZE)
#define FLASH_AREA_END_ADDRESS          (FLASH_BASE_ADDRESS + FLASH_AREA_END_OFFSET)
/* Control flash area end */
#if (FLASH_AREA_END_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_END_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_END_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

/*
 * The maximum number of status entries supported by the bootloader.
 */
#define MCUBOOT_STATUS_MAX_ENTRIES         ((FLASH_MAX_PARTITION_SIZE) / \
                                            FLASH_AREA_SCRATCH_SIZE)
/* Maximum number of image sectors supported by the bootloader. */
#define MCUBOOT_MAX_IMG_SECTORS           ((FLASH_MAX_PARTITION_SIZE) / \
                                           FLASH_AREA_IMAGE_SECTOR_SIZE)


/* Flash device name used by BL2
 * Name is defined in flash driver file: low_level_flash.c
 */
#define FLASH_DEV_NAME Driver_EXT_FLASH0
#define EXT_FLASH_DEV_NAME Driver_EXT_FLASH0


/* Flash device names and IDs for BL2 flash areas */
#if defined(FLASH_AREA_0_ID)
#define FLASH_DEV_NAME_0  EXT_FLASH_DEV_NAME
#define FLASH_DEVICE_ID_0 EXT_FLASH_DEVICE_ID
#endif /* FLASH_AREA_0_ID */

#if defined(FLASH_AREA_2_ID)
#define FLASH_DEV_NAME_2  EXT_FLASH_DEV_NAME
#define FLASH_DEVICE_ID_2 EXT_FLASH_DEVICE_ID
#endif /* FLASH_AREA_2_ID */

#if defined(FLASH_AREA_SCRATCH_ID)
#define FLASH_DEV_NAME_SCRATCH  EXT_FLASH_DEV_NAME
#define FLASH_DEVICE_ID_SCRATCH EXT_FLASH_DEVICE_ID
#endif /* FLASH_AREA_SCRATCH_ID */

#endif /* __FLASH_LAYOUT_H__ */
