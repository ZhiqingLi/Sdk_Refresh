################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bt_audio_app_src/device/QN8035.c \
../bt_audio_app_src/device/RDA5807.c \
../bt_audio_app_src/device/adc_key.c \
../bt_audio_app_src/device/adc_levels.c \
../bt_audio_app_src/device/breakpoint.c \
../bt_audio_app_src/device/code_key.c \
../bt_audio_app_src/device/deepsleep.c \
../bt_audio_app_src/device/device_detect.c \
../bt_audio_app_src/device/flash_boot.c \
../bt_audio_app_src/device/io_key.c \
../bt_audio_app_src/device/ir_key.c \
../bt_audio_app_src/device/ir_nec_key.c \
../bt_audio_app_src/device/key.c \
../bt_audio_app_src/device/power_monitor.c \
../bt_audio_app_src/device/string_convert.c \
../bt_audio_app_src/device/upgrade.c 

OBJS += \
./bt_audio_app_src/device/QN8035.o \
./bt_audio_app_src/device/RDA5807.o \
./bt_audio_app_src/device/adc_key.o \
./bt_audio_app_src/device/adc_levels.o \
./bt_audio_app_src/device/breakpoint.o \
./bt_audio_app_src/device/code_key.o \
./bt_audio_app_src/device/deepsleep.o \
./bt_audio_app_src/device/device_detect.o \
./bt_audio_app_src/device/flash_boot.o \
./bt_audio_app_src/device/io_key.o \
./bt_audio_app_src/device/ir_key.o \
./bt_audio_app_src/device/ir_nec_key.o \
./bt_audio_app_src/device/key.o \
./bt_audio_app_src/device/power_monitor.o \
./bt_audio_app_src/device/string_convert.o \
./bt_audio_app_src/device/upgrade.o 

C_DEPS += \
./bt_audio_app_src/device/QN8035.d \
./bt_audio_app_src/device/RDA5807.d \
./bt_audio_app_src/device/adc_key.d \
./bt_audio_app_src/device/adc_levels.d \
./bt_audio_app_src/device/breakpoint.d \
./bt_audio_app_src/device/code_key.d \
./bt_audio_app_src/device/deepsleep.d \
./bt_audio_app_src/device/device_detect.d \
./bt_audio_app_src/device/flash_boot.d \
./bt_audio_app_src/device/io_key.d \
./bt_audio_app_src/device/ir_key.d \
./bt_audio_app_src/device/ir_nec_key.d \
./bt_audio_app_src/device/key.d \
./bt_audio_app_src/device/power_monitor.d \
./bt_audio_app_src/device/string_convert.d \
./bt_audio_app_src/device/upgrade.d 


# Each subdirectory must supply rules for building sources it contributes
bt_audio_app_src/device/%.o: ../bt_audio_app_src/device/%.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -DFUNC_OS_EN=1 -DCFG_APP_CONFIG -DHAVE_CONFIG_H -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/inc/otg" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/audio/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/bluetooth/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/cec/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/fatfs/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/flashfs/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/lrc/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/mv_utils/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/rtc/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/rtos/freertos/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/rtos/rtos_api" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/user/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai/md5" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai/speex/include" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai/speex/include/speex" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai/speex" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai/speex/libspeex" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/audio" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/display" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/user" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/rtos/rtos_api" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/inc" -O3 -mcmodel=small -funroll-all-loops -fgcse-sm -finline-limit=500 -Wall -mcpu=d1088-spu -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '


