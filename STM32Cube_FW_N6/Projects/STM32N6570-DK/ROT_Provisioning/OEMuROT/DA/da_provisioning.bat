call ../../env.bat

:: Password binary
set projectdir=%~dp0
set password_bin=%projectdir%\password.bin
set hash_bin=%projectdir%\hash.bin

set connect_no_reset=-c port=SWD ap=1 speed=fast mode=Hotplug

::line for window executable
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
if exist %applicfg% (
echo run da_provisioning with windows executable
goto hash
)
:py
::line for python
echo run da_provisioning with python script
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python=python "

:hash
:: Generate password hash binary
%python%%applicfg% hashcontent %hash_bin% -i %password_bin%

:: Write password hash in OTP bits
set command="%stm32programmercli%" %connect_no_reset% -el "%stm32ExtOTPInterace%" -otp fwrite "%hash_bin%" word=284
::!command!
IF !errorlevel! NEQ 0 goto :error

del %hash_bin%
cmd /k
exit 0

:error
echo Error
del %hash_bin%
cmd /k
exit 1