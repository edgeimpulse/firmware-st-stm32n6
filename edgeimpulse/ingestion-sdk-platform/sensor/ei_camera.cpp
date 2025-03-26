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

/* Include ----------------------------------------------------------------- */
#include "ei_camera.h"
#include "firmware-sdk/at_base64_lib.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include <cmath>
#include "app_config.h"

static uint8_t *global_camera_buffer;

extern "C" void CAM_ei_PipeInitNn(int width, int height);
extern "C" uint8_t *CAM_ei_capture_frame(void);

ei_device_snapshot_resolutions_t EiSTCamera::resolutions[] = {
        {96, 96},
        {160, 120},
        {160, 160},
        {224, 224},
        {320, 240},
        {320, 320},
        {640, 480},
        {640, 640},
};


/**
 * @brief 
 * 
 * @param width 
 * @param height 
 * @return true 
 * @return false 
 */
bool EiSTCamera::init(uint16_t width, uint16_t height)
{
    camera_found = true;

    /** @todo This breaks, somehow this function can only be called once */
    // CAM_ei_PipeInitNn(width, height);

    this->width = width;
    this->height = height;

    global_camera_buffer = CAM_ei_capture_frame();

    return true;
}

/**
 * @brief 
 * 
 * @return true 
 * @return false 
 */
bool EiSTCamera::deinit(void)
{
    return true;
}

/**
 * @brief 
 * 
 * @param res 
 * @param res_num 
 */
void EiSTCamera::get_resolutions(ei_device_snapshot_resolutions_t **res, uint8_t *res_num)
{
    *res = &EiSTCamera::resolutions[0];
    *res_num = sizeof(EiSTCamera::resolutions) / sizeof(ei_device_snapshot_resolutions_t);
}

/**
 * @brief 
 * 
 * @param res 
 * @return true 
 * @return false 
 */
bool EiSTCamera::set_resolution(const ei_device_snapshot_resolutions_t res)
{
    this->width = res.width;
    this->height = res.height;

    return true;
}

bool EiSTCamera::get_fb_ptr(uint8_t** fb_ptr)
{
    *fb_ptr = global_camera_buffer;
    return true;
}

bool EiSTCamera::ei_camera_capture_rgb888_packed_big_endian(
    uint8_t *image,
    uint32_t image_size)
{
    global_camera_buffer = CAM_ei_capture_frame();
    return 1;
}

/**
 * @brief 
 * 
 * @return ei_device_snapshot_resolutions_t 
 */
ei_device_snapshot_resolutions_t EiSTCamera::get_min_resolution(void)
{
    return EiSTCamera::resolutions[0];
}

/**
 *
 * @return
 */
EiCamera* EiCamera::get_camera(void)
{
    static EiSTCamera cam;

    return &cam;
}

/**
 * @brief 
 * 
 * @param required_width 
 * @param required_height 
 * @return ei_device_snapshot_resolutions_t 
 */
ei_device_snapshot_resolutions_t EiSTCamera::search_resolution(uint32_t required_width, uint32_t required_height)
{
    ei_device_snapshot_resolutions_t res;
    // uint16_t max_width;
    // uint16_t max_height;

    // camera_get_max_res(&max_width, &max_height);

    // if ((required_width <= max_width) && (required_height <= max_height)) {
    //     res.height = required_height;
    //     res.width = required_width;
    // }
    // else {
    //     res.height = max_height;
    //     res.width = max_width;
    // }
    res.height = NN_WIDTH;
    res.width = NN_HEIGHT;

    return res;
}

/**
 *
 * @param input
 * @param length
 * @return
 */
bool ei_camera_read_encode_send_sample_buffer(const char *input, size_t length)
{
    uint32_t address = 0;
    // we are encoiding data into base64, so it needs to be divisible by 3
    //const int buffer_size = 16416;
    const int buffer_size = 513;
    uint8_t* buffer = (uint8_t*)input;

    size_t output_size_check = floor(buffer_size / 3 * 4);
    size_t mod = buffer_size % 3;
    output_size_check += mod;

    uint8_t* buffer_out = (uint8_t*)ei_malloc(output_size_check);

    while (1) {
        size_t bytes_to_read = buffer_size;

        if (bytes_to_read > length) {
            bytes_to_read = length;
        }

        if (bytes_to_read == 0) {
            ei_free(buffer_out);
            return true;
        }

        int to_send = base64_encode_buffer((char *)&buffer[address], bytes_to_read, (char *)buffer_out, output_size_check);
#if (USE_UART == 1)
        uart_print_to_console(buffer_out, to_send);
#else
        // comms_send(buffer_out, to_send, 1000);
        for(int i = 0; i < to_send; i++) {
            ei_putchar(buffer_out[i]);
        }
#endif

        address += bytes_to_read;
        length -= bytes_to_read;
    }

    return true;
}