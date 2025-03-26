@echo off
call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Data updated with the postbuild of OEMuROT-Boot
set bootaddress=0x70000000
set appli_s_address=0x70080000
set data_s_address=0x700A0000
set appli_ns_address=0x700B0000
set data_ns_address=0x700D0000
set s_data_image_number=0x1
set ns_data_image_number=0x1

set bootpath=%cube_fw_path%\Projects\STM32N6570-DK\Applications\ROT\OEMuROT_Boot\Binary\OEMuROT_Boot.bin
set signedbootpath=%cube_fw_path%\Projects\STM32N6570-DK\Applications\ROT\OEMuROT_Boot\Binary\OEMuROT_Boot_Signed.bin
set appli_s_path=..\..\%oemurot_boot_path_project%\Binary\%oemurot_appli_s%
set appli_ns_path=..\..\%oemurot_boot_path_project%\Binary\%oemurot_appli_ns%
set data_s_path=Binary\s_data_enc_sign.bin
set data_ns_path=Binary\ns_data_enc_sign.bin

:: CubeProgrammer connection
set connect_no_reset=-c port=SWD ap=1 speed=fast mode=Hotplug
set connect_reset=-c port=SWD ap=1 speed=fast mode=UR
set erase_all=-e all

:: ========================================================= Configure Option Bytes ========================================================

set "action=Generate boot signed image"
echo %action%
set command="%stm32signingtoolcli%" -bin "%bootpath%" -nk -of 0x80000000 -t fsbl -o "%signedbootpath%" -hv 2.3 -dump "%signedbootpath%" --silent
!command!
IF !errorlevel! NEQ 0 goto :error

:: ============================================================ Download images ============================================================

set "action=Mass erase"
echo %action%
set command="%stm32programmercli%" "%connect_no_reset%" -el "%stm32ExtLoaderFlash%" -e [0 14]
!command!
IF !errorlevel! NEQ 0 goto :error
echo "Mass erase done"

set "action=Write OEMuROT_Boot"
echo %action%
set command="%stm32programmercli%" "%connect_no_reset%" -el "%stm32ExtLoaderFlash%" -d "%signedbootpath%" %bootaddress% -v
!command!
IF !errorlevel! NEQ 0 goto :error
echo "OEMuROT_Boot Written"

echo "Application images programming in download slots"

set "action=Write OEMuROT_Appli Secure"
echo %action%
set command="%stm32programmercli%" %connect_no_reset% -el %stm32ExtLoaderFlash% -d %appli_s_path% %appli_s_address% -v"
!command!
IF !errorlevel! NEQ 0 goto :error
echo "OEMuROT_Appli Secure Written"

if  "%s_data_image_number%" == "0x1" (
set "action=Write OEMuROT Data Secure"
echo %action%
set command="%stm32programmercli%" %connect_no_reset% -el %stm32ExtLoaderFlash% -d %data_s_path% %data_s_address% -v"
!command!
IF !errorlevel! NEQ 0 goto :error
echo "OEMuROT Data Secure Written"
)

set "action=Write OEMuROT_Appli Non-Secure"
echo %action%
set command="%stm32programmercli%" %connect_no_reset% -el %stm32ExtLoaderFlash% -d %appli_ns_path% %appli_ns_address% -v"
!command!
IF !errorlevel! NEQ 0 goto :error
echo "OEMuROT_Appli Non-Secure Written"

if  "%ns_data_image_number%" == "0x1" (
set "action=Write OEMuROT Data Non-Secure"
echo %action%
set command="%stm32programmercli%" %connect_no_reset% -el %stm32ExtLoaderFlash% -d %data_ns_path% %data_ns_address% -v"
!command!
IF !errorlevel! NEQ 0 goto :error
echo "OEMuROT Data Non-Secure Written"
)

echo Programming success
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo      Error when trying to "%action%" >CON
echo      Programming aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1
