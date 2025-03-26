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
#include "main.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <cstdint>

extern UART_HandleTypeDef huart1;


char ei_getchar(void)
{
    HAL_StatusTypeDef status;
    char data[1] = {0};

    status = HAL_UART_Receive(&huart1, (uint8_t *)data, 1, 0);

    return data[0];
}

void ei_printf(const char *format, ...) {

    extern UART_HandleTypeDef huart1;
    char buffer[256] = {0};
    int length;
    va_list myargs;
    va_start(myargs, format);
    length = vsnprintf(buffer, sizeof(buffer), format, myargs);
    va_end(myargs);

    if (length > 0){
        //comms_send((uint8_t*)buffer, length, 100);
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, length, 10);
    }
    
}

void ei_uart_baudrate_switch(int32_t baudrate)
{
    huart1.Init.BaudRate = baudrate;

    HAL_UART_DeInit(&huart1);
    HAL_UART_Init(&huart1);
    HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8);
    HAL_UARTEx_EnableFifoMode(&huart1);
}