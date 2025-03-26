## <b>TIM_PWMOutput Example Description</b>

This example shows how to configure the TIM peripheral in PWM (Pulse Width Modulation)
mode.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

This project runs from the internal RAM. It is launched by the bootROM after being copied from external flash to internal RAM
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).

    In this example TIM2 input clock (TIM2CLK) is set to TIMPRE clock,
    since TIMPRE prescaler is equal to 1.
      TIMPRE = SYSB
      => TIM2CLK = HCLK = SystemCoreClock

    To get TIM2 counter clock at 1 MHz, the prescaler is computed as follows:
       Prescaler = (TIM2CLK / TIM2 counter clock) - 1
       Prescaler = ((SystemCoreClock/2) /1 MHz) - 1

    To get TIM2 output clock at 25 KHz, the period (ARR)) is computed as follows:
       ARR = (TIM2 counter clock / TIM2 output clock) - 1
           = 39

    TIM2 Channel1 duty cycle = (TIM2_CCR1/ TIM1_ARR + 1)* 100 = 50%
    TIM2 Channel2 duty cycle = (TIM2_CCR2/ TIM1_ARR + 1)* 100 = 37.5%
    TIM2 Channel3 duty cycle = (TIM2_CCR3/ TIM1_ARR + 1)* 100 = 25%
    TIM2 Channel4 duty cycle = (TIM2_CCR4/ TIM1_ARR + 1)* 100 = 12.5%

LED2 is ON when there are an error.

The PWM waveforms can be displayed using an oscilloscope.

#### <b>Notes</b>

 1. The duty cycles values mentioned above are theoretical (obtained when the system clock frequency is exactly 600 MHz).
    They might be slightly different depending on system clock frequency precision.

 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. This example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Timer, TIM, Output, signal, PWM, Oscilloscope, Frequency, Duty cycle, Waveform

### <b>Directory contents</b>

  - TIM/TIM_PWMOutput/Inc/stm32n6xx_nucleo_conf.h     BSP configuration file
  - TIM/TIM_PWMOutput/FSBL/Inc/stm32n6xx_hal_conf.h    HAL configuration file
  - TIM/TIM_PWMOutput/FSBL/Inc/stm32n6xx_it.h          Interrupt handlers header file
  - TIM/TIM_PWMOutput/FSBL/Inc/main.h                  Header for main.c module  
  - TIM/TIM_PWMOutput/FSBL/Src/stm32n6xx_it.c          Interrupt handlers
  - TIM/TIM_PWMOutput/FSBL/Src/main.c                  Main program
  - TIM/TIM_PWMOutput/FSBL/Src/stm32n6xx_hal_msp.c     HAL MSP file
  - TIM/TIM_PWMOutput/FSBL/Src/system_stm32n6xx.c      STM32N6xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0HxQ devices.
  - In this example, the clock is set to 600 MHz.

  - This example has been tested with STMicroelectronics NUCLEO-N657X0-Q 
    board and can be easily tailored to any other supported device 
    and development board.

  - NUCLEO-N657X0-Q Set-up
   Connect the following pins to an oscilloscope to monitor the different waveforms:

       - TIM2_CH1 : PA.00 (pin 34 in CN15 connector)
       - TIM2_CH2 : PA.01 (pin 38 in CN15 connector)
       - TIM2_CH3 : PA.02 (pin 34 in CN3 connector)
       - TIM2_CH4 : PF.06 (pin 32 in CN3 connector)


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
 