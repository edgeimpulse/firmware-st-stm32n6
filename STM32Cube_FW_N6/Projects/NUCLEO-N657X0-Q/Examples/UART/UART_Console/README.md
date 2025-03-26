## <b>UART_Console Example Description</b>

How to use the HAL UART API for UART transmission (printf/getchar) via console with user interaction.

   - Board: NUCLEO-N657X0-Q (embeds a STM32N657X0H3Q device)
   - UART Instance : USART1
   - Tx Pin: PE5
   - Rx Pin: PE6

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 600 MHz for STM32N6xx Devices.

The UART peripheral configuration is ensured by the HAL_UART_Init() function.
This later is calling the HAL_UART_MspInit() function which core is implementing
the configuration of the needed UART resources according to the used hardware (CLOCK,
GPIO, DMA and NVIC). You may update this function to change UART configuration.

The UART/Hyperterminal communication is then initiated (Board is sending an information message using printf service,
inviting user to enter characters on Terminal). Sent messages are displayed on Hyperterminal console.
For getchar test, the user is prompted to enter any sequence of characters, followed by "ENTER" key.
Note that the getchar implementation ensures an echo of the received data.

The UART instance (USART1) is configured as follows:

    - BaudRate = 115200 baud
    - Word Length = 8 bits (8 data bits, no parity bit)
    - One Stop Bit
    - Hardware flow control disabled (RTS and CTS signals)
    - Reception and transmission are enabled

LED3 turns ON when there is an error.

#### <b>Notes</b>

1. When the parity is enabled, the computed parity is inserted at the MSB position of the transmitted data.

2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
   based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
   a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
   than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
   To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

3. The application needs to ensure that the SysTick time base is always set to 1 millisecond
   to have correct HAL operation.

### <b>Keywords</b>

Connectivity, UART/USART, baud rate, RS-232, full-duplex, Printf, HyperTerminal, Transmitter, Receiver, Asynchronous

### <b>Directory contents</b>

  - UART/UART_Console/FSBL/Inc/stm32n6xx_nucleo_conf.h BSP configuration file
  - UART/UART_Console/FSBL/Inc/stm32n6xx_hal_conf.h    HAL configuration file
  - UART/UART_Console/FSBL/Inc/stm32n6xx_it.h          IT interrupt handlers header file
  - UART/UART_Console/FSBL/Inc/main.h                  Header for main.c module
  - UART/UART_Console/FSBL/Src/stm32n6xx_it.c          Interrupt handlers
  - UART/UART_Console/FSBL/Src/main.c                  Main program
  - UART/UART_Console/FSBL/Src/stm32n6xx_hal_msp.c     HAL MSP module
  - UART/UART_Console/FSBL/Src/stm32n6xx_hal_msp.c     HAL MSP module
  - UART/UART_Console/FSBL/Src/system_stm32n6xx.c      STM32N6xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0HxQ devices.
  - This example has been tested with one NUCLEO-N657X0-Q board embedding
    a STM32N657X0HxQ device and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-N657X0-Q Set-up
     - Connect ST-Link cable to the PC USB port to send/receive data between PC and board.
       A virtual COM port will then appear in the HyperTerminal.

  - Hyperterminal configuration:
    - Data Length = 8 Bits
    - One Stop Bit
    - No parity
    - BaudRate = 115200 baud
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
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in the board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.

