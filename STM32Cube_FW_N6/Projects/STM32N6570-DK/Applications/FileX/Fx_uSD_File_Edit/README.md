
## <b>Fx_uSD_File_Edit Application Description</b>

This application provides an example of Azure RTOS FileX stack usage on STM32N6570-DK, it shows how to develop a basic SD card file
operations application. The application is designed to handle SD card insertion/removal events, and depending on that state, it starts
and stops file operations from and into the SD card.

The application is developed using HAL, so managing leds, SD initialisation or any SD card insertion/removal mechanisms are implemented using HAL drivers without any reference to BSP.

The main entry function, tx_application_define(), is called by ThreadX during kernel start. At this stage, all FileX resources are initialized,
the SD card detection event is registered and drivers are initialized.

A single thread is created:

  - fx_app_thread (Prio : 10; PreemptionPrio : 10) used to initialize the SD driver and starting FileX's file operations.

A message queue is used to signal the SD card detection event to the fx_app_thread thread:

  - tx_msg_queue (Msg size : 1 (UINT); total messages : 16) used to notify the fx_app_thread about the SD card insertion status.

The fx_app_thread starts by checking whether the SD card is initially inserted or not. In the true case, it sends a message to the queue to ensure
that the first iteration starts properly. The wait on the message queue blocks till it receives a new message about whether the SD card is inserted
or removed. Interrupt callback for SD detection is registered and it is used to send the event information to the message queue.

The fx_app_thread uses FileX services to open the SD media for file operations and attempts to create file STM32.TXT. If the file exists already,
it will be overwritten. Dummy content is then written into the file and it is closed. The file is opened once again in read mode and its content
is checked if matches what was previously written.

It is possible to unplug/plug or replace the SD card without any need to reset the STM32N6570-DK.
That is why MX_SDMMC1_SD_Init() should be public to initialize the newly plugged SD card.

#### <b>Expected success behavior</b>

- A file named STM32.TXT should be visible in the root directory of the SD card.
- A blinking green LED light marks the success of the file operations.

#### <b>Error behaviors</b>

- On failure, the red LED should start blinking.
- Error handler is called at the spot where the error occurred.

#### <b>Assumptions if any</b>

- The SD card should be plugged prior to run the application.

#### <b>Known limitations</b>

Performing quick plug/unplug of SD card may trigger the Error_Handler() function.

#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 100 ticks/sec by default, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it, by updating the "TX_TIMER_TICKS_PER_SECOND" define in the "tx_user.h" file. The update should be reflected in "tx_initialize_low_level.S" file too.
 - ThreadX is disabling all interrupts during kernel start-up to avoid any unexpected behavior, therefore all system related calls (HAL) should be done either at the beginning of the application or inside the thread entry functions.
 - ThreadX offers the "tx_application_define()" function, that is automatically called by the tx_kernel_enter() API.
   It is highly recommended to use it to create all applications ThreadX related resources (threads, semaphores, memory pools...)  but it should not in any way contain a system API call (HAL).
 - Using dynamic memory allocation requires to apply some changes to the linker file.
   ThreadX needs to pass a pointer to the first free memory location in RAM to the tx_application_define() function,
   using the "first_unused_memory" argument.
   This requires changes in the linker files to expose this memory location.
    + For EWARM add the following section into the .icf file:
     ```
	 place in RAM_region    { last section FREE_MEM };
	 ```

#### <b>FileX/LevelX usage hints</b>

- When calling the fx_media_format() API, it is highly recommended to understand all the parameters used by the API to correctly generate a valid filesystem.
- FileX is using data buffers, passed as arguments to fx_media_open(), fx_media_read() and fx_media_write() API it is recommended that these buffers are multiple of sector size and "32 bytes" aligned to avoid cache maintenance issues.

### <b>Keywords</b>

RTOS, ThreadX, FileX, File system, SDMMC, FAT32

### <b>Hardware and Software environment</b>

  - This application runs on STM32N6xx devices.
  - This application has been tested with STMicroelectronics STM32N6570-DK boards Revision: MB1939-N6570-A03.
    and can be easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

  - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn't matter).
  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.
  - Run the application

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Fx_uSD_File_Edit.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Fx_uSD_File_Edit.bin -nk -of 0x80000000 -t fsbl -o Fx_uSD_File_Edit-trusted.bin -hv 2.1 -dump Fx_uSD_File_Edit-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin Fx_uSD_File_Edit.bin -nk -of 0x80000000 -t fsbl -o Fx_uSD_File_Edit-trusted.bin -hv 2.3 -dump Fx_uSD_File_Edit-trusted.bin*
   - The resulting binary is Fx_uSD_File_Edit-trusted.bin.         
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Fx_uSD_File_Edit_trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.