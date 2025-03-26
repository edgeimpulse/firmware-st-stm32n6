## <b>HAL_TimeBase_TIM Example Description</b>

How to customize HAL using a general-purpose timer as main source of time base
instead of Systick.

In this example the used timer is TIM2.

Time base duration is kept unchanged: 1ms  since PPP_TIMEOUT_VALUEs are defined
and handled in milliseconds basis.

The example brings, in user file, a new implementation of the following HAL weak functions:

	HAL_InitTick()
	HAL_SuspendTick()
	HAL_ResumeTick()

This implementation will overwrite native implementation in stm32n6xx_hal.c
and so user functions will be invoked instead when called.

The following time base functions are kept as implemented natively:

	HAL_IncTick()
	HAL_Delay()

When user pushes the USER push-button, the Tick increment is suspended if it is already
enabled, else it will be resumed.
In an infinite loop, LED1 toggles spaced out over 1s delay.

### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in TIM2 ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the TIM2 interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the TIM2 interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the TIM2 time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, TIM, Time base, HAL

### <b>Directory contents</b>

  - HAL/HAL_TimeBase_TIM/FSBL/Inc/stm32n6xx_nucleo_conf.h       BSP configuration file
  - HAL/HAL_TimeBase_TIM/FSBL/Inc/stm32n6xx_hal_conf.h          HAL configuration file
  - HAL/HAL_TimeBase_TIM/FSBL/Inc/stm32n6xx_it.h                Interrupt handlers header file
  - HAL/HAL_TimeBase_TIM/FSBL/Inc/stm32n6xx.h                   Header for main.c module  
  - HAL/HAL_TimeBase_TIM/FSBL/Src/stm32n6xx_it.c                Interrupt handlers
  - HAL/HAL_TimeBase_TIM/FSBL/Src/main.c                        Main program
  - HAL/HAL_TimeBase_TIM/FSBL/Src/stm32n6xx_hal_msp.c           HAL MSP file
  - HAL/HAL_TimeBase_TIM/FSBL/Src/stm32n6xx_hal_timebase_tim.c  HAL time base functions
  - HAL/HAL_TimeBase_TIM/FSBL/Src/system_stm32n6xx.c            STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0HxQ devices.

  - This example has been tested with STMicroelectronics NUCLEO-N657X0-Q board and can be
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

