################################################################################
# 自动生成的文件。不要编辑！
################################################################################

ifndef ANDESIGHT_ROOT
ANDESIGHT_ROOT=/cygdrive/C/Andestech/AndeSight300RDS
endif

ifndef CROSS_COMPILE
CROSS_COMPILE=nds32le-elf-
endif

ifndef SECONDARY_OUTPUT_PATH
SECONDARY_OUTPUT_PATH=output
endif

$(shell mkdir -p $(SECONDARY_OUTPUT_PATH))

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include bt_audio_app_src/ai/subdir.mk
-include bt_audio_app_src/ai/md5/subdir.mk
-include bt_audio_app_src/ai/speex/libspeex/subdir.mk
-include bt_audio_app_src/apps/subdir.mk
-include bt_audio_app_src/audio/subdir.mk
-include bt_audio_app_src/audio/effect_parameter/subdir.mk
-include bt_audio_app_src/audio/music_parameter/subdir.mk
-include bt_audio_app_src/device/subdir.mk
-include bt_audio_app_src/display/subdir.mk
-include bt_audio_app_src/subdir.mk
-include bt_audio_app_src/services/subdir.mk
-include bt_audio_app_src/startup/subdir.mk
-include bt_audio_app_src/user/subdir.mk
-include driver/driver_api/src/subdir.mk
-include driver/driver_api/src/otg/device/subdir.mk
-include driver/driver_api/src/otg/host/subdir.mk
-include middleware/audio/src/subdir.mk
-include middleware/bluetooth/src/subdir.mk
-include middleware/fatfs/subdir.mk
-include middleware/fatfs/src/subdir.mk
-include middleware/flashfs/src/subdir.mk
-include middleware/mv_utils/src/subdir.mk
-include middleware/rtos/freertos/src/subdir.mk
-include middleware/rtos/rtos_api/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
BUILD_ARTIFACT_FILE_BASE_NAME = BT_Audio_APP

LINKER_OUTPUTS = BT_Audio_APP.adx

GCOV_OUT += \
$(foreach subdir,$(SUBDIRS),$(wildcard $(subdir)/*.gcda) $(wildcard $(subdir)/*.gcno)) \

GPROF_OUT += \
../gmon.sum ../gmon.out \

SYMBOL_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/symbol.txt \
$(SECONDARY_OUTPUT_PATH)/symbol.txt \
$(SECONDARY_OUTPUT_PATH)/symbol.txt \

READELF_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/readelf.txt \
$(SECONDARY_OUTPUT_PATH)/readelf.txt \
$(SECONDARY_OUTPUT_PATH)/readelf.txt \

OBJDUMP_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/objdump.txt \
$(SECONDARY_OUTPUT_PATH)/objdump.txt \
$(SECONDARY_OUTPUT_PATH)/objdump.txt \

OBJCOPY_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/$(BUILD_ARTIFACT_FILE_BASE_NAME).bin \
$(SECONDARY_OUTPUT_PATH)/$(BUILD_ARTIFACT_FILE_BASE_NAME).bin \
$(SECONDARY_OUTPUT_PATH)/$(BUILD_ARTIFACT_FILE_BASE_NAME).bin \

SIZE_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/.PHONY.size \
$(SECONDARY_OUTPUT_PATH)/.PHONY.size \
$(SECONDARY_OUTPUT_PATH)/.PHONY.size \


# 所有目标
all: pre-build 
	$(MAKE) main-build

# 主构建目标
main-build: BT_Audio_APP.adx secondary-outputs

# 工具调用
BT_Audio_APP.adx: $(OBJS) $(USER_OBJS)
	@echo '正在构建目标： $@'
	@echo '正在调用： Andes C Linker'
	$(CROSS_COMPILE)gcc -Og -nostartfiles -static -L"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver" -L"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/cec" -L"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/audio" -L"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/rtc" -L"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/flashfs" -L"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/bluetooth" -L"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/lrc" -L"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/fatfs" -mcmodel=medium -ldsp -mext-dsp -T"../nds32-ae210p.ld" -Wl,--gc-sections -mvh -o "BT_Audio_APP.adx" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo '已结束构建目标： $@'
	@echo ' '

$(foreach subdir,$(SUBDIRS),$(wildcard $(subdir)/*.gcda) $(wildcard $(subdir)/*.gcno)): BT_Audio_APP.adx $(OBJS) $(USER_OBJS)
../gmon.sum ../gmon.out: BT_Audio_APP.adx $(OBJS) $(USER_OBJS)

$(SECONDARY_OUTPUT_PATH)/symbol.txt: $(LINKER_OUTPUTS)
	@echo '正在调用： NM (symbol listing)'
	$(CROSS_COMPILE)nm -n -l -C "BT_Audio_APP.adx" > $(SECONDARY_OUTPUT_PATH)/symbol.txt
	@echo '已结束构建： $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/readelf.txt: $(LINKER_OUTPUTS)
	@echo '正在调用： Readelf (ELF info listing)'
	$(CROSS_COMPILE)readelf -a "BT_Audio_APP.adx" > $(SECONDARY_OUTPUT_PATH)/readelf.txt
	@echo '已结束构建： $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/objdump.txt: $(LINKER_OUTPUTS)
	@echo '正在调用： Objdump (disassembly)'
	$(CROSS_COMPILE)objdump -x -d -C "BT_Audio_APP.adx" > $(SECONDARY_OUTPUT_PATH)/objdump.txt
	@echo '已结束构建： $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/$(BUILD_ARTIFACT_FILE_BASE_NAME).bin: $(LINKER_OUTPUTS)
	@echo '正在调用： Objcopy (object content copy)'
	$(CROSS_COMPILE)objcopy -S -O binary "BT_Audio_APP.adx" $(SECONDARY_OUTPUT_PATH)/$(BUILD_ARTIFACT_FILE_BASE_NAME).bin
	@echo '已结束构建： $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/.PHONY.size: $(LINKER_OUTPUTS)
	@echo '正在调用： Size (section size listing)'
	$(CROSS_COMPILE)size  "BT_Audio_APP.adx" | tee $(SECONDARY_OUTPUT_PATH)/.PHONY.size
	@echo '已结束构建： $@'
	@echo ' '

# 其他目标
clean:
	-$(RM) $(OBJCOPY_OUTPUTS)$(OBJS)$(C_DEPS)$(GCOV_OUT)$(SYMBOL_OUTPUTS)$(OBJDUMP_OUTPUTS)$(READELF_OUTPUTS)$(GPROF_OUT)$(SIZE_OUTPUTS)$(EXECUTABLES)$(S_UPPER_DEPS) BT_Audio_APP.adx
	-@echo ' '

pre-build:
	-..\script.bat
	-@echo ' '

secondary-outputs: $(SYMBOL_OUTPUTS) $(READELF_OUTPUTS) $(OBJDUMP_OUTPUTS) $(OBJCOPY_OUTPUTS) $(SIZE_OUTPUTS)

.PHONY: all clean dependents
.SECONDARY: main-build pre-build
.NOTPARALLEL: pre-build

-include ../makefile.targets
