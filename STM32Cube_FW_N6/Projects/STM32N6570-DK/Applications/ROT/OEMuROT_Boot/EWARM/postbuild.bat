@ECHO OFF
:: Getting the Trusted Package Creator and STM32CubeProgammer CLI path
set "projectdir=%~dp0"
pushd %projectdir%\..\..\..\..\ROT_Provisioning
set provisioningdir=%cd%
popd
call "%provisioningdir%\env.bat"

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Environment variable for log file
set current_log_file="%projectdir%\postbuild.log"
echo. > %current_log_file%

:exe
::line for window executable
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
if exist %applicfg% (
echo run config Appli with windows executable
goto postbuild
)
:py
::line for python
echo run config Appli with python script
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python= "

:postbuild
set "appli_dir=..\..\..\..\%oemurot_boot_path_project%"
set preprocess_bl2_file="%projectdir%\image_macros_preprocessed_bl2.c"

:: Environment variable for AppliCfg
set provisioning="%projectdir%\..\..\..\..\ROT_Provisioning\OEMuROT\provisioning.bat"
set flash_programming="%projectdir%\..\..\..\..\ROT_Provisioning\OEMuROT\flash_programming.bat"

set appli_s_linker_file="%appli_dir%\EWARM\AppliSecure\stm32n657xx_ROMxspi2_axisram_s.icf"
set appli_ns_linker_file="%appli_dir%\EWARM\AppliNonSecure\stm32n657xx_ROMxspi2_axisram_ns.icf"

:provisioning
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b s_data_image_number -m RE_S_DATA_IMAGE_NUMBER %provisioning% --vb > %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b ns_data_image_number -m RE_NS_DATA_IMAGE_NUMBER %provisioning% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:flashprogramming
REM set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b s_data_image_number -m RE_S_DATA_IMAGE_NUMBER %flash_programming% --vb >> %current_log_file% 2>&1"
REM %command%
REM IF !errorlevel! NEQ 0 goto :error

REM set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b ns_data_image_number -m RE_NS_DATA_IMAGE_NUMBER %flash_programming% --vb >> %current_log_file% 2>&1"
REM %command%
REM IF !errorlevel! NEQ 0 goto :error

REM set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b appli_s_address -m RE_FLASH_AREA_2_ADDRESS %flash_programming% --vb >> %current_log_file% 2>&1"
REM %command%
REM IF !errorlevel! NEQ 0 goto :error

REM set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b data_s_address -m RE_FLASH_AREA_6_ADDRESS %flash_programming% --vb >> %current_log_file% 2>&1"
REM %command%
REM IF !errorlevel! NEQ 0 goto :error

REM set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b appli_ns_address -m RE_FLASH_AREA_3_ADDRESS %flash_programming% --vb >> %current_log_file% 2>&1"
REM %command%
REM IF !errorlevel! NEQ 0 goto :error

REM set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b data_ns_address -m RE_FLASH_AREA_7_ADDRESS %flash_programming% --vb >> %current_log_file% 2>&1"
REM %command%
REM IF !errorlevel! NEQ 0 goto :error

:appli
set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_BASE_ADDRESS_APP_S -n BASE_ADDRESS %appli_s_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_FLASH_AREA_0_OFFSET -n CODE_S_OFFSET %appli_s_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_FLASH_S_PARTITION_SIZE -n CODE_S_SIZE %appli_s_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_BASE_ADDRESS_APP_NS -n BASE_ADDRESS %appli_s_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_FLASH_AREA_1_OFFSET -n CODE_NS_OFFSET %appli_ns_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_FLASH_NS_PARTITION_SIZE -n CODE_NS_SIZE %appli_ns_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error