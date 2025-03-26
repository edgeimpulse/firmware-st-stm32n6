@echo off
call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Keys folder
set projectdir=%~dp0
pushd "%projectdir%"\Keys
set keys_pem_dir=%cd%
popd

set auth_public_key_bin=%keys_pem_dir%\OEMuRoT_Authentication_Public_Hash.bin
set enc_private_key_bin=%keys_pem_dir%\OEMuRoT_Encryption_Private.bin

set connect_no_reset=-c port=SWD ap=1 speed=fast mode=Hotplug

set command="%stm32programmercli%" %connect_no_reset% -el "%stm32ExtOTPInterace%" -otp fwrite "%auth_public_key_bin%" word=260
::!command!
IF !errorlevel! NEQ 0 goto :error

set command="%stm32programmercli%" %connect_no_reset% -el "%stm32ExtOTPInterace%" -otp fwrite "%auth_public_key_bin%" word=268
::!command!
IF !errorlevel! NEQ 0 goto :error

set command="%stm32programmercli%" %connect_no_reset% -el "%stm32ExtOTPInterace%" -otp fwrite "%enc_private_key_bin%" word=276
::!command!
IF !errorlevel! NEQ 0 goto :error

echo Keys programmed successfully
cmd /k
exit 0

:error
echo Error
cmd /k
exit 1