## <b>CRC_ReverseModes Example Description</b>

How to configure the CRC using the HAL API. The CRC (cyclic
redundancy check) calculation unit computes the CRC code of a given buffer of
32-bit data( words), input or output data reversal features are
enabled, using a fixed generator polynomial(0x4C11DB7):
X^32 + X^26 + X^23 + X^22 + X^16 + X^12 + X^11 + X^10 +X^8 + X^7 + X^5 + X^4 + X^2+ X +1  used in MPEG2.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 600 MHz.

The CRC peripheral configuration is ensured by HAL_CRC_Init() function.
The latter is calling HAL_CRC_MspInit() function which core is implementing
the configuration of the needed CRC resources according to the used hardware (CLOCK).
You can update HAL_CRC_Init() input parameters to change the CRC configuration.


In this example,  the default generating polynomial is configured by
HAL_CRC_Init(). At the same time, it is set that neither input or output data
must be reversed, the default init value is used and it is specified that input
data type is word.

This example consists of CRC calculating with the different reversal modes in the following order:

 First, input data reversal modes: bit-reversal by word, bit-reversal by byte,bit-reversal by half-word
                             byte-reversal by word, half-word reversal by word.
                             
 Second, output data reversal modes: bit-reversal, byte reversal by word, half-word reversal by word.

Each time, the calculated CRC code is stored in uwCRCValue variable.
Once calculated, the CRC value (uwCRCValue) is compared to the CRC expected value.

STM32 board LED is used to monitor the example status:

  - Green LED (LED3) is ON when the correct CRC value is calculated.
  - Red  LED (LED2) is slowly blinking, when there is an error in initialization or if an incorrect CRC value is calculated.

#### <b>Notes</b>

1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
   based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
   a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
   than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
   To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
   to have correct HAL operation.

### <b>Keywords</b>

Security, CRC, CRC Polynomial, CRC-32/MPEG2, hardware CRC, Input/Output Data reversing

### <b>Directory contents</b>

  - CRC/CRC_ReverseModes/FSBL/Inc/stm32n6xx_nucleo_conf.h     BSP configuration file
  - CRC/CRC_ReverseModes/FSBL/Inc/stm32n6xx_hal_conf.h        HAL configuration file
  - CRC/CRC_ReverseModes/FSBL/Inc/stm32n6xx_it.h              Interrupt handlers header file
  - CRC/CRC_ReverseModes/FSBL/Inc/main.h                      Header for main.c module  
  - CRC/CRC_ReverseModes/FSBL/Src/stm32n6xx_it.c              Interrupt handlers
  - CRC/CRC_ReverseModes/FSBL/Src/stm32n6xx_hal_msp.c         HAL MSP file
  - CRC/CRC_ReverseModes/FSBL/Src/main.c                      Main program
  - CRC/CRC_ReverseModes/FSBL/Src/system_stm32n6xx_fsbl.c     STM32N6xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32N657X0HxQ devices.

  - This example has been tested with NUCLEO-N657X0-Q board and can be
    easily tailored to any other supported device and development board.

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

