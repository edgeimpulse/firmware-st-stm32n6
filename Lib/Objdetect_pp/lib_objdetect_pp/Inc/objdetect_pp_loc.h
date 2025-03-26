/*---------------------------------------------------------------------------------------------
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *--------------------------------------------------------------------------------------------*/

#ifndef __OBJDETECT_PP_LOC_H__
#define __OBJDETECT_PP_LOC_H__


#ifdef __cplusplus
 extern "C" {
#endif

#include "arm_math.h"

#ifdef ARM_MATH_MVEF
#define AI_YOLOV5_PP_MVEF_OPTIM
#define AI_YOLOV8_PP_MVEF_OPTIM
#endif
#ifdef ARM_MATH_MVEI
#define AI_YOLOV5_PP_MVEI_OPTIM
#define AI_YOLOV8_PP_MVEI_OPTIM
#endif

#ifndef MIN
  #define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

#ifndef MAX
  #define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

extern void objdetect_maxi(float32_t *arr, int32_t len_arr, float32_t *maxim, int32_t *index);
#ifdef AI_YOLOV8_PP_MVEF_OPTIM
extern void objdetect_maxi_transpose(float32_t *arr, int32_t len_arr, int32_t nb_total_boxes, float32_t *maxim, uint32_t *index, int32_t parallelize);
#else
extern void objdetect_maxi_transpose(float32_t *arr, int32_t len_arr, int32_t nb_total_boxes, float32_t *maxim, int32_t *index);
#endif
extern void objdetect_maxi_uint8(uint8_t *arr, int32_t len_arr, uint8_t *maxim, int32_t *index);
#ifdef AI_YOLOV8_PP_MVEI_OPTIM
extern void objdetect_maxi_transpose_int8(int8_t *arr, int32_t len_arr, int32_t nb_total_boxes, int8_t *maxim, uint8_t *index, int32_t parallelize);
extern void objdetect_maxi_transpose_int8_large(int8_t *arr, int32_t len_arr, int32_t nb_total_boxes, int8_t *maxim, uint16_t *index, int32_t parallelize);
#else
extern void objdetect_maxi_transpose_int8(int8_t *arr, int32_t len_arr, int32_t nb_total_boxes, int8_t *maxim, int32_t *index);
extern void objdetect_maxi_transpose_int8_large(int8_t *arr, int32_t len_arr, int32_t nb_total_boxes, int8_t *maxim, uint16_t *index);
#endif
extern float32_t objdetect_sigmoid_f(float32_t x);
extern void objdetect_softmax_f(float32_t *input_x, float32_t *output_x, int32_t len_x, float32_t *tmp_x);
extern float32_t objdetect_box_iou(float32_t *a, float32_t *b);
extern void transpose_flattened_2D(float32_t *arr, int32_t rows, int32_t cols, float32_t *tmp_x);
extern void dequantize(int32_t* arr, float32_t* tmp, int32_t n, int32_t zero_point, float32_t scale);


/*-----------------------------     YOLO_V2      -----------------------------*/
/* Offsets to access YoloV2 input data */
#define AI_YOLOV2_PP_XCENTER      (0)
#define AI_YOLOV2_PP_YCENTER      (1)
#define AI_YOLOV2_PP_WIDTHREL     (2)
#define AI_YOLOV2_PP_HEIGHTREL    (3)
#define AI_YOLOV2_PP_OBJECTNESS   (4)
#define AI_YOLOV2_PP_CLASSPROB    (5)

typedef int _Cmpfun(const void *, const void *);
extern void qsort(void *, size_t, size_t, _Cmpfun *);

/*-----------------------------     YOLO_V8      -----------------------------*/
/* Offsets to access YoloV8 input data */
#define AI_YOLOV8_PP_XCENTER      (0)
#define AI_YOLOV8_PP_YCENTER      (1)
#define AI_YOLOV8_PP_WIDTHREL     (2)
#define AI_YOLOV8_PP_HEIGHTREL    (3)
#define AI_YOLOV8_PP_CLASSPROB    (4)
#define AI_YOLOV8_PP_CLASSID      (5)
#define AI_YOLOV8_PP_BOX_STRIDE   (4)

/*-----------------------------     YOLO_V5      -----------------------------*/
/* Offsets to access YoloV5 input data */
#define AI_YOLOV5_PP_XCENTER      (0)
#define AI_YOLOV5_PP_YCENTER      (1)
#define AI_YOLOV5_PP_WIDTHREL     (2)
#define AI_YOLOV5_PP_HEIGHTREL    (3)
#define AI_YOLOV5_PP_CONFIDENCE   (4)
#define AI_YOLOV5_PP_CLASSPROB    (5)
#define AI_YOLOV5_PP_CLASSID      (5)
#define AI_YOLOV5_PP_BOX_STRIDE   (4)

/*-----------------------------       SSD        -----------------------------*/
/* Offsets to access SSD input data */
#define AI_SSD_PP_CENTROID_YCENTER          (0)
#define AI_SSD_PP_CENTROID_XCENTER          (1)
#define AI_SSD_PP_CENTROID_HEIGHTREL        (2)
#define AI_SSD_PP_CENTROID_WIDTHREL         (3)
#define AI_SSD_PP_BOX_STRIDE                (4)

/*-----------------------------       SSD  ST      -----------------------------*/
/* Offsets to access SSD ST input data */
#define AI_SSD_ST_PP_XMIN          (0)
#define AI_SSD_ST_PP_YMIN          (1)
#define AI_SSD_ST_PP_XMAX          (2)
#define AI_SSD_ST_PP_YMAX          (3)
#define AI_SSD_ST_PP_BOX_STRIDE    (4)

#define AI_SSD_ST_PP_CENTROID_YCENTER          (0)
#define AI_SSD_ST_PP_CENTROID_XCENTER          (1)
#define AI_SSD_ST_PP_CENTROID_HEIGHTREL        (2)
#define AI_SSD_ST_PP_CENTROID_WIDTHREL         (3)


/*-----------------------------     CENTER_NET      -----------------------------*/
/* Offsets to access CenterNet input data */
#define AI_CENTERNET_PP_CONFCENTER   (0)
#define AI_CENTERNET_PP_WIDTH        (1)
#define AI_CENTERNET_PP_HEIGHT       (2)
#define AI_CENTERNET_PP_XOFFSET      (3)
#define AI_CENTERNET_PP_YOFFSET      (4)
#define AI_CENTERNET_PP_CLASSPROB    (5)
#define AI_CENTERNET_PP_MAPSEG_NEXTOFFSET    (1)


#endif   /*  __OBJDETECT_PP_LOC_H__  */
