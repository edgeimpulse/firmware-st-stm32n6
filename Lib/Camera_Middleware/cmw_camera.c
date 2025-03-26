 /**
 ******************************************************************************
 * @file    cmw_camera.c
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


/* Includes ------------------------------------------------------------------*/
#include "cmw_camera.h"

#include "isp_api.h"
#include "stm32n6xx_hal_dcmipp.h"
#include "cmw_utils.h"
#include "cmw_vd55g1.h"
#include "cmw_imx335.h"
#include "cmw_ov5640.h"
#include "cmw_vd66gy.h"
#include "cmw_vd1941.h"
#include "cmw_vd5941.h"

typedef struct
{
  uint32_t Resolution;
  uint32_t pixel_format;
  uint32_t LightMode;
  uint32_t ColorEffect;
  int32_t  Brightness;
  int32_t  Saturation;
  int32_t  Contrast;
  int32_t  HueDegree;
  int32_t  Gain;
  int32_t  Exposure;
  int32_t  ExposureMode;
  uint32_t MirrorFlip;
  uint32_t Zoom;
  uint32_t NightMode;
  uint32_t IsMspCallbacksValid;
  uint32_t TestPattern;
} CAMERA_Ctx_t;

CMW_CameraInit_t  camera_conf;
CAMERA_Ctx_t  Camera_Ctx;

DCMIPP_HandleTypeDef hcamera_dcmipp;
static CMW_Sensor_if_t Camera_Drv;

static union
{
#if defined(USE_IMX335_SENSOR)
  CMW_IMX335_t imx335_bsp;
#endif
#if defined(USE_VD55G1_SENSOR)
  CMW_VD55G1_t vd55g1_bsp;
#endif
#if defined(USE_VD66GY_SENSOR)
  CMW_VD66GY_t vd66gy_bsp;
#endif
#if defined(USE_OV5640_SENSOR)
  CMW_OV5640_t ov5640_bsp;
#endif
#if defined(USE_VD1941_SENSOR)
  CMW_VD1941_t vd1941_bsp;
#endif
#if defined(USE_VD5941_SENSOR)
  CMW_VD5941_t vd5941_bsp;
#endif
} camera_bsp;

int is_camera_init = 0;
int is_camera_started = 0;

static void DCMIPP_MspInit(DCMIPP_HandleTypeDef *hdcmipp);
static void DCMIPP_MspDeInit(DCMIPP_HandleTypeDef *hdcmipp);
#if defined(USE_IMX335_SENSOR)
static int32_t CMW_CAMERA_IMX335_Init( CMW_Sensor_Init_t *initSensors_params);
#endif
#if defined(USE_VD55G1_SENSOR)
static int32_t CMW_CAMERA_VD55G1_Init( CMW_Sensor_Init_t *initSensors_params);
#endif
#if defined(USE_OV5640_SENSOR)
static int32_t CMW_CAMERA_OV5640_Init( CMW_Sensor_Init_t *initSensors_params);
#endif
#if defined(USE_VD66GY_SENSOR)
static int32_t CMW_CAMERA_VD66GY_Init(CMW_Sensor_Init_t *initValues);
#endif
#if defined(USE_VD1941_SENSOR)
static int32_t CMW_CAMERA_VD1941_Init(CMW_Sensor_Init_t *initValues);
#endif
#if defined(USE_VD5941_SENSOR)
static int32_t CMW_CAMERA_VD5941_Init(CMW_Sensor_Init_t *initValues);
#endif
static void CMW_CAMERA_EnableGPIOs(void);
static void CMW_CAMERA_PwrDown(void);
static int32_t CMW_CAMERA_SetPipe(DCMIPP_HandleTypeDef *hdcmipp, uint32_t pipe, DCMIPP_Conf_t *p_conf);

DCMIPP_HandleTypeDef* CMW_CAMERA_GetDCMIPPHandle(void)
{
    return &hcamera_dcmipp;
}

int32_t CMW_CAMERA_SetPipeConfig(uint32_t pipe, DCMIPP_Conf_t *p_conf)
{
  if (pipe == DCMIPP_PIPE0)
  {
    // @TODO implement Pipe0
    return CMW_ERROR_WRONG_PARAM;
  }
  return CMW_CAMERA_SetPipe(&hcamera_dcmipp, pipe, p_conf);
}

/**
  * @brief  Initializes the camera.
  * @param  initConf  Camera sensor requested config
  * @retval CMW status
  */
int32_t CMW_CAMERA_Init(CMW_CameraInit_t *initConf)
{
  int32_t ret = CMW_ERROR_NONE;
  CMW_Sensor_Init_t initValues;

  initValues.width = initConf->width;
  initValues.height = initConf->height;
  initValues.fps = initConf->fps;
  initValues.pixel_format = initConf->pixel_format;
  initValues.mirrorFlip = initConf->mirror_flip;

  if (is_camera_init != 0)
  {
    return CMW_ERROR_NONE;
  }

  camera_conf = *initConf;

  /* Set DCMIPP instance */
  hcamera_dcmipp.Instance = DCMIPP;

  /* Configure DCMIPP clock */
  ret = MX_DCMIPP_ClockConfig(&hcamera_dcmipp);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }
  /* Enable DCMIPP clock */
  DCMIPP_MspInit(&hcamera_dcmipp);
  ret = HAL_DCMIPP_Init(&hcamera_dcmipp);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  CMW_CAMERA_EnableGPIOs();

#if defined(USE_IMX335_SENSOR)
  ret = CMW_CAMERA_IMX335_Init(&initValues);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_UNKNOWN_COMPONENT;
  }
#elif defined(USE_VD55G1_SENSOR)
  ret = CMW_CAMERA_VD55G1_Init(&initValues);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_UNKNOWN_COMPONENT;
  }
#elif defined(USE_OV5640_SENSOR)
  ret = CMW_CAMERA_OV5640_Init(&initValues);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_UNKNOWN_COMPONENT;
  }
#elif defined(USE_VD66GY_SENSOR)
  ret = CMW_CAMERA_VD66GY_Init(&initValues);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_UNKNOWN_COMPONENT;
  }
#elif defined(USE_VD1941_SENSOR)
  ret = CMW_CAMERA_VD1941_Init(&initValues);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_UNKNOWN_COMPONENT;
  }
#elif defined(USE_VD5941_SENSOR)
  ret = CMW_CAMERA_VD5941_Init(&initValues);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_UNKNOWN_COMPONENT;
  }
#endif

  is_camera_init++;
  /* CMW status */
  ret = CMW_ERROR_NONE;
  return ret;
}

/**
  * @brief  Set the camera Mirror/Flip.
  * @param  MirrorFlip CMW_MIRRORFLIP_NONE CMW_MIRRORFLIP_FLIP CMW_MIRRORFLIP_MIRROR CMW_MIRRORFLIP_FLIP_MIRROR
  * @retval CMW status
*/
int32_t CMW_CAMERA_SetMirrorFlip(int32_t MirrorFlip)
{
  int ret;

  if (Camera_Drv.SetMirrorFlip == NULL)
  {
    return CMW_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = Camera_Drv.SetMirrorFlip(&camera_bsp, MirrorFlip);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  camera_conf.mirror_flip = MirrorFlip;
  ret = CMW_ERROR_NONE;
  /* Return CMW status */
  return ret;
}

/**
  * @brief  Get the camera Mirror/Flip.
  * @param  MirrorFlip CMW_MIRRORFLIP_NONE CMW_MIRRORFLIP_FLIP CMW_MIRRORFLIP_MIRROR CMW_MIRRORFLIP_FLIP_MIRROR
  * @retval CMW status
*/
int32_t CMW_CAMERA_GetMirrorFlip(int32_t *MirrorFlip)
{
  *MirrorFlip = camera_conf.mirror_flip;
  return CMW_ERROR_NONE;
}

/**
  * @brief  Starts the camera capture in the selected mode.
  * @param  pipe  DCMIPP Pipe
  * @param  pbuff pointer to the camera output buffer
  * @param  mode  CAMERA_MODE_CONTINUOUS or CAMERA_MODE_SNAPSHOT
  * @retval CMW status
  */
int32_t CMW_CAMERA_Start(uint32_t pipe, uint8_t *pbuff, uint32_t mode)
{
  int32_t ret = CMW_ERROR_NONE;

  if (pipe >= DCMIPP_NUM_OF_PIPES)
  {
    return CMW_ERROR_WRONG_PARAM;
  }

  ret = HAL_DCMIPP_CSI_PIPE_Start(&hcamera_dcmipp, pipe, DCMIPP_VIRTUAL_CHANNEL0, (uint32_t)pbuff, mode);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  if (!is_camera_started)
  {
    ret = Camera_Drv.Start(&camera_bsp);
    if (ret != CMW_ERROR_NONE)
    {
      return CMW_ERROR_COMPONENT_FAILURE;
    }
    is_camera_started++;
  }

  /* Return CMW status */
  return ret;
}

#if defined (STM32N657xx)
/**
  * @brief  Starts the camera capture in the selected mode.
  * @param  pipe  DCMIPP Pipe
  * @param  pbuff1 pointer to the first camera output buffer
  * @param  pbuff2 pointer to the second camera output buffer
  * @param  mode  CAMERA_MODE_CONTINUOUS or CAMERA_MODE_SNAPSHOT
  * @retval CMW status
  */
int32_t CMW_CAMERA_DoubleBufferStart(uint32_t pipe, uint8_t *pbuff1, uint8_t *pbuff2, uint32_t Mode)
{
  int32_t ret = CMW_ERROR_NONE;

  if (pipe >= DCMIPP_NUM_OF_PIPES)
  {
    return CMW_ERROR_WRONG_PARAM;
  }

  if (HAL_DCMIPP_CSI_PIPE_DoubleBufferStart(&hcamera_dcmipp, pipe, DCMIPP_VIRTUAL_CHANNEL0, (uint32_t)pbuff1,
                                            (uint32_t)pbuff2, Mode) != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  if (!is_camera_started)
  {
    ret = Camera_Drv.Start(&camera_bsp);
    if (ret != CMW_ERROR_NONE)
    {
      return CMW_ERROR_COMPONENT_FAILURE;
    }
    is_camera_started++;
  }

  /* Return CMW status */
  return ret;
}
#endif




/**
  * @brief  DCMIPP Clock Config for DCMIPP.
  * @param  hdcmipp  DCMIPP Handle
  *         Being __weak it can be overwritten by the application
  * @retval HAL_status
  */
__weak HAL_StatusTypeDef MX_DCMIPP_ClockConfig(DCMIPP_HandleTypeDef *hdcmipp)
{
  UNUSED(hdcmipp);

  return HAL_OK;
}

/**
  * @brief  DeInitializes the camera.
  * @retval CMW status
  */
int32_t CMW_CAMERA_DeInit(void)
{
  int32_t ret = CMW_ERROR_NONE;


  ret = HAL_DCMIPP_CSI_PIPE_Stop(&hcamera_dcmipp, DCMIPP_PIPE1, DCMIPP_VIRTUAL_CHANNEL0);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  ret = HAL_DCMIPP_CSI_PIPE_Stop(&hcamera_dcmipp, DCMIPP_PIPE2, DCMIPP_VIRTUAL_CHANNEL0);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  ret = HAL_DCMIPP_DeInit(&hcamera_dcmipp);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  if (is_camera_init <= 0)
  {
    return CMW_ERROR_NONE;
  }

  /* Disable DCMIPP and CSI clock */
  DCMIPP_MspDeInit(&hcamera_dcmipp);

  /* De-initialize the camera module */
  ret = Camera_Drv.DeInit(&camera_bsp);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }
  /* Set Camera in Power Down */
  CMW_CAMERA_PwrDown();

  /* Update DCMIPPInit counter */
  is_camera_init--;
  is_camera_started--;

  /* Return CMW status */
  ret = CMW_ERROR_NONE;
  return ret;
}

/**
  * @brief  Suspend the CAMERA capture on selected pipe
  * @param  pipe Dcmipp pipe.
  * @retval CMW status
  */
int32_t CMW_CAMERA_Suspend(uint32_t pipe)
{
  if (hcamera_dcmipp.PipeState[pipe] > HAL_DCMIPP_PIPE_STATE_READY)
  {
    if (HAL_DCMIPP_PIPE_Suspend(&hcamera_dcmipp, pipe) != HAL_OK)
    {
      return CMW_ERROR_PERIPH_FAILURE;
    }
  }

  /* Return CMW status */
  return CMW_ERROR_NONE;
}

/**
  * @brief  Set the camera gain.
  * @param  Gain     Gain in dB
  * @retval CMW status
  */
int CMW_CAMERA_SetGain(int32_t Gain)
{
  int ret;
  if(Camera_Drv.SetGain == NULL)
  {
    return CMW_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = Camera_Drv.SetGain(&camera_bsp, Gain);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  Camera_Ctx.Gain = Gain;
  return CMW_ERROR_NONE;
}

/**
  * @brief  Get the camera gain.
  * @param  Gain     Gain in dB
  * @retval CMW status
  */
int CMW_CAMERA_GetGain(int32_t *Gain)
{
  *Gain = Camera_Ctx.Gain;
  return CMW_ERROR_NONE;
}

/**
  * @brief  Set the camera exposure.
  * @param  exposure exposure in microseconds
  * @retval CMW status
  */
int CMW_CAMERA_SetExposure(int32_t exposure)
{
  int ret;

  if(Camera_Drv.SetExposure == NULL)
  {
    return CMW_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = Camera_Drv.SetExposure(&camera_bsp, exposure);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  Camera_Ctx.Exposure = exposure;
  return CMW_ERROR_NONE;
}

/**
  * @brief  Get the camera exposure.
  * @param  exposure exposure in microseconds
  * @retval CMW status
  */
int CMW_CAMERA_GetExposure(int32_t *exposure)
{
  *exposure = Camera_Ctx.Exposure;
  return CMW_ERROR_NONE;
}

/**
  * @brief  Set the camera exposure mode.
  * @param  exposureMode Exposure mode CAMERA_EXPOSURE_AUTO, CAMERA_EXPOSURE_AUTOFREEZE, CAMERA_EXPOSURE_MANUAL
  * @retval CMW status
  */
int32_t CMW_CAMERA_SetExposureMode(int32_t exposureMode)
{
  int ret;

  if(Camera_Drv.SetExposureMode == NULL)
  {
    return CMW_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = Camera_Drv.SetExposureMode(&camera_bsp, exposureMode);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  Camera_Ctx.ExposureMode = exposureMode;
  return CMW_ERROR_NONE;
}

/**
  * @brief  Get the camera exposure mode.
  * @param  exposureMode Exposure mode CAMERA_EXPOSURE_AUTO, CAMERA_EXPOSURE_AUTOFREEZE, CAMERA_EXPOSURE_MANUAL
  * @retval CMW status
  */
int32_t CMW_CAMERA_GetExposureMode(int32_t *exposureMode)
{
  *exposureMode = Camera_Ctx.ExposureMode;
  return CMW_ERROR_NONE;
}

/**
  * @brief  Set (Enable/Disable and Configure) the camera test pattern
  * @param  mode Pattern mode (sensor specific value) to be configured. '-1' means disable.
  * @retval CMW status
  */
int32_t CMW_CAMERA_SetTestPattern(int32_t mode)
{
  int32_t ret;

  if(Camera_Drv.SetTestPattern == NULL)
  {
    return CMW_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = Camera_Drv.SetTestPattern(&camera_bsp, mode);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  Camera_Ctx.TestPattern = mode;
  return CMW_ERROR_NONE;
}

/**
  * @brief  Get the camera test pattern
  * @param  mode Pattern mode (sensor specific value) to be returned. '-1' means disable.
  * @retval CMW status
  */
int32_t CMW_CAMERA_GetTestPattern(int32_t *mode)
{
  *mode = Camera_Ctx.TestPattern;
  return CMW_ERROR_NONE;
}

/**
  * @brief  Get the Camera Sensor info.
  * @param  info  pointer to sensor info
  * @note   This function should be called after the init. This to get Capabilities
  *         from the camera sensor
  * @retval Component status
  */
int32_t CMW_CAMERA_GetSensorInfo(ISP_SensorInfoTypeDef *info)
{

  int32_t ret;

  if(Camera_Drv.GetSensorInfo == NULL)
  {
    return CMW_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = Camera_Drv.GetSensorInfo(&camera_bsp, info);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  return CMW_ERROR_NONE;
}



int32_t CMW_CAMERA_Run()
{
  if(Camera_Drv.Run != NULL)
  {
      return Camera_Drv.Run(&camera_bsp);
  }
  return CMW_ERROR_NONE;
}

/**
 * @brief  Vsync Event callback on pipe
 * @param  hdcmipp DCMIPP device handle
 *         Pipe    Pipe receiving the callback
 * @retval None
 */
__weak int CMW_CAMERA_PIPE_VsyncEventCallback(uint32_t pipe)
{
  UNUSED(pipe);

  return HAL_OK;
}

/**
 * @brief  Frame Event callback on pipe
 * @param  hdcmipp DCMIPP device handle
 *         Pipe    Pipe receiving the callback
 * @retval None
 */
__weak int CMW_CAMERA_PIPE_FrameEventCallback(uint32_t pipe)
{
  UNUSED(pipe);

  return HAL_OK;
}

/**
 * @brief  Vsync Event callback on pipe
 * @param  hdcmipp DCMIPP device handle
 *         Pipe    Pipe receiving the callback
 * @retval None
 */
void HAL_DCMIPP_PIPE_VsyncEventCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe)
{
  UNUSED(hdcmipp);
  if(Camera_Drv.VsyncEventCallback != NULL)
  {
      Camera_Drv.VsyncEventCallback(&camera_bsp, Pipe);
  }
  CMW_CAMERA_PIPE_VsyncEventCallback(Pipe);
}

/**
 * @brief  Frame Event callback on pipe
 * @param  hdcmipp DCMIPP device handle
 *         Pipe    Pipe receiving the callback
 * @retval None
 */
void HAL_DCMIPP_PIPE_FrameEventCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe)
{
  UNUSED(hdcmipp);
  if(Camera_Drv.FrameEventCallback != NULL)
  {
      Camera_Drv.FrameEventCallback(&camera_bsp, Pipe);
  }
  CMW_CAMERA_PIPE_FrameEventCallback(Pipe);
}

/**
  * @brief  Initializes the DCMIPP MSP.
  * @param  hdcmipp  DCMIPP handle
  * @retval None
  */
static void DCMIPP_MspInit(DCMIPP_HandleTypeDef *hdcmipp)
{
  UNUSED(hdcmipp);

  /*** Enable peripheral clock ***/
  /* Enable DCMIPP clock */
  __HAL_RCC_DCMIPP_CLK_ENABLE();
  __HAL_RCC_DCMIPP_CLK_SLEEP_DISABLE();
  __HAL_RCC_DCMIPP_FORCE_RESET();
  __HAL_RCC_DCMIPP_RELEASE_RESET();

  /*** Configure the NVIC for DCMIPP ***/
  /* NVIC configuration for DCMIPP transfer complete interrupt */
  HAL_NVIC_SetPriority(DCMIPP_IRQn, 0x07, 0);
  HAL_NVIC_EnableIRQ(DCMIPP_IRQn);

  /*** Enable peripheral clock ***/
  /* Enable CSI clock */
#if defined (STM32MP257Fxx)
  __HAL_RCC_CSI2_CLK_ENABLE();
  __HAL_RCC_CSI2_CLK_SLEEP_DISABLE();
  __HAL_RCC_CSI2_FORCE_RESET();
  __HAL_RCC_CSI2_RELEASE_RESET();

#else
  __HAL_RCC_CSI_CLK_ENABLE();
  __HAL_RCC_CSI_CLK_SLEEP_DISABLE();
  __HAL_RCC_CSI_FORCE_RESET();
  __HAL_RCC_CSI_RELEASE_RESET();
#endif
  /*** Configure the NVIC for CSI ***/
  /* NVIC configuration for CSI transfer complete interrupt */
  HAL_NVIC_SetPriority(CSI_IRQn, 0x07, 0);
  HAL_NVIC_EnableIRQ(CSI_IRQn);

}

/**
  * @brief  DeInitializes the DCMIPP MSP.
  * @param  hdcmipp  DCMIPP handle
  * @retval None
  */
static void DCMIPP_MspDeInit(DCMIPP_HandleTypeDef *hdcmipp)
{
  UNUSED(hdcmipp);
#if defined (STM32MP257Fxx)
  #error Implement Deinit for MP2
#else
  __HAL_RCC_DCMIPP_FORCE_RESET();
  __HAL_RCC_DCMIPP_RELEASE_RESET();

  /* Disable NVIC  for DCMIPP transfer complete interrupt */
  HAL_NVIC_DisableIRQ(DCMIPP_IRQn);

  /* Disable DCMIPP clock */
  __HAL_RCC_DCMIPP_CLK_DISABLE();

  __HAL_RCC_CSI_FORCE_RESET();
  __HAL_RCC_CSI_RELEASE_RESET();

  /* Disable NVIC  for DCMIPP transfer complete interrupt */
  HAL_NVIC_DisableIRQ(CSI_IRQn);

  /* Disable DCMIPP clock */
  __HAL_RCC_CSI_CLK_DISABLE();
  #endif
}

/**
  * @brief  CAMERA hardware reset
  * @retval CMW status
  */
static void CMW_CAMERA_EnableGPIOs(void)
{
  GPIO_InitTypeDef gpio_init_structure = {0};

  /* Enable GPIO clocks */
  EN_CAM_GPIO_ENABLE_VDDIO();
  EN_CAM_GPIO_CLK_ENABLE();
  NRST_CAM_GPIO_ENABLE_VDDIO();
  NRST_CAM_GPIO_CLK_ENABLE();

  gpio_init_structure.Pin       = EN_CAM_PIN;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(EN_CAM_PORT, &gpio_init_structure);

  gpio_init_structure.Pin       = NRST_CAM_PIN;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(NRST_CAM_PORT, &gpio_init_structure);
}

/**
  * @brief  CAMERA power down
  * @retval CMW status
  */
static void CMW_CAMERA_PwrDown(void)
{
  GPIO_InitTypeDef gpio_init_structure = {0};

  gpio_init_structure.Pin       = EN_CAM_PIN;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(EN_CAM_PORT, &gpio_init_structure);

  gpio_init_structure.Pin       = NRST_CAM_PIN;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(NRST_CAM_PORT, &gpio_init_structure);

  /* Camera power down sequence */
  /* Assert the camera Enable pin (active high) */
  HAL_GPIO_WritePin(EN_CAM_PORT, EN_CAM_PIN, GPIO_PIN_RESET);

  /* De-assert the camera NRST pin (active low) */
  HAL_GPIO_WritePin(NRST_CAM_PORT, NRST_CAM_PIN, GPIO_PIN_RESET);

}

static void CMW_CAMERA_ShutdownPin(int value)
{
  HAL_GPIO_WritePin(NRST_CAM_PORT, NRST_CAM_PIN, value ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void CMW_CAMERA_EnablePin(int value)
{
  HAL_GPIO_WritePin(EN_CAM_PORT, EN_CAM_PIN, value ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

#if defined(USE_VD66GY_SENSOR) || defined(USE_IMX335_SENSOR)
static ISP_StatusTypeDef CB_ISP_SetSensorGain(uint32_t camera_instance, int32_t gain)
{
  if (CMW_CAMERA_SetGain(gain) != CMW_ERROR_NONE)
    return ISP_ERR_SENSORGAIN;

  return ISP_OK;
}

static ISP_StatusTypeDef CB_ISP_GetSensorGain(uint32_t camera_instance, int32_t *gain)
{
  if (CMW_CAMERA_GetGain(gain) != CMW_ERROR_NONE)
    return ISP_ERR_SENSORGAIN;

  return ISP_OK;
}

static ISP_StatusTypeDef CB_ISP_SetSensorExposure(uint32_t camera_instance, int32_t exposure)
{
  if (CMW_CAMERA_SetExposure(exposure) != CMW_ERROR_NONE)
    return ISP_ERR_SENSOREXPOSURE;

  return ISP_OK;
}

static ISP_StatusTypeDef CB_ISP_GetSensorExposure(uint32_t camera_instance, int32_t *exposure)
{
  if (CMW_CAMERA_GetExposure(exposure) != CMW_ERROR_NONE)
    return ISP_ERR_SENSOREXPOSURE;

  return ISP_OK;
}

static ISP_StatusTypeDef CB_ISP_GetSensorInfo(uint32_t camera_instance, ISP_SensorInfoTypeDef *Info)
{
  if(Camera_Drv.GetSensorInfo != NULL)
  {
    if (Camera_Drv.GetSensorInfo(&camera_bsp, Info) != CMW_ERROR_NONE)
      return ISP_ERR_SENSOREXPOSURE;
  }
  return ISP_OK;
}
#endif

#if defined(USE_VD55G1_SENSOR)
static int32_t CMW_CAMERA_VD55G1_Init( CMW_Sensor_Init_t *initSensors_params)
{
  int32_t ret = CMW_ERROR_NONE;
  DCMIPP_CSI_ConfTypeDef csi_conf = { 0 };
  DCMIPP_CSI_PIPE_ConfTypeDef csi_pipe_conf = { 0 };

  camera_bsp.vd55g1_bsp.Address     = CAMERA_VD55G1_ADDRESS;
  camera_bsp.vd55g1_bsp.ClockInHz   = CAMERA_VD55G1_FREQ_IN_HZ;
  camera_bsp.vd55g1_bsp.Init        = CMW_I2C_INIT;
  camera_bsp.vd55g1_bsp.DeInit      = CMW_I2C_DEINIT;
  camera_bsp.vd55g1_bsp.WriteReg    = CMW_I2C_WRITEREG16;
  camera_bsp.vd55g1_bsp.ReadReg     = CMW_I2C_READREG16;
  camera_bsp.vd55g1_bsp.Delay       = HAL_Delay;
  camera_bsp.vd55g1_bsp.ShutdownPin = CMW_CAMERA_ShutdownPin;
  camera_bsp.vd55g1_bsp.EnablePin   = CMW_CAMERA_EnablePin;

  ret = CMW_VD55G1_Probe(&camera_bsp.vd55g1_bsp, &Camera_Drv);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = Camera_Drv.Init(&camera_bsp, initSensors_params);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  csi_conf.NumberOfLanes = DCMIPP_CSI_ONE_DATA_LANE;
  csi_conf.DataLaneMapping = DCMIPP_CSI_PHYSICAL_DATA_LANES;
  csi_conf.PHYBitrate = DCMIPP_CSI_PHY_BT_800;
  ret = HAL_DCMIPP_CSI_SetConfig(&hcamera_dcmipp, &csi_conf);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  ret = HAL_DCMIPP_CSI_SetVCConfig(&hcamera_dcmipp, DCMIPP_VIRTUAL_CHANNEL0, DCMIPP_CSI_DT_BPP8);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  csi_pipe_conf.DataTypeMode = DCMIPP_DTMODE_DTIDA;
  csi_pipe_conf.DataTypeIDA = DCMIPP_DT_RAW8;
  csi_pipe_conf.DataTypeIDB = 0;
  ret = HAL_DCMIPP_CSI_PIPE_SetConfig(&hcamera_dcmipp, DCMIPP_PIPE1, &csi_pipe_conf);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  return CMW_ERROR_NONE;
}
#endif

#if defined(USE_OV5640_SENSOR)
static int32_t CMW_CAMERA_OV5640_Init( CMW_Sensor_Init_t *initSensors_params)
{
  int32_t ret = CMW_ERROR_NONE;
  DCMIPP_CSI_ConfTypeDef csi_conf = { 0 };
  DCMIPP_CSI_PIPE_ConfTypeDef csi_pipe_conf = { 0 };

  camera_bsp.ov5640_bsp.Address     = CAMERA_OV5640_ADDRESS;
  camera_bsp.ov5640_bsp.Init        = CMW_I2C_INIT;
  camera_bsp.ov5640_bsp.DeInit      = CMW_I2C_DEINIT;
  camera_bsp.ov5640_bsp.WriteReg    = CMW_I2C_WRITEREG16;
  camera_bsp.ov5640_bsp.ReadReg     = CMW_I2C_READREG16;
  camera_bsp.ov5640_bsp.GetTick     = BSP_GetTick;
  camera_bsp.ov5640_bsp.Delay       = HAL_Delay;
  camera_bsp.ov5640_bsp.ShutdownPin = CMW_CAMERA_ShutdownPin;
  camera_bsp.ov5640_bsp.EnablePin   = CMW_CAMERA_EnablePin;

  ret = CMW_OV5640_Probe(&camera_bsp.ov5640_bsp, &Camera_Drv);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = Camera_Drv.Init(&camera_bsp, initSensors_params);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  csi_conf.NumberOfLanes = DCMIPP_CSI_TWO_DATA_LANES;
  csi_conf.DataLaneMapping = DCMIPP_CSI_PHYSICAL_DATA_LANES;
  csi_conf.PHYBitrate = DCMIPP_CSI_PHY_BT_250;
  ret = HAL_DCMIPP_CSI_SetConfig(&hcamera_dcmipp, &csi_conf);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  ret = HAL_DCMIPP_CSI_SetVCConfig(&hcamera_dcmipp, DCMIPP_VIRTUAL_CHANNEL0, DCMIPP_CSI_DT_BPP8);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  csi_pipe_conf.DataTypeMode = DCMIPP_DTMODE_DTIDA;
  csi_pipe_conf.DataTypeIDA = DCMIPP_DT_RGB565;
  csi_pipe_conf.DataTypeIDB = 0;
  ret = HAL_DCMIPP_CSI_PIPE_SetConfig(&hcamera_dcmipp, DCMIPP_PIPE1, &csi_pipe_conf);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  return CMW_ERROR_NONE;
}
#endif

#if defined(USE_VD66GY_SENSOR)
static int32_t CMW_CAMERA_VD66GY_Init( CMW_Sensor_Init_t *initSensors_params)
{
  int32_t ret = CMW_ERROR_NONE;
  DCMIPP_CSI_ConfTypeDef csi_conf = { 0 };
  DCMIPP_CSI_PIPE_ConfTypeDef csi_pipe_conf = { 0 };

  camera_bsp.vd66gy_bsp.Address     = CAMERA_VD66GY_ADDRESS;
  camera_bsp.vd66gy_bsp.ClockInHz   = CAMERA_VD66GY_FREQ_IN_HZ;
  camera_bsp.vd66gy_bsp.Init        = CMW_I2C_INIT;
  camera_bsp.vd66gy_bsp.DeInit      = CMW_I2C_DEINIT;
  camera_bsp.vd66gy_bsp.ReadReg     = CMW_I2C_READREG16;
  camera_bsp.vd66gy_bsp.WriteReg    = CMW_I2C_WRITEREG16;
  camera_bsp.vd66gy_bsp.Delay       = HAL_Delay;
  camera_bsp.vd66gy_bsp.ShutdownPin = CMW_CAMERA_ShutdownPin;
  camera_bsp.vd66gy_bsp.EnablePin   = CMW_CAMERA_EnablePin;
  camera_bsp.vd66gy_bsp.hdcmipp     = &hcamera_dcmipp;
  camera_bsp.vd66gy_bsp.appliHelpers.SetSensorGain = CB_ISP_SetSensorGain;
  camera_bsp.vd66gy_bsp.appliHelpers.GetSensorGain = CB_ISP_GetSensorGain;
  camera_bsp.vd66gy_bsp.appliHelpers.SetSensorExposure = CB_ISP_SetSensorExposure;
  camera_bsp.vd66gy_bsp.appliHelpers.GetSensorExposure = CB_ISP_GetSensorExposure;
  camera_bsp.vd66gy_bsp.appliHelpers.GetSensorInfo = CB_ISP_GetSensorInfo;

  ret = CMW_VD66GY_Probe(&camera_bsp.vd66gy_bsp, &Camera_Drv);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = Camera_Drv.Init(&camera_bsp, initSensors_params);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  csi_conf.NumberOfLanes = DCMIPP_CSI_TWO_DATA_LANES;
  csi_conf.DataLaneMapping = DCMIPP_CSI_PHYSICAL_DATA_LANES;
  csi_conf.PHYBitrate = DCMIPP_CSI_PHY_BT_800;
  ret = HAL_DCMIPP_CSI_SetConfig(&hcamera_dcmipp, &csi_conf);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  ret = HAL_DCMIPP_CSI_SetVCConfig(&hcamera_dcmipp, DCMIPP_VIRTUAL_CHANNEL0, DCMIPP_CSI_DT_BPP8);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  csi_pipe_conf.DataTypeMode = DCMIPP_DTMODE_DTIDA;
  csi_pipe_conf.DataTypeIDA = DCMIPP_DT_RAW8;
  csi_pipe_conf.DataTypeIDB = 0;
  ret = HAL_DCMIPP_CSI_PIPE_SetConfig(&hcamera_dcmipp, DCMIPP_PIPE1, &csi_pipe_conf);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  return CMW_ERROR_NONE;
}
#endif

#if defined(USE_VD1941_SENSOR)
static int32_t CMW_CAMERA_VD1941_Init( CMW_Sensor_Init_t *initSensors_params)
{
  int32_t ret = CMW_ERROR_NONE;
  DCMIPP_CSI_ConfTypeDef csi_conf = { 0 };
  DCMIPP_CSI_PIPE_ConfTypeDef csi_pipe_conf = { 0 };

  camera_bsp.vd1941_bsp.Address     = CAMERA_VD1941_ADDRESS;
  camera_bsp.vd1941_bsp.Init        = CMW_I2C_INIT;
  camera_bsp.vd1941_bsp.DeInit      = CMW_I2C_DEINIT;
  camera_bsp.vd1941_bsp.WriteReg    = CMW_I2C_WRITEREG16;
  camera_bsp.vd1941_bsp.ReadReg     = CMW_I2C_READREG16;
  camera_bsp.vd1941_bsp.Delay       = HAL_Delay;
  camera_bsp.vd1941_bsp.ShutdownPin = CMW_CAMERA_ShutdownPin;
  camera_bsp.vd1941_bsp.EnablePin   = CMW_CAMERA_EnablePin;
  camera_bsp.vd1941_bsp.hdcmipp     = &hcamera_dcmipp;

  ret = CMW_VD1941_Probe(&camera_bsp.vd1941_bsp, &Camera_Drv);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = Camera_Drv.Init(&camera_bsp, initSensors_params);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  csi_conf.NumberOfLanes = DCMIPP_CSI_TWO_DATA_LANES;
  csi_conf.DataLaneMapping = DCMIPP_CSI_PHYSICAL_DATA_LANES;
  csi_conf.PHYBitrate = DCMIPP_CSI_PHY_BT_1300;
  ret = HAL_DCMIPP_CSI_SetConfig(&hcamera_dcmipp, &csi_conf);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  ret = HAL_DCMIPP_CSI_SetVCConfig(&hcamera_dcmipp, DCMIPP_VIRTUAL_CHANNEL0, DCMIPP_CSI_DT_BPP8);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  csi_pipe_conf.DataTypeMode = DCMIPP_DTMODE_DTIDA;
  csi_pipe_conf.DataTypeIDA = DCMIPP_DT_RAW8;
  csi_pipe_conf.DataTypeIDB = 0;
  ret = HAL_DCMIPP_CSI_PIPE_SetConfig(&hcamera_dcmipp, DCMIPP_PIPE1, &csi_pipe_conf);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  return CMW_ERROR_NONE;
}
#endif

#if defined(USE_VD5941_SENSOR)
static int32_t CMW_CAMERA_VD5941_Init( CMW_Sensor_Init_t *initSensors_params)
{
  int32_t ret = CMW_ERROR_NONE;
  DCMIPP_CSI_ConfTypeDef csi_conf = { 0 };
  DCMIPP_CSI_PIPE_ConfTypeDef csi_pipe_conf = { 0 };

  camera_bsp.vd5941_bsp.Address     = CAMERA_VD5941_ADDRESS;
  camera_bsp.vd5941_bsp.Init        = BSP_I2C2_Init;
  camera_bsp.vd5941_bsp.DeInit      = BSP_I2C2_DeInit;
  camera_bsp.vd5941_bsp.WriteReg    = BSP_I2C2_WriteReg16;
  camera_bsp.vd5941_bsp.ReadReg     = BSP_I2C2_ReadReg16;
  camera_bsp.vd5941_bsp.Delay       = HAL_Delay;
  camera_bsp.vd5941_bsp.ShutdownPin = CMW_CAMERA_ShutdownPin;
  camera_bsp.vd5941_bsp.EnablePin   = CMW_CAMERA_EnablePin;
  camera_bsp.vd5941_bsp.hdcmipp     = &hcamera_dcmipp;

  ret = CMW_VD5941_Probe(&camera_bsp.vd5941_bsp, &Camera_Drv);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = Camera_Drv.Init(&camera_bsp, initSensors_params);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  csi_conf.NumberOfLanes = DCMIPP_CSI_TWO_DATA_LANES;
  csi_conf.DataLaneMapping = DCMIPP_CSI_PHYSICAL_DATA_LANES;
  csi_conf.PHYBitrate = DCMIPP_CSI_PHY_BT_1300;
  ret = HAL_DCMIPP_CSI_SetConfig(&hcamera_dcmipp, &csi_conf);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  ret = HAL_DCMIPP_CSI_SetVCConfig(&hcamera_dcmipp, DCMIPP_VIRTUAL_CHANNEL0, DCMIPP_CSI_DT_BPP8);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  csi_pipe_conf.DataTypeMode = DCMIPP_DTMODE_DTIDA;
  csi_pipe_conf.DataTypeIDA = DCMIPP_DT_RAW8;
  csi_pipe_conf.DataTypeIDB = 0;
  ret = HAL_DCMIPP_CSI_PIPE_SetConfig(&hcamera_dcmipp, DCMIPP_PIPE1, &csi_pipe_conf);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  return CMW_ERROR_NONE;
}
#endif

#if defined(USE_IMX335_SENSOR)
static int32_t CMW_CAMERA_IMX335_Init( CMW_Sensor_Init_t *initSensors_params)
{
  int32_t ret = CMW_ERROR_NONE;
  DCMIPP_CSI_ConfTypeDef csi_conf = { 0 };
  DCMIPP_CSI_PIPE_ConfTypeDef csi_pipe_conf = { 0 };
  camera_bsp.imx335_bsp.Address     = CAMERA_IMX335_ADDRESS;
  camera_bsp.imx335_bsp.Init        = CMW_I2C_INIT;
  camera_bsp.imx335_bsp.DeInit      = CMW_I2C_DEINIT;
  camera_bsp.imx335_bsp.ReadReg     = CMW_I2C_READREG16;
  camera_bsp.imx335_bsp.WriteReg    = CMW_I2C_WRITEREG16;
  camera_bsp.imx335_bsp.GetTick     = BSP_GetTick;
  camera_bsp.imx335_bsp.Delay       = HAL_Delay;
  camera_bsp.imx335_bsp.ShutdownPin = CMW_CAMERA_ShutdownPin;
  camera_bsp.imx335_bsp.EnablePin   = CMW_CAMERA_EnablePin;
  camera_bsp.imx335_bsp.hdcmipp     = &hcamera_dcmipp;
  camera_bsp.imx335_bsp.appliHelpers.SetSensorGain = CB_ISP_SetSensorGain;
  camera_bsp.imx335_bsp.appliHelpers.GetSensorGain = CB_ISP_GetSensorGain;
  camera_bsp.imx335_bsp.appliHelpers.SetSensorExposure = CB_ISP_SetSensorExposure;
  camera_bsp.imx335_bsp.appliHelpers.GetSensorExposure = CB_ISP_GetSensorExposure;
  camera_bsp.imx335_bsp.appliHelpers.GetSensorInfo = CB_ISP_GetSensorInfo;

  ret = CMW_IMX335_Probe(&camera_bsp.imx335_bsp, &Camera_Drv);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = Camera_Drv.Init(&camera_bsp, initSensors_params);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = Camera_Drv.SetFrequency(&camera_bsp, IMX335_INCK_24MHZ);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  csi_conf.NumberOfLanes = DCMIPP_CSI_TWO_DATA_LANES;
  csi_conf.DataLaneMapping = DCMIPP_CSI_PHYSICAL_DATA_LANES;
  csi_conf.PHYBitrate = DCMIPP_CSI_PHY_BT_1600;
  ret = HAL_DCMIPP_CSI_SetConfig(&hcamera_dcmipp, &csi_conf);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  ret = HAL_DCMIPP_CSI_SetVCConfig(&hcamera_dcmipp, DCMIPP_VIRTUAL_CHANNEL0, DCMIPP_CSI_DT_BPP10);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  csi_pipe_conf.DataTypeMode = DCMIPP_DTMODE_DTIDA;
  csi_pipe_conf.DataTypeIDA = DCMIPP_DT_RAW10;
  csi_pipe_conf.DataTypeIDB = 0;
  ret = HAL_DCMIPP_CSI_PIPE_SetConfig(&hcamera_dcmipp, DCMIPP_PIPE1, &csi_pipe_conf);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }


  return ret;
}
#endif

static int32_t CMW_CAMERA_SetPipe(DCMIPP_HandleTypeDef *hdcmipp, uint32_t pipe, DCMIPP_Conf_t *p_conf)
{
  DCMIPP_DecimationConfTypeDef dec_conf = { 0 };
  DCMIPP_PipeConfTypeDef pipe_conf = { 0 };
  DCMIPP_DownsizeTypeDef down_conf = { 0 };
  DCMIPP_CropConfTypeDef crop_conf = { 0 };
  int ret;

  CMW_UTILS_GetPipeConfig(camera_conf.width, camera_conf.height, p_conf, &crop_conf, &dec_conf, &down_conf);

  if (crop_conf.VSize != 0 && crop_conf.HSize != 0)
  {
    ret = HAL_DCMIPP_PIPE_SetCropConfig(hdcmipp, pipe, &crop_conf);
    if (ret != HAL_OK)
    {
      return CMW_ERROR_COMPONENT_FAILURE;
    }

    ret = HAL_DCMIPP_PIPE_EnableCrop(hdcmipp, pipe);
    if (ret != HAL_OK)
    {
      return CMW_ERROR_COMPONENT_FAILURE;
    }
  }

  ret = HAL_DCMIPP_PIPE_SetDecimationConfig(hdcmipp, pipe, &dec_conf);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = HAL_DCMIPP_PIPE_EnableDecimation(hdcmipp, pipe);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = HAL_DCMIPP_PIPE_SetDownsizeConfig(hdcmipp, pipe, &down_conf);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  ret = HAL_DCMIPP_PIPE_EnableDownsize(hdcmipp, pipe);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

  if (p_conf->enable_swap)
  {
    /* Config pipe */
    ret = HAL_DCMIPP_PIPE_EnableRedBlueSwap(hdcmipp, pipe);
    if (ret != HAL_OK)
    {
      return CMW_ERROR_COMPONENT_FAILURE;
    }
  }

  if (p_conf->enable_gamma_conversion)
  {
    ret = HAL_DCMIPP_PIPE_EnableGammaConversion(&hcamera_dcmipp, pipe);
    if (ret != HAL_OK)
    {
      return CMW_ERROR_COMPONENT_FAILURE;
    }
  }

  if (pipe == DCMIPP_PIPE2)
  {
    ret = HAL_DCMIPP_PIPE_CSI_EnableShare(hdcmipp, pipe);
    if (ret != HAL_OK)
    {
      return CMW_ERROR_COMPONENT_FAILURE;
    }
  }

  pipe_conf.FrameRate = DCMIPP_FRAME_RATE_ALL;
  pipe_conf.PixelPipePitch = p_conf->output_width * p_conf->output_bpp;
  pipe_conf.PixelPackerFormat = p_conf->output_format;
  ret = HAL_DCMIPP_PIPE_SetConfig(hdcmipp, pipe, &pipe_conf);
  if (ret != HAL_OK)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }
  return HAL_OK;
}

