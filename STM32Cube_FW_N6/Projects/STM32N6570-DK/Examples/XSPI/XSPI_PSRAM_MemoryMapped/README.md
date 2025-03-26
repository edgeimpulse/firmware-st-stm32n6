## <b>XSPI_PSRAM_MemoryMapped Example Description</b>

How to use an XSPI PSRAM memory in memory-mapped mode.

This example describes how to write and read data in memory-mapped mode in an XSPI
PSRAM memory and compare the result.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32N6xx Devices :

 - The SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 600 MHz.
 - The template project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
   the Layer 1 Core Instruction and Data Caches. This is provided as template implementation that
   the User may integrate in his application in order to enhance the performance.

LED_GREEN is turned ON, when comparison is good.
LED_RED is ON as soon as a comparison error occurs.
LED_RED toggles as soon as an error is returned by HAL API.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

FSBL, Memory, XSPI, Memory Mapped, PSRAM

### <b>Directory contents</b>

  - XSPI/XSPI_PSRAM_MemoryMapped/FSBL/Src/main.c                       Main program.
  - XSPI/XSPI_PSRAM_MemoryMapped/FSBL/Src/stm32n6xx_hal_msp.c          HAL MSP module.
  - XSPI/XSPI_PSRAM_MemoryMapped/FSBL/Src/stm32n6xx_it.c               Interrupt handlers.
  - XSPI/XSPI_PSRAM_MemoryMapped/FSBL/Src/system_stm32n6xx_fsbl.c      STM32N6xx system clock configuration file
  - XSPI/XSPI_PSRAM_MemoryMapped/FSBL/Inc/main.h                       Header for main.c module.
  - XSPI/XSPI_PSRAM_MemoryMapped/FSBL/Inc/stm32n6xx_hal_conf.h         HAL Configuration file.
  - XSPI/XSPI_PSRAM_MemoryMapped/FSBL/Inc/stm32n6xx_it.h               Interrupt handlers header file.
  - XSPI/XSPI_PSRAM_MemoryMapped/FSBL/Inc/stm32n6570_discovery_conf.h  BSP Configuration file.


### <b>Hardware and Software environment</b>

  - This example runs on STM32N6xx devices.

  - This example has been tested with STMicroelectronics STM32N6570-DK
    boards and can be easily tailored to any other supported device
    and development board.

  - On STM32N6570-DK board, the BOOT0 mechanical slide switch must be set to SW1.


### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn't matter).
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

