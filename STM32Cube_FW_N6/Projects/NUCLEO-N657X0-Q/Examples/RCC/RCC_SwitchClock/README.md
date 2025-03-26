
## <b>RCC_SwitchClock Example Description</b>

This example describes how to use the RCC HAL API to configure the CPU and system buses clocks and
modify the clock settings on run time.

After startup, the HAL_Init() function is called to reset all the peripherals and initialize the systick used as 1ms HAL timebase. The SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 400 Mhz and the CPU clock at 800 MHz.

In this example, after startup CPU and system buses clocks are configured to the max frequency using the PLL with HSI as clock source. The B1 user push-button (connected to EXTI_Line13) will be used to change the CPU and system buses clocks source:
- from PLL1 with HSI source to PLL1 with HSE source
- from PLL1 with HSE source to PLL2 with HSI source
- from PLL2 with HSI source to PLL2 with HSE source
- from PLL2 with HSE source to HSI source
- from HSI source to HSE source
- from HSE source to PLL1 with HSI source

Every time the user push-button is pressed EXTI_Line13 interrupt is generated and in the ISR a flag is set.
In the main loop, when this flag is set, the next System and buses Clocks source is selected:

    - If PLL1 with HSI source is selected as CPU and system buses clocks source, the following steps will be followed to switch CPU and system buses clocks to PLL1 with HSE:
         1. Select HSI output as CPU and System bus clocks source (To be able to reconfigure PLL)
         2. Enable HSE and Configure PLL1
         3. Select PLL1 output as CPU and System bus clocks source
         4. Disable HSI Oscillator (Optional, if HSI is no more needed by the application)

    - If PLL1 with HSE source is selected as CPU and system buses clocks source, the following steps will be followed to switch CPU and system buses clocks to PLL2 with HSI:
         1. Enable HSI and Configure PLL2
         2. Select PLL2 output as CPU and System bus clocks source
         3. Disable PLL1 Oscillator (Optional, if PLL1 is no more needed by the application)
         4. Disable HSE Oscillator (Optional, if HSE is no more needed by the application)

    - If PLL2 with HSI source is selected as CPU and system buses clocks source, the following steps will be followed to switch CPU and system buses clocks to PLL2 with HSE:
         1. Select HSI output as CPU and System bus clocks source (To be able to reconfigure PLL)
         2. Enable HSE and Configure PLL2
         3. Select PLL2 output as CPU and System bus clocks source
         4. Disable HSI Oscillator (Optional, if HSI is no more needed by the application)

    - If PLL2 with HSE source is selected as CPU and system buses clocks source, the following steps will be followed to switch CPU and system buses clocks to HSI:
         1. Enable HSI
         2. Select HSI output as CPU and System bus clocks source
         3. Disable PLL2 Oscillator (Optional, if PLL2 is no more needed by the application)
         4. Disable HSE Oscillator (Optional, if HSE is no more needed by the application)

    - If HSI source is selected as CPU and system buses clocks source, the following steps will be followed to switch CPU and system buses clocks to HSE:
         1. Enable HSE
         2. Select HSE output as CPU and System bus clocks source
         3. Disable HSI Oscillator (Optional, if HSI is no more needed by the application)

    - If HSE source is selected as CPU and system buses clocks source, the following steps will be followed to switch CPU and system buses clocks to PLL1 with HSI:
         1. Enable HSI and Configure PLL1 (Also re-able PLL2 as this was the previous selected source for Icx)
         2. Select PLL1 output as CPU and System bus clocks source
         3. Disable HSE Oscillator (Optional, if HSE is no more needed by the application)


- In this example the CPU clock (sysa_ck) is outputted on the MCO1 pin(PA.08).
- LED1 is toggling every time the user push button is pressed
- LED2 is toggling at 10Hz.



#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, CPU, RCC, PLL, PLLCLK, SYSCLK, HSI, HSE, Switch Clock, Oscillator

### <b>Directory contents</b>

      - RCC/RCC_SwitchClock/FSBL/Inc/main.h                       Header for main.c module
      - RCC/RCC_SwitchClock/FSBL/Inc/stm32n6xx_hal_conf.h         HAL Configuration file
      - RCC/RCC_SwitchClock/FSBL/Inc/stm32n6xx_nucleo_conf.h      BSP Configuration file
      - RCC/RCC_SwitchClock/FSBL/Inc/stm32n6xx_it.h               Interrupt handlers header file
      - RCC/RCC_SwitchClock/FSBL/Src/main.c                       Main program
      - RCC/RCC_SwitchClock/FSBL/Src/stm32n6xx_hal_msp.c          HAL MSP module
      - RCC/RCC_SwitchClock/FSBL/Src/stm32n6xx_it.c               Interrupt handlers
      - RCC/RCC_SwitchClock/FSBL/Src/system_stm32n6xx_fsbl.c      STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N6xx devices.

  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-N657X0-Q Set-up
     - Connect MCO1 pin (pin 1 of U7 component) to an oscilloscope to monitor the system clock.

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
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in NUCLEO-N657X0-Q board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.
 - Run the example

