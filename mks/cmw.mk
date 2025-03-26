CMW_REL_DIR := Lib/Camera_Middleware
ISP_REL_DIR := $(CMW_REL_DIR)/ISP_Library

C_SOURCES_CMW += $(CMW_REL_DIR)/cmw_camera.c
C_SOURCES_CMW += $(CMW_REL_DIR)/cmw_utils.c
C_SOURCES_CMW += $(CMW_REL_DIR)/sensors/cmw_vd55g1.c
C_SOURCES_CMW += $(CMW_REL_DIR)/sensors/vd55g1/vd55g1.c
C_SOURCES_CMW += $(CMW_REL_DIR)/sensors/cmw_ov5640.c
C_SOURCES_CMW += $(CMW_REL_DIR)/sensors/ov5640/ov5640_reg.c
C_SOURCES_CMW += $(CMW_REL_DIR)/sensors/ov5640/ov5640.c
C_SOURCES_CMW += $(CMW_REL_DIR)/sensors/cmw_imx335.c
C_SOURCES_CMW += $(CMW_REL_DIR)/sensors/imx335/imx335.c
C_SOURCES_CMW += $(CMW_REL_DIR)/sensors/imx335/imx335_reg.c
C_SOURCES_CMW += $(CMW_REL_DIR)/sensors/cmw_vd66gy.c
C_SOURCES_CMW += $(CMW_REL_DIR)/sensors/vd6g/vd6g.c
C_SOURCES_CMW += $(CMW_REL_DIR)/sensors/cmw_vd1941.c
C_SOURCES_CMW += $(CMW_REL_DIR)/sensors/vd1941/vdx941.c
C_SOURCES_CMW += $(ISP_REL_DIR)/isp/Src/isp_algo.c
C_SOURCES_CMW += $(ISP_REL_DIR)/isp/Src/isp_cmd_parser.c
C_SOURCES_CMW += $(ISP_REL_DIR)/isp/Src/isp_core.c
C_SOURCES_CMW += $(ISP_REL_DIR)/isp/Src/isp_services.c
C_SOURCES_CMW += $(ISP_REL_DIR)/isp/Src/isp_tool_com.c
C_INCLUDES_CMW += -I$(CMW_REL_DIR)
C_INCLUDES_CMW += -I$(CMW_REL_DIR)/sensors
C_INCLUDES_CMW += -I$(CMW_REL_DIR)/sensors/imx335
C_INCLUDES_CMW += -I$(CMW_REL_DIR)/sensors/ov5640
C_INCLUDES_CMW += -I$(CMW_REL_DIR)/sensors/vd55g1
C_INCLUDES_CMW += -I$(CMW_REL_DIR)/sensors/vd6g
C_INCLUDES_CMW += -I$(CMW_REL_DIR)/sensors/vd1941
C_INCLUDES_CMW += -I$(ISP_REL_DIR)/isp/Inc
C_INCLUDES_CMW += -I$(ISP_REL_DIR)/evision/Inc

ifeq ($(SENSOR),IMX335)
C_DEFS_CMW += -DUSE_IMX335_SENSOR
endif
ifeq ($(SENSOR),VD66GY)
C_DEFS_CMW += -DUSE_VD66GY_SENSOR 
endif
ifeq ($(SENSOR),OV5640)
C_DEFS_CMW += -DUSE_OV5640_SENSOR
endif
ifeq ($(SENSOR),VD55G1)
C_DEFS_CMW += -DUSE_VD55G1_SENSOR
endif
ifeq ($(SENSOR),VD1941)
C_DEFS_CMW += -DUSE_VD1941_SENSOR
endif

C_SOURCES += $(C_SOURCES_CMW)
C_INCLUDES += $(C_INCLUDES_CMW)
C_DEFS += $(C_DEFS_CMW)
LIBS += -ln6-evision
LIBDIR += -L$(ISP_REL_DIR)/evision/Lib
LIBS_IAR += $(ISP_REL_DIR)/evision/Lib/libn6-evision_IAR.a
