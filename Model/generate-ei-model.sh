#!/bin/bash

stedgeai generate --model ei-conference-dataset-person-logo-only-object-detection-tensorflow-lite-int8-quantized-model.lite --target stm32n6 --st-neural-art default@user_neuralart.json -t tflite
cp st_ai_output/network.c .
cp st_ai_output/network_atonbuf.xSPI2.raw network_data.xSPI2.bin
arm-none-eabi-objcopy -I binary network_data.xSPI2.bin --change-addresses 0x70180000 -O ihex network_data.hex
rm network_data.xSPI2.bin