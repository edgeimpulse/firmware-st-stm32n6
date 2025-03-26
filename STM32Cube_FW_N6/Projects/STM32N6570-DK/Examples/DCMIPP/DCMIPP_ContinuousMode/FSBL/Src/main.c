/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

DCMIPP_HandleTypeDef phdcmipp;
LTDC_HandleTypeDef phltdc;

/* USER CODE BEGIN PV */
static __IO uint32_t NbMainFrames = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_DCMIPP_Init(void);
static void LCD_Init(uint32_t Width, uint32_t Height);
/* USER CODE BEGIN PFP */
static void OV5640_Probe(uint32_t Resolution, uint32_t PixelFormat);
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
  /* USER CODE END 1 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_DCMIPP_Init();

  /* Initialize the OV5640 Sensor ----------------------------- */
  OV5640_Probe(OV5640_R800x480, OV5640_RGB565);

  /* USER CODE BEGIN 2 */
  LCD_Init(FRAME_WIDTH, FRAME_HEIGHT);

  if (HAL_DCMIPP_CSI_PIPE_Start(&phdcmipp, DCMIPP_PIPE1, DCMIPP_VIRTUAL_CHANNEL0 , BUFFER_ADDRESS, DCMIPP_MODE_CONTINUOUS) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    while(NbMainFrames != 1);

    NbMainFrames = 0;
    BSP_LED_Toggle(LED_GREEN);
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
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /** Configure the System Power Supply */
  if (HAL_PWREx_ConfigSupply(PWR_EXTERNAL_SOURCE_SUPPLY ) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable HSI
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_NONE;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Get current CPU/System buses clocks configuration and
 if necessary switch to intermediate HSI clock to ensure target clock can be set
  */
  HAL_RCC_GetClockConfig(&RCC_ClkInitStruct);
  if((RCC_ClkInitStruct.CPUCLKSource == RCC_CPUCLKSOURCE_IC1) ||
     (RCC_ClkInitStruct.SYSCLKSource == RCC_SYSCLKSOURCE_IC2_IC6_IC11))
  {
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_CPUCLK|RCC_CLOCKTYPE_SYSCLK);
    RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_HSI;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
    {
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
  RCC_ClkInitStruct.IC2Selection.ClockDivider = 4;
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
  * @brief DCMIPP Initialization Function
  * @param None
  * @retval None
  */
static void MX_DCMIPP_Init(void)
{
  /* USER CODE BEGIN DCMIPP_Init 0 */
  /* USER CODE END DCMIPP_Init 0 */
  DCMIPP_CSI_PIPE_ConfTypeDef pCSIPipeConf = {0};
  DCMIPP_PipeConfTypeDef PipeConfig = {0};
  DCMIPP_CSI_ConfTypeDef csiconf = {0};

  /* USER CODE BEGIN DCMIPP_Init 1 */
  /* USER CODE END DCMIPP_Init 1 */
  phdcmipp.Instance = DCMIPP;

  if (HAL_DCMIPP_Init(&phdcmipp) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure the CSI */
  csiconf.DataLaneMapping = DCMIPP_CSI_PHYSICAL_DATA_LANES;
  csiconf.NumberOfLanes   = DCMIPP_CSI_TWO_DATA_LANES;
  csiconf.PHYBitrate      = DCMIPP_CSI_PHY_BT_250;
  HAL_DCMIPP_CSI_SetConfig(&phdcmipp, &csiconf);

  /* Set Virtual Channel config */
  HAL_DCMIPP_CSI_SetVCConfig(&phdcmipp, DCMIPP_VIRTUAL_CHANNEL0, DCMIPP_CSI_DT_BPP8);

  /* Pipe 1 Config */
  pCSIPipeConf.DataTypeMode = DCMIPP_DTMODE_DTIDA;
  pCSIPipeConf.DataTypeIDA  = DCMIPP_DT_RGB565;
  pCSIPipeConf.DataTypeIDB  = DCMIPP_DT_RGB565;
  HAL_DCMIPP_CSI_PIPE_SetConfig(&phdcmipp, DCMIPP_PIPE1, &pCSIPipeConf);

  PipeConfig.FrameRate = DCMIPP_FRAME_RATE_ALL;
  PipeConfig.PixelPackerFormat = DCMIPP_PIXEL_PACKER_FORMAT_RGB565_1;
  PipeConfig.PixelPipePitch = FRAME_WIDTH * 2; /* Number of bytes per Line */
  if (HAL_DCMIPP_PIPE_SetConfig(&phdcmipp, DCMIPP_PIPE1, &PipeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DCMIPP_Init 2 */
  /* USER CODE END DCMIPP_Init 2 */
}

static void LCD_Init(uint32_t Width, uint32_t Height)
{
  LTDC_LayerCfgTypeDef pLayerCfg ={0};

  phltdc.Instance = LTDC;

  phltdc.Instance = LTDC;
  phltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  phltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  phltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  phltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

  phltdc.Init.HorizontalSync     = RK050HR18_HSYNC - 1;
  phltdc.Init.AccumulatedHBP     = RK050HR18_HSYNC + RK050HR18_HBP - 1;
  phltdc.Init.AccumulatedActiveW = RK050HR18_HSYNC + Width + RK050HR18_HBP -1;
  phltdc.Init.TotalWidth         = RK050HR18_HSYNC + Width + RK050HR18_HBP + RK050HR18_HFP - 1;
  phltdc.Init.VerticalSync       = RK050HR18_VSYNC - 1;
  phltdc.Init.AccumulatedVBP     = RK050HR18_VSYNC + RK050HR18_VBP - 1;
  phltdc.Init.AccumulatedActiveH = RK050HR18_VSYNC + Height + RK050HR18_VBP -1 ;
  phltdc.Init.TotalHeigh         = RK050HR18_VSYNC + Height + RK050HR18_VBP + RK050HR18_VFP - 1;

  phltdc.Init.Backcolor.Blue  = 0x0;
  phltdc.Init.Backcolor.Green = 0xFF;
  phltdc.Init.Backcolor.Red   = 0x0;

  if(HAL_LTDC_Init(&phltdc) != HAL_OK)
  {
    Error_Handler();
  }

  pLayerCfg.WindowX0       = 0;
  pLayerCfg.WindowX1       = Width;
  pLayerCfg.WindowY0       = 0;
  pLayerCfg.WindowY1       = Height;
  pLayerCfg.PixelFormat    = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.FBStartAdress  = BUFFER_ADDRESS;
  pLayerCfg.Alpha = LTDC_LxCACR_CONSTA;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg.ImageWidth = Width;
  pLayerCfg.ImageHeight = Height;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if(HAL_LTDC_ConfigLayer(&phltdc, &pLayerCfg, LTDC_LAYER_1))
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Register Bus IOs if component ID is OK
  * @retval error status
  */
static void OV5640_Probe(uint32_t Resolution, uint32_t PixelFormat)
{
  OV5640_IO_t              IOCtx;
  uint32_t                 id = 0;
  static OV5640_Object_t   OV5640Obj;

  /* Configure the camera driver */
  IOCtx.Address     = CAMERA_OV5640_ADDRESS;
  IOCtx.Init        = BSP_I2C2_Init;
  IOCtx.DeInit      = BSP_I2C2_DeInit;
  IOCtx.ReadReg     = BSP_I2C2_ReadReg16;
  IOCtx.WriteReg    = BSP_I2C2_WriteReg16;
  IOCtx.GetTick     = BSP_GetTick;

  if (OV5640_RegisterBusIO(&OV5640Obj, &IOCtx) != OV5640_OK)
  {
    Error_Handler();
  }

  /* Read Sensor ID */
  if (OV5640_ReadID(&OV5640Obj, &id) != OV5640_OK)
  {
    Error_Handler();
  }

  /* Check Senodr ID correctness */
  if (id == OV5640_ID)
  {
    /* Configure the sensor in serial Mode with Virtual Channel 0 */
    OV5640Obj.Mode = SERIAL_MODE;
    OV5640Obj.VirtualChannelID = 0U;

    if(OV5640_Init(&OV5640Obj, Resolution, PixelFormat) != OV5640_OK)
    {
      Error_Handler();
    }
  }
  else
  {
    Error_Handler();
  }
}

void HAL_DCMIPP_PIPE_FrameEventCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe)
{
  NbMainFrames++;
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
    HAL_Delay(250);
    BSP_LED_Toggle(LED_RED);
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
