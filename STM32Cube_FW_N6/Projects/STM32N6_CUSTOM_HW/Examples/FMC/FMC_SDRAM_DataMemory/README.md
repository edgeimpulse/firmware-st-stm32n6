---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}


## <b>FMC_SDRAM_DataMemory Description</b>

This example describes how to configure the FMC controller to access the SDRAM memory.

The SDRAM device used is Micron device MT48LC16M16A2.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) on AXI to run at 400 MHz, and FMC Kernel clock at 200 Mhz.

The goal of this example is to demonstrate the different steps to configure the FMC
and make the SDRAM device ready for access R/W.

The example scenario does not reflect a real application case, its purpose is to
provide only the procedure to follow to use the external SDRAM as data memory.

All FMC configuration and its associated GPIOs are done earlier in system_stm32n6xx_fsbl.c
through function SystemInit_ExtMemCtl(), to configure SDRAM to be ready to contain data.

The user has to configure his preferred toolchain using the provided linker file.
The RAM zone is modified in order to use the external SDRAM memory as a RAM.

At this stage, all the used data can be located in the external SDRAM memory.

The user can use the debugger's watch to evaluate "uwTabAddr" variable value which should be above 0xC0000000.

If uwTabAddr value is in the external SDRAM memory, GPIO PG7 is ON, otherwise the GPIO PG6 is ON.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.


### <b>Keywords</b>

FMC, NAND, Load and Run, Executable, TC58BVG2S0HBAI4

### <b>Directory contents</b>

      - Examples/FMC/FMC_SDRAM_DataMemory/FSBL/Src/main.c                         Main program
      - Examples/FMC/FMC_SDRAM_DataMemory/FSBL/Src/stm32n6xx_it.c                 Interrupt handlers
      - Examples/FMC/FMC_SDRAM_DataMemory/FSBL/Src/stm32n6xx_hal_msp.c            HAL MSP module
      - Examples/FMC/FMC_SDRAM_DataMemory/FSBL/Src/system_stm32n6xx_fsbl.c        STM32N6xx system source file
      - Examples/FMC/FMC_SDRAM_DataMemory/FSBL/Inc/main.h                         Main program header file
      - Examples/FMC/FMC_SDRAM_DataMemory/FSBL/Inc/stm32n6xx_hal_conf.h           HAL Configuration file
      - Examples/FMC/FMC_SDRAM_DataMemory/FSBL/Inc/stm32n6xx_it.h                 Interrupt handlers header file
      - Examples/FMC/FMC_SDRAM_DataMemory/FSBL/Inc/partition_stm32n657xx.h        Partition file
      - Examples/FMC/FMC_SDRAM_DataMemory/EWARM/stm32n657xx_sdram_fsbl.icf        STM32N6xx scatter file with SDRAM section


### <b>Hardware and Software environment</b>

  - This example runs on STM32N6xx devices.

  - This template has been tested with STMicroelectronics STM32N6570 Custom Board with a SDRAM Micron MT48LC16M16A2.

  - The IOs to be connected between the memory and the MCU are detailed in
     FMC/FMC_SDRAM_DataMemory/FSBL/Src/system_stm32n6xx_fsbl.c file section "GPIO Configuration".


### <b>How to use it ?</b>

This example is meant to run only in development mode.

In order to make the program work, you must do the following :

 - Set the boot mode in development mode.
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

:::
:::


:::
:::

