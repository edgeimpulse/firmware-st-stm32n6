## <b>PKA_ECDSA_Verify Example Description</b>

How to determine if a given signature is valid regarding the Elliptic curve digital signature algorithm
(ECDSA).

For this example, a test vector have been extracted from National Institute of Standards and Technology (NIST)
 - Cryptographic Algorithm Validation Program (CAVP) in order to demonstrate the usage of the hal.
This reference files can be found under:
"http://csrc.nist.gov/groups/STM/cavp/documents/dss/186-3ecdsatestvectors.zip (ZIP SigGen.rsp)"

This test vector has been chosen to demonstrate the behavior in a case where the input signature
is valid. A second input is provided where one element of the hash message has been modified to 
demonstrate the behavior in a case where the signature is invalid. Their definitions are included 
in SigVer.c. You can refer to this file for more information.

The selected curve for this example is P-256 (ECDSA-256) published by NIST in 
Federal Information Processing Standards Publication FIPS PUB 186-4. The description
of this curve is present in file Src/prime256v1.c.

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
Security, PKA, NIST, CAVP, ECDSA verification

### <b>Directory contents</b>
  
File | Description
 --- | ---  
  - PKA/PKA_ECDSA_Verify/FSBL/Inc/stm32n6xx_hal_conf.h   | HAL configuration file
  - PKA/PKA_ECDSA_Verify/FSBL/Inc/stm32n6xx_it.h         | Interrupt handlers header file
  - PKA/PKA_ECDSA_Verify/FSBL/Inc/main.h                 | Header for main.c module
  - PKA/PKA_ECDSA_Verify/FSBL/Src/stm32n6xx_it.c         | Interrupt handlers
  - PKA/PKA_ECDSA_Verify/FSBL/Src/main.c                 | Main program
  - PKA/PKA_ECDSA_Verify/FSBL/Src/stm32n6xx_hal_msp.c    | HAL MSP module 
  - PKA/PKA_ECDSA_Verify/FSBL/Src/system_stm32n6xx.c     | STM32N6xx system source file
  - PKA/PKA_ECDSA_Verify/FSBL/Src/prime256v1.c           | Description of P-256 (ECDSA-256)
  - PKA/PKA_ECDSA_Verify/FSBL/Inc/prime256v1.h           | Header for prime256v1.c
  - PKA/PKA_ECDSA_Verify/FSBL/Src/SigVer.c               | Reflect the content of the test vector from SigVer.rsp
  - PKA/PKA_ECDSA_Verify/FSBL/Src/SigVer.rsp             | Extract from NIST CAVP
  - PKA/PKA_ECDSA_Verify/FSBL/Inc/SigVer.h               | Header of SigVer.c

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
 
