@ECHO OFF
:: Getting the CubeProgammer_cli path 
call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

set "projectdir=%~dp0"

set s_data_image_number=0x1
set ns_data_image_number=0x1

set s_data_xml="%projectdir%Images\OEMuROT_S_Data_Image.xml"
set ns_data_xml="%projectdir%Images\OEMuROT_NS_Data_Image.xml"
set flash_programming="flash_programming.bat"

set flash_log="flash_programming.log"
set provisioning_log="provisioning.log"

set appli_dir="../../%oemurot_boot_path_project%"

:prov
echo =====
echo ===== Provisioning of OEMuRoT boot path
echo ===== Application selected through env.bat:
echo =====   %oemurot_boot_path_project%
echo =====
echo.

:: Check if application path exists
if not exist %appli_dir% (
echo =====
echo ===== Wrong Boot path: %oemurot_boot_path_project%
echo ===== please modify the env.bat to set the right path
goto step_error
)

:: ======================================================== Images generation steps ========================================================
:image_generation
echo    * Boot firmware image generation
echo        Open the OEMuROT_Boot project with preferred toolchain and rebuild all files.
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

echo    * Application firmware image generation
echo        Open the OEMuROT_Appli project with preferred toolchain and rebuild all files.
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

if "%s_data_image_number%" == "0x1" (
echo    * Data Secure image generation...
"%stm32tpccli%" -pb "%s_data_xml%" > %provisioning_log%
if !errorlevel! neq 0 goto :step_error
)

if "%ns_data_image_number%" == "0x1" (
echo    * Data Non-Secure image generation...
"%stm32tpccli%" -pb "%ns_data_xml%" >> %provisioning_log%
if !errorlevel! neq 0 goto :step_error
)

:: ================================================== Flash programming ===================================================
set "action=Programming the option bytes and flashing the images..."
set current_log_file=%flash_log%
set "command=start /w /b call %flash_programming% AUTO"
echo    * %action%
%command% > %flash_log%

if !errorlevel! neq 0 goto :step_error

echo        Successful option bytes programming and images flashing
echo        (see %flash_log% for details)
echo.

:: ============================================================= End functions =============================================================
:: All the steps to set the STM32N6 product were executed correctly
:final_step
echo =====
echo ===== The board is correctly configured.
echo ===== Connect UART console (115200 baudrate) to get application menu.
echo =====

:: Keep the window open
cmd /k

exit 0

:: Error when a command fails
:step_error
echo.
echo =====
echo ===== Error while executing "%action%".
echo ===== See %current_log_file% for details. Then try again.
echo =====
cmd /k
exit 1