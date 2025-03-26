## <b>EXTI_ToggleLedOnIT_Init Example Description</b>

This example describes how to configure the EXTI and use
GPIOs to toggle the user LEDs available on the board when
a user button is pressed. This example is based on the
STM32N6xx LL API. Peripheral initialization is done using LL
initialization function to demonstrate LL init usage.

In this example, one EXTI line (External line 13) is configured to generate
an interrupt on each falling edge.

In the interrupt routine a led connected to a specific GPIO pin is toggled.

In this example:

    - External line 13 is connected to PC.13 pin
      - when falling edge is detected on External line 13 by pressing USER push-button, LED1 toggles

On NUCLEO-N657X0-Q:

    - External line 13 is connected to USER push-button


### <b>Keywords</b>

System, GPIO, Output, Alternate function, EXTI, Toggle

### <b>Directory contents</b>

  - EXTI/EXTI_ToggleLedOnIT_Init/FSBL/Inc/stm32n6xx_it.h          Interrupt handlers header file
  - EXTI/EXTI_ToggleLedOnIT_Init/FSBL/Inc/main.h                  Header for main.c module
  - EXTI/EXTI_ToggleLedOnIT_Init/FSBL/Inc/stm32_assert.h          Template file to include assert_failed function
  - EXTI/EXTI_ToggleLedOnIT_Init/FSBL/Src/stm32n6xx_it.c          Interrupt handlers
  - EXTI/EXTI_ToggleLedOnIT_Init/FSBL/Src/main.c                  Main program
  - EXTI/EXTI_ToggleLedOnIT_Init/FSBL/Src/system_stm32n6xx.c      STM32N6xx system source file

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


