## <b>TIM_Encoder Example Description</b>

This example shows how to configure the TIM1 peripheral in encoder mode to
determinate the rotation direction.

     -------------                               -------------
    |             |CH1:PE5          CH1:PA8     |             |
    |             |---------------------------->|             |
    |    TIM4     |                             |    TIM1     |
    |             |CH2:PE6          CH2:PA9     |             |
    | Quadrature  |---------------------------->| Quadrature  |
    | encoder     |                             | encoder     |
    | emulator    |                             | interface   |
    |             |                             |             |
    |             |                             |             |
    |             |                             |-------------|     -----------
    |             |                             |  Direction  |--> |uwDirection|
     -------------                               -------------      -----------
               Encoder interface example description




To emulate a quadrature encoder, TIM3 is configured in toggle mode to generate
2 quadrature signals on ( and ) at 10KHz. Each 1s, signals change phase
(+90°/-90°) to emulate a Forward/Backward rotation.

 TIM1 is configured in encoder mode interface, counting on TI1 and TI2.

 The counting direction corresponds to the rotation direction of the connected
sensor (emulated by TIM4 signals).

Rotation direction can be monitored by putting "uwDirection" variable in the
Live Watch window.

When uwDirection = 0, LED1 (blue) and according to the "Counting direction versus encoder
signals" table, rotation direction is Forward.

When uwDirection = 1, LED3 (green)  and according to the "Counting direction versus encoder
signals" table, rotation direction is Backward.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

### <b>Keywords</b>

Timer, Encoder mode, Master, Slave, Duty Cycle, rotation direction, Waveform, Oscilloscope, Output, Signal

### <b>Directory contents</b>

  - TIM/TIM_Encoder/Inc/stm32n6xx_nucleo_conf.h     BSP configuration file
  - TIM/TIM_Encoder/Inc/stm32n6xx_hal_conf.h        HAL configuration file
  - TIM/TIM_Encoder/Inc/stm32n6xx_it.h              Interrupt handlers header file
  - TIM/TIM_Encoder/Inc/main.h                      Header for main.c module
  - TIM/TIM_Encoder/Src/stm32n6xx_it.c              Interrupt handlers
  - TIM/TIM_Encoder/Src/main.c                      Main program
  - TIM/TIM_Encoder/Src/stm32n6xx_hal_msp.c         HAL MSP file
  - TIM/TIM_Encoder/Src/system_stm32n6xx.c          STM32N6xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0HxQ devices.

  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-N657X0-Q Set-up
    - Connect  PE5 (TIM4_Channel 1) to PA8 (TIM1_Channel 1).
    - Connect  PE6 (TIM4_Channel 2) to PA9 (TIM1_Channel 2).
    - Put uwDirection in the debugger live watch to detect the rotation direction
      change.


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
