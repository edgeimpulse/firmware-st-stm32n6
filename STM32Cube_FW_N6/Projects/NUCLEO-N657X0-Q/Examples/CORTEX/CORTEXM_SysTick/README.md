## <b>CORTEXM_SysTick Example Description</b>

How to use the default SysTick configuration with a 1 ms timebase to toggle LEDs.

  A "HAL_Delay" function is implemented based on the SysTick end-of-count event.
  Three LEDs are toggled with a timing defined by the HAL_Delay function.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate
    delay (in milliseconds) based on variable incremented in SysTick ISR. This
    implies that if HAL_Delay() is called from a peripheral ISR process, then 
    the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, Cortex, Time base, Systick, CPU clock 

### <b>Directory contents</b>

  - CORTEX/CORTEXM_SysTick/FSBL/Inc/stm32n6xx_nucleo_conf.h     BSP configuration file
  - CORTEX/CORTEXM_SysTick/FSBL/Inc/stm32n6xx_hal_conf.h    HAL configuration file
  - CORTEX/CORTEXM_SysTick/FSBL/Inc/stm32n6xx_it.h          Interrupt handlers header file
  - CORTEX/CORTEXM_SysTick/FSBL/Inc/main.h                  Header for main.c module
  - CORTEX/CORTEXM_SysTick/FSBL/Src/stm32n6xx_it.c          Interrupt handlers
  - CORTEX/CORTEXM_SysTick/FSBL/Src/stm32n6xx_hal_msp.c     HAL MSP file
  - CORTEX/CORTEXM_SysTick/FSBL/Src/main.c                  Main program
  - CORTEX/CORTEXM_SysTick/FSBL/Src/system_stm32n6xx.c      STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0HxQ devices.
    
  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :
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

