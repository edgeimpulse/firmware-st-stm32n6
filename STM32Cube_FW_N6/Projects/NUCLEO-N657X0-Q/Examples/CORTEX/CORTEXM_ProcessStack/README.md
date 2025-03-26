## <b>CORTEXM_ProcessStack Example Description</b>

How to modify the Thread mode stack. Thread mode is entered on reset, and can be
entered as a result of an exception return.

The associated program is used to:

1. Switch the Thread mode stack from Main stack to Process stack

2. Generate ISR (SVC) to confirm Stack pointer used under exception is Main Stack

3. Returning to main(), check is performed on current Stack to show cortex has switched back to Process
automatically.
In case of error, LED2 remains off. In case of success, LED1 is turned on.

To monitor the stack used a set of variables is available within the program. It is also
possible to use the 'Cortex register' window of the debugger.

#### <b>Notes</b>

1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
   based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
   a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
   than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
   To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

2. The application need to ensure that the SysTick time base is always set to 1 millisecond
   to have correct HAL operation.

### <b>Keywords</b>

System, Cortex, Process Stack, HCLK, Thread mode Stack, main stack, process stack, ISR,

### <b>Directory contents</b>

  - CORTEX/CORTEXM_ProcessStack/FSBL/Inc/stm32n6xx_nucleo_conf.h BSP configuration file
  - CORTEX/CORTEXM_ProcessStack/FSBL/Inc/stm32n6xx_hal_conf.h    HAL configuration file
  - CORTEX/CORTEXM_ProcessStack/FSBL/Inc/stm32n6xx_it.h          Interrupt handlers header file
  - CORTEX/CORTEXM_ProcessStack/FSBL/Inc/main.h                  Header for main.c module
  - CORTEX/CORTEXM_ProcessStack/FSBL/Src/stm32n6xx_it.c          Interrupt handlers
  - CORTEX/CORTEXM_ProcessStack/FSBL/Src//stm32n6xx_hal_msp.c    HAL MSP file
  - CORTEX/CORTEXM_ProcessStack/FSBL/Src/main.c                  Main program
  - CORTEX/CORTEXM_ProcessStack/FSBL/Src/system_stm32n6xx.c      STM32N6xx system source file

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
