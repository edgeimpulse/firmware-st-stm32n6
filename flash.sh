#!/bin/bash
set -e

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
while [[ $# -gt 0 ]]; do
    case $1 in
        --target)
            TARGET="$2"
            shift # past argument
            shift # past value
            ;;
        --nucleo)
            NUCLEO=1
            shift # past argument
            ;;
        *)
            POSITIONAL_ARGS+=("$1") # save positional arg
            shift # past argument
            ;;
    esac
done

if [ "$NUCLEO" == 1 ]; then
    EL="$(dirname "$(which STM32_Programmer_CLI)")/ExternalLoader/MX25UM51245G_STM32N6570-NUCLEO.stldr"
else
    EL="$(dirname "$(which STM32_Programmer_CLI)")/ExternalLoader/MX66UW1G45G_STM32N6570-DK.stldr"
fi

if [ -z "$TARGET" ]; then
    TARGET="all"
fi

echo ${EL}

# sign bin
rm -f build/project-trusted.bin
STM32_SigningTool_CLI -s -bin build/Project.bin -nk -t fsbl -hv 2.1 -o build/project-trusted.bin

echo "Flashing ${TARGET}"
if [ "$TARGET" == "firmware" ]; then
    STM32_Programmer_CLI -c port=SWD mode=HOTPLUG ap=1 -el ${EL} -hardRst -w build/project-trusted.bin 0x70080000
elif [ "$TARGET" == "all" ]; then
    STM32_Programmer_CLI -c port=SWD mode=HOTPLUG ap=1 -el ${EL} -hardRst -w Model/network_data.hex
    STM32_Programmer_CLI -c port=SWD mode=HOTPLUG ap=1 -el ${EL} -hardRst -w build/project-trusted.bin 0x70080000
elif [ "$TARGET" == "weights" ]; then
    STM32_Programmer_CLI -c port=SWD mode=HOTPLUG ap=1 -el ${EL} -hardRst -w Model/network_data.hex
elif [ "$TARGET" == "bootloader" ]; then
    STM32_Programmer_CLI -c port=SWD mode=HOTPLUG ap=1 -el ${EL} -hardRst -w ai_fsbl_cut_2_0.hex
else
    echo "Invalid target: $TARGET"
    exit 1
fi
