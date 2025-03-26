---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}


## <b>Example Description</b>

How to enter the Standby mode and wake up from this mode by using a wake-up pin.

- This project is targeted to run on STM32N657 device on STM32N6570-DK board from STMicroelectronics.
At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.
- The SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 400 Mhz and the CPU clock at 800 MHz.
- The Example project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches. This is provided as template implementation that the User may
integrate in his application in order to enhance the performance.
- The FSBL project runs in AXISRAM2 (0x3418'0000) and is executed upon power-on after bootROM execution.
- The application is loaded into AXISRAM1 (0x3400'0000) and executed after the FSBL.
- The application then sets up the necessary configuration for standby operation and wake-up upon wake-up pin press.
- After power on, the LED blinks at 1Hz to signify post-reset operation for 5 seconds.
- The device enters standby mode.
- The device is woken up upon USER1 button press (connected to wake-up pin PC13).
- The LED blinks at 5Hz for 5 seconds to signify exit from standby mode.
- The device enters again standby mode. Any USER1 button press can wake it up and the same sequence described in the last steps above unrolls.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.
 3. By default, the VDDIO3_HSLV bit in OTP124 is not set. The external memory therefore runs slower than optimal.
     To take advantage of the full performance of the external flash, this bit should be set.


### <b>Keywords</b>

Power, PWR, Standby mode, Wakeup, Low Power


### <b>Sub-project FSBL directory contents</b>

      - PWR/PWR_STANDBY/FSBL/Inc/main.h                       Header for main.c module
      - PWR/PWR_STANDBY/FSBL/Inc/extmem.h                     Header for extmem.c module
      - PWR/PWR_STANDBY/FSBL/Inc/partition_stm32n657xx.h      Partition file
      - PWR/PWR_STANDBY/FSBL/Inc/stm32n6xx_hal_conf.h         HAL Configuration file
      - PWR/PWR_STANDBY/FSBL/Inc/stm32n6xx_it.h               Interrupt handlers header file
      - PWR/PWR_STANDBY/FSBL/Inc/stm32_extmem_conf.h          External memory manager Configuration file
      - PWR/PWR_STANDBY/FSBL/Src/main.c                       Main program
      - PWR/PWR_STANDBY/FSBL/Src/extmem.c                     Code to initialize external memory
      - PWR/PWR_STANDBY/FSBL/Src/stm32n6xx_hal_msp.c          HAL MSP module
      - PWR/PWR_STANDBY/FSBL/Src/stm32n6xx_it.c               Interrupt handlers
      - PWR/PWR_STANDBY/FSBL/Src/system_stm32n6xx_fsbl.c      STM32N6xx system source file

### <b>Sub-project Appli directory contents</b>

      - PWR/PWR_STANDBY/Appli/Inc/main.h                      Header for main.c module
      - PWR/PWR_STANDBY/Appli/Inc/stm32n6xx_hal_conf.h        HAL Configuration file
      - PWR/PWR_STANDBY/Appli/Inc/stm32n6xx_it.h              Interrupt handlers header file
      - PWR/PWR_STANDBY/Appli/Inc/partition_stm32n657xx.h     Partition file
      - PWR/PWR_STANDBY/Appli/Inc/stm32n6570_discovery_conf.h BSP Configuration file 
      - PWR/PWR_STANDBY/Appli/Src/main.c                      Main program
      - PWR/PWR_STANDBY/Appli/Src/stm32n6xx_hal_msp.c         HAL MSP module
      - PWR/PWR_STANDBY/Appli/Src/stm32n6xx_it.c              Interrupt handlers
      - PWR/PWR_STANDBY/Appli/Src/system_stm32n6xx.c          STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This template runs on STM32N6 devices.

  - This template has been tested with STMicroelectronics STM32N6570-DK (MB1939)
    board and can be easily tailored to any other supported device
    and development board.


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn't matter).
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
 - Next, in resorting again to CubeProgrammer, load the Appli binary and its header (Project-trusted.bin) in DK board external Flash at address 0x7010'0000.
 - Load the FSBL binary in internal RAM using the IDE
 - Run the example

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary FSBL.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin FSBL.bin -nk -of 0x80000000 -t fsbl -o FSBL-trusted.bin -hv 2.1 -dump FSBL-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin FSBL.bin -nk -of 0x80000000 -t fsbl -o FSBL-trusted.bin -hv 2.3 -dump FSBL-trusted.bin*
   - The resulting binary is FSBL-trusted.bin. 
 - In resorting again to CubeProgrammer, load the FSBL binary and its header (FSBL-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.

:::
:::


