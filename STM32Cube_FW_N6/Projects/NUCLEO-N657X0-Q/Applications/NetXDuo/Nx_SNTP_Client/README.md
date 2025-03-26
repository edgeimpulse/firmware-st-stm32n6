
## <b>Nx_SNTP_Client Application Description</b>

This application provides an example of Azure RTOS NetX/NetXDuo stack usage.
It shows how to develop a NetX SNTP client and connect with an STNP server to get a time update.

The main entry function tx_application_define() is called by ThreadX during kernel start, at this stage, all NetX resources are created.

 + A NX_PACKET_POOL is allocated.
 + A NX_IP instance using that pool is initialized.
 + The ARP, ICMP and UDP protocols are enabled for the NX_IP instance.
 + An SNTP Client "SntpClient" is created.
 + A DHCP client is created.

The application then creates 2 threads with the same priorities:

 + **NxAppThread** (priority 10, PreemtionThreashold 10) : created with the TX_AUTO_START flag to start automatically.
 + **AppSNTPThread** (priority 5, PreemtionThreashold 5) : created with the TX_DONT_START flag to be started later.

The **NxAppThread** starts and performs the following actions:

  + Starts the DHCP client
  + Waits for the IP address resolution
  + Resumes the **AppSNTPThread**

The **AppSNTPThread**, once started:

  + Creates a dns_client with USER_DNS_ADDRESS used as DNS server.
  + Initializes SntpClient in Unicast mode and set SNTP_SERVER_NAME predefined in app_netxduo.h.
  + Runs SntpClient.
  + Once a valid time update received, time will be displayed on the Hyperterminal and set to RTC.
  + RTC time will be displayed each second on the Hyperterminal.

#### <b>Expected success behavior</b>

 + The board IP address is printed on the HyperTerminal.
 + The time update sent by the SNTP server is printed on the HyperTerminal.
 + Each second, time from RTC is printed on the HyperTerminal and the green LED is toggling.

#### <b>Error behaviors</b>

+ The red LED is toggling to indicate any error that has occurred.

#### <b>Assumptions if any</b>

- The application is using the DHCP to acquire IP address, thus a DHCP server should be reachable by the board in the LAN used to test the application.
- The application is configuring the Ethernet IP with a static predefined _MAC Address_, make sure to change it in case multiple boards are connected on the same LAN to avoid any potential network traffic issues.
- The _MAC Address_ is defined in the `main.c`

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
     ```

#### <b>NetX Duo usage hints</b>

- Depending on the application scenario, the total TX and RX descriptors may need to be increased by updating respectively  the "ETH_TX_DESC_CNT" and "ETH_RX_DESC_CNT" in the "stm32n6xx_hal_conf.h", to guarantee the application correct behaviour, but this will cost extra memory to allocate.
- The NetXDuo application needs to allocate the <b> <i> NX_PACKET </i> </b> pool in a dedicated section.
Below is an example of the section declaration for different IDEs.
   + For EWARM ".icf" file
   ```
   define symbol __ICFEDIT_region_NXDATA_start__ = 0x341F5C00;
   define symbol __ICFEDIT_region_NXDATA_end__   = 0x341FFC00;
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

RTOS, Network, ThreadX, NetXDuo, SNTP, UART

### <b>Hardware and Software environment</b>

  - This application runs on STM32N657xx devices.
  - This application has been tested with STMicroelectronics NUCLEO-N657X0-Q boards Revision MB1940-N657XOQ-A02 and can be easily tailored to any other supported device and development board.

  - This application uses USART1 to display logs, the hyperterminal configuration is as follows:
      - BaudRate = 115200 baud
      - Word Length = 8 Bits
      - Stop Bit = 1
      - Parity = None
      - Flow control = None

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Edit the file app_netxduo.h : define the USER_DNS_ADDRESS, define the SNTP server name to connect to.
 - Rebuild all files and load your image into target memory. Code can be executed in this mode for debugging purposes.

Next, this program can be run in boot from flash mode. This can be done by following the instructions below:

 - Resort to CubeProgrammer to add a header to the generated binary Nx_SNTP_Client.bin with the following command
   - for Cut1.1,
     - *STM32MP_SigningTool_CLI.exe -bin Nx_SNTP_Client.bin -nk -of 0x80000000 -t fsbl -o Nx_SNTP_Client-trusted.bin -hv 2.1 -dump Nx_SNTP_Client-trusted.bin*
   - for Cut2.0, 
      - *STM32MP_SigningTool_CLI.exe -bin Nx_SNTP_Client.bin -nk -of 0x80000000 -t fsbl -o Nx_SNTP_Client-trusted.bin -hv 2.3 -dump Nx_SNTP_Client-trusted.bin*
   - The resulting binary is Nx_SNTP_Client-trusted.bin.         
 - Next, in resorting again to CubeProgrammer, load the binary and its header (Nx_SNTP_Client_trusted.bin) in Nucleo board external Flash at address 0x7000'0000.
 - Set the boot mode in boot from external Flash (BOOT0 switch position is 1-2 and BOOT1 switch position is 1-2).
 - Press the reset button. The code then executes in boot from external Flash mode.