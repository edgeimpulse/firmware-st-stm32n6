## <b>PKA_ECCscalarMultiplication Example Description</b>

How to use the PKA peripheral to execute ECC scalar multiplication. This 
allows generating a public key from a private key.

For this example, a .pem file has been created using openssl external tool. (https://www.openssl.org)
This file is located under Src/ecdsa_priv_prime256v1.pem.

To ease the usage of this .pem files, 2 files, Src/ecdsa_priv_prime256v1.c and Inc/ecdsa_priv_prime256v1.h 
are present to reflect its content. The explanation of how to create those file from a .pem is embedded in 
Src/ecdsa_priv_prime256v1.c.

The selected curve for this example is P-256 (ECDSA-256) published by NIST in 
Federal Information Processing Standards Publication FIPS PUB 186-4. The description
of this curve is present in file Src/prime256v1.c. 

Those two information are provided to PKA using the PKA_ECCMulInTypeDef "in" variable.

The result of the operation is the public key part of the .pem file. So the example compare the computed
result to the content of the .pem file to ensure the validity of the operation.

In case of success, the LED1 (BLUE) is ON.

In case of any error, the LED1 (BLUE) is toggling slowly.

#### <b>Notes</b>

1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

### <b>Keywords</b>

Security, PKA, FIPS PUB 186-4, ECC scalar multiplication

### <b>Directory contents</b>
  
  - PKA/PKA_ECCscalarMultiplication/FSBL/Inc/stm32n6xx_hal_conf.h            HAL configuration file
  - PKA/PKA_ECCscalarMultiplication/FSBL/Inc/stm32n6xx_it.h                  Interrupt handlers header file
  - PKA/PKA_ECCscalarMultiplication/FSBL/Inc/main.h                          Header for main.c module
  - PKA/PKA_ECCscalarMultiplication/FSBL/Src/stm32n6xx_it.c                  Interrupt handlers
  - PKA/PKA_ECCscalarMultiplication/FSBL/Src/main.c                          Main program
  - PKA/PKA_ECCscalarMultiplication/FSBL/Src/stm32n6xx_hal_msp.c             HAL MSP module 
  - PKA/PKA_ECCscalarMultiplication/FSBL/Src/system_stm32n6xx.c              STM32N6xx system source file
  - PKA/PKA_ECCscalarMultiplication/FSBL/Src/prime256v1.c                    Description of P-256 (ECDSA-256)
  - PKA/PKA_ECCscalarMultiplication/FSBL/Inc/prime256v1.h                    Header for prime256v1.c
  - PKA/PKA_ECCscalarMultiplication/FSBL/Src/ecdsa_priv_prime256v1.pem       ecdsa private key generated using openssl
  - PKA/PKA_ECCscalarMultiplication/FSBL/Src/ecdsa_priv_prime256v1.c         Reflect the content of ecdsa_priv_prime256v1.pem in array format
  - PKA/PKA_ECCscalarMultiplication/FSBL/Inc/ecdsa_priv_prime256v1.h         Header for ecdsa_priv_prime256v1.c

### <b>Hardware and Software environment</b> 

  - This example runs on STM32N657X0HxQ devices.
  
  - This example has been tested with an STMicroelectronics NUCLEO-N657X0-Q
    board and can be easily tailored to any other supported device 
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :
 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:
 
 - Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.1 -dump Project-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in the board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.
 
