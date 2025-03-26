Prerequisites:

Flashing the device requires STM32 Programmer installed min version 2.18, download from https://www.st.com/en/development-tools/stm32cubeprog.html
STM32Programmer should be added to your path.

Flashing:

Make sure the switch BOOT1 is on the right!
Launch the script for your os, argument can be:
- all : flash weights and firmware
- firmware : flash the firmware
- weights : flash weights (network_data.hex) 
- bootloader : flash the fsbl (ai_fsbl_cut_2_0.hex)

If no argument is provided, 'all' is the default one, which flash the weights and then the firmware.

After flashing is finished, switch BOOT1 to left.
BOOT0 should be left to left postion.

NOTE: The bootloader should be flashed just once!
