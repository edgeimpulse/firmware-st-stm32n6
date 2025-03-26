@ECHO OFF

:: arg1 is the binary type (1 nonsecure, 2 secure)
set "signing=%1"

:: Getting the Trusted Package Creator CLI path
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

if %signing% == "secure" (
echo Creating secure image >> %current_log_file% 2>&1
"%stm32tpccli%" -pb "%provisioningdir%\OEMuROT\Images\OEMuROT_S_Code_Image.xml" >> %current_log_file% 2>&1
IF !errorlevel! NEQ 0 goto :error
)

if %signing% == "nonsecure" (
echo Creating non-secure image >> %current_log_file% 2>&1
"%stm32tpccli%" -pb "%provisioningdir%\OEMuROT\Images\OEMuROT_NS_Code_Image.xml" >> %current_log_file% 2>&1
IF !errorlevel! NEQ 0 goto :error
)

exit 0

:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo =====
exit 1
