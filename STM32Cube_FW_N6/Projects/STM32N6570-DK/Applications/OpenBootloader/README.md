## <b>OpenBootloader Application Description</b>

This application exploits OpenBootloader Middleware to demonstrate how to develop an application
that can be used to program OTP words.

This application runs from RAM memory.

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock (SYSCLK).

Then, these protocols and memories interfaces are configured:

  - USART1
  - USB1 HS
  - RAM interface
  - OTP interface

Then, the OpenBootloader application will wait for incoming communication on one of the supported protocols.

Once a communication is established through one of the supported protocols,
the OpenBootloader application will wait for commands sent by the host.

### <b>Keywords</b>

OpenBootloader, USART, USB, OTP

### <b>Directory contents</b>

     - Core/Src/main.c                              Main program file
     - Core/Src/stm32n6xx_hal_msp.c                 MSP Initialization file
     - Core/Src/stm32n6xx_it.c                      Interrupt handlers file
     - Core/Src/system_stm32n6xx_fsbl.c             STM32N6xx system clock configuration file
     - Core/Inc/main.h                              Main program header file
     - Core/Inc/stm32n6xx_hal_conf.h                HAL Library Configuration file
     - Core/Inc/stm32n6xx_it.h                      Interrupt handlers header file
     - OpenBootloader/App/app_openbootloader.c      OpenBootloader application entry point
     - OpenBootloader/App/app_openbootloader.h      Header for OpenBootloader application entry file
     - OpenBootloader/Target/common_interface.c     Contains common functions used by different interfaces
     - OpenBootloader/Target/common_interface.h     Header for common functions file
     - OpenBootloader/Target/interfaces_conf.h      Contains Interfaces configuration
     - OpenBootloader/Target/iwdg_interface.c       Contains IWDG interface
     - OpenBootloader/Target/iwdg_interface.h       Header of IWDG interface file
     - OpenBootloader/Target/openbootloader_conf.h  Header file that contains OpenBootloader HW dependent configuration
     - OpenBootloader/Target/otp_interface.c        Contains OTP interface
     - OpenBootloader/Target/otp_interface.h        Header of OTP interface file
     - OpenBootloader/Target/platform.h             Header for patterns
     - OpenBootloader/Target/ram_interface.c        Contains RAM interface
     - OpenBootloader/Target/ram_interface.h        Header of RAM interface file
     - OpenBootloader/Target/usart_interface.c      Contains USART interface
     - OpenBootloader/Target/usart_interface.h      Header of USART interface file
     - OpenBootloader/Target/usb_interface.c        Contains USB interface
     - OpenBootloader/Target/usb_interface.h        Header of USB interface file
     - USB_Device/App/usb_device.c                  USB Device applicative file
     - USB_Device/App/usb_device.h                  USB Device applicative header file
     - USB_Device/App/usbd_desc.c                   USB Device descriptor file
     - USB_Device/App/usbd_desc.h                   USB Device descriptor header file
     - USB_Device/App/usbd_dfu_if.c                 USB Device interface applicative file
     - USB_Device/App/usbd_dfu_if.h                 USB Device interface applicative header file
     - USB_Device/Target/usbd_conf.c                Contains user defines for USB configuration
     - USB_Device/Target/usbd_conf.h                USB configuration header file

### <b>Hardware and Software environment</b>

  - This application runs on STM32N6 devices..

  - This application has been tested with STMicroelectronics STM32N6570-DK (MB1939-N6570-A03A)
    board and can be easily tailored to any other supported device and development board.

  - STM32H7S78-DK set-up to use USART:
    - To use the USART1 for communication you have to connect:
      - Tx pin of your host adapter to PE5
      - Rx pin of your host adapter to PE6
    - These pins are already connected to the board embedded ST-Link

  - STM32H7S78-DK set-up to use USB:
    - USB1 CN18

**WARNING**

When OTP fuses are set, they can not be reset.

NOTE : for Cut1.1, the bootrom prevents OTP programming when booting from flash. The FSBL code must then be executed once in dev boot mode to set the OTP fuse.

### <b>How to use it?</b>

The example can be run either in development mode or in Load and Run mode.

#### <b>Development mode</b>

In order to make the program work, you must do the following :

  - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn't matter).
  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.
  - Run the application
  - Run STM32CubeProgrammer and connect to OpenBootloader using USART1</br>
    Or
  - Connect your USB host adapter and connect to OpenBootloader using USB HS</br>

#### <b>Load and Run mode</b>

This mode enables execution without having to connect through an IDE. The application will be stored in RAM memory and therefore will no require any external tools once loaded onto the board.

 - Resort to STM32CubeProgrammer to add a header to a binary OpenBootloader_STM32N6570-DK.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin OpenBootloader_STM32N6570.bin -nk -of 0x80000000 -t fsbl -o OpenBootloader_STM32N6570-trusted.bin -hv 2.1 -dump OpenBootloader_STM32N6570-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin OpenBootloader_STM32N6570.bin -nk -of 0x80000000 -t fsbl -o OpenBootloader_STM32N6570-trusted.bin -hv 2.3 -dump OpenBootloader_STM32N6570-trusted.bin*
   - The resulting binary is OpenBootloader_STM32N6570-trusted.bin.       
 - Create a file called FlashLayout.tsv and write this content in it
   -#Opt	Id	Name	Type	IP	Offset	Binary
   -P	0x1	fsbl-openbl	Binary	none	0x0	OpenBootloader_STM32N6570-DK-Signed.stm32

### <b>How to use program NOR memory (MX66UW1G45G)?</b>

To be able to program the NOR memory (MX66UW1G45G) present in the STM32N6570-DK board, these steps must be followed:

  - Set the boot mode to boot on Boot Rom (BOOT1 switch position is 1-2, BOOT0 switch position doesn't matter).
  - Open the folder *NOR_Binary* that is inside *Binaries* directory.
  - Open the file *FlashLayout.tsv* in STM32CubeProgrammer.
  - Connect to USART or USB.
  - Then press *Download* button.

To load a different binary file, place the binary inside the directory *Binaries\NOR_Binary\* and edit the *FlashLayout.tsv* file to change the file name from *RamdomData.bin* to your file name.
