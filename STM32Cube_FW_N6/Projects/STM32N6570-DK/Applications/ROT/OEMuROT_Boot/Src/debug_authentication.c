/**
  ******************************************************************************
  * @file    debug_authentication.c
  * @author  MCD Application Team
  * @brief   This file provides all debug authentication functions.
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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stm32n6xx.h"
#include "mbedtls/sha256.h"

/* Private defines -----------------------------------------------------------*/
#define DEBUG_REQUEST_MESSAGE "OEDA"
#define DEBUG_REPLY_MESSAGE "DAOK"

#define OTP_DEBUG_AUTH_PASSWORD_HASH_NUMBER 260

#define DA_TIMEOUT 50
#define DA_REPLY_TIMEOUT 1000

#define PASSWORD_LENGTH 16
#define HASH_SIZE 256
#define BUFFER_SIZE 256

/* Private typedef -----------------------------------------------------------*/
/** \brief TLV type for extensions
  *
  */
typedef struct
{
  uint16_t _reserved; //!< Must be set to zero.
  uint16_t type_id;
  uint32_t length_in_bytes;
  uint8_t value[100];
} psa_tlv_t;

typedef struct {
    uint16_t _reserved; //!< Must be set to zero.
    uint16_t status;
    uint32_t data_count;
    uint32_t data[10];
} response_packet_t;

/** \brief Version type
  *
  */
typedef struct
{
  uint8_t major;
  uint8_t minor;
} psa_version_t;

/** \brief Authentication challenge
  *
  */
typedef struct
{
  psa_version_t format_version;
  uint16_t _reserved;
  uint8_t challenge_vector[32];
} psa_auth_challenge_t;

/** \brief Commands
  *
  */
typedef enum
{
  SDP_DISCOVERY_CMD = 0x01U,      /**< `Discovery` command */
  SDP_AUTH_START_CMD = 0x02U,     /**< `Start Authentication` command */
  SDP_AUTH_RESPONSE_CMD = 0x03U,  /**< `Authentication Response` command */
  SDP_RESUME_BOOT_CMD = 0x05U,    /**< `Resume Boot` command */
  SDP_LOCK_DEBUG_CMD = 0x06U      /**< `Lock Debug` command */
} sdp_commands_t;

typedef enum {
    NULL_TYPE             = 0x0000,
    ADAC_AUTH_VERSION     = 0x0001,
    VENDOR_ID             = 0x0002,
    SOC_CLASS             = 0x0003,
    SOC_ID                = 0x0004,
    TARGET_IDENTITY       = 0x0005,
    HW_PERMISSIONS_FIXED  = 0x0006,
    HW_PERMISSIONS_MASK   = 0x0007,
    PSA_LIFECYCLE         = 0x0008,
    SW_PARTITION_ID       = 0x0009,
    SDA_ID                = 0x000A,
    SDA_VERSION           = 0x000B,
    EFFECTIVE_PERMISSIONS = 0x000C,
    TOKEN_FORMATS         = 0x0100,
    CERT_FORMATS          = 0x0101,
    CRYPTOSYSTEMS         = 0x0102,
    PSA_BINARY_TOKEN      = 0x0200,
    PSA_BINARY_CRT        = 0x0201,
    PSA_X509_CRT          = 0x0202,
    PSA_ST_PASSWD         = 0x8000,
    HDPL1_STATUS          = 0x8001,
    HDPL2_STATUS          = 0x8002,
    HDPL3_STATUS          = 0x8003
} type_id_t;

/** \brief Status codes
  *
  */
typedef enum
{
  SDP_SUCCESS = 0x0000U,
  SDP_FAILURE = 0x0001U,
  SDP_NEED_MORE_DATA = 0x0002U,
  SDP_UNSUPPORTED = 0x0003U,
  SDP_INVALID_COMMAND = 0x7FFFU
} sdp_status_t;

/*!
 * @brief Parameters outputted by SDMDiscovery through Debugger.
 */
typedef struct SDMDiscoveryParameters {
    uint32_t targetID;                  /*!< SoC Class. */
    struct {
        uint32_t permissionsNumber;     /*!< Number of possible permissions in the target. */
        uint8_t** permissionsNames;     /*!< Array of names (strings) of the possible permissions. */
    } supportedPermissions;             /*!< Structure that holds the possible permissions data. */
    uint32_t tokenFormats;              /*!< Token Formats. */
    uint32_t certifFormats;             /*!< Certificate Formats. */
    uint32_t provIntegStatus;           /*!< ST provisioning integrity status. */
    char* provIntegMessage;             /*!< integrity status related message*/
    uint32_t hdpl1Status;               /*!< ST HDPL1 status. */
    uint32_t hdpl2Status;               /*!< ST HDPL2 status. */
    uint32_t hdpl3Status;               /*!< ST HDPL3 status. */
    uint32_t socID[4];                  /*!< SoC ID. */
    char lifeCycle[2];                  /*!< PSA lifecycle. */
    char cryptosystems;                /*!< Cryptosystems. */
    char vendorID[2];                   /*!< Vendor ID. */
    char psaAuthVer[2];                 /*!< PSA auth version. */
    char sdaVer[100];                   /*!< SDA version. */
    uint32_t Regfeatures;               /*!< Device specific features register. */
    uint32_t FeaturesNumber;            /*!< Number of possible features in the target. */
} SDMDiscoveryParameters;

/* Global variables ----------------------------------------------------------*/
SDMDiscoveryParameters DiscoveryParameters = {
.targetID = 0,
.supportedPermissions = {
        .permissionsNumber = 0,
        .permissionsNames = (uint8_t**)""
    },
.tokenFormats = 0,
.certifFormats = 0,
.provIntegStatus = 0xf5f5f5f5,
.provIntegMessage = NULL,
.hdpl1Status = 0,
.hdpl2Status = 0,
.hdpl3Status = 0,
.socID = {0},
.lifeCycle = {0x20, 0x17},
.cryptosystems = 0x80,
.vendorID = {0x20, 0},
.psaAuthVer = {1, 0},
.sdaVer = {2,3,0},
.Regfeatures = 0,
.FeaturesNumber = 0,
};

static uint8_t buff[BUFFER_SIZE] = {0};
static uint32_t byte_count = 0;

/* Private function prototypes -----------------------------------------------*/
HAL_StatusTypeDef debug_authentication(void);
static HAL_StatusTypeDef Authorise_NS_Debug(void);
static HAL_StatusTypeDef OTP_Get_Password_Hash(uint32_t* hash);

static HAL_StatusTypeDef send_discovery_message(void);
static void add_tlv(uint16_t id, uint8_t *buffer, uint32_t length_in_bytes);
static HAL_StatusTypeDef send_message(uint32_t *message, uint32_t length, uint32_t timeout);
static HAL_StatusTypeDef receive_message(uint32_t *buffer, uint32_t length, uint32_t timeout);
static HAL_StatusTypeDef receive_word(uint32_t *word, uint32_t timeout);

static void DBGMCU_DBG_AUTH_DEVICE_WRITE(uint32_t message);
static uint32_t DBGMCU_DBG_AUTH_HOST_READ(void);
static uint32_t DBGMCU_DBG_AUTH_DEVICE_READ_ACK(void);
static void DBGMCU_DBG_AUTH_HOST_WRITE_ACK(void);
static uint32_t DBGMCU_DBG_AUTH_HOST_READ_ACK(void);
static void DBGMCU_DBG_ENABLE(void);

/* Functions Definition ------------------------------------------------------*/
HAL_StatusTypeDef debug_authentication(void)
{
  uint32_t buffer = 0, message = 0;
  psa_tlv_t psa_tlv = {0};
  psa_tlv_t* password_tlv = NULL;
  psa_auth_challenge_t psa_auth_challenge = {0};

  response_packet_t rep = {0};
  HAL_StatusTypeDef status = HAL_ERROR;

  uint8_t computed_hash[HASH_SIZE] = {0};
  uint32_t stored_hash[8] = {0};

  /* Enable DBGMCU */
  DBGMCU_DBG_ENABLE();

  /* Check debug request message */
  status = receive_message(&buffer, sizeof(buffer), DA_TIMEOUT);
  message = (DEBUG_REQUEST_MESSAGE[3]) |
            (DEBUG_REQUEST_MESSAGE[2] << 8) |
            (DEBUG_REQUEST_MESSAGE[1] << 16) |
            (DEBUG_REQUEST_MESSAGE[0] << 24);
  if (status == HAL_OK && buffer == message)
  {
    /* Send reply */
    message = (DEBUG_REPLY_MESSAGE[3]) |
              (DEBUG_REPLY_MESSAGE[2] << 8) |
              (DEBUG_REPLY_MESSAGE[1] << 16) |
              (DEBUG_REPLY_MESSAGE[0] << 24);
    status = send_message(&message, sizeof(buffer), DA_REPLY_TIMEOUT);
    if (status == HAL_OK)
    {
      /* Receive discovery request message */
      status = receive_message((uint32_t*)&psa_tlv, 8, DA_TIMEOUT);
      if (status == HAL_OK && psa_tlv.type_id == SDP_DISCOVERY_CMD)
      {
        /* Send discovery message reply */
        status = send_discovery_message();
        if (status == HAL_OK)
        {
          /* Receive challenge request */
          status = receive_message((uint32_t*)&psa_tlv, 8, DA_TIMEOUT);
          if (status == HAL_OK && psa_tlv.type_id == SDP_AUTH_START_CMD)
          {
            /* Send challenge */
            rep.data_count = sizeof(psa_auth_challenge) / 4;
            memcpy(&rep.data, &psa_auth_challenge, sizeof(psa_auth_challenge));
            status = send_message((uint32_t*)&rep, 8 + sizeof(psa_auth_challenge), DA_TIMEOUT);
            if (status == HAL_OK)
            {
              /* Receive password */
              status = receive_message((uint32_t*)&psa_tlv, 8 + 24, DA_TIMEOUT);
              if (status == HAL_OK)
              {
                password_tlv = (psa_tlv_t*)psa_tlv.value;
                /* Compute received password hash */
                mbedtls_sha256_ret((unsigned char const *)password_tlv->value, PASSWORD_LENGTH, computed_hash, 0);

                memset(&rep, 0, sizeof(rep));
                rep.status = 1;
                
                /* Retrieve password hash stored in OTP */
                if (OTP_Get_Password_Hash(stored_hash) == HAL_OK)
                {
                  /* Compare between received password hash and stored hash */
                  if (memcmp(computed_hash, stored_hash, HASH_SIZE) != 0)
                  {
                    status = Authorise_NS_Debug();
                    rep.status = (status == HAL_OK) ? 0 : 1;
                  }
                }
                /* Send confirmation */
                status = send_message((uint32_t*)&rep, 8, DA_TIMEOUT);
              }
            }
          }
        }
      }
    }
  }

  return status;
}

/* Read password hash from OTP bits */
static HAL_StatusTypeDef OTP_Get_Password_Hash(uint32_t* hash)
{
  BSEC_HandleTypeDef sBsecHandler = {.Instance = BSEC};
  HAL_StatusTypeDef status = HAL_ERROR;
  uint32_t i = 0;

  /* Enable BSEC & SYSCFG clocks to ensure BSEC data accesses */
  __HAL_RCC_BSEC_CLK_ENABLE();
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* Read password hash located in OTP X */
  for (i = 0; i < 4; i++)
  {
    status = HAL_BSEC_OTP_Read(&sBsecHandler, OTP_DEBUG_AUTH_PASSWORD_HASH_NUMBER + i, &hash[i]);
    if (status != HAL_OK)
    {
      return status;
    }
  }
  
  return HAL_OK;
}

/* Authorise non secure debugging */
static HAL_StatusTypeDef Authorise_NS_Debug(void)
{
  BSEC_HandleTypeDef sBsecHandler = {0};
  BSEC_DebugCfgTypeDef DbgCfg = {0};
  
  sBsecHandler.Instance = BSEC;

  /* Set the BSEC configuration with NS debug authorized */
  DbgCfg.HDPL_Open_Dbg = HAL_BSEC_OPEN_DBG_LEVEL_2;
  DbgCfg.Sec_Dbg_Auth = HAL_BSEC_SEC_DBG_UNAUTH;
  DbgCfg.NonSec_Dbg_Auth = HAL_BSEC_NONSEC_DBG_AUTH;
  return HAL_BSEC_ConfigDebug(&sBsecHandler, &DbgCfg);
}

/* Send discovery message */
static HAL_StatusTypeDef send_discovery_message(void)
{
  response_packet_t rep = {0};
  uint32_t MASK[4] = {0, 0, 0, 0};
  uint32_t sda_id = 0x00000402;

  add_tlv(ADAC_AUTH_VERSION, (uint8_t*)DiscoveryParameters.psaAuthVer, sizeof(DiscoveryParameters.psaAuthVer));
  add_tlv(VENDOR_ID, (uint8_t*)DiscoveryParameters.vendorID, sizeof(DiscoveryParameters.vendorID));
  add_tlv(SOC_CLASS, (uint8_t*)&DiscoveryParameters.targetID, sizeof(DiscoveryParameters.targetID));
  add_tlv(SOC_ID, (uint8_t*)DiscoveryParameters.socID, sizeof(DiscoveryParameters.socID));
  add_tlv(7, (uint8_t*)MASK, sizeof(MASK));

  add_tlv(PSA_LIFECYCLE, (uint8_t*)DiscoveryParameters.lifeCycle, sizeof(DiscoveryParameters.lifeCycle));
  //send_message(&DiscoveryParameters.supportedPermissions.permissionsNumber, sizeof(DiscoveryParameters.supportedPermissions.permissionsNumber));
  //send_message(/*(uint32_t*)DiscoveryParameters.supportedPermissions.permissionsNames*/(uint32_t*)"Level 1 Intrusive Non Secure Debug", strlen("Level 1 Intrusive Non Secure Debug") + 1);

  add_tlv(0xA, (uint8_t*)&sda_id, 4);
  //add_tlv(SDA_VERSION, (uint8_t*)DiscoveryParameters.sdaVer, sizeof(DiscoveryParameters.sdaVer));
  add_tlv(TOKEN_FORMATS, (uint8_t*)&DiscoveryParameters.tokenFormats, 2/*sizeof(DiscoveryParameters.tokenFormats)*/);
  add_tlv(CERT_FORMATS, (uint8_t*)&DiscoveryParameters.certifFormats, 2/*sizeof(DiscoveryParameters.certifFormats)*/);
  add_tlv(CRYPTOSYSTEMS, (uint8_t*)&DiscoveryParameters.cryptosystems, sizeof(DiscoveryParameters.cryptosystems));
  uint32_t st_password = 0xeaeaeaea;
  add_tlv(PSA_ST_PASSWD, (uint8_t*)&st_password, 4);
  //add_tlv(&DiscoveryParameters.provIntegStatus, sizeof(DiscoveryParameters.provIntegStatus));
  //add_tlv((uint32_t*)DiscoveryParameters.provIntegMessage, strlen(DiscoveryParameters.provIntegMessage) + 1);
  add_tlv(HDPL1_STATUS, (uint8_t*)&DiscoveryParameters.hdpl1Status, sizeof(DiscoveryParameters.hdpl1Status));
  add_tlv(HDPL2_STATUS, (uint8_t*)&DiscoveryParameters.hdpl2Status, sizeof(DiscoveryParameters.hdpl2Status));
  add_tlv(HDPL3_STATUS, (uint8_t*)&DiscoveryParameters.hdpl3Status, sizeof(DiscoveryParameters.hdpl3Status));
  //add_tlv(&DiscoveryParameters.Regfeatures, sizeof(DiscoveryParameters.Regfeatures));
  //add_tlv(&DiscoveryParameters.FeaturesNumber, sizeof(DiscoveryParameters.FeaturesNumber));
  
  rep.data_count = (byte_count + 5)/4; 
  send_message((uint32_t*)&rep, 8, DA_TIMEOUT);
  DiscoveryParameters.targetID = DBGMCU->IDCODE;

  return send_message((uint32_t*)buff, byte_count + 5, DA_TIMEOUT);
}

static void add_tlv(uint16_t id, uint8_t *buffer, uint32_t length_in_bytes)
{
  psa_tlv_t psa_tlv = {0};

  psa_tlv.type_id = id;
  psa_tlv.length_in_bytes = length_in_bytes;
  memcpy(psa_tlv.value, buffer, length_in_bytes);
  memcpy(&buff[byte_count], &psa_tlv, 8 + length_in_bytes);
  byte_count += 8 + length_in_bytes;
}

/* Send a message. */
static HAL_StatusTypeDef send_message(uint32_t *message, uint32_t length, uint32_t timeout)
{
  uint32_t word_count = length / sizeof(uint32_t);
  uint32_t word_offset = 0U;
  uint32_t tick_start = HAL_GetTick();

  while (word_offset < word_count)
  {
    /* Write a word */
    DBGMCU_DBG_AUTH_DEVICE_WRITE(message[word_offset++]);

    /* Wait reading of DBG_AUTH_DEV */
    while (DBGMCU_DBG_AUTH_DEVICE_READ_ACK() != 0U)
    {
      /* Wait data reading from HOST */
      if (HAL_GetTick() - tick_start > timeout)
      {
        return HAL_TIMEOUT;
      }
    }
  }

  return HAL_OK;
}

/* Receive a message */
static HAL_StatusTypeDef receive_message(uint32_t *buffer, uint32_t length, uint32_t timeout)
{
  uint32_t max_words = length / sizeof(uint32_t);
  uint32_t word_offset = 0U;
  HAL_StatusTypeDef status = HAL_ERROR;

  while (word_offset < max_words)
  {
    /* Read incoming word */
    uint32_t word = 0xFF;
    status = receive_word(&word, timeout);
    if (status != HAL_OK)
    {
      return status;
    }
    buffer[word_offset++] = word;
  }

  return HAL_OK;
}

/* Receive a word */
static HAL_StatusTypeDef receive_word(uint32_t *word, uint32_t timeout)
{
  uint32_t tick_start = HAL_GetTick();
  
  *word = 0;

  /* Read Host ACK */
  while (DBGMCU_DBG_AUTH_HOST_READ_ACK() != 1U)
  {
    /* Wait data availability */
    if (HAL_GetTick() - tick_start > timeout)
    {
      return HAL_TIMEOUT;
    }
  }

  *word = DBGMCU_DBG_AUTH_HOST_READ();
  DBGMCU_DBG_AUTH_HOST_WRITE_ACK();

  return HAL_OK;
}

static void DBGMCU_DBG_AUTH_DEVICE_WRITE(uint32_t message)
{
  WRITE_REG(DBGMCU->DBG_AUTH_DEV, message);
}

static uint32_t DBGMCU_DBG_AUTH_HOST_READ(void)
{
  return READ_REG(DBGMCU->DBG_AUTH_HOST);
}

static uint32_t DBGMCU_DBG_AUTH_DEVICE_READ_ACK(void)
{
  return ((READ_BIT(DBGMCU->DBG_AUTH_ACK, DBGMCU_DBG_AUTH_ACK_DEVICE_ACK) == DBGMCU_DBG_AUTH_ACK_DEVICE_ACK) ? 1UL : 0UL);
}

static void DBGMCU_DBG_AUTH_HOST_WRITE_ACK(void)
{
  CLEAR_BIT(DBGMCU->DBG_AUTH_ACK, DBGMCU_DBG_AUTH_ACK_HOST_ACK);
}

static uint32_t DBGMCU_DBG_AUTH_HOST_READ_ACK(void)
{
  return ((READ_BIT(DBGMCU->DBG_AUTH_ACK, DBGMCU_DBG_AUTH_ACK_HOST_ACK) == DBGMCU_DBG_AUTH_ACK_HOST_ACK) ? 1UL : 0UL);
}

static void DBGMCU_DBG_ENABLE(void)
{
  __HAL_RCC_DBGMCU_CLK_ENABLE();
}
