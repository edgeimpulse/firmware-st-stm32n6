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

#ifndef EI_DEVICE_ST_STM32N6
#define EI_DEVICE_ST_STM32N6

/* Include ----------------------------------------------------------------- */
#include "firmware-sdk/ei_device_info_lib.h"
#include "firmware-sdk/ei_device_memory.h"
#include "ingestion-sdk-platform/sensor/ei_camera.h"

/* Const defines ----------------------------------------------------------- */
#define EI_DEVICE_N_RESOLUTIONS     5

/* Supported baud rates --------------------------------------------------- */
#define DEFAULT_BAUD    115200
#define MAX_BAUD        921600

class EiDeviceStm32n6 : public EiDeviceInfo
{
private:
    /** Sensors */
    typedef enum
    {
        MAX_USED_SENSOR = 0,
    } used_sensors_t;
    
    EiSTCamera *cam;
    ei_device_sensor_t sensors[MAX_USED_SENSOR];    
    EiState state;
    bool is_sampling;
    void (*sample_read_callback)(void);
#if MULTI_FREQ_ENABLED == 1
    void (*sample_multi_read_callback)(uint8_t);
#endif

public:    
    EiDeviceStm32n6(EiDeviceMemory* mem);
    ~EiDeviceStm32n6();

    EiSTCamera* get_camera(void) { return cam; };
    void init_device_id(void) override;
    void set_default_data_output_baudrate(void) override;
    void set_max_data_output_baudrate(void) override;
    EiSnapshotProperties get_snapshot_list(void) override;
    bool get_sensor_list(const ei_device_sensor_t **p_sensor_list, size_t *sensor_list_size) override;
    bool start_sample_thread(void (*sample_read_cb)(void), float sample_interval_ms) override;
    bool stop_sample_thread(void) override;

#if MULTI_FREQ_ENABLED == 1
    bool start_multi_sample_thread(void (*sample_multi_read_cb)(uint8_t), float* multi_sample_interval_ms, uint8_t num_fusioned) override;
#endif

    void sample_thread(void);
};

#endif
