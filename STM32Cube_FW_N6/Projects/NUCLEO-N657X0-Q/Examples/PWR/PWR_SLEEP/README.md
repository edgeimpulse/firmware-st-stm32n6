## <b>PWR_SLEEP Example Description</b>

How to enter the Sleep mode and wake up from this mode by using an interrupt.

In the associated software, the system clock is set to 600 MHz.
an EXTI line is connected to the user button through PC.13 and configured to generate an interrupt on falling edge upon key press.
The SysTick is programmed to generate an interrupt each 1 ms and in the SysTick interrupt handler, LED1 is toggled in order to indicate
 whether the MCU is in SLEEP mode or RUN mode.

5 seconds after start-up, the system automatically enters SLEEP mode and LED1 stops toggling.

The USER push-button can be pressed at any time to wake-up the system.

The software then comes back in RUN mode for 5 sec. before automatically entering SLEEP mode again.

LED1 is used to monitor the system state as follows:

 - LED1 toggling: system in RUN mode
 - LED1 off : system in SLEEP mode

#### <b>Notes</b>

 1. These steps are repeated in an infinite loop.

 5. To measure the current consumption in SLEEP mode, please refer to @subpage PWR_CurrentConsumption example.

 2. This example can not be used in DEBUG mode due to the fact that the Cortex-M55 core is no longer clocked during low power mode
    so debugging features are disabled.

 3. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds) based on variable incremented in SysTick ISR.
    This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)than the peripheral interrupt. Otherwise the 
	caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 4. The application needs to ensure that the SysTick time base is always set to 1 millisecond to have correct HAL operation.

### <b>Keywords</b>

Power, PWR, EXTI, Sleep mode, Interrupt, Wakeup, External reset

### <b>Directory contents</b>

  - PWR/PWR_SLEEP/FSBL/Inc/stm32n6xx_nucleo_conf.h     BSP configuration file
  - PWR/PWR_SLEEP/FSBL/Inc/stm32n6xx_conf.h         HAL Configuration file
  - PWR/PWR_SLEEP/FSBL/Inc/stm32n6xx_it.h           Header for stm32n6xx_it.c
  - PWR/PWR_SLEEP/FSBL/Inc/main.h                         Header file for main.c
  - PWR/PWR_SLEEP/FSBL/Src/system_stm32n6xx.c       STM32N6xx system clock configuration file
  - PWR/PWR_SLEEP/FSBL/Src/stm32n6xx_it.c           Interrupt handlers
  - PWR/PWR_SLEEP/FSBL/Src/stm32n6xx_hal_msp.c      HAL MSP module
  - PWR/PWR_SLEEP/FSBL/Src/main.c                         Main program

### <b>Hardware and Software environment</b>

  - This example runs on STM32N6xx devices

  - This example has been tested with STMicroelectronics NUCLEO-N657X0-Q
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-N657X0-Q set-up:
    - LED1 connected to PG.08 pin
    - Use the USER push-button connected to pin PC.13 (External line 13)

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
