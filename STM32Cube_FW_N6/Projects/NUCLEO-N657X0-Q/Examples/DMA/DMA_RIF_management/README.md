## <b>DMA_RIF_management Example Description</b>

How to use CID information to isolate DMA channels.

This project is targeted to run on STM32N6xx device on NUCLEO-N657X0 board from STMicroelectronics.

This project runs from the internal RAM. It is launched by the bootROM after being copied from internal RAM to internal RAM.
Configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).

This example describes how to use an HPDMA to transfer a data buffer from SRAM memory to another 
SRAM through the HAL API with isolation. The example highlights steps for configuring and running
a DMA transfer in unprivileged code with CID transfer to and from memory with isolation accesses restrictions.

The privileged/unprivileged, secure, isolation (CID), HPDMA channel attributes 
are set thanks to MPU, RISAF, DMA & SYSCFG. 

This example runs without operating system (OS). Should an OS be running, in a multi-tenancy scenario, 
a distinct CID (values ≠ 1 and ≠7) is assigned to each tenant(process). 
The secure OS configures the RISAFs to ensure tenant data are only accessible by
accesses carrying the tenant CID. 
On STM32N6xx devices, a mechanism allows a secure tenant (unprivileged) to use the HPDMA to move
its own data, but no-one else's. Just before scheduling a tenant's user process, 
the secure OS (privileged) must configure the N6 so that all AHB secure user accesses
which arrive at the HPDMA, appear to carry that CID 
(in other words allows the user (unprivileged) to write into the DMA control registers).
The tenant’s secure user process is then only able to use secure HPDMA channels allocated to its own CID.
For more information please refer to Multi-tenancy section of the reference manual.

In this example (without OS) a single tenant(process) is emulated.
The emulated user tenant(process) basically runs when the example runs in privileged.

Isolation of the HPDMA channel is enabled with CID 4 that is CID different that 1 
(CID 1 is Cortex CID, 7 being debug CID). SYSCFG API are used so that accesses arriving at the HPDMA 
are perceived with CID 4 whenever necessary and set back to 1.

2 transfers are run using RAM1 and RAM2 are run one after the other.
The 1st transfer is from source buffer to intermediate buffer
and the 2nd from the intermediate buffer to destination buffer.

3 STEP are proposed and can be chose by seleting exclusive compilation flags STEP1
, STEP2 & STEP3 respectively as per table herafter.
STEP2 and STEP3 are non functional cases but that purpose is to highlighting
how to identify the error.

  CID Source Buffer CIR Intermediate Buffer CID Destination Buffer Perceived CID DMA channel CID Status
  ----------------- ----------------------- ---------------------- ------------- --------------- --------------
  1 & 4             4                       1 & 4                  4             4               OK
  SRAM2             SRAM1   
  1 & 4             4                       1 & 4                  5             4               KO ILAC HPDMA1
  SRAM2             SRAM1
  1 & 4             5                       1 & 4                  4             4               KO
  SRAM2             SRAM1  


STM32 board LED is used to monitor the example status:
  - Green LED (LED3) is ON when the example was executed correctly.
  - Error otherwise.
  - Watch window allows checking that content of source buffer aSRC_Buffer
    is eventually copied into aDST_Buffer.

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
      - DMA/DMA_RIF_management/FSBL/Inc/main.h                   | Header for main.c module
      - DMA/DMA_RIF_management/FSBL/Inc/stm32n6xx_hal_conf.h     | HAL configuration file
      - DMA/DMA_RIF_management/FSBL/Inc/stm32n6xx_it.h           | Interrupt handlers header file
      - DMA/DMA_RIF_management/FSBL/Inc/stm32n6xx_nucleo_conf.h  | BSP configuration file
      - DMA/DMA_RIF_management/FSBL/Src/main.c                   | Main program
      - DMA/DMA_RIF_management/FSBL/Src/stm32n6xx_hal_msp.c      | HAL MSP module
      - DMA/DMA_RIF_management/FSBL/Src/stm32n6xx_it.c           | Interrupt handlers
      - DMA/DMA_RIF_management/FSBL/Src/system_stm32n6xx_fsbl.c  | STM32N6xx system source file

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

