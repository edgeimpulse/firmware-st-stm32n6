## <b>TIM_TimeBase Example Description</b>

This example shows how to configure the TIM peripheral to generate a time base of 
one second with the corresponding Interrupt request.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32N657X0HxQ Devices :
The CPU at 600 MHz 

    In this example TIM2 input clock (TIM2CLK) is set to TIMPRE clock,
      TIMPRE = SYSB
      => TIM2CLK = SystemBusClock
    To get TIM2 counter clock at 10 kHz, the Prescaler is computed as following:
    Prescaler = (TIM2CLK / TIM2 counter clock) - 1
    Prescaler = (SystemBusClock /10 KHz) - 1

SystemCoreClock is set to 600 MHz for STM32N6xx Devices.

The TIM2 ARR register value is equal to 10000 - 1

Update rate = TIM2 counter clock / (Period + 1) = 1 Hz

So the TIM2 generates an interrupt each 1 s

When the counter value reaches the auto-reload register value, the TIM update 
interrupt is generated and, in the handler routine, pin PG.08 (connected to LED1 on board NUCLEO-N657X0-Q)
is toggled. So led blinks at the following frequency: 0.5Hz.

In case of error, LED2 is turned ON.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. This example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Timer, TIM, time base , Interrupt, Clock source

### <b>Directory contents</b>

  - TIM/TIM_TimeBase/FSBL/Inc/stm32n6xx_nucleo_conf.h     BSP configuration file
  - TIM/TIM_TimeBase/FSBL/Inc/stm32n6xx_hal_conf.h    HAL configuration file
  - TIM/TIM_TimeBase/FSBL/Inc/stm32n6xx_it.h          Interrupt handlers header file
  - TIM/TIM_TimeBase/FSBL/Inc/main.h                  Header for main.c module  
  - TIM/TIM_TimeBase/FSBL/Src/stm32n6xx_it.c          Interrupt handlers
  - TIM/TIM_TimeBase/FSBL/Src/main.c                  Main program
  - TIM/TIM_TimeBase/FSBL/Src/stm32n6xx_hal_msp.c     HAL MSP file
  - TIM/TIM_TimeBase/FSBL/Src/system_stm32n6xx.c      STM32N6xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0HxQ devices.
  - In this example, the clock is set to 600 MHz.
    
  - This example has been tested with STMicroelectronics NUCLEO-N657X0-Q
    board and can be easily tailored to any other supported device
    and development board.      

  - NUCLEO-N657X0-Q Set-up
    - Use LED1 connected to PG.08 pin () and connect it on an oscilloscope 
      to show the time base signal.  


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


