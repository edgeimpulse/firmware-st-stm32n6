/**
******************************************************************************
* @file    main.c
* @author  MCD Application Team
* @brief   This project is a HAL template project for STM32N6xx devices.
******************************************************************************
* @attention
*
* Copyright (c) 2023 STMicroelectronics.
* All rights reserved.
*
* This software is licensed under terms that can be found in the LICENSE file
* in the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
#include "ewl.h"
#include "H264EncApi.h"
#include "venc_app.h"
#include "stlogo.h"
#include "imx335.h"
#include "stm32n6xx_ll_venc.h"
#include "stm32n6570_discovery.h"
#include "stm32n6570_discovery_lcd.h"
#include "stm32n6570_discovery_camera.h"
#include "tx_api.h"
#include "nx_api.h"
#include "app_rtsp_over_rtp.h"
#include "app_netxduo.h"
#include "stm32_lcd.h"

/** @addtogroup Templates
* @{
*/

/** @addtogroup HAL
* @{
*/

/* Private typedef -----------------------------------------------------------*/
typedef struct {
  uint32_t size;
  uint32_t * block_addr;
  uint32_t * aligned_block_addr;
} venc_output_frame_t;
/* Private define ------------------------------------------------------------*/

#define FRAMERATE 20
#define VENC_APP_QUEUE_SIZE 15
#define INPUT_FRAME_BUFFER_ADDRESS 0x34050000UL
#define VENC_OUTPUT_BLOCK_SIZE 0x20000
#define VENC_OUTPUT_BLOCK_NBR 10
/* Private macro -------------------------------------------------------------*/

#define ALIGNED(ptr, bytes) (((ptr + bytes - 1)/bytes) * bytes)
/* Private variables ---------------------------------------------------------*/


static H264EncIn encIn= {0};
static H264EncOut encOut= {0};
static H264EncInst encoder= {0};
static H264EncConfig cfg= {0};
static uint32_t frame_nb = 0;
venc_output_frame_t queue_buf[VENC_OUTPUT_BLOCK_NBR];
uint8_t output_block_buffer[VENC_OUTPUT_BLOCK_NBR * VENC_OUTPUT_BLOCK_SIZE] __NON_CACHEABLE __attribute((aligned(8)));
TX_EVENT_FLAGS_GROUP venc_app_flags;
TX_QUEUE enc_frame_queue;
TX_BLOCK_POOL venc_block_pool;

/* Private function prototypes -----------------------------------------------*/
static int encoder_prepare(uint32_t width, uint32_t height);
static int encode_frame(void);
static int encoder_end(void);
static int encoder_start(void);

void venc_thread_func(ULONG arg){
  int err;
  ULONG flags;
  uint32_t x_size;
  uint32_t y_size;

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);
  /* initialize LEDs to signal processing is ongoing */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);

  if(tx_event_flags_create(&venc_app_flags, "venc_app_events") != TX_SUCCESS)
  {
    return ;
  }
  if(tx_queue_create(&enc_frame_queue, "ENC frame queue", sizeof(venc_output_frame_t)/4, &queue_buf, sizeof(queue_buf)) != TX_SUCCESS){
    Error_Handler();
  }


   /* Initialize camera */
  if(BSP_CAMERA_Init(0,CAMERA_R2592x1944, CAMERA_PF_RAW_RGGB10) != BSP_ERROR_NONE){
    Error_Handler();
  }
 /* Initialize LCD */
  err = BSP_LCD_InitEx(0, LCD_ORIENTATION_LANDSCAPE, LCD_PIXEL_FORMAT_RGB565, LCD_DEFAULT_WIDTH, LCD_DEFAULT_HEIGHT);
  if(err){
   // printf("error initializing LCD : %d\n", err);
    Error_Handler();
  }
  UTIL_LCD_SetFuncDriver(&LCD_Driver);
  BSP_LCD_SetLayerAddress(0, 0, LCD_LAYER_0_ADDRESS);
  BSP_LCD_FillRect(0, 0, 0, 800, 480, LCD_COLOR_RGB565_CYAN);
  BSP_LCD_DrawBitmap(0, 0, 0, stlogo);
  IMX335_SetFramerate(Camera_CompObj, 20);
  /* initialize VENC */
  LL_VENC_Init();

  /* initialization done. Turn on the LEDs */
  BSP_LED_On(LED1);
  BSP_LED_On(LED2);

  encoder_prepare(800, 480);

  tx_event_flags_get(&venc_app_flags, VIDEO_START_FLAG, TX_AND_CLEAR, &flags, TX_WAIT_FOREVER);
  while(1){
    tx_event_flags_get(&venc_app_flags, FRAME_RECEIVED_FLAG, TX_AND_CLEAR, &flags, TX_WAIT_FOREVER);
    if(BSP_CAMERA_BackgroundProcess() != BSP_ERROR_NONE)
    {
      printf("Error in BSP image processing\n");
    }
    if(encode_frame()){
      printf("error encoding frame\n");
    }else
    {
      BSP_LED_Toggle(LED_GREEN);
      tx_event_flags_set(&demo_test_events, DEMO_VIDEO_DATA_READY_EVENT, TX_OR);
    }
  }
}

int encoder_prepare(uint32_t width, uint32_t height)
{
  H264EncRet ret;
  H264EncPreProcessingCfg preproc_cfg = {0};
  H264EncRateCtrl ratectrl_cfg = {0};

  /* software workaround for Linemult triggering VENC interrupt. Make it happen as little as possible */
  MODIFY_REG(DCMIPP->P1PPCR, DCMIPP_P1PPCR_LINEMULT_Msk,DCMIPP_MULTILINE_128_LINES);

  frame_nb = 0;
  /* Step 1: Initialize an encoder instance */
  /* set config to 1 ref frame */
  cfg.refFrameAmount = 1;
  /* 30 fps frame rate */
  cfg.frameRateDenom = 1;
  cfg.frameRateNum = FRAMERATE;
  /* Image resolution */
  cfg.width = width;
  cfg.height = height;
  /* Stream type */
  cfg.streamType = H264ENC_BYTE_STREAM;

  /* encoding level*/
  /*See API guide for level depending on resolution and framerate*/
  cfg.level = H264ENC_LEVEL_2_2;
  cfg.svctLevel = 0;

  ret = H264EncInit(&cfg, &encoder);
  if (ret != H264ENC_OK)
  {
    return -1;
  }

  /* set format conversion for preprocessing */
  ret = H264EncGetPreProcessing(encoder, &preproc_cfg);
  if(ret != H264ENC_OK){
    return -1;
  }
  preproc_cfg.inputType = H264ENC_RGB565;
  ret = H264EncSetPreProcessing(encoder, &preproc_cfg);
  if(ret != H264ENC_OK){
    return -1;
  }

  /* set bit rate configuration */
  ret = H264EncGetRateCtrl(encoder, &ratectrl_cfg);
  if(ret != H264ENC_OK){
    return -1;
  }
  ratectrl_cfg.qpHdr = 25;
  ratectrl_cfg.bitPerSecond = 1000000;
  ratectrl_cfg.pictureRc = 0;
  ratectrl_cfg.gopLen = 30;
  ratectrl_cfg.intraQpDelta = 0;
  ratectrl_cfg.fixedIntraQp = 0;
  ratectrl_cfg.hrd = 0;
  ret = H264EncSetRateCtrl(encoder, &ratectrl_cfg);
  if(ret != H264ENC_OK){
    return -1;
  }
  return 0;
}

static int encoder_start(void)
{
  H264EncRet ret;
  venc_output_frame_t frame_buffer = {0};
 /* software workaround for Linemult triggering VENC interrupt. Make it happen as little as possible */
  MODIFY_REG(DCMIPP->P1PPCR, DCMIPP_P1PPCR_LINEMULT_Msk,DCMIPP_MULTILINE_128_LINES);

  /* start camera acquisition */
  if(BSP_CAMERA_Start(0, (uint8_t *)(INPUT_FRAME_BUFFER_ADDRESS), CAMERA_MODE_CONTINUOUS)!= BSP_ERROR_NONE){
    Error_Handler();
  }

  if(tx_block_pool_create(&venc_block_pool, "venc output bock pool", VENC_OUTPUT_BLOCK_SIZE,
                          output_block_buffer, sizeof(output_block_buffer)) != TX_SUCCESS){
    Error_Handler();
  }

  if(tx_block_allocate(&venc_block_pool, (void **) &frame_buffer.block_addr, TX_WAIT_FOREVER) != TX_SUCCESS){
    return -1;
  }
  frame_buffer.aligned_block_addr = (uint32_t *) ALIGNED((uint32_t) frame_buffer.block_addr, 8);
  encIn.pOutBuf = frame_buffer.aligned_block_addr;
  encIn.busOutBuf = (uint32_t) encIn.pOutBuf;
  encIn.outBufSize = VENC_OUTPUT_BLOCK_SIZE;

  /* create stream */
  ret = H264EncStrmStart(encoder, &encIn, &encOut);
  if (ret != H264ENC_OK)
  {
    return -1;
  }
  frame_buffer.size = encOut.streamSize;
  if(tx_queue_send(&enc_frame_queue, (void *) &frame_buffer, TX_NO_WAIT) != TX_SUCCESS){
    tx_block_release(frame_buffer.block_addr);
  }
  encIn.codingType = H264ENC_INTRA_FRAME;
  return 0;
}


static int encode_frame(void)
{
  venc_output_frame_t frame_buffer = {0};
  int ret = H264ENC_FRAME_READY;
  if (!(frame_nb % 30))
  {
    /* if frame is the first : set as intra coded */
    encIn.codingType = H264ENC_INTRA_FRAME;
  }
  else
  {
    /* if there was a frame previously, set as predicted */
    encIn.timeIncrement = 1;
    encIn.codingType = H264ENC_PREDICTED_FRAME;
  }
  encIn.ipf = H264ENC_REFERENCE_AND_REFRESH;
  encIn.ltrf = H264ENC_REFERENCE;
  /* set input buffers to structures */
  encIn.busLuma = INPUT_FRAME_BUFFER_ADDRESS;

  /* allocate and set output buffer */

  if(tx_block_allocate(&venc_block_pool, (void **) &frame_buffer.block_addr, TX_WAIT_FOREVER) != TX_SUCCESS){
    return -1;
  }
  frame_buffer.aligned_block_addr = (uint32_t *) ALIGNED((uint32_t) frame_buffer.block_addr, 8);
  encIn.pOutBuf = frame_buffer.aligned_block_addr;
  encIn.busOutBuf = (uint32_t) encIn.pOutBuf;
  encIn.outBufSize = VENC_OUTPUT_BLOCK_SIZE - 8;
  ret = H264EncStrmEncode(encoder, &encIn, &encOut, NULL, NULL, NULL);
  switch (ret)
  {
  case H264ENC_FRAME_READY:
    /*save stream */
    if(encOut.streamSize == 0){
      encIn.codingType = H264ENC_INTRA_FRAME;
      tx_block_release(frame_buffer.block_addr);
      return -1;
    }
    frame_buffer.size = encOut.streamSize;
    if(tx_queue_send(&enc_frame_queue, (void *) &frame_buffer, TX_NO_WAIT) != TX_SUCCESS){
      tx_block_release(frame_buffer.block_addr);
    }
    encIn.codingType = H264ENC_PREDICTED_FRAME;
    break;
  default:
    tx_block_release(encIn.pOutBuf);
    encIn.codingType = H264ENC_INTRA_FRAME;
    return -1;
    break;
  }
  frame_nb++;
  return 0;
}


static int encoder_end(void){
  BSP_CAMERA_Stop(0);
  int ret = H264EncStrmEnd(encoder, &encIn, &encOut);
  if (ret != H264ENC_OK)
  {
    return -1;
  }
  tx_queue_flush(&enc_frame_queue);
  tx_block_pool_delete(&venc_block_pool);
  return 0;
}


HAL_StatusTypeDef MX_DCMIPP_ClockConfig(DCMIPP_HandleTypeDef *hdcmipp)
{
 RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /* Configure DCMIPP ck_ker_dcmipp to ic17 with PLL2 (1000MHz) / 3 = 333MHz */
  PeriphClkInit.PeriphClockSelection |= RCC_PERIPHCLK_DCMIPP;
  PeriphClkInit.DcmippClockSelection = RCC_DCMIPPCLKSOURCE_IC17;
  PeriphClkInit.ICSelection[RCC_IC17].ClockSelection = RCC_ICCLKSOURCE_PLL1;
  PeriphClkInit.ICSelection[RCC_IC17].ClockDivider = 4;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    return HAL_ERROR;
  }
  LL_RCC_IC18_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
  LL_RCC_IC18_SetDivider(60);   /* 800/40=20Mhz */
  LL_RCC_IC18_Enable();

  return HAL_OK;
}

void BSP_CAMERA_FrameEventCallback(uint32_t instance)
{
  /* signal new frame*/
  tx_event_flags_set(&venc_app_flags, FRAME_RECEIVED_FLAG, TX_OR);
}

HAL_StatusTypeDef MX_LTDC_ClockConfig(LTDC_HandleTypeDef *hltdc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hltdc);

  HAL_StatusTypeDef status = HAL_OK;
  RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct = {0};

  RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  RCC_PeriphCLKInitStruct.LtdcClockSelection = RCC_LTDCCLKSOURCE_IC16;
  RCC_PeriphCLKInitStruct.ICSelection[RCC_IC16].ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_PeriphCLKInitStruct.ICSelection[RCC_IC16].ClockDivider = 30;
  if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct) != HAL_OK)
  {
    status = HAL_ERROR;
  }

  return status;
}

/* Functions. */
void VENC_APP_EncodingStart(void)
{
  /* initialize encoder software for camera feed encoding */
  encoder_start();
  tx_event_flags_set(&venc_app_flags, VIDEO_START_FLAG, TX_OR);
}

UINT VENC_APP_GetData(UCHAR **data, ULONG *size)
{
  static uint32_t * curr_block = NULL;
  if(curr_block){
    tx_block_release(curr_block);
    curr_block = NULL;
  }
  venc_output_frame_t frame_block;
  if(tx_queue_receive(&enc_frame_queue, (void *) &frame_block, TX_NO_WAIT) != TX_SUCCESS){
    *data = NULL;
    *size = 0;
    return(NX_PTR_ERROR);
  }
  *data = (UCHAR *) frame_block.aligned_block_addr;
  *size = frame_block.size + 15;
  curr_block = frame_block.block_addr;
  return(NX_SUCCESS);
}

UINT VENC_APP_EncodingStop(void)
{
  char IP_addr_str[140];
  (void) encoder_end();
  BSP_LCD_FillRect(0, 0, 0, 800, 480, LCD_COLOR_RGB565_CYAN);
  BSP_LCD_DrawBitmap(0, 0, 0, stlogo);
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
  return(NX_SUCCESS);
}
