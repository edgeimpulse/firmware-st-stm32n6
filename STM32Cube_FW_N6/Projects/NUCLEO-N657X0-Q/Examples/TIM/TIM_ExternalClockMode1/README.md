## <b>TIM_ExternalClockMode1 Example Description</b>

This example shows how to configure the TIM peripheral in external clock mode 1 and use the button as a clock source to light a LED after 5 presses.

- At the beginning of the main program the HAL_Init() function is called to reset all the peripherals,
initialize the Flash interface and the systick. Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 600 MHz.

- The objective is to configure TIM1 channel 2 in input capture mode and select this signal (tim_ti2_fp2) as a clock
source. To do so, tim_ti2_fp2 is chosen as trigger input source for the slave mode controller in external clock mode 1.

- To prevent button bounce, the button signal is not directly redirected to the timer input. The button pin is
configured in interrupt mode for the rising edges. Thanks to this event, a 250ms pulse is generated on an intermediate
GPIO (TIM_IN_GPIO) and redirected to the timer input. During this amount of time, no other button events are considered.

- The TIM1 channel 1 is configured in output compare mode so that the compare match event is used to light the LED.
Because the output is meaning less, it is frozen.

- The following configuration values are used for TIM1:  
      - Prescaler = 0  
      - Repetition counter = 0  
      - Autoreload = 5  
      - Pulse (CCR1) = 5

NUCLEO-N657X0-Q board's LEDs can be used to monitor the example status:
- LED_GREEN is ON when the button is pressed 5 times.
- LED_RED is slowly blinking when there is an error.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

TIM, Input, Clock source, External Clock Mode 1, Non-periodic clock, Button.

### <b>Sub-project FSBL directory contents</b>

      - TIM/TIM_ExternalClockMode1/FSBL/Inc/main.h                       Header for main.c module
      - TIM/TIM_ExternalClockMode1/FSBL/Inc/stm32n6xx_hal_conf.h         HAL Configuration file
      - TIM/TIM_ExternalClockMode1/FSBL/Inc/stm32n6xx_nucleo_conf.h      BSP configuration file
      - TIM/TIM_ExternalClockMode1/FSBL/Inc/stm32n6xx_it.h               Interrupt handlers header file
      - TIM/TIM_ExternalClockMode1/FSBL/Src/main.c                       Main program
      - TIM/TIM_ExternalClockMode1/FSBL/Src/stm32n6xx_hal_msp.c          HAL MSP module
      - TIM/TIM_ExternalClockMode1/FSBL/Src/stm32n6xx_it.c               Interrupt handlers
      - TIM/TIM_ExternalClockMode1/FSBL/Src/system_stm32n6xx_fsbl.c      STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0HxQ devices.

  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-N657X0-Q Set-up
    Connect the following pins together:
    - TIM1_CH2    : PE.11 (PIN23 CN15)
    - TIM_IN_GPIO : PE.13 (PIN28 CN15)

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
