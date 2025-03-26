/**
 * @file        evision-api-ae.h
 * @author      LACROIX - Impulse
 * @copyright   Copyright (c) 2023 LACROIX - Impulse. All rights reserved.
 * @brief       eVision Auto Exposure algorithm public header.
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 */

#ifndef EVISION_API_AE_H_
#define EVISION_API_AE_H_

/************************************************************************
 * Includes
 ************************************************************************/

#include <stdint.h>
#include <stdlib.h>

#include "evision-api-utils.h"

/************************************************************************
 * Public Defines
 ************************************************************************/

/*** Luminance parameters ***/
/*! @brief Example value for desired_luminosity. The images will appear darker. */
#define EVISION_AE_LUMINOSITY_LOW (50u)
/*! @brief Example value for desired_luminosity. The images will be balanced in luminosity. */
#define EVISION_AE_LUMINOSITY_MEDIUM (100u)
/*! @brief Example value for desired_luminosity. The images will appear brighter. */
#define EVISION_AE_LUMINOSITY_HIGH (200u)

/*** Default run-time parameters ***/
/*! @brief Default history length for exposure and gain values. */
#define EVISION_AE_DEFAULT_HISTORY_LENGTH (2u)

/*** Default sensor cofiguration parameters ***/
/*! @brief Maximum size of the exposure LUTs for Auto Exposure algorithm. */
#define EVISION_AE_LUT_EXPOSURE_MAX_SIZE (9000u)
/*! @brief Maximum size of the gain LUTs for Auto Exposure algorithm . */
#define EVISION_AE_LUT_GAIN_MAX_SIZE (250u)

/*** Default estimator parameters ***/
/*! @brief Maximum number of supported sensor configurations for Auto Exposure algorithm. */
#define EVISION_AE_MAX_SENSOR_CONFIGS (2u)
/*! @brief Default number of ROIs. */
#define EVISION_AE_DEFAULT_NB_ROI (1u)

/*** Sensor configurations ***/
/*! @brief Default sensor configuration index. */
#define EVISION_AE_DEFAULT_SENSOR_CONFIG (0u)

/************************************************************************
 * Public Structures
 ************************************************************************/

/**
 * @typedef evision_ae_priv_param_runtime_t
 * @brief AE algorithm private run-time parameters, member of #evision_ae_estimator_t.
 *
 * @struct evision_ae_priv_param_runtime
 * @brief AE algorithm private run-time parameters, member of #evision_ae_estimator_t.
 */
typedef struct evision_ae_priv_param_runtime {
    /*! @brief
     * Index of the current exposure time in evision_ae_estimator_t.active_sensor_cfg.lut_exposure LUT. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Must not be changed! <br/>
     * <b>Default value:</b> 0 */
    uint32_t index_exposure;
    /*! @brief
     * Index of the current gain in evision_ae_estimator_t.active_sensor_cfg.full_lut_gain LUT. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Must not be changed! <br/>
     * <b>Default value:</b> 0 */
    uint32_t index_gain;

    /*! @brief
     * Current convergence speed. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Must not be changed! <br/>
     * <b>Default value:</b> 1.0 */
    double speed_p_value;

    /*! @brief
     * History of previous dlum values. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Must not be changed! <br/>
     * <b>Default value:</b> 0 */
    double history_dlum[EVISION_AE_DEFAULT_HISTORY_LENGTH];
    /*! @brief
     * History of previous exposure indexes. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Must not be changed! <br/>
     * <b>Default value:</b> 0 */
    uint32_t history_index_exposure[EVISION_AE_DEFAULT_HISTORY_LENGTH];
    /*! @brief
     * History of previous gain indexes. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Must not be changed! <br/>
     * <b>Default value:</b> 0 */
    uint32_t history_index_gain[EVISION_AE_DEFAULT_HISTORY_LENGTH];
    /*! @brief
     * Current number of elements in the history. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Must not be changed! <br/>
     * <b>Default value:</b> 0 */
    uint32_t history_counter;

    /*! @brief
     * Flag to indicate convergence. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Must not be changed! <br/>
     * <b>Default value:</b> 0 */
    uint8_t converged;
    /*! @brief
     * Flag to indicate oscillation of the AE algorithm. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Must not be changed! <br/>
     * <b>Default value:</b> 1 */
    uint8_t flickering;

} evision_ae_priv_param_runtime_t;

/**
 * @typedef evision_ae_sensor_config_t
 * @brief AE algorithm sensor configuration, member of #evision_ae_estimator_t.
 *
 * @struct evision_ae_sensor_config
 * @brief AE algorithm sensor configuration, member of #evision_ae_estimator_t.
 *
 */
typedef struct evision_ae_sensor_config {
    /*! @brief
     * Size of exposure time LUT. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Set during estimator initialization. <br/>
     * <b>Default value:</b> 0 */
    uint32_t lut_exposure_size;
    /*! @brief
     * Pointer to the LUT containing the set of valid exposure times for the sensor. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Set during estimator initialization. <br/>
     * <b>Default value:</b> NULL. */
    const uint32_t* lut_exposure;

    /*! @brief
     * Size of the LUT containing the full set of valid analog gain values for the sensor. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Set during estimator initialization. <br/>
     * <b>Default value:</b> 0 */
    uint32_t full_lut_gain_size;
    /*! @brief
     * Pointer to the LUT containing the full set of valid analog gain values for the sensor. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Set during estimator initialization. <br/>
     * <b>Default value:</b> NULL */
    const uint32_t* full_lut_gain;
    /*! @brief
     * Mask indicating the subset of gain values the estimator is allowed to use. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Currently not in use! <br/>
     * <b>Default value:</b> NULL */
    uint8_t full_lut_mask[EVISION_AE_LUT_GAIN_MAX_SIZE];
    /*! @brief
     * Size of the sensor gain sub-LUT. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Currently not in use! <br/>
     * <b>Default value:</b> 0 */
    uint32_t sub_lut_gain_size;
    /*! @brief
     * Array containg the sensor gain sub-LUT. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Currently not in use! <br/>
     * <b>Default value:</b> NULL */
    uint32_t sub_lut_gain[EVISION_AE_LUT_GAIN_MAX_SIZE];

} evision_ae_sensor_config_t;

/**
 * @typedef evision_ae_hyper_param_t
 * @brief AE algorithm hyper-parameters, member of #evision_ae_estimator_t.
 *
 * @struct evision_ae_hyper_param
 * @brief AE algorithm hyper-parameters, member of #evision_ae_estimator_t.
 *
 */
typedef struct evision_ae_hyper_param {
    /*! @brief
     * Ratio of pixels to be processed for AE when using the SW measuring block with direct impact on processing time.
     * (Ex. A value of 2 will process 1 pixel every 2*2 pixel block in the entire ROI). <br/>
     * <b>Restrictions:</b> >= 1 AND < image_size. <br/>
     * <b>Default value:</b> 1 */
    uint16_t ae_process_ratio;

    /*! @brief
     * Convergence speed increment. <br/>
     * <b>Restrictions:</b> > 0 AND < #speed_p_max. <br/>
     * <b>Default value:</b> 0.1 */
    double speed_p_increment;
    /*! @brief
     * Minimum convergence speed. <br/>
     * <b>Restrictions:</b> > 0 AND < #speed_p_max. <br/>
     * <b>Default value:</b> 1.0 */
    double speed_p_min;
    /*! @brief
     * Maximum convergence speed. <br/>
     * <b>Restrictions:</b> > #speed_p_min. <br/>
     * <b>Default value:</b> 3.0 */
    double speed_p_max;

    /*! @brief
     * Target luminosity value. <br/>
     * <b>Restrictions:</b> >= 0 AND < 255. <br/>
     * <b>Default value:</b> #EVISION_AE_LUMINOSITY_MEDIUM. */
    uint8_t desired_luminosity;
    /*! @brief
     * Width of tolerance region around the target luminosity value.
     * Convergence is assumed whenever |measured_luminosity - #desired_luminosity| <= delta_c. <br/>
     * <b>Restrictions:</b> 0.0 < delta_c < 255.0. <br/>
     * <b>Default value:</b> 15.0 */
    double delta_c;
    /*! @brief
     * Factor governing the width of slow exposure adaptation region.
     * Algorithm operates in a slow exposure adaptation mode whenever #delta_c < |measured_luminosity - #desired_luminosity| < #delta_c * delta_c_bw.
     * In slow exposure adaptation mode the sensor exposure parameters are varied in small increments to ensure a smooth variation in the output luminosity level.<br/>
     * The width of the slow exposure adaptation region plays a part in the trade-off between fast exposure adaptation and smooth exposure adaptation.<br/>
     * <b>Restrictions:</b> 1.0 < delta_c_bw. <br/>
     * <b>Default value:</b> 2.0 */
    double delta_c_bw;

    /*! @brief
     * Guard exposure index difference from minimum exposure index for fast convergence purposes.
     * Small values enable fast convergence when decreasing exposure.
     * However, too small values can lead to overshoots during exposure convergence. <br/>
     * <b>Restrictions:</b> > 0.0 AND < #EVISION_AE_LUT_EXPOSURE_MAX_SIZE. <br/>
     * <b>Default value:</b> 5 */
    uint32_t index_diff_exposure_min;
    /*! @brief
     * Guard exposure index difference from maximum exposure index for fast convergence purposes.
     * Small values enable fast convergence when increasing exposure.
     * However, too small values can lead to overshoots during exposure convergence. <br/>
     * <b>Restrictions:</b> > 0.0 AND < #EVISION_AE_LUT_EXPOSURE_MAX_SIZE. <br/>
     * <b>Default value:</b> 5 */
    uint32_t index_diff_exposure_max;
    /*! @brief
     * Minimum value accepted for exposure time. <br/>
     * <b>Restrictions:</b> >= 0.0 AND < #exposure_max. <br/>
     * <b>Default value:</b> 0.0*/
    double exposure_min;
    /*! @brief
     * Maximum value accepted for exposure time. <br/>
     * <b>Restrictions:</b> > 0.0 AND >= #exposure_min. <br/>
     * <b>Default value:</b> 1.0 */
    double exposure_max;

    /*! @brief
     * Enable the use of analog gain during AE operations. <br/>
     * <b>Default value:</b> 1 [Enabled] */
    uint8_t enable_gain;
    /*! @brief
     * Guard gain index difference from minimum gain index for fast convergence purposes.
     * Small values enable fast converges when decreasing exposure.
     * However, too small values can lead to overshoots during exposure convergence. <br/>
     * <b>Restrictions:</b> > 0.0 AND < #EVISION_AE_LUT_GAIN_MAX_SIZE. <br/>
     * <b>Default value:</b> 5 */
    uint32_t index_diff_gain_min;
    /*! @brief
     * Guard gain index difference from maximum gain index for fast convergence purposes.
     * Small values enable fast converges when increasing exposure.
     * However, too small values can lead to overshoots during exposure convergence. <br/>
     * <b>Restrictions:</b> > 0.0 AND < #EVISION_AE_LUT_GAIN_MAX_SIZE. <br/>
     * <b>Default value:</b> 5 */
    uint32_t index_diff_gain_max;
    /*! @brief
     * Minimum possible gain value. <br/>
     * <b>Restrictions:</b> > 1 AND < #gain_max. <br/>
     * <b>Default value:</b> 1 */
    uint32_t gain_min;
    /*! @brief
     * Maximum possible gain value. <br/>
     * <b>Restrictions:</b> >= 1 AND > #gain_min. <br/>
     * <b>Default value:</b> 1 */
    uint32_t gain_max;

} evision_ae_hyper_param_t;

/**
 * @typedef evision_ae_estimator_t
 * @brief AE estimator structure.
 *
 * @struct evision_ae_estimator
 * @brief AE estimator structure.
 *
 * Set of all data structures required for the functioning of the AE algorithm.
 */
typedef struct evision_ae_estimator {
    /*! @brief
     * Indicates the state of the AE algorithm. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Must not be changed! <br/>
     * Possible values: see #evision_state_t */
    evision_state_t state;

    /*! @brief
     * Handler for logging purposes. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Must not be changed! */
    evision_log_t log;

    /*! @brief
     * The set of run-time variables. <br/>
     * <b>Restrictions:</b> Internal algorithm parameters.
     * Must not be changed! */
    evision_ae_priv_param_runtime_t runtime_vars;

    /*! @brief
     * The set of sensor configurations. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter.
     * Must be set during initialization using the provided functions!
     * Must not be changed during runtime! */
    evision_ae_sensor_config_t sensor_configs[EVISION_AE_MAX_SENSOR_CONFIGS];

    /*! @brief
     * Pointer to active sensor configuration. <br/>
     * <b>Default value:</b> NULL.
     * Set to #sensor_configs[#EVISION_AE_DEFAULT_SENSOR_CONFIG] during initialization.
     */
    evision_ae_sensor_config_t* active_sensor_cfg;

    /*! @brief
     * Computed exposure time to apply to sensor. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Must not be changed! */
    double exposure;

    /*! @brief
     * Computed analog gain value to apply to sensor. <br/>
     * <b>Restrictions:</b> Internal algorithm parameter. Must not be changed! */
    uint32_t gain;

    /*! @brief
     * The set of AE algorithm hyper-parameters. Set to default values during initialization! <br/>
     * #evision_ae_hyper_param_t.exposure_min, #evision_ae_hyper_param_t.exposure_max,
     * #evision_ae_hyper_param_t.gain_min and #evision_ae_hyper_param_t.gain_max hyper-parameters
     * <b> must be updated </b> before running the estimator with respect to the sensor in use. <br/>
     * As a general note, the hyper-parameters can be updated from their default values.
     * Care must be taken as in such a case there is no guarantee of proper functioning of the estimator! <br/>
     * <b>Restrictions:</b> Must not be updated during runtime!
     */
    evision_ae_hyper_param_t hyper_params;

} evision_ae_estimator_t;

/************************************************************************
 * Public Variables
 ************************************************************************/

/************************************************************************
 * Public Function Prototypes
 ************************************************************************/

/************************************************************************
 * Public Function Definitions
 ************************************************************************/

/**
 * @fn evision_ae_estimator_t* evision_api_ae_new(void);
 * @brief Create a new #evision_ae_estimator_t instance.
 *
 * @return The address of the created instance. NULL if it failed.
 *
 * This function will allocate the needed memory for the existence of the estimator.
 * If the process fails, NULL is returned.
 *
 * @warning Allocates memory. Free memory with evision_api_ae_delete().
 */
evision_ae_estimator_t* evision_api_ae_new(void);

/**
 * @fn evision_return_t evision_api_ae_delete(evision_ae_estimator_t* self);
 * @brief Releases the memory allocated for the #evision_ae_estimator_t estimator instance.
 *
 * @param[in, out] self Concerned estimator instance address.
 * @return
 * - EVISION_RET_SUCCESS
 * - EVISION_RET_PARAM_ERR
 *
 * This function releases the memory allocated for an AE estimator.
 * To be called when the estimator is no longer required.
 */
evision_return_t evision_api_ae_delete(evision_ae_estimator_t* self);

/**
 * @fn evision_return_t evision_api_ae_init(evision_ae_estimator_t* const self, uint32_t exposure_lut_sizes[EVISION_AE_MAX_SENSOR_CONFIGS], const uint32_t* exposure_luts[EVISION_AE_MAX_SENSOR_CONFIGS], uint32_t gain_lut_full_sizes[EVISION_AE_MAX_SENSOR_CONFIGS], const uint32_t* gain_luts[EVISION_AE_MAX_SENSOR_CONFIGS])
 * @brief Initialize the AE estimator structure.
 *
 * @param[in, out] self AE estimator to be initialized.
 * @param[in] exposure_lut_sizes Array of exposure time LUTs sizes.
 * @param[in] exposure_luts Array of pointers to the exposure time LUTs.
 * @param[in] gain_lut_full_sizes Array of sensor gain LUTs sizes.
 * @param[in] gain_luts Array of pointers to the sensor gain LUTs.
 * @return
 * - EVISION_RET_SUCCESS
 * - EVISION_RET_PARAM_ERROR
 * - EVISION_RET_FAILURE
 *
 * Initializes the AE estimator structure. Except for the exposure and gain LUTs and the respective sizes, all other parameters are initialized
 * to pre-defined default values.
 *
 */
evision_return_t evision_api_ae_init(evision_ae_estimator_t* const self,
    uint32_t exposure_lut_sizes[EVISION_AE_MAX_SENSOR_CONFIGS],
    const uint32_t* exposure_luts[EVISION_AE_MAX_SENSOR_CONFIGS],
    uint32_t gain_lut_full_sizes[EVISION_AE_MAX_SENSOR_CONFIGS],
    const uint32_t* gain_luts[EVISION_AE_MAX_SENSOR_CONFIGS]);

/**
 * @fn evision_return_t evision_api_ae_run_average(evision_ae_estimator_t* const self, const evision_image_t* const image, uint8_t use_ext_lum, double ext_lum)
 * @brief Run the estimator on the full frame data.
 *
 * @param[in, out] self Concerned estimator instance address.
 * @param[in] image The current frame.
 * @param[in] use_ext_lum Flag to indicate the use of external AE measurements: 0 - use internal AE measurement, any other valid value - use external measurement.
 * @param[in] ext_lum External AE luminance measurement value. Range should normally be between 0 (completely dark image) and 255 (completely white image).
 *
 * @return
 * - EVISION_RET_SUCCESS
 * - EVISION_RET_PARAM_ERROR
 * - EVISION_RET_FAILURE
 *
 * Run an execution of the control loop on the current camera setup and frame.
 * The parameters #evision_ae_estimator_t.exposure and #evision_ae_estimator_t.gain are updated to reflect the new
 * exposure time and gain to apply to the sensor.
 *
 * The entire data frame is considered in the extraction of the AE statistics.
 * The parameter #evision_ae_hyper_param_t.ae_process_ratio defines a portion of pixels to check PER row and PER column.
 * For example: a process_ratio of 4 means that for every 4 pixels per row and per column, (block of 16 pixels) only 1 is considered.
 *
 * <b>Raises:</b>
 * - EVISION_LOGSEV_INFO
 * - EVISION_LOGSEV_WARNING
 * - EVISION_LOGSEV_ERROR
 */
evision_return_t evision_api_ae_run_average(evision_ae_estimator_t* const self, const evision_image_t* const image,
    uint8_t use_ext_lum, double ext_lum);

/**
 * @fn evision_return_t evision_api_ae_run_roi(evision_ae_estimator_t* const self, const evision_image_t* const image, const evision_roi_array_t* const roi_array, uint8_t use_ext_lum, double ext_lum)
 * @brief Run the estimator on the selected ROIs.
 *
 * @param[in, out] self Concerned estimator instance address.
 * @param[in] image Current frame.
 * @param[in] roi_array Set of ROIs where the process will be applied, other regions will not be considered.
 * @param[in] use_ext_lum Flag to indicate the use of external AE measurements: 0 - use internal AE measurement, any other valid value - use external measurement.
 * @param[in] ext_lum External AE luminance measurement value. Range should normally be between 0 (completely dark image) and 255 (completely white image).
 *
 * @return
 * - EVISION_RET_SUCCESS
 * - EVISION_RET_PARAM_ERROR
 * - EVISION_RET_FAILURE
 *
 * Run an execution of the control loop on the current camera setup and frame.
 * The parameters #evision_ae_estimator_t.exposure and #evision_ae_estimator_t.gain are updated to reflect the new
 * exposure time and gain to apply to the sensor.
 *
 * The extraction of the AE statistcs will only considered the pixels within the specified ROIs.
 * The parameter #evision_ae_hyper_param_t.ae_process_ratio defines, within the ROI to check, a portion of pixels to check PER row and PER column.
 * For example: a process_ratio of 4 means that for every 4 pixels per row and per column, (block of 16 pixels) only 1 is considered.
 *
 * <b>Raises:</b>
 * - EVISION_LOGSEV_INFO
 * - EVISION_LOGSEV_WARNING
 * - EVISION_LOGSEV_ERROR
 */
evision_return_t evision_api_ae_run_roi(evision_ae_estimator_t* const self, const evision_image_t* const image, const evision_roi_array_t* const roi_array,
    uint8_t use_ext_lum, double ext_lum);

#endif
