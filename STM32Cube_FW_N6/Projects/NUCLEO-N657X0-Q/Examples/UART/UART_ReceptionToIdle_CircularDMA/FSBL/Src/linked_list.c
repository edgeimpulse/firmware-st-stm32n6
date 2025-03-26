/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : linked_list.c
  * Description        : This file provides code for the configuration
  *                      of the LinkedList.
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
#include "linked_list.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

DMA_NodeTypeDef pNode_GPDMACH0 __NON_CACHEABLE;
DMA_QListTypeDef pQueueLinkList_GPDMACH0;

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/**
  * @brief  DMA Linked-list pQueueLinkList_GPDMACH0 configuration
  * @param  None
  * @retval None
  */
HAL_StatusTypeDef MX_pQueueLinkList_GPDMACH0_Config(void)
{
  HAL_StatusTypeDef ret = HAL_OK;
  /* DMA node configuration declaration */
  DMA_NodeConfTypeDef pNodeConfig;

  /* Set node configuration ################################################*/
  pNodeConfig.NodeType = DMA_GPDMA_LINEAR_NODE;
  pNodeConfig.SrcSecure = DMA_CHANNEL_SRC_SEC;
  pNodeConfig.DestSecure = DMA_CHANNEL_DEST_SEC;
  pNodeConfig.Init.Request = GPDMA1_REQUEST_USART1_RX;
  pNodeConfig.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
  pNodeConfig.Init.Direction = DMA_PERIPH_TO_MEMORY;
  pNodeConfig.Init.SrcInc = DMA_SINC_FIXED;
  pNodeConfig.Init.DestInc = DMA_DINC_INCREMENTED;
  pNodeConfig.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
  pNodeConfig.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
  pNodeConfig.Init.SrcBurstLength = 1;
  pNodeConfig.Init.DestBurstLength = 1;
  pNodeConfig.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT1|DMA_DEST_ALLOCATED_PORT1;
  pNodeConfig.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
  pNodeConfig.Init.Mode = DMA_NORMAL;
  pNodeConfig.TriggerConfig.TriggerPolarity = DMA_TRIG_POLARITY_MASKED;
  pNodeConfig.TriggerConfig.TriggerSelection = GPDMA1_TRIGGER_GPDMA1_CH0_TCF;
  pNodeConfig.DataHandlingConfig.DataExchange = DMA_EXCHANGE_NONE;
  pNodeConfig.DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;
  pNodeConfig.SrcAddress = 0;
  pNodeConfig.DstAddress = 0;
  pNodeConfig.DataSize = 0;

  /* Build pNode_GPDMACH0 Node */
  ret |= HAL_DMAEx_List_BuildNode(&pNodeConfig, &pNode_GPDMACH0);

  /* Insert pNode_GPDMACH0 to Queue */
  ret |= HAL_DMAEx_List_InsertNode_Tail(&pQueueLinkList_GPDMACH0, &pNode_GPDMACH0);

  ret |= HAL_DMAEx_List_SetCircularMode(&pQueueLinkList_GPDMACH0);

  return ret;
}

