/**
 ******************************************************************************
 * @file    isp_algo.c
 * @author  AIS Application Team
 * @brief   ISP algorithm
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

/* Includes ------------------------------------------------------------------*/
#include "isp_core.h"
#include "isp_algo.h"
#include "isp_services.h"
#include "evision-api-ae.h"
#include "evision-api-awb.h"
#include "evision-api-utils.h"
#include <limits.h>
#include <math.h>

/* Private types -------------------------------------------------------------*/
/* ISP algorithms identifier */
typedef enum
{
  ISP_ALGO_ID_BADPIXEL = 0U,
  ISP_ALGO_ID_BLACKLEVEL,
  ISP_ALGO_ID_SIMPLEAWB,
  ISP_ALGO_ID_SIMPLEAEC,
  ISP_ALGO_ID_AEC,
  ISP_ALGO_ID_AWB,
  ISP_ALGO_ID_SIMPLEAWB_CCT,
} ISP_AlgoIDTypeDef;

/* Private constants ---------------------------------------------------------*/
#define EVISION_AE_LUT_EXPOSURE_SIZE (2048)
#define EVISION_AE_LUT_GAIN_SIZE (241)
#define AEC_GAIN_UPDATE_MAX 5000
#define AEC_TOLERANCE       5
#define AEC_COEFF_LUM_GAIN  100

#define ALGO_ISP_VSYNC_LATENCY        2
#define ALGO_ISP_SENSOR_VSYNC_LATENCY (2 + 1)

#define ALGO_AWB_CCT_PREVENT_NB 11

/* Debug logs control */
//#define ALGO_AWB_CCT_DBG_LOGS
//#define ALGO_AWB_DBG_LOGS
//#define ALGO_AEC_DBG_LOGS
//#define ALGO_PERF_DBG_LOGS

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
ISP_StatusTypeDef ISP_Algo_BadPixel_Init(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_BadPixel_DeInit(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_BadPixel_Process(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_BlackLevel_Init(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_BlackLevel_DeInit(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_BlackLevel_Process(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_SimpleAWB_Init(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_SimpleAWB_DeInit(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_SimpleAWB_Process(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_SimpleAEC_Init(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_SimpleAEC_DeInit(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_SimpleAEC_Process(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_AEC_Init(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_AEC_DeInit(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_AEC_Process(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_AWB_Init(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_AWB_DeInit(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_AWB_Process(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_SimpleAWB_CCT_Init(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_SimpleAWB_CCT_DeInit(void *hIsp, void *pAlgo);
ISP_StatusTypeDef ISP_Algo_SimpleAWB_CCT_Process(void *hIsp, void *pAlgo);

/* Private variables ---------------------------------------------------------*/
/* Bad Pixel algorithm handle */
ISP_AlgoTypeDef ISP_Algo_BadPixel = {
    .id = ISP_ALGO_ID_BADPIXEL,
    .Init = ISP_Algo_BadPixel_Init,
    .DeInit = ISP_Algo_BadPixel_DeInit,
    .Process = ISP_Algo_BadPixel_Process,
};

/* Black Level algorithm handle */
ISP_AlgoTypeDef ISP_Algo_BlackLevel = {
    .id = ISP_ALGO_ID_BLACKLEVEL,
    .Init = ISP_Algo_BlackLevel_Init,
    .DeInit = ISP_Algo_BlackLevel_DeInit,
    .Process = ISP_Algo_BlackLevel_Process,
};

/* Simple AWB algorithm handle */
ISP_AlgoTypeDef ISP_Algo_SimpleAWB = {
    .id = ISP_ALGO_ID_SIMPLEAWB,
    .Init = ISP_Algo_SimpleAWB_Init,
    .DeInit = ISP_Algo_SimpleAWB_DeInit,
    .Process = ISP_Algo_SimpleAWB_Process,
};

/* Simple AEC algorithm handle */
ISP_AlgoTypeDef ISP_Algo_SimpleAEC = {
    .id = ISP_ALGO_ID_SIMPLEAEC,
    .Init = ISP_Algo_SimpleAEC_Init,
    .DeInit = ISP_Algo_SimpleAEC_DeInit,
    .Process = ISP_Algo_SimpleAEC_Process,
};

/* AEC algorithm handle */
ISP_AlgoTypeDef ISP_Algo_AEC = {
    .id = ISP_ALGO_ID_AEC,
    .Init = ISP_Algo_AEC_Init,
    .DeInit = ISP_Algo_AEC_DeInit,
    .Process = ISP_Algo_AEC_Process,
};

/* Simple AWB algorithm handle */
ISP_AlgoTypeDef ISP_Algo_AWB = {
    .id = ISP_ALGO_ID_AWB,
    .Init = ISP_Algo_AWB_Init,
    .DeInit = ISP_Algo_AWB_DeInit,
    .Process = ISP_Algo_AWB_Process,
};

/* Simple AWB CCT algorithm handle */
ISP_AlgoTypeDef ISP_Algo_SimpleAWB_CCT = {
    .id = ISP_ALGO_ID_SIMPLEAWB_CCT,
    .Init = ISP_Algo_SimpleAWB_CCT_Init,
    .DeInit = ISP_Algo_SimpleAWB_CCT_DeInit,
    .Process = ISP_Algo_SimpleAWB_CCT_Process,
};

#ifdef ALGO_PERF_DBG_LOGS
#define MEAS_ITERATION 10
uint32_t tickstart;
uint32_t duration[MEAS_ITERATION];
uint32_t iter = 0;
#endif

/* Registered algorithm list */
ISP_AlgoTypeDef *ISP_Algo_List[] = {
    &ISP_Algo_BadPixel,
    &ISP_Algo_BlackLevel,
    &ISP_Algo_SimpleAWB,
    &ISP_Algo_SimpleAEC,
    &ISP_Algo_AEC,
    &ISP_Algo_AWB,
    &ISP_Algo_SimpleAWB_CCT,
};

/* Algo internal */
static evision_ae_estimator_t *pIspAECestimator;
static evision_awb_estimator_t* pIspAWBestimator;

static uint32_t evision_ae_lut_exposure [EVISION_AE_LUT_EXPOSURE_SIZE];
static uint32_t evision_ae_lut_gain [EVISION_AE_LUT_GAIN_SIZE];

/* Global variables ----------------------------------------------------------*/
uint32_t current_awb_profId = 0;

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  ISP_Algo_BadPixel_Init
  *         Initialize the BadPixel algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_BadPixel_Init(void *hIsp, void *pAlgo)
{
  (void)hIsp; /* unused */

  ((ISP_AlgoTypeDef *)pAlgo)->state = ISP_ALGO_STATE_INIT;

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_BadPixel_DeInit
  *         Deinitialize the BadPixel algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_BadPixel_DeInit(void *hIsp, void *pAlgo)
{
  (void)hIsp; /* unused */
  (void)pAlgo; /* unused */

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_BadPixel_Process
  *         Process the BadPixel algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_BadPixel_Process(void *hIsp, void *pAlgo)
{
  (void)hIsp; /* unused */
  (void)pAlgo; /* unused */
  static uint32_t BadPixelCount;
  static int8_t Step;
  ISP_BadPixelTypeDef BadPixelConfig;
  ISP_IQParamTypeDef *IQParamConfig;
  ISP_StatusTypeDef ret;

  IQParamConfig = ISP_SVC_IQParam_Get(hIsp);

  if (IQParamConfig->badPixelAlgo.enable == false)
  {
    return ISP_OK;
  }

  if (Step++ >= 0)
  {
    /* Measure the number of bad pixels */
    ret  = ISP_SVC_ISP_GetBadPixel(hIsp, &BadPixelConfig);
    if (ret != ISP_OK)
    {
      return ret;
    }
    BadPixelCount += BadPixelConfig.count;
  }

  if (Step == 10)
  {
    /* All measures done : make an average and compare with threshold */
    BadPixelCount /= 10;

    if ((BadPixelCount > IQParamConfig->badPixelAlgo.threshold) && (BadPixelConfig.strength > 0))
    {
      /* Bad pixel is above target : decrease strength */
      BadPixelConfig.strength--;
    }
    else if ((BadPixelCount < IQParamConfig->badPixelAlgo.threshold) && (BadPixelConfig.strength < ISP_BADPIXEL_STRENGTH_MAX - 1))
    {
      /* Bad pixel is below target : increase strength. (exclude ISP_BADPIXEL_STRENGTH_MAX which gives weird results) */
      BadPixelConfig.strength++;
    }

    /* Set updated Strength */
    BadPixelConfig.enable = 1;
    ret = ISP_SVC_ISP_SetBadPixel(hIsp, &BadPixelConfig);
    if (ret != ISP_OK)
    {
      return ret;
    }

    /* Set Step to -1 to wait for an extra frame before a new measurement (the ISP HW needs one frame to update after reconfig) */
    Step = -1;
    BadPixelCount = 0;
  }

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_BlackLevel_Init
  *         Initialize the BlackLevel algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_BlackLevel_Init(void *hIsp, void *pAlgo)
{
  (void)hIsp; /* unused */

  ((ISP_AlgoTypeDef *)pAlgo)->state = ISP_ALGO_STATE_INIT;

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_BlackLevel_DeInit
  *         Deinitialize the BlackLevel algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_BlackLevel_DeInit(void *hIsp, void *pAlgo)
{
  (void)hIsp; /* unused */
  (void)pAlgo; /* unused */

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_BlackLevel_Process
  *         Process the BlackLevel algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_BlackLevel_Process(void *hIsp, void *pAlgo)
{
  (void)pAlgo; /* unused */
  static int8_t BLCR_current, BLCG_current, BLCB_current;
  ISP_SensorGainTypeDef Gain;
  uint32_t GainDiff, MinGainDiff;
  uint8_t i, i_ref;
  ISP_BlackLevelTypeDef BL_ref;
  ISP_IQParamTypeDef *IQParamConfig;
  ISP_StatusTypeDef ret;

  IQParamConfig = ISP_SVC_IQParam_Get(hIsp);

  if (IQParamConfig->blackLevelAlgo.enable == false)
  {
    return ISP_OK;
  }

  /* The Black Level values depend on the sensor gain : read its value */
  ret = ISP_SVC_Sensor_GetGain(hIsp, &Gain);
  if (ret != ISP_OK)
  {
    return ret;
  }

  /* Search for the closest ReferenceGain identified by i_ref */
  i_ref = 0;
  MinGainDiff = UINT_MAX;
  for (i = 0; i < ISP_BLACKLEVEL_GAIN_REF; i++)
  {
    GainDiff = abs((int)(Gain.gain - IQParamConfig->blackLevelAlgo.referenceGain[i]));
    if (GainDiff < MinGainDiff)
    {
      i_ref = i;
      MinGainDiff = GainDiff;
    }
  }

  /* Check for Black Level value update */
  BL_ref.BLCR = IQParamConfig->blackLevelAlgo.BLCR[i_ref];
  BL_ref.BLCG = IQParamConfig->blackLevelAlgo.BLCG[i_ref];
  BL_ref.BLCB = IQParamConfig->blackLevelAlgo.BLCB[i_ref];
  if ((BL_ref.BLCR != BLCR_current) || (BL_ref.BLCG != BLCG_current) || (BL_ref.BLCB != BLCB_current))
  {
    /* Apply new Black Level values */
    BL_ref.enable = 1;
    ret = ISP_SVC_ISP_SetBlackLevel(hIsp, &BL_ref);
    if (ret != ISP_OK)
    {
      return ret;
    }

    BLCR_current = BL_ref.BLCR;
    BLCG_current = BL_ref.BLCG;
    BLCB_current = BL_ref.BLCB;
  }

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_SimpleAWB_Init
  *         Initialize the SimpleAWB algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_SimpleAWB_Init(void *hIsp, void *pAlgo)
{
  ISP_AlgoTypeDef *algo = (ISP_AlgoTypeDef *)pAlgo;
  ISP_IQParamTypeDef *IQParamConfig;
  ISP_ISPGainTypeDef gainConfig;
  ISP_StatusTypeDef ret;

  // TODO: find a better fix. Disable simple AWB
  return ISP_OK;

  IQParamConfig = ISP_SVC_IQParam_Get(hIsp);
  if (IQParamConfig->AWBAlgo.enable == false)
  {
    return ISP_OK;
  }

  /* Enable ISP Gain with an initial x1.0 gain */
  gainConfig.enable = 1;
  gainConfig.ispGainR = 1 * ISP_GAIN_PRECISION_FACTOR;
  gainConfig.ispGainG = 1 * ISP_GAIN_PRECISION_FACTOR;
  gainConfig.ispGainB = 1 * ISP_GAIN_PRECISION_FACTOR;
  ret = ISP_SVC_ISP_SetGain(hIsp, &gainConfig);

  /* Update State */
  algo->state = ISP_ALGO_STATE_INIT;

  return ret;
}

/**
  * @brief  ISP_Algo_SimpleAWB_DeInit
  *         Deinitialize the SimpleAWB algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_SimpleAWB_DeInit(void *hIsp, void *pAlgo)
{
  (void)pAlgo; /* unused */
  ISP_IQParamTypeDef *IQParamConfig;
  ISP_ISPGainTypeDef gainConfig;
  ISP_StatusTypeDef ret;

  IQParamConfig = ISP_SVC_IQParam_Get(hIsp);
  if (IQParamConfig->AWBAlgo.enable == false)
  {
    return ISP_OK;
  }

  /* Disable ISP Gain */
  gainConfig.enable = 0;
  ret = ISP_SVC_ISP_SetGain(hIsp, &gainConfig);

  return ret;
}

/**
  * @brief  ISP_Algo_SimpleAWB_StatCb
  *         Callback informing that statistics are available
  * @param  pAlgo: ISP algorithm handle.
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_SimpleAWB_StatCb(ISP_AlgoTypeDef *pAlgo)
{
  /* Update State */
  pAlgo->state = ISP_ALGO_STATE_STAT_READY;

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_SimpleAWB_Process
  *         Process the SimpleAWB algorithm. This basic algorithm controls the ISP gain in order
  *         to have all the RGB components equals and without modifying the overall gain.
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_SimpleAWB_Process(void *hIsp, void *pAlgo)
{
  static ISP_SVC_StatStateTypeDef stats;
  ISP_AlgoTypeDef *algo = (ISP_AlgoTypeDef *)pAlgo;
  ISP_IQParamTypeDef *IQParamConfig;
  ISP_ISPGainTypeDef gainConfig;
  ISP_StatusTypeDef ret = ISP_OK;
  uint64_t globalGain;
  uint64_t val;

  // TODO: find a better fix. Disable simple AWB
  return ISP_OK;

  IQParamConfig = ISP_SVC_IQParam_Get(hIsp);
  if (IQParamConfig->AWBAlgo.enable == false)
  {
    return ISP_OK;
  }

  switch(algo->state)
  {
  case ISP_ALGO_STATE_INIT:
  case ISP_ALGO_STATE_NEED_STAT:
    /* Ask for stats */
    ret = ISP_SVC_Stats_GetNext(hIsp, &ISP_Algo_SimpleAWB_StatCb, pAlgo, &stats, ISP_STAT_LOC_DOWN, ISP_STAT_TYPE_AVG, 0);
    if (ret != ISP_OK)
    {
      return ret;
    }

    /* Wait for stats to be ready */
    algo->state = ISP_ALGO_STATE_WAITING_STAT;
    break;

  case ISP_ALGO_STATE_WAITING_STAT:
    /* Do nothing */
    break;

  case ISP_ALGO_STATE_STAT_READY:
    if ((stats.down.averageR != 0) && (stats.down.averageG != 0) && (stats.down.averageB != 0))
    {
      /* Read the current ISP gain */
      if (ISP_SVC_ISP_GetGain(hIsp, &gainConfig) == ISP_OK)
      {
        if (gainConfig.enable == 0)
        {
          /* In case the ISP gain was not set yet (enabled during runtime), assume a default value of x1.0 */
          gainConfig.ispGainR = 1 * ISP_GAIN_PRECISION_FACTOR;
          gainConfig.ispGainG = 1 * ISP_GAIN_PRECISION_FACTOR;
          gainConfig.ispGainB = 1 * ISP_GAIN_PRECISION_FACTOR;
        }

        /* Compute the ISP gain to reach 128 for all components */
        gainConfig.enable = 1;
        val = gainConfig.ispGainR;
        val = val * 128 / stats.down.averageR;
        gainConfig.ispGainR = val;
        val = gainConfig.ispGainG;
        val = val * 128 / stats.down.averageG;
        gainConfig.ispGainG = val;
        val = gainConfig.ispGainB;
        val = val * 128 / stats.down.averageB;
        gainConfig.ispGainB = val;

        /* Update component gains so the overall gain equals x1.0 */
        globalGain = (gainConfig.ispGainR + 2 * gainConfig.ispGainG + gainConfig.ispGainB) / 4;
        val = gainConfig.ispGainR;
        val = val * ISP_GAIN_PRECISION_FACTOR / globalGain;
        gainConfig.ispGainR = val;
        val = gainConfig.ispGainG;
        val = val * ISP_GAIN_PRECISION_FACTOR / globalGain;
        gainConfig.ispGainG = val;
        val = gainConfig.ispGainB;
        val = val * ISP_GAIN_PRECISION_FACTOR / globalGain;
        gainConfig.ispGainB = val;

        ISP_SVC_ISP_SetGain(hIsp, &gainConfig);
      }
    }

    /* Ask for stats */
    ret = ISP_SVC_Stats_GetNext(hIsp, &ISP_Algo_SimpleAWB_StatCb, pAlgo, &stats, ISP_STAT_LOC_DOWN,
                                ISP_STAT_TYPE_AVG, ALGO_ISP_VSYNC_LATENCY);

    /* Wait for stats to be ready */
    algo->state = ISP_ALGO_STATE_WAITING_STAT;
    break;
  }

  return ret;
}

/**
  * @brief  ISP_Algo_SimpleAEC_Init
  *         Initialize the SimpleAEC algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_SimpleAEC_Init(void *hIsp, void *pAlgo)
{
  (void)hIsp; /* unused */
  ISP_AlgoTypeDef *algo = (ISP_AlgoTypeDef *)pAlgo;

  /* Update State */
  algo->state = ISP_ALGO_STATE_INIT;

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_SimpleAEC_DeInit
  *         Deinitialize the SimpleAEC algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_SimpleAEC_DeInit(void *hIsp, void *pAlgo)
{
  (void)hIsp; /* unused */
  (void)pAlgo; /* unused */

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_SimpleAEC_StatCb
  *         Callback informing that statistics are available
  * @param  pAlgo: ISP algorithm handle.
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_SimpleAEC_StatCb(ISP_AlgoTypeDef *pAlgo)
{
  /* Update State */
  pAlgo->state = ISP_ALGO_STATE_STAT_READY;

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_SimpleAEC_Process
  *         Process the SimpleAEC algorithm. This basic algorithm controls the sensor gain
  *         in order to reach an average luminance of exposureTarget.
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_SimpleAEC_Process(void *hIsp, void *pAlgo)
{
  static ISP_SVC_StatStateTypeDef stats;
  ISP_AlgoTypeDef *algo = (ISP_AlgoTypeDef *)pAlgo;
  ISP_IQParamTypeDef *IQParamConfig;
  ISP_StatusTypeDef ret = ISP_OK;
  ISP_SensorGainTypeDef gainConfig;
  uint32_t avgL;
  int32_t gainUpdate = 0;

  // TODO: find a better fix. Disable simple AEC
  return ISP_OK;

  IQParamConfig = ISP_SVC_IQParam_Get(hIsp);
  if (IQParamConfig->AECAlgo.enable == false)
  {
    return ISP_OK;
  }

  switch(algo->state)
  {
  case ISP_ALGO_STATE_INIT:
  case ISP_ALGO_STATE_NEED_STAT:
    /* Ask for stats */
    ret = ISP_SVC_Stats_GetNext(hIsp, &ISP_Algo_SimpleAEC_StatCb, pAlgo, &stats, ISP_STAT_LOC_UP, ISP_STAT_TYPE_AVG, 0);
    if (ret != ISP_OK)
    {
      return ret;
    }

    /* Wait for stats to be ready */
    algo->state = ISP_ALGO_STATE_WAITING_STAT;
    break;

  case ISP_ALGO_STATE_WAITING_STAT:
    /* Do nothing */
    break;

  case ISP_ALGO_STATE_STAT_READY:
    avgL = stats.up.averageL;
    if (avgL != 0)
    {
      /* Read the current sensor gain */
      if (ISP_SVC_Sensor_GetGain(hIsp, &gainConfig) == ISP_OK)
      {
        /* Compare the average luminance with the exposureTarget */
        if (avgL > IQParamConfig->AECAlgo.exposureTarget + AEC_TOLERANCE)
        {
          /* Too bright, decrease gain */
          gainUpdate = (int32_t)(IQParamConfig->AECAlgo.exposureTarget - avgL) * AEC_COEFF_LUM_GAIN;
          if (gainUpdate < -AEC_GAIN_UPDATE_MAX)
          {
            gainUpdate = -AEC_GAIN_UPDATE_MAX;
          }
        }
        else if (avgL < IQParamConfig->AECAlgo.exposureTarget - AEC_TOLERANCE)
        {
          /* Too dark vador, call a Jedi and increase gain */
          gainUpdate = (int32_t)(IQParamConfig->AECAlgo.exposureTarget - avgL) * AEC_COEFF_LUM_GAIN;
          if (gainUpdate > AEC_GAIN_UPDATE_MAX)
          {
            gainUpdate = AEC_GAIN_UPDATE_MAX;
          }
        }

        if (gainUpdate != 0)
        {
          /* Update sensor gain */
          if ((gainUpdate > 0) || ((gainUpdate < 0) && (gainConfig.gain > -gainUpdate)))
          {
            gainConfig.gain += gainUpdate;
          }
          else
          {
            gainConfig.gain = 0;
          }
          ISP_SVC_Sensor_SetGain(hIsp, &gainConfig);
        }
      }
    }

    /* Ask for stats */
    ret = ISP_SVC_Stats_GetNext(hIsp, &ISP_Algo_SimpleAEC_StatCb, pAlgo, &stats,
                                ISP_STAT_LOC_UP, ISP_STAT_TYPE_AVG, ALGO_ISP_SENSOR_VSYNC_LATENCY);
    /* Wait for stats to be ready */
    algo->state = ISP_ALGO_STATE_WAITING_STAT;
    break;
  }

  return ret;
}

/**
  * @brief  ISP_Algo_ApplyGammaInverse
  *         Apply Gamma 1/2.2 correction to a component value
  * @param  hIsp:  ISP device handle.
  * @param  comp: component value
  * @retval gamma corrected value
  */
double ISP_Algo_ApplyGammaInverse(ISP_HandleTypeDef *hIsp, uint32_t comp)
{
  double out;

  /* Check if gamma is enabled */
  if (ISP_SVC_Misc_IsGammaEnabled(hIsp, 1 /*main pipe*/) != 0) {
    out = 255 * pow((float)comp / 255, 1.0 / 2.2);
  }
  else
  {
    out = (double) comp;
  }
  return out;
}

/**
  * @brief  ISP_Algo_ApplyCConv
  *         Apply Color Conversion matrix to RGB components, clamping output values to [0-255]
  * @param  hIsp:  ISP device handle.
  * @param  inR: Red component value
  * @param  inG: Green component value
  * @param  inB: Blue component value
  * @param  outR: pointer to Red component value after color conversion
  * @param  outG: pointer to Green component value after color conversion
  * @param  outB: pointer to Blue component value after color conversion
  * @retval None
  */
void ISP_Algo_ApplyCConv(ISP_HandleTypeDef *hIsp, uint32_t inR, uint32_t inG, uint32_t inB, uint32_t *outR, uint32_t *outG, uint32_t *outB)
{
  ISP_ColorConvTypeDef colorConv;
  int64_t ccR, ccG, ccB;

  if ((ISP_SVC_ISP_GetColorConv(hIsp, &colorConv) == ISP_OK) && (colorConv.enable == 1))
  {
    /* Apply ColorConversion matrix to the input components */
    ccR = (int64_t) inR * colorConv.coeff[0][0] + (int64_t) inG * colorConv.coeff[0][1] + (int64_t) inB * colorConv.coeff[0][2];
    ccG = (int64_t) inR * colorConv.coeff[1][0] + (int64_t) inG * colorConv.coeff[1][1] + (int64_t) inB * colorConv.coeff[1][2];
    ccB = (int64_t) inR * colorConv.coeff[2][0] + (int64_t) inG * colorConv.coeff[2][1] + (int64_t) inB * colorConv.coeff[2][2];

    ccR /= ISP_CCM_PRECISION_FACTOR;
    ccG /= ISP_CCM_PRECISION_FACTOR;
    ccB /= ISP_CCM_PRECISION_FACTOR;

    /* Clamp values to 0-255 */
    ccR = (ccR < 0) ? 0 : (ccR > 255) ? 255 : ccR;
    ccG = (ccG < 0) ? 0 : (ccG > 255) ? 255 : ccG;
    ccB = (ccB < 0) ? 0 : (ccB > 255) ? 255 : ccB;

    *outR = (uint32_t) ccR;
    *outG = (uint32_t) ccG;
    *outB = (uint32_t) ccB;
  }
  else
  {
    *outR = inR;
    *outG = inG;
    *outB = inB;
  }
}

/**
  * @brief  ISP_Algo_AEC_Init
  *         Initialize the AEC algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_AEC_Init(void *hIsp, void *pAlgo)
{
  ISP_AlgoTypeDef *algo = (ISP_AlgoTypeDef *)pAlgo;
  ISP_IQParamTypeDef *IQParamConfig;
  evision_return_t e_ret;
  ISP_StatusTypeDef ret = ISP_OK;
  ISP_SensorGainTypeDef gainConfig;
  ISP_SensorExposureTypeDef exposureConfig;
  uint32_t exposure_lut_sizes[EVISION_AE_MAX_SENSOR_CONFIGS] = {
    EVISION_AE_LUT_EXPOSURE_SIZE,
    EVISION_AE_LUT_EXPOSURE_SIZE
  };
  uint32_t* exposure_luts[EVISION_AE_MAX_SENSOR_CONFIGS] = {
    evision_ae_lut_exposure,
    evision_ae_lut_exposure
  };
  uint32_t gain_lut_full_sizes[EVISION_AE_MAX_SENSOR_CONFIGS] = {
    EVISION_AE_LUT_GAIN_SIZE,
    EVISION_AE_LUT_GAIN_SIZE
  };
  uint32_t* gain_luts[EVISION_AE_MAX_SENSOR_CONFIGS] = {
    evision_ae_lut_gain,
    evision_ae_lut_gain
  };
  ISP_SensorInfoTypeDef SensorInfo = {0};
  uint32_t idx;

  IQParamConfig = ISP_SVC_IQParam_Get(hIsp);

  /* Get Sensor Info */
  ret = ISP_SVC_Sensor_GetInfo(hIsp, &SensorInfo);
  if (ret != ISP_OK)
  {
    return ret;
  }

  /* Fill lookup tables */
  for (idx = 0; idx < EVISION_AE_LUT_EXPOSURE_SIZE; idx++)
  {
    evision_ae_lut_exposure[idx] = SensorInfo.exposure_min + idx * (SensorInfo.exposure_max - SensorInfo.exposure_min) / (EVISION_AE_LUT_EXPOSURE_SIZE - 1);
  }

  for (idx = 0; idx < EVISION_AE_LUT_GAIN_SIZE; idx++)
  {
    evision_ae_lut_gain[idx] = SensorInfo.gain_min + idx * (SensorInfo.gain_max - SensorInfo.gain_min) / (EVISION_AE_LUT_GAIN_SIZE - 1);
  }

  /* Create estimator */
  pIspAECestimator = evision_api_ae_new();
  if (pIspAECestimator == NULL)
  {
    return ISP_ERR_ALGO;
  }

  /* Initialize estimator */
  e_ret = evision_api_ae_init(pIspAECestimator,
                              exposure_lut_sizes,
                              (const uint32_t**)exposure_luts,
                              gain_lut_full_sizes,
                              (const uint32_t**)gain_luts);
  if (e_ret != EVISION_RET_SUCCESS)
  {
    evision_api_ae_delete(pIspAECestimator);
    return ISP_ERR_ALGO;
  }

  /* Configure algo (AEC target) */
  pIspAECestimator->hyper_params.desired_luminosity = IQParamConfig->AECAlgo.exposureTarget;

  /* Configure algo (sensor config) */
  pIspAECestimator->active_sensor_cfg = &pIspAECestimator->sensor_configs[EVISION_AE_DEFAULT_SENSOR_CONFIG];
  pIspAECestimator->hyper_params.exposure_min = pIspAECestimator->active_sensor_cfg->lut_exposure[0u];
  pIspAECestimator->hyper_params.exposure_max = pIspAECestimator->active_sensor_cfg->lut_exposure[pIspAECestimator->active_sensor_cfg->lut_exposure_size - 1u];
  pIspAECestimator->hyper_params.gain_min = pIspAECestimator->active_sensor_cfg->full_lut_gain[0u];
  pIspAECestimator->hyper_params.gain_max = pIspAECestimator->active_sensor_cfg->full_lut_gain[pIspAECestimator->active_sensor_cfg->full_lut_gain_size - 1u];

  /* Initialize exposure and gain at min value */
  pIspAECestimator->exposure = pIspAECestimator->active_sensor_cfg->lut_exposure[0];
  pIspAECestimator->gain = pIspAECestimator->active_sensor_cfg->full_lut_gain[0];
  exposureConfig.exposure = (uint32_t)pIspAECestimator->exposure;
  gainConfig.gain = pIspAECestimator->gain;

  if (IQParamConfig->AECAlgo.enable == true)
  {
    ret = ISP_SVC_Sensor_SetExposure(hIsp, &exposureConfig);
    if (ret == ISP_OK)
    {
      ret = ISP_SVC_Sensor_SetGain(hIsp, &gainConfig);
    }
  }

  /* Update State */
  algo->state = ISP_ALGO_STATE_INIT;

  return ret;
}

/**
  * @brief  ISP_Algo_AEC_DeInit
  *         Deinitialize the AEC algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_AEC_DeInit(void *hIsp, void *pAlgo)
{
  (void)hIsp; /* unused */
  (void)pAlgo; /* unused */

  if (pIspAECestimator != NULL)
  {
    evision_api_ae_delete(pIspAECestimator);
  }

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_AEC_StatCb
  *         Callback informing that statistics are available
  * @param  pAlgo: ISP algorithm handle.
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_AEC_StatCb(ISP_AlgoTypeDef *pAlgo)
{
  /* Update State */
  pAlgo->state = ISP_ALGO_STATE_STAT_READY;

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_AEC_Process
  *         Process the AEC algorithm. This algorithm controls the sensor exposure time and gain
  *         in order to reach a targeted luminance.
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_AEC_Process(void *hIsp, void *pAlgo)
{
  static ISP_SVC_StatStateTypeDef stats;
  ISP_AlgoTypeDef *algo = (ISP_AlgoTypeDef *)pAlgo;
  ISP_IQParamTypeDef *IQParamConfig;
  evision_return_t e_ret;
  ISP_StatusTypeDef ret_stat, ret = ISP_OK;
  ISP_SensorGainTypeDef gainConfig;
  ISP_SensorExposureTypeDef exposureConfig;
  double ccAvgL;
  uint32_t index_max, index_current, index_max_step;
#ifdef ALGO_AEC_DBG_LOGS
  static double currentL;
#endif

  IQParamConfig = ISP_SVC_IQParam_Get(hIsp);

  if (IQParamConfig->AECAlgo.enable == false)
  {
    return ISP_OK;
  }
  else if (algo->state != ISP_ALGO_STATE_INIT)
  {
    /* Restore sensor exposure and gain, if it has been changed by user or application */
    ret = ISP_SVC_Sensor_GetGain(hIsp, &gainConfig);

    if ((ret == ISP_OK) && (gainConfig.gain != pIspAECestimator->gain))
    {
      gainConfig.gain = pIspAECestimator->gain;
      ret = ISP_SVC_Sensor_SetGain(hIsp, &gainConfig);
    }

    if (ret != ISP_OK)
    {
      return ret;
    }

    ret = ISP_SVC_Sensor_GetExposure(hIsp, &exposureConfig);

    if ((ret == ISP_OK) && (exposureConfig.exposure != pIspAECestimator->exposure))
    {
      exposureConfig.exposure = (uint32_t) pIspAECestimator->exposure;
      ret = ISP_SVC_Sensor_SetExposure(hIsp, &exposureConfig);
    }

    if (ret != ISP_OK)
    {
      return ret;
    }
  }

  switch(algo->state)
  {
  case ISP_ALGO_STATE_INIT:
    /* Upon estimator first run, it is expected that the sensor has min values of sensor gain & exposure, so with a Luminance of 0 */
    memset(&stats, 0, sizeof(stats));

    /* Claim that stats are ready */
    algo->state = ISP_ALGO_STATE_STAT_READY;
    break;

  case ISP_ALGO_STATE_NEED_STAT:
    /* Ask for stats */
    ret = ISP_SVC_Stats_GetNext(hIsp, &ISP_Algo_AEC_StatCb, pAlgo, &stats, ISP_STAT_LOC_DOWN,
                                ISP_STAT_TYPE_AVG, ALGO_ISP_SENSOR_VSYNC_LATENCY);
    if (ret != ISP_OK)
    {
      return ret;
    }

    /* Wait for stats to be ready */
    algo->state = ISP_ALGO_STATE_WAITING_STAT;
    break;

  case ISP_ALGO_STATE_WAITING_STAT:
    /* Do nothing */
    break;

  case ISP_ALGO_STATE_STAT_READY:
    /* Align on the target update (may have been updated with ISP_SetExposureTarget()) */
    pIspAECestimator->hyper_params.desired_luminosity = IQParamConfig->AECAlgo.exposureTarget;
    ccAvgL = (double)stats.down.averageL;
#ifdef ALGO_AEC_DBG_LOGS
    if (ccAvgL != currentL)
    {
      printf("L = %ld\r\n", (uint32_t) ccAvgL);
      currentL = ccAvgL;
    }
#endif

    /* Run algo to estimate exposure and gain to apply */
    e_ret = evision_api_ae_run_average(pIspAECestimator, NULL, 1, ccAvgL);
    if (e_ret == EVISION_RET_SUCCESS)
    {
      ret = ISP_SVC_Sensor_GetGain(hIsp, &gainConfig);

      if ((ret == ISP_OK) && (gainConfig.gain != pIspAECestimator->gain))
      {
        /* Set new gain */
#ifdef ALGO_AEC_DBG_LOGS
        printf("New gain = %ld\r\n", pIspAECestimator->gain);
#endif
        gainConfig.gain = pIspAECestimator->gain;
        ret = ISP_SVC_Sensor_SetGain(hIsp, &gainConfig);
      }

      if (ret != ISP_OK)
      {
        return ret;
      }

      ret = ISP_SVC_Sensor_GetExposure(hIsp, &exposureConfig);

      if ((ret == ISP_OK) && (exposureConfig.exposure != pIspAECestimator->exposure))
      {
        /* Set new exposure */
#ifdef ALGO_AEC_DBG_LOGS
        printf("New Exposure = %ld\r\n", (uint32_t)pIspAECestimator->exposure);
#endif
        exposureConfig.exposure = (uint32_t) pIspAECestimator->exposure;
        ret = ISP_SVC_Sensor_SetExposure(hIsp, &exposureConfig);
      }

      if (ret != ISP_OK)
      {
        return ret;
      }

      /* Limit to 6 dB steps (when the algo accepts that constraint) */
      /* TODO: since this is specific to IMX335, it shall be reworked to be generic */
      index_max = pIspAECestimator->active_sensor_cfg->full_lut_gain_size;
      index_current = pIspAECestimator->runtime_vars.index_gain;
      index_max_step = EVISION_AE_LUT_GAIN_SIZE / 10;
      pIspAECestimator->hyper_params.index_diff_gain_max = index_max - (index_current + index_max_step);
      if (index_current > index_max_step)
      {
        pIspAECestimator->hyper_params.index_diff_gain_min = index_current - index_max_step;
      }
      else
      {
        pIspAECestimator->hyper_params.index_diff_gain_min = 0;
      }
    }
    else
    {
      ret = ISP_ERR_ALGO;
    }

    /* Ask for stats */
    ret_stat = ISP_SVC_Stats_GetNext(hIsp, &ISP_Algo_AEC_StatCb, pAlgo, &stats, ISP_STAT_LOC_DOWN,
                                     ISP_STAT_TYPE_AVG, ALGO_ISP_SENSOR_VSYNC_LATENCY);
    ret = (ret != ISP_OK) ? ret : ret_stat;

    /* Wait for stats to be ready */
    algo->state = ISP_ALGO_STATE_WAITING_STAT;
    break;
  }

  return ret;
}

/**
  * @brief  ISP_Algo_AWB_Init
  *         Initialize the AWB algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_AWB_Init(void *hIsp, void *pAlgo)
{
  (void)hIsp; /* unused */
  ISP_AlgoTypeDef *algo = (ISP_AlgoTypeDef *)pAlgo;

  /* Create estimator */
  pIspAWBestimator = evision_api_awb_new();
  if (pIspAWBestimator == NULL)
  {
    return ISP_ERR_ALGO;
  }

  /* Continue the initialization in ISP_Algo_AWB_Process() function when state is ISP_ALGO_STATE_INIT.
   * This allows to read the IQ params after an algo stop/start cycle */
  algo->state = ISP_ALGO_STATE_INIT;

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_AWB_DeInit
  *         Deinitialize the AWB algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_AWB_DeInit(void *hIsp, void *pAlgo)
{
  (void)hIsp; /* unused */
  (void)pAlgo; /* unused */

  if (pIspAWBestimator != NULL)
  {
    evision_api_awb_delete(pIspAWBestimator);
  }

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_AWB_StatCb
  *         Callback informing that statistics are available
  * @param  pAlgo: ISP algorithm handle.
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_AWB_StatCb(ISP_AlgoTypeDef *pAlgo)
{
  /* Update State */
  if (pAlgo->state != ISP_ALGO_STATE_INIT)
  {
    pAlgo->state = ISP_ALGO_STATE_STAT_READY;
  }

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_AWB_Process
  *         Process the AWB algorithm. This algorithm controls the ISP gain and color conversion
  *         in order to output realistic colors (white balance).
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_AWB_Process(void *hIsp, void *pAlgo)
{
  static ISP_SVC_StatStateTypeDef stats;
  static uint8_t enableCurrent = false;
  static uint8_t reconfigureRequest = false;
  static uint32_t currentColorTemp = 0;
  static evision_awb_profile_t awbProfiles[ISP_AWB_COLORTEMP_REF];
  static float colorTempThresholds[ISP_AWB_COLORTEMP_REF - 1];
  ISP_IQParamTypeDef *IQParamConfig;
  ISP_ColorConvTypeDef ColorConvConfig;
  ISP_ISPGainTypeDef ISPGainConfig;
  ISP_AlgoTypeDef *algo = (ISP_AlgoTypeDef *)pAlgo;
  ISP_StatusTypeDef ret_stat, ret = ISP_OK;
  evision_return_t e_ret;
  uint32_t ccAvgR, ccAvgG, ccAvgB, colorTemp, i, j, profId, profNb;
  float cfaGains[4], ccmCoeffs[3][3], ccmOffsets[3] = { 0 };
  double meas[3];

  IQParamConfig = ISP_SVC_IQParam_Get(hIsp);

  if (IQParamConfig->AWBAlgo.enable == false)
  {
    enableCurrent = false;
    return ISP_OK;
  }
  else if ((enableCurrent == false) || (IQParamConfig->AWBAlgo.enable == ISP_AWB_ENABLE_RECONFIGURE))
  {
    /* Start or resume algo : set state to INIT in order to read the IQ params */
    algo->state = ISP_ALGO_STATE_INIT;
    IQParamConfig->AWBAlgo.enable = true;
    reconfigureRequest = true;
    enableCurrent = true;
  }

  switch(algo->state)
  {
  case ISP_ALGO_STATE_INIT:
    /* Set profiles (color temperature, gains, color conv matrix) */
    profNb = 0;
    for (profId = 0; profId < ISP_AWB_COLORTEMP_REF && profId < EVISION_AWB_MAX_PROFILE_COUNT; profId++)
    {
      colorTemp = (float) IQParamConfig->AWBAlgo.referenceColorTemp[profId];
      if (colorTemp == 0)
        break;

      if (profNb > 0)
      {
        /* Profile decision threshold = average of two reference temperatures */
        colorTempThresholds[profNb - 1] = (float) ((colorTemp + IQParamConfig->AWBAlgo.referenceColorTemp[profId - 1]) /2 );

        /* Improve D65 detection : set D50/D65 threshold to 5500 */
        if ((colorTemp == 6500) && (IQParamConfig->AWBAlgo.referenceColorTemp[profId - 1] == 5000))
        {
          colorTempThresholds[profNb - 1] = 5300;
        }
      }

      /* Set cfa gains (RGGB) */
      cfaGains[0] = (float) IQParamConfig->AWBAlgo.ispGainR[profId] / ISP_GAIN_PRECISION_FACTOR;
      cfaGains[1] = (float) IQParamConfig->AWBAlgo.ispGainG[profId] / ISP_GAIN_PRECISION_FACTOR;
      cfaGains[2] = cfaGains[1];
      cfaGains[3] = (float) IQParamConfig->AWBAlgo.ispGainB[profId] / ISP_GAIN_PRECISION_FACTOR;

      /* Set CCM Coeff */
      for (i = 0; i < 3; i++)
      {
        for (j = 0; j < 3; j++)
        {
          ccmCoeffs[i][j] = (float) IQParamConfig->AWBAlgo.coeff[profId][i][j] / ISP_CCM_PRECISION_FACTOR;
        }
      }

      /* Set profile */
      evision_api_awb_set_profile(&awbProfiles[profId], (float) colorTemp, cfaGains, ccmCoeffs, ccmOffsets);
      profNb++;
    }

    if (profNb == 0)
    {
      return ISP_ERR_WB_COLORTEMP;
    }

    /* Register profiles */
    e_ret = evision_api_awb_init_profiles(pIspAWBestimator, (double) IQParamConfig->AWBAlgo.referenceColorTemp[0],
                                          (double) IQParamConfig->AWBAlgo.referenceColorTemp[profNb - 1], profNb,
                                          colorTempThresholds, awbProfiles);
    if (e_ret != EVISION_RET_SUCCESS)
    {
      return ISP_ERR_ALGO;
    }

    /* Configure algo */
    /* TODO: check if this shall be an IQ tuning parameter (like the LUT tables of AE algo) */
    pIspAWBestimator->hyper_params.speed_p_min = 1.35;
    pIspAWBestimator->hyper_params.speed_p_max = 2.0;
    pIspAWBestimator->hyper_params.gm_tolerance = 1;
    pIspAWBestimator->hyper_params.conv_criterion = 3;

    /* Ask for stats */
    ret = ISP_SVC_Stats_GetNext(hIsp, &ISP_Algo_AWB_StatCb, pAlgo, &stats, ISP_STAT_LOC_DOWN,
                                ISP_STAT_TYPE_AVG, ALGO_ISP_VSYNC_LATENCY);
    if (ret != ISP_OK)
    {
      return ret;
    }

    /* Wait for stats to be ready */
    algo->state = ISP_ALGO_STATE_WAITING_STAT;
    break;

  case ISP_ALGO_STATE_NEED_STAT:
    ret = ISP_SVC_Stats_GetNext(hIsp, &ISP_Algo_AWB_StatCb, pAlgo, &stats, ISP_STAT_LOC_DOWN,
                                ISP_STAT_TYPE_AVG, ALGO_ISP_VSYNC_LATENCY);
    if (ret != ISP_OK)
    {
      return ret;
    }

    /* Wait for stats to be ready */
    algo->state = ISP_ALGO_STATE_WAITING_STAT;
    break;

  case ISP_ALGO_STATE_WAITING_STAT:
    /* Do nothing */
    break;

  case ISP_ALGO_STATE_STAT_READY:
    /* Get stats after color conversion */
    ISP_Algo_ApplyCConv(hIsp, stats.down.averageR, stats.down.averageG, stats.down.averageB, &ccAvgR, &ccAvgG, &ccAvgB);

    /* Apply gamma */
    meas[0] = ISP_Algo_ApplyGammaInverse(hIsp, ccAvgR);
    meas[1] = ISP_Algo_ApplyGammaInverse(hIsp, ccAvgG);
    meas[2] = ISP_Algo_ApplyGammaInverse(hIsp, ccAvgB);

    /* Run algo to estimate gain and color conversion to apply */
    e_ret = evision_api_awb_run_average(pIspAWBestimator, NULL, 1, meas);
    if (e_ret == EVISION_RET_SUCCESS)
    {
#ifdef ALGO_AWB_DBG_LOGS
      static int nb_meas, nb_changes;
      static int nb_colortemp_change[ISP_AWB_COLORTEMP_REF];

      nb_meas++;
      if (pIspAWBestimator->out_temp != currentColorTemp)
        nb_changes++;
      for (int i = 0; i < ISP_AWB_COLORTEMP_REF; i++) {
        if (pIspAWBestimator->out_temp == IQParamConfig->AWBAlgo.referenceColorTemp[i])
        {
          nb_colortemp_change[i]++;
          continue;
        }
      }

      if (nb_meas == 100)
      {
        printf("Last 100 measures:\r\n");
        for (int i = 0; i < ISP_AWB_COLORTEMP_REF; i++) {
          printf("\t%ld: %d\r\n",
                 IQParamConfig->AWBAlgo.referenceColorTemp[i],
                 nb_colortemp_change[i]);
        }
        printf("\nChanges: %d\r\n", nb_changes);

        nb_meas = 0;
        nb_changes = 0;
        for (int i = 0; i < ISP_AWB_COLORTEMP_REF; i++) {
          nb_colortemp_change[i] = 0;
        }
      }
#endif
      if (pIspAWBestimator->out_temp != currentColorTemp || reconfigureRequest == true)
      {
        /* Force to apply a WB profile when reconfigureRequest is true */
        reconfigureRequest = false;

#ifdef ALGO_AWB_DBG_LOGS
        printf("Color temperature = %ld\r\n", (uint32_t) pIspAWBestimator->out_temp);
#endif
        /* Find the index profile for this referenceColorTemp */
        for (profId = 0; profId < ISP_AWB_COLORTEMP_REF; profId++)
        {
          if (pIspAWBestimator->out_temp == IQParamConfig->AWBAlgo.referenceColorTemp[profId])
            break;
        }

        if (profId == ISP_AWB_COLORTEMP_REF)
        {
          /* Unknown profile */
          ret  = ISP_ERR_WB_COLORTEMP;
        }
        else
        {
          /* Apply Color Conversion */
          ColorConvConfig.enable = 1;
          memcpy(ColorConvConfig.coeff, IQParamConfig->AWBAlgo.coeff[profId], sizeof(ColorConvConfig.coeff));
          ret = ISP_SVC_ISP_SetColorConv(hIsp, &ColorConvConfig);

          /* Apply gain */
          if (ret == ISP_OK)
          {
            ISPGainConfig.enable = 1;
            ISPGainConfig.ispGainR = IQParamConfig->AWBAlgo.ispGainR[profId];
            ISPGainConfig.ispGainG = IQParamConfig->AWBAlgo.ispGainG[profId];
            ISPGainConfig.ispGainB = IQParamConfig->AWBAlgo.ispGainB[profId];
            ret = ISP_SVC_ISP_SetGain(hIsp, &ISPGainConfig);
            if (ret == ISP_OK)
            {
              currentColorTemp = (uint32_t) pIspAWBestimator->out_temp ;
              current_awb_profId = profId;
            }
          }
        }
      }
    }
    else
    {
      ret = ISP_ERR_ALGO;
    }

    /* Ask for stats */
    ret_stat = ISP_SVC_Stats_GetNext(hIsp, &ISP_Algo_AWB_StatCb, pAlgo, &stats, ISP_STAT_LOC_DOWN,
                                     ISP_STAT_TYPE_AVG, ALGO_ISP_VSYNC_LATENCY);
    ret = (ret != ISP_OK) ? ret : ret_stat;

    /* Wait for stats to be ready */
    algo->state = ISP_ALGO_STATE_WAITING_STAT;
    break;
  }

  return ret;
}

/**
  * @brief  ISP_Algo_SimpleAWB_CCT_LinearSrgb
  *         Convert sRGB component to linear value
  * @param  c:   sRGB component
  * @retval linear component
  */
static double_t ISP_Algo_SimpleAWB_CCT_LinearSrgb(double c)
{
  double linh;

  if (c <= 0.04045)
  {
    linh = c / 12.92;
  }
  else
  {
    linh = pow((c + 0.055) / 1.055, 2.4);
  }
  return linh;
}

/**
  * @brief  ISP_Algo_SimpleAWB_CCT_ComputeCCT
  *         Estimate Correlated Color Temperature from rgb using McCamy's formula
  * @param  r: Red component
  * @param  g: Green component
  * @param  b: Blue component
  * @retval Correlated Color Temperature in °K
  */
static double_t ISP_Algo_SimpleAWB_CCT_ComputeCCT(uint8_t r, uint8_t g, uint8_t b)
{
  /* Correlation matrix used in order to convert RBG values to XYZ space */
  /* Illuminant = D65      RGB (R709) [sRGB or HDTV] to XYZ */
  const double_t Cx[] = {0.4124, 0.3576, 0.1805};
  const double_t Cy[] = {0.2126, 0.7152, 0.0722};
  const double_t Cz[] = {0.0193, 0.1192, 0.9505};
  uint8_t i;
  double_t data[3], xyNormFactor, m_xNormCoeff, m_yNormCoeff, nCoeff, cct;
  double_t X_tmp = 0, Y_tmp = 0, Z_tmp = 0;

  /* Normalize and prepare RGB channels values for cct computation */
  data[0] = ISP_Algo_SimpleAWB_CCT_LinearSrgb(r / 255.0);
  data[1] = ISP_Algo_SimpleAWB_CCT_LinearSrgb(g / 255.0);
  data[2] = ISP_Algo_SimpleAWB_CCT_LinearSrgb(b / 255.0);

  /* Apply correlation matrix to RGB channels to obtain (X,Y,Z) */
  for (i = 0; i < 3; i++)
  {
    X_tmp += Cx[i] * data[i];
    Y_tmp += Cy[i] * data[i];
    Z_tmp += Cz[i] * data[i];
  }

  /* Transform (X,Y,Z) to (x,y) */
  xyNormFactor = X_tmp + Y_tmp + Z_tmp;
  m_xNormCoeff = X_tmp / xyNormFactor;
  m_yNormCoeff = Y_tmp / xyNormFactor;

  /* Apply McCamy's formula to obtain CCT value */
  nCoeff = (m_xNormCoeff - 0.3320) / (0.1858 - m_yNormCoeff);
  cct = (449 * pow(nCoeff, 3) + 3525 * pow(nCoeff, 2) + 6823.3 * nCoeff + 5520.33);

  return cct;
}

/**
  * @brief  ISP_Algo_SimpleAWB_CCT_FixCCT
  *         Get the expected color temperature (from experiments with IMX335 sensor) from the theoretical computed value
  * @param  cct: theoretical correlated color temperature
  * @retval Correlated Color Temperature corrected
  */
static double_t ISP_Algo_SimpleAWB_CCT_FixCCT(double cct)
{
  /* Correction = 0.0005517 CCT² – 4.597 CCT + 12208 */
  return 0.0005517 * cct * cct -4.597 * cct + 12208;
}

/**
  * @brief  ISP_Algo_SimpleAWB_CCT_StatNoGain
  *         Get the RGB stats "before" ISP Gain block
  * @param  hIsp:  ISP device handle.
  * @param  inR:   Input red component
  * @param  inG:   Input green component
  * @param  inB:   Input blue component
  * @param  *outR: Pointer to output red component without ISP Gain
  * @param  *outG: Pointer to output green component without ISP Gain
  * @param  *outB: Pointer to output blue component without ISP Gain
  * @retval None
  */
void ISP_Algo_SimpleAWB_CCT_StatNoGain(ISP_HandleTypeDef *hIsp, uint32_t inR, uint32_t inG, uint32_t inB, uint32_t *outR, uint32_t *outG, uint32_t *outB)
{
  ISP_ISPGainTypeDef gainConfig;
  uint64_t R, G, B;

  /* Read the current ISP gain */
  if ((ISP_SVC_ISP_GetGain(hIsp, &gainConfig) == ISP_OK) && (gainConfig.enable == 1))
  {
    R = ((uint64_t) inR * ISP_GAIN_PRECISION_FACTOR) / gainConfig.ispGainR;
    *outR = (uint32_t) R;

    G = ((uint64_t) inG * ISP_GAIN_PRECISION_FACTOR) / gainConfig.ispGainG;
    *outG = (uint32_t) G;

    B = ((uint64_t) inB * ISP_GAIN_PRECISION_FACTOR) / gainConfig.ispGainB;
    *outB = (uint32_t) B;
  }
  else
  {
    *outR = inR;
    *outG = inG;
    *outB = inB;
  }
}

/**
  * @brief  ISP_Algo_SimpleAWB_CCT_ApplyProfile
  *         Apply Gain and Color Conversion settings according to an AWB profile
  * @param  hIsp:   ISP device handle
  * @param  profId: AWB algo profile ID
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_SimpleAWB_CCT_ApplyProfile(ISP_HandleTypeDef *hIsp, int profId)
{
  ISP_IQParamTypeDef *IQParamConfig;
  ISP_ColorConvTypeDef colorConvConfig;
  ISP_ISPGainTypeDef gainConfig;
  ISP_StatusTypeDef ret;

  IQParamConfig = ISP_SVC_IQParam_Get(hIsp);

  /* Apply Color Conversion */
  colorConvConfig.enable = 1;
  memcpy(colorConvConfig.coeff, IQParamConfig->AWBAlgo.coeff[profId], sizeof(colorConvConfig.coeff));
  ret = ISP_SVC_ISP_SetColorConv(hIsp, &colorConvConfig);

  /* Apply gain */
  if (ret == ISP_OK)
  {
    gainConfig.enable = 1;
    gainConfig.ispGainR = IQParamConfig->AWBAlgo.ispGainR[profId];
    gainConfig.ispGainG = IQParamConfig->AWBAlgo.ispGainG[profId];
    gainConfig.ispGainB = IQParamConfig->AWBAlgo.ispGainB[profId];
    ret = ISP_SVC_ISP_SetGain(hIsp, &gainConfig);
  }

  return ret;
}

/**
  * @brief  ISP_Algo_SimpleAWB_CCT_Init
  *         Initialize the SimpleAWB_CCT algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_SimpleAWB_CCT_Init(void *hIsp, void *pAlgo)
{
  ISP_AlgoTypeDef *algo = (ISP_AlgoTypeDef *)pAlgo;
  ISP_IQParamTypeDef *IQParamConfig;
  ISP_StatusTypeDef ret = ISP_OK;

  // TODO: find a better fix. Disable AWB-CCT
  return ISP_OK;

  IQParamConfig = ISP_SVC_IQParam_Get(hIsp);
  if (IQParamConfig->AWBAlgo.enable == false)
  {
    return ISP_OK;
  }

  /* Update State */
  algo->state = ISP_ALGO_STATE_INIT;

  return ret;
}

/**
  * @brief  ISP_Algo_SimpleAWB_CCT_DeInit
  *         Deinitialize the SimpleAWB_CCT algorithm
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_SimpleAWB_CCT_DeInit(void *hIsp, void *pAlgo)
{
  (void)pAlgo; /* unused */
  ISP_IQParamTypeDef *IQParamConfig;
  ISP_StatusTypeDef ret = ISP_OK;

  IQParamConfig = ISP_SVC_IQParam_Get(hIsp);
  if (IQParamConfig->AWBAlgo.enable == false)
  {
    return ISP_OK;
  }

  return ret;
}

/**
  * @brief  ISP_Algo_SimpleAWB_CCT_StatCb
  *         Callback informing that statistics are available
  * @param  pAlgo: ISP algorithm handle.
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_SimpleAWB_CCT_StatCb(ISP_AlgoTypeDef *pAlgo)
{
  /* Update State */
  pAlgo->state = ISP_ALGO_STATE_STAT_READY;

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_SimpleAWB_CCT_Process
  *         Process the SimpleAWB_CCT algorithm. This basic algorithm estimates the color temperature
  *         to apply gain and color conversion settings identified by predefined profiles.
  * @param  hIsp:  ISP device handle. To cast in (ISP_HandleTypeDef *).
  * @param  pAlgo: ISP algorithm handle. To cast in (ISP_AlgoTypeDef *).
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_SimpleAWB_CCT_Process(void *hIsp, void *pAlgo)
{
  static ISP_SVC_StatStateTypeDef stats;
  ISP_AlgoTypeDef *algo = (ISP_AlgoTypeDef *)pAlgo;
  ISP_IQParamTypeDef *IQParamConfig;
  ISP_StatusTypeDef ret = ISP_OK;
  double cct;
  uint32_t noGainR, noGainG, noGainB, fixedCCT, distance, colorTemp;
  uint32_t profId, bestProfId, bestNbSel;
  static uint32_t colorTempCurrent, preventUpdate, nbSelection[ISP_AWB_COLORTEMP_REF];
#ifdef ALGO_AWB_CCT_DBG_LOGS
  static int dbg_nb_cct_meas, dbg_nb_cct_change, dbg_nb_cct_6500, dbg_nb_cct_5000, dbg_nb_cct_4200, dbg_nb_cct_4000, dbg_nb_cct_2856;
#endif

  // TODO: find a better fix. Disable AWB-CCT
  return ISP_OK;

  IQParamConfig = ISP_SVC_IQParam_Get(hIsp);
  if (IQParamConfig->AWBAlgo.enable == false)
  {
    return ISP_OK;
  }

  switch(algo->state)
  {
  case ISP_ALGO_STATE_INIT:
  case ISP_ALGO_STATE_NEED_STAT:
    /* Ask for stats */
    ret = ISP_SVC_Stats_GetNext(hIsp, &ISP_Algo_SimpleAWB_CCT_StatCb, pAlgo, &stats, ISP_STAT_LOC_DOWN, ISP_STAT_TYPE_AVG, 0);
    if (ret != ISP_OK)
    {
      return ret;
    }

    /* Wait for stats to be ready */
    algo->state = ISP_ALGO_STATE_WAITING_STAT;
    break;

  case ISP_ALGO_STATE_WAITING_STAT:
    /* Do nothing */
    break;

  case ISP_ALGO_STATE_STAT_READY:
    if ((stats.down.averageR != 0) && (stats.down.averageG != 0) && (stats.down.averageB != 0))
    {
      /* Get RGB before ISP gain */
      ISP_Algo_SimpleAWB_CCT_StatNoGain(hIsp, stats.down.averageR, stats.down.averageG, stats.down.averageB, &noGainR, &noGainG, &noGainB);

      /* Get CCT from McCamy’s approximation */
      cct = ISP_Algo_SimpleAWB_CCT_ComputeCCT(noGainR, noGainG, noGainB);

      /* Fix the computed CCT to match IMX335 experimentations */
      fixedCCT = (uint32_t) ISP_Algo_SimpleAWB_CCT_FixCCT(cct);
#ifdef ALGO_AWB_CCT_DBG_LOGS
      //printf("CCT_corrected = %ld \t CCT=%f \t R=%03ld G=%03ld B=%03ld\r\n", fixedCCT, cct, noGainR, noGainG, noGainB);
#endif
      /* Find the index of the closest profile matching this color temperature */
      distance = UINT_MAX;
      profId = 0;
      for (uint32_t i = 0; i < ISP_AWB_COLORTEMP_REF; i++)
      {
        if (IQParamConfig->AWBAlgo.referenceColorTemp[i] == 0)
          continue;

        uint32_t diff = abs((int)(IQParamConfig->AWBAlgo.referenceColorTemp[i] - (fixedCCT)));
        if (diff < distance)
        {
          distance = diff;
          profId = i;
        }
      }

      colorTemp = IQParamConfig->AWBAlgo.referenceColorTemp[profId];

      if (preventUpdate == 0)
      {
        /* Profile can be updated */
        if (colorTemp != colorTempCurrent)
        {
          /* Apply profile */
#ifdef ALGO_AWB_CCT_DBG_LOGS
          printf("New color temperature = %ld\r\n", colorTemp);
#endif
          ret = ISP_Algo_SimpleAWB_CCT_ApplyProfile(hIsp, profId);
          if (ret == ISP_OK)
          {
            colorTempCurrent = colorTemp;

            /* Prevent update for a while */
            preventUpdate = ALGO_AWB_CCT_PREVENT_NB;
            memset(nbSelection, 0, sizeof(nbSelection));

#ifdef ALGO_AWB_CCT_DBG_LOGS
            dbg_nb_cct_change++;
#endif
          }
        }
      }
      else
      {
#ifdef ALGO_AWB_CCT_DBG_LOGS
        //printf("! Skipping color temperature = %ld\r\n", colorTemp);
#endif
        /* Monitor the proposed profiles */
        nbSelection[profId]++;

        /* Check if profile can not be update now */
        if (--preventUpdate == 0)
        {
          /* End of profile selection monitoring. Get the 'best' profile */
          bestProfId = 0;
          bestNbSel = 0;
          for (uint32_t i = 0; i < ISP_AWB_COLORTEMP_REF; i++)
          {
            if (nbSelection[i] > bestNbSel)
            {
              bestNbSel = nbSelection[i];
              bestProfId = i;
            }
          }

          colorTemp = IQParamConfig->AWBAlgo.referenceColorTemp[bestProfId];
          if (colorTemp != colorTempCurrent)
          {
            /* Apply new profile */
#ifdef ALGO_AWB_CCT_DBG_LOGS
            printf("New color temperature (best) = %ld  (%ld)\r\n", colorTemp, bestProfId);
#endif
            ret = ISP_Algo_SimpleAWB_CCT_ApplyProfile(hIsp, bestProfId);
            if (ret == ISP_OK)
            {
              colorTempCurrent = colorTemp;

              /* Prevent update for a while */
              preventUpdate = ALGO_AWB_CCT_PREVENT_NB;
              memset(nbSelection, 0, sizeof(nbSelection));

#ifdef ALGO_AWB_CCT_DBG_LOGS
              dbg_nb_cct_change++;
#endif
            }
          }
        }
      }

#ifdef ALGO_AWB_CCT_DBG_LOGS
      if (colorTempCurrent == 6500)
        dbg_nb_cct_6500++;
      else if (colorTempCurrent == 5000)
        dbg_nb_cct_5000++;
      else if (colorTempCurrent == 4200)
        dbg_nb_cct_4200++;
      else if (colorTempCurrent == 4000)
        dbg_nb_cct_4000++;
      else if (colorTempCurrent == 2856)
        dbg_nb_cct_2856++;

      if (++dbg_nb_cct_meas == 100)
      {
        printf("Last 100 measures:\r\n\t6500: %d\r\n\t5000: %d\r\n\t4200: %d\r\n\t4000: %d\r\n\t2856: %d\r\nChanges: %d\r\n",
               dbg_nb_cct_6500, dbg_nb_cct_5000, dbg_nb_cct_4200, dbg_nb_cct_4000, dbg_nb_cct_2856, dbg_nb_cct_change);

        dbg_nb_cct_meas = 0;
        dbg_nb_cct_change = 0;
        dbg_nb_cct_6500 = 0;
        dbg_nb_cct_5000 = 0;
        dbg_nb_cct_4200 = 0;
        dbg_nb_cct_4000 = 0;
        dbg_nb_cct_2856 = 0;
      }
#endif
    }

    /* Ask for stats */
    ret = ISP_SVC_Stats_GetNext(hIsp, &ISP_Algo_SimpleAWB_CCT_StatCb, pAlgo, &stats, ISP_STAT_LOC_DOWN,
                                ISP_STAT_TYPE_AVG, ALGO_ISP_VSYNC_LATENCY);

    /* Wait for stats to be ready */
    algo->state = ISP_ALGO_STATE_WAITING_STAT;
    break;
  }

  return ret;
}

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  ISP_Algo_Init
  *         Register and initialize all the algorithms
  * @param  hIsp: ISP device handle
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_Init(ISP_HandleTypeDef *hIsp)
{
  ISP_AlgoTypeDef *algo;
  ISP_StatusTypeDef ret;
  uint8_t i;

  hIsp->algorithm = ISP_Algo_List;

  for (i = 0; i < sizeof(ISP_Algo_List) / sizeof(*ISP_Algo_List); i++)
  {
    algo = hIsp->algorithm[i];
    if ((algo != NULL) && (algo->Init != NULL))
    {
      ret = algo->Init((void*)hIsp, (void*)algo);
      if (ret != ISP_OK)
      {
        return ret;
      }
    }
  }

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_DeInit
  *         Deinitialize all the algorithms
  * @param  hIsp: ISP device handle
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_DeInit(ISP_HandleTypeDef *hIsp)
{
  ISP_AlgoTypeDef *algo;
  ISP_StatusTypeDef ret;
  uint8_t i;

  for (i = 0; i < sizeof(ISP_Algo_List) / sizeof(*ISP_Algo_List); i++)
  {
    algo = hIsp->algorithm[i];
    if ((algo != NULL) && (algo->DeInit != NULL))
    {
      ret = algo->DeInit((void*)hIsp, (void*)algo);
      if (ret != ISP_OK)
      {
        return ret;
      }
    }
  }

  return ISP_OK;
}

/**
  * @brief  ISP_Algo_Process
  *         Process all the algorithms
  * @param  hIsp: ISP device handle
  * @retval operation result
  */
ISP_StatusTypeDef ISP_Algo_Process(ISP_HandleTypeDef *hIsp)
{
  ISP_AlgoTypeDef *algo;
  ISP_StatusTypeDef ret;
  uint8_t i;

  for (i = 0; i < sizeof(ISP_Algo_List) / sizeof(*ISP_Algo_List); i++)
  {
    algo = hIsp->algorithm[i];
    if ((algo != NULL) && (algo->Process != NULL))
    {
#ifdef ALGO_PERF_DBG_LOGS
      uint32_t tickstart = HAL_GetTick();
#endif
      ret = algo->Process((void*)hIsp, (void*)algo);
      if (ret != ISP_OK)
      {
        return ret;
      }
#ifdef ALGO_PERF_DBG_LOGS
      algo->perf_meas[iter] = HAL_GetTick() - tickstart;
      algo->iter++;
      if (algo->iter == NB_PERF_MEASURES) {
        uint32_t sum = 0;
        for(uint32_t j = 0; j < NB_PERF_MEASURES; j++)
        {
          sum += algo->perf_meas[j];
        }
        switch (algo->id)
        {
          case ISP_ALGO_ID_BADPIXEL:
            printf("BadPixel algo      ");
            break;
          case ISP_ALGO_ID_BLACKLEVEL:
            printf("BlackLevel algo    ");
            break;
          case ISP_ALGO_ID_SIMPLEAWB:
            printf("Simple AWB algo    ");
            break;
          case ISP_ALGO_ID_SIMPLEAEC:
            printf("Simple AEC algo    ");
            break;
          case ISP_ALGO_ID_AEC:
            printf("AEC algo           ");
            break;
          case ISP_ALGO_ID_AWB:
            printf("AWB algo           ");
            break;
          case ISP_ALGO_ID_SIMPLEAWB_CCT:
            printf("Simple AWB CCT algo");
            break;
        }
        uint32_t meas = sum / NB_PERF_MEASURES;
        if (meas == 0)
        {
          printf(" <1 ms\r\n");
        }
        else
        {
          printf(" %ld ms\r\n", meas);
        }
        algo->iter = 0;
      }
#endif
    }
  }

  return ISP_OK;
}
