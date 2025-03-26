## <b>RNG_MultiRNG Example Description</b>
  

How to generate random numbers with the RNG HAL API. 

This example uses the RNG to generate 32-bit long random numbers.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.

This project runs from the internal RAM. It is launched by the bootROM after being copied from external flash to internal RAM
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).

The RNG peripheral configuration is ensured by the HAL_RNG_Init() function.

The latter is calling the HAL_RNG_MspInit() function which implements
the configuration of the needed RNG resources according to the used hardware (CLOCK, 
GPIO, DMA and NVIC).

You may update this function to change RNG configuration.

After startup, user is asked to press USER push-button.

The 8-entry array aRandom32bit[] is filled up by 32-bit long random numbers 
at each USER push-button press.

 
The random numbers can be displayed on the debugger in aRandom32bit variable.

In case of error, LED2 is toggling at a frequency of 1Hz.

### <b>Keywords</b>

Analog, RNG, Random, FIPS PUB 140-2, Analog Random number generator, Entropy, Period

### <b>Directory contents</b> 

File | Description
 --- | ---  
  - RNG/RNG_Config/FSBL/Inc/stm32n6xx_hal_conf.h         | HAL configuration file
  - RNG/RNG_Config/FSBL/Inc/stm32n6xx_it.h               | Interrupt handlers header file
  - RNG/RNG_Config/FSBL/Inc/main.h                       | Header for main.c module
  - RNG/RNG_Config/FSBL/Inc/stm32n6xx_nucleo_conf.h      | STM32N6xx_Nucleo board configuration file
  - RNG/RNG_Config/FSBL/Src/stm32n6xx_it.c               | Interrupt handlers
  - RNG/RNG_Config/FSBL/Src/main.c                       | Main program
  - RNG/RNG_Config/FSBL/Src/stm32n6xx_hal_msp.c          | HAL MSP module
  - RNG/RNG_Config/FSBL/Src/system_stm32n6xx.c           | STM32N6xx system source file


### <b>Hardware and Software environment</b> 

  - This example runs on STM32N657X0HxQ devices.
  
  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.

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
 

