## <b>UART_HyperTerminal_DMA Example Description</b>

UART transmission (transmit/receive) in DMA mode
between a board and an HyperTerminal PC application.

    Board: NUCLEO-N657X0-Q (embeds a STM32N657X0HxQ device)
    Tx Pin: PE5(available through VCP)
    Rx Pin: PE6(available through VCP)

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.
- The SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 400 Mhz and the CPU clock at 800 MHz.

The UART peripheral (LPUART1) configuration is ensured by the HAL_UART_Init() function.
This later is calling the HAL_UART_MspInit() function which core is implementing
the configuration of the needed UART resources according to the used hardware (CLOCK,
GPIO, DMA and NVIC). You may update this function to change UART configuration.

The UART/Hyperterminal communication is then initiated.
The HAL_UART_Receive_DMA() and the HAL_UART_Transmit_DMA() functions allow respectively
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

NUCLEO-N657X0-Q board LEDs are used to monitor the transfer status:

 - LD2 turns ON if transmission/reception is complete and OK.
 - LD3 turns ON when when there is an error in transmission/reception process.

The UART is configured as follows:

    - BaudRate = 9600 baud
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

Connectivity, UART/USART, Printf, baud rate, RS-232, HyperTerminal, full-duplex, DMA,
Transmitter, Receiver, Asynchronous

### <b>Directory contents</b>

      - UART/UART_HyperTerminal_DMA/FSBL/Inc/main.h                       Header for main.c module
      - UART/UART_HyperTerminal_DMA/FSBL/Inc/stm32n6xx_hal_conf.h         HAL Configuration file
      - UART/UART_HyperTerminal_DMA/FSBL/Inc/stm32n6xx_nucleo_conf.h      BSP Configuration file
      - UART/UART_HyperTerminal_DMA/FSBL/Inc/stm32n6xx_it.h               Interrupt handlers header file
      - UART/UART_HyperTerminal_DMA/FSBL/Src/main.c                       Main program
      - UART/UART_HyperTerminal_DMA/FSBL/Src/stm32n6xx_hal_msp.c          HAL MSP module
      - UART/UART_HyperTerminal_DMA/FSBL/Src/stm32n6xx_it.c               Interrupt handlers
      - UART/UART_HyperTerminal_DMA/FSBL/Src/system_stm32n6xx_fsbl.c      STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N6xx devices.

  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-N657X0-Q Set-up
     - Connect ST-Link cable to the PC USB port to display data on the HyperTerminal (and to receive data).
       A virtual COM port will then appear in the HyperTerminal.

  - Hyperterminal configuration:
    - Data Length = 7 Bits
    - One Stop Bit
    - Odd parity
    - BaudRate = 9600 baud
    - Flow control: None

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
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in NUCLEO-N657X0-Q board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.
 - Run the example

