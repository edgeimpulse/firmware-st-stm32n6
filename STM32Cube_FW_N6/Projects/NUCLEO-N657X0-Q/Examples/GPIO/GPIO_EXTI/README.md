## <b>GPIO_EXTI Example Description</b>

How to configure external interrupt lines.

This project is targeted to run on STM32N6xx device on NUCLEO-N657X0 board from STMicroelectronics.

This project runs from the internal RAM. It is launched by the bootROM after being copied from external flash to internal RAM
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).

In this example, one EXTI line (External line 13) is configured to generate
an interrupt on each falling edge.
In the interrupt routine a led connected to a specific GPIO pin is toggled.

In this example:

    - External line 13 is connected to PC.13 pin
    - when falling edge is detected on External line 13 by pressing USER push-button, LED1 toggles once

On NUCLEO-N657X0-Q:

    - External line 13 is connected to USER push-button

In this example, HCLK is configured at 600 MHz.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, GPIO, EXTI, Output, Alternate function, Push-pull, Toggle

### <b>Directory contents</b>

  - GPIO/GPIO_EXTI/FSBL/Inc/stm32n6xx_nucleo_conf.h     BSP configuration file
  - GPIO/GPIO_EXTI/FSBL/Inc/stm32n6xx_hal_conf.h        HAL configuration file
  - GPIO/GPIO_EXTI/FSBL/Inc/stm32n6xx_it.h              Interrupt handlers header file
  - GPIO/GPIO_EXTI/FSBL/Inc/main.h                      Header for main.c module  
  - GPIO/GPIO_EXTI/FSBL/Src/stm32n6xx_it.c              Interrupt handlers
  - GPIO/GPIO_EXTI/FSBL/Src/stm32n6xx_hal_msp.c         HAL MSP file
  - GPIO/GPIO_EXTI/FSBL/Src/main.c                      Main program
  - GPIO/GPIO_EXTI/FSBL/Src/system_stm32n6xx.c          STM32N6xx system source file

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



