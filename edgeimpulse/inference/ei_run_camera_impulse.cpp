/* The Clear BSD License
 *
 * Copyright (c) 2025 EdgeImpulse Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *   * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "model-parameters/model_metadata.h"
#if defined(EI_CLASSIFIER_SENSOR) && (EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_CAMERA)
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/classifier/ei_print_results.h"
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include "firmware-sdk/ei_camera_interface.h"
#include "ingestion-sdk-platform/sensor/ei_camera.h"
#include "firmware-sdk/at_base64_lib.h"
#include "firmware-sdk/jpeg/encode_as_jpg.h"
#include "firmware-sdk/ei_device_info_lib.h"
#include "../Objdetect_pp/lib_objdetect_pp/Inc/objdetect_pp_output_if.h"

#include "utils.h"

typedef enum {
    INFERENCE_STOPPED,
    INFERENCE_WAITING,
    INFERENCE_SAMPLING,
    INFERENCE_DATA_READY
} inference_state_t;

static inference_state_t state = INFERENCE_STOPPED;
static uint64_t last_inference_ts = 0;

static bool debug_mode = false;
static bool continuous_mode = false;
static bool use_max_uart = false;

EiSTCamera *camera = nullptr;

uint8_t *snapshot_buf = nullptr;
static uint32_t snapshot_buf_size;

static ei_device_snapshot_resolutions_t snapshot_resolution;

static bool resize_required = false;
static bool crop_required = false;

static uint32_t inference_delay = 1000;
static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr);
static int ei_camera_get_buffer(size_t offset, size_t length, float *out_ptr);

ei_impulse_result_t result = { 0 };

/**
 * @brief 
 * 
 * @param continuous 
 * @param debug 
 * @param use_max_uart_speed 
 */
void ei_start_impulse(bool continuous, bool debug, bool use_max_uart_speed)
{
    auto* dev = EiDeviceInfo::get_device();
    camera = static_cast<EiSTCamera*>(EiCamera::get_camera());

    debug_mode = debug;
    continuous_mode = debug? true : continuous;
    use_max_uart = use_max_uart_speed;

    if (camera->is_camera_present() == false) {
        ei_printf("ERR: Failed to start inference, camera is missing!\n");
        return;
    }

    snapshot_resolution = camera->search_resolution(EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
    if (camera->set_resolution(snapshot_resolution) == false) {
        ei_printf("ERR: Failed to set snapshot resolution (%ux%u)!\n", EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
        return;
    }

    if (snapshot_resolution.width > EI_CLASSIFIER_INPUT_WIDTH || snapshot_resolution.height > EI_CLASSIFIER_INPUT_HEIGHT) {
        crop_required = true;
        resize_required = false;
    }
    else if (snapshot_resolution.width < EI_CLASSIFIER_INPUT_WIDTH || snapshot_resolution.height < EI_CLASSIFIER_INPUT_HEIGHT) {
        crop_required = false;
        resize_required = true;
    }
    else {
        crop_required = false;
        resize_required = false;
    }

    snapshot_buf_size = snapshot_resolution.width * snapshot_resolution.height * 3;

    // summary of inferencing settings (from model_metadata.h)
    ei_printf("Inferencing settings:\n");
    ei_printf("\tImage resolution: %dx%d\n", EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
    ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));

    if (continuous_mode == true) {
        inference_delay = 0;
        state = INFERENCE_DATA_READY;
    }
    else {
        inference_delay = 2000;
        last_inference_ts = ei_read_timer_ms();
        state = INFERENCE_WAITING;
        ei_printf("Starting inferencing in %d seconds...\n", inference_delay / 1000);
    }

    if ((use_max_uart) || (debug_mode)) {
        ei_printf("OK\r\n");
        ei_sleep(100);
        dev->set_max_data_output_baudrate();
        ei_sleep(100);
    }

}

/**
 * @brief 
 * 
 */
void ei_stop_impulse(void)
{
    auto* dev = EiDeviceInfo::get_device();

    if ((use_max_uart) || (debug_mode)) {
        ei_printf("\r\nOK\r\n");
        ei_sleep(100);
        dev->set_default_data_output_baudrate();
        ei_sleep(100);
    }
    else {
        ei_printf("Stopping inferencing...\n");
    }

    state = INFERENCE_STOPPED;
}

void ei_run_impulse(void)
{
    switch(state) {
        case INFERENCE_STOPPED:
            // nothing to do
            return;
        case INFERENCE_WAITING:
            if(ei_read_timer_ms() > (last_inference_ts + inference_delay)) {
                state = INFERENCE_DATA_READY;
            }
            break;
        default:
            break;
    }    

    bool isOK = camera->ei_camera_capture_rgb888_packed_big_endian(snapshot_buf, snapshot_buf_size);
    if (!isOK) {
        return;
    }
    camera->get_fb_ptr(&snapshot_buf);

    if (resize_required || crop_required) {

        ei::image::processing::crop_and_interpolate_rgb888(
            snapshot_buf,
            snapshot_resolution.width,
            snapshot_resolution.height,
            snapshot_buf,
            EI_CLASSIFIER_INPUT_WIDTH,
            EI_CLASSIFIER_INPUT_HEIGHT);
    }

    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    
    // Print framebuffer as JPG during debugging
    if(debug_mode) {
        signal.get_data = &ei_camera_get_data;
        ei_printf("Begin output\n");

        size_t jpeg_buffer_size = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT >= 128 * 128 ?
            8192 * 3 :
            4096 * 4;
        uint8_t *jpeg_buffer = NULL;
        jpeg_buffer = (uint8_t*)ei_malloc(jpeg_buffer_size);
        if (!jpeg_buffer) {
            ei_printf("ERR: Failed to allocate JPG buffer\r\n");
            return;
        }

        size_t out_size;
        int x = encode_rgb888_signal_as_jpg(&signal, EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT, jpeg_buffer, jpeg_buffer_size, &out_size);
        if (x != 0) {
            ei_printf("Failed to encode frame as JPEG (%d)\n", x);
            ei_free(jpeg_buffer);
            return;
        }

        ei_printf("Framebuffer: ");
        base64_encode((char*)jpeg_buffer, out_size, ei_putchar);
        ei_printf("\r\n");

        if (jpeg_buffer) {
            ei_free(jpeg_buffer);
        }
    }

    signal.get_data = &ei_camera_get_buffer;
    
    EI_IMPULSE_ERROR ei_error = run_classifier(&signal, &result, false);
    if (ei_error != EI_IMPULSE_OK) {
        ei_printf("ERR: Failed to run impulse (%d)\n", ei_error);
        return;
    }

    if(state != INFERENCE_WAITING) {
        ei_print_results(&ei_default_impulse, &result);
    }

    if (debug_mode) {
        ei_printf("\r\n----------------------------------\r\n");
        ei_printf("End output\r\n");
    }

    if(continuous_mode == false && state != INFERENCE_WAITING) {
        // ei_printf("Starting inferencing in %d seconds...\n", inference_delay / 1000);
        last_inference_ts = ei_read_timer_ms();
        state = INFERENCE_WAITING;
    }
}

/**
 * @brief 
 * 
 * @return true 
 * @return false 
 */
bool is_inference_running(void)
{
    return (state != INFERENCE_STOPPED);
}

/**
 *
 * @param offset
 * @param length
 * @param out_ptr
 * @return
 */
static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr)
{
    // we already have a RGB888 buffer, so recalculate offset into pixel index
    size_t pixel_ix = offset * 3;
    size_t pixels_left = length;
    size_t out_ptr_ix = 0;

    while (pixels_left != 0) {
        out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix] << 16) + (snapshot_buf[pixel_ix + 1] << 8) + snapshot_buf[pixel_ix + 2];

        // go to the next pixel
        out_ptr_ix++;
        pixel_ix+=3;
        pixels_left--;
    }

    // and done!
    return 0;
}

/**
 * 
 */
static int ei_camera_get_buffer(size_t offset, size_t length, float *out_ptr)
{
    if ((snapshot_buf != nullptr) && (out_ptr != nullptr)) {
        memcpy(out_ptr, &snapshot_buf[offset], length);
    }

    return 0;
}

static int compareString(const char *str, const char *array[], int size)
{
    for (int i = 0; i < size; i++) {
        if (strcmp(str, array[i]) == 0) {
            return i;  // Return the index if a match is found
        }
    }
    return -1; // Return -1 if no match is found
}

extern "C" int32_t update_score_buffer(postprocess_outBuffer_t *pOutput, int32_t max_objects)
{
    int32_t count = 0;

    #if EI_CLASSIFIER_OBJECT_DETECTION == 1
    for (int32_t i = 0; i < result.bounding_boxes_count; i++) {

        auto bb = result.bounding_boxes[i];
        if (bb.value == 0.f) {
            continue;
        }

        pOutput[count].x_center = (float32_t)bb.x;
        pOutput[count].y_center = (float32_t)bb.y;
        pOutput[count].width = (float32_t)bb.width;
        pOutput[count].height = (float32_t)bb.height;
        pOutput[count].conf = bb.value;
        pOutput[count].class_index = compareString(bb.label, ei_classifier_inferencing_categories, EI_CLASSIFIER_LABEL_COUNT);
        pOutput[count].label_pointer = bb.label;
        pOutput[count].nn_width = (float32_t)EI_CLASSIFIER_INPUT_WIDTH;
        pOutput[count].nn_height = (float32_t)EI_CLASSIFIER_INPUT_HEIGHT;
        #if EI_CLASSIFIER_OBJECT_DETECTION_LAST_LAYER == EI_CLASSIFIER_LAST_LAYER_FOMO
        pOutput[count].model_type = MODEL_OBJECT_DETECTION_CENTROIDS;
        #else
        pOutput[count].model_type = MODEL_OBJECT_DETECTION_BOUNDING_BOXES;
        #endif

        count++;

        if(count >= max_objects) {
            break;
        }
    }
    #else
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        if(result.classification[ix].value > 0.6) {

            pOutput[count].x_center = 0;
            pOutput[count].y_center = 0;
            pOutput[count].width = 0;
            pOutput[count].height = 0;
            pOutput[count].conf = result.classification[ix].value;
            pOutput[count].class_index = ix;
            pOutput[count].label_pointer = result.classification[ix].label;
            pOutput[count].nn_width = (float32_t)EI_CLASSIFIER_INPUT_WIDTH;
            pOutput[count].nn_height = (float32_t)EI_CLASSIFIER_INPUT_HEIGHT;
            pOutput[count].model_type = MODEL_IMAGE_CLASSIFICATION;
            count = 1;

            break;
        }
    }
    #endif

    return is_inference_running() ? count : -1;
}


#endif /* defined(EI_CLASSIFIER_SENSOR) && EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_CAMERA */