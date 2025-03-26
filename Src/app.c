/**
  ******************************************************************************
  * @file    app.c
  * @author  MDG Application Team
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

#include "app.h"

#include <stdint.h>

#include "app_cam.h"
#include "app_config.h"
#include "objdetect_pp_output_if.h"
#include "isp_api.h"
#include "ll_aton_runtime.h"
#include "cmw_camera.h"
#include "stm32n6570_discovery_lcd.h"
#include "stm32_lcd.h"
#include "stm32_lcd_ex.h"
#include "stm32n6xx_hal.h"
#include "tx_api.h"
#include "utils.h"

extern void ei_main(void);
extern void ei_init(void);
extern int32_t update_score_buffer(postprocess_outBuffer_t *pOutput, int32_t max_objects);

#define MAX_DISPLAY_OBJECTS 64

#define CACHE_OP(__op__) do { \
  if (is_cache_enable()) { \
    __op__; \
  } \
} while (0)

#define NUMBER_COLORS 10
#define BQUEUE_MAX_BUFFERS 2

#define DISPLAY_BUFFER_NB (DISPLAY_DELAY + 2)

typedef struct
{
  uint32_t X0;
  uint32_t Y0;
  uint32_t XSize;
  uint32_t YSize;
} Rectangle_TypeDef;

typedef struct {
  TX_SEMAPHORE free;
  TX_SEMAPHORE ready;
  int buffer_nb;
  uint8_t *buffers[BQUEUE_MAX_BUFFERS];
  int free_idx;
  int ready_idx;
} bqueue_t;

typedef struct {
  int32_t nb_detect;
  postprocess_outBuffer_t detects[MAX_DISPLAY_OBJECTS];

} display_info_t;

typedef struct {
  TX_SEMAPHORE update;
  TX_MUTEX lock;
  display_info_t info;
} display_t;

/* Globals */
// DECLARE_CLASSES_TABLE;
/* Lcd Background area */
static Rectangle_TypeDef lcd_bg_area = {
  .X0 = 0,
  .Y0 = 0,
  .XSize = LCD_BG_WIDTH,
  .YSize = LCD_BG_HEIGHT,
};
/* Lcd Foreground area */
static Rectangle_TypeDef lcd_fg_area = {
  .X0 = 0,
  .Y0 = 0,
  .XSize = LCD_FG_WIDTH,
  .YSize = LCD_FG_HEIGHT,
};
static const uint32_t colors[NUMBER_COLORS] = {
    UTIL_LCD_COLOR_GREEN,
    UTIL_LCD_COLOR_RED,
    UTIL_LCD_COLOR_CYAN,
    UTIL_LCD_COLOR_MAGENTA,
    UTIL_LCD_COLOR_YELLOW,
    UTIL_LCD_COLOR_GRAY,
    UTIL_LCD_COLOR_BLACK,
    UTIL_LCD_COLOR_BROWN,
    UTIL_LCD_COLOR_BLUE,
    UTIL_LCD_COLOR_ORANGE
};
/* Lcd Background Buffer */
static uint8_t lcd_bg_buffer[DISPLAY_BUFFER_NB][LCD_BG_WIDTH * LCD_BG_HEIGHT * 2] ALIGN_32 IN_PSRAM;
static int lcd_bg_buffer_disp_idx = 1;
static int lcd_bg_buffer_capt_idx = 0;
/* Lcd Foreground Buffer */
static uint8_t lcd_fg_buffer[2][LCD_FG_WIDTH * LCD_FG_HEIGHT* 2] ALIGN_32 IN_PSRAM;
static int lcd_fg_buffer_rd_idx;
static display_t disp;

 /* new frame flag */
volatile int32_t cameraFrameReceived;

 /* threads */
  /* nn thread */
static TX_THREAD nn_thread;
static uint8_t nn_tread_stack[4096];
  /* display thread */
static TX_THREAD dp_thread;
static uint8_t dp_tread_stack[4096];
  /* isp thread */
static TX_THREAD isp_thread;
static uint8_t isp_tread_stack[4096];
static TX_SEMAPHORE isp_sem;

static int is_cache_enable()
{
#if defined(USE_DCACHE)
  return 1;
#else
  return 0;
#endif
}

static void app_main_pipe_frame_event()
{
  int next_disp_idx = (lcd_bg_buffer_disp_idx + 1) % DISPLAY_BUFFER_NB;
  int next_capt_idx = (lcd_bg_buffer_capt_idx + 1) % DISPLAY_BUFFER_NB;
  int ret;

  ret = HAL_DCMIPP_PIPE_SetMemoryAddress(CMW_CAMERA_GetDCMIPPHandle(), DCMIPP_PIPE1,
                                         DCMIPP_MEMORY_ADDRESS_0, (uint32_t) lcd_bg_buffer[next_capt_idx]);
  assert(ret == HAL_OK);

  ret = HAL_LTDC_SetAddress_NoReload(&hlcd_ltdc, (uint32_t) lcd_bg_buffer[next_disp_idx], LTDC_LAYER_1);
  assert(ret == HAL_OK);
  ret = HAL_LTDC_ReloadLayer(&hlcd_ltdc, LTDC_RELOAD_VERTICAL_BLANKING, LTDC_LAYER_1);
  assert(ret == HAL_OK);
  lcd_bg_buffer_disp_idx = next_disp_idx;
  lcd_bg_buffer_capt_idx = next_capt_idx;
}

static void app_main_pipe_vsync_event()
{
  int ret;

  ret = tx_semaphore_put(&isp_sem);
  assert(ret == 0);
}

static void LCD_init()
{
  BSP_LCD_LayerConfig_t LayerConfig = {0};

  BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);

  /* Preview layer Init */
  LayerConfig.X0          = lcd_bg_area.X0;
  LayerConfig.Y0          = lcd_bg_area.Y0;
  LayerConfig.X1          = lcd_bg_area.X0 + lcd_bg_area.XSize;
  LayerConfig.Y1          = lcd_bg_area.Y0 + lcd_bg_area.YSize;
  LayerConfig.PixelFormat = LCD_PIXEL_FORMAT_RGB565;
  LayerConfig.Address     = (uint32_t) lcd_bg_buffer[lcd_bg_buffer_disp_idx];

  BSP_LCD_ConfigLayer(0, LTDC_LAYER_1, &LayerConfig);

  LayerConfig.X0 = lcd_fg_area.X0;
  LayerConfig.Y0 = lcd_fg_area.Y0;
  LayerConfig.X1 = lcd_fg_area.X0 + lcd_fg_area.XSize;
  LayerConfig.Y1 = lcd_fg_area.Y0 + lcd_fg_area.YSize;
  LayerConfig.PixelFormat = LCD_PIXEL_FORMAT_ARGB4444;
  LayerConfig.Address = (uint32_t) lcd_fg_buffer[1]; /* External XSPI1 PSRAM */

  BSP_LCD_ConfigLayer(0, LTDC_LAYER_2, &LayerConfig);
  UTIL_LCD_SetFuncDriver(&LCD_Driver);
  UTIL_LCD_SetLayer(LTDC_LAYER_2);
  UTIL_LCD_Clear(0x00000000);
  UTIL_LCD_SetFont(&Font20);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
}

static void Display_DetectionBoxes(postprocess_outBuffer_t *detect)
{
      uint32_t x0 = (uint32_t) detect->x_center * ((float32_t)lcd_bg_area.XSize / (float32_t)detect->nn_width);
      uint32_t y0 = (uint32_t) detect->y_center * ((float32_t)lcd_bg_area.YSize / (float32_t)detect->nn_height);
      uint32_t width = (uint32_t) detect->width * ((float32_t)lcd_bg_area.XSize / (float32_t)detect->nn_width);
      uint32_t height = (uint32_t) detect->height * ((float32_t)lcd_bg_area.YSize / (float32_t)detect->nn_height);

      /* Draw boxes without going outside of the image to avoid clearing the text area to clear the boxes */
      x0 = x0 < lcd_bg_area.X0 + lcd_bg_area.XSize ? x0 : lcd_bg_area.X0 + lcd_bg_area.XSize - 1;
      y0 = y0 < lcd_bg_area.Y0 + lcd_bg_area.YSize ? y0 : lcd_bg_area.Y0 + lcd_bg_area.YSize  - 1;
      width = ((x0 + width) < lcd_bg_area.X0 + lcd_bg_area.XSize) ? width : (lcd_bg_area.X0 + lcd_bg_area.XSize - x0 - 1);
      height = ((y0 + height) < lcd_bg_area.Y0 + lcd_bg_area.YSize) ? height : (lcd_bg_area.Y0 + lcd_bg_area.YSize - y0 - 1);
      UTIL_LCD_DrawRect(x0, y0, width, height, colors[detect->class_index % NUMBER_COLORS]);
      UTIL_LCDEx_PrintfAt(x0, y0, LEFT_MODE, detect->label_pointer);
}

static void Display_DetectionCentroids(postprocess_outBuffer_t *detect)
{
      uint32_t x0 = (uint32_t) detect->x_center * ((float32_t)lcd_bg_area.XSize / (float32_t)detect->nn_width);
      uint32_t y0 = (uint32_t) detect->y_center * ((float32_t)lcd_bg_area.YSize / (float32_t)detect->nn_height);
      uint32_t radius = (uint32_t) 5.f * ((float32_t)lcd_bg_area.XSize / (float32_t)detect->nn_width);

      /* Draw boxes without going outside of the image to avoid clearing the text area to clear the boxes */
      x0 = x0 < lcd_bg_area.X0 + lcd_bg_area.XSize ? x0 : lcd_bg_area.X0 + lcd_bg_area.XSize - 1;
      y0 = y0 < lcd_bg_area.Y0 + lcd_bg_area.YSize ? y0 : lcd_bg_area.Y0 + lcd_bg_area.YSize  - 1;
      radius = ((x0 + radius) < lcd_bg_area.X0 + lcd_bg_area.XSize) ? radius : (lcd_bg_area.X0 + lcd_bg_area.XSize - x0 - 1);
      radius = ((y0 + radius) < lcd_bg_area.Y0 + lcd_bg_area.YSize) ? radius : (lcd_bg_area.Y0 + lcd_bg_area.YSize - y0 - 1);
      UTIL_LCD_DrawCircle(x0, y0, radius, colors[detect->class_index % NUMBER_COLORS]);
      UTIL_LCDEx_PrintfAt(x0, y0 + 10, LEFT_MODE, detect->label_pointer);
}

static void Display_NetworkOutput(display_info_t *info)
{
  postprocess_outBuffer_t *rois = info->detects;
  int32_t nb_rois = info->nb_detect;

  int i;

  /* clear previous ui */
  UTIL_LCD_FillRect(lcd_fg_area.X0, lcd_fg_area.Y0, lcd_fg_area.XSize, lcd_fg_area.YSize, 0x00000000); /* Clear previous boxes */

  if(nb_rois < 0) {
    UTIL_LCDEx_PrintfAt(0, LINE(22), LEFT_MODE, " Inference not running");
  }
  else if(rois->model_type == MODEL_IMAGE_CLASSIFICATION) {
    UTIL_LCDEx_PrintfAt(0, LINE(22), LEFT_MODE, " %s (%.2f)", rois->label_pointer, rois->conf);
  }
  else {
    UTIL_LCDEx_PrintfAt(0, LINE(22), LEFT_MODE, " Objects %u", nb_rois);

    /* Draw bounding boxes */
    for (i = 0; i < nb_rois; i++)
      if(rois[i].model_type == MODEL_OBJECT_DETECTION_BOUNDING_BOXES)
        Display_DetectionBoxes(&rois[i]);
      else
        Display_DetectionCentroids(&rois[i]);
  }
}

static void nn_thread_ei_fct(ULONG arg)
{
  ei_init();

  while (1)
  {
    HAL_Delay(5);
    ei_main();

    /* update display stats and detection info */
    int ret = tx_mutex_get(&disp.lock, TX_NO_WAIT);
    if (ret != TX_NO_INSTANCE) {
      disp.info.nb_detect = update_score_buffer(disp.info.detects, MAX_DISPLAY_OBJECTS);

      tx_mutex_put(&disp.lock);
      tx_semaphore_ceiling_put(&disp.update, 1);
    }

  }
}

static void dp_update_drawing_area()
{
  int ret;

  __disable_irq();
  ret = HAL_LTDC_SetAddress_NoReload(&hlcd_ltdc, (uint32_t) lcd_fg_buffer[lcd_fg_buffer_rd_idx], LTDC_LAYER_2);
  assert(ret == HAL_OK);
  __enable_irq();
}

static void dp_commit_drawing_area()
{
  int ret;

  __disable_irq();
  ret = HAL_LTDC_ReloadLayer(&hlcd_ltdc, LTDC_RELOAD_VERTICAL_BLANKING, LTDC_LAYER_2);
  assert(ret == HAL_OK);
  __enable_irq();
  lcd_fg_buffer_rd_idx = 1 - lcd_fg_buffer_rd_idx;
}

static void dp_thread_fct(ULONG arg)
{
  display_info_t info;
  int ret;

  while (1)
  {
    HAL_Delay(50);
    ret = tx_semaphore_get(&disp.update, TX_WAIT_FOREVER);
    assert(ret == 0);

    tx_mutex_get(&disp.lock, TX_WAIT_FOREVER);
    info = disp.info;
    tx_mutex_put(&disp.lock);

    dp_update_drawing_area();
    Display_NetworkOutput(&info);
    SCB_CleanDCache_by_Addr(lcd_fg_buffer[lcd_fg_buffer_rd_idx], LCD_FG_WIDTH * LCD_FG_HEIGHT* 2);
    dp_commit_drawing_area();
  }
}

static void isp_thread_fct(ULONG arg)
{
  int ret;

  while (1) {
    ret = tx_semaphore_get(&isp_sem, TX_WAIT_FOREVER);
    assert(ret == 0);

    CAM_IspUpdate();
  }
}

void app_run()
{
  const UINT isp_priority = TX_MAX_PRIORITIES / 2 - 2;
  const UINT dp_priority = TX_MAX_PRIORITIES / 2 + 2;
  const UINT nn_priority = TX_MAX_PRIORITIES / 2 - 1;
  const ULONG time_slice = 10;
  int ret;

  printf("Init application\n");
  /* Enable DWT so DWT_CYCCNT works when debugger not attached */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

  /* screen init */
  memset(lcd_bg_buffer, 0, sizeof(lcd_bg_buffer));
  CACHE_OP(SCB_CleanInvalidateDCache_by_Addr(lcd_bg_buffer, sizeof(lcd_bg_buffer)));
  memset(lcd_fg_buffer, 0, sizeof(lcd_fg_buffer));
  CACHE_OP(SCB_CleanInvalidateDCache_by_Addr(lcd_fg_buffer, sizeof(lcd_fg_buffer)));
  LCD_init();

  /*** Camera Init ************************************************************/  
  CAM_Init();

  /* sems + mutex init */
  ret = tx_semaphore_create(&isp_sem, NULL, 0);
  assert(ret == 0);
  ret = tx_semaphore_create(&disp.update, NULL, 0);
  assert(ret == 0);
  ret= tx_mutex_create(&disp.lock, NULL, TX_INHERIT);
  assert(ret == 0);

  /* Start LCD Display camera pipe stream */
  CAM_DisplayPipe_Start(lcd_bg_buffer[0], CAMERA_MODE_CONTINUOUS);

  /* threads init */
  ret = tx_thread_create(&nn_thread, "nn", nn_thread_ei_fct, 0, nn_tread_stack,
                         sizeof(nn_tread_stack), nn_priority, nn_priority, time_slice, TX_AUTO_START);
  assert(ret == TX_SUCCESS);

  ret = tx_thread_create(&dp_thread, "dp", dp_thread_fct, 0, dp_tread_stack,
                         sizeof(dp_tread_stack), dp_priority, dp_priority, time_slice, TX_AUTO_START);
  assert(ret == TX_SUCCESS);
  ret = tx_thread_create(&isp_thread, "isp", isp_thread_fct, 0, isp_tread_stack,
                         sizeof(isp_tread_stack), isp_priority, isp_priority, time_slice, TX_AUTO_START);
  assert(ret == TX_SUCCESS);
}

int CMW_CAMERA_PIPE_FrameEventCallback(uint32_t pipe)
{
  if (pipe == DCMIPP_PIPE1)
    app_main_pipe_frame_event();
  else if (pipe == DCMIPP_PIPE2) {
    cameraFrameReceived++;
  }

  return HAL_OK;
}

int CMW_CAMERA_PIPE_VsyncEventCallback(uint32_t pipe)
{
  if (pipe == DCMIPP_PIPE1)
    app_main_pipe_vsync_event();

  return HAL_OK;
}
