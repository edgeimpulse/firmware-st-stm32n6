## <b>MCE_NoekeonEncryptDecryptData Example Description</b>

This project provides a description of how to encrypt and decrypt data from external memory (PSRAM).
The encryption/decryption of the data is performed on the fly using the MCE IP(Noekeon encryption algorithm).


At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32N6xx Devices.  

STM32N6570-DK board's LED can be used to monitor the execution status:
 - [**LD1**]{style="color: GREEN"} toggles when the the encrypted data is successfully done
 - [**LD2**]{style="color: RED"}  toggles as soon as an error occurs. 

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

MCE, Cryptography, Security, Noekeon, Basics

### <b>Directory contents</b>

File                                                                   | Description
 --- | ---
  MCE/MCE_AESEncryptDecryptData/FSBL/Inc/stm32n6xx_hal_conf.h          |  HAL configuration file  
  MCE/MCE_AESEncryptDecryptData/FSBL/Inc/stm32n6xx_it.h                |  Interrupt handlers header file
  MCE/MCE_AESEncryptDecryptData/FSBL/Inc/main.h                        |  Header for main.c module
  MCE/MCE_AESEncryptDecryptData/FSBL/Inc/stm32n6570_discovery_conf.h   |  BSP Configuration file
  MCE/MCE_AESEncryptDecryptData/FSBL/Src/stm32n6xx_it.c                |  Interrupt handlers
  MCE/MCE_AESEncryptDecryptData/FSBL/Src/main.c                        |  Main program
  MCE/MCE_AESEncryptDecryptData/FSBL/Src/stm32n6xx_hal_msp.c           |  HAL MSP module
  MCE/MCE_AESEncryptDecryptData/FSBL/Src/system_stm32n6xx.c            |  STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0H3QU devices.
  - In this example, the clock is set to 600 MHz.

  - This example has been tested with STMicroelectronics STM32N6570-DK (MB1939)
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn’t matter).
 - Open your preferred toolchain
- Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:
 
 - Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command
 - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.1 -dump Project-trusted.bin*
 - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.


