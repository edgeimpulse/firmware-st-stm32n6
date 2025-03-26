/**
  ******************************************************************************
  * @file    Template_Isolation_LRUN/AppliNonSecure/Inc/main.h
  * @author  GPM Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2023 STMicroelectronics</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"
#include "stm32n6570_discovery.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
#if defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U)
  #if defined ( __ICCARM__ )
  #  define CMSE_NS_CALL  __cmse_nonsecure_call
  #  define CMSE_NS_ENTRY __cmse_nonsecure_entry
  #else
  #  define CMSE_NS_CALL  __attribute((cmse_nonsecure_call))
  #  define CMSE_NS_ENTRY __attribute((cmse_nonsecure_entry))
  #endif
#endif
/* Exported types ------------------------------------------------------------*/
#if defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U)
  /* Function pointer declaration in non-secure*/
  typedef void (CMSE_NS_CALL *funcptr)(void);

  /* typedef for non-secure callback functions */
  typedef funcptr funcptr_NS;
#endif

/* Exported functions ------------------------------------------------------- */

/******************************************************************************/
#ifdef __MAIN_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Exported variables ------------------------------------------------------- */

/******************************************************************************/
#endif /* MAIN_H */
