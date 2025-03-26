/**
  ******************************************************************************
  * @file    venc_app.h
  * @author  MCD Application Team
  * @brief   Header for venc_app.c module
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
#ifndef VENC_APP_H
#define VENC_APP_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stm32n6xx_hal.h"
#include "tx_api.h"
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

#define FRAME_RECEIVED_FLAG (0b1 << 0)
#define VIDEO_START_FLAG (0b1 << 1)
/* Exported variables --------------------------------------------------------*/
extern TX_EVENT_FLAGS_GROUP USB_video_device_flags;
extern TX_QUEUE enc_frame_queue;
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void venc_thread_func(ULONG arg);
void VENC_APP_EncodingStart(void);
UINT VENC_APP_GetData(UCHAR **data, ULONG *size);
UINT VENC_APP_EncodingStop(void);
#endif /* MAIN_H */
