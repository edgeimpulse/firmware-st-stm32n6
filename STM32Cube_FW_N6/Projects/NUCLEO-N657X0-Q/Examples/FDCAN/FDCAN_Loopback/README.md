## <b>FDCAN_Loopback Example Description</b>

This example describes how to configure the FDCAN peripheral to  operate in loopback mode.

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.

This project runs from the internal RAM. It is launched by the bootROM after being copied from external flash to internal RAM
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).

The example is divided into several steps:<br>

  - In the first step, the *HAL_FDCAN_Init()* function is called to configure the module in external loopback mode,  with a **Nominal Bit Rate of 1 MBit/s** and a **sampling point at 75%**.<br>
    Additionally, it has a **Data Bit Rate of 2 MBit/s** and a **sampling point at 80%**.<br>
    Reception filters are then configured with *HAL_FDCAN_ConfigFilter()*, to receive:<br>
      - messages with pre-defined standard ID to Rx FIFO 0<br>
      - messages with pre-defined extended ID to Rx FIFO 1<br>

    Then FDCAN module is started with *HAL_FDCAN_Start()*.<br>
  - In the second step, the following messages are sent:<br>
      - one standard ID message matching Rx FIFO 0 filter<br>
      - two extended ID messages matching Rx FIFO 1 filter<br>
  - In the third step, the application checks that the three messages were received and as expected.


The FDCAN peripheral configuration is ensured by the *HAL_FDCAN_Init()* function.
This later is calling the *HAL_FDCAN_MspInit()* function which core is implementing
the configuration of the needed FDCAN resources according to the used hardware (CLOCK, GPIO, NVIC and DMA).
User may update this function to change FDCAN configuration.


NUCLEO-N657X0-Q's LEDs can be used to monitor the transfer status:

  - LED1 is ON when all three messages were successfully transmitted and received.
  - LED2 toggle slowly when there is an error in transmission/reception process.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Connectivity, CAN/FDCAN, Loopback, Polling, CAN FIFO, CAN Filter

### <b>Directory contents</b>

File | Description
 --- | ---
FDCAN/FDCAN_Loopback/FSBL/Inc/stm32n6xx_nucleo_conf.h    | BSP configuration file
FDCAN/FDCAN_Loopback/FSBL/Inc/stm32n6xx_hal_conf.h   | HAL configuration file
FDCAN/FDCAN_Loopback/FSBL/Inc/stm32n6xx_it.h         | Header for stm32n6xx_it.c
FDCAN/FDCAN_Loopback/FSBL/Inc/main.h                 | Header for main.c module
FDCAN/FDCAN_Loopback/FSBL/Src/stm32n6xx_it.c         | Interrupt handlers
FDCAN/FDCAN_Loopback/FSBL/Src/main.c                 | Main program
FDCAN/FDCAN_Loopback/FSBL/Src/stm32n6xx_hal_msp.c    | HAL MSP module
FDCAN/FDCAN_Loopback/FSBL/Src/system_stm32n6xx.c     | stm32n6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0HxQ devices.

  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-N657X0-Q Set-up:
    No FDCAN connector available on this board. FDCAN RX/TX signals are accessible through Arduino connector.
      - FDCAN RX : PD0
      - FDCAN TX : PA12

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
