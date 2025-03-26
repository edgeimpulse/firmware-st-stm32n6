/**
  ******************************************************************************
  * @file    stm32n6xx_it.h
  * @author  MCD Application Team
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2024 STMicroelectronics</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6XX_IT_H
#define STM32N6XX_IT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void NMI_Handler(void);
void MemManage_Handler(void);
void SecureFault_Handler(void);
void BusFault_Handler(void);
void DebugMon_Handler(void);
void TIM6_IRQHandler(void);
void SDMMC2_IRQHandler(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */
#ifdef __cplusplus
}
#endif

#endif /* STM32N6XX_IT_H */
