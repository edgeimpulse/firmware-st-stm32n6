#!/bin/bash
set -e

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"

EL="$(dirname "$(which STM32_Programmer_CLI)")/ExternalLoader/MX66UW1G45G_STM32N6570-DK.stldr"

TARGET=$1

if [ -z "$TARGET" ]; then
    TARGET="all"
fi

echo ${EL}

echo "Flashing ${TARGET}"
if [ "$TARGET" == "firmware" ]; then
    STM32_Programmer_CLI -c port=SWD mode=HOTPLUG ap=1 -el ${EL} -hardRst -w firmware-st-stm32n6.bin 0x70080000
elif [ "$TARGET" == "all" ]; then
    STM32_Programmer_CLI -c port=SWD mode=HOTPLUG ap=1 -el ${EL} -hardRst -w network_data.hex
    STM32_Programmer_CLI -c port=SWD mode=HOTPLUG ap=1 -el ${EL} -hardRst -w firmware-st-stm32n6.bin 0x70080000
elif [ "$TARGET" == "weights" ]; then
    STM32_Programmer_CLI -c port=SWD mode=HOTPLUG ap=1 -el ${EL} -hardRst -w network_data.hex
elif [ "$TARGET" == "bootloader" ]; then
    STM32_Programmer_CLI -c port=SWD mode=HOTPLUG ap=1 -el ${EL} -hardRst -w ai_fsbl_cut_2_0.hex
else
    echo "Invalid target: $TARGET"
    exit 1
fi
