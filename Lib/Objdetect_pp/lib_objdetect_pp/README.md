# STM32 computer vision models post processing

This readme explains how to use the different available post processing code. 


## Available post processing


| Models        | Task                 |
|---------------|----------------------|
| YOLOv8        | person_detection     |
| YOLOv5        | person_detection     |
| YOLOv5nu      | person_detection     |
| Tiny Yolo V2  | person_detection     |
| Standard SSD  | person_detection     |
| ST SSD        | person_detection     |
| Centernet     | person_detection     |


# Post-Processing Output Structures
<details>

---
## `postprocess_outBuffer_t`

This structure represents the output buffer for a single detection result. It contains information about the detected object's position, size, confidence score, and class index.

Parameters:

- **float32_t x_center**: The normalized x-coordinate of the center of the detected object.
- **float32_t y_center**: The normalized y-coordinate of the center of the detected object.
- **float32_t width**: The normalized width of the detected object.
- **float32_t height**: The normalized height of the detected object.
- **float32_t conf**: The confidence (between 0.0 and 1.0) score of the detection.
- **int32_t class_index**: The index of the detected object's class.


---
## `postprocess_out_t`

This structure represents the overall output of the post-processing step. It contains a pointer to an array of postprocess_outBuffer_t structures and the number of detections.

Parameters:

- **postprocess_outBuffer_t \*pOutBuff**: Pointer to an array of postprocess_outBuffer_t structures.
- **int32_t nb_detect**: The number of detections in the output buffer.

</details>

# YOLOV8 Post Processing
<details>

## YOLOv8 Structures
---
### `yolov8_pp_in_centroid_t`

This structure is used for YOLOv8 post-processing input where the raw detections are in float32 format.

Parameters:

- **float32_t \*pRaw_detections**: Pointer to raw detection data in float32 format.
---
### `yolov8_pp_in_centroid_int8_t`

This structure is used for YOLOv8 post-processing input where the raw detections are in int8 format.

Parameters:

- **int8_t \*pRaw_detections**: Pointer to raw detection data in int8 format.
---
### `yolov8_pp_static_param_t`

This structure holds the static parameters required for YOLOv8 post-processing.

Parameters:

- **int32_t nb_classes**: Number of classes in the detection model. To extract fom the model output shape.
- **int32_t nb_total_boxes**: Total number of boxes predicted by the model. To extract fom the model output shape.
- **int32_t max_boxes_limit**: Maximum number of boxes per class to be considered after post-processing.
- **float32_t conf_threshold**: Confidence threshold for filtering detections. High confidence helps filtering out low-confidence detections (False positives), However, it is essential to balance the threshold value to ensure that you do not miss too many true positives.
- **float32_t iou_threshold**: Intersection over Union (IoU) threshold for Non-Maximum Suppression (NMS).A high IoU threshold means that more overlapping will be allowed between boxes, while a lower threshold will allow less boxes to be retained.
- **float32_t raw_output_scale**: Scale factor for raw output values.
- **int8_t raw_output_zero_point**: Zero point for quantized raw output values.
- **int32_t nb_detect**: Number of detections after post-processing.
---
## YOLOv8 Routines
---
### `objdetect_yolov8_pp_reset`

**Purpose**:  
Resets the static parameters for YOLOv8 post-processing.

**Prototype**:  
```c
int32_t objdetect_yolov8_pp_reset(yolov8_pp_static_param_t *pInput_static_param);
```

**Parameters**:  
- **pInput_static_param**: Pointer to the static parameters structure.

**Returns**:  
- **AI_OBJDETECT_POSTPROCESS_ERROR_NO** on success.

**Description**:  
This function initializes the static parameters for the YOLOv8 post-processing by setting the number of detected objects to zero.

---

### `objdetect_yolov8_pp_process`

**Purpose**:  
Processes the YOLOv8 post-processing pipeline for float32 input data.

**Prototype**:  
```c
int32_t objdetect_yolov8_pp_process(yolov8_pp_in_centroid_t *pInput,
                                    postprocess_out_t *pOutput,
                                    yolov8_pp_static_param_t *pInput_static_param);
```

**Parameters**:  
- **pInput**: Pointer to the input centroid data.
- **pOutput**: Pointer to the output post-processing data.
- **pInput_static_param**: Pointer to the static parameters structure.

**Returns**:  
- AI_OBJDETECT_POSTPROCESS_ERROR_NO on success, or an error code on failure.

**Description**:  
This function performs the post-processing steps for YOLOv8 object detection. It first retrieves the neural network boxes, then applies Non-Maximum Suppression (NMS), and finally performs score re-filtering.

---

#### `objdetect_yolov8_pp_process_int8`

**Purpose**:  
Processes the YOLOv8 post-processing pipeline for int8 input data.

**Prototype**:  
```c
int32_t objdetect_yolov8_pp_process_int8(yolov8_pp_in_centroid_int8_t *pInput,
                                         postprocess_out_t *pOutput,
                                         yolov8_pp_static_param_t *pInput_static_param);
```

**Parameters**:  
- **pInput**: Pointer to the int8 input centroid data.
- **pOutput**: Pointer to the output post-processing data.
- **pInput_static_param**: Pointer to the static parameters structure.

**Returns**:  
- **AI_OBJDETECT_POSTPROCESS_ERROR_NO** on success, or an error code on failure.

**Description**:  
This function performs the post-processing steps for YOLOv8 object detection with int8 input data. It first retrieves the neural network boxes, then applies Non-Maximum Suppression (NMS), and finally performs score re-filtering.

---

### Error Codes

- **AI_OBJDETECT_POSTPROCESS_ERROR_NO**: Indicates successful execution of the function.

---

</details>

# YOLOV5 Post Processing
<details>

## YOLOv5 Structures
---
### `yolov5_pp_in_centroid_t`

This structure is used for YOLOv5 post-processing input where the raw detections are in float32 format.

Parameters:

- **float32_t \*pRaw_detections**: Pointer to raw detection data in float32 format.
---
### `yolov5_pp_in_centroid_uint8_t`

This structure is used for YOLOv5 post-processing input where the raw detections are in uint8 format.

Parameters:

- **uint8_t \*pRaw_detections**: Pointer to raw detection data in uint8 format.
---
### `yolov5_pp_static_param_t`

This structure holds the static parameters required for YOLOv5 post-processing.

Parameters:

- **int32_t nb_classes**: Number of classes in the detection model. To extract fom the model output shape.
- **int32_t nb_total_boxes**: Total number of boxes predicted by the model. To extract fom the model output shape.
- **int32_t max_boxes_limit**: Maximum number of boxes per class to be considered after post-processing.
- **float32_t conf_threshold**: Confidence threshold for filtering detections. High confidence helps filtering out low-confidence detections (False positives), However, it is essential to balance the threshold value to ensure that you do not miss too many true positives.
- **float32_t iou_threshold**: Intersection over Union (IoU) threshold for Non-Maximum Suppression (NMS).A high IoU threshold means that more overlapping will be allowed between boxes, while a lower threshold will allow less boxes to be retained.
- **float32_t raw_output_scale**: Scale factor for raw output values.
- **int8_t raw_output_zero_point**: Zero point for quantized raw output values.
- **int32_t nb_detect**: Number of detections after post-processing.
---
## YOLOv5 Routines
---
### `objdetect_yolov5_pp_reset`

**Purpose**:  
Resets the static parameters for YOLOv5 post-processing.

**Prototype**:  
```c
int32_t objdetect_yolov5_pp_reset(yolov5_pp_static_param_t *pInput_static_param);
```

**Parameters**:  
- **pInput_static_param**: Pointer to the static parameters structure.

**Returns**:  
- **AI_OBJDETECT_POSTPROCESS_ERROR_NO** on success.

**Description**:  
This function initializes the static parameters for the YOLOv5 post-processing by setting the number of detected objects to zero.

---

### `objdetect_yolov5_pp_process`

**Purpose**:  
Processes the YOLOv5 post-processing pipeline for float32 input data.

**Prototype**:  
```c
int32_t objdetect_yolov5_pp_process(yolov5_pp_in_centroid_t *pInput,
                                    postprocess_out_t *pOutput,
                                    yolov5_pp_static_param_t *pInput_static_param);
```

**Parameters**:  
- **pInput**: Pointer to the input centroid data.
- **pOutput**: Pointer to the output post-processing data.
- **pInput_static_param**: Pointer to the static parameters structure.

**Returns**:  
- AI_OBJDETECT_POSTPROCESS_ERROR_NO on success, or an error code on failure.

**Description**:  
This function performs the post-processing steps for YOLOv5 object detection. It first retrieves the neural network boxes, then applies Non-Maximum Suppression (NMS), and finally performs score re-filtering.

---

#### `objdetect_yolov5_pp_process_uint8`

**Purpose**:  
Processes the YOLOv5 post-processing pipeline for uint8 input data.

**Prototype**:  
```c
int32_t objdetect_yolov5_pp_process_uint8(yolov5_pp_in_centroid_uint8_t *pInput,
                                         postprocess_out_t *pOutput,
                                         yolov5_pp_static_param_t *pInput_static_param);
```

**Parameters**:  
- **pInput**: Pointer to the uint8 input centroid data.
- **pOutput**: Pointer to the output post-processing data.
- **pInput_static_param**: Pointer to the static parameters structure.

**Returns**:  
- **AI_OBJDETECT_POSTPROCESS_ERROR_NO** on success, or an error code on failure.

**Description**:  
This function performs the post-processing steps for YOLOv5 object detection with uint8 input data. It first retrieves the neural network boxes, then applies Non-Maximum Suppression (NMS), and finally performs score re-filtering.

---

### Error Codes

- **AI_OBJDETECT_POSTPROCESS_ERROR_NO**: Indicates successful execution of the function.

---

</details>

# Tiny YOLOV2 Post Processing
<details>

## Tiny YOLOV2 Structures
---
### `yolov2_pp_in_t`

This structure is used for Tiny YOLOV2 post-processing input where the raw detections are in float32 format.

Parameters:

- **float32_t \*pRaw_detections**: Pointer to raw detection data in float32 format.
---
### `yolov2_pp_static_param_t`

This structure holds the static parameters required for Tiny YOLOV2 post-processing.

Parameters:

- **int32_t nb_classes**: Number of classes in the detection model. To extract fom the model output shape.
- **int32_t nb_anchors**: Number of the anchors used by the model. To extract fom the model output shape.
- **int32_t grid_width**: The width of the model output. To extract fom the model output shape.
- **int32_t grid_height**: The height of the model output. To extract fom the model output shape.
- **int32_t nb_input_boxes**: Total number of boxes predicted by the model. nb_input_boxes = nb_anchors x grid_width x grid_height
- **int32_t max_boxes_limit**: Maximum number of boxes per class to be considered after post-processing.
- **float32_t conf_threshold**: Confidence threshold for filtering detections. High confidence helps filtering out low-confidence detections (False positives), However, it is essential to balance the threshold value to ensure that you do not miss too many true positives.
- **float32_t iou_threshold**: Intersection over Union (IoU) threshold for Non-Maximum Suppression (NMS).A high IoU threshold means that more overlapping will be allowed between boxes, while a lower threshold will allow less boxes to be retained.
- **yolov2_pp_optim_e optim**: An optimization parameter for the post-processing step. The specific values and their meanings are defined by the yolov2_pp_optim_e enumeration.
- **int32_t nb_detect**: Number of detections after post-processing.
- **const float32_t \*pAnchors**: A pointer to an array of anchor box dimensions. Each anchor box is defined by its width and height. The array should have a length of 2 x nb_anchors, where each pair of values represents the width and height of an anchor box.
---
## Tiny YOLOV2 Routines
---
### `objdetect_yolov2_pp_reset`

**Purpose**:  
Resets the static parameters for Tiny YOLOV2 post-processing.

**Prototype**:  
```c
int32_t objdetect_yolov2_pp_reset(yolov2_pp_static_param_t *pInput_static_param);

```

**Parameters**:  
- **pInput_static_param**: Pointer to the static parameters structure.

**Returns**:  
- **AI_OBJDETECT_POSTPROCESS_ERROR_NO** on success.

**Description**:  
This function initializes the static parameters for the Tiny YOLOV2 post-processing by setting the number of detected objects to zero.

---

### `objdetect_yolov2_pp_process`

**Purpose**:  
Processes the Tiny YOLOV2 post-processing pipeline for float32 input data.

**Prototype**:  
```c
int32_t objdetect_yolov2_pp_process(yolov2_pp_in_t *pInput,
                                    postprocess_out_t *pOutput,
                                    yolov2_pp_static_param_t *pInput_static_param);
```

**Parameters**:  
- **pInput**: Pointer to the input centroid data.
- **pOutput**: Pointer to the output post-processing data.
- **pInput_static_param**: Pointer to the static parameters structure.

**Returns**:  
- AI_OBJDETECT_POSTPROCESS_ERROR_NO on success, or an error code on failure.

**Description**:  
This function performs the post-processing steps for Tiny YOLOV2 object detection. It first retrieves the neural network boxes, then applies Non-Maximum Suppression (NMS), and finally performs score re-filtering.

---

### Error Codes

- **AI_OBJDETECT_POSTPROCESS_ERROR_NO**: Indicates successful execution of the function.

---

</details>

</details>

# Standard SSD Post Processing
<details>

## Standard SSD Structures
---
### `ssd_pp_in_centroid_t`

This structure is used for Standard SSD post-processing input where the raw detections are in float32 format.

Parameters:

- **float32_t \*pBoxes**: Pointer to the raw Boxes data in float32 format.
- **float32_t \*pAnchors**: Pointer to the Anchors data in float32 format.
- **float32_t \*pScores**: Pointer to the Scores data in float32 format.
---
### `ssd_pp_static_param_t`

This structure holds the static parameters required for Standard SSD post-processing.

Parameters:

- **int32_t nb_classes**: Number of classes in the detection model. To extract fom the model output shape.
- **float32_t XY_scale**: Scale factor applied to the XY coordinates of the bounding boxes. To extract fom the tflite post processing layer (before removing it from the model).
- **float32_t WH_scale**: Scale factor applied to the width and height of the bounding boxes. To extract fom the tflite post processing layer (before removing it from the model).
- **int32_t nb_detections**: Total number of boxes predicted by the model. To extract fom the model output shape.
- **int32_t max_boxes_limit**: Maximum number of boxes per class to be considered after post-processing.
- **float32_t conf_threshold**: Confidence threshold for filtering detections. High confidence helps filtering out low-confidence detections (False positives), However, it is essential to balance the threshold value to ensure that you do not miss too many true positives.
- **float32_t iou_threshold**: Intersection over Union (IoU) threshold for Non-Maximum Suppression (NMS).A high IoU threshold means that more overlapping will be allowed between boxes, while a lower threshold will allow less boxes to be retained.
- **int32_t nb_detect**: Number of detections after post-processing.
---
## Standard SSD Routines
---
### `objdetect_ssd_pp_reset`

**Purpose**:  
Resets the static parameters for Standard SSD post-processing.

**Prototype**:  
```c
int32_t objdetect_ssd_pp_reset(ssd_pp_static_param_t *pInput_static_param);

```

**Parameters**:  
- **pInput_static_param**: Pointer to the static parameters structure.

**Returns**:  
- **AI_OBJDETECT_POSTPROCESS_ERROR_NO** on success.

**Description**:  
This function initializes the static parameters for the Standard SSD post-processing by setting the number of detected objects to zero.

---

### `objdetect_ssd_pp_process`

**Purpose**:  
Processes the Standard SSD post-processing pipeline for float32 input data.

**Prototype**:  
```c
int32_t objdetect_ssd_pp_process(ssd_pp_in_centroid_t *pInput, 
                                 postprocess_out_t *pOutput, 
                                 ssd_pp_static_param_t *pInput_static_param);
```

**Parameters**:  
- **pInput**: Pointer to the input centroid data.
- **pOutput**: Pointer to the output post-processing data.
- **pInput_static_param**: Pointer to the static parameters structure.

**Returns**:  
- AI_OBJDETECT_POSTPROCESS_ERROR_NO on success, or an error code on failure.

**Description**:  
This function performs the post-processing steps for Standard SSD object detection. It first retrieves the neural network boxes, then applies Non-Maximum Suppression (NMS), and finally performs score re-filtering.

---

### Error Codes

- **AI_OBJDETECT_POSTPROCESS_ERROR_NO**: Indicates successful execution of the function.

---

</details>

</details>

# ST SSD Post Processing
<details>

## ST SSD Structures
---
### `ssd_st_pp_in_centroid_t`

This structure is used for ST SSD post-processing input where the raw detections are in float32 format.

Parameters:

- **float32_t \*pBoxes**: Pointer to the raw Boxes data in float32 format.
- **float32_t \*pAnchors**: Pointer to the Anchors data in float32 format.
- **float32_t \*pScores**: Pointer to the Scores data in float32 format.
---
### `ssd_st_pp_static_param_t`

This structure holds the static parameters required for ST SSD post-processing.

Parameters:

- **int32_t nb_classes**: Number of classes in the detection model. To extract fom the model output shape.
- **int32_t nb_detections**: Total number of boxes predicted by the model. To extract fom the model output shape.
- **int32_t max_boxes_limit**: Maximum number of boxes per class to be considered after post-processing.
- **float32_t conf_threshold**: Confidence threshold for filtering detections. High confidence helps filtering out low-confidence detections (False positives), However, it is essential to balance the threshold value to ensure that you do not miss too many true positives.
- **float32_t iou_threshold**: Intersection over Union (IoU) threshold for Non-Maximum Suppression (NMS).A high IoU threshold means that more overlapping will be allowed between boxes, while a lower threshold will allow less boxes to be retained.
- **int32_t nb_detect**: Number of detections after post-processing.
---
## ST SSD Routines
---
### `objdetect_ssd_st_pp_reset`

**Purpose**:  
Resets the static parameters for ST SSD post-processing.

**Prototype**:  
```c
int32_t objdetect_ssd_st_pp_reset(ssd_st_pp_static_param_t *pInput_static_param);

```

**Parameters**:  
- **pInput_static_param**: Pointer to the static parameters structure.

**Returns**:  
- **AI_OBJDETECT_POSTPROCESS_ERROR_NO** on success.

**Description**:  
This function initializes the static parameters for the ST SSD post-processing by setting the number of detected objects to zero.

---

### `objdetect_ssd_st_pp_process`

**Purpose**:  
Processes the ST SSD post-processing pipeline for float32 input data.

**Prototype**:  
```c
int32_t objdetect_ssd_st_pp_process(ssd_st_pp_in_centroid_t *pInput,
                                    postprocess_out_t *pOutput,
                                    ssd_st_pp_static_param_t *pInput_static_param);
```

**Parameters**:  
- **pInput**: Pointer to the input centroid data.
- **pOutput**: Pointer to the output post-processing data.
- **pInput_static_param**: Pointer to the static parameters structure.

**Returns**:  
- AI_OBJDETECT_POSTPROCESS_ERROR_NO on success, or an error code on failure.

**Description**:  
This function performs the post-processing steps for ST SSD object detection. It first retrieves the neural network boxes, then applies Non-Maximum Suppression (NMS), and finally performs score re-filtering.

---

### Error Codes

- **AI_OBJDETECT_POSTPROCESS_ERROR_NO**: Indicates successful execution of the function.

---

</details>
