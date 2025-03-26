/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.c
  * @author  MCD Application Team
  * @brief   NetXDuo applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_netxduo.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nxd_dhcp_client.h"
#include "app_rtsp_over_rtp.h"
#include "main.h"
#include "stm32_lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

TX_THREAD AppMainThread;
TX_THREAD AppUDPThread;
TX_THREAD AppLinkThread;
TX_SEMAPHORE DHCPSemaphore;

NX_PACKET_POOL AppPool;

NX_IP IpInstance;
NX_DHCP DHCPClient;
ULONG IpAddress;
ULONG NetMask;

UCHAR *pointer;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEMO_DATA_READY_EVENT             ((ULONG)0x00000008)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static VOID App_Main_Thread_Entry(ULONG thread_input);
static VOID App_Link_Thread_Entry(ULONG thread_input);
static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr);
extern VOID sample_entry(NX_IP* ip_ptr, NX_PACKET_POOL* pool_ptr, VOID *usr_ptr, UINT (*unix_time_callback)(ULONG *unix_time));
/* USER CODE END PFP */

/**
  * @brief  Application NetXDuo Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN MX_NetXDuo_MEM_POOL */

  /* USER CODE END MX_NetXDuo_MEM_POOL */

  /* USER CODE BEGIN MX_NetXDuo_Init */
  printf("Nx_RTP_RTSP_Server application started..\n");

  /* Allocate the memory for packet_pool.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_PACKET_POOL_SIZE, TX_NO_WAIT)!= TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the Packet pool to be used for packet allocation */
  ret = nx_packet_pool_create(&AppPool, "Main Packet Pool", PAYLOAD_SIZE, pointer, NX_PACKET_POOL_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_POOL_ERROR;
  }

   /* Allocate the NX_IP instance pool. */
  ret = tx_byte_allocate(byte_pool, (VOID **) &pointer, 2 * DEFAULT_MEMORY_SIZE, TX_NO_WAIT);

  if (ret != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the main NX_IP instance */
  ret = nx_ip_create(&IpInstance, "Main Ip instance", NX_APP_DEFAULT_IP_ADDRESS, NX_APP_DEFAULT_NET_MASK, &AppPool, nx_stm32_eth_driver,
                     pointer, 2 * DEFAULT_MEMORY_SIZE, IP_THREAD_PRIORITY);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Allocate the packet pool. */
  ret = tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_MEMORY_SIZE, TX_NO_WAIT);

  if (ret != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /*  Enable the ARP protocol and provide the ARP cache size for the IP instance */
  ret = nx_arp_enable(&IpInstance, (VOID *)pointer, DEFAULT_MEMORY_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable the ICMP */
  ret = nx_icmp_enable(&IpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable TCP traffic. */
  ret = nx_tcp_enable(&IpInstance);

  if (ret != TX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable UDP traffic. */
  ret = nx_udp_enable(&IpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Allocate the main thread pool. */
  ret = tx_byte_allocate(byte_pool, (VOID **) &pointer, 2 * DEFAULT_MEMORY_SIZE, TX_NO_WAIT);

  if (ret != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the main thread */
  ret = tx_thread_create(&AppMainThread, "App Main thread", App_Main_Thread_Entry, 0, pointer, 2 * DEFAULT_MEMORY_SIZE,
                        DEFAULT_PRIORITY, DEFAULT_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* Allocate the memory for Link thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,2 *  DEFAULT_MEMORY_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* create the Link thread */
  ret = tx_thread_create(&AppLinkThread, "App Link Thread", App_Link_Thread_Entry, 0, pointer, 2 * DEFAULT_MEMORY_SIZE,
                         LINK_PRIORITY, LINK_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* create the DHCP client */
  ret = nx_dhcp_create(&DHCPClient, &IpInstance, "DHCP Client");

  if (ret != NX_SUCCESS)
  {
    return NX_DHCP_ERROR;
  }

  /* Create a semaphore to be used to notify the main thread when the IP address is resolved*/
  tx_semaphore_create(&DHCPSemaphore, "DHCP Semaphore", 0);

  /* USER CODE END MX_NetXDuo_Init */

  return ret;
}

/* USER CODE BEGIN 1 */

/**
* @brief  ip address change callback.
* @param ip_instance: NX_IP instance
* @param ptr: user data
* @retval none
*/
static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr)
{
  /* release the semaphore as soon as an IP address is available */
  tx_semaphore_put(&DHCPSemaphore);
}

/**
* @brief  Main thread entry.
* @param thread_input: ULONG user argument used by the thread entry
* @retval none
*/
static VOID App_Main_Thread_Entry(ULONG thread_input)
{
  UINT ret;

  /* register the IP address change callback */
  ret = nx_ip_address_change_notify(&IpInstance, ip_address_change_notify_callback, NULL);

  if (ret != NX_SUCCESS)
  {
    /* USER CODE BEGIN IP address change callback error */
    Error_Handler();
    /* USER CODE END IP address change callback error */
  }

  /* start the DHCP client */
  ret = nx_dhcp_start(&DHCPClient);

  if (ret != NX_SUCCESS)
  {
    /* USER CODE BEGIN DHCP client start error */
    Error_Handler();
    /* USER CODE END DHCP client start error */
  }

  /* wait until an IP address is ready */
  if(tx_semaphore_get(&DHCPSemaphore, TX_WAIT_FOREVER) != TX_SUCCESS)
  {
    /* USER CODE BEGIN DHCPSemaphore get error */
    Error_Handler();
    /* USER CODE END DHCPSemaphore get error */
  }
  /* get IP address */
  ret = nx_ip_address_get(&IpInstance, &IpAddress, &NetMask);

  PRINT_IP_ADDRESS(IpAddress);

  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }
  {
    char IP_addr_str[140];
    sprintf(IP_addr_str, "RTSP server address : %lu.%lu.%lu.%lu",
                                                (IpAddress >> 24) & 0xff,
                                                (IpAddress >> 16) & 0xff,
                                                (IpAddress >> 8) & 0xff,
                                                IpAddress& 0xff);
    UTIL_LCD_SetLayer(0);
    UTIL_LCD_SetDevice(0);
    UTIL_LCD_SetFont(&Font24);
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
    UTIL_LCD_DisplayStringAt(0, 240, IP_addr_str, CENTER_MODE);
  }
  /* the network is correctly initialized, start the sample */
  sample_entry(&IpInstance, &AppPool, NX_NULL, NX_NULL);

  /* this thread is not needed any more, relinquish it */
  tx_thread_relinquish();

  return;
}

VOID MX_NetXDuo_Notify_Video_Frame_Ready(VOID)
{
  tx_event_flags_set(&demo_test_events, DEMO_VIDEO_DATA_READY_EVENT, TX_OR);
}

/**
* @brief  Link thread entry
* @param thread_input: ULONG thread parameter
* @retval none
*/
static VOID App_Link_Thread_Entry(ULONG thread_input)
{
  ULONG actual_status;
  UINT linkdown = 0, status;

  while(1)
  {
    /* Get Physical Link status. */
    status = nx_ip_interface_status_check(&IpInstance, 0, NX_IP_LINK_ENABLED,
                                      &actual_status, 50);

    if(status == NX_SUCCESS)
    {
      if(linkdown == 1)
      {
        linkdown = 0;
        status = nx_ip_interface_status_check(&IpInstance, 0, NX_IP_ADDRESS_RESOLVED,
                                      &actual_status, 50);
        if(status == NX_SUCCESS)
        {
          /* The network cable is connected again. */
          printf("The network cable is connected again.\n");
        }
        else
        {
          /* The network cable is connected. */
          printf("The network cable is connected.\n");
          /* Send command to Enable Nx driver. */
          nx_ip_driver_direct_command(&IpInstance, NX_LINK_ENABLE,
                                      &actual_status);
          /* Restart DHCP Client. */
          nx_dhcp_stop(&DHCPClient);
          nx_dhcp_start(&DHCPClient);
        }
      }
    }
    else
    {
      if(0 == linkdown)
      {
        linkdown = 1;
        /* The network cable is not connected. */
        printf("The network cable is not connected.\n");
      }
    }

    tx_thread_sleep(NX_APP_CABLE_CONNECTION_CHECK_PERIOD);
  }
}

/* USER CODE END 1 */
