## <b>PSSI_Master_Single_Com Example Description</b>
How to handle a single communication procedure using two boards with PSSI in polling mode.

      - Board: NUCLEO-N657X0-Q's (embeds a STM32N657X0 device)
      - PSSI_PDCK Pin: PG1 (Morpho CN15 pin13)
      - PSSI_D0   Pin: PA1 (Morpho CN15 pin38)
      - PSSI_D1   Pin: PE6 (Morpho CN15 pin2)
      - PSSI_D2   Pin: PC3 (Morpho CN3 pin25)
      - PSSI_D3   Pin: PB9 (Morpho CN15 pin16)
      - PSSI_D4   Pin: PC11 (Morpho CN3 pin27)
      - PSSI_D5   Pin: PE4 (Morpho CN3 pin21)
      - PSSI_D6   Pin: PG2 (Morpho CN15 pin15)
      - PSSI_D7   Pin: PC1 (Morpho CN15 pin5)
      - PSSI_DE   Pin: PC0 (Morpho CN3 pin17)
      - TIM1      Pin: PD6 (Morpho CN15 pin6)
      Connect each PSSI_Dx with wires between the boards.
      Connect PSSI_DE together to allow the synchronization with the Slave board.
      Connect PSSI_PDCK pin to the TIM1 pin on the Master Board.
      Connect GND between the two boards.

At the beginning of the main program the HAL_Init() function is called to reset all the peripherals, initialize
the flash interface and the systick.
This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This is provided as template implementation that the User may integrate in his application in order to enhance the performance.

The PSSI peripheral configuration is ensured by the HAL_PSSI_Init() function.
This later is calling the HAL_PSSI_MspInit()function which core is implementing the configuration of the needed PSSI
resources according to the used hardware (CLOCK, GPIO and NVIC).
User may update this function to change PSSI configuration.

Then, the TIM peripheral is initialized to generate a PWM signal to simulate a clock on the Master board.

In this example, the PSSI is set to 8-bit width data with 8 lines bus width.
The project is split in two parts : the Master Board and the Slave Board.
Master Board :
  The Master Board will generate a clock for the Slave with the function Start_Clock_Generation() and will send a buffer
  using the function HAL_PSSI_Transmit().

Slave Board :
  The Slave Board will receive the buffer sent by the Master Board with the function HAL_PSSI_Receive(). It will then
  compare the buffer received to the buffer expected.

For this example the buffer sent is predefined.

In a first step, after the user press the USER push-button on the Master Board, the simulated Master will generate a
signal PWM as PSSI_PDCK clock generation with the function Start_Clock_Generation() before launching the transmission of
a buffer through HAL_PSSI_Transmit(). When the transmission is done, the Master will stop the PWM generation with
Stop_Clock_Generation() and the LED1 will light up. On the Slave side, it will receive the buffer sent through
HAL_PSSI_Receive(). After the reception is done, the Slave will compare the buffer received to the buffer sent. If the
message expected is in the buffer received, the LED1 will also light up on the Slave Board.

NUCLEO-N657X0-Q's LEDs can be used to monitor the transfer status:

 - LED1 toggles when the transmission and the reception are done.
 - LED2 toggles slowly when there is an error in the initialization and transmission process.


#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>
Transmission, Reception, Master, Slave, PSSI, PWM, Signal, Buffer, Bus Width.

### <b>Directory contents</b>

File | Description
 --- | ---  
  - PSSI/PSSI_Master_Single_Com/FSBL/Inc/stm32n6xx_nucleo_conf.h   | BSP configuration file
  - PSSI/PSSI_Master_Single_Com/FSBL/Inc/stm32n6xx_hal_conf.h      | HAL configuration file
  - PSSI/PSSI_Master_Single_Com/FSBL/Inc/stm32n6xx_it.h            | PSSI interrupt handlers header file
  - PSSI/PSSI_Master_Single_Com/FSBL/Inc/main.h                    | Header for main.c module
  - PSSI/PSSI_Master_Single_Com/FSBL/Src/stm32n6xx_it.c            | PSSI interrupt handlers
  - PSSI/PSSI_Master_Single_Com/FSBL/Src/main.c                    | Main program
  - PSSI/PSSI_Master_Single_Com/FSBL/Src/system_stm32n6xx.c        | stm32n6xx system source file
  - PSSI/PSSI_Master_Single_Com/FSBL/Src/stm32n6xx_hal_msp.c       | HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0 devices.

  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-N657X0-Q Set-up
    - Use a wire to connect the TIM1 pin (PD6 Morpho CN15 pin6) of the Master board to the
    PSSI_PDCK Pin (PG1 Morpho CN15 pin13) of the two boards.
    - Connect each PSSI_Dx pin with wires between the two boards.
    - Connect GND of both boards together.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

Next, this program can be run in boot from flash mode. This can be done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Project.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.1 -dump Project-trusted.bin*
   - for Cut2.0,
      - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Project-trusted.bin) in Nucleo board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.
