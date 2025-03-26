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

#ifndef EI_CAMERA_H
#define EI_CAMERA_H

/* Include ----------------------------------------------------------------- */
#include "firmware-sdk/ei_camera_interface.h"
#include "firmware-sdk/ei_device_info_lib.h"
#include <stddef.h>
#include <stdio.h>

class EiSTCamera : public EiCamera
{
private:
    static ei_device_snapshot_resolutions_t resolutions[];

    bool camera_found;
    uint16_t width;
    uint16_t height;

public:
    bool is_camera_present(void) {return camera_found;};
    bool init(uint16_t width, uint16_t height) override;
    bool deinit(void) override; 
    void get_resolutions(ei_device_snapshot_resolutions_t **res, uint8_t *res_num) override;
    bool set_resolution(const ei_device_snapshot_resolutions_t res) override;
    ei_device_snapshot_resolutions_t get_min_resolution(void) override;
    ei_device_snapshot_resolutions_t search_resolution(uint32_t required_width, uint32_t required_height) override;

    bool ei_camera_capture_rgb888_packed_big_endian(
        uint8_t *image,
        uint32_t image_size) override;

    bool get_fb_ptr(uint8_t** fb_ptr) override;
};



/* Prototypes -------------------------------------------------------------- */
bool ei_camera_read_encode_send_sample_buffer(const char *input, size_t length);


#endif //EI_CAMERA_H