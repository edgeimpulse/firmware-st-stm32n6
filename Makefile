######################################
# quiet mode
######################################
V = 0
ifeq ($(V), 0)
  quiet = quiet_
else
  quiet =
endif
quiet_CC  = @echo "  CC $@"; $(CC)
quiet_LD  = @echo "  LD $@"; $(CC)
quiet_AS  = @echo "  AS $@"; $(AS)
quiet_SZ  = @echo "  SZ $@"; $(SZ)
quiet_HEX  = @echo "  HEX $@"; $(HEX)
quiet_BIN  = @echo "  BIN $@"; $(BIN)

######################################
# helpers
######################################
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

######################################
# target
######################################
TARGET = Project
 # Supported Options: IMX335; OV5640; VD66GY; VD55G1; VD1941
SENSOR = IMX335
 # Supported Options: C01; B01; A01; A03
REV_BOARD = C01

MODEL_DIR = Model
BINARY_DIR = Binary

######################################
# building variables
######################################
OPT = -O3 -g3

#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES += Src/main.c
C_SOURCES += Src/app.c
C_SOURCES += Src/app_fuseprogramming.c
C_SOURCES += Src/stm32_lcd_ex.c
C_SOURCES += Src/stm32n6xx_it.c
C_SOURCES += Src/mcu_cache.c
C_SOURCES += Model/network.c
C_SOURCES += Src/app_cam.c
C_SOURCES += Src/threadx_hal.c
C_SOURCES += Src/sysmem.c

# ASM sources
ASM_SOURCES =
ASM_SOURCES_S =

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
CXX = $(GCC_PATH)/$(PREFIX)g++
READELF = $(GCC_PATH)/$(PREFIX)readelf
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
CP = $(PREFIX)objcopy
CXX = $(PREFIX)g++
READELF = $(PREFIX)readelf
endif
LD = $(CC)
HEX = $(CP) -O ihex
BIN = $(CP) -O binary
FLASHER = STM32_Programmer_CLI
SIGNER = STM32MP_SigningTool_CLI
OBJCOPY = arm-none-eabi-objcopy
EL = "$(shell dirname "$(shell which $(FLASHER))")/ExternalLoader/MX66UW1G45G_STM32N6570-DK.stldr"

#######################################
# CFLAGS
#######################################
CPU = -mcpu=cortex-m55 -mcmse -mthumb
FPU = -mfpu=fpv5-d16 -mfloat-abi=hard

# mcu
MCU = $(CPU) $(FPU)

# C defines
C_DEFS += -DSTM32N657xx
C_DEFS += -DUSE_FULL_ASSERT
C_DEFS += -DUSE_FULL_LL_DRIVER
C_DEFS += -DVECT_TAB_SRAM
ifeq ($(REV_BOARD),B01)
C_DEFS += -DSTM32N6570_DK_REV=STM32N6570_DK_B01
endif
ifeq ($(REV_BOARD),C01)
C_DEFS += -DSTM32N6570_DK_REV=STM32N6570_DK_C01
endif
ifeq ($(REV_BOARD),A01)
C_DEFS += -DSTM32N6570_DK_REV=STM32N6570_DK_A01
endif
ifeq ($(REV_BOARD),A03)
#this is not a typo
C_DEFS += -DSTM32N6570_DK_REV=STM32N6570_DK_A01
endif

ifneq ($(REV_BOARD),C01)
C_DEFS += -DSTM32N6XX_SI_CUT1_1
AS_DEFS += -DSTM32N6XX_SI_CUT1_1
endif

# We only support single model
C_DEFS += -DTX_MAX_PARALLEL_NETWORKS=1

# C includes
# Patched files
C_INCLUDES += -IInc
CXX_VERSION_FLAGS = -std=gnu++11

# For now using the C flags for C++ as well
CXXFLAGS = $(MCU) $(C_DEFS) $(CXX_INCLUDES) $(OPT) $(CXX_VERSION_FLAGS)


ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fstack-usage -fdata-sections -ffunction-sections #-fcyclomatic-complexity
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fstack-usage -fdata-sections -ffunction-sections #-fcyclomatic-complexity
# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

CFLAGS += -std=gnu11

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = Gcc/STM32N657xx.ld

# libraries
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) $(LDFLAGS_OTHERS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections,-lstdc++
# Uncomment to enable %f formatted output
LDFLAGS_OTHERS += -u _printf_float
# Avoid 'build/Project.elf has a LOAD segment with RWX permissions' warning
LDFLAGS_OTHERS += -Wl,--no-warn-rwx-segments
LDFLAGS += -Wl,--print-memory-usage

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

#######################################
# Include mk files
#######################################
include mks/ei.mk
include mks/fw.mk
include mks/ai.mk
include mks/cmw.mk
include mks/threadx.mk
include mks/gcc.mk
include mks/iar.mk

CXX_INCLUDES += $(C_INCLUDES_THREADX)

#######################################
# build the application
#######################################
OBJECTS = $(addprefix $(BUILD_DIR)/, $(C_SOURCES:.c=.o))
OBJECTS += $(addprefix $(BUILD_DIR)/, $(CXX_SOURCES:.cpp=.o))
OBJECTS += $(addprefix $(BUILD_DIR)/, $(CC_SOURCES:.cc=.o))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(ASM_SOURCES:.s=.o))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(ASM_SOURCES_S:.S=.o))


$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$($(quiet)CC)  -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cpp=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.cc Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cc=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$($(quiet)CC)  -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$($(quiet)AS) -c $(CFLAGS) $< -o $@

# $(BUILD_DIR)/$(TARGET).list: $(OBJECTS)
# 	$(file > $@, $(OBJECTS))

# $(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile $(BUILD_DIR)/$(TARGET).list
# 	$($(quiet)LD) @$(BUILD_DIR)/$(TARGET).list $(LDFLAGS) -o $@
# 	$($(quiet)SZ) $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$($(quiet)HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$($(quiet)BIN) $< $@

$(BUILD_DIR)/$(TARGET)_sign.bin: $(BUILD_DIR)/$(TARGET).bin
	$(SIGNER) -s -bin $< -nk -t fsbl -hv 2.1 -o $(BUILD_DIR)/$(TARGET)_sign.bin

$(BUILD_DIR):
	mkdir -p $@

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

#######################################
# flash
#######################################
flash: $(BUILD_DIR)/$(TARGET).bin
	@$(FLASHER) -c port=SWD mode=HOTPLUG ap=1 -el $(EL) -hardRst -w $< 0x70080000
	@echo FLASH $<

flash_sign: $(BUILD_DIR)/$(TARGET)_sign.bin
	@$(FLASHER) -c port=SWD mode=HOTPLUG ap=1 -el $(EL) -hardRst -w $< 0x70080000
	@echo FLASH $<

flash_weights: $(MODEL_DIR)/network_data.hex
	$(FLASHER) -c port=SWD mode=HOTPLUG ap=1 -el $(EL) -hardRst -w $<

#######################################
# dependencies
#######################################
-include $(call rwildcard,$(BUILD_DIR),*.d)
