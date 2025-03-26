
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## <b>Template_LL Description</b>

This project provides a reference template based on the STM32Cube LL API that can be used
to build any firmware application. Note that security is enabled by default on Cortex-M55.

- This project is targeted to run on STM32N657X0H3QU device on STM32N657-DK board from STMicroelectronics.  
- The reference template project configures the system clock to 400 MHz and the CPU clock to 800 MHz.
- Once the clock is set, the red LED and the green LED (GPIO PG.10 and PO.01) toggle in an infinite loop with a 400-ms period.
 

### <b>Keywords</b>

Reference, Template 

### <b>Directory contents</b>

      - Templates_LL/Template/Src/main.c                         Main program
      - Templates_LL/Template/Src/stm32n6xx_it.c                 Interrupt handlers
      - Templates_LL/Template/Inc/main.h                         Main program header file
      - Templates_LL/Template/Inc/stm32_assert.h                 Assert file
      - Templates_LL/Template/Inc/stm32n6xx_it.h                 Interrupt handlers header file
      - Templates_LL/Template/Inc/partition_stm32n657xx.h        Partition file


### <b>Hardware and Software environment</b>

  - This template runs on STM32N657X0H3QU devices.
  - This template has been tested with STMicroelectronics STM32N657-DK (MB1939)
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

 Next, this program can be run in boot from flash mode. This can be done by following the instructions below:
 
 - Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.1 -dump Project-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.

:::
:::

