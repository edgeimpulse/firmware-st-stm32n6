/**
 ******************************************************************************
 * @file    isp_param_conf.h
 * @author  AIS Application Team
 * @brief   Header file for IQT parameters of the ISP middleware.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under SLA0044 terms that can be found here:
 * https://www.st.com/resource/en/license_agreement/SLA0044.txt
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ISP_PARAM_CONF__H
#define __ISP_PARAM_CONF__H

#include "app_config.h"
#include "cmw_camera.h"

#if defined(USE_IMX335_SENSOR)
  #include "imx335_isp_param_conf.h"
#elif defined(USE_VD66GY_SENSOR)
  #include "vd66gy_isp_param_conf.h"
#else
/* Default case when isp is not used */
static const ISP_IQParamTypeDef ISP_IQParamCacheInit;
#endif

#endif /* __ISP_PARAM_CONF__H */
