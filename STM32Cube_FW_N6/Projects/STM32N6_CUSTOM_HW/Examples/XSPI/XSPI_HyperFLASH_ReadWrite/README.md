## <b>XSPI_HyperFLASH_ReadWrite Example Description</b>

How to use a XSPI HyperFLASH memory in indirect mode.

This example describes how to write and read data in indirect mode
in a XSPI HyperFLASH memory (s26ks512) and compare the result.

Note: uncomment XSPI_DEBUG_PRINTF inside main.c to display information on Terminal IO.

- This project is targeted to run on STM32N657X0H3QU device from STMicroelectronics.
- The FSBL project configures the maximum system clock frequency at 600 MHz.
- STEP1: Program erase sectors defined by MAX_ADDRESS_TO_TEST
- STEP2: Program write a lopped buffer into s26ks512 memory until MAX_ADDRESS_TO_TEST
- STEP3: Program compare read data from s26ks512 memory until MAX_ADDRESS_TO_TEST.
- In case of success; Program stay in infinite loop. Otherwise Error_Handler function is called.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Memory, XSPI, OSPI, HyperFLASH, s26ks512

### <b>Directory contents</b>

  - XSPI/XSPI_HyperFLASH_ReadWrite_IT/FSBL/Src/main.c                       Main program
  - XSPI/XSPI_HyperFLASH_ReadWrite_IT/FSBL/Src/xspi_s26ks512.h              XSPI s26ks512 driver file
  - XSPI/XSPI_HyperFLASH_ReadWrite_IT/FSBL/Src/system_stm32n6xx_fsbl.c      STM32N6xx system clock configuration file
  - XSPI/XSPI_HyperFLASH_ReadWrite_IT/FSBL/Src/stm32n6xx_it.c               Interrupt handlers
  - XSPI/XSPI_HyperFLASH_ReadWrite_IT/FSBL/Src/stm32n6xx_hal_msp.c          HAL MSP module
  - XSPI/XSPI_HyperFLASH_ReadWrite_IT/FSBL/Inc/main.h                       Main program header file
  - XSPI/XSPI_HyperFLASH_ReadWrite_IT/FSBL/Inc/xspi_s26ks512.h              XSPI s26ks512 driver header file
  - XSPI/XSPI_HyperFLASH_ReadWrite_IT/FSBL/Inc/stm32n6xx_hal_conf.h         HAL Configuration file
  - XSPI/XSPI_HyperFLASH_ReadWrite_IT/FSBL/Inc/stm32n6xx_it.h               Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This example has been tested with STMicroelectronics STM32N6570
    and HyperFLASH Memory s26ks512. Example can be easily tailored to any
    other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
