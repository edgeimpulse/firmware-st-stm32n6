
##  <b>NX_RTP_RTSP_Server Application Description</b>

This application provides an example of the H264 video encoder streamed through Azure RTOS NetX/NetXDuo on STM32N6570-DK board.
It shows how to develop a NetXDuo RTSP server to send encoded camera video to a client.

The application aims to stream an encoded video format H264. Then,the H264 frames are sent through Ethernet peripheral
using the RTP protocol (Real-time Transport Protocol) to a remote client for example
VLC media player. The RTSP protocol (Real Time Streaming Protocol) is used to control
media sessions (SETUP, PLAY, TEARDOWN).

The main entry function tx_application_define() is called by ThreadX during kernel start, at this stage, all NetX resources are created.

 + A <i>NX_PACKET_POOL</i>is allocated
 + A <i>NX_IP</i> instance using that pool is initialized
 + The <i>ARP</i>, <i>ICMP</i>, <i>UDP</i> and <i>TCP</i> protocols are enabled for the <i>NX_IP</i> instance
 + A <i>DHCP client is created.</i>

The application then creates 2 threads with different priorities:

 + **NxAppThread** (priority 10, PreemtionThreashold 10) : created with the <i>TX_AUTO_START</i> flag to start automatically.
 + **AppServerThread** (priority 4, PreemtionThreashold 4) : created with the <i>TX_AUTO_START</i> flag to start automatically.
 + **VENCAppThread** (priority 10, PreemtionThreashold 10) : created with the <i>TX_AUTO_START</i> flag to start automatically.

The **NxAppThread** starts and perform the following actions:

  + Starts the DHCP client
  + Waits for the IP address resolution
  + Resumes the **AppServerThread**

The **AppServerThread**, once started:

  + Starts RTSP server
  + Starts the demo video
  + Displays a report message on the HyperTerminal at each received frame on the VLC media player


The **VENCAppThread**, once started:

  + Starts the LCD screen for camera preview
  + Initialize the camera stream
  + Initialize the video encoder
  + Encode the video stream and place in an output buffer

####  <b>Expected success behavior</b>

 + The board IP address is printed on the HyperTerminal
 + Streaming video is well displayed on the VLC media player after entering the url: rtsp://IP@  (Media/Open Network Stream/Open Media window)
 + It is recommended that the caching value is set to 500ms on the VLC to keep the stream running smoothly
  (Media/Open Network Stream/Open Media window => Show more options), then press Play on VLC Play button.
 + The response messages sent by the server are printed on the HyperTerminal.

#### <b>Expected error behavior</b>

+ The red LED toggles each 1000ms to indicate that an error has occurred.
+ In case the message exchange is not completed the Hyperterminal is not printing the received messages.
+ ""Critical error has occurred" message is printed on the HyperTerminal.

#### <b>Assumptions if any</b>

- The application is using the DHCP to acquire IP address, thus a DHCP server should be reachable by the board in the RTL used to test the application.
- The application is configuring the Ethernet IP with a static predefined <i>MAC Address</i>, make sure to change it in case multiple boards are connected on the same RTL to avoid any potential network traffic issues.
- The <i>MAC Address</i> is defined in the `main.c`

```
void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH1;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE0;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x10;
  MACAddr[5] = 0x00;
```
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


#### <b>NetX Duo usage hints</b>

- Depending on the application scenario, the total TX and RX descriptors may need to be increased by updating respectively  the "ETH_TX_DESC_CNT" and "ETH_RX_DESC_CNT" in the "stm32n6xx_hal_conf.h", to guarantee the application correct behaviour, but this will cost extra memory to allocate.
- The NetXDuo application needs to allocate the <b> <i> NX_PACKET </i> </b> pool in a dedicated section.
Below is an example of the section declaration for different IDEs.
   + For EWARM ".icf" file
   ```
   define symbol __ICFEDIT_region_NXDATA_start__ = 0x341F8180;
   define symbol __ICFEDIT_region_NXDATA_end__   = 0x341FF980;
   define region NXApp_region  = mem:[from __ICFEDIT_region_NXDATA_start__ to __ICFEDIT_region_NXDATA_end__];
   place in NXApp_region { section .NetXPoolSection};

  This section is then used in the <code> app_azure_rtos.c</code> file to force the <code>nx_byte_pool_buffer</code> allocation.

```
/* USER CODE BEGIN NX_Pool_Buffer */  

#if defined ( __ICCARM__ ) /* IAR Compiler */
#pragma location = ".NetXPoolSection"

#else /* GNU and AC6 compilers */
__attribute__((section(".NetXPoolSection")))

#endif

/* USER CODE END NX_Pool_Buffer */
static UCHAR  nx_byte_pool_buffer[NX_APP_MEM_POOL_SIZE];
static TX_BYTE_POOL nx_app_byte_pool;
```
For more details about the MPU configuration please refer to the [AN4838](https://www.st.com/resource/en/application_note/dm00272912-managing-memory-protection-unit-in-stm32-mcus-stmicroelectronics.pdf)

### <b>Keywords</b>

RTOS, Network, ThreadX, NetXDuo, RTP, RTSP, TCP, UDP, UART

### <b>Hardware and Software environment</b>

  - This application runs on STM32N657xx devices.
  - This application has been tested with STMicroelectronics STM32N6570-DK boards Revision MB1939-N6570-A03 and can be easily tailored to any other supported device and development board.

  - This application uses USART1 to display logs, the hyperterminal configuration is as follows:
      - BaudRate = 115200 baud
      - Word Length = 8 Bits
      - Stop Bit = 1
      - Parity = None
      - Flow control = None

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

In order to make the program work, you must do the following :

 - Set the boot mode in development mode (BOOT1 switch position is 1-3, BOOT0 switch position doesn't matter).
 - Open your preferred toolchain
 - Select first the FSBL workspace
 - Rebuild all files from sub-project FSBL (if no modification is done on FSBL project, this step can be done only once)
 - Select the Appli workspace
 - Rebuild all files from sub-project Appli
 - Resort to CubeProgrammer to add a header to the generated Appli binary Project.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.1 -dump Project-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin Project.bin -nk -of 0x80000000 -t fsbl -o Project-trusted.bin -hv 2.3 -dump Project-trusted.bin*
   - The resulting binary is Project-trusted.bin.
 - Next, in resorting again to CubeProgrammer, load the Appli binary and its header (Project-trusted.bin) in DK board external Flash at address 0x7010'0000.
 - Load the FSBL binary in internal RAM using the IDE
 - Run the example

 Next, this program can be run in boot from flash mode. This is done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary FSBL.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin FSBL.bin -nk -of 0x80000000 -t fsbl -o FSBL-trusted.bin -hv 2.1 -dump FSBL-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin FSBL.bin -nk -of 0x80000000 -t fsbl -o FSBL-trusted.bin -hv 2.3 -dump FSBL-trusted.bin*
   - The resulting binary is FSBL-trusted.bin.       
 - In resorting again to CubeProgrammer, load the FSBL binary and its header (FSBL-trusted.bin) in DK board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.
