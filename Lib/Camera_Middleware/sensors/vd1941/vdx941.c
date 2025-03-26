/**
  ******************************************************************************
  * @file    vdx941.c
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

#include "vdx941.h"

#include <assert.h>
#include <stdint.h>

#include "vd1941_patch_cut_13.c"
#include "vd5941_patch_cut_13.c"
#include "vdx941_vt_patch.c"

/* STATUS */
#define VDx941_REG_MODEL_ID                                   0x0000
  #define VDx941_MODEL_ID                                     0x53393430
#define VDx941_REG_DEVICE_REVISION                            0x0004
  #define VDx941_DEVICE_REVISION                              0x0a020a0a
#define VDx941_REG_UI_REVISION                                0x0008
  #define VDx941_UI_REVISION                                  0x0301
#define VDx941_REG_ROM_REVISION                               0x000c
  #define VDx941_ROM_REVISION_13                              0x00000400
#define VDx941_REG_SYSTEM_FSM_STATE                           0x0044
  #define VDx941_HW_STBY                                      0
  #define VDx941_SYSTEM_UP                                    1
  #define VDx941_BOOT                                         2
  #define VDx941_SW_STBY                                      3
  #define VDx941_STREAMING                                    4
  #define VDx941_HALT                                         6
#define VDx941_REG_SYSTEM_ERROR                               0x0048
#define VDx941_FWPATCH_REVISION                               0x004a
#define VDx941_REG_SYSTEM_PLL_CLK                             0x0228
#define VDx941_REG_PIXEL_CLK                                  0x022c
/* CMD */
  #define VDx941_CMD_ACK                                      0
#define VDx941_REG_SYSTEM_UP                                  0x0514
  #define VDx941_CMD_START_SENSOR                             1
#define VDx941_REG_BOOT                                       0x0515
  #define VDx941_CMD_LOAD_CERTIFICATE                         1
  #define VDx941_CMD_LOAD_FWP                                 2
  #define VDx941_CMD_END_BOOT                                 0x10
#define VDx941_REG_SW_STBY                                    0x0516
  #define VDx941_CMD_START_STREAMING                          1
  #define VDx941_CMD_UPDATE_VT_RAM_START                      3
  #define VDx941_CMD_UPDATE_VT_RAM_END                        4
#define VDx941_REG_STREAMING                                  0x0517
  #define VDx941_CMD_STOP_STREAMING                           1
/* SENSOR SETTINGS */
#define VDx941_REG_EXT_CLOCK                                  0x0734
#define VDx941_REG_MIPI_DATA_RATE                             0x0738
#define VDx941_REG_LANE_NB_SEL                                0x0743
  #define VDx941_LANE_NB_SEL_4                                0
  #define VDx941_LANE_NB_SEL_2                                1
/* DIAG SETTINGS */
#define VDx941_REG_DIAG_DISABLE_FW_0_ERR                      0x078d
  #define VDx941_DIAG_DISABLE_FW_0_ERR_UI_CRC                 1
#define VDx941_REG_DIAG_DISABLE_FW_1_ERR                      0x078e
  #define VDx941_DIAG_DISABLE_FW_1_ERR_TASK_MONITOR           4
#define VDx941_REG_DIAG_DISABLE_STREAMING_ERR                 0x078f
  #define VDx941_DIAG_DISABLE_STREAMING_ERR_ALL               0xff
/* STREAM STATICS */
#define VDx941_REG_ROI_A_WIDTH_OFFSET                         0x090c
#define VDx941_REG_ROI_A_HEIGHT_OFFSET                        0x090e
#define VDx941_REG_ROI_A_WIDTH                                0x0910
#define VDx941_REG_ROI_A_HEIGHT                               0x0912
#define VDx941_REG_ROI_A_DT                                   0x0914
#define VDx941_REG_LINE_LENGTH                                0x0934
#define VDx941_REG_ORIENTATION                                0x0937
#define VDx941_REG_PATGEN_CTRL                                0x0938
 #define VDx941_PATGEN_CTRL_DISABLE                           0x0000
 #define VDx941_PATGEN_CTRL_DGREY                             0x2201
 #define VDx941_PATGEN_CTRL_PN28                              0x2801
#define VDx941_REG_OIF_LANE_PHY_MAP                           0x093a
#define VDx941_REG_OIF_LANE_PHY_SWAP                          0x093b
#define VDx941_REG_OIF_INTERPACKET_DELAY                      0x093c
#define VDx941_REG_VT_CTRL                                    0x0ac6
  #define VDx941_VT_CTRL_MASTER                               0
  #define VDx941_VT_CTRL_SLAVE                                1
#define VDx941_REG_OIF_ISL_ENABLE                             0x0ac7
#define VDx941_REG_GPIO_x_CTRL(_i_)                           (0xad4 + (_i_))
/* STREAM_CTXx_STATIC */
#define VDx941_REG_CTXx_STATIC_OFFSET(_x_, _addr_)            (0x0b40 + (_x_) * 72 + (_addr_))
#define VDx941_REG_SENSOR_CONFIGURATION(_x_)                  VDx941_REG_CTXx_STATIC_OFFSET(_x_, 0x0000)
#define VDx941_REG_FRAME_LENGTH(_x_)                          VDx941_REG_CTXx_STATIC_OFFSET(_x_, 0x0006)
#define VDx941_REG_GPIO_CTRL(_x_)                             VDx941_REG_CTXx_STATIC_OFFSET(_x_, 0x0009)
 #define VDx941_GPIO_DISABLE                                  0
 #define VDx941_GPIO_ENABLE                                   1
/* STREAM DYNAMICS */
/* STREAM_CTXx_DYNAMIC */
#define VDx941_REG_CTXx_DYNAMIC_OFFSET(_x_, _addr_)           (0x0c78 + (_x_) * 40 + (_addr_))

#define VDx941_REG_GROUP_PARAMETER_HOLD(_x_)                  VDx941_REG_CTXx_DYNAMIC_OFFSET(_x_, 0x0000)
 #define VDx941_GROUP_PARAMETER_HOLD_DISABLE                  0
 #define VDx941_GROUP_PARAMETER_HOLD_ENABLE                   1
#define VDx941_REG_ANALOG_GAIN(_x_)                           VDx941_REG_CTXx_DYNAMIC_OFFSET(_x_, 0x0001)
#define VDx941_REG_INTEGRATION_TIME_PRIMARY(_x_)              VDx941_REG_CTXx_DYNAMIC_OFFSET(_x_, 0x0002)
#define VDx941_REG_INTEGRATION_TIME_IR(_x_)                   VDx941_REG_CTXx_DYNAMIC_OFFSET(_x_, 0x0004)
#define VDx941_REG_INTEGRATION_TIME_SHORT(_x_)                VDx941_REG_CTXx_DYNAMIC_OFFSET(_x_, 0x0006)
#define VDx941_DIGITAL_GAIN_R(_x_)                            VDx941_REG_CTXx_DYNAMIC_OFFSET(_x_, 0x0008)
#define VDx941_DIGITAL_GAIN_G(_x_)                            VDx941_REG_CTXx_DYNAMIC_OFFSET(_x_, 0x000a)
#define VDx941_DIGITAL_GAIN_B(_x_)                            VDx941_REG_CTXx_DYNAMIC_OFFSET(_x_, 0x000c)
#define VDx941_DIGITAL_GAIN_IR(_x_)                           VDx941_REG_CTXx_DYNAMIC_OFFSET(_x_, 0x000e)

/* DEBUG */
#define VDx941_REG_DPHYTX_CTRL                                0x108c
#define VDx941_REG_BOTTOM_STATUS_LINE_DISABLE                 0x110c
/* CERTIFICATE */
#define VDx941_REG_CERTIFICATE_AREA_START_ADDR                0x1aa8
/* PATCH AREA */
#define VDx941_REG_FWPATCH_START_ADDR                         0x2000

/* FIXME : taken from wolfy. What is value for maneki ? */
#define VDx941_MIN_VBLANK                                     86
#define VDx941_MAX_WIDTH                                      2560
#define VDx941_MAX_HEIGHT                                     1984

/* Yes we are above maximum recommended line time duration. But this value allow to output a 10 bits full frame in
 * global shutter mode in a 2 lanes configuration.
 */
#define VDx941_LINE_TIME_IN_NS                                14500
#define VDx941_ANALOG_GAIN_MAX                                12
#define VDx941_DIGITAL_GAIN_MAX                               0x1fff

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#define VDx941_COMPUTE_LEFT(_w_)                              ((VDx941_MAX_WIDTH - (_w_)) / 2)
#define VDx941_COMPUTE_TOP(_h_)                               ((VDx941_MAX_HEIGHT - (_h_)) / 2)

struct vdx941_rect {
  int32_t left;
  int32_t top;
  uint32_t width;
  uint32_t height;
};

static const struct vdx941_rect vdx941_supported_modes[] = {
  /* VDx941_RES_QVGA_320_240 */
  {
    .left = VDx941_COMPUTE_LEFT(320),
    .top = VDx941_COMPUTE_TOP(240),
    .width = 320,
    .height = 240,
  },
  /* VDx941_RES_VGA_640_480 */
  {
    .left = VDx941_COMPUTE_LEFT(640),
    .top = VDx941_COMPUTE_TOP(480),
    .width = 640,
    .height = 480,
  },
  /* VDx941_RES_VGA_800_600 */
  {
    .left = VDx941_COMPUTE_LEFT(800),
    .top = VDx941_COMPUTE_TOP(600),
    .width = 800,
    .height = 600,
  },
  /* VDx941_RES_XGA_1024_768 */
  {
    .left = VDx941_COMPUTE_LEFT(1024),
    .top = VDx941_COMPUTE_TOP(768),
    .width = 1024,
    .height = 768,
  },
  /* VDx941_RES_720P_1280_720 */
  {
    .left = VDx941_COMPUTE_LEFT(1280),
    .top = VDx941_COMPUTE_TOP(720),
    .width = 1280,
    .height = 720,
  },
  /* VDx941_RES_SXGA_1280_1024 */
  {
    .left = VDx941_COMPUTE_LEFT(1280),
    .top = VDx941_COMPUTE_TOP(1024),
    .width = 1280,
    .height = 1024,
  },
  /* VDx941_RES_1080P_1920_1080 */
  {
    .left = VDx941_COMPUTE_LEFT(1920),
    .top = VDx941_COMPUTE_TOP(1080),
    .width = 1920,
    .height = 1080,
  },
  /* VDx941_RES_QXGA_2048_1536 */
  {
    .left = VDx941_COMPUTE_LEFT(2048),
    .top = VDx941_COMPUTE_TOP(1536),
    .width = 2048,
    .height = 1536,
  },
  /* VDx941_RES_FULL_2560_1984 */
  {
    .left = VDx941_COMPUTE_LEFT(2560),
    .top = VDx941_COMPUTE_TOP(1984),
    .width = 2560,
    .height = 1984,
  },
};

#define VDx941_TraceError(_ctx_,_ret_) do { \
  if (_ret_) VDx941_error(_ctx_, "Error on %s:%d : %d\n", __func__, __LINE__, _ret_); \
  if (_ret_) display_error(_ctx_); \
  if (_ret_) return _ret_; \
} while(0)

static void VDx941_log_impl(VDx941_Ctx_t *ctx, int lvl, const char *format, ...)
{
  va_list ap;

  if (!ctx->log)
    return ;

  va_start(ap, format);
  ctx->log(ctx, lvl, format, ap);
  va_end(ap);
}

#define VDx941_dbg(_ctx_, _lvl_, _fmt_, ...) do { \
  VDx941_log_impl(_ctx_, VDx941_LVL_DBG(_lvl_), "VDx941_DG%d-%d : " _fmt_, _lvl_, __LINE__, ##__VA_ARGS__); \
} while(0)

#define VDx941_notice(_ctx_, _fmt_, ...) do { \
  VDx941_log_impl(_ctx_, VDx941_LVL_NOTICE, "VDx941_NOT-%d : " _fmt_, __LINE__, ##__VA_ARGS__); \
} while(0)

#define VDx941_warn(_ctx_, _fmt_, ...) do { \
  VDx941_log_impl(_ctx_, VDx941_LVL_WARNING, "VDx941_WRN-%d : " _fmt_, __LINE__, ##__VA_ARGS__); \
} while(0)

#define VDx941_error(_ctx_, _fmt_, ...) do { \
  VDx941_log_impl(_ctx_, VDx941_LVL_ERROR, "VDx941_ERR-%d : " _fmt_, __LINE__, ##__VA_ARGS__); \
} while(0)

static void display_error(VDx941_Ctx_t *ctx)
{
  uint16_t reg16;
  int ret;

  ret = ctx->read16(ctx, VDx941_REG_SYSTEM_ERROR, &reg16);
  assert(ret == 0);
  VDx941_error(ctx, "ERROR_CODE : 0x%04x\n", reg16);
}

static int VDx941_PollReg8(VDx941_Ctx_t *ctx, uint16_t addr, uint8_t poll_val)
{
  const unsigned int loop_delay_ms = 10;
  const unsigned int timeout_ms = 500;
  int loop_nb = timeout_ms / loop_delay_ms;
  uint8_t val;
  int ret;

  while (--loop_nb) {
    ret = ctx->read8(ctx, addr, &val);
    if (ret < 0)
      return ret;
    if (val == poll_val)
      return 0;
    ctx->delay(ctx, loop_delay_ms);
  }

  VDx941_dbg(ctx, 0, "current state %d\n", val);

  return -1;
}

static int VDx941_WaitState(VDx941_Ctx_t *ctx, int state)
{
  int ret = VDx941_PollReg8(ctx, VDx941_REG_SYSTEM_FSM_STATE, state);

  if (ret)
    VDx941_warn(ctx, "Unable to reach state %d\n", state);
  else
    VDx941_dbg(ctx, 0, "reach state %d\n", state);

  return ret;
}

static int VDx941_ApplyCmdAndWait(VDx941_Ctx_t *ctx, uint16_t addr, uint8_t cmd)
{
  int ret;

  VDx941_dbg(ctx, 0, "Apply cmd %d to 0x%04x\n", cmd, addr);
  ret = ctx->write8(ctx, addr, cmd);
  VDx941_TraceError(ctx, ret);

  ret = VDx941_PollReg8(ctx, addr, VDx941_CMD_ACK);
  VDx941_TraceError(ctx, ret);

  return 0;
}

static int VDx941_CheckModelId(VDx941_Ctx_t *ctx)
{
  uint32_t reg32;
  uint16_t reg16;
  int ret;

  ret = ctx->read32(ctx, VDx941_REG_MODEL_ID, &reg32);
  VDx941_TraceError(ctx, ret);
  VDx941_dbg(ctx, 0, "model_id = 0x%08x\n", reg32);
  if (reg32 != VDx941_MODEL_ID) {
    VDx941_error(ctx, "Bad model id expected 0x%08x / got 0x%08x\n", VDx941_MODEL_ID, reg32);
    return -1;
  }

  ret = ctx->read32(ctx, VDx941_REG_DEVICE_REVISION, &reg32);
  VDx941_TraceError(ctx, ret);
  VDx941_dbg(ctx, 0, "device revision = 0x%08x\n", reg32);
  if (reg32 != VDx941_DEVICE_REVISION) {
    VDx941_error(ctx, "Bad device revision expected 0x%08x / got 0x%08x\n", VDx941_DEVICE_REVISION, reg32);
    return -1;
  }

  ret = ctx->read32(ctx, VDx941_REG_ROM_REVISION, &reg32);
  VDx941_TraceError(ctx, ret);
  VDx941_dbg(ctx, 0, "rom revision = 0x%08x\n", reg32);
  if (reg32 != VDx941_ROM_REVISION_13) {
    VDx941_error(ctx, "Bad rom revision expected 0x%08x / got 0x%08x\n", VDx941_ROM_REVISION_13, reg32);
    return -1;
  }

  ret = ctx->read16(ctx, VDx941_REG_UI_REVISION, &reg16);
  VDx941_TraceError(ctx, ret);
  VDx941_dbg(ctx, 0, "ui revision = 0x%04x\n", reg16);
  if (reg16 != VDx941_UI_REVISION) {
    VDx941_error(ctx, "Bad ui revision expected 0x%04x / got 0x%04x\n", VDx941_UI_REVISION, reg16);
    return -1;
  }

  return 0;
}

static int VDx941_GoToBootState(VDx941_Ctx_t *ctx)
{
  int ret;

  ret = VDx941_ApplyCmdAndWait(ctx, VDx941_REG_SYSTEM_UP, VDx941_CMD_START_SENSOR);
  if (ret)
    return ret;

  ret = VDx941_WaitState(ctx, VDx941_BOOT);
  if (ret)
    return ret;

  return 0;
}

static int VDx941_ApplyCertificate(VDx941_Ctx_t *ctx)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;
  int ret;

  if (drv_vdx941_ctx->is_mono) {
      ret = ctx->write_array(ctx, VDx941_REG_CERTIFICATE_AREA_START_ADDR, (uint8_t *)fw_patch_13_mono_cert,
                             sizeof(fw_patch_13_mono_cert));
  } else {
      ret = ctx->write_array(ctx, VDx941_REG_CERTIFICATE_AREA_START_ADDR, (uint8_t *)fw_patch_13_rgbnir_cert,
                             sizeof(fw_patch_13_rgbnir_cert));
  }
  VDx941_TraceError(ctx, ret);

  ret = VDx941_ApplyCmdAndWait(ctx, VDx941_REG_BOOT, VDx941_CMD_LOAD_CERTIFICATE);
  if (ret)
    return ret;

  return 0;
}

static int VDx941_ApplyFwPatch(VDx941_Ctx_t *ctx)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;
  uint16_t reg16;
  int ret;

  if (drv_vdx941_ctx->is_mono) {
      ret = ctx->write_array(ctx, VDx941_REG_FWPATCH_START_ADDR, (uint8_t *)fw_patch_13_mono_array,
                             sizeof(fw_patch_13_mono_array));
  } else {
      ret = ctx->write_array(ctx, VDx941_REG_FWPATCH_START_ADDR, (uint8_t *)fw_patch_13_rgbnir_array,
                             sizeof(fw_patch_13_rgbnir_array));
  }
  VDx941_TraceError(ctx, ret);

  ret = VDx941_ApplyCmdAndWait(ctx, VDx941_REG_BOOT, VDx941_CMD_LOAD_FWP);
  if (ret)
    return ret;

  ret = ctx->read16(ctx, VDx941_FWPATCH_REVISION, &reg16);
  VDx941_TraceError(ctx, ret);
  VDx941_notice(ctx, "patch = %d.%d\n", reg16 >> 8, reg16 & 0xff);

  return 0;
}

static int VDx941_GoToStandbyState(VDx941_Ctx_t *ctx)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;
  int ret;

  ret = VDx941_ApplyCertificate(ctx);
  if (ret)
    return ret;

  ret = VDx941_ApplyFwPatch(ctx);
  if (ret)
    return ret;

  /* Need to setup input clock before boot */
  ret = ctx->write32(ctx, VDx941_REG_EXT_CLOCK, drv_vdx941_ctx->config_save.ext_clock_freq_in_hz);
  VDx941_TraceError(ctx, ret);

  ret = VDx941_ApplyCmdAndWait(ctx, VDx941_REG_BOOT, VDx941_CMD_END_BOOT);
  if (ret)
    return ret;

  ret = VDx941_WaitState(ctx, VDx941_SW_STBY);
  if (ret)
    return ret;

  return 0;
}

static int VDx941_ApplyVTPatch(VDx941_Ctx_t *ctx)
{
  int ret;

  ret = VDx941_ApplyCmdAndWait(ctx, VDx941_REG_SW_STBY, VDx941_CMD_UPDATE_VT_RAM_START);
  if (ret)
    return ret;

  ret = ctx->write_array(ctx, 0x5bc0, (uint8_t *) gt_ram_pat_content, sizeof(gt_ram_pat_content));
  VDx941_TraceError(ctx, ret);

  ret = ctx->write_array(ctx, 0x5e40, (uint8_t *) gt_ram_seq1_content, sizeof(gt_ram_seq1_content));
  VDx941_TraceError(ctx, ret);

  ret = ctx->write_array(ctx, 0x5f80, (uint8_t *) gt_ram_seq2_content, sizeof(gt_ram_seq2_content));
  VDx941_TraceError(ctx, ret);

  ret = ctx->write_array(ctx, 0x60c0, (uint8_t *) gt_ram_seq3_content, sizeof(gt_ram_seq3_content));
  VDx941_TraceError(ctx, ret);

  ret = ctx->write_array(ctx, 0x6160, (uint8_t *) gt_ram_seq4_content, sizeof(gt_ram_seq4_content));
  VDx941_TraceError(ctx, ret);

  ret = ctx->write_array(ctx, 0x6ac0, (uint8_t *) rd_ram_pat_content, sizeof(rd_ram_pat_content));
  VDx941_TraceError(ctx, ret);

  ret = ctx->write_array(ctx, 0x5000, (uint8_t *) rd_ram_seq1_content, sizeof(rd_ram_seq1_content));
  VDx941_TraceError(ctx, ret);

  ret = VDx941_ApplyCmdAndWait(ctx, VDx941_REG_SW_STBY, VDx941_CMD_UPDATE_VT_RAM_END);
  if (ret)
    return ret;

  return 0;
}

static int VDx941_SetDigitalGain_RGB(VDx941_Ctx_t *ctx, unsigned int gain)
{
  int ret;

  ret = ctx->write16(ctx, VDx941_DIGITAL_GAIN_R(0), gain);
  VDx941_TraceError(ctx, ret);
  ret = ctx->write16(ctx, VDx941_DIGITAL_GAIN_G(0), gain);
  VDx941_TraceError(ctx, ret);
  ret = ctx->write16(ctx, VDx941_DIGITAL_GAIN_B(0), gain);
  VDx941_TraceError(ctx, ret);

  return 0;
}

static int VDx941_SetDigitalGain_IR(VDx941_Ctx_t *ctx, unsigned int gain)
{
  int ret;

  ret = ctx->write16(ctx, VDx941_DIGITAL_GAIN_IR(0), gain);
  VDx941_TraceError(ctx, ret);

  return 0;
}

static int VDx941_SetDigitalGain_RGBNIR(VDx941_Ctx_t *ctx, unsigned int gain)
{
  int ret;

  ret = VDx941_SetDigitalGain_RGB(ctx, gain);
  if (ret)
    return ret;
  ret = VDx941_SetDigitalGain_IR(ctx, gain);
  if (ret)
    return ret;

  return 0;
}

static int VDx941_GetExpoCommon(VDx941_Ctx_t *ctx, unsigned int *expo_in_us)
{
  uint16_t reg16;
  int ret;

  ret = ctx->read16(ctx, VDx941_REG_INTEGRATION_TIME_PRIMARY(0), &reg16);
  VDx941_TraceError(ctx, ret);
  *expo_in_us = (reg16 * VDx941_LINE_TIME_IN_NS) / 1000;

  return 0;
}

static int VDx941_SetExpo_GS(VDx941_Ctx_t *ctx, unsigned int expo_in_us)
{
  unsigned int expo_in_line = (expo_in_us * 1000 + VDx941_LINE_TIME_IN_NS - 1) / VDx941_LINE_TIME_IN_NS;
  int ret;

  if (expo_in_line < 4)
    return -1;

  ret = ctx->write16(ctx, VDx941_REG_INTEGRATION_TIME_PRIMARY(0), expo_in_line);
  VDx941_TraceError(ctx, ret);

  return 0;
}

static int VDx941_SetExpoSplit_GS(VDx941_Ctx_t *ctx, unsigned int expo_in_us)
{
  unsigned int expo_in_line = (expo_in_us * 1000 + VDx941_LINE_TIME_IN_NS - 1) / VDx941_LINE_TIME_IN_NS;
  int ret;

  if (expo_in_line < 4)
    return -1;

  ret = ctx->write16(ctx, VDx941_REG_INTEGRATION_TIME_PRIMARY(0), expo_in_line);
  VDx941_TraceError(ctx, ret);

  ret = ctx->write16(ctx, VDx941_REG_INTEGRATION_TIME_IR(0), expo_in_line);
  VDx941_TraceError(ctx, ret);

  return 0;
}

static int VDx941_GetExpo_GS(VDx941_Ctx_t *ctx, unsigned int *expo_in_us)
{
  return VDx941_GetExpoCommon(ctx, expo_in_us);
}

static int VDx941_SetExpo_RS(VDx941_Ctx_t *ctx, unsigned int expo_in_us)
{
  unsigned int expo_in_line = (expo_in_us * 1000 + VDx941_LINE_TIME_IN_NS - 1) / VDx941_LINE_TIME_IN_NS;
  int ret;

  if (expo_in_line < 2)
    return -1;

  ret = ctx->write16(ctx, VDx941_REG_INTEGRATION_TIME_PRIMARY(0), expo_in_line);
  VDx941_TraceError(ctx, ret);

  return 0;
}

static int VDx941_GetExpo_RS(VDx941_Ctx_t *ctx, unsigned int *expo_in_us)
{
  return VDx941_GetExpoCommon(ctx, expo_in_us);
}

static int VDx941_SetExpo_RS_HDR(VDx941_Ctx_t *ctx, unsigned int expo_in_us)
{
  /* we use a short expo ratio of 1/20 */
  unsigned int expo_short_in_line = (expo_in_us * 50 + VDx941_LINE_TIME_IN_NS - 1) / VDx941_LINE_TIME_IN_NS;
  int ret;

  ret = VDx941_SetExpo_RS(ctx, expo_in_us);
  if (ret)
    return ret;

  expo_short_in_line = MAX(2, expo_short_in_line);
  ret = ctx->write16(ctx, VDx941_REG_INTEGRATION_TIME_SHORT(0), expo_short_in_line);
  VDx941_TraceError(ctx, ret);

  return 0;
}

static int VDx941_SetBayerType_RGB(VDx941_Ctx_t *ctx)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;

  switch (drv_vdx941_ctx->config_save.flip_mirror_mode) {
  case VDx941_MIRROR_FLIP_NONE:
    ctx->bayer = VDx941_BAYER_GBRG;
    break;
  case VDx941_FLIP:
    ctx->bayer = VDx941_BAYER_RGGB;
    break;
  case VDx941_MIRROR:
    ctx->bayer = VDx941_BAYER_BGGR;
    break;
  case VDx941_MIRROR_FLIP:
    ctx->bayer = VDx941_BAYER_GBRG;
    break;
  default:
    assert(0);
  }

  return 0;
}

static int VDx941_SetBayerType_RGBNIR(VDx941_Ctx_t *ctx)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;

  switch (drv_vdx941_ctx->config_save.flip_mirror_mode) {
  case VDx941_MIRROR_FLIP_NONE:
    ctx->bayer = VDx941_BAYER_RGBNIR;
    break;
  case VDx941_FLIP:
    ctx->bayer = VDx941_BAYER_RGBNIR_FLIP;
    break;
  case VDx941_MIRROR:
    ctx->bayer = VDx941_BAYER_RGBNIR_MIRROR;
    break;
  case VDx941_MIRROR_FLIP:
    ctx->bayer = VDx941_BAYER_RGBNIR_FLIP_MIRROR;
    break;
  default:
    assert(0);
  }

  return 0;
}

static int VDx941_SetBayerType(VDx941_Ctx_t *ctx)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;

  switch (drv_vdx941_ctx->config_save.image_processing_mode) {
  case VD1941_GS_SS1_RGB_8:
  case VD1941_GS_SS1_RGB_10:
  case VD1941_RS_SDR_RGB_8:
  case VD1941_RS_SDR_RGB_10:
  case VD1941_RS_SDR_RGB_12:
  case VD1941_RS_HDR_RGB_10:
  case VD1941_RS_HDR_RGB_12:
    return VDx941_SetBayerType_RGB(ctx);
    break;
  case VDx941_GS_SS1_NATIVE_8:
  case VDx941_GS_SS1_NATIVE_10:
  case VDx941_GS_SS1_SPLIT_NATIVE_8:
  case VDx941_GS_SS1_SPLIT_NATIVE_10:
  case VDx941_RS_SDR_NATIVE_8:
  case VDx941_RS_SDR_NATIVE_10:
  case VDx941_RS_SDR_NATIVE_12:
  case VDx941_RS_HDR_NATIVE_10:
  case VDx941_RS_HDR_NATIVE_12:
    if (drv_vdx941_ctx->is_mono)
      return VDx941_BAYER_NONE;
    else
      return VDx941_SetBayerType_RGBNIR(ctx);
    break;
  case VD1941_GS_SS1_IR_8:
  case VD1941_GS_SS1_IR_10:
  case VD1941_GS_SS1_SPLIT_IR_8:
  case VD1941_GS_SS1_SPLIT_IR_10:
  case VDx941_GS_SS2_MONO_8:
  case VDx941_GS_SS2_MONO_10:
  case VDx941_GS_SS4_MONO_8:
  case VDx941_GS_SS4_MONO_10:
  case VDx941_GS_SS32_MONO_8:
  case VDx941_GS_SS32_MONO_10:
    return VDx941_BAYER_NONE;
  default:
    assert(0);
  }

  return 0;
}

static int VDx941_GetSubSamplingFactor(VDx941_Ctx_t *ctx, int *ss_factor)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;

  switch (drv_vdx941_ctx->config_save.image_processing_mode) {
  case VDx941_GS_SS32_MONO_8:
  case VDx941_GS_SS32_MONO_10:
    *ss_factor = 32;
    break;
  case VDx941_GS_SS2_MONO_8:
  case VDx941_GS_SS2_MONO_10:
    *ss_factor = 2;
    break;
  case VDx941_GS_SS4_MONO_8:
  case VDx941_GS_SS4_MONO_10:
    *ss_factor = 4;
    break;
  default:
    *ss_factor = 1;
  }

  return 0;
}

static int VDx941_Boot(VDx941_Ctx_t *ctx)
{
  int ret;

  ret = VDx941_WaitState(ctx, VDx941_SYSTEM_UP);
  if (ret)
    return ret;

  ret = VDx941_CheckModelId(ctx);
  if (ret)
    return ret;

  ret = VDx941_GoToBootState(ctx);
  if (ret)
    return ret;

  ret = VDx941_GoToStandbyState(ctx);
  if (ret)
    return ret;

  ret = VDx941_ApplyVTPatch(ctx);
  if (ret)
    return ret;

  ret = VDx941_SetBayerType(ctx);
  if (ret)
    return ret;

  return 0;
}

static uint8_t VDx941_GetLaneNb(VDx941_OutItf_Config_t *out_itf)
{
  switch (out_itf->datalane_nb) {
  case 2:
    return VDx941_LANE_NB_SEL_2;
    break;
  case 4:
    return VDx941_LANE_NB_SEL_4;
    break;
  default:
    assert(0);
  }

  return VDx941_LANE_NB_SEL_2;
}

static uint8_t VDx941_GetLogicalPhysicalMapping(VDx941_OutItf_Config_t *out_itf)
{
  return (out_itf->logic_lane_mapping[0] << 0) |
         (out_itf->logic_lane_mapping[1] << 2) |
         (out_itf->logic_lane_mapping[2] << 4) |
         (out_itf->logic_lane_mapping[3] << 6);
}

static uint8_t VDx941_GetLaneSwap(VDx941_OutItf_Config_t *out_itf)
{
  return (out_itf->physical_lane_swap_enable[0] << 0) |
         (out_itf->physical_lane_swap_enable[1] << 1) |
         (out_itf->physical_lane_swap_enable[2] << 2) |
         (out_itf->physical_lane_swap_enable[3] << 3) |
         (out_itf->clock_lane_swap_enable << 4);
}

static int VDx941_SetupOutputDataType(VDx941_Ctx_t *ctx)
{
  uint8_t dt;
  int ret;

  switch (ctx->ctx.config_save.image_processing_mode) {
  case VDx941_GS_SS1_NATIVE_8:
  case VDx941_GS_SS1_SPLIT_NATIVE_8:
  case VD1941_GS_SS1_RGB_8:
  case VD1941_GS_SS1_IR_8:
  case VD1941_GS_SS1_SPLIT_IR_8:
  case VDx941_GS_SS2_MONO_8:
  case VDx941_GS_SS4_MONO_8:
  case VDx941_GS_SS32_MONO_8:
  case VDx941_RS_SDR_NATIVE_8:
  case VD1941_RS_SDR_RGB_8:
    dt = 0x2a;
    break;
  case VDx941_GS_SS1_NATIVE_10:
  case VDx941_GS_SS1_SPLIT_NATIVE_10:
  case VD1941_GS_SS1_RGB_10:
  case VD1941_GS_SS1_IR_10:
  case VD1941_GS_SS1_SPLIT_IR_10:
  case VDx941_GS_SS2_MONO_10:
  case VDx941_GS_SS4_MONO_10:
  case VDx941_GS_SS32_MONO_10:
  case VDx941_RS_SDR_NATIVE_10:
  case VD1941_RS_SDR_RGB_10:
  case VDx941_RS_HDR_NATIVE_10:
  case VD1941_RS_HDR_RGB_10:
    dt = 0x2b;
    break;
  case VDx941_RS_SDR_NATIVE_12:
  case VD1941_RS_SDR_RGB_12:
  case VDx941_RS_HDR_NATIVE_12:
  case VD1941_RS_HDR_RGB_12:
    dt = 0x2c;
    break;
  default:
    assert(0);
    return -1;
  }

  ret = ctx->write8(ctx, VDx941_REG_ROI_A_DT, dt);
  VDx941_TraceError(ctx, ret);

  return ret;
}

static int VDx941_SetupOutput(VDx941_Ctx_t *ctx)
{
  VDx941_OutItf_Config_t *out_itf = &ctx->ctx.config_save.out_itf;
  int ret;

  /* setup sensor mode */
  ret = ctx->write32(ctx, VDx941_REG_SENSOR_CONFIGURATION(0), ctx->ctx.config_save.image_processing_mode);
  VDx941_TraceError(ctx, ret);

  /* csi-2 output itf */
   /* lane number */
  ret = ctx->write8(ctx, VDx941_REG_LANE_NB_SEL, VDx941_GetLaneNb(out_itf));
  VDx941_TraceError(ctx, ret);
  /* logical / physical mapping */
  ret = ctx->write8(ctx, VDx941_REG_OIF_LANE_PHY_MAP, VDx941_GetLogicalPhysicalMapping(out_itf));
  VDx941_TraceError(ctx, ret);
  /* lane swap */
  ret = ctx->write8(ctx, VDx941_REG_OIF_LANE_PHY_SWAP, VDx941_GetLaneSwap(out_itf));
  VDx941_TraceError(ctx, ret);
   /* data rate */
  ret = ctx->write32(ctx, VDx941_REG_MIPI_DATA_RATE, out_itf->data_rate_in_mps);
  VDx941_TraceError(ctx, ret);
   /* data type / RAW8*/
  ret = VDx941_SetupOutputDataType(ctx);
  if (ret)
    return ret;

  return 0;
}

static int VDx941_SetupSize(VDx941_Ctx_t *ctx)
{
  const struct vdx941_rect *rect = &vdx941_supported_modes[ctx->ctx.config_save.resolution];
  int ss_factor;
  int ret;

  assert(rect->left % 4 == 0);
  assert(rect->top % 4 == 0);
  assert(rect->width % 4 == 0);
  assert(rect->height % 4 == 0);

  ret = VDx941_GetSubSamplingFactor(ctx, &ss_factor);
  if (ret)
    return ret;

  ret = ctx->write16(ctx, VDx941_REG_ROI_A_WIDTH_OFFSET, rect->left);
  VDx941_TraceError(ctx, ret);
  ret = ctx->write16(ctx, VDx941_REG_ROI_A_HEIGHT_OFFSET, rect->top);
  VDx941_TraceError(ctx, ret);
  ret = ctx->write16(ctx, VDx941_REG_ROI_A_WIDTH, rect->width / ss_factor);
  VDx941_TraceError(ctx, ret);
  ret = ctx->write16(ctx, VDx941_REG_ROI_A_HEIGHT, rect->height / ss_factor);
  VDx941_TraceError(ctx, ret);

  return 0;
}

static int VDx941_SetupLineLen(VDx941_Ctx_t *ctx)
{
  uint32_t system_pll_clk;
  uint16_t line_length;
  int ret;

  ret = ctx->read32(ctx, VDx941_REG_SYSTEM_PLL_CLK, &system_pll_clk);
  VDx941_TraceError(ctx, ret);
  VDx941_dbg(ctx, 0, "system pll clock = %d hz\n", system_pll_clk);

  /* moving *4 outside insure line_length is a multiple of 4 */
  line_length = ((VDx941_LINE_TIME_IN_NS * (uint64_t)system_pll_clk) / 16000000000) * 4;
  VDx941_dbg(ctx, 0, "line_length = %d\n", line_length);

  ret = ctx->write16(ctx, VDx941_REG_LINE_LENGTH, line_length);
  VDx941_TraceError(ctx, ret);

  return 0;
}

static int VDx941_ComputeFrameLength(VDx941_Ctx_t *ctx, int fps, uint16_t *frame_length)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;
  int min_frame_length;
  int req_frame_length;
  uint32_t system_pll_clk;
  uint16_t line_length;
  uint16_t height;
  int ret;

  ret = ctx->read32(ctx, VDx941_REG_SYSTEM_PLL_CLK, &system_pll_clk);
  VDx941_TraceError(ctx, ret);
  ret = ctx->read16(ctx, VDx941_REG_LINE_LENGTH, &line_length);
  VDx941_TraceError(ctx, ret);
  ret = ctx->read16(ctx, VDx941_REG_ROI_A_HEIGHT, &height);
  VDx941_TraceError(ctx, ret);

  min_frame_length = height + VDx941_MIN_VBLANK;
  req_frame_length =  system_pll_clk / (4 * line_length * drv_vdx941_ctx->config_save.frame_rate);
  *frame_length = MIN(MAX(min_frame_length, req_frame_length), 65535);

  VDx941_dbg(ctx, 0, "frame_length to MAX(%d, %d) = %d to reach %d fps\n", min_frame_length, req_frame_length,
             *frame_length, fps);

  return 0;
}

static int VDx941_SetupFrameRate(VDx941_Ctx_t *ctx)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;
  uint16_t frame_length;
  int ret;

  ret = VDx941_SetupLineLen(ctx);
  if (ret)
    return ret;

  ret = VDx941_ComputeFrameLength(ctx, drv_vdx941_ctx->config_save.frame_rate, &frame_length);
  if (ret)
    return ret;

  VDx941_dbg(ctx, 0, "Set frame_length to %d to reach %d fps\n", frame_length, drv_vdx941_ctx->config_save.frame_rate);
  ret = ctx->write16(ctx, VDx941_REG_FRAME_LENGTH(0), frame_length);
  VDx941_TraceError(ctx, ret);

 return 0;
}

static int VDx941_SetupMirrorFlip(VDx941_Ctx_t *ctx)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;
  uint8_t mode;
  int ret;

  switch (drv_vdx941_ctx->config_save.flip_mirror_mode) {
  case VDx941_MIRROR_FLIP_NONE:
    mode = 0;
    break;
  case VDx941_FLIP:
    mode = 2;
    break;
  case VDx941_MIRROR:
    mode = 1;
    break;
  case VDx941_MIRROR_FLIP:
    mode = 3;
    break;
  default:
    return -1;
  }

  ret = ctx->write8(ctx, VDx941_REG_ORIENTATION, mode);
  VDx941_TraceError(ctx, ret);

  return 0;
}

static int VDx941_SetupPatGen(VDx941_Ctx_t *ctx)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;
  uint16_t reg16;
  int ret;

  switch (drv_vdx941_ctx->config_save.patgen) {
  case VDx941_PATGEN_DISABLE:
    reg16 = VDx941_PATGEN_CTRL_DISABLE;
    break;
  case VDx941_PATGEN_DIAGONAL_GRAYSCALE:
    reg16 = VDx941_PATGEN_CTRL_DGREY;
    break;
  case VDx941_PATGEN_PSEUDO_RANDOM:
    reg16 = VDx941_PATGEN_CTRL_PN28;
    break;
  default:
    return -1;
  }

  ret = ctx->write16(ctx, VDx941_REG_PATGEN_CTRL, reg16);
  VDx941_TraceError(ctx, ret);

  return 0;
}

static int VDx941_Gpios(VDx941_Ctx_t *ctx)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;
  int ret;
  int i;

  for (i = 0; i < VDx941_GPIO_NB; i++) {
    ret = ctx->write8(ctx, VDx941_REG_GPIO_x_CTRL(i), drv_vdx941_ctx->config_save.gpios[i].gpio_ctrl);
    VDx941_TraceError(ctx, ret);
    ret = ctx->write8(ctx, VDx941_REG_GPIO_CTRL(0), drv_vdx941_ctx->config_save.gpios[i].enable);
    VDx941_TraceError(ctx, ret);
  }

  return 0;
}

static int VDx941_VT_Sync(VDx941_Ctx_t *ctx)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;
  int i;
  int ret;

  /* If Slave mode is enabled, ensure at lease one GPIO is configured accordingly */
  if (drv_vdx941_ctx->config_save.sync_mode == VDx941_SLAVE) {
    for (i = 0; i < VDx941_GPIO_NB; i++) {
      if (((drv_vdx941_ctx->config_save.gpios[i].gpio_ctrl & VDx941_GPIO_FSYNC_IN) == VDx941_GPIO_FSYNC_IN) &&
        (drv_vdx941_ctx->config_save.gpios[i].enable == VDx941_GPIO_ENABLE)) {
        break;
      }
    }

    if (i == VDx941_GPIO_NB) {
      // Slave mode is selected but no GPIO is configured
      return -1;
    }
  }

  ret = ctx->write8(ctx, VDx941_REG_VT_CTRL, drv_vdx941_ctx->config_save.sync_mode);
  VDx941_TraceError(ctx, ret);

  return 0;
}

static int VDx941_SafetyDisable(VDx941_Ctx_t *ctx)
{
  int ret;

  ret = ctx->write8(ctx, VDx941_REG_DIAG_DISABLE_FW_0_ERR, VDx941_DIAG_DISABLE_FW_0_ERR_UI_CRC);
  VDx941_TraceError(ctx, ret);
  ret = ctx->write8(ctx, VDx941_REG_DIAG_DISABLE_FW_1_ERR, VDx941_DIAG_DISABLE_FW_1_ERR_TASK_MONITOR);
  VDx941_TraceError(ctx, ret);
  ret = ctx->write8(ctx, VDx941_REG_DIAG_DISABLE_STREAMING_ERR, VDx941_DIAG_DISABLE_STREAMING_ERR_ALL);
  VDx941_TraceError(ctx, ret);

  return 0;
}

static int VDx941_Setup(VDx941_Ctx_t *ctx)
{
  int ret;

  ret = VDx941_SetupOutput(ctx);
  if (ret)
    return ret;

  ret = VDx941_SetupSize(ctx);
  if (ret)
    return ret;

  ret = VDx941_SetupFrameRate(ctx);
  if (ret)
    return ret;

  ret = VDx941_SetupMirrorFlip(ctx);
  if (ret)
    return ret;

  ret = VDx941_SetupPatGen(ctx);
  if (ret)
    return ret;

  ret = VDx941_Gpios(ctx);
  if (ret)
    return ret;

  ret = VDx941_VT_Sync(ctx);
  if (ret)
    return ret;

  ret = VDx941_SafetyDisable(ctx);
  if (ret)
    return ret;

  return 0;
}

static int VDx941_StartStreaming(VDx941_Ctx_t *ctx)
{
  int ret;

  ret = VDx941_ApplyCmdAndWait(ctx, VDx941_REG_SW_STBY, VDx941_CMD_START_STREAMING);
  if (ret)
    return ret;

  ret = VDx941_WaitState(ctx, VDx941_STREAMING);
  if (ret)
    return ret;

  VDx941_notice(ctx, "Streaming is on\n");

  return 0;
}

static int VDx941_StopStreaming(VDx941_Ctx_t *ctx)
{
  int ret;

  ret = VDx941_ApplyCmdAndWait(ctx, VDx941_REG_STREAMING, VDx941_CMD_STOP_STREAMING);
  if (ret)
    return ret;

  ret = VDx941_WaitState(ctx, VDx941_SW_STBY);
  if (ret)
    return ret;

  VDx941_notice(ctx, "Streaming is on\n");

  return 0;
}

static int VDx941_CheckConfig(VDx941_Ctx_t *ctx, VDx941_Config_t *config)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;

  if (config->ext_clock_freq_in_hz < VDx941_MIN_CLOCK_FREQ || config->ext_clock_freq_in_hz > VDx941_MAX_CLOCK_FREQ) {
    VDx941_error(ctx, "ext_clock_freq_in_hz out of range");
    return -1;
  }

  if (config->out_itf.data_rate_in_mps < VDx941_MIN_DATARATE || config->out_itf.data_rate_in_mps > VDx941_MAX_DATARATE) {
    VDx941_error(ctx, "data_rate_in_mps out of range");
    return -1;
  }

  if (config->out_itf.datalane_nb != 2 && config->out_itf.datalane_nb != 4) {
    VDx941_error(ctx, "datalane_nb out of range");
    return -1;
  }

  if (config->resolution < VDx941_RES_QVGA_320_240 ||
      config->resolution > VDx941_RES_FULL_2560_1984) {
    VDx941_error(ctx, "resolution out of range");
    return -1;
  }

  switch (config->image_processing_mode) {
  case VDx941_GS_SS1_NATIVE_8:
  case VDx941_GS_SS1_NATIVE_10:
  case VDx941_GS_SS1_SPLIT_NATIVE_8:
  case VDx941_GS_SS1_SPLIT_NATIVE_10:
  case VDx941_GS_SS2_MONO_8:
  case VDx941_GS_SS2_MONO_10:
  case VDx941_GS_SS4_MONO_8:
  case VDx941_GS_SS4_MONO_10:
  case VDx941_GS_SS32_MONO_8:
  case VDx941_GS_SS32_MONO_10:
  case VDx941_RS_SDR_NATIVE_8:
  case VDx941_RS_SDR_NATIVE_10:
  case VDx941_RS_SDR_NATIVE_12:
  case VDx941_RS_HDR_NATIVE_10:
  case VDx941_RS_HDR_NATIVE_12:
    break;
  case VD1941_GS_SS1_RGB_8:
  case VD1941_GS_SS1_RGB_10:
  case VD1941_GS_SS1_IR_8:
  case VD1941_GS_SS1_IR_10:
  case VD1941_GS_SS1_SPLIT_IR_8:
  case VD1941_GS_SS1_SPLIT_IR_10:
  case VD1941_RS_SDR_RGB_8:
  case VD1941_RS_SDR_RGB_10:
  case VD1941_RS_SDR_RGB_12:
  case VD1941_RS_HDR_RGB_10:
  case VD1941_RS_HDR_RGB_12:
    // Unsupported modes for VD5941 (Mono) variant
    if (drv_vdx941_ctx->is_mono)
      return -1;
    else
      break;
  default:
    return -1;
  }

  return 0;
}

static int VDx941_SetFctPtr(VDx941_Ctx_t *ctx)
{
  switch (ctx->ctx.config_save.image_processing_mode) {
  case VDx941_GS_SS1_NATIVE_8:
  case VDx941_GS_SS1_NATIVE_10:
    ctx->ctx.set_digital_gain = VDx941_SetDigitalGain_RGBNIR;
    ctx->ctx.set_expo = VDx941_SetExpo_GS;
    ctx->ctx.get_expo = VDx941_GetExpo_GS;
    break;
  case VDx941_GS_SS1_SPLIT_NATIVE_8:
  case VDx941_GS_SS1_SPLIT_NATIVE_10:
  case VD1941_GS_SS1_SPLIT_IR_8:
  case VD1941_GS_SS1_SPLIT_IR_10:
    ctx->ctx.set_digital_gain = VDx941_SetDigitalGain_RGBNIR;
    ctx->ctx.set_expo = VDx941_SetExpoSplit_GS;
    ctx->ctx.get_expo = VDx941_GetExpo_GS;
    break;
  case VD1941_GS_SS1_RGB_8:
  case VD1941_GS_SS1_RGB_10:
    ctx->ctx.set_digital_gain = VDx941_SetDigitalGain_RGB;
    ctx->ctx.set_expo = VDx941_SetExpo_GS;
    ctx->ctx.get_expo = VDx941_GetExpo_GS;
    break;
  case VD1941_GS_SS1_IR_8:
  case VD1941_GS_SS1_IR_10:
  case VDx941_GS_SS2_MONO_8:
  case VDx941_GS_SS2_MONO_10:
  case VDx941_GS_SS4_MONO_8:
  case VDx941_GS_SS4_MONO_10:
  case VDx941_GS_SS32_MONO_8:
  case VDx941_GS_SS32_MONO_10:
    ctx->ctx.set_digital_gain = VDx941_SetDigitalGain_IR;
    ctx->ctx.set_expo = VDx941_SetExpo_GS;
    ctx->ctx.get_expo = VDx941_GetExpo_GS;
    break;
  case VDx941_RS_SDR_NATIVE_8:
  case VDx941_RS_SDR_NATIVE_10:
  case VDx941_RS_SDR_NATIVE_12:
    ctx->ctx.set_digital_gain = VDx941_SetDigitalGain_RGBNIR;
    ctx->ctx.set_expo = VDx941_SetExpo_RS;
    ctx->ctx.get_expo = VDx941_GetExpo_RS;
  case VD1941_RS_SDR_RGB_8:
  case VD1941_RS_SDR_RGB_10:
  case VD1941_RS_SDR_RGB_12:
    ctx->ctx.set_digital_gain = VDx941_SetDigitalGain_RGB;
    ctx->ctx.set_expo = VDx941_SetExpo_RS;
    ctx->ctx.get_expo = VDx941_GetExpo_RS;
    break;
  case VDx941_RS_HDR_NATIVE_10:
  case VDx941_RS_HDR_NATIVE_12:
  case VD1941_RS_HDR_RGB_10:
  case VD1941_RS_HDR_RGB_12:
    ctx->ctx.set_digital_gain = VDx941_SetDigitalGain_RGB;
    ctx->ctx.set_expo = VDx941_SetExpo_RS_HDR;
    ctx->ctx.get_expo = VDx941_GetExpo_RS;
    break;
  default:
    assert(0);
    return -1;
  }

  return 0;
}

int VDx941_Init(VDx941_Ctx_t *ctx, VDx941_Config_t *config)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;
  int ret;

  drv_vdx941_ctx->config_save = *config;
  drv_vdx941_ctx->digital_gain = 0x100;

/* On cut 1.4, below code should be move in 'VDx941_CheckModelId' and rely on optical_register read*/
#ifdef USE_VD5941_SENSOR
  drv_vdx941_ctx->is_mono = 1;
#else
  drv_vdx941_ctx->is_mono = 0;
#endif

  ctx->shutdown_pin(ctx, 0);
  ctx->delay(ctx, 10);
  ctx->shutdown_pin(ctx, 1);
  ctx->delay(ctx, 10);

  ret = VDx941_CheckConfig(ctx, config);
  if (ret)
    return ret;

  ret = VDx941_SetFctPtr(ctx);
  if (ret)
    return ret;

  ret = VDx941_Boot(ctx);
  if (ret)
    return ret;

  drv_vdx941_ctx->state = VDx941_ST_IDLE;

  return 0;
}

int VDx941_DeInit(VDx941_Ctx_t *ctx)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;

  if (drv_vdx941_ctx->state == VDx941_ST_STREAMING)
    return -1;

  ctx->shutdown_pin(ctx, 0);
  ctx->delay(ctx, 10);

  return 0;
}

//#define DUMP_REG
#ifdef DUMP_REG
static void dump_reg8(VDx941_Ctx_t *ctx, char *name, int addr)
{
  uint8_t reg;
  int ret;

  ret = ctx->read8(ctx, addr, &reg);
  assert(ret == 0);

  VDx941_notice(ctx, "Read %s - 0x%04x => 0x%02x (%d)\n", name, addr, reg, reg);
}

static void dump_reg16(VDx941_Ctx_t *ctx, char *name, int addr)
{
  uint16_t reg;
  int ret;

  ret = ctx->read16(ctx, addr, &reg);
  assert(ret == 0);

  VDx941_notice(ctx, "Read %s - 0x%04x => 0x%04x (%d)\n", name, addr, reg, reg);
}

static void dump_reg32(VDx941_Ctx_t *ctx, char *name, int addr)
{
  uint32_t reg;
  int ret;

  ret = ctx->read32(ctx, addr, &reg);
  assert(ret == 0);

  VDx941_notice(ctx, "Read %s - 0x%04x => 0x%08x (%d)\n", name, addr, reg, reg);
}

static void dump_reg(VDx941_Ctx_t *ctx, char *name, int addr, int bit_nb)
{
  if (bit_nb == 8)
    dump_reg8(ctx, name, addr);
  else if (bit_nb == 16)
    dump_reg16(ctx, name, addr);
  else if (bit_nb == 32)
    dump_reg32(ctx, name, addr);
  else
    assert(0);
}

static void VDx941_Dbg(VDx941_Ctx_t *ctx)
{
  ctx->delay(ctx, 100);
  dump_reg(ctx, "SYSTEM_FSM_STATE", 0x44, 8);
  dump_reg(ctx, "MIPI_LANE_NB", 0x45, 8);
  dump_reg(ctx, "SYSTEM_WARNING", 0x46, 16);
  dump_reg(ctx, "SYSTEM_ERROR", 0x48, 16);
  dump_reg(ctx, "FRAME_COUNTER", 0x6e, 16);
  dump_reg(ctx, "OUTPUT_FORMAT", 0x79, 8);
  dump_reg(ctx, "VIRTUAL_CHANNEL", 0x7a, 8);
  dump_reg(ctx, "ROI_SELECTION", 0x7b, 8);
  dump_reg(ctx, "ROI_A_OFFSET", 0x7c, 32);
  dump_reg(ctx, "ROI_A_SIZE", 0x80, 32);
  dump_reg(ctx, "ROI_A_DT", 0x9c, 8);
  dump_reg(ctx, "LINE_LENGTH", 0xb2, 16);
  dump_reg(ctx, "FRAME_LENGTH", 0xb4, 32);
  dump_reg(ctx, "READOUT_CTRL", 0xb8, 8);
  dump_reg(ctx, "IMAGE_CONFIG", 0xbc, 8);
  dump_reg(ctx, "SHUTTER_MODE", 0xbd, 8);
  dump_reg(ctx, "X_START", 0xc8, 16);
  dump_reg(ctx, "X_END", 0xca, 16);
  dump_reg(ctx, "Y_START", 0xcc, 16);
  dump_reg(ctx, "Y_END", 0xce, 16);
  dump_reg(ctx, "X_SIZE", 0xd0, 16);
  dump_reg(ctx, "Y_SIZE", 0xd2, 16);
  dump_reg(ctx, "OIF_LANE_PHY_MAP", 0x16c, 8);
  dump_reg(ctx, "OIF_LANE_PHY_SWAP", 0x16d, 8);
  dump_reg(ctx, "STREAMING_FSM", 0x201, 8);
  dump_reg(ctx, "SYSTEM_PLL_CLK", 0x228, 32);
  dump_reg(ctx, "PIXEL_CLK", 0x22c, 32);
  dump_reg(ctx, "MCU_CLK", 0x230, 32);
  dump_reg(ctx, "FRAME_RATE", 0x236, 16);
  dump_reg(ctx, "WAIT_DELAY", 0x2be, 16);
}
#endif

int VDx941_Start(VDx941_Ctx_t *ctx)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;
  int ret;

  if (drv_vdx941_ctx->state != VDx941_ST_IDLE)
    return -1;

  ret = VDx941_Setup(ctx);
  if (ret)
    return ret;

  ret = VDx941_StartStreaming(ctx);
  if (ret)
    return ret;
  drv_vdx941_ctx->state = VDx941_ST_STREAMING;

#ifdef DUMP_REG
  VDx941_Dbg(ctx);
#endif

  return 0;
}

int VDx941_Stop(VDx941_Ctx_t *ctx)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;
  int ret;

  if (drv_vdx941_ctx->state != VDx941_ST_STREAMING)
    return -1;

  ret = VDx941_StopStreaming(ctx);
  if (ret)
    return ret;
  drv_vdx941_ctx->state = VDx941_ST_IDLE;

  return 0;
}

int VDx941_Hold(VDx941_Ctx_t *ctx, int is_enable)
{
  int ret;

  ret = ctx->write8(ctx, VDx941_REG_GROUP_PARAMETER_HOLD(0),
                    is_enable ? VDx941_GROUP_PARAMETER_HOLD_ENABLE : VDx941_GROUP_PARAMETER_HOLD_DISABLE);
  VDx941_TraceError(ctx, ret);

  return 0;
}

int VDx941_SetAnalogGain(VDx941_Ctx_t *ctx, unsigned int gain)
{
  int ret;

  if (gain > VDx941_ANALOG_GAIN_MAX)
    return -1;

  ret = ctx->write8(ctx, VDx941_REG_ANALOG_GAIN(0), gain);
  VDx941_TraceError(ctx, ret);

  return 0;
}

int VDx941_GetAnalogGain(VDx941_Ctx_t *ctx, unsigned int *gain)
{
  uint8_t reg8;
  int ret;

  ret = ctx->read8(ctx, VDx941_REG_ANALOG_GAIN(0), &reg8);
  VDx941_TraceError(ctx, ret);
  *gain = reg8;

  return 0;
}

int VDx941_SetDigitalGain(VDx941_Ctx_t *ctx, unsigned int gain)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;
  int ret;

  if (gain > VDx941_DIGITAL_GAIN_MAX)
    return -1;

  assert(drv_vdx941_ctx->set_digital_gain);
  ret = drv_vdx941_ctx->set_digital_gain(ctx, gain);
  if (ret)
    return ret;
  ctx->ctx.digital_gain = gain;

  return 0;
}

int VDx941_GetDigitalGain(VDx941_Ctx_t *ctx, unsigned int *gain)
{
  return ctx->ctx.digital_gain;
}

int VDx941_SetExpo(VDx941_Ctx_t *ctx, unsigned int expo_in_us)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;

  assert(drv_vdx941_ctx->set_expo);
  return drv_vdx941_ctx->set_expo(ctx, expo_in_us);
}

int VDx941_GetExpo(VDx941_Ctx_t *ctx, unsigned int *expo_in_us)
{
  struct drv_vdx941_ctx *drv_vdx941_ctx = &ctx->ctx;

  assert(drv_vdx941_ctx->get_expo);
  return drv_vdx941_ctx->get_expo(ctx, expo_in_us);
}
