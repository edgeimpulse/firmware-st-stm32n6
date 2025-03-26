---
pagetitle: Readme
lang: en
---

## <b>SYSCFG FLEXMEM Configuration Example Description</b>

This project is targeted to run on STM32N6xx device on NUCLEO-N657X0 board from STMicroelectronics.

This project runs from the internal RAM. It is launched by the bootROM after being copied from external flash to internal RAM.
Configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).

This example shows the configuration of TCM sizes on STM32N6.

First on power-on reset, the TCM configuration gives the following address spaces :
 - FLEXRAM : from 0x3400'0000 to 0x3406'4000 (400KB)
 - DTCM :    from 0x3000'0000 to 0x3002'0000 (128KB)
 - ITCM :    from 0x1000'0000 to 0x1001'0000 (64KB)
The example sets the TCM configuration register which sets the parameters that will be used after a cold reset.
The examples writes in FLEXRAM to show that it is accessible as expected (from 0x3400'0000 to 0x3403'C000).
Next, extended DTCM is accessed (address 0x3002'00A0) to show that the address space is not available and triggers a Hardfault.
The Hardfault handler resets the device which applies the TCM configuration.

Now the TCM configuration gives the following address spaces :

 - FLEXRAM : from 0x3403'C000 to 0x3406'4000 (160KB)
 - DTCM :    from 0x3000'0000 to 0x3002'0000 (256KB + 8KB ECC)
 - ITCM :    from 0x1000'0000 to 0x1001'0000 (128KB + 16KB ECC)

The examples writes in DTCM to show that it is accessible as expected (from 0x3000'0000 to 0x3004'0000).
Next, reduced FLEXRAM is accessed (address 0x3403'BFF0) to show that the address space is not available and triggers a Hardfault.

The second Hardfault turns on the green LED to show that the examples worked as expected.

The expected behaviour for all FLEXMEM configurations is described in the FLEXRAM control section of the reference manual.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.


### <b>Keywords</b>

SYSCFG, FLEXMEM, FLEXRAM, TCM, ITCM, DTCM, SRAM

### <b>Directory contents</b>

      - SYSCFG/FLEXMEM_Configurations/FSBL/Inc/main.h                   | Header for main.c module
      - SYSCFG/FLEXMEM_Configurations/FSBL/Inc/stm32n6xx_hal_conf.h     | HAL Configuration file
      - SYSCFG/FLEXMEM_Configurations/FSBL/Inc/stm32n6xx_it.h           | Interrupt handlers header file
      - SYSCFG/FLEXMEM_Configurations/FSBL/Inc/stm32n6xx_nucleo_conf.h  | BSP Configuration file
      - SYSCFG/FLEXMEM_Configurations/FSBL/Src/main.c                   | Main program
      - SYSCFG/FLEXMEM_Configurations/FSBL/Src/stm32n6xx_hal_msp.c      | HAL MSP module
      - SYSCFG/FLEXMEM_Configurations/FSBL/Src/stm32n6xx_it.c           | Interrupt handlers
      - SYSCFG/FLEXMEM_Configurations/FSBL/Src/system_stm32n6xx_fsbl.c  | STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N6xx devices.

  - This example has been tested with STMicroelectronics NUCLEO-N657X0
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>
 This example requires a reset of the device. Therefore it can only be run in boot from flash mode due to RAM erase on reset.

 This is done by following the instructions below:

 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files.
 - Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.1 -dump Project-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.


