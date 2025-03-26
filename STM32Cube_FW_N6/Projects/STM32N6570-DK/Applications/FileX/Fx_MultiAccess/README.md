
## <b>Fx_MultiAccess Application Description</b>

This application provides an example of Azure RTOS FileX stack usage on STM32N6570-DK board, it demonstrates the FileX's concurrent file access capabilities. The application is designed to execute file operations on the SD card device, the code provides all required software code for handling SD card I/O operations.

The application starts by calling the ThreadX's initialization routine which creates multithreaded environment for starting the concurrent FileX's two main threads that handle file operations. At this stage, all FileX resources are created, the SDIO driver is initialized and the application creates 3 threads with the same priorities:

  - fx_app_thread  (Prio : 10; PreemptionPrio : 10) used to initialize the SD card driver and opening it as a FileX Media.
  - fx_thread_one  (Prio : 10; PreemptionPrio : 10) used to create, write and read operations for file fx_file_one.
  - fx_thread_two  (Prio : 10; PreemptionPrio : 10) used to create, write and read operations for file fx_file_two.

<b>Note</b>:
A FAT32 compatible SD card is expected to be used with this example. The program will start file operations without formatting the media, so all user related files are kept.

#### <b>Expected success behavior</b>

- When an SD card is inserted into the STM32N6570-DK SD card reader and the board is powered up, the application starts file operations.
- During the start phase, the main thread starts by opening the SD card using FileX media open service. Upon successful opening of the media and its underlying FAT32 file
  system, the main thread starts the two file operations threads, fx_thread_one and fx_thread_two. The two threads then start file operations, each at a different file.
- This demonstrates the concurrent access handling capabilities of FileX, without the need for an external access-aribitration locks.
- Successful operation is marked by a toggling LED_GREEN light.

#### <b>Error behaviors</b>

On failure, the LED_RED starts toggling while the LED_GREEN is switched OFF.

#### <b>Assumptions if any</b>

Hotplug is not implemented for this application, the SD card is expected to be inserted before application start.

#### <b>Known limitations</b>

None

#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 100 ticks/sec by default, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it, by updating the "TX_TIMER_TICKS_PER_SECOND" define in the "tx_user.h" file. The update should be reflected in "tx_initialize_low_level.S" file too.
 - ThreadX is disabling all interrupts during kernel start-up to avoid any unexpected behavior, therefore all system related call (HAL) should be done either at the beginning of the application or inside the thread entry functions.
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

RTOS, ThreadX, FileX, File system, SDMMC, SDIO, FAT32

### <b>Hardware and Software environment</b>

  - This example runs on STM32N6xx devices.
  - This example has been tested with STMicroelectronics STM32N6570-DK boards revision: MB1939-N6570-A03.
    and can be easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

  - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn't matter).
  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.
  - Run the application

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Fx_MultiAccess.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Fx_MultiAccess.bin -nk -of 0x80000000 -t fsbl -o Fx_MultiAccess-trusted.bin -hv 2.1 -dump Fx_MultiAccess-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin Fx_MultiAccess.bin -nk -of 0x80000000 -t fsbl -o Fx_MultiAccess-trusted.bin -hv 2.3 -dump Fx_MultiAccess-trusted.bin*
   - The resulting binary is Fx_MultiAccess-trusted.bin.       
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Fx_MultiAccess_trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.
