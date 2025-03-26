
## <b>Ux_Host_HID Application Description</b>

This application provides an example of Azure RTOS USBX stack usage.
It shows how to develop a USB Host Human Interface "HID" able to enumerate and communicate with a mouse or a keyboard.

The application is designed to behave as a USB HID Host, the code provides required requests to properly enumerate
HID devices, HID class APIs to decode HID reports received from a mouse or a keyboard and display data on UART HyperTerminal.

The main entry function tx_application_define() is then called by ThreadX during kernel start, at this stage, all USBx resources
are initialized, the HID class driver and HID clients are registered.
The application creates 3 threads with different priorities :
  - usbx_app_thread_entry     (Priority : 25; Preemption threshold : 25) used to initialize USB OTG HAL HCD driver and start the Host.
  - hid_mouse_thread_entry    (Priority : 30; Preemption threshold : 30) used to decode HID reports received  from a mouse.
  - hid_keyboard_thread_entry (Priority : 30; Preemption threshold : 30) used to decode HID reports received  from a keyboard.

#### <b>Expected success behavior</b>

When a HID device is plugged to NUCLEO-N657X0-Q board, a message will be displayed on the UART HyperTerminal showing
the Vendor ID and Product ID of the attached device.
After enumeration phase, a message will indicate that the device is ready for use.
The host must be able to properly decode HID reports sent by the corresponding device and display those information on the HyperTerminal.

The received HID reports are used by host to identify:
- in case of a mouse
   - (x,y) mouse position
   - Wheel position
   - Pressed mouse buttons

- in case of a keyboard
   - Pressed key

#### <b>Error behaviors</b>

Errors are detected (such as unsupported device, enumeration fail) and the corresponding message is displayed on the HyperTerminal.

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" specification and HID class specification.

#### <b>Known limitations</b>

None

#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 100 ticks/sec by default, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it, by updating the "TX_TIMER_TICKS_PER_SECOND" define in the "tx_user.h" file. The update should be reflected in "tx_initialize_low_level.S" file too.
 - ThreadX is disabling all interrupts during kernel start-up to avoid any unexpected behavior, therefore all system related calls (HAL, BSP) should be done either at the beginning of the application or inside the thread entry functions.
 - ThreadX offers the "tx_application_define()" function, that is automatically called by the tx_kernel_enter() API.
   It is highly recommended to use it to create all applications ThreadX related resources (threads, semaphores, memory pools...)  but it should not in any way contain a system API call (HAL or BSP).
 - Using dynamic memory allocation requires to apply some changes to the linker file.
   ThreadX needs to pass a pointer to the first free memory location in RAM to the tx_application_define() function,
   using the "first_unused_memory" argument.
   This requires changes in the linker files to expose this memory location.
    + For EWARM add the following section into the .icf file:
     ```
     place in RAM_region    { last section FREE_MEM };
     ```


    + The "tx_initialize_low_level.S" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.

#### <b>USBX usage hints</b>

None

### <b>Keywords</b>

Connectivity, USBX, Host, ThreadX, USB, HID, Mouse, Keyboard, UART, USART,

### <b>Hardware and Software environment</b>

  - This application runs on STM32N657xx devices.
  - This application has been tested with STMicroelectronics NUCLEO-N657X0-Q boards Revision MB1940-N657XOQ-A02 and can be easily tailored to any other supported device and development board.

- NUCLEO-N657X0-Q Set-up
    - Plug the USB HID device into the STM32N657xx board through 'Type C  to A-Female' cable to the connector:
      - CN8 : to use USB Full Speed OTG IP
    - Connect ST-Link cable to the PC USB port to display data on the HyperTerminal.

  - A virtual COM port will then appear in the HyperTerminal:
     - Hyperterminal configuration
       - Data Length = 8 Bits
       - One Stop Bit
       - No parity
       - BaudRate = 115200 baud
       - Flow control: None

<b>Note</b>
The board must be powered from an external 5V source.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

Next, this program can be run in boot from flash mode. This can be done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Ux_Host_HID.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Ux_Host_HID.bin -nk -of 0x80000000 -t fsbl -o Ux_Host_HID-trusted.bin -hv 2.1 -dump Ux_Host_HID-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin Ux_Host_HID.bin -nk -of 0x80000000 -t fsbl -o Ux_Host_HID-trusted.bin -hv 2.3 -dump Ux_Host_HID-trusted.bin*
   - The resulting binary is Ux_Host_HID-trusted.bin.       
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Ux_Host_HID_trusted.bin) in Nucleo board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Unplug the board then plug it again. The code then executes in boot from external Flash mode.


### <b>Note</b>

The user has to check the list of the COM ports in Device Manager to find out the number
of the COM ports that have been assigned (by OS) to the Stlink VCP.
The application uses the external HSE clock as USB source clock.

