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

/* Includes ---------------------------------------------------------------- */
#include "ei_device_st_stm32n6.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "edge-impulse-sdk/dsp/ei_utils.h"
#include "stm32n6xx_hal.h"

/* Private variables ------------------------------------------------------- */

extern uint32_t        se_services_s_handle;
extern void ei_uart_baudrate_switch(int32_t baudrate);

/** Data Output Baudrate */
const ei_device_data_output_baudrate_t ei_dev_max_data_output_baudrate = {
    ei_xstr(MAX_BAUD),
    MAX_BAUD,
};

const ei_device_data_output_baudrate_t ei_dev_default_data_output_baudrate = {
    ei_xstr(DEFAULT_BAUD),
    DEFAULT_BAUD,
};

EiDeviceStm32n6::EiDeviceStm32n6(EiDeviceMemory* mem)
{
    EiDeviceInfo::memory = mem;

    init_device_id();
    load_config();

    device_type = "ST_STM32N6";

    /* Init camera instance */
    cam = static_cast<EiSTCamera*>(EiCamera::get_camera());
}

EiDeviceStm32n6::~EiDeviceStm32n6()
{
}

/**
 * @brief 
 * 
 */
void EiDeviceStm32n6::init_device_id(void)
{
    char temp[20];
    uint8_t eui[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    uint32_t uid[3];

    uid[0] = HAL_GetUIDw0();    // UID[31:0]: X and Y coordinates on the wafer expressed in BCD format
    uid[1] = HAL_GetUIDw1();    // UID[39:32]: WAF_NUM[7:0] Wafer number (8-bit unsigned number) + UID[63:40]: LOT_NUM[23:0] Lot number (ASCII encoded)
    uid[2] = HAL_GetUIDw2();    // UID[95:64]: LOT_NUM[55:24] Lot number (ASCII encoded)

    uint8_t *ptr;

    ptr = (uint8_t *)&uid[0];
    eui[0] = ptr[0];
    eui[1] = ptr[1];

    ptr = (uint8_t *)&uid[1];
    eui[2] = ptr[0];
    eui[3] = ptr[1];

    ptr = (uint8_t *)&uid[2];
    eui[4] = ptr[0];

    snprintf(temp, sizeof(temp), "%02x:%02x:%02x:%02x:%02x",
        eui[4],
        eui[3],
        eui[2],
        eui[1],
        eui[0]);

    device_id = std::string(temp);
}

/**
 * @brief      Set output baudrate to max
 *
 */
void EiDeviceStm32n6::set_max_data_output_baudrate(void)
{
    ei_uart_baudrate_switch(MAX_BAUD);
}

/**
 * @brief      Set output baudrate to default
 *
 */
void EiDeviceStm32n6::set_default_data_output_baudrate(void)
{
    ei_uart_baudrate_switch(DEFAULT_BAUD);
}

/**
 * @brief      Create resolution list for snapshot setting
 *             The studio and daemon require this list
 * @param      snapshot_list       Place pointer to resolution list
 * @param      snapshot_list_size  Write number of resolutions here
 *
 * @return     False if all went ok
 */
EiSnapshotProperties EiDeviceStm32n6::get_snapshot_list(void)
{
    ei_device_snapshot_resolutions_t **res;
    uint8_t res_num;

    cam->get_resolutions(res, &res_num);

    EiSnapshotProperties props = {
        .has_snapshot = true,
        .support_stream = true,
        .color_depth = "RGB",
        .resolutions_num = res_num,
        .resolutions = *res
    };

    return props;
}

bool EiDeviceStm32n6::get_sensor_list(const ei_device_sensor_t **p_sensor_list, size_t *sensor_list_size)
{
    *p_sensor_list = sensors;
    *sensor_list_size = ARRAY_LENGTH(sensors);
    return true;
}

/**
 * @brief get_device is a static method of EiDeviceInfo class
 * It is used to implement singleton paradigm, so we are returning
 * here pointer always to the same object (dev)
 * 
 * @return EiDeviceInfo* 
 */
EiDeviceInfo* EiDeviceInfo::get_device(void)
{
    // __attribute__((aligned(32), section(".bss.device_info")))  static EiDeviceRAM<262144, 4> memory(sizeof(EiConfig));
    static EiDeviceRAM<262144, 4> memory(sizeof(EiConfig));
    static EiDeviceStm32n6 dev(&memory);

    return &dev;
}

bool EiDeviceStm32n6::start_sample_thread(void (*sample_read_cb)(void), float sample_interval_ms)
{
    this->is_sampling = true;
    this->sample_read_callback = sample_read_cb;
    this->sample_interval_ms = sample_interval_ms;

#if MULTI_FREQ_ENABLED == 1
    this->actual_timer = 0;
    this->fusioning = 1;        //
#endif
    
    // timer_sensor_start(this->sample_interval_ms);

    return true;
}

bool EiDeviceStm32n6::stop_sample_thread(void)
{
    // timer_sensor_stop();

    this->set_state(eiStateIdle);
    this->is_sampling = false;

    return true;
}

void EiDeviceStm32n6::sample_thread(void)
{       
#if MULTI_FREQ_ENABLED == 1
    if (this->fusioning == 1) {
        if (timer_sensor_get() == true)
        {
            if (this->sample_read_callback != nullptr) {
                this->sample_read_callback();
            }
        }
    }
    else {
        uint8_t flag = 0;
        uint8_t i = 0;

        this->actual_timer += (uint32_t)this->sample_interval;

        if (timer_sensor_get() == true) {

            for (i = 0; i < this->fusioning; i++) {
                if (((uint32_t)(this->actual_timer % (uint32_t)this->multi_sample_interval[i])) == 0) {
                    flag |= (1<<i);
                }
            }

            if (this->sample_multi_read_callback != nullptr) {
                this->sample_multi_read_callback(flag);
            }
        }
    }
#else
    // if (timer_sensor_get() == true)
    // {
    //     if (this->sample_read_callback != nullptr) {
    //         this->sample_read_callback();
    //     }
    // }

#endif
}

#if MULTI_FREQ_ENABLED == 1
/**
 *
 * @param sample_read_cb
 * @param multi_sample_interval_ms
 * @param num_fusioned
 * @return
 */
bool EiDeviceStm32n6::start_multi_sample_thread(void (*sample_multi_read_cb)(uint8_t), float* multi_sample_interval_ms, uint8_t num_fusioned)
{
    uint8_t i;
    uint8_t flag = 0;

    this->is_sampling = true;
    this->sample_multi_read_callback = sample_multi_read_cb;
    this->fusioning = num_fusioned;

    this->multi_sample_interval.clear();

    for (i = 0; i < num_fusioned; i++) {
        this->multi_sample_interval.push_back(1.f/multi_sample_interval_ms[i]*1000.f);
    }

    this->sample_interval = ei_fusion_calc_multi_gcd(this->multi_sample_interval.data(), this->fusioning);

    /* force first reading */
    for (i = 0; i < this->fusioning; i++) {
            flag |= (1<<i);
    }
    this->sample_multi_read_callback(flag);

    this->actual_timer = 0;
    timer_sensor_start((uint32_t)this->sample_interval);

    return true;
}
#endif