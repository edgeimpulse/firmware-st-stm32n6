@echo off
call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Keys folder
set projectdir=%~dp0
pushd "%projectdir%"\Keys
set keys_pem_dir=%cd%
popd

set auth_key_pair_pem=%keys_pem_dir%\OEMuRoT_Authentication.pem
set enc_key_pair_pem=%keys_pem_dir%\OEMuRoT_Encryption.pem
set auth_public_key_hash_bin=%keys_pem_dir%\OEMuRoT_Authentication_Pub_Hash.bin
set enc_private_key_bin=%keys_pem_dir%\OEMuRoT_Encryption_Priv.bin

:: Generate keypairs for signature and encryption
set command="%imgtool%" keygen -k "%auth_key_pair_pem%" -t ecdsa-p256
!command!
IF !errorlevel! NEQ 0 goto :error

set command="%imgtool%" keygen -k "%enc_key_pair_pem%" -t ecdsa-p256
!command!
IF !errorlevel! NEQ 0 goto :error

:: Generate public keys hash in binary format
set command="%imgtool%" getpubbin --sha -k "%auth_key_pair_pem%"
!command! > "%auth_public_key_hash_bin%"
IF !errorlevel! NEQ 0 goto :error

:: Generate encrpytion private key in binary format
set command="%imgtool%" getprivbin --raw --minimal -k "%enc_key_pair_pem%"
!command! > "%enc_private_key_bin%"
IF !errorlevel! NEQ 0 goto :error

echo Keys generated successfully
cmd /k
exit 0

:error
echo Failed to generate keys
echo "%command% failed"
cmd /k
exit 1