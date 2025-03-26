## <b>CORTEX_InterruptSwitch_TrustZone Example Description</b>

This project describes how to switch from secure to non-secure domain.

This project is targeted to run on STM32N6xx device on NUCLEO-N657X0 board from STMicroelectronics.

This project runs from the internal RAM. It is launched by the bootROM after being copied from external flash to internal RAM
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).

This project is composed of two sub-projects:

- one for the First Stage BootLoader (FSBL)
- one for the non-secure application part (Project_ns).


- FLEXRAM and SRAM1 are defined as fully secure
-       FLEXRAM (0x3400'0000 to 0x3406'3FFF) is defined as fully secure RAM area
-       SRAM1 (0x3406'4000 to 0x340F'FFFF) is defined as fully secure RAM area

- SRAM2 is split between secure and non-secure.
-       (0x3418'0000 to 0x2419'FFFF) is defined as Secure ROM area
-       (0x241A'0000 to 0x241B'FFFF) is defined as non-secure ROM area
-       (0x241C'0000 to 0x241F'FFFF) is defined as non-secure RAM area

\

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

In this example, the interrupt associated to EXTI line 13 (connected to
the USER push-button) is used by the secure application that is waiting from a user push-button
key press event before assigning then the interrupt to the non-secure application with
NVIC_SetTargetState() API and launching the non-secure application.

At the first USER push-button key press, the LED1 is switched on by the secure application.
Any further USER push-button key press makes the LED2 managed by the non-secure application toggling.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.


### <b>Keywords</b>

CORTEX, Interrupt, TrustZone, SRAM

### <b>Directory contents</b>

File | Description
 --- | ---
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/FSBL/Src/main.c                           | Secure Main program
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/FSBL/Src/secure_nsc.c                     | Secure Non-Secure Callable (NSC) module
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/FSBL/Src/stm32n6xx_hal_msp.c              | Secure HAL MSP module
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/FSBL/Src/stm32n6xx_it.c                   | Secure Interrupt handlers
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/FSBL/Src/system_stm32n6xx_s.c             | Secure STM32N6xx system clock configuration file
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/FSBL/Inc/main.h                           | Secure Main program header file
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/FSBL/Inc/partition_stm32n6xx.h            | STM32N6 Device System Configuration file
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/FSBL/Inc/stm32n6xx_hal_conf.h             | Secure HAL Configuration file
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/FSBL/Inc/stm32n6xx_it.h                   | Secure Interrupt handlers header file
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/Secure_nsclib/secure_nsc.h                | Secure Non-Secure Callable (NSC) module header file
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/AppliNonSecure/Src/main.c                 | Non-secure Main program
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/AppliNonSecure/Src/stm32n6xx_hal_msp.c    | Non-secure HAL MSP module
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/AppliNonSecure/Src/stm32n6xx_it.c         | Non-secure Interrupt handlers
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/AppliNonSecure/Src/system_stm32n6xx_ns.c  | Non-secure STM32N6xx system clock configuration file
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/AppliNonSecure/Inc/main.h                 | Non-secure Main program header file
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/AppliNonSecure/Inc/stm32n6xx_hal_conf.h   | Non-secure HAL Configuration file
  - CORTEX/CORTEX_InterruptSwitch_TrustZone/AppliNonSecure/Inc/stm32n6xx_it.h         | Non-secure Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N6xx devices.

  - This example has been tested with STMicroelectronics NUCLEO-N657X0
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

To run this example in development mode :
 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files (both FSBL and non-secure application) and load the non secure application into target memory. Code can be executed in this mode for debugging purposes.
 
To run this example in boot from flash mode, do the following :

 - Compile the FSBL and the Non Secure application.
 - Resort to CubeProgrammer to add a header to the generated binary Project.bin *in the non secure folder* with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.1 -dump Project-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.