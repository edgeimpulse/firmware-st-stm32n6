/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/* Indicate that driver source is being compiled.  */

#define NX_DRIVER_SOURCE

/****** DRIVER SPECIFIC ****** Start of part/vendor specific include area.  Include driver-specific include file here!  */

#ifndef NX_STM32_ETH_DRIVER_H

/* Determine if the driver uses IP deferred processing or direct ISR processing.  */

#define NX_DRIVER_ENABLE_DEFERRED                /* Define this to enable deferred ISR processing.  */

/* Include driver specific include file.  */
#include "nx_stm32_eth_driver.h"

/* Include the phy driver header */
#include "nx_stm32_phy_driver.h"

#endif /* NX_STM32_ETH_DRIVER_H */

/****** DRIVER SPECIFIC ****** End of part/vendor specific include file area!  */


/* Define the driver information structure that is only available within this file.  */
/* Place Ethernet BD at uncacheable memory*/
static  NX_DRIVER_INFORMATION nx_driver_information;

/* Rounded header size */
static ULONG header_size;

ETH_TxPacketConfigTypeDef  TxPacketCfg;
ETH_MACFilterConfigTypeDef FilterConfig;
USHORT                     packet_type;

#ifdef NX_DRIVER_ENABLE_PTP
TIMESTAMP_CALLBACK timestamp_callback = NULL;
#define SUBSECOND_INCREMENT 20 << 16
#define HAL_PTP_TIMEOUT      0xFU
#define PTP_REF_CLK 50000000UL
static ETH_PTP_ConfigTypeDef ptpconfig = {0};
#endif
/****** DRIVER SPECIFIC ****** Start of part/vendor specific data area.  Include hardware-specific data here!  */

/****** DRIVER SPECIFIC ****** End of part/vendor specific data area!  */


/* Define the routines for processing each driver entry request.  The contents of these routines will change with
each driver. However, the main driver entry function will not change, except for the entry function name.  */

static VOID         _nx_driver_interface_attach(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_initialize(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_enable(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_disable(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_packet_send(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_multicast_join(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_multicast_leave(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_get_status(NX_IP_DRIVER *driver_req_ptr);
#ifdef NX_ENABLE_INTERFACE_CAPABILITY
static VOID         _nx_driver_capability_get(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_capability_set(NX_IP_DRIVER *driver_req_ptr);
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

static VOID         _nx_driver_deferred_processing(NX_IP_DRIVER *driver_req_ptr);

#ifdef NX_DRIVER_ENABLE_PTP
static VOID         _nx_driver_transfer_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr, ULONG *ptp_ts_ptr);
#else
static VOID         _nx_driver_transfer_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr);
#endif /* NX_DRIVER_ENABLE_PTP */

#ifdef NX_DRIVER_INTERNAL_TRANSMIT_QUEUE
static VOID         _nx_driver_transmit_packet_enqueue(NX_PACKET *packet_ptr);
static NX_PACKET    *_nx_driver_transmit_packet_dequeue(VOID);
#endif /* NX_DRIVER_INTERNAL_TRANSMIT_QUEUE */

/* Define the prototypes for the hardware implementation of this driver. The contents of these routines are
driver-specific.  */

static UINT         _nx_driver_hardware_initialize(NX_IP_DRIVER *driver_req_ptr);
static UINT         _nx_driver_hardware_enable(NX_IP_DRIVER *driver_req_ptr);
static UINT         _nx_driver_hardware_disable(NX_IP_DRIVER *driver_req_ptr);
static UINT         _nx_driver_hardware_packet_send(NX_PACKET *packet_ptr);
#ifdef MULTI_QUEUE_FEATURE
static UINT         _nx_driver_hardware_packet_send_distribute(NX_PACKET *packet_ptr, UINT queue_number);
#endif
static UINT         _nx_driver_hardware_multicast_join(NX_IP_DRIVER *driver_req_ptr);
static UINT         _nx_driver_hardware_multicast_leave(NX_IP_DRIVER *driver_req_ptr);
static UINT         _nx_driver_hardware_get_status(NX_IP_DRIVER *driver_req_ptr);
static VOID         _nx_driver_hardware_packet_received(VOID);
#ifdef NX_ENABLE_INTERFACE_CAPABILITY
static UINT         _nx_driver_hardware_capability_set(NX_IP_DRIVER *driver_req_ptr);
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    nx_stm32_eth_driver                                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This is the entry point of the NetX Ethernet Driver. This driver    */
/*    function is responsible for initializing the Ethernet controller,   */
/*    enabling or disabling the controller as need, preparing             */
/*    a packet for transmission, and getting status information.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        The driver request from the   */
/*                                            IP layer.                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_interface_attach           Process attach request        */
/*    _nx_driver_initialize                 Process initialize request    */
/*    _nx_driver_enable                     Process link enable request   */
/*    _nx_driver_disable                    Process link disable request  */
/*    _nx_driver_packet_send                Process send packet requests  */
/*    _nx_driver_multicast_join             Process multicast join request*/
/*    _nx_driver_multicast_leave            Process multicast leave req   */
/*    _nx_driver_get_status                 Process get status request    */
/*    _nx_driver_deferred_processing        Drive deferred processing     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    IP layer                                                            */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
/****** DRIVER SPECIFIC ****** Start of part/vendor specific global driver entry function name.  */
VOID  nx_stm32_eth_driver(NX_IP_DRIVER *driver_req_ptr)
/****** DRIVER SPECIFIC ****** End of part/vendor specific global driver entry function name.  */
{

#ifdef NX_ENABLE_VLAN
NX_INTERFACE *interface_ptr;
#endif /* NX_ENABLE_VLAN */

  /* Default to successful return.  */
  driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;

#ifdef NX_ENABLE_VLAN
    /* Let link layer to preprocess the driver request and return actual interface.  */
  if (nx_link_driver_request_preprocess(driver_req_ptr, &interface_ptr) != NX_SUCCESS)
  {
      driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
      return;
  }
#endif /* NX_ENABLE_VLAN */

  /* Process according to the driver request type in the IP control
  block.  */
  switch (driver_req_ptr -> nx_ip_driver_command)
  {

  case NX_LINK_INTERFACE_ATTACH:

    /* Process link interface attach requests.  */
    _nx_driver_interface_attach(driver_req_ptr);
    break;

  case NX_LINK_INITIALIZE:
    {

      /* Process link initialize requests.  */
      _nx_driver_initialize(driver_req_ptr);
      break;
    }

  case NX_LINK_ENABLE:
    {

      /* Process link enable requests.  */
      _nx_driver_enable(driver_req_ptr);
      break;
    }

  case NX_LINK_DISABLE:
    {

      /* Process link disable requests.  */
      _nx_driver_disable(driver_req_ptr);
      break;
    }


  case NX_LINK_ARP_SEND:
  case NX_LINK_ARP_RESPONSE_SEND:
  case NX_LINK_PACKET_BROADCAST:
  case NX_LINK_RARP_SEND:
  case NX_LINK_PACKET_SEND:
  case NX_LINK_RAW_PACKET_SEND:
    {

      /* Process packet send requests.  */
      _nx_driver_packet_send(driver_req_ptr);
      break;
    }


  case NX_LINK_MULTICAST_JOIN:
    {

      /* Process multicast join requests.  */
      _nx_driver_multicast_join(driver_req_ptr);
      break;
    }


  case NX_LINK_MULTICAST_LEAVE:
    {

      /* Process multicast leave requests.  */
      _nx_driver_multicast_leave(driver_req_ptr);
      break;
    }

  case NX_LINK_GET_STATUS:
    {

      /* Process get status requests.  */
      _nx_driver_get_status(driver_req_ptr);
      break;
    }

  case NX_LINK_DEFERRED_PROCESSING:
    {

      /* Process driver deferred requests.  */

      /* Process a device driver function on behave of the IP thread. */
      _nx_driver_deferred_processing(driver_req_ptr);

      break;
    }


#ifdef NX_ENABLE_INTERFACE_CAPABILITY
  case NX_INTERFACE_CAPABILITY_GET:
    {

      /* Process get capability requests.  */
      _nx_driver_capability_get(driver_req_ptr);
      break;
    }

  case NX_INTERFACE_CAPABILITY_SET:
    {

      /* Process set capability requests.  */
      _nx_driver_capability_set(driver_req_ptr);
      break;
    }
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

  default:


    /* Invalid driver request.  */

    /* Return the unhandled command status.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_UNHANDLED_COMMAND;

  }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_interface_attach                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the interface attach request.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_interface_attach(NX_IP_DRIVER *driver_req_ptr)
{


  /* Setup the driver's interface.  This example is for a simple one-interface
  Ethernet driver. Additional logic is necessary for multiple port devices.  */
  nx_driver_information.nx_driver_information_interface =  driver_req_ptr -> nx_ip_driver_interface;

#ifdef NX_ENABLE_INTERFACE_CAPABILITY
  driver_req_ptr -> nx_ip_driver_interface -> nx_interface_capability_flag = NX_DRIVER_CAPABILITY;
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

  /* Return successful status.  */
  driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_initialize                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the initialize request.  The processing    */
/*    in this function is generic. All ethernet controller logic is to    */
/*    be placed in _nx_driver_hardware_initialize.                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_initialize        Process initialize request    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_initialize(NX_IP_DRIVER *driver_req_ptr)
{

  NX_IP           *ip_ptr;
  NX_INTERFACE    *interface_ptr;
  UINT            status;
  CHAR           *payload_address;       /* Address of the first payload*/
  VOID           *rounded_pool_start;    /* Rounded stating address     */

  /* Setup the IP pointer from the driver request.  */
  ip_ptr =  driver_req_ptr -> nx_ip_driver_ptr;

  /* Setup interface pointer.  */
  interface_ptr = driver_req_ptr -> nx_ip_driver_interface;

  /* Initialize the driver's information structure.  */

  /* Default IP pointer to NULL.  */
  nx_driver_information.nx_driver_information_ip_ptr =               NX_NULL;

  /* Setup the driver state to not initialized.  */
  nx_driver_information.nx_driver_information_state =                NX_DRIVER_STATE_NOT_INITIALIZED;

  /* Setup the default packet pool for the driver's received packets.  */
  nx_driver_information.nx_driver_information_packet_pool_ptr = ip_ptr -> nx_ip_default_packet_pool;

  /* Get the rounded start pool start. */
  rounded_pool_start = nx_driver_information.nx_driver_information_packet_pool_ptr->nx_packet_pool_start;

  /* Calculate the address of payload. */
  payload_address = (CHAR *)((ALIGN_TYPE)rounded_pool_start + sizeof(NX_PACKET));

  /* Align the address of payload. */
  payload_address = (CHAR *)((((ALIGN_TYPE)payload_address + NX_PACKET_ALIGNMENT  - 1) / NX_PACKET_ALIGNMENT) * NX_PACKET_ALIGNMENT);

  /* Calculate the header size. */
  header_size = (ULONG)((ALIGN_TYPE)payload_address - (ALIGN_TYPE)rounded_pool_start);

  /* Clear the deferred events for the driver.  */
  nx_driver_information.nx_driver_information_deferred_events =       0;

#ifdef NX_DRIVER_INTERNAL_TRANSMIT_QUEUE

  /* Clear the transmit queue count and head pointer.  */
  nx_driver_information.nx_driver_transmit_packets_queued =  0;
  nx_driver_information.nx_driver_transmit_queue_head =      NX_NULL;
  nx_driver_information.nx_driver_transmit_queue_tail =      NX_NULL;
#endif /* NX_DRIVER_INTERNAL_TRANSMIT_QUEUE */
  /* Call the hardware-specific ethernet controller initialization.  */
  status =  _nx_driver_hardware_initialize(driver_req_ptr);

  /* Determine if the request was successful.  */
  if (status == NX_SUCCESS)
  {

    /* Successful hardware initialization.  */

    /* Setup driver information to point to IP pointer.  */
    nx_driver_information.nx_driver_information_ip_ptr = driver_req_ptr -> nx_ip_driver_ptr;

    /* Setup the link maximum transfer unit. */
    interface_ptr -> nx_interface_ip_mtu_size =  NX_DRIVER_ETHERNET_MTU - NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Setup the physical address of this IP instance.  Increment the
    physical address lsw to simulate multiple nodes hanging on the
    ethernet.  */
    interface_ptr -> nx_interface_physical_address_msw =
      (ULONG)(( eth_handle.Init.MACAddr[0] << 8) | ( eth_handle.Init.MACAddr[1]));
    interface_ptr -> nx_interface_physical_address_lsw =
      (ULONG)(( eth_handle.Init.MACAddr[2] << 24) | ( eth_handle.Init.MACAddr[3] << 16) |
              ( eth_handle.Init.MACAddr[4] << 8) | ( eth_handle.Init.MACAddr[5]));

    /* Indicate to the IP software that IP to physical mapping
    is required.  */
    interface_ptr -> nx_interface_address_mapping_needed =  NX_TRUE;

    /* Move the driver's state to initialized.  */
    nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_INITIALIZED;

    /* Indicate successful initialize.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
  }
  else
  {

    /* Initialization failed.  Indicate that the request failed.  */
    driver_req_ptr -> nx_ip_driver_status =   NX_DRIVER_ERROR;
  }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_enable                                                   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the initialize request. The processing     */
/*    in this function is generic. All ethernet controller logic is to    */
/*    be placed in _nx_driver_hardware_enable.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_enable            Process enable request        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_enable(NX_IP_DRIVER *driver_req_ptr)
{

  NX_IP           *ip_ptr;
  ETH_MACConfigTypeDef MACConf;
  UINT            status, duplex, speed = 0;
  INT             PHYLinkState;

  /* Setup the IP pointer from the driver request.  */
  ip_ptr =  driver_req_ptr -> nx_ip_driver_ptr;

  /* See if we can honor the NX_LINK_ENABLE request.  */
  if (nx_driver_information.nx_driver_information_state < NX_DRIVER_STATE_INITIALIZED)
  {

    /* Mark the request as not successful.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
    return;
  }

  /* Check if it is enabled by someone already */
  if (nx_driver_information.nx_driver_information_state >=  NX_DRIVER_STATE_LINK_ENABLED)
  {

    /* Yes, the request has already been made.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_ALREADY_ENABLED;
    return;
  }

  if (nx_eth_phy_init() != ETH_PHY_STATUS_OK)
  {
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
    return;
  }

  HAL_Delay(1500);

  PHYLinkState = nx_eth_phy_get_link_state();

  /* Get link state */
  if(PHYLinkState <= ETH_PHY_STATUS_LINK_DOWN)
  {
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
    return;
  }
  else
  {
    switch (PHYLinkState)
    {
#if defined(ETH_PHY_1000MBITS_SUPPORTED)
    case ETH_PHY_STATUS_1000MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_1000M;
      break;
    case ETH_PHY_STATUS_1000MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_1000M;
      break;
#endif
case ETH_PHY_STATUS_100MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    case ETH_PHY_STATUS_100MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    case ETH_PHY_STATUS_10MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      break;
    case ETH_PHY_STATUS_10MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      break;
    default:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    }

    /* Get MAC Config MAC */
    HAL_ETH_GetMACConfig(&eth_handle, &MACConf);
    MACConf.DuplexMode = duplex;
    MACConf.Speed = speed;
if (speed == ETH_SPEED_1000M)
    MACConf.PortSelect = DISABLE;
else
    MACConf.PortSelect = ENABLE;

    HAL_ETH_SetMACConfig(&eth_handle, &MACConf);
  }

  /* Call hardware specific enable.  */
  status =  _nx_driver_hardware_enable(driver_req_ptr);

  /* Was the hardware enable successful?  */
  if (status == NX_SUCCESS)
  {

    /* Update the driver state to link enabled.  */
    nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_LINK_ENABLED;

    /* Mark request as successful.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;

    /* Mark the IP instance as link up.  */
    ip_ptr -> nx_ip_driver_link_up =  NX_TRUE;
  }
  else
  {

    /* Enable failed.  Indicate that the request failed.  */
    driver_req_ptr -> nx_ip_driver_status =   NX_DRIVER_ERROR;
  }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_disable                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the disable request. The processing        */
/*    in this function is generic. All ethernet controller logic is to    */
/*    be placed in _nx_driver_hardware_disable.                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_disable           Process disable request       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_disable(NX_IP_DRIVER *driver_req_ptr)
{

  NX_IP           *ip_ptr;
  UINT            status;


  /* Setup the IP pointer from the driver request.  */
  ip_ptr =  driver_req_ptr -> nx_ip_driver_ptr;

  /* Check if the link is enabled.  */
  if (nx_driver_information.nx_driver_information_state !=  NX_DRIVER_STATE_LINK_ENABLED)
  {

    /* The link is not enabled, so just return an error.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
    return;
  }

  /* Call hardware specific disable.  */
  status =  _nx_driver_hardware_disable(driver_req_ptr);

  /* Was the hardware disable successful?  */
  if (status == NX_SUCCESS)
  {

    /* Mark the IP instance as link down.  */
    ip_ptr -> nx_ip_driver_link_up =  NX_FALSE;

    /* Update the driver state back to initialized.  */
    nx_driver_information.nx_driver_information_state =  NX_DRIVER_STATE_INITIALIZED;

    /* Mark request as successful.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
  }
  else
  {

    /* Disable failed, return an error.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
  }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_packet_send                                              */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the packet send request. The processing    */
/*    in this function is generic. All ethernet controller packet send    */
/*    logic is to be placed in _nx_driver_hardware_packet_send.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_packet_send       Process packet send request   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
#ifdef MULTI_QUEUE_FEATURE
static VOID  _nx_driver_packet_send(NX_IP_DRIVER *driver_req_ptr)
{

NX_PACKET *packet_ptr;
UINT       status;
USHORT     ether_type;
NX_INTERFACE
          *interface_ptr;

    /* Check to make sure the link is up.  */
    if (nx_driver_information.nx_driver_information_state != NX_DRIVER_STATE_LINK_ENABLED)
    {

        /* Inidate an unsuccessful packet send.  */
        driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;

        /* Link is not up, simply free the packet.  */
        nx_packet_transmit_release(driver_req_ptr -> nx_ip_driver_packet);
        return;
    }

    interface_ptr = driver_req_ptr -> nx_ip_driver_interface;

    /* Process driver send packet.  */

    /* Place the ethernet frame at the front of the packet.  */
    packet_ptr =  driver_req_ptr -> nx_ip_driver_packet;

    if (driver_req_ptr -> nx_ip_driver_command != NX_LINK_RAW_PACKET_SEND)
    {

        /* Get Ethernet type.  */
        switch (driver_req_ptr -> nx_ip_driver_command)
        {
        case NX_LINK_ARP_SEND:
        case NX_LINK_ARP_RESPONSE_SEND:
        {
            ether_type = NX_DRIVER_ETHERNET_ARP;
            break;
        }
        case NX_LINK_RARP_SEND:
        {
            ether_type = NX_DRIVER_ETHERNET_RARP;
            break;
        }
        default:
        {
            if (packet_ptr -> nx_packet_ip_version == 4)
            {
                ether_type = NX_DRIVER_ETHERNET_IP;
            }
            else
            {
                ether_type = NX_DRIVER_ETHERNET_IPV6;
            }
            break;
        }
        }

        /* Add Ethernet header.  */
        if (nx_link_ethernet_header_add(nx_driver_information.nx_driver_information_ip_ptr,
                                        interface_ptr -> nx_interface_index,
                                        packet_ptr,
                                        driver_req_ptr -> nx_ip_driver_physical_address_msw,
                                        driver_req_ptr -> nx_ip_driver_physical_address_lsw,
                                        ether_type))
        {

            /* Release the packet.  */
            nx_packet_transmit_release(packet_ptr);
            return;
        }
    }

    /* Transmit the packet through the Ethernet controller low level access routine. */
    status = _nx_driver_hardware_packet_send(packet_ptr);

    /* Determine if there was an error.  */
    if (status != NX_SUCCESS)
    {

        /* Driver's hardware send packet routine failed to send the packet.  */

        /* Remove the Ethernet header.  */
        NX_DRIVER_ETHERNET_HEADER_REMOVE(packet_ptr);

        /* Indicate an unsuccessful packet send.  */
        driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;

        /* Link is not up, simply free the packet.  */
        nx_packet_transmit_release(packet_ptr);
    }
    else
    {

        /* Set the status of the request.  */
        driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
    }
}
#else
static VOID  _nx_driver_packet_send(NX_IP_DRIVER *driver_req_ptr)
{

  NX_IP           *ip_ptr;
  NX_PACKET       *packet_ptr;
  ULONG           *ethernet_frame_ptr;
  UINT            status;


  /* Setup the IP pointer from the driver request.  */
  ip_ptr =  driver_req_ptr -> nx_ip_driver_ptr;

  /* Check to make sure the link is up.  */
  if (nx_driver_information.nx_driver_information_state != NX_DRIVER_STATE_LINK_ENABLED)
  {

    /* Inidate an unsuccessful packet send.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;

    /* Link is not up, simply free the packet.  */
    nx_packet_transmit_release(driver_req_ptr -> nx_ip_driver_packet);
    return;
  }

  /* Process driver send packet.  */

  /* Place the ethernet frame at the front of the packet.  */
  packet_ptr =  driver_req_ptr -> nx_ip_driver_packet;

  /* Adjust the prepend pointer.  */
  packet_ptr -> nx_packet_prepend_ptr =
    packet_ptr -> nx_packet_prepend_ptr - NX_DRIVER_ETHERNET_FRAME_SIZE;

  /* Adjust the packet length.  */
  packet_ptr -> nx_packet_length = packet_ptr -> nx_packet_length + NX_DRIVER_ETHERNET_FRAME_SIZE;

  /* Setup the ethernet frame pointer to build the ethernet frame.  Backup another 2
  * bytes to get 32-bit word alignment.  */
  ethernet_frame_ptr =  (ULONG *) (packet_ptr -> nx_packet_prepend_ptr - 2);

  /* Set up the hardware addresses in the Ethernet header. */
  *ethernet_frame_ptr       =  driver_req_ptr -> nx_ip_driver_physical_address_msw;
  *(ethernet_frame_ptr + 1) =  driver_req_ptr -> nx_ip_driver_physical_address_lsw;

  *(ethernet_frame_ptr + 2) =  (ip_ptr -> nx_ip_arp_physical_address_msw << 16) |
    (ip_ptr -> nx_ip_arp_physical_address_lsw >> 16);
  *(ethernet_frame_ptr + 3) =  (ip_ptr -> nx_ip_arp_physical_address_lsw << 16);

  /* Set up the frame type field in the Ethernet harder. */
  if ((driver_req_ptr -> nx_ip_driver_command == NX_LINK_ARP_SEND)||
      (driver_req_ptr -> nx_ip_driver_command == NX_LINK_ARP_RESPONSE_SEND))
  {

    *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_ARP;
  }
  else if(driver_req_ptr -> nx_ip_driver_command == NX_LINK_RARP_SEND)
  {

    *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_RARP;
  }

#ifdef FEATURE_NX_IPV6
  else if(packet_ptr -> nx_packet_ip_version == NX_IP_VERSION_V6)
  {

    *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_IPV6;
  }
#endif /* FEATURE_NX_IPV6 */

  else
  {

    *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_IP;
  }

  /* Endian swapping if NX_LITTLE_ENDIAN is defined.  */
  NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr));
  NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 1));
  NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 2));
  NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 3));

  /* Determine if the packet exceeds the driver's MTU.  */
  if (packet_ptr -> nx_packet_length > NX_DRIVER_ETHERNET_MTU)
  {

    /* This packet exceeds the size of the driver's MTU. Simply throw it away! */

    /* Remove the Ethernet header.  */
    NX_DRIVER_ETHERNET_HEADER_REMOVE(packet_ptr);

    /* Indicate an unsuccessful packet send.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;

    /* Link is not up, simply free the packet.  */
    nx_packet_transmit_release(packet_ptr);
    return;
  }

  /* Transmit the packet through the Ethernet controller low level access routine. */
  status = _nx_driver_hardware_packet_send(packet_ptr);

  /* Determine if there was an error.  */
  if (status != NX_SUCCESS)
  {

    /* Driver's hardware send packet routine failed to send the packet.  */

    /* Remove the Ethernet header.  */
    NX_DRIVER_ETHERNET_HEADER_REMOVE(packet_ptr);

    /* Indicate an unsuccessful packet send.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;

    /* Link is not up, simply free the packet.  */
    nx_packet_transmit_release(packet_ptr);
  }
  else
  {

    /* Set the status of the request.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
  }
}
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_multicast_join                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the multicast join request. The processing */
/*    in this function is generic. All ethernet controller multicast join */
/*    logic is to be placed in _nx_driver_hardware_multicast_join.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_multicast_join    Process multicast join request*/
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_multicast_join(NX_IP_DRIVER *driver_req_ptr)
{

  UINT        status;


  /* Call hardware specific multicast join function. */
  status =  _nx_driver_hardware_multicast_join(driver_req_ptr);

  /* Determine if there was an error.  */
  if (status != NX_SUCCESS)
  {

    /* Indicate an unsuccessful request.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
  }
  else
  {

    /* Indicate the request was successful.   */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
  }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_multicast_leave                                          */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the multicast leave request. The           */
/*    processing in this function is generic. All ethernet controller     */
/*    multicast leave logic is to be placed in                            */
/*    _nx_driver_hardware_multicast_leave.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_multicast_leave   Process multicast leave req   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_multicast_leave(NX_IP_DRIVER *driver_req_ptr)
{

  UINT        status;


  /* Call hardware specific multicast leave function. */
  status =  _nx_driver_hardware_multicast_leave(driver_req_ptr);

  /* Determine if there was an error.  */
  if (status != NX_SUCCESS)
  {

    /* Indicate an unsuccessful request.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
  }
  else
  {

    /* Indicate the request was successful.   */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
  }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_get_status                                               */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the get status request. The processing     */
/*    in this function is generic. All ethernet controller get status     */
/*    logic is to be placed in _nx_driver_hardware_get_status.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_hardware_get_status        Process get status request    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_get_status(NX_IP_DRIVER *driver_req_ptr)
{

  UINT        status;


  /* Call hardware specific get status function. */
  status =  _nx_driver_hardware_get_status(driver_req_ptr);

  /* Determine if there was an error.  */
  if (status != NX_SUCCESS)
  {

    /* Indicate an unsuccessful request.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
  }
  else
  {

    /* Indicate the request was successful.   */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
  }
}


#ifdef NX_ENABLE_INTERFACE_CAPABILITY
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_capability_get                                           */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the get capability request.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_capability_get(NX_IP_DRIVER *driver_req_ptr)
{

  /* Return the capability of the Ethernet controller.  */
  *(driver_req_ptr -> nx_ip_driver_return_ptr) = NX_DRIVER_CAPABILITY;

  /* Return the success status.  */
  driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_capability_set                                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the set capability request.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_capability_set(NX_IP_DRIVER *driver_req_ptr)
{

  UINT        status;


  /* Call hardware specific get status function. */
  status =  _nx_driver_hardware_capability_set(driver_req_ptr);

  /* Determine if there was an error.  */
  if (status != NX_SUCCESS)
  {

    /* Indicate an unsuccessful request.  */
    driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
  }
  else
  {

    /* Indicate the request was successful.   */
    driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
  }
}
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_deferred_processing                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    XC, Microsoft Corporation                                           */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing the deferred ISR action within the context */
/*    of the IP thread.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver command from the IP    */
/*                                            thread                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_driver_packet_transmitted        Clean up after transmission    */
/*    _nx_driver_packet_received           Process a received packet      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Driver entry function                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID  _nx_driver_deferred_processing(NX_IP_DRIVER *driver_req_ptr)
{

  TX_INTERRUPT_SAVE_AREA

  ULONG       deferred_events;

  /* Disable interrupts.  */
  TX_DISABLE

  /* Pickup deferred events.  */
  deferred_events =  nx_driver_information.nx_driver_information_deferred_events;
  nx_driver_information.nx_driver_information_deferred_events =  0;

  /* Restore interrupts.  */
  TX_RESTORE

#ifdef MULTI_QUEUE_FEATURE
    if(deferred_events & NX_DRIVER_DEFERRED_PACKET_TRANSMITTED_CH0)
    {
      if (eth_handle.TxDescList[ETH_DMA_CH0].BuffersInUse >= (NX_DRIVER_TX_DESCRIPTORS / 2))
      {
        nx_driver_information.nx_driver_information_number_of_transmit_buffers_in_use[ETH_DMA_CH0] = 0;
        eth_handle.TxCH = ETH_DMA_CH0;
        HAL_ETH_ReleaseTxPacket(&eth_handle);
      }
    }

  if(deferred_events & NX_DRIVER_DEFERRED_PACKET_TRANSMITTED_CH1)
  {
    if (eth_handle.TxDescList[ETH_DMA_CH1].BuffersInUse >= (NX_DRIVER_TX_DESCRIPTORS / 2))
    {
      nx_driver_information.nx_driver_information_number_of_transmit_buffers_in_use[ETH_DMA_CH1] = 0;
      eth_handle.TxCH = ETH_DMA_CH1;
      HAL_ETH_ReleaseTxPacket(&eth_handle);
    }
  }

  if(deferred_events & NX_DRIVER_DEFERRED_PACKET_RECEIVED_CH0)
  {
    eth_handle.RxCH = ETH_DMA_CH0;
    _nx_driver_hardware_packet_received();
  }

  if(deferred_events & NX_DRIVER_DEFERRED_PACKET_RECEIVED_CH1)
  {
    eth_handle.RxCH = ETH_DMA_CH1;
    _nx_driver_hardware_packet_received();
  }

#else
  /* Check for a transmit complete event.  */
  if(deferred_events & NX_DRIVER_DEFERRED_PACKET_TRANSMITTED)
  {

    /* Process transmitted packet(s).  */
    HAL_ETH_ReleaseTxPacket(&eth_handle);
  }
  /* Check for received packet.  */
  if(deferred_events & NX_DRIVER_DEFERRED_PACKET_RECEIVED)
  {

    /* Process received packet(s).  */
    _nx_driver_hardware_packet_received();
  }
#endif
  /* Mark request as successful.  */
  driver_req_ptr->nx_ip_driver_status =  NX_SUCCESS;
}

#ifdef NX_DRIVER_ENABLE_PTP
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_transfer_to_netx                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing incoming packets.  This routine would      */
/*    be called from the driver-specific receive packet processing        */
/*    function _nx_driver_hardware.                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    ip_ptr                                Pointer to IP protocol block  */
/*    packet_ptr                            Packet pointer                */
/*    ptp_ts_ptr                            PTP timestamp pointer         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Error indication                                                    */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_ip_packet_receive                 NetX IP packet receive        */
/*    _nx_ip_packet_deferred_receive        NetX IP packet receive        */
/*    _nx_arp_packet_deferred_receive       NetX ARP packet receive       */
/*    _nx_rarp_packet_deferred_receive      NetX RARP packet receive      */
/*    _nx_packet_release                    Release packet                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_hardware_packet_received   Driver packet receive function*/
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_transfer_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr, ULONG *ptp_ts_ptr)
{
NX_LINK_TIME link_time;

    link_time.nano_second = ptp_ts_ptr[0];
    link_time.second_high = 0;
    link_time.second_low = ptp_ts_ptr[1];

#ifdef DEMO_TSN_LISTENER
TIMESTAMP_INFO *ts_info_ptr;
NX_PTP_TIME rx_ts;
TIMESTAMP_INFO rx_ts_info;
UINT type =( *(packet_ptr->nx_packet_prepend_ptr + 16) << 8) | (*(packet_ptr->nx_packet_prepend_ptr + 17));

    if((timestamp_callback) &&(type == packet_type))
    {

        rx_ts.second_high = link_time.second_high;
        rx_ts.second_low = link_time.second_low;
        rx_ts.nanosecond = link_time.nano_second;
        timestamp_callback(packet_ptr, TIME_STAMP_TYPE_PACKET_RX, &rx_ts);
#ifdef OPCUA_RAW_PACKET
        ts_info_ptr = (TIMESTAMP_INFO *)(packet_ptr->nx_packet_prepend_ptr + 18);
        memcpy(&rx_ts_info, ts_info_ptr, sizeof(rx_ts_info));
        rx_ts_info.ts_gen.nanosecond = NX_CHANGE_ULONG_ENDIAN(rx_ts_info.ts_gen.nanosecond);
        rx_ts_info.ts_gen.second_low = NX_CHANGE_ULONG_ENDIAN(rx_ts_info.ts_gen.second_low);
        rx_ts_info.ts_gen.second_high = NX_CHANGE_ULONG_ENDIAN(rx_ts_info.ts_gen.second_high);

        rx_ts_info.ts_in_txbd.nanosecond = NX_CHANGE_ULONG_ENDIAN(rx_ts_info.ts_in_txbd.nanosecond);
        rx_ts_info.ts_in_txbd.second_low = NX_CHANGE_ULONG_ENDIAN(rx_ts_info.ts_in_txbd.second_low);
        rx_ts_info.ts_in_txbd.second_high = NX_CHANGE_ULONG_ENDIAN(rx_ts_info.ts_in_txbd.second_high);

        rx_ts_info.ts_tx_done.nanosecond = NX_CHANGE_ULONG_ENDIAN(rx_ts_info.ts_tx_done.nanosecond);
        rx_ts_info.ts_tx_done.second_low = NX_CHANGE_ULONG_ENDIAN(rx_ts_info.ts_tx_done.second_low);
        rx_ts_info.ts_tx_done.second_high = NX_CHANGE_ULONG_ENDIAN(rx_ts_info.ts_tx_done.second_high);

        rx_ts_info.ts_counter = NX_CHANGE_ULONG_ENDIAN(rx_ts_info.ts_counter);

        rx_timestamp(packet_ptr, &rx_ts_info);
#endif /* OPCUA_RAW_PACKET */
    }

#endif
    nx_link_ethernet_packet_received(ip_ptr,
                                     nx_driver_information.nx_driver_information_interface -> nx_interface_index,
                                     packet_ptr, &link_time);
}
#else
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_transfer_to_netx                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processing incoming packets.  This routine would      */
/*    be called from the driver-specific receive packet processing        */
/*    function _nx_driver_hardware.                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    ip_ptr                                Pointer to IP protocol block  */
/*    packet_ptr                            Packet pointer                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Error indication                                                    */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_ip_packet_receive                 NetX IP packet receive        */
/*    _nx_ip_packet_deferred_receive        NetX IP packet receive        */
/*    _nx_arp_packet_deferred_receive       NetX ARP packet receive       */
/*    _nx_rarp_packet_deferred_receive      NetX RARP packet receive      */
/*    _nx_packet_release                    Release packet                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_hardware_packet_received   Driver packet receive function*/
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_transfer_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr)
{


  /* Set the interface for the incoming packet.  */
  packet_ptr -> nx_packet_ip_interface = nx_driver_information.nx_driver_information_interface;

  /* Pickup the packet header to determine where the packet needs to be
  sent.  */
  packet_type =  (USHORT)(((UINT) (*(packet_ptr -> nx_packet_prepend_ptr+12))) << 8) |
    ((UINT) (*(packet_ptr -> nx_packet_prepend_ptr+13)));

  /* Route the incoming packet according to its ethernet type.  */
  if (packet_type == NX_DRIVER_ETHERNET_IP || packet_type == NX_DRIVER_ETHERNET_IPV6)
  {
    /* Note:  The length reported by some Ethernet hardware includes
    bytes after the packet as well as the Ethernet header.  In some
    cases, the actual packet length after the Ethernet header should
    be derived from the length in the IP header (lower 16 bits of
    the first 32-bit word).  */

    /* Clean off the Ethernet header.  */
    packet_ptr -> nx_packet_prepend_ptr += NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Adjust the packet length.  */
    packet_ptr -> nx_packet_length -= NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Route to the ip receive function.  */
    _nx_ip_packet_deferred_receive(ip_ptr, packet_ptr);
  }
  else if (packet_type == NX_DRIVER_ETHERNET_ARP)
  {

    /* Clean off the Ethernet header.  */
    packet_ptr -> nx_packet_prepend_ptr += NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Adjust the packet length.  */
    packet_ptr -> nx_packet_length -= NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Route to the ARP receive function.  */
    _nx_arp_packet_deferred_receive(ip_ptr, packet_ptr);
  }
  else if (packet_type == NX_DRIVER_ETHERNET_RARP)
  {

    /* Clean off the Ethernet header.  */
    packet_ptr -> nx_packet_prepend_ptr += NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Adjust the packet length.  */
    packet_ptr -> nx_packet_length -= NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Route to the RARP receive function.  */
    _nx_rarp_packet_deferred_receive(ip_ptr, packet_ptr);
  }
  else
  {
    /* Invalid ethernet header... release the packet.  */
    nx_packet_release(packet_ptr);
  }
}
#endif /* NX_DRIVER_ENABLE_PTP */

#ifdef NX_DRIVER_INTERNAL_TRANSMIT_QUEUE
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_transmit_packet_enqueue                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function queues a transmit packet when the hardware transmit   */
/*    queue does not have the resources (buffer descriptors, etc.) to     */
/*    send the packet.  The queue is maintained as a singularly linked-   */
/*    list with head and tail pointers. The maximum number of packets on  */
/*    the transmit queue is regulated by the constant                     */
/*    NX_DRIVER_MAX_TRANSMIT_QUEUE_DEPTH. When this number is exceeded,   */
/*    the oldest packet is discarded after the new packet is queued.      */
/*                                                                        */
/*    Note: that it is assumed further driver interrupts are locked out   */
/*    during the call to this driver utility.                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    packet_ptr                            Packet pointer                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_packet_transmit_release           Release packet                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_hardware_packet_send       Driver packet send function   */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static VOID _nx_driver_transmit_packet_enqueue(NX_PACKET *packet_ptr)
{

  /* Determine if there is anything on the queue.  */
  if (nx_driver_information.nx_driver_transmit_queue_tail)
  {

    /* Yes, something is on the transmit queue. Simply add the new packet to the
    tail.  */
    nx_driver_information.nx_driver_transmit_queue_tail -> nx_packet_queue_next  =  packet_ptr;

    /* Update the tail pointer.  */
    nx_driver_information.nx_driver_transmit_queue_tail =  packet_ptr;
  }
  else
  {

    /* First packet on the transmit queue.  */

    /* Setup head pointers.  */
    nx_driver_information.nx_driver_transmit_queue_head =  packet_ptr;
    nx_driver_information.nx_driver_transmit_queue_tail =  packet_ptr;

    /* Set the packet's next pointer to NULL.  */
    packet_ptr -> nx_packet_queue_next =  NX_NULL;
  }

  /* Increment the total packets queued.  */
  nx_driver_information.nx_driver_transmit_packets_queued++;

  /* Determine if the total packet queued exceeds the driver's maximum transmit
  queue depth.  */
  if (nx_driver_information.nx_driver_transmit_packets_queued > NX_DRIVER_MAX_TRANSMIT_QUEUE_DEPTH)
  {

    /* Yes, remove the head packet (oldest) packet in the transmit queue and release it.  */
    packet_ptr =  nx_driver_information.nx_driver_transmit_queue_head;

    /* Adjust the head pointer to the next packet.  */
    nx_driver_information.nx_driver_transmit_queue_head =  packet_ptr -> nx_packet_queue_next;

    /* Decrement the transmit packet queued count.  */
    nx_driver_information.nx_driver_transmit_packets_queued--;

    /* Remove the ethernet header.  */
    NX_DRIVER_ETHERNET_HEADER_REMOVE(packet_ptr);

    /* Release the packet.  */
    nx_packet_transmit_release(packet_ptr);
  }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_transmit_packet_dequeue                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function removes the oldest transmit packet when the hardware  */
/*    transmit queue has new resources (usually after a transmit complete */
/*    interrupt) to send the packet. If there are no packets in the       */
/*    transmit queue, a NULL is returned.                                 */
/*                                                                        */
/*    Note: that it is assumed further driver interrupts are locked out   */
/*    during the call to this driver utility.                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    packet_ptr                            Packet pointer                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_hardware_packet_send       Driver packet send function   */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static NX_PACKET *_nx_driver_transmit_packet_dequeue(VOID)
{

  NX_PACKET   *packet_ptr;


  /* Pickup the head pointer of the tranmit packet queue.  */
  packet_ptr =  nx_driver_information.nx_driver_transmit_queue_head;

  /* Determine if there is anything on the queue.  */
  if (packet_ptr)
  {

    /* Yes, something is on the transmit queue. Simply the packet from the head of the queue.  */

    /* Update the head pointer.  */
    nx_driver_information.nx_driver_transmit_queue_head =  packet_ptr -> nx_packet_queue_next;

    /* Clear the next pointer in the packet.  */
    packet_ptr -> nx_packet_queue_next =  NX_NULL;

    /* Decrement the transmit packet queued count.  */
    nx_driver_information.nx_driver_transmit_packets_queued--;
  }

  /* Return the packet pointer - NULL if there are no packets queued.  */
  return(packet_ptr);
}

#endif /* NX_DRIVER_INTERNAL_TRANSMIT_QUEUE */



/****** DRIVER SPECIFIC ****** Start of part/vendor specific internal driver functions.  */

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_initialize                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific initialization.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    ETH_BSP_Config                        Configure Ethernet            */
/*    ETH_MACAddressConfig                  Setup MAC address             */
/*    ETH_DMARxDescReceiveITConfig          Enable receive descriptors    */
/*    nx_packet_allocate                    Allocate receive packet(s)    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_initialize                 Driver initialize processing  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_initialize(NX_IP_DRIVER *driver_req_ptr)
{
#ifdef MULTI_QUEUE_FEATURE
  uint32_t ch = 0;
#endif

  /* Default to successful return.  */
  driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;

  /* Setup indices.  */
  nx_driver_information.nx_driver_information_receive_current_index = 0;
  nx_driver_information.nx_driver_information_transmit_current_index = 0;
  nx_driver_information.nx_driver_information_transmit_release_index = 0;

  /* Clear the number of buffers in use counter.  */
#ifdef MULTI_QUEUE_FEATURE
  for (ch = 0; ch < ETH_DMA_TX_CH_CNT; ch++)
  {
    nx_driver_information.nx_driver_information_number_of_transmit_buffers_in_use[ch] = 0;
  }
#else
  nx_driver_information.nx_driver_information_number_of_transmit_buffers_in_use = 0;
#endif

  /* Make sure there are receive packets... otherwise, return an error.  */
  if (nx_driver_information.nx_driver_information_packet_pool_ptr == NULL)
  {

    /* There must be receive packets. If not, return an error!  */
    return(NX_DRIVER_ERROR);
  }

#ifdef NX_DRIVER_ETH_HW_IP_INIT
  nx_eth_init();
#endif /* NX_DRIVER_ETH_HW_IP_INIT */

  ETH_DMAConfigTypeDef dmaDefaultConf;

#ifdef MULTI_QUEUE_FEATURE
  /*--------------- ETHERNET DMA registers default Configuration --------------*/
  /* Common DMA configuration */
  dmaDefaultConf.AddressAlignedBeats = ENABLE;
  dmaDefaultConf.AXIBLENMaxSize = ETH_BLEN_MAX_SIZE_4;
  dmaDefaultConf.BurstMode = ETH_BURSTLENGTH_FIXED;
  dmaDefaultConf.RxOSRLimit = ETH_RX_OSR_LIMIT_3;
  dmaDefaultConf.TxOSRLimit = ETH_TX_OSR_LIMIT_3;
  dmaDefaultConf.TransmitArbitrationAlgorithm = ETH_DMATXARBITRATION_FIXED_PRIO;
  dmaDefaultConf.TransmitPriority = DISABLE;

  for (ch = 0; ch < ETH_DMA_CH_CNT; ch++)
  {
    /* DMA CH configuration */
    dmaDefaultConf.DMACh[ch].FlushRxPacket = DISABLE;
    dmaDefaultConf.DMACh[ch].PBLx8Mode = DISABLE;
    dmaDefaultConf.DMACh[ch].RxDMABurstLength = ETH_RXDMABURSTLENGTH_32BEAT;
    dmaDefaultConf.DMACh[ch].SecondPacketOperate = DISABLE;
    dmaDefaultConf.DMACh[ch].TCPSegmentation = DISABLE;
    dmaDefaultConf.DMACh[ch].TxDMABurstLength = ETH_TXDMABURSTLENGTH_32BEAT;
    dmaDefaultConf.DMACh[ch].DescriptorSkipLength = ETH_DMA_DESC_SKIP_LENGTH_32;
    dmaDefaultConf.DMACh[ch].MaximumSegmentSize = 0x218U;
  }
#else
#if defined(ETH_DMASBMR_BLEN4) /* ETH AXI support*/
  dmaDefaultConf.DMAArbitration = ETH_DMAARBITRATION_TX;
#else
  dmaDefaultConf.DMAArbitration = ETH_DMAARBITRATION_RX1_TX1;
#endif
  dmaDefaultConf.AddressAlignedBeats = ENABLE;
  dmaDefaultConf.BurstMode = ETH_BURSTLENGTH_FIXED;
  dmaDefaultConf.TxDMABurstLength = ETH_TXDMABURSTLENGTH_32BEAT;
  dmaDefaultConf.RxDMABurstLength = ETH_RXDMABURSTLENGTH_32BEAT;
  dmaDefaultConf.FlushRxPacket = DISABLE;
#ifndef STM32_ETH_HAL_LEGACY
  dmaDefaultConf.PBLx8Mode = DISABLE;
#if defined(ETH_DMASBMR_BLEN4) /* ETH AXI support*/
  dmaDefaultConf.RxOSRLimit = ETH_RX_OSR_LIMIT_3;
  dmaDefaultConf.TxOSRLimit = ETH_TX_OSR_LIMIT_3;
  dmaDefaultConf.AXIBLENMaxSize = ETH_BLEN_MAX_SIZE_16;
#else
  dmaDefaultConf.RebuildINCRxBurst = DISABLE;
#endif
  dmaDefaultConf.SecondPacketOperate = ENABLE;
  dmaDefaultConf.TCPSegmentation = DISABLE;
  dmaDefaultConf.MaximumSegmentSize = 536;
#endif
#ifdef STM32_ETH_HAL_LEGACY
  dmaDefaultConf.DropTCPIPChecksumErrorFrame = ENABLE;
  dmaDefaultConf.ReceiveStoreForward =  DISABLE;
  dmaDefaultConf.TransmitStoreForward =  ENABLE;
  dmaDefaultConf.TransmitThresholdControl =  ENABLE;
  dmaDefaultConf.ForwardErrorFrames =  DISABLE;
  dmaDefaultConf.ReceiveThresholdControl =  DISABLE;
  dmaDefaultConf.SecondFrameOperate =  DISABLE;
  dmaDefaultConf.EnhancedDescriptorFormat =  DISABLE;
  dmaDefaultConf.DescriptorSkipLength =  DISABLE;
#endif
#endif /* MULTI_QUEUE_FEATURE */

  /* enable OSF bit to enhance throughput */
  HAL_ETH_SetDMAConfig(&eth_handle, &dmaDefaultConf);
#ifdef STM32_ETH_PROMISCUOUS_ENABLE
  FilterConfig.PromiscuousMode = ENABLE;
#else
  FilterConfig.PromiscuousMode = DISABLE;
#endif
  FilterConfig.HashUnicast = DISABLE;
  FilterConfig.HashMulticast = DISABLE;
  FilterConfig.DestAddrInverseFiltering = DISABLE;
  FilterConfig.PassAllMulticast = DISABLE;
  FilterConfig.BroadcastFilter = ENABLE;
  FilterConfig.SrcAddrInverseFiltering = DISABLE;
  FilterConfig.SrcAddrFiltering = DISABLE;
  FilterConfig.HachOrPerfectFilter = DISABLE;
  FilterConfig.ReceiveAllMode = DISABLE;
  FilterConfig.ControlPacketsFilter = 0x00;

  /* Set Tx packet config common parameters */
  memset(&TxPacketCfg, 0, sizeof(ETH_TxPacketConfigTypeDef));
  TxPacketCfg.Attributes = ETH_TX_PACKETS_FEATURES_CSUM ;
  TxPacketCfg.CRCPadCtrl = ETH_CRC_PAD_DISABLE;

  /* Clear the number of buffers in use counter.  */
  nx_driver_information.nx_driver_information_multicast_count = 0;

#ifdef NX_DRIVER_ENABLE_PTP
  /*
  * PTP timestamp support
  */
  uint32_t ptp_clk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_ETH1PTP);
  /* initialize tx timestamp support */
  nx_driver_information.nx_driver_ptp_ptr = NULL;

  /* Initialize PTP timestamp clock */
  ptpconfig.Timestamp = ENABLE;
  ptpconfig.TimestampInitialize = DISABLE;
  ptpconfig.TimestampUpdate = DISABLE;
  ptpconfig.TimestampEthernet = DISABLE;
  ptpconfig.TimestampIPv6 = DISABLE;
  ptpconfig.TimestampIPv4 = DISABLE;
  ptpconfig.TimestampEvent = DISABLE;
  ptpconfig.TimestampMaster = DISABLE;
  ptpconfig.TimestampFilter = DISABLE;
  ptpconfig.TimestampRolloverMode = ENABLE;
  ptpconfig.TimestampUpdateMode = HAL_ETH_PTP_FINE_TS_UPDATE;
  ptpconfig.TimestampAll = ENABLE;
  ptpconfig.TimestampV2 = ENABLE;
  ptpconfig.TimestampAddendUpdate = ENABLE;
  ptpconfig.TimestampSubsecondInc = SUBSECOND_INCREMENT;
  ptpconfig.TimestampAddend = (ULONG)((0x100000000UL * PTP_REF_CLK + ptp_clk / 2) / ptp_clk);

  /* Set PTP Configuration */
  HAL_ETH_PTP_SetConfig(&eth_handle, &ptpconfig);
#endif /* NX_DRIVER_ENABLE_PTP */

  /* Return success!  */
  return(NX_SUCCESS);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_enable                                          */
/*                                                            6.1         */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific link enable requests.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    ETH_Start                             Start Ethernet operation      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_enable                     Driver link enable processing */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_enable(NX_IP_DRIVER *driver_req_ptr)
{

  /* Call STM32 library to start Ethernet operation.  */
  HAL_ETH_Start_IT(&eth_handle);

  /* Return success!  */
  return(NX_SUCCESS);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_disable                                         */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific link disable requests.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    ETH_MACTransmissionCmd                Disable transmit              */
/*    ETH_FlushTransmitFIFO                 Flush transmit FIFO           */
/*    ETH_MACReceptionCmd                   Disable receive               */
/*    ETH_DMATransmissionCmd                Stop DMA transmission         */
/*    ETH_DMAReceptionCmd                   Stop DMA reception            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_disable                    Driver link disable processing*/
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_disable(NX_IP_DRIVER *driver_req_ptr)
{

  HAL_ETH_Stop(&eth_handle);

  /* Return success!  */
  return(NX_SUCCESS);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_packet_send                                     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific packet send requests.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    packet_ptr                            Pointer to packet to send     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [_nx_driver_transmit_packet_enqueue]  Optional internal transmit    */
/*                                            packet queue routine        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_packet_send                Driver packet send processing */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
#ifdef MULTI_QUEUE_FEATURE
static UINT  _nx_driver_hardware_packet_send(NX_PACKET *packet_ptr)
{

UINT status;
UCHAR queue_number = 0;

NX_INTERFACE *interface_ptr;

    interface_ptr = nx_driver_information.nx_driver_information_interface;

    status = nx_shaper_hw_queue_id_get(interface_ptr,packet_ptr,&queue_number);

    status =  _nx_driver_hardware_packet_send_distribute(packet_ptr, queue_number);

 return status;

}

static UINT  _nx_driver_hardware_packet_send_distribute(NX_PACKET *packet_ptr, UINT queue_number)
{
  NX_PACKET       *pktIdx;
  UINT            buffLen = 0;
  int i = 0;

  uint32_t TxTimeout = 10;
  uint32_t tickstart;

  ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT];

  memset(Txbuffer, 0 , ETH_TX_DESC_CNT*sizeof(ETH_BufferTypeDef));

#ifdef DEMO_TSN_TALKER
ETH_TimeTypeDef time1;
ETH_TimeTypeDef time2;

UINT type;

    type =( *(packet_ptr->nx_packet_prepend_ptr + 16) << 8) | (*(packet_ptr->nx_packet_prepend_ptr + 17));
    if (type == packet_type)
    {
       HAL_ETH_PTP_GetTime(&heth, &time1);
       HAL_ETH_PTP_GetTime(&heth, &time2);


        set_transmit_timestamp.second_low = time1.NanoSeconds < 500000000UL ? time2.Seconds : time1.Seconds;
        set_transmit_timestamp.nanosecond = (LONG)time1.NanoSeconds;

        if(timestamp_callback)
            timestamp_callback(packet_ptr, TIME_STAMP_TYPE_PACKET_IN_TXBD, &set_transmit_timestamp);
    }
#endif /* DEMO_TSN_TALKER */

  for (pktIdx = packet_ptr;pktIdx != NX_NULL ; pktIdx = pktIdx -> nx_packet_next)
  {
    if (i >= ETH_TX_DESC_CNT)
    {
      return NX_DRIVER_ERROR;
    }

    Txbuffer[i].buffer = pktIdx->nx_packet_prepend_ptr;
    Txbuffer[i].len = (pktIdx -> nx_packet_append_ptr - pktIdx->nx_packet_prepend_ptr);
    buffLen += (pktIdx -> nx_packet_append_ptr - pktIdx->nx_packet_prepend_ptr);

    if(i>0)
    {
      Txbuffer[i-1].next = &Txbuffer[i];
    }

    if (pktIdx-> nx_packet_next ==NULL)
    {
      Txbuffer[i].next = NULL;
    }

    i++;
    clean_cache_by_addr((uint32_t*)(pktIdx -> nx_packet_data_start), pktIdx -> nx_packet_data_end - pktIdx -> nx_packet_data_start);
  }

#ifdef NX_ENABLE_INTERFACE_CAPABILITY
  if (packet_ptr -> nx_packet_interface_capability_flag & (NX_INTERFACE_CAPABILITY_TCP_TX_CHECKSUM |
                                                           NX_INTERFACE_CAPABILITY_UDP_TX_CHECKSUM |
                                                             NX_INTERFACE_CAPABILITY_ICMPV4_TX_CHECKSUM |
                                                               NX_INTERFACE_CAPABILITY_ICMPV6_TX_CHECKSUM))
  {
    TxPacketCfg.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  }
  else if (packet_ptr -> nx_packet_interface_capability_flag & NX_INTERFACE_CAPABILITY_IPV4_TX_CHECKSUM)
  {
    TxPacketCfg.ChecksumCtrl = ETH_CHECKSUM_IPHDR_INSERT;
  }
#else
  TxPacketCfg.ChecksumCtrl = ETH_CHECKSUM_DISABLE;
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */


#ifdef NX_DRIVER_ENABLE_PTP
  /* Enable PTP timestamp */
  if (packet_ptr -> nx_packet_interface_capability_flag & NX_INTERFACE_CAPABILITY_PTP_TIMESTAMP)
  {
    HAL_ETH_PTP_InsertTxTimestamp(&eth_handle);
  }
#endif /* NX_DRIVER_ENABLE_PTP */

  TxPacketCfg.TxCH = queue_number;
  TxPacketCfg.Length = buffLen;
  TxPacketCfg.TxBuffer = Txbuffer;
  TxPacketCfg.pData = (uint32_t *)packet_ptr;

  tickstart = HAL_GetTick();

  /* Wait for descriptors to become available */
  do
  {
    if(HAL_ETH_Transmit_IT(&eth_handle, &TxPacketCfg) != HAL_OK)
    {
      /* Transmit failed */
      if(HAL_ETH_GetError(&eth_handle) & HAL_ETH_ERROR_BUSY)
      {
        eth_handle.TxCH = queue_number;
        /* No TX descriptors available, wait for previous transfers to be completed  */
        HAL_ETH_ReleaseTxPacket(&eth_handle);
      }
      else
      {
        /* Other error */
        HAL_ETH_TxFreeCallback((uint32_t *)packet_ptr);
      }
    }
    else
    {
      nx_driver_information.nx_driver_information_number_of_transmit_buffers_in_use[queue_number]++;

      /* Transmit successful */
      return  NX_SUCCESS;
    }

    /* Check for the Timeout */
    if (TxTimeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > TxTimeout) || (TxTimeout == 0U))
      {
        eth_handle.ErrorCode |= HAL_ETH_ERROR_TIMEOUT;
        return NX_DRIVER_ERROR;
      }
    }
  }while(HAL_ETH_GetError(&eth_handle) & HAL_ETH_ERROR_BUSY);

  return  NX_SUCCESS;
}

#else
static UINT  _nx_driver_hardware_packet_send(NX_PACKET *packet_ptr)
{
  NX_PACKET       *pktIdx;
  UINT            buffLen = 0;

  ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT];
  memset(Txbuffer, 0 , ETH_TX_DESC_CNT*sizeof(ETH_BufferTypeDef));


  int i = 0;

  for (pktIdx = packet_ptr;pktIdx != NX_NULL ; pktIdx = pktIdx -> nx_packet_next)
  {
    if (i >= ETH_TX_DESC_CNT)
    {
      return NX_DRIVER_ERROR;
    }

    Txbuffer[i].buffer = pktIdx->nx_packet_prepend_ptr;
    Txbuffer[i].len = (pktIdx -> nx_packet_append_ptr - pktIdx->nx_packet_prepend_ptr);
    buffLen += (pktIdx -> nx_packet_append_ptr - pktIdx->nx_packet_prepend_ptr);

    if(i>0)
    {
      Txbuffer[i-1].next = &Txbuffer[i];
    }

    if (pktIdx-> nx_packet_next ==NULL)
    {
      Txbuffer[i].next = NULL;
    }

    i++;
    clean_cache_by_addr((uint32_t*)(pktIdx -> nx_packet_data_start), pktIdx -> nx_packet_data_end - pktIdx -> nx_packet_data_start);
  }

#ifdef NX_ENABLE_INTERFACE_CAPABILITY
  if (packet_ptr -> nx_packet_interface_capability_flag & (NX_INTERFACE_CAPABILITY_TCP_TX_CHECKSUM |
                                                           NX_INTERFACE_CAPABILITY_UDP_TX_CHECKSUM |
                                                             NX_INTERFACE_CAPABILITY_ICMPV4_TX_CHECKSUM |
                                                               NX_INTERFACE_CAPABILITY_ICMPV6_TX_CHECKSUM))
  {
    TxPacketCfg.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  }
  else if (packet_ptr -> nx_packet_interface_capability_flag & NX_INTERFACE_CAPABILITY_IPV4_TX_CHECKSUM)
  {
    TxPacketCfg.ChecksumCtrl = ETH_CHECKSUM_IPHDR_INSERT;
  }
#else
  TxPacketCfg.ChecksumCtrl = ETH_CHECKSUM_DISABLE;
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */

  TxPacketCfg.Length = buffLen;
  TxPacketCfg.TxBuffer = Txbuffer;
  TxPacketCfg.pData = (uint32_t *)packet_ptr;

  if(HAL_ETH_Transmit_IT(&eth_handle, &TxPacketCfg))
  {
    return(NX_DRIVER_ERROR);
  }

  return(NX_SUCCESS);
}
#endif /* MULTI_QUEUE_FEATURE */

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_multicast_join                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific multicast join requests.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_multicast_join             Driver multicast join         */
/*                                            processing                  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_multicast_join(NX_IP_DRIVER *driver_req_ptr)
{


  /* Increase the multicast count.  */
  nx_driver_information.nx_driver_information_multicast_count++;

  /* Enable multicast frame reception.  */
  FilterConfig.PassAllMulticast = ENABLE;
  HAL_ETH_SetMACFilterConfig(&eth_handle, &FilterConfig);

  /* Return success.  */
  return(NX_SUCCESS);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_multicast_leave                                 */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific multicast leave requests. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]*/
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_multicast_leave            Driver multicast leave        */
/*                                            processing                  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_multicast_leave(NX_IP_DRIVER *driver_req_ptr)
{

  /* Decrease the multicast count.  */
  nx_driver_information.nx_driver_information_multicast_count--;

  /* If multicast count reaches zero, disable multicast frame reception.  */
  if (nx_driver_information.nx_driver_information_multicast_count == 0)
  {

    /* Disable multicast frame reception.  */
    FilterConfig.PassAllMulticast = DISABLE;
    HAL_ETH_SetMACFilterConfig(&eth_handle, &FilterConfig);
  }

  /* Return success.  */
  return(NX_SUCCESS);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_get_status                                      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific get status requests.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                        Driver request pointer        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]*/
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_get_status                 Driver get status processing  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT  _nx_driver_hardware_get_status(NX_IP_DRIVER *driver_req_ptr)
{
  INT PHYLinkState;

  /* Get link status. */
  PHYLinkState = nx_eth_phy_get_link_state();

  /* Check link status. */
  if(PHYLinkState <= ETH_PHY_STATUS_LINK_DOWN)
  {
    /* Update Link status if physical link is down. */
    *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_FALSE;
  }
  else
  {
    /* Update Link status if physical link is up. */
    *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_TRUE;
  }

  /* Return success. */
  return NX_SUCCESS;
}

void HAL_ETH_TxFreeCallback(uint32_t * buff)
{
  NX_PACKET * release_packet = (NX_PACKET *) buff;

  /* Remove the Ethernet header and release the packet.  */
  NX_DRIVER_ETHERNET_HEADER_REMOVE(release_packet);

  /* Release the packet.  */
  nx_packet_transmit_release(release_packet);
}

#ifdef NX_DRIVER_ENABLE_PTP

void HAL_ETH_TxPtpCallback(uint32_t * buff, ETH_TimeStampTypeDef *timestamp)
{
  NX_PTP_TIME ts;
  NX_PACKET * release_packet = (NX_PACKET *) buff;

  if (release_packet -> nx_packet_interface_capability_flag &
      NX_INTERFACE_CAPABILITY_PTP_TIMESTAMP)
  {
    /* store timestamp in NX_PTP_TIME structure */
    ts.second_high = 0;
    ts.second_low  = timestamp->TimeStampHigh;
    ts.nanosecond  = timestamp->TimeStampLow;

    /* call notification callback */
    nx_ptp_client_packet_timestamp_notify(nx_driver_information.nx_driver_ptp_ptr,
                                          release_packet, &ts);
  }

#ifdef DEMO_TSN_TALKER

UINT        type =( *(release_packet->nx_packet_prepend_ptr - 2 ) << 8) |
                     (*(release_packet->nx_packet_prepend_ptr - 1));
            /* Handle PTP timestamp notification */
            if (type == packet_type)
            {
                if (0U == (heth.Instance ->MACTSCR & ETH_MACTSCR_TSCTRLSSR))
                {
                    /* Binary rollover, 0.465ns accuracy. */
                    ts.nanosecond = (tas_timestamp.nanosecond * 465U) / 1000U;
                }

                if (timestamp_callback)
                    timestamp_callback(release_packet, TIME_STAMP_TYPE_PACKET_TX_DONE, &ts);

            }
#endif
}
#endif /* NX_DRIVER_ENABLE_PTP */

static VOID  _nx_driver_hardware_packet_received(VOID)
{
  NX_PACKET  *received_packet_ptr;

#ifdef NX_DRIVER_ENABLE_PTP
  ULONG      ts[2] = {0, 0};
  ETH_TimeStampTypeDef timestamp;
#endif /* NX_DRIVER_ENABLE_PTP */

  while (HAL_ETH_ReadData(&eth_handle, (void **)&received_packet_ptr) == HAL_OK)
  {
#ifdef NX_DRIVER_ENABLE_PTP
    /* Save PTP timestamp */
    HAL_ETH_PTP_GetRxTimestamp(&eth_handle, &timestamp);
    ts[0] = timestamp.TimeStampLow;
    ts[1] = timestamp.TimeStampHigh;

    /* Transfer the packet to NetX.  */
    _nx_driver_transfer_to_netx(nx_driver_information.nx_driver_information_ip_ptr, received_packet_ptr, ts);
#else
    /* Transfer the packet to NetX.  */
    _nx_driver_transfer_to_netx(nx_driver_information.nx_driver_information_ip_ptr, received_packet_ptr);

#endif /* NX_DRIVER_ENABLE_PTP */
  }
}

void HAL_ETH_RxAllocateCallback(uint8_t ** buff)
{
  NX_PACKET     *packet_ptr;
  if (nx_packet_allocate(nx_driver_information.nx_driver_information_packet_pool_ptr, &packet_ptr,
                         NX_RECEIVE_PACKET, NX_NO_WAIT) == NX_SUCCESS)
  {
    /* Adjust the packet.  */
    packet_ptr -> nx_packet_prepend_ptr += 2;
    invalidate_cache_by_addr((uint32_t*)packet_ptr -> nx_packet_data_start, packet_ptr -> nx_packet_data_end - packet_ptr -> nx_packet_data_start);
    *buff = packet_ptr -> nx_packet_prepend_ptr;
  }
  else
  {
    /* Rx Buffer Pool is exhausted. */
    *buff = NULL;
  }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                                              */
/*                                                                        */
/*    HAL_ETH_RxLinkCallback                                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function chains all received packets before passing            */
/*    the first one to NetXDuo stack.                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    first_packet_ptr           pointer to first received packet         */
/*    last_packet_ptr            pointer to last received packet          */
/*    buff                       pointer to received data                 */
/*    length                     received data length                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    received packet pointer                                             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    HAL_ETH_ReadData              Read a received packet                */
/*                                                                        */
/**************************************************************************/
void HAL_ETH_RxLinkCallback(void **first_packet_ptr, void **last_packet_ptr, uint8_t *buff, uint16_t Length)
{
  NX_PACKET **first_nx_packet_ptr = (NX_PACKET **)first_packet_ptr;
  NX_PACKET **last_nx_packet_ptr = (NX_PACKET **)last_packet_ptr;
  NX_PACKET  *received_packet_ptr;

  /* Indicate the offset of the received data.  */
  uint8_t *data_buffer_ptr = buff - 2U - header_size;

  received_packet_ptr = (NX_PACKET *)data_buffer_ptr;
  received_packet_ptr->nx_packet_append_ptr = received_packet_ptr->nx_packet_prepend_ptr + Length;
  received_packet_ptr->nx_packet_length = Length;

  /* Check whether this is the first packet. */
  if (*first_nx_packet_ptr == NULL)
  {
    /* Add the first buffer of the packet. */
    *first_nx_packet_ptr = received_packet_ptr;
  }
  /* This is not the first packet. */
  else
  {
    /* Add the rest of the buffer to the end of the packet. */
    (*last_nx_packet_ptr)->nx_packet_next = received_packet_ptr;
  }
  /* Save the current packet in order to use it in the next iteration. */
  *last_nx_packet_ptr  = received_packet_ptr;
}

#ifdef NX_ENABLE_INTERFACE_CAPABILITY
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_driver_hardware_capability_set                                  */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes hardware-specific capability set requests.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    driver_req_ptr                         Driver request pointer       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _nx_driver_capability_set             Capability set processing     */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
static UINT _nx_driver_hardware_capability_set(NX_IP_DRIVER *driver_req_ptr)
{

  return NX_SUCCESS;
}
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */


void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{

  ULONG deffered_events;
  deffered_events = nx_driver_information.nx_driver_information_deferred_events;
#ifdef MULTI_QUEUE_FEATURE
  uint32_t channel = heth->RxCH;

  if (channel == ETH_DMA_CH0)
  {
    nx_driver_information.nx_driver_information_deferred_events |= NX_DRIVER_DEFERRED_PACKET_RECEIVED_CH0;
  }
  else /* ETH_DMA_CH1 */
    nx_driver_information.nx_driver_information_deferred_events |= NX_DRIVER_DEFERRED_PACKET_RECEIVED_CH1;
#else
  nx_driver_information.nx_driver_information_deferred_events |= NX_DRIVER_DEFERRED_PACKET_RECEIVED;
#endif /* MULTI_QUEUE_FEATURE */
  if (!deffered_events)
  {
    /* Call NetX deferred driver processing.  */
    _nx_ip_driver_deferred_processing(nx_driver_information.nx_driver_information_ip_ptr);
  }
}

void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth)
{
  ULONG deffered_events;
  deffered_events = nx_driver_information.nx_driver_information_deferred_events;
#ifdef MULTI_QUEUE_FEATURE
  uint32_t channel = heth->TxCH;

  if (channel == ETH_DMA_CH0)
    nx_driver_information.nx_driver_information_deferred_events |= NX_DRIVER_DEFERRED_PACKET_TRANSMITTED_CH0;

  if (channel == ETH_DMA_CH1)
    nx_driver_information.nx_driver_information_deferred_events |= NX_DRIVER_DEFERRED_PACKET_TRANSMITTED_CH1;
#else
    nx_driver_information.nx_driver_information_deferred_events |= NX_DRIVER_DEFERRED_PACKET_TRANSMITTED;
#endif /* MULTI_QUEUE_FEATURE */

  if (!deffered_events)
  {
    /* Call NetX deferred driver processing.  */
    _nx_ip_driver_deferred_processing(nx_driver_information.nx_driver_information_ip_ptr);
  }
}

#ifdef NX_DRIVER_ENABLE_PTP
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    nx_driver_ptp_clock_callback                                        */
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function handles PTP clock operations.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    client_ptr                                  Client pointer          */
/*    operation                                   operation               */
/*    time_ptr                                    Time pointer            */
/*    packet_ptr                                  Packet pointer          */
/*    callback_data                               callback data           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                [NX_SUCCESS|NX_DRIVER_ERROR]  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    NetX PTP Client                                                     */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*  xx-xx-xxxx     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  nx_driver_ptp_clock_callback(NX_PTP_CLIENT *client_ptr, UINT operation,
                                   NX_PTP_TIME *time_ptr, NX_PACKET *packet_ptr,
                                   VOID *callback_data)
{
  TX_INTERRUPT_SAVE_AREA
  ULONG sec1, sec2, ns;
  ETH_TimeTypeDef time;
  uint32_t tickstart;
  UINT NX_PTP_Status =0;
  ETH_TimeTypeDef time_offset;

  NX_PARAMETER_NOT_USED(callback_data);

  switch (operation)
  {

    /* Save pointer to PTP client.  */
    case NX_PTP_CLIENT_CLOCK_INIT:
      nx_driver_information.nx_driver_ptp_ptr = client_ptr;
      break;

    /* Set clock.  */
    case NX_PTP_CLIENT_CLOCK_SET:
      TX_DISABLE

      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait to get PTP control or timeout occurred */
      while (((HAL_GetTick() - tickstart) > HAL_PTP_TIMEOUT) )
      {
        if (__HAL_ETH_GET_PTP_CONTROL(&eth_handle, ETH_MACTSCR_TSUPDT) == 0)
        {
          NX_PTP_Status = NX_WAIT_ERROR;
          break;
        }
      }

      time.Seconds = time_ptr -> second_low;
      time.NanoSeconds = time_ptr -> nanosecond;
      HAL_ETH_PTP_SetTime(&eth_handle, &time);

      TX_RESTORE
      break;

    /* Extract timestamp from packet.  */
    case NX_PTP_CLIENT_CLOCK_PACKET_TS_EXTRACT:

      /* XXX second timestamp is only 32-bit */
      time_ptr -> second_high = 0;

      /* Return timestamp stored at the beginning of the packet.  */
      time_ptr -> nanosecond = ((ULONG *)packet_ptr -> nx_packet_data_start)[0];
      time_ptr -> second_low = ((LONG *)packet_ptr -> nx_packet_data_start)[1];
      break;

    /* Get clock.  */
    case NX_PTP_CLIENT_CLOCK_GET:
      TX_DISABLE

      HAL_ETH_PTP_GetTime(&eth_handle, &time);
      sec1 = time.Seconds;
      ns   = time.NanoSeconds;
      HAL_ETH_PTP_GetTime(&eth_handle, &time);
      sec2 = time.Seconds;
      time_ptr -> second_high = 0;
      /* The offset standard deviation is below 50 ns */
      time_ptr -> second_low = ns < 500000000UL ? sec2 : sec1;
      time_ptr -> nanosecond = (LONG)ns;

      TX_RESTORE
      break;

    /* Adjust clock.  */
    case NX_PTP_CLIENT_CLOCK_ADJUST:
      TX_DISABLE
      time_offset.Seconds = 0;
      if(time_ptr->nanosecond < 0)
      {
        time_offset.NanoSeconds = - time_ptr->nanosecond;
        HAL_ETH_PTP_AddTimeOffset(&heth, HAL_ETH_PTP_NEGATIVE_UPDATE, &time_offset);
      }
      else
      {
        time_offset.NanoSeconds = time_ptr->nanosecond;
        HAL_ETH_PTP_AddTimeOffset(&heth, HAL_ETH_PTP_POSITIVE_UPDATE, &time_offset);
      }
      TX_RESTORE

      break;

    /* Prepare timestamp for current packet.  */
    case NX_PTP_CLIENT_CLOCK_PACKET_TS_PREPARE:
      packet_ptr -> nx_packet_interface_capability_flag |= NX_INTERFACE_CAPABILITY_PTP_TIMESTAMP;
      break;

    /* Update soft timer. Not used by hardware callback function.  */
    case NX_PTP_CLIENT_CLOCK_SOFT_TIMER_UPDATE:
      break;

    default:
      NX_PTP_Status = NX_PTP_PARAM_ERROR;
  }

  return(NX_PTP_Status);
}

UINT nx_driver_set_timestamp_callback(USHORT type, TIMESTAMP_CALLBACK callback)
{
    timestamp_callback = callback;
    packet_type = type;

    return 0;
}
#endif /* NX_DRIVER_ENABLE_PTP */

#ifdef NX_DRIVER_ENABLE_TSN
UINT nx_driver_shaper_config(NX_SHAPER_DRIVER_PARAMETER* parameter, UCHAR shaper_capability )
{
UINT status;
NX_SHAPER_HW_QUEUE hw_queue[ETH_MTL_TX_Q_CNT];


    hw_queue[0].hw_queue_id = 0;
    hw_queue[0].priority = 1;
    hw_queue[0].type = NX_SHAPER_HW_QUEUE_NORMAL;

    hw_queue[1].hw_queue_id = 1;
    hw_queue[1].priority = 2;
    hw_queue[1].type = NX_SHAPER_HW_QUEUE_CBS;

    status = nx_shaper_config(parameter -> nx_ip_driver_interface, PORT_RATE, shaper_capability,ETH_MTL_TX_Q_CNT, hw_queue);

    return status;
}

#ifdef NX_DRIVER_ENABLE_CBS
UINT nx_driver_shaper_cbs_entry(NX_SHAPER_DRIVER_PARAMETER *parameter)
{
UCHAR shaper_capability;
UINT status = NX_SUCCESS;
NX_SHAPER_CBS_PARAMETER * cbs_parameter;
ETH_CBSConfigTypeDef cbsconf;
unsigned int idle_slope;

    switch (parameter->nx_shaper_driver_command)
    {
        case NX_SHAPER_COMMAND_INIT:

            break;

        case NX_SHAPER_COMMAND_CONFIG:

            shaper_capability =  NX_SHAPER_CAPABILITY_CBS_SUPPORTED;

            nx_driver_shaper_config(parameter,shaper_capability);

            break;

        case NX_SHAPER_COMMAND_PARAMETER_SET:

            cbs_parameter = (NX_SHAPER_CBS_PARAMETER *)parameter -> shaper_parameter;

            idle_slope = cbs_parameter -> idle_slope * 1000000; //bit per second

            cbsconf.QueueIdx = cbs_parameter -> hw_queue_id;
            cbsconf.SlotCount = 0;
            cbsconf.CreditControl = 0;
            cbsconf.IdleSlope = ((((uint64_t) idle_slope * 1024) + AVB_IDLE_SLOPE_CYCLE_FACTOR_100M - 1) / AVB_IDLE_SLOPE_CYCLE_FACTOR_100M);
            cbsconf.SendSlope = (AVB_MAX_PORT_TRANSIT_RATE_100M - cbsconf.IdleSlope) ;
            cbsconf.HiCredit = AVB_HI_CREDIT;
            cbsconf.LoCredit = AVB_LO_CREDIT;

            /* Enable CBS feature */
            HAL_ETHEx_EnableCBS(&heth, cbsconf.QueueIdx);

            /* Set CBS Configuration */
            HAL_ETHEx_SetCBSConfig(&heth, &cbsconf);

            break;

        default:

            break;
    }

    return status;
}
#endif /* NX_DRIVER_ENABLE_CBS */

#ifdef NX_DRIVER_ENABLE_FPE
UINT nx_driver_shaper_fpe_entry(NX_SHAPER_DRIVER_PARAMETER *parameter)
{
NX_SHAPER_FP_PARAMETER * fpe_parameter;
UCHAR shaper_capability;
ETH_FPEConfigTypeDef fpeconf;

    switch (parameter->nx_shaper_driver_command)
    {

        case NX_SHAPER_COMMAND_INIT:

            /* Enable Tx Frame Preemption */
            HAL_ETHEx_EnableFPE(&heth);

            break;
        case NX_SHAPER_COMMAND_CONFIG:

            shaper_capability =  NX_SHAPER_CAPABILITY_PREEMPTION_SUPPORTED;

            nx_driver_shaper_config(parameter,shaper_capability);

            break;
        case NX_SHAPER_COMMAND_PARAMETER_SET:

            memset(&fpeconf,0,sizeof(ETH_FPEConfigTypeDef));

            fpe_parameter = (NX_SHAPER_FP_PARAMETER *)parameter -> shaper_parameter;

            fpeconf.AdditionalFragmentSize = 0;
            fpeconf.SendRespondmPacket = DISABLE;
            fpeconf.SendVerifymPacket = DISABLE;
            fpeconf.HoldReleaseStatus = 0;
            fpeconf.PreemptionClassification = (((uint32_t)(((uint32_t)(~fpe_parameter->express_queue_bitmap)) << 8)) & 0x300);
            fpeconf.ReleaseAdvance = (fpe_parameter->ra << 16);
            fpeconf.HoldAdvance = fpe_parameter->ha;

            HAL_ETHEx_SetFPEConfig(&heth, &fpeconf);

            break;

        default:

            break;
    }

    return 0;
}
#endif /* NX_DRIVER_ENABLE_FPE */

#ifdef NX_DRIVER_ENABLE_TAS
#ifdef NX_DRIVER_ENABLE_FPE
void reconfig_fpe_gcl(NX_SHAPER_TAS_PARAMETER *parameter,UCHAR queue_bit)
{
UINT i = 0;

    for(i=0; i < parameter->gcl_length; i++)
    {
        /*queue_bit: bit=1 means express; bit =0 means preemtable*/
        /*gata_control: 1 means open for express queue, 1 means hold(close)for express queue*/
        if(parameter->gcl[i].operation == NX_SHAPER_GATE_OPERATION_HOLD)
        {
            parameter->gcl[i].gate_control |= 1;
        }
        else if(parameter->gcl[i].operation == NX_SHAPER_GATE_OPERATION_RELEASE)
        {
            parameter->gcl[i].gate_control &= 0xfe;
        }
    }
}
#endif /* NX_DRIVER_ENABLE_FPE */
UINT nx_driver_shaper_tas_entry(NX_SHAPER_DRIVER_PARAMETER *parameter)
{
UCHAR shaper_capability;
ETH_TimeTypeDef time;
UINT status = NX_SUCCESS;
NX_SHAPER_TAS_PARAMETER * tas_parameter;
UINT i;
ETH_ESTConfigTypeDef estconfig;
ETH_GCLConfigTypeDef* gate_ctl = &estconfig.GCLRegisters;
ETH_TASOperationConfigTypeDef opconfig[NX_SHAPER_GCL_LENGTH_MAX];

    switch (parameter->nx_shaper_driver_command)
    {
        case NX_SHAPER_COMMAND_INIT:
            break;

        case NX_SHAPER_COMMAND_CONFIG:

            shaper_capability =  NX_SHAPER_CAPABILITY_TAS_SUPPORTED;
            nx_driver_shaper_config(parameter,shaper_capability);
            break;

        case NX_SHAPER_COMMAND_PARAMETER_SET:

            memset(&estconfig,0,sizeof(estconfig));

            tas_parameter = (NX_SHAPER_TAS_PARAMETER *)parameter -> shaper_parameter;
#ifdef NX_DRIVER_ENABLE_FPE
            if(tas_parameter -> fp_parameter)
            {
                reconfig_fpe_gcl(tas_parameter,((NX_SHAPER_FP_PARAMETER*)tas_parameter->fp_parameter)->express_queue_bitmap);
            }
#endif /* NX_DRIVER_ENABLE_FPE */
            if (tas_parameter->gcl_length > NX_SHAPER_GCL_LENGTH_MAX)
            {
              status = NX_NOT_SUCCESSFUL;
              return status;
            }

            gate_ctl -> BaseTimeRegister = tas_parameter->base_time;              /*! Base Time 32 bits seconds 32 bits nanoseconds */
            gate_ctl-> CycleTimeRegister = tas_parameter->cycle_time;             /*! Cycle Time 32 bits seconds 32 bits nanoseconds */
            gate_ctl->TimeExtensionRegister = tas_parameter->cycle_time_extension;  /*! Time Extension 32 bits seconds 32 bits nanoseconds */
            gate_ctl->ListLengthRegister = tas_parameter->gcl_length;            /*! Number of entries */
            gate_ctl->opList = opconfig;

            memset(opconfig, 0, sizeof(opconfig));

            for (i=0; i<gate_ctl->ListLengthRegister; i++)
            {
              opconfig[i].Gate = tas_parameter->gcl[i].gate_control;
              opconfig[i].Interval = tas_parameter->gcl[i].duration;
            }

            /*we will set the value added with a micro delay */
            if(gate_ctl->BaseTimeRegister == 0 )
            {
              HAL_ETH_PTP_GetTime(&heth, &time);
              gate_ctl->BaseTimeRegister = (ULONG64)time.Seconds << 32| (ULONG64)time.NanoSeconds;
              /*add  5 second delay from now */
              gate_ctl->BaseTimeRegister += (ULONG64)5 << 32;
            }

            estconfig.PTPTimeOffset = PTP_REF_CLK;
            estconfig.SwitchToSWOL = 1;

            HAL_ETHEx_SetESTConfig(&heth,&estconfig);
            break;

        default:

            break;
    }

    return status;
}
#endif /* NX_DRIVER_ENABLE_TAS */
#endif /* NX_DRIVER_ENABLE_TSN */
/****** DRIVER SPECIFIC ****** Start of part/vendor specific internal driver functions.  */
