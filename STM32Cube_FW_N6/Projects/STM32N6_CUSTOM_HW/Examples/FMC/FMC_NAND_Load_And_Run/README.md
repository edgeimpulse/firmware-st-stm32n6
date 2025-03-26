---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}


## <b>FMC_NAND_Load_And_Run Description</b>

This example describes how to configure the FMC controller to access the NAND memory.

The NAND device used is a Toshiba device TC58BVG2S0HBAI4.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) on AXI to run at 400 MHz, and FMC Kernel clock at 200 Mhz.

The goal of this example is to demonstrate the different steps to configure the FMC
and make the NAND device ready for access.

In this example, the NAND device is configured and initialized explicitly
following all initialization sequence steps based on NAND timing capabilities.

After initializing the device, user can perform read/write operations on it based on TC58BVG2S0HBAI4 storage area
specification:
- Pages Size is 4096 bytes + 128 bytes in spare area
- Block Size is 64 pages
- Block number is 2048 elements

This mean that NAND size is (4096 + 128) * 64 * 2048

The example contains 2 main use cases:
First one is related to a NAND loader application
Second one is related to a load, then an execution of the data retrieved from the NAND device.

The NAND loader application goal, is to store an executable code (aAppliBuffer) inside the NAND Device
through HAL_NAND_Write_Page_8b(),

The load of NAND data is done through HAL_NAND_Read_Page_8b() and checked to verify its correctness as example.

At this stage, the content of NAND device is stored inside the aExecBuffer.
Then the application jumps to the content of aExecBuffer then executes its content as an executable application.

The executable application contains a GPIO toggling through the pinout PG7.

User can easily monitor the toggle of GPIO PG7, to verify the execution of the executable application.

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

      - Examples/FMC/FMC_NAND_Load_And_Run/FSBL/Src/main.c                         Main program
      - Examples/FMC/FMC_NAND_Load_And_Run/FSBL/Src/stm32n6xx_it.c                 Interrupt handlers
      - Examples/FMC/FMC_NAND_Load_And_Run/FSBL/Src/stm32n6xx_hal_msp.c            HAL MSP module
      - Examples/FMC/FMC_NAND_Load_And_Run/FSBL/Src/system_stm32n6xx_fsbl.c        STM32N6xx system source file
      - Examples/FMC/FMC_NAND_Load_And_Run/FSBL/Inc/main.h                         Main program header file
      - Examples/FMC/FMC_NAND_Load_And_Run/FSBL/Inc/stm32n6xx_hal_conf.h           HAL Configuration file
      - Examples/FMC/FMC_NAND_Load_And_Run/FSBL/Inc/stm32n6xx_it.h                 Interrupt handlers header file
      - Examples/FMC/FMC_NAND_Load_And_Run/FSBL/Inc/partition_stm32n657xx.h        Partition file
      - Examples/FMC/FMC_NAND_Load_And_Run/FSBL/Inc/application_binary.h           Executable Application buffer file


### <b>Hardware and Software environment</b>

  - This example runs on STM32N6xx devices.

  - This template has been tested with STMicroelectronics STM32N6570 Custom Board with a NAND Toshiba TC58BVG2S0HBAI4.

  - The IOs to be connected between the memory and the MCU are detailed in
     FMC/FMC_NAND_Load_And_Run/FSBL/Src/stm32n6xx_hal_msp.c file.


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

