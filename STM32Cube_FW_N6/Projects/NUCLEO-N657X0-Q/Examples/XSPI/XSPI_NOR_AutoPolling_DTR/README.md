## <b>XSPI_NOR_AutoPolling_DTR Example Description</b>

How to use an XSPI NOR memory in Automatic polling mode.

This example describes how to write and read data in Automatic polling mode in an XSPI 
NOR memory and compare the result.

This project provides a reference FSBL LRUN template based on the STM32Cube HAL API that can be used 
to build any firmware application to execute in internal RAM (sub-project Appli). 
After bootROM execution, the FSBL project executes in internal RAM, ensures proper MPU, 
caches and clock setting then copies the application binary from external Flash to internal RAM. When done, 
the program counter is set to the application entry point and the application in turn executes.

- This project is targeted to run on STM32N657X0H3QU device on NUCLEO-N657X0-Q board from STMicroelectronics.  
- The FSBL project configures the maximum system clock frequency at 600 MHz.
LED_GREEN LED is ON when the checked data is correct.  
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

Memory, XSPI, Read, Write, auto polling, NOR, DTR Mode

### <b>Directory contents</b>

  - XSPI/XSPI_NOR_AutoPolling_DTR/FSBL/Inc/stm32n6xx_hal_conf.h    HAL configuration file
  - XSPI/XSPI_NOR_AutoPolling_DTR/FSBL/Inc/stm32n6xx_it.h          Interrupt handlers header file
  - XSPI/XSPI_NOR_AutoPolling_DTR/FSBL/Inc/main.h                  Header for main.c module
  - XSPI/XSPI_NOR_AutoPolling_DTR/FSBL/Src/stm32n6xx_it.c          Interrupt handlers
  - XSPI/XSPI_NOR_AutoPolling_DTR/FSBL/Src/main.c                  Main program
  - XSPI/XSPI_NOR_AutoPolling_DTR/FSBL/Src/stm32n6xx_hal_msp.c     HAL MSP module
  - XSPI/XSPI_NOR_AutoPolling_DTR/FSBL/Src/system_stm32n6xx.c      STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example has been tested with STMicroelectronics NUCLEO-N657X0-Q (MB1940)
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
 
