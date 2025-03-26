---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## <b>UART_HyperTerminal_IT Example Description</b>

UART transmission (transmit/receive) in Interrupt mode between a board and
an HyperTerminal PC application.

       Board: STM32N6570-DK
       - Tx Pin: PE5
       - Rx Pin: PE6

This project is targeted to run on STM32N657X0H3QU device on STM32N6570-DK board from STMicroelectronics.

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.
The SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 400 Mhz and the CPU clock at 800 MHz.

The Example project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches. This is provided as template implementation that the User may
integrate in his application in order to enhance the performance.

The application is loaded into AXISRAM2 at address 0x3418'0400 and executed.

The UART peripheral configuration is ensured by the HAL_UART_Init() function.
This later is calling the HAL_UART_MspInit() function which core is implementing
the configuration of the needed UART resources according to the used hardware (CLOCK,
GPIO and NVIC). You may update this function to change UART configuration.

The UART/Hyperterminal communication is then initiated.
The HAL_UART_Receive_IT() and the HAL_UART_Transmit_IT() functions allow respectively
the reception of Data from Hyperterminal and the transmission of a predefined data
buffer.

The Asynchronous communication aspect of the UART is clearly highlighted as the
data buffers transmission/reception to/from Hyperterminal are done simultaneously.

For this example the TxBuffer (aTxStartMessage) is predefined and the RxBuffer (aRxBuffer)
size is limited to 10 data by the mean of the RXBUFFERSIZE define in the main.c file.

In a first step the TxBuffer buffer content will be displayed in the Hyperterminal
interface and the received data will be stored in the RxBuffer buffer.
In a second step the received data in the RxBuffer buffer will be sent back to
Hyperterminal and displayed.

The end of this two steps are monitored through the HAL_UART_GetState() function
result.

STM32N6570-DK board LED1 and LED2 are used to monitor the transfer status:

 - LED1 toggles with a 100 ms period while waiting user to enter 10 characters on HyperTerminal console.
 - LED1 turns ON when transmission/reception is complete and OK.
 - LED2 turns ON when there is an error in transmission/reception process.

The UART is configured as follows:

    - BaudRate = 115200 baud
    - Word Length = 8 Bits (7 data bit + 1 parity bit)
    - One Stop Bit
    - Odd parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Reception and transmission are enabled in the time

#### <b>Notes</b>

 1. When the parity is enabled, the computed parity is inserted at the MSB
    position of the transmitted data.

 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Connectivity, UART/USART, baud rate, RS-232, HyperTerminal, full-duplex, Interrupt
Transmitter, Receiver, Asynchronous

### <b>Directory contents</b>

    - UART/UART_HyperTerminal_IT/Inc/partition_stm32n657xx.h        Partition file
    - UART/UART_HyperTerminal_IT/Inc/stm32n6570_discovery_conf.h    BSP configuration file
    - UART/UART_HyperTerminal_IT/Inc/stm32n6xx_hal_conf.h           HAL configuration file
    - UART/UART_HyperTerminal_IT/Inc/stm32n6xx_it.h                 IT interrupt handlers header file
    - UART/UART_HyperTerminal_IT/Inc/main.h                         Header for main.c module
    - UART/UART_HyperTerminal_IT/Src/stm32n6xx_it.c                 IT interrupt handlers
    - UART/UART_HyperTerminal_IT/Src/main.c                         Main program
    - UART/UART_HyperTerminal_IT/Src/stm32n6xx_hal_msp.c            HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32N657xx devices.

  - This example has been tested with STMicroelectronics STM32N6570-DK (MB1939) board and can be
    easily tailored to any other supported device and development board.

  - STM32N6570-DK Set-up
    - Connect ST-Link cable to the PC USB port to display data on the HyperTerminal (and to receive data).
      A virtual COM port will then appear in the HyperTerminal.

  - Hyperterminal configuration:

    - Data Length = 7 Bits
    - One Stop Bit
    - Odd parity
    - BaudRate = 115200 baud
    - Flow control: None

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

