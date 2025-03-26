/*---------------------------------------------------------------------------------------------
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *--------------------------------------------------------------------------------------------*/

#ifndef __OBJDETECT_POSTPROCESS_INTERFACE_IF_H__
#define __OBJDETECT_POSTPROCESS_INTERFACE_IF_H__


#ifdef __cplusplus
 extern "C" {
#endif

#include "arm_math.h"


/* Error return codes */
#define AI_OBJDETECT_POSTPROCESS_ERROR_NO                    (0)
#define AI_OBJDETECT_POSTPROCESS_ERROR_BAD_HW                (-1)
#define AI_OBJDETECT_POSTPROCESS_ERROR                       (-2)

typedef enum {
	MODEL_OBJECT_DETECTION_BOUNDING_BOXES = 0,
	MODEL_OBJECT_DETECTION_CENTROIDS,
	MODEL_IMAGE_CLASSIFICATION,
}t_model_type;


typedef struct
{
	float32_t x_center;
	float32_t y_center;
	float32_t width;
	float32_t height;
	float32_t conf;
	int32_t   class_index;
	const char *label_pointer;
	float32_t nn_width;
	float32_t nn_height;
	t_model_type model_type;
} postprocess_outBuffer_t;

typedef struct
{
	postprocess_outBuffer_t *pOutBuff;
	int32_t nb_detect;
} postprocess_out_t;


#ifdef __cplusplus
 }
#endif

#endif      /* __OBJDETECT_CENTERNET_PP_IF_H__  */


