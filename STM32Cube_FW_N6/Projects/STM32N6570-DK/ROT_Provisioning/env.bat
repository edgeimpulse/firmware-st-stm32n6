@ECHO OFF

:: ==============================================================================
::                                    General
:: ==============================================================================
:: Configure tools installation path
set stm32tool_path=C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer-N6\bin

set stm32programmercli=%stm32tool_path%\STM32_Programmer_CLI.exe
set stm32tpccli=%stm32tool_path%\STM32TrustedPackageCreator_CLI.exe
set stm32signingtoolcli=%stm32tool_path%\STM32MP_SigningTool_CLI.exe
set stm32ExtLoaderFlash=%stm32tool_path%\ExternalLoader\MX66UW1G45G_STM32N6570-DK.stldr
set stm32ExtOTPInterace=%stm32tool_path%\ExternalLoader\OTP_FUSES_STM32N6xx.stldr
set imgtool=%stm32tool_path%\Utilities\Windows\imgtool.exe

:: ==============================================================================
::                                OEMiROT bootpath
:: ==============================================================================
:: Select application project below
set oemurot_boot_path_project=Applications\ROT\OEMuROT_Appli

:: ==============================================================================
::               !!!! DOT NOT EDIT --- UPDATED AUTOMATICALLY !!!!
:: ==============================================================================
set cube_fw_path=%~dp0..\..\..\
set oemurot_appli_s=rot_s_app_enc_sign.bin
set oemurot_appli_ns=rot_ns_app_enc_sign.bin
set rot_provisioning_path=%~dp0
