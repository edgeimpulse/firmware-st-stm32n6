/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stm32n6xx_hal.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  int32_t  (*DemoFunc)(void);
  uint8_t  DemoName[50];
} BSP_DemoTypedef;

/* Exported constants --------------------------------------------------------*/
#define COUNT_OF_EXAMPLE(x)    (sizeof(x)/sizeof(BSP_DemoTypedef))

/* Exported variables --------------------------------------------------------*/
extern __IO uint32_t UserButtonPressed;
extern __IO uint32_t ButtonState;

/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void hexdump(uint8_t * buf, size_t len, char * msg);
int32_t  Led_demo(void);
int32_t  Button_demo(void);
int32_t  Com_demo(void);
void     Error_Handler(void);
uint32_t CheckResult(void);
void     StartTest(void);
uint8_t CheckForUserInput(void);

#endif /* MAIN_H */
