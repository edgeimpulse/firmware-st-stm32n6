## <b>DMA_RAMToRAM Example Description</b>

This example describes how to use DMA to transfer a word data buffer from SRAM memory to embedded
SRAM through the HAL API. Isolation of the HPDMA channel is aligned with targeted memories.

This project is targeted to run on STM32N6xx device on NUCLEO-N657X0 board from STMicroelectronics.

This project runs from the internal RAM. It is launched by the bootROM after being copied from internal RAM to internal RAM.
Configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).

The fastest (throughput wise) DMA channel HPDMA configuration is proposed thanks to:
HPDMA1_Channel12 (biggest DMA internal fifo), 64-word data
(DoubleWord) source and destination wordlength and AXI.

Mind that isolation feature is enabled. The SRAM source and destination is assigned the same CID
as the HPDMA channel (CID 1, i.e. the CID carried by the CPU).

The start of transfer is triggered by software. HPDMA1_Channel12 memory-to-memory
transfer is enabled. Source and destination addresses incrementing is also enabled.

The transfer is started by setting the channel enable bit for HPDMA1_Channel12.

At the end of the transfer a Transfer Complete interrupt is generated since it
is enabled and the callback function (customized by user) is called.

STM32 board LED is used to monitor the example status:

 - Green LED (LED3) is ON when the example was executed correctly.
 
 - Error otherwise.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

  DMA, Data Transfer, Memory to memory, RAM

### <b>Directory contents</b>

File | Description
 --- | ---
      - DMA/DMA_RAMToRAM/FSBL/Inc/main.h                   | Header for main.c module
      - DMA/DMA_RAMToRAM/FSBL/Inc/stm32n6xx_hal_conf.h     | HAL configuration file
      - DMA/DMA_RAMToRAM/FSBL/Inc/stm32n6xx_it.h           | Interrupt handlers header file
      - DMA/DMA_RAMToRAM/FSBL/Inc/stm32n6xx_nucleo_conf.h  | BSP configuration file
      - DMA/DMA_RAMToRAM/FSBL/Src/main.c                   | Main program
      - DMA/DMA_RAMToRAM/FSBL/Src/stm32n6xx_hal_msp.c      | HAL MSP module
      - DMA/DMA_RAMToRAM/FSBL/Src/stm32n6xx_it.c           | Interrupt handlers
      - DMA/DMA_RAMToRAM/FSBL/Src/system_stm32n6xx_fsbl.c  | STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N6xx devices.

  - This example has been tested with STMicroelectronics NUCLEO-N657X0
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:
 
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


:::
:::

