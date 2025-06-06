/**
  ******************************************************************************
  * @file    imx335.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the imx335.c
  *          driver.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IMX335_H
#define IMX335_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "imx335_reg.h"
#include <stddef.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup imx335
  * @{
  */

/** @defgroup IMX335_Exported_Types
  * @{
  */

typedef int32_t (*IMX335_Init_Func)    (void);
typedef int32_t (*IMX335_DeInit_Func)  (void);
typedef int32_t (*IMX335_GetTick_Func) (void);
typedef int32_t (*IMX335_Delay_Func)   (uint32_t);
typedef int32_t (*IMX335_WriteReg_Func)(uint16_t, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*IMX335_ReadReg_Func) (uint16_t, uint16_t, uint8_t*, uint16_t);

typedef struct
{
  IMX335_Init_Func          Init;
  IMX335_DeInit_Func        DeInit;
  uint16_t                  Address;
  IMX335_WriteReg_Func      WriteReg;
  IMX335_ReadReg_Func       ReadReg;
  IMX335_GetTick_Func       GetTick;
} IMX335_IO_t;

typedef struct
{
  IMX335_IO_t         IO;
  imx335_ctx_t        Ctx;
  uint8_t             IsInitialized;
} IMX335_Object_t;

typedef struct
{
  uint32_t Config_Resolution;
  uint32_t Config_LightMode;
  uint32_t Config_SpecialEffect;
  uint32_t Config_Brightness;
  uint32_t Config_Saturation;
  uint32_t Config_Contrast;
  uint32_t Config_HueDegree;
  uint32_t Config_Gain;
  uint32_t Config_Exposure;
  uint32_t Config_MirrorFlip;
  uint32_t Config_Zoom;
  uint32_t Config_NightMode;
  uint32_t Config_ExposureMode;
  uint32_t Config_SensorInfo;
  uint32_t Config_TestPattern;
  uint32_t Config_ColorEffect;
} IMX335_Capabilities_t;

#define IMX335_SENSOR_INFO_MAX_LENGTH      (32U)

typedef struct
{
  char name[IMX335_SENSOR_INFO_MAX_LENGTH];
  uint8_t bayer_pattern;
  uint8_t color_depth;
  uint32_t width;
  uint32_t height;
  uint32_t gain_min;
  uint32_t gain_max;
  uint32_t exposure_min;
  uint32_t exposure_max;
} IMX335_SensorInfo_t;

typedef struct
{
  int32_t  (*Init              )(IMX335_Object_t*, uint32_t, uint32_t);
  int32_t  (*DeInit            )(IMX335_Object_t*);
  int32_t  (*ReadID            )(IMX335_Object_t*, uint32_t*);
  int32_t  (*GetCapabilities   )(IMX335_Object_t*, IMX335_Capabilities_t*);
  int32_t  (*SetLightMode      )(IMX335_Object_t*, uint32_t);
  int32_t  (*SetColorEffect    )(IMX335_Object_t*, uint32_t);
  int32_t  (*SetBrightness     )(IMX335_Object_t*, int32_t);
  int32_t  (*SetSaturation     )(IMX335_Object_t*, int32_t);
  int32_t  (*SetContrast       )(IMX335_Object_t*, int32_t);
  int32_t  (*SetHueDegree      )(IMX335_Object_t*, int32_t);
  int32_t  (*MirrorFlipConfig  )(IMX335_Object_t*, uint32_t);
  int32_t  (*ZoomConfig        )(IMX335_Object_t*, uint32_t);
  int32_t  (*SetResolution     )(IMX335_Object_t*, uint32_t);
  int32_t  (*GetResolution     )(IMX335_Object_t*, uint32_t*);
  int32_t  (*SetPixelFormat    )(IMX335_Object_t*, uint32_t);
  int32_t  (*GetPixelFormat    )(IMX335_Object_t*, uint32_t*);
  int32_t  (*NightModeConfig   )(IMX335_Object_t*, uint32_t);
  int32_t  (*SetFrequency      )(IMX335_Object_t*, int32_t);
  int32_t  (*SetGain           )(IMX335_Object_t*, int32_t);
  int32_t  (*SetExposure       )(IMX335_Object_t*, int32_t);
  int32_t  (*SetExposureMode   )(IMX335_Object_t*, int32_t);
  int32_t  (*GetSensorInfo     )(IMX335_Object_t *, IMX335_SensorInfo_t *);
  int32_t  (*SetTestPattern    )(IMX335_Object_t*, int32_t);
}IMX335_CAMERA_Drv_t;
/**
  * @}
  */

/** @defgroup IMX335_Exported_Constants
  * @{
  */
#define IMX335_OK                      (0)
#define IMX335_ERROR                   (-1)
/**
 * @brief  IMX335 Features Parameters
 */
/* Camera resolutions */
#define IMX335_R2592_1944                6U	/* 2592x1944 Resolution       */

/* Camera Pixel Format */
#define IMX335_RAW_RGGB10               10U    /* Pixel Format RAW_RGGB10    */

/* Camera Frequency */
#define  IMX335_INCK_6MHZ     0U
#define  IMX335_INCK_18MHZ    1U
#define  IMX335_INCK_24MHZ    2U
#define  IMX335_INCK_27MHZ    3U
#define  IMX335_INCK_74MHZ    4U

/* Mirror/Flip */
#define IMX335_MIRROR_FLIP_NONE         0x00U   /* Set camera normal mode     */
#define IMX335_FLIP                     0x01U   /* Set camera flip config     */
#define IMX335_MIRROR                   0x02U   /* Set camera mirror config   */
#define IMX335_MIRROR_FLIP              0x03U   /* Set camera mirror and flip */

/**
  * @}
  */

/** @defgroup IMX335_Exported_Functions IMX335 Exported Functions
  * @{
  */
int32_t IMX335_RegisterBusIO(IMX335_Object_t *pObj, IMX335_IO_t *pIO);
int32_t IMX335_Init(IMX335_Object_t *pObj, uint32_t Resolution, uint32_t PixelFormat);
int32_t IMX335_DeInit(IMX335_Object_t *pObj);
int32_t IMX335_ReadID(IMX335_Object_t *pObj, uint32_t *Id);
int32_t IMX335_GetCapabilities(IMX335_Object_t *pObj, IMX335_Capabilities_t *Capabilities);
int32_t IMX335_SetGain(IMX335_Object_t *pObj, int32_t gain);
int32_t IMX335_SetExposure(IMX335_Object_t *pObj, int32_t exposure);
int32_t IMX335_SetFrequency(IMX335_Object_t *pObj, int32_t frequency);
int32_t IMX335_SetFramerate(IMX335_Object_t *pObj, int32_t framerate);
int32_t IMX335_MirrorFlipConfig(IMX335_Object_t *pObj, uint32_t Config);
int32_t IMX335_GetSensorInfo(IMX335_Object_t *pObj, IMX335_SensorInfo_t *Info);
int32_t IMX335_SetTestPattern(IMX335_Object_t *pObj, int32_t mode);

/* CAMERA driver structure */
extern IMX335_CAMERA_Drv_t   IMX335_CAMERA_Driver;
/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */