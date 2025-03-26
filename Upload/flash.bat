@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION
setlocal
REM go to the folder where this bat script is located
cd /d %~dp0


set FLASH_FIRMWARE=firmware
set FLASH_WEIGHTS=weights
set FLASH_BOOTLOADER=bootloader
set FLASHER = STM32_Programmer_CLI
set EL = \\ExternalLoader\\MX66UW1G45G_STM32N6570-DK.stldr

set TARGET=%1

if not defined TARGET SET TARGET="all"

if %TARGET% NEQ "firmware" and %TARGET% NEQ "weights" and %TARGET% NEQ "bootloader" and %TARGET% NEQ "all" goto INVALIDTARGET

echo Flashing %TARGET%

IF %COMMAND% == %FLASH_WEIGHTS% goto :FLASH_W
IF %COMMAND% == %FLASH_BOOTLOADER% goto :FLASH_B
IF %COMMAND% == %FLASH_FIRMWARE% goto :FLASH_F

%FLASHER% -c port=SWD mode=HOTPLUG ap=1 -el $(EL) -hardRst -w network_data.hex
%FLASHER% -c port=SWD mode=HOTPLUG ap=1 -el $(EL) -hardRst -w firmware-st-stm32n6.bin 0x70080000

goto :COMMON_EXIT

:FLASH_F
    %FLASHER% -c port=SWD mode=HOTPLUG ap=1 -el $(EL) -hardRst -w firmware-st-stm32n6.bin 0x70080000
goto :COMMON_EXIT

:FLASH_W
    %FLASHER% -c port=SWD mode=HOTPLUG ap=1 -el $(EL) -hardRst -w network_data.hex
goto :COMMON_EXIT

:FLASH_B
    %FLASHER% -c port=SWD mode=HOTPLUG ap=1 -el $(EL) -hardRst -w ai_fsbl_cut_2_0.hex
goto :COMMON_EXIT

:INVALIDTARGET

echo %TARGET% is an invalid target!
exit \b 1


:COMMON_EXIT
