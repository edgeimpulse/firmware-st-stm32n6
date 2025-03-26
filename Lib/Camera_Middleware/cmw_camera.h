 /**
 ******************************************************************************
 * @file    cmw_camera.h
 * @author  GPM Application Team
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CMW_CAMERA_H
#define CMW_CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmw_errno.h"
#include "cmw_camera_conf.h"
#include "cmw_sensors_if.h"

typedef enum {
  CAM_Aspect_ratio_crop = 0x0,
  CAM_Aspect_ratio_fit,
  CAM_Aspect_ratio_fullscreen,
  CAM_Aspect_ratio_manual,
} CAM_Aspect_Ratio_Mode_t;

typedef struct {
  uint32_t width;
  uint32_t height;
  uint32_t offset_x;
  uint32_t offset_y;
} CMW_Manual_Crop_t;

/* Possible values are 1, 2, 4, 8 */
typedef struct {
  uint32_t horizontal_ratio;
  uint32_t vertical_ratio;
} CMW_Manual_Decimation_t;

/* Downsize engine output resolution */
typedef struct {
  uint32_t width;
  uint32_t height;
} CMW_Manual_Downsize_t;

typedef struct {
  CMW_Manual_Crop_t crop;
  CMW_Manual_Decimation_t decimation;
  CMW_Manual_Downsize_t downsize;
} CMW_Manual_Configuration_t;

typedef struct {
  /* Camera settings */
  uint32_t width;
  uint32_t height;
  int fps;
  uint32_t pixel_format;
  int anti_flicker;
  int mirror_flip;
} CMW_CameraInit_t;

typedef struct {
  /* pipe output settings */
  uint32_t output_width;
  uint32_t output_height;
  int output_format;
  int output_bpp;
  int enable_swap;
  int enable_gamma_conversion;
  /*Output buffer of the pipe*/
  int mode;
  /* You must fill manual_conf when mode is CAM_Aspect_ratio_manual */
  CMW_Manual_Configuration_t manual_conf;
} DCMIPP_Conf_t;

/* Camera capture mode */
#define CAMERA_MODE_CONTINUOUS          DCMIPP_MODE_CONTINUOUS
#define CAMERA_MODE_SNAPSHOT            DCMIPP_MODE_SNAPSHOT

/* Sensors parameters */
#define CAMERA_VD55G1_ADDRESS          0x20U
#define CAMERA_VD55G1_FREQ_IN_HZ       12000000U
#define CAMERA_IMX335_ADDRESS          0x34U
#define CAMERA_OV5640_ADDRESS          0x78U
#define CAMERA_VD66GY_ADDRESS          0x20U
#define CAMERA_VD66GY_FREQ_IN_HZ       12000000U
#define CAMERA_VD1941_ADDRESS          0x20U
#define CAMERA_VD5941_ADDRESS          0x20U

#if defined (STM32N657xx)

#if !defined (STM32N6570_DK_REV)
 #error "STM32N6570_DK_REV not defined"
#endif

#if (STM32N6570_DK_REV == STM32N6570_DK_C01) || (STM32N6570_DK_REV == STM32N6570_DK_B01)
  #define NRST_CAM_PIN                    GPIO_PIN_8
  #define NRST_CAM_PORT                   GPIOC
  #define NRST_CAM_GPIO_ENABLE_VDDIO()    HAL_PWREx_EnableVddIO4();
  #define NRST_CAM_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE();

  #define EN_CAM_PIN                      GPIO_PIN_2
  #define EN_CAM_PORT                     GPIOD
  /* PD2 on VDD Core */
  #define EN_CAM_GPIO_ENABLE_VDDIO()
  #define EN_CAM_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOD_CLK_ENABLE()
#elif defined (USE_STM32N6570_NUCLEO_REV_B01)
  #define NRST_CAM_PIN                    GPIO_PIN_5
  #define NRST_CAM_PORT                   GPIOO
  #define NRST_CAM_GPIO_ENABLE_VDDIO()    HAL_PWREx_EnableVddIO2();
  #define NRST_CAM_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOO_CLK_ENABLE()

  #define EN_CAM_PIN                      GPIO_PIN_0
  #define EN_CAM_PORT                     GPIOA
  /* PD10 on VDD Core */
  #define EN_CAM_GPIO_ENABLE_VDDIO()
  #define EN_CAM_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#else /* STM32N6570_DK_A01/STM32N6570_DK_A03 */
  #define NRST_CAM_PIN                    GPIO_PIN_5
  #define NRST_CAM_PORT                   GPIOO
  #define NRST_CAM_GPIO_ENABLE_VDDIO()    HAL_PWREx_EnableVddIO2();
  #define NRST_CAM_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOO_CLK_ENABLE()

  #define EN_CAM_PIN                      GPIO_PIN_10
  #define EN_CAM_PORT                     GPIOD
  /* PD10 on VDD Core */
  #define EN_CAM_GPIO_ENABLE_VDDIO()
  #define EN_CAM_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOD_CLK_ENABLE()
#endif

#if (STM32N6570_DK_REV == STM32N6570_DK_C01)
  #define CMW_I2C_INIT BSP_I2C1_Init
  #define CMW_I2C_DEINIT BSP_I2C1_DeInit
  #define CMW_I2C_READREG16 BSP_I2C1_ReadReg16
  #define CMW_I2C_WRITEREG16 BSP_I2C1_WriteReg16
#else /* STM32N6570_DK_B01/STM32N6570_DK_A01/STM32N6570_DK_A03 */
  #define CMW_I2C_INIT BSP_I2C2_Init
  #define CMW_I2C_DEINIT BSP_I2C2_DeInit
  #define CMW_I2C_READREG16 BSP_I2C2_ReadReg16
  #define CMW_I2C_WRITEREG16 BSP_I2C2_WriteReg16
#endif

#define CSI2_CLK_ENABLE()               __HAL_RCC_CSI_CLK_ENABLE()
#define CSI2_CLK_SLEEP_DISABLE()        __HAL_RCC_CSI_CLK_SLEEP_DISABLE()
#define CSI2_CLK_FORCE_RESET()          __HAL_RCC_CSI_FORCE_RESET()
#define CSI2_CLK_RELEASE_RESET()        __HAL_RCC_CSI_RELEASE_RESET()

#elif defined (STM32MP257Fxx)

#define NRST_CAM_PIN                    GPIO_PIN_0
#define NRST_CAM_PORT                   GPIOI
  #define NRST_CAM_GPIO_ENABLE_VDDIO()
#define NRST_CAM_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOI_CLK_ENABLE()

#define EN_CAM_PIN                      GPIO_PIN_7
#define EN_CAM_PORT                     GPIOI
#define EN_CAM_GPIO_ENABLE_VDDIO()
#define EN_CAM_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOI_CLK_ENABLE()

#define CSI2_CLK_ENABLE()               __HAL_RCC_CSI2_CLK_ENABLE()
#define CSI2_CLK_SLEEP_DISABLE()        __HAL_RCC_CSI2_CLK_SLEEP_DISABLE()
#define CSI2_CLK_FORCE_RESET()          __HAL_RCC_CSI2_FORCE_RESET()
#define CSI2_CLK_RELEASE_RESET()        __HAL_RCC_CSI2_RELEASE_RESET()

#define CMW_I2C_INIT BSP_I2C2_Init
#define CMW_I2C_DEINIT BSP_I2C2_DeInit
#define CMW_I2C_READREG16 BSP_I2C2_ReadReg16
#define CMW_I2C_WRITEREG16 BSP_I2C2_WriteReg16
#else
#error Add camera configuration GPIO for your specific board
#endif


/* Mirror/Flip */
#define CMW_MIRRORFLIP_NONE          0x00U   /* Set camera normal mode          */
#define CMW_MIRRORFLIP_FLIP          0x01U   /* Set camera flip config          */
#define CMW_MIRRORFLIP_MIRROR        0x02U   /* Set camera mirror config        */
#define CMW_MIRRORFLIP_FLIP_MIRROR   0x03U   /* Set camera flip + mirror config */

DCMIPP_HandleTypeDef* CMW_CAMERA_GetDCMIPPHandle();

int32_t CMW_CAMERA_Init( CMW_CameraInit_t *init_conf );
int32_t CMW_CAMERA_DeInit();
int32_t CMW_CAMERA_Run();
int32_t CMW_CAMERA_SetPipeConfig(uint32_t pipe, DCMIPP_Conf_t *p_conf);

int32_t CMW_CAMERA_Start(uint32_t pipe, uint8_t *pbuff, uint32_t Mode);
int32_t CMW_CAMERA_DoubleBufferStart(uint32_t pipe, uint8_t *pbuff1, uint8_t *pbuff2, uint32_t Mode);
int32_t CMW_CAMERA_Suspend(uint32_t pipe);


int CMW_CAMERA_SetAntiFlickerMode(int flicker_mode);
int CMW_CAMERA_GetAntiFlickerMode(int *flicker_mode);

int CMW_CAMERA_SetBrightness(int Brightness);
int CMW_CAMERA_GetBrightness(int *Brightness);

int CMW_CAMERA_SetContrast(int Contrast);
int CMW_CAMERA_GetContrast(int *Contrast);

int CMW_CAMERA_SetGain(int32_t Gain);
int CMW_CAMERA_GetGain(int32_t *Gain);

int CMW_CAMERA_SetExposure(int32_t exposure);
int CMW_CAMERA_GetExposure(int32_t *exposure);

int32_t CMW_CAMERA_SetMirrorFlip(int32_t MirrorFlip);
int32_t CMW_CAMERA_GetMirrorFlip(int32_t *MirrorFlip);

HAL_StatusTypeDef MX_DCMIPP_ClockConfig(DCMIPP_HandleTypeDef *hdcmipp);

int CMW_CAMERA_PIPE_FrameEventCallback(uint32_t pipe);
int CMW_CAMERA_PIPE_VsyncEventCallback(uint32_t pipe);

#ifdef __cplusplus
}
#endif

#endif /* __MW_CAMERA_H */
