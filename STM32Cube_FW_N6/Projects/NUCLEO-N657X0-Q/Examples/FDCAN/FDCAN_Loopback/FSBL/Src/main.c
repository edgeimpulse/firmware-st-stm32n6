/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FDCAN/FDCAN_Loopback/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to configure the FDCAN to adapt to
  *          different CAN bit rates using restricted mode.
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TX_TIMEOUT               (1000U)   /* Transmission timeout in ms           */

/* Hardware related, can't be changed */
#define NB_RX_FIFO               (64U)      /* Number of RX FIFO Elements available */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define COUNTOF(BUFFER) (sizeof((BUFFER)) / sizeof(*(BUFFER)))
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

FDCAN_HandleTypeDef hfdcan1;

/* USER CODE BEGIN PV */
FDCAN_RxHeaderTypeDef rxHeader;
uint8_t               rxData[12U];
static const uint8_t  txData0[] = {0x10, 0x32, 0x54, 0x76, 0x98, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
static const uint8_t  txData1[] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
static const uint8_t  txData2[] = {0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00};


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_FDCAN1_Init(void);
/* USER CODE BEGIN PFP */

static uint32_t BufferCmp8b(const uint8_t *pBuffer1, const uint8_t *pBuffer2, uint16_t BufferLength);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */


  /* STM32N6xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  /* USER CODE END 1 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Configure LED1 and LED2 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FDCAN1_Init();
  /* USER CODE BEGIN 2 */

  /*##-1 Configure the FDCAN filters ########################################*/

  /* Configure standard ID reception filter to Rx FIFO 0. Only accept ID = FilterID1 */
  FDCAN_FilterTypeDef        sFilterConfig;
  sFilterConfig.IdType       = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex  = 0U;
  sFilterConfig.FilterType   = FDCAN_FILTER_DUAL;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterID1    = 0x444;
  sFilterConfig.FilterID2    = 0x444; /* For acceptance, MessageID and FilterID1 must match exactly */
  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure extended ID reception filter to Rx FIFO 1. Only accept ID between FilterID1 and FilterID2. */
  sFilterConfig.IdType       = FDCAN_EXTENDED_ID;
  sFilterConfig.FilterIndex  = 0U;
  sFilterConfig.FilterType   = FDCAN_FILTER_RANGE_NO_EIDM;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
  sFilterConfig.FilterID1    = 0x1111111;
  sFilterConfig.FilterID2    = 0x2222222;
  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /**
    *  Configure global filter:
    *    - Filter all remote frames with STD and EXT ID
    *    - Reject non matching frames with STD ID and EXT ID
    */
  if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,
                                   FDCAN_REJECT, FDCAN_REJECT,
                                   FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
  {
    Error_Handler();
  }

  /*##-2 Start FDCAN controller (continuous listening CAN bus) ##############*/

  if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }

  /*##-3 Transmit messages ##################################################*/

  FDCAN_TxHeaderTypeDef        txHeader;

  /* Add message to Tx FIFO */
  txHeader.Identifier          = 0x444;
  txHeader.IdType              = FDCAN_STANDARD_ID;
  txHeader.TxFrameType         = FDCAN_DATA_FRAME;
  txHeader.DataLength          = FDCAN_DLC_BYTES_12;
  txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txHeader.BitRateSwitch       = FDCAN_BRS_ON;
  txHeader.FDFormat            = FDCAN_FD_CAN;
  txHeader.TxEventFifoControl  = FDCAN_STORE_TX_EVENTS;
  txHeader.MessageMarker       = 0x52U;
  if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, txData0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Add second message to Tx FIFO */
  txHeader.Identifier          = 0x1111112;
  txHeader.IdType              = FDCAN_EXTENDED_ID;
  txHeader.TxFrameType         = FDCAN_DATA_FRAME;
  txHeader.DataLength          = FDCAN_DLC_BYTES_12;
  txHeader.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
  txHeader.BitRateSwitch       = FDCAN_BRS_ON;
  txHeader.FDFormat            = FDCAN_FD_CAN;
  txHeader.TxEventFifoControl  = FDCAN_STORE_TX_EVENTS;
  txHeader.MessageMarker       = 0xCCU;
  if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, txData1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Add third message to Tx FIFO */
  txHeader.Identifier          = 0x1111113;
  txHeader.IdType              = FDCAN_EXTENDED_ID;
  txHeader.TxFrameType         = FDCAN_DATA_FRAME;
  txHeader.DataLength          = FDCAN_DLC_BYTES_12;
  txHeader.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
  txHeader.BitRateSwitch       = FDCAN_BRS_OFF;
  txHeader.FDFormat            = FDCAN_FD_CAN;
  txHeader.TxEventFifoControl  = FDCAN_STORE_TX_EVENTS;
  txHeader.MessageMarker       = 0xDDU;
  if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, txData2) != HAL_OK)
  {
    Error_Handler();
  }

  /* Get tick */
  uint32_t tickstart = HAL_GetTick();

  /* Wait transmission complete */
 while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) != hfdcan1.Init.TxFifoQueueElmtsNbr)
  {
    /* Timeout handling */
    if ((HAL_GetTick() - tickstart) > TX_TIMEOUT)
    {
      Error_Handler();
    }
  }

  /*##-4 Receive messages ###################################################*/

  /* Check one message is received in Rx FIFO 0 */
  if (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0) != 1U)
  {
    Error_Handler();
  }

  /* Retrieve message from Rx FIFO 0 */
  if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &rxHeader, rxData) != HAL_OK)
  {
    Error_Handler();
  }

  /* Compare received RX message to expected data */
  if ((rxHeader.Identifier != 0x444) ||
      (rxHeader.IdType     != FDCAN_STANDARD_ID) ||
      (rxHeader.DataLength != FDCAN_DLC_BYTES_12) ||
      (BufferCmp8b(txData0, rxData, COUNTOF(rxData)) != 0U))
  {
    Error_Handler();
  }

  /* Check two messages are received in Rx FIFO 1 */
  if (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO1) != 2U)
  {
    Error_Handler();
  }

  /* Retrieve message from Rx FIFO 1 */
  if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO1, &rxHeader, rxData) != HAL_OK)
  {
    Error_Handler();
  }

  /* Compare received RX message to expected data */
  if ((rxHeader.Identifier != 0x1111112) ||
      (rxHeader.IdType     != FDCAN_EXTENDED_ID) ||
      (rxHeader.DataLength != FDCAN_DLC_BYTES_12) ||
      (BufferCmp8b(txData1, rxData, COUNTOF(rxData)) != 0U))
  {
    Error_Handler();
  }

  /* Retrieve next message from Rx FIFO 1 */
  if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO1, &rxHeader, rxData) != HAL_OK)
  {
    Error_Handler();
  }

  /* Compare received RX message to expected data */
  if ((rxHeader.Identifier != 0x1111113) ||
      (rxHeader.IdType     != FDCAN_EXTENDED_ID) ||
      (rxHeader.DataLength != FDCAN_DLC_BYTES_12) ||
      (BufferCmp8b(txData2, rxData, COUNTOF(rxData)) != 0U))
  {
    Error_Handler();
  }

  /* Execution success. Turn ON LED1 */
  BSP_LED_On(LED1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

/** Configure the System Power Supply
  */
  if (HAL_PWREx_ConfigSupply(PWR_EXTERNAL_SOURCE_SUPPLY ) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable HSI */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Get current CPU/System buses clocks configuration and if necessary switch
  * to intermediate HSI clock to ensure target clock can be set
  */
  HAL_RCC_GetClockConfig(&RCC_ClkInitStruct);
  if ((RCC_ClkInitStruct.CPUCLKSource == RCC_CPUCLKSOURCE_IC1) ||
      (RCC_ClkInitStruct.SYSCLKSource == RCC_SYSCLKSOURCE_IC2_IC6_IC11))
  {
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_SYSCLK);
    RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_HSI;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
    {
      /* Initialization Error */
      Error_Handler();
    }
  }

/** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL1.PLLM = 4;
  RCC_OscInitStruct.PLL1.PLLN = 75;
  RCC_OscInitStruct.PLL1.PLLFractional = 0;
  RCC_OscInitStruct.PLL1.PLLP1 = 1;
  RCC_OscInitStruct.PLL1.PLLP2 = 1;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_CPUCLK|RCC_CLOCKTYPE_HCLK
                              |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2|RCC_CLOCKTYPE_PCLK5
                              |RCC_CLOCKTYPE_PCLK4;
  RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_IC1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_IC2_IC6_IC11;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;
  RCC_ClkInitStruct.APB5CLKDivider = RCC_APB5_DIV1;
  RCC_ClkInitStruct.IC1Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC1Selection.ClockDivider = 2;
  RCC_ClkInitStruct.IC2Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC2Selection.ClockDivider = 3;
  RCC_ClkInitStruct.IC6Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC6Selection.ClockDivider = 4;
  RCC_ClkInitStruct.IC11Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC11Selection.ClockDivider = 3;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief FDCAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN1_Init(void)
{
  FDCAN_FilterTypeDef sFilterConfig;
  FDCAN_TxHeaderTypeDef TxHeader;
  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
  hfdcan1.Init.Mode = FDCAN_MODE_EXTERNAL_LOOPBACK;
  hfdcan1.Init.AutoRetransmission = ENABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = ENABLE;
  hfdcan1.Init.NominalPrescaler = 1;
  hfdcan1.Init.NominalSyncJumpWidth = 50;
  hfdcan1.Init.NominalTimeSeg1 = 149;
  hfdcan1.Init.NominalTimeSeg2 = 50;
  hfdcan1.Init.DataPrescaler = 4;
  hfdcan1.Init.DataSyncJumpWidth = 5;
  hfdcan1.Init.DataTimeSeg1 = 19;
  hfdcan1.Init.DataTimeSeg2 = 5;
  hfdcan1.Init.StdFiltersNbr = 1;
  hfdcan1.Init.ExtFiltersNbr = 2;
  hfdcan1.Init.MessageRAMOffset=0;
  hfdcan1.Init.RxFifo0ElmtsNbr=1;
  hfdcan1.Init.RxFifo0ElmtSize=FDCAN_DATA_BYTES_64;
  hfdcan1.Init.RxFifo1ElmtsNbr=2;
  hfdcan1.Init.RxFifo1ElmtSize=FDCAN_DATA_BYTES_48;
  hfdcan1.Init.RxBuffersNbr=1;
  hfdcan1.Init.RxFifo1ElmtSize=FDCAN_DATA_BYTES_12;
  hfdcan1.Init.TxEventsNbr=4;
  hfdcan1.Init.TxBuffersNbr=2;
  hfdcan1.Init.TxFifoQueueElmtsNbr = 3;
  hfdcan1.Init.TxFifoQueueMode=FDCAN_TX_FIFO_OPERATION;
  hfdcan1.Init.TxElmtSize=FDCAN_DATA_BYTES_64;
  HAL_FDCAN_Init(&hfdcan1);

   /* Configure standard ID reception filter to Rx buffer 0 */
  sFilterConfig.IdType = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXBUFFER;
  sFilterConfig.FilterID1 = 0x111;
  sFilterConfig.RxBufferIndex = 0;
  HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig);

  /* Configure extended ID reception filter to Rx FIFO 1 */
  sFilterConfig.IdType = FDCAN_EXTENDED_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = FDCAN_FILTER_RANGE_NO_EIDM;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
  sFilterConfig.FilterID1 = 0x1111111;
  sFilterConfig.FilterID2 = 0x2222222;
  HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig);

  /* Configure Tx buffer message */
  TxHeader.Identifier = 0x111;
  TxHeader.IdType = FDCAN_STANDARD_ID;
  TxHeader.TxFrameType = FDCAN_DATA_FRAME;
  TxHeader.DataLength = FDCAN_DLC_BYTES_12;
  TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  TxHeader.BitRateSwitch = FDCAN_BRS_ON;
  TxHeader.FDFormat = FDCAN_FD_CAN;
  TxHeader.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
  TxHeader.MessageMarker = 0x52;
  HAL_FDCAN_AddMessageToTxBuffer(&hfdcan1, &TxHeader, txData0, FDCAN_TX_BUFFER0);

  /* Start the FDCAN module */
  HAL_FDCAN_Start(&hfdcan1);
  /* * */

  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  /* USER CODE END FDCAN1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


/**
  * @brief  Compares two buffers.
  * @param  pBuffer1 buffer to be compared.
  * @param  pBuffer2 buffer to be compared.
  * @param  BufferLength: buffer's length.
  * @retval 0: pBuffer1 is identical to pBuffer2
  * @retval 1: pBuffer1 differs from pBuffer2
  */
static uint32_t BufferCmp8b(const uint8_t *pBuffer1, const uint8_t *pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1U;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return 0U;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  while (1)
  {
    /* Toggle LED2 on */
    BSP_LED_Toggle(LED2);

    /* 1s delay */
    HAL_Delay(1000U);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
