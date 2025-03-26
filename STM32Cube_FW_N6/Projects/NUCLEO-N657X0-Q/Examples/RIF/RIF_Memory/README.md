## <b>RIF_Memory Example Description</b>

This example describes how to use the RIF HAL API to configure a RISAF in order to protect a memory from illegal accesses.
  0. The FSBL enables the illegal access controller interrupt for the RISAF2 (protecting SRAM1 memory).
  1. With default parameters applied for the RISAF2, the software writes to a un buffer located in SRAM1 (0x34064000U). This memory is secure, privilege, CID1 software only by default, so we don't expect any illegal access.
  2. A *base region* is then configured to let only non-secure masters to access the buffer. An illegal access is expected with attributes related to the CPU (secure, privileged, CID1).
  3. The base region is re-configured to its default parameters then a *subregion* is set to let only non-secure accesses to the buffer. An illegal access with attributes related to the CPU is expected while accessing the buffer, as the buffer is located in an enabled nested subregion which filters applies.
  4. A subregion *overlapping* the previous one is configured to let only secure accesses to the buffer. An illegal access with attributes related to the CPU is expected while accessing the buffer as the filtering rule states that if one subregion is non-secure, the overlap is non-secure.

After startup, the HAL_Init() function is called to reset all the peripherals and initialize the systick used as 1ms HAL timebase. The SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 400 Mhz and the CPU clock at 800 MHz.

The Example project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable the Layer 1 Core Instruction and Data Caches. This is provided as template implementation that the User may integrate in his application in order to enhance the performance.

STM32 board LEDs are used to monitor the example status:

  - Green LED (LED3) is ON when the example was executed correctly.
  - Blue  LED (LED1) is ON when there was an an error in initialization or if the RISAF has not reported the correct illegal accesses attributes.

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

Resource isolation framework, RIF, Resource Isolation slave unit for address space protection, RISAF, Security

### <b>Directory contents</b>

  - RIF/RIF_Memory/FSBL/Inc/stm32n6xx_nucleo_conf.h     BSP configuration file
  - RIF/RIF_Memory/FSBL/Inc/stm32n6xx_hal_conf.h        HAL configuration file
  - RIF/RIF_Memory/FSBL/Inc/stm32n6xx_it.h              Interrupt handlers header file
  - RIF/RIF_Memory/FSBL/Inc/main.h                      Header for main.c module
  - RIF/RIF_Memory/FSBL/Src/stm32n6xx_it.c              Interrupt handlers
  - RIF/RIF_Memory/FSBL/Src/stm32n6xx_hal_msp.c         HAL MSP file
  - RIF/RIF_Memory/FSBL/Src/main.c                      Main program
  - RIF/RIF_Memory/FSBL/Src/system_stm32n6xx_fsbl.c     STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N6 devices.

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

