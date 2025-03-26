/**
  ******************************************************************************
  * @file    Examples/BSP/Inc/main.h
  * @author  MDG Application Team
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"
#include "stm32n6xx_hal.h"
#include "stm32n6570_discovery.h"
#include "stm32n6570_discovery_lcd.h"
#include "stm32_lcd.h"

/* Exported variables --------------------------------------------------------*/
extern unsigned char stlogo[];
extern __IO uint32_t UserButtonPressed ;

typedef struct
{
  void     (*DemoFunc)(void);
  uint8_t  DemoName[50];
  uint32_t DemoIndex;
} BSP_DemoTypedef;

void Lcd_Demo(void);

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
#define COUNT_OF_EXAMPLE(x)    (sizeof(x)/sizeof(BSP_DemoTypedef))

/* Exported functions ------------------------------------------------------- */

void  Error_Handler(void);
uint8_t CheckForUserInput(void);
#endif /* MAIN_H */
