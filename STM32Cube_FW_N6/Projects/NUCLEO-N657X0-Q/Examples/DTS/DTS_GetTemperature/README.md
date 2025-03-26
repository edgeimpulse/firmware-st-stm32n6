## <b>DTS_GetTemperature Example Description</b>

How to configure and use the DTS to get the temperature of the die.

      - Board: NUCLEO-N657X0-Q (embeds a STM32N657X0HxQ device)

At the beginning of the main program the HAL_Init() function is called to initialize the systick.
Then the SystemClock_Config() function is used to configure the CPU clock to run at 600 MHz.

The DTS is configured to get the temperature.
The Temperature variable will hold the temperature value in °C unit.
The Temperature value can be displayed on the debugger in "Temperature" variable.

NUCLEO-N657X0-Q board's LEDs can be used to monitor the process status:
  - LED2 toggles and example is stopped (using infinite loop) when there is an error during process.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

DTS, Sensor, Temperature, Die

### <b>Directory contents</b>

  - DTS/DTS_GetTemperature/FSBL/Inc/stm32n6xx_nucleo_conf.h BSP configuration file
  - DTS/DTS_GetTemperature/FSBL/Inc/stm32n6xx_hal_conf.h    HAL configuration file
  - DTS/DTS_GetTemperature/FSBL/Inc/stm32n6xx_it.h          HAL interrupt handlers header file
  - DTS/DTS_GetTemperature/FSBL/Inc/main.h                  Header for main.c module
  - DTS/DTS_GetTemperature/FSBL/Src/stm32n6xx_it.c          HAL interrupt handlers
  - DTS/DTS_GetTemperature/FSBL/Src/main.c                  Main program
  - DTS/DTS_GetTemperature/FSBL/Src/stm32n6xx_hal_msp.c     HAL MSP file
  - DTS/DTS_GetTemperature/FSBL/Src/system_stm32n6xx.c      STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0HxQ devices.

  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.

  - To get the temperature information, put the "Temperature" variable in the debugger watch.

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
