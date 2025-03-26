## <b>TIM_TimeBase_Init Example Description</b>

Configuration of the TIM peripheral to generate a timebase. This 
example is based on the STM32N6xx TIM LL API. The peripheral initialization 
uses LL unitary service functions for optimization purposes (performance and size). 

In this example TIM1 input clock TIM1CLK is set to TIMPRE clock.
      As TIMPRE pre-scaler is equal to 1, and since AHB pre-scaler is equal to 2,
      => TIM1CLK = SystemBusClock (400 MHz)

To set the TIM1 counter clock frequency to 10 KHz, the pre-scaler (PSC) is calculated as follows:

	PSC = (TIM1CLK / TIM1 counter clock) - 1
	PSC = ((SystemCoreClock/1) /10 KHz) - 1

SystemCoreClock is set to 600 MHz for STM32N6xx Devices.

The auto-reload (ARR) is calculated to get a timebase period of 100ms,
meaning that initial timebase frequency is 10 Hz.

	ARR = (TIM1 counter clock / timebase frequency) - 1
	ARR = (TIM1 counter clock / 10) - 1

Update interrupts are enabled. Within the update interrupt service routine, pin PG8
(connected to LED1 on board NUCLEO-N657X0-Q) is toggled. So the period of
blinking of LED1 = 2 * timebase period.

USER push-button can be used to modify the timebase period from 100 ms
to 1 s in 100 ms steps. To do so, every time USER push-button is pressed, the
autoreload register (ARR) is updated. In up-counting update event is generated 
at each counter overflow (when the counter reaches the auto-reload value). 

Finally the timebase frequency is calculated as follows:
timebase frequency = TIM1 counter clock /((PSC + 1)*(ARR + 1)*(RCR + 1))
### <b>Keywords</b>

Timer, TIM, timebase, Interrupt, Clock source

### <b>Directory contents</b>

  - TIM/TIM_TimeBase_Init/FSBL/Inc/stm32n6xx_it.h          Interrupt handlers header file
  - TIM/TIM_TimeBase_Init/FSBL/Inc/main.h                  Header for main.c module
  - TIM/TIM_TimeBase_Init/FSBL/Inc/stm32_assert.h          Template file to include assert_failed function
  - TIM/TIM_TimeBase_Init/FSBL/Src/stm32n6xx_it.c          Interrupt handlers
  - TIM/TIM_TimeBase_Init/FSBL/Src/main.c                  Main program
  - TIM/TIM_TimeBase_Init/FSBL/Src/system_stm32n6xx.c      STM32N6xx system source file


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


