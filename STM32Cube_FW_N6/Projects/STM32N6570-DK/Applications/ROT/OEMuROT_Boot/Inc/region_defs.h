/*
 * Copyright (c) 2017-2018 ARM Limited
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

#ifndef __REGION_DEFS_H__
#define __REGION_DEFS_H__

#include "flash_layout.h"

#define BL2_HEAP_SIZE           0x0002000
#define BL2_MSP_STACK_SIZE      0x0001000

#define S_HEAP_SIZE             0x0001000
#define S_MSP_STACK_SIZE_INIT   0x0000400
#define S_MSP_STACK_SIZE        0x0000800
#define S_PSP_STACK_SIZE        0x0000800

#define NS_HEAP_SIZE            0x0001000
#define NS_MSP_STACK_SIZE       0x0000C00
#define NS_PSP_STACK_SIZE       0x0000C00

#define _SRAM2_AXI_BASE_S      (0x34100000)   /* Base address of 1 MB system RAM 2 accessible over AXI */
#define _SRAM2_AXI_SIZE        (0x100000)     /* SRAM2_AXI = 1024 Kbytes */

#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
#define IMAGE_PRIMARY_RUN_PARTITION_BASE    (EXT_RAM_BASE_ADDRESS)
#elif (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM)
#define IMAGE_PRIMARY_RUN_PARTITION_BASE    (SRAM1_AXI_BASE_S)
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

/*  This area in SRAM 2 is updated BL2 and can be lock to avoid any changes */
#define BOOT_SHARED_DATA_SIZE        0
#define BOOT_SHARED_DATA_BASE        0

/*
 * Boot partition structure if MCUBoot is used:
 * 0x0_0000 Bootloader header
 * 0x0_0400 Image area
 * 0xz_zzzz Trailer
 */
/* IMAGE_CODE_SIZE is the space available for the software binary image.
 * It is less than the FLASH_PARTITION_SIZE because we reserve space
 * for the image header and trailer introduced by the bootloader.
 */

#define BL2_HEADER_SIZE                     (0x400) /*!< Appli image header size */
#define BL2_DATA_HEADER_SIZE                (0x20)  /*!< Data image header size */
#define BL2_TRAILER_SIZE                    (0x2000)

#define S_IMAGE_PRIMARY_PARTITION_OFFSET    (FLASH_AREA_0_OFFSET)
#define NS_IMAGE_PRIMARY_PARTITION_OFFSET   (FLASH_AREA_1_OFFSET)
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define S_DATA_IMAGE_PRIMARY_PARTITION_OFFSET    (FLASH_AREA_4_OFFSET)
#define S_DATA_IMAGE_SECONDARY_PARTITION_OFFSET  (FLASH_AREA_6_OFFSET)
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define NS_DATA_IMAGE_PRIMARY_PARTITION_OFFSET   (FLASH_AREA_5_OFFSET)
#define NS_DATA_IMAGE_SECONDARY_PARTITION_OFFSET (FLASH_AREA_7_OFFSET)
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */

/* Primary run partition for Load & Run configuration */
#if (OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
#define IMAGE_PRIMARY_RUN_PARTITION_OFFSET  (0x00000000)
#define PRIMARY_RUN_PARTITION_START         (IMAGE_PRIMARY_RUN_PARTITION_BASE + IMAGE_PRIMARY_RUN_PARTITION_OFFSET)
#endif /* OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */

#define IMAGE_S_CODE_SIZE       (FLASH_S_PARTITION_SIZE - BL2_HEADER_SIZE - BL2_TRAILER_SIZE)
#define IMAGE_NS_CODE_SIZE      (FLASH_NS_PARTITION_SIZE - BL2_HEADER_SIZE - BL2_TRAILER_SIZE)

/* External Flash base */
#define ROM_ALIAS(x)                        (EXT_FLASH_BASE_ADDRESS + (x))

#define S_RAM_ALIAS_BASE                    (_SRAM2_AXI_BASE_S)
#define NS_RAM_ALIAS_BASE                   (SRAM2_AXI_BASE_NS)

/* Alias definitions for secure and non-secure areas*/
#define S_ROM_ALIAS(x)                      (EXT_FLASH_BASE_ADDRESS + (x))
#define NS_ROM_ALIAS(x)                     (EXT_FLASH_BASE_ADDRESS + (x))

#define S_RAM_ALIAS(x)                      (S_RAM_ALIAS_BASE + (x))
#define NS_RAM_ALIAS(x)                     (NS_RAM_ALIAS_BASE + (x))

#define PRIMARY_AREA_START                  (ROM_ALIAS(FLASH_AREA_0_OFFSET))
#define PRIMARY_AREA_LIMIT                  (ROM_ALIAS(FLASH_AREA_5_OFFSET + FLASH_AREA_5_SIZE) - 1)
#define SECONDARY_PARTITION_START           (ROM_ALIAS(FLASH_AREA_2_OFFSET))
#define SECONDARY_PARTITION_LIMIT           (ROM_ALIAS(FLASH_AREA_7_OFFSET + FLASH_AREA_7_SIZE) - 1)

/* BL2 regions */
#define BL2_CODE_OFFSET                     (0x80400)
#define BL2_CODE_START                      (S_RAM_ALIAS(BL2_CODE_OFFSET))
#define BL2_CODE_SIZE                       (0x40000 - 0x400)
#define BL2_CODE_LIMIT                      (BL2_CODE_START + BL2_CODE_SIZE - 1)

#define BL2_DATA_OFFSET                     (0xC0000)
#define BL2_DATA_START                      (S_RAM_ALIAS(BL2_DATA_OFFSET))
#define BL2_DATA_SIZE                       (80000)
#define BL2_DATA_LIMIT                      (BL2_DATA_START + BL2_DATA_SIZE - 1)

/* Secure regions */
#define S_IMAGE_PRIMARY_AREA_OFFSET         (S_IMAGE_PRIMARY_PARTITION_OFFSET + BL2_HEADER_SIZE)
#define S_CODE_START                        (S_ROM_ALIAS(S_IMAGE_PRIMARY_AREA_OFFSET))
#define S_CODE_SIZE                         (IMAGE_S_CODE_SIZE)
#define S_CODE_LIMIT                        (S_CODE_START + S_CODE_SIZE - 1)

#define S_DATA_START                        (_SRAM2_AXI_BASE_S)
#define S_DATA_SIZE                         (0x80000)
#define S_DATA_LIMIT                        (S_DATA_START + S_DATA_SIZE - 1)

/* Non-secure regions */
#define NS_IMAGE_PRIMARY_AREA_OFFSET        (NS_IMAGE_PRIMARY_PARTITION_OFFSET + BL2_HEADER_SIZE)
#define NS_CODE_START                       (NS_ROM_ALIAS(NS_IMAGE_PRIMARY_AREA_OFFSET))
#define NS_CODE_SIZE                        (IMAGE_NS_CODE_SIZE)
#define NS_CODE_LIMIT                       (NS_CODE_START + NS_CODE_SIZE - 1)

#define NS_DATA_START                       (0x24060000)
#define NS_DATA_SIZE                        (0xA0000)
#define NS_DATA_LIMIT                       (NS_DATA_START + NS_DATA_SIZE - 1)

/* Additional Check to detect flash download slot overlap or overflow */
#define FLASH_AREA_END_OFFSET_MAX (EXT_FLASH_TOTAL_SIZE)
#if FLASH_AREA_END_OFFSET > FLASH_AREA_END_OFFSET_MAX
#error "Flash memory overflow"
#endif /* FLASH_AREA_END_OFFSET > FLASH_AREA_END_OFFSET_MAX */

#endif /* __REGION_DEFS_H__ */
