/**
  ******************************************************************************
  * @file    vdx941.h
  * @author  MDG Application Team
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

#ifndef VDx941_H
#define VDx941_H

#include <stdarg.h>
#include <stdint.h>

#define VDx941_LVL_ERROR 0
#define VDx941_LVL_WARNING 1
#define VDx941_LVL_NOTICE 2
#define VDx941_LVL_DBG(l) (3 + (l))

typedef enum {
  VDx941_BAYER_NONE,
  VDx941_BAYER_RGGB,
  VDx941_BAYER_GRBG,
  VDx941_BAYER_GBRG,
  VDx941_BAYER_BGGR,
  VDx941_BAYER_RGBNIR,
  VDx941_BAYER_RGBNIR_MIRROR,
  VDx941_BAYER_RGBNIR_FLIP,
  VDx941_BAYER_RGBNIR_FLIP_MIRROR
} VDx941_BayerType_t;

/* Output image will have resolution of VDx941_Res_t / ss_ratio */
typedef enum {
  VDx941_RES_QVGA_320_240,
  VDx941_RES_VGA_640_480,
  VDx941_RES_SVGA_800_600,
  VDx941_RES_XGA_1024_768,
  VDx941_RES_720P_1280_720,
  VDx941_RES_SXGA_1280_1024,
  VDx941_RES_1080P_1920_1080,
  VDx941_RES_QXGA_2048_1536,
  VDx941_RES_FULL_2560_1984,
} VDx941_Res_t;

typedef enum {
  VDx941_MIRROR_FLIP_NONE,
  VDx941_FLIP,
  VDx941_MIRROR,
  VDx941_MIRROR_FLIP
} VDx941_MirrorFlip_t;

typedef enum {
  VDx941_PATGEN_DISABLE,
  VDx941_PATGEN_DIAGONAL_GRAYSCALE,
  VDx941_PATGEN_PSEUDO_RANDOM,
} VDx941_PatGen_t;

enum {
  VDx941_ST_IDLE,
  VDx941_ST_STREAMING,
};

enum {
  VDx941_MIN_CLOCK_FREQ = 12000000,
  VDx941_MAX_CLOCK_FREQ = 50000000,
};

enum {
  VDx941_MIN_DATARATE = 250000000,
  VDx941_DEFAULT_DATARATE = 1300000000,
  VDx941_MAX_DATARATE = 1500000000,
};

typedef enum {
  VDx941_GPIO_STROBE = 0,
  VDx941_GPIO_PWM_STROBE = 1,
  VDx941_GPIO_PWM = 2,
  VDx941_GPIO_IN = 3,
  VDx941_GPIO_OUT = 4,
  VDx941_GPIO_FSYNC_IN = 5,
} VDx941_GPIO_Mode_t;

typedef enum {
  VDx941_GPIO_LOW = (0 << 4),
  VDx941_GPIO_HIGH = (1 << 4),
} VDx941_GPIO_Value_t;

typedef enum {
  VDx941_GPIO_NO_INVERSION = (0 << 5),
  VDx941_GPIO_INVERTED = (1 << 5),
} VDx941_GPIO_Polarity_t;

enum {
  VDx941_GPIO_0,
  VDx941_GPIO_1,
  VDx941_GPIO_2,
  VDx941_GPIO_3,
  VDx941_GPIO_NB
};

typedef struct {
  /* VD6G_GPIO_Mode_t | VD6G_GPIO_Value_t | VD6G_GPIO_Polarity_t */
  uint8_t gpio_ctrl;
  uint8_t enable;
} VDx941_GPIO_t;

typedef enum {
  VDx941_GS_SS1_NATIVE_8 = 0x01,
  VDx941_GS_SS1_NATIVE_10 = 0x02,
  VDx941_GS_SS1_SPLIT_NATIVE_8 = 0x03,
  VDx941_GS_SS1_SPLIT_NATIVE_10 = 0x04,
  VD1941_GS_SS1_RGB_8 = 0x05,
  VD1941_GS_SS1_RGB_10 = 0x06,
  VD1941_GS_SS1_IR_8 = 0x0f,
  VD1941_GS_SS1_IR_10 = 0x10,
  VD1941_GS_SS1_SPLIT_IR_8 = 0x11,
  VD1941_GS_SS1_SPLIT_IR_10 = 0x12,
  VDx941_GS_SS2_MONO_8 = 0x09,
  VDx941_GS_SS2_MONO_10 = 0x0a,
  VDx941_GS_SS4_MONO_8 = 0x0b,
  VDx941_GS_SS4_MONO_10 = 0x0c,
  VDx941_GS_SS32_MONO_8 = 0x0d,
  VDx941_GS_SS32_MONO_10 = 0x0e,
  VDx941_RS_SDR_NATIVE_8 = 0x1a,
  VDx941_RS_SDR_NATIVE_10 = 0x1b,
  VDx941_RS_SDR_NATIVE_12 = 0x1c,
  VD1941_RS_SDR_RGB_8 = 0x1d,
  VD1941_RS_SDR_RGB_10 = 0x1e,
  VD1941_RS_SDR_RGB_12 = 0x1f,
  VDx941_RS_HDR_NATIVE_10 = 0x20,
  VDx941_RS_HDR_NATIVE_12 = 0x21,
  VD1941_RS_HDR_RGB_10 = 0x22,
  VD1941_RS_HDR_RGB_12 = 0x23,
} VDx941_MODE_t;

typedef enum {
  VDx941_MASTER,
  VDx941_SLAVE,
} VDx941_VT_Sync_T;

/* Output interface configuration */
typedef struct {
  int data_rate_in_mps;
  int datalane_nb;
  /* Define lane mapping for the four lane even in 2 data lanes case */
  int logic_lane_mapping[4];
  int clock_lane_swap_enable;
  int physical_lane_swap_enable[4];
} VDx941_OutItf_Config_t;

/* VDx941 configuration */
typedef struct {
  int ext_clock_freq_in_hz;
  VDx941_Res_t resolution;
  int frame_rate;
  VDx941_MODE_t image_processing_mode;
  VDx941_MirrorFlip_t flip_mirror_mode;
  VDx941_PatGen_t patgen;
  VDx941_OutItf_Config_t out_itf;
  VDx941_VT_Sync_T sync_mode;
  VDx941_GPIO_t gpios[VDx941_GPIO_NB];
} VDx941_Config_t;

typedef struct VDx941_Ctx
{
  /* API client must set these values */
  void (*shutdown_pin)(struct VDx941_Ctx *ctx, int value);
  int (*read8)(struct VDx941_Ctx *ctx, uint16_t addr, uint8_t *value);
  int (*read16)(struct VDx941_Ctx *ctx, uint16_t addr, uint16_t *value);
  int (*read32)(struct VDx941_Ctx *ctx, uint16_t addr, uint32_t *value);
  int (*write8)(struct VDx941_Ctx *ctx, uint16_t addr, uint8_t value);
  int (*write16)(struct VDx941_Ctx *ctx, uint16_t addr, uint16_t value);
  int (*write32)(struct VDx941_Ctx *ctx, uint16_t addr, uint32_t value);
  int (*write_array)(struct VDx941_Ctx *ctx, uint16_t addr, uint8_t *data, int data_len);
  void (*delay)(struct VDx941_Ctx *ctx, uint32_t delay_in_ms);
  void (*log)(struct VDx941_Ctx *ctx, int lvl, const char *format, va_list ap);
  /* driver fill those values on VDx941_Init */
  VDx941_BayerType_t bayer;
  /* driver internals. do not touch */
  struct drv_vdx941_ctx {
    int state;
    uint8_t is_mono;
    unsigned int digital_gain;
    VDx941_Config_t config_save;
    int (*set_digital_gain)(struct VDx941_Ctx *, unsigned int);
    int (*set_expo)(struct VDx941_Ctx *, unsigned int);
    int (*get_expo)(struct VDx941_Ctx *, unsigned int *);
  } ctx;
} VDx941_Ctx_t;

int VDx941_Init(VDx941_Ctx_t *ctx, VDx941_Config_t *config);
int VDx941_DeInit(VDx941_Ctx_t *ctx);
int VDx941_Start(VDx941_Ctx_t *ctx);
int VDx941_Stop(VDx941_Ctx_t *ctx);
int VDx941_Hold(VDx941_Ctx_t *ctx, int is_enable);
/* Valid value are in [0:12]. Which correspond in a gain [1.0:4.0] (not linear) */
int VDx941_SetAnalogGain(VDx941_Ctx_t *ctx, unsigned int gain);
int VDx941_GetAnalogGain(VDx941_Ctx_t *ctx, unsigned int *gain);
/* Format is in 5.8 fixed point. Valid range are from [0x0000:0x1fff] which correspond in a gain [0:31.99] (linear) */
int VDx941_SetDigitalGain(VDx941_Ctx_t *ctx, unsigned int gain);
int VDx941_GetDigitalGain(VDx941_Ctx_t *ctx, unsigned int *gain);
int VDx941_SetExpo(VDx941_Ctx_t *ctx, unsigned int expo_in_us);
int VDx941_GetExpo(VDx941_Ctx_t *ctx, unsigned int *expo_in_us);

#endif
