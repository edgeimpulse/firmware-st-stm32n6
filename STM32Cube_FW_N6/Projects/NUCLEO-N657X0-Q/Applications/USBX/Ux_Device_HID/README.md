
## <b>Ux_Device_HID Application Description</b>

This application provides an example of Azure RTOS USBX stack usage on NUCLEO-N657X0-Q board,
it shows how to develop USB Device Human Interface "HID" mouse based application.

The application is designed to emulate an USB HID mouse device, the code provides all required device descriptors framework
and associated Class descriptor report to build a compliant USB HID mouse device.

At the beginning ThreadX call the entry function tx_application_define(), at this stage, all USBx resources
are initialized, the HID Class driver is registered and the application creates 2 threads with the same priorities :

  - app_ux_device_thread_entry (Prio : 10; PreemptionPrio : 10) used to initialize USB_OTG HAL PCD driver and start the device.
  - usbx_hid_thread_entry (Prio : 20; PreemptionPrio : 20) used to send HID reports to move automatically the PC host machine cursor.
To customize the HID application by sending the mouse position step by step every 10ms.
For each 10ms, the application calls the GetPointerData() API to update the mouse position (x, y) and send
the report buffer through the ux_device_class_hid_event_set() API.

#### <b>Expected success behavior</b>

When plugged to PC host, the NUCLEO-N657X0-Q must be properly enumerated as an USB HID mouse device.
During the enumeration phase, device provides host with the requested descriptors (Device, configuration, string).
Those descriptors are used by host driver to identify the device capabilities.
Once the NUCLEO-N657X0-Q USB device successfully completed the enumeration phase, the device sends a HID report after a user button press.
Each report sent should move the mouse cursor by one step on host side.

#### <b>Error behaviors</b>

Host PC shows that USB device does not operate as designed (Mouse enumeration failed, the mouse pointer doesn't move).

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" Specification and HID class Specification.

#### <b>Known limitations</b>

The remote wakeup feature is not yet implemented (used to bring the USB suspended bus back to the active condition).

#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 100 ticks/sec by default, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it in the "tx_user.h", the "TX_TIMER_TICKS_PER_SECOND" define,but this should be reflected in "tx_initialize_low_level.S" file too.
 - ThreadX is disabling all interrupts during kernel start-up to avoid any unexpected behavior, therefore all system related calls (HAL, BSP) should be done either at the beginning of the application or inside the thread entry functions.
 - ThreadX offers the "tx_application_define()" function, that is automatically called by the tx_kernel_enter() API.
   It is highly recommended to use it to create all applications ThreadX related resources (threads, semaphores, memory pools...)  but it should not in any way contain a system API call (HAL or BSP).
 - Using dynamic memory allocation requires to apply some changes to the linker file.
   ThreadX needs to pass a pointer to the first free memory location in RAM to the tx_application_define() function,
   using the "first_unused_memory" argument.
   This require changes in the linker files to expose this memory location.
    + For EWARM add the following section into the .icf file:
     ```
     place in RAM_region    { last section FREE_MEM };
     ```

    + The "tx_initialize_low_level.S" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.

### <b>Keywords</b>

RTOS, ThreadX, USBX, Device, USB_OTG, High Speed, HID, Mouse,

### <b>Hardware and Software environment</b>

  - This application runs on STM32N657xx devices.
  - This application has been tested with STMicroelectronics NUCLEO-N657X0-Q boards Revision MB1940-N657XOQ-A02 and can be easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 2-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

Next, this program can be run in boot from flash mode. This can be done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Ux_Device_HID.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Ux_Device_HID.bin -nk -of 0x80000000 -t fsbl -o Ux_Device_HID-trusted.bin -hv 2.1 -dump Ux_Device_HID-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin Ux_Device_HID.bin -nk -of 0x80000000 -t fsbl -o Ux_Device_HID-trusted.bin -hv 2.3 -dump Ux_Device_HID-trusted.bin*
   - The resulting binary is Ux_Device_HID-trusted.bin.         
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Ux_Device_HID_trusted.bin) in Nucleo board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Unplug the board then plug it again. The code then executes in boot from external Flash mode.