---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}


## <b>Template FSBL LRUN (Load&Run) Description</b>

- This project provides a reference FSBL LRUN template based on the STM32Cube HAL API that can be used to build any firmware application to execute in internal RAM (sub-project Appli). 
After bootROM execution, the FSBL project executes in internal RAM, ensures proper MPU, caches and clock setting then copies the application binary from external Flash to internal RAM. When done, the program counter is set to the application entry point and the application in turn executes.

- This project is targeted to run on STM32N657X0H3QU device on NUCLEO-N657X0-Q board from STMicroelectronics.  
- The FSBL project configures the maximum system clock frequency at 600 MHz.
- Once the application runs, the green LED (GPIO PG.00) toggles in an infinite loop with a 250-ms period.


#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Reference, Template, FSBL, LRUN

### <b>Sub-project FSBL directory contents</b>

      - Templates/Template_FSBL_LRUN/FSBL/Inc/main.h                       Header for main.c module
      - Templates/Template_FSBL_LRUN/FSBL/Inc/extmem.h                     Header for extmem.c module
      - Templates/Template_FSBL_LRUN/FSBL/Inc/partition_stm32n657xx.h      Partition file
      - Templates/Template_FSBL_LRUN/FSBL/Inc/stm32n6xx_hal_conf.h         HAL Configuration file
      - Templates/Template_FSBL_LRUN/FSBL/Inc/stm32n6xx_it.h               Interrupt handlers header file
      - Templates/Template_FSBL_LRUN/FSBL/Inc/stm32_extmem_conf.h          External memory manager Configuration file
      - Templates/Template_FSBL_LRUN/FSBL/Src/main.c                       Main program
      - Templates/Template_FSBL_LRUN/FSBL/Src/extmem.c                     Code to initialize external memory
      - Templates/Template_FSBL_LRUN/FSBL/Src/stm32n6xx_hal_msp.c          HAL MSP module
      - Templates/Template_FSBL_LRUN/FSBL/Src/stm32n6xx_it.c               Interrupt handlers
      - Templates/Template_FSBL_LRUN/FSBL/Src/system_stm32n6xx_fsbl.c      STM32N6xx system source file

### <b>Sub-project Appli directory contents</b>

      - Templates/Template_FSBL_LRUN/Appli/Inc/main.h                      Header for main.c module
      - Templates/Template_FSBL_LRUN/Appli/Inc/stm32n6xx_hal_conf.h        HAL Configuration file
      - Templates/Template_FSBL_LRUN/Appli/Inc/stm32n6xx_it.h              Interrupt handlers header file
      - Templates/Template_FSBL_LRUN/Appli/Inc/partition_stm32n657xx.h     Partition file
      - Templates/Template_FSBL_LRUN/Appli/Inc/stm32n6xx_nucleo_conf.h     BSP Configuration file 
      - Templates/Template_FSBL_LRUN/Appli/Src/main.c                      Main program
      - Templates/Template_FSBL_LRUN/Appli/Src/stm32n6xx_hal_msp.c         HAL MSP module
      - Templates/Template_FSBL_LRUN/Appli/Src/stm32n6xx_it.c              Interrupt handlers
      - Templates/Template_FSBL_LRUN/Appli/Src/system_stm32n6xx.c          STM32N6xx system source file


### <b>Hardware and Software environment</b>

  - This template runs on STM32N6 devices.

  - This template has been tested with STMicroelectronics NUCLEO-N657X0-Q (MB1940)
    board and can be easily tailored to any other supported device
    and development board.

  - The following OTP fuses are set in this template:

    - VDDIO3_HSLV=1     I/O XSPIM_P2 High speed option enabled

**WARNING**

When OTP fuses are set, they can not be reset.

If the OTP fuseVDDIO3_HSLV is not already set, run the template once with boot mode set in development mode and NO_OTP_FUSE compilation switch disabled
(otherwise, it isn't possible to benefit from the maximum transfer rate to access the external flash).

For the following runs, select NO_OTP_FUSE option by default using the preprocessor definition in the IDE.


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Select first the FSBL workspace
 - Rebuild all files from sub-project FSBL (if no modification is done on FSBL project, this step can be done only once)
 - Select the Appli workspace
 - Rebuild all files from sub-project Appli
 - Resort to CubeProgrammer to add a header to the generated Appli binary Project.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.1 -dump Project-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the Appli binary and its header (Project-trusted.bin) in Nucleo board external Flash at address 0x7010'0000.
 - Load the FSBL binary in internal RAM using the IDE
 - Run the example

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary FSBL.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o FSBL-trusted.bin -hv 2.1 -dump FSBL-trusted.bin*
   - for Cut2.0, 
     - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o FSBL-trusted.bin -hv 2.3 -dump FSBL-trusted.bin*
   - The resulting binary is FSBL-trusted.bin.
 - In resorting again to CubeProgrammer, load the FSBL binary and its header (FSBL-trusted.bin) in Nucleo board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.


:::
:::
