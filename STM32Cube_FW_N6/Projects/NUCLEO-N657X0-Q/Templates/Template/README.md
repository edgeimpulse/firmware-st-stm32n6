

---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## <b>Template Description</b>

This project provides a reference template based on the STM32Cube HAL API that can be used
to build any firmware application. Note that security is enabled by default on Cortex-M55.

- This project is targeted to run on STM32N657X0H3QU device on NUCLEO-N657X0-Q board from STMicroelectronics.  
- The reference template project configures the maximum system clock frequency at 600 MHz.
- Once the clock is set, the green, blue and red LEDs toggle in an infinite loop with a 600-ms period.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.
 

### <b>Keywords</b>

Reference, Template 

### <b>Directory contents</b>

      - Templates/Template/Src/main.c                         Main program
      - Templates/Template/Src/stm32n6xx_it.c                 Interrupt handlers
      - Templates/Template/Inc/main.h                         Main program header file
      - Templates/Template/Inc/stm32n6xx_nucleo_conf.h        BSP Configuration file
      - Templates/Template/Inc/stm32n6xx_hal_conf.h           HAL Configuration file
      - Templates/Template/Inc/stm32n6xx_it.h                 Interrupt handlers header file
      - Templates/Template/Inc/partition_stm32n657xx.h        Partition file


### <b>Hardware and Software environment</b>

  - This template runs on STM32N657X0H3QU devices.
  - This template has been tested with STMicroelectronics NUCLEO-N657X0-Q (MB1940)
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

Next, this program can be run in boot from flash mode. This can be done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.1 -dump Project-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in Nucleo board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.


:::
:::

