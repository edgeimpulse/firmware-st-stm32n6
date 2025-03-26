---
pagetitle: Readme
lang: en
---


## <b>SYSCFG Write Posting Errors Example Description</b>

This example shows how to detect write posting errors.
In the example, first a wrong access to an unallowed address is attempted. It triggers a write posting detection and interrupt.
The example shows how to retrieve the reported address and reactivate the detection mechanism.
A second error is finally triggered.

This project is targeted to run on STM32N6xx device on NUCLEO-N657X0 board from STMicroelectronics.

This project runs from the internal RAM. It is launched by the bootROM after being copied from external flash to internal RAM.
Configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).

The example uses the following API:

  - HAL_NVIC_EnableIRQ():  Enables of the write posting interrupt.
  - PAHB_ERR_IRQHandler(): For error detection and reactiving the detection mechanism calling:
    - HAL_SYSCFG_ReEnableWritePostingErrorCapture().
  - HAL_SYSCFG_GetAddressWritePostingBuffer(): Gets the address of first error in P-AHB write-posting buffer.

STM32 board LED is used to monitor the example status:

  - Green LED (LED3) is ON when the example was executed correctly.
  - Error otherwise.

The expected behaviour is described in Error capture in write posting buffer section of the reference manual.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.


### <b>Keywords</b>

  SYSCFG, Write posting error

### <b>Directory contents</b>

      - SYSCFG/SYSCFG_WritePostingErrors/FSBL/Inc/main.h                   | Header for main.c module
      - SYSCFG/SYSCFG_WritePostingErrors/FSBL/Inc/stm32n6xx_hal_conf.h     | HAL Configuration file
      - SYSCFG/SYSCFG_WritePostingErrors/FSBL/Inc/stm32n6xx_it.h           | Interrupt handlers header file
      - SYSCFG/SYSCFG_WritePostingErrors/FSBL/Inc/stm32n6xx_nucleo_conf.h  | BSP Configuration file
      - SYSCFG/SYSCFG_WritePostingErrors/FSBL/Src/main.c                   | Main program
      - SYSCFG/SYSCFG_WritePostingErrors/FSBL/Src/stm32n6xx_hal_msp.c      | HAL MSP module
      - SYSCFG/SYSCFG_WritePostingErrors/FSBL/Src/stm32n6xx_it.c           | Interrupt handlers
      - SYSCFG/SYSCFG_WritePostingErrors/FSBL/Src/system_stm32n6xx_fsbl.c  | STM32N6xx system source file

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


