################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bt_audio_app_src/apps/bt_hf_api.c \
../bt_audio_app_src/apps/bt_hf_mode.c \
../bt_audio_app_src/apps/bt_play_api.c \
../bt_audio_app_src/apps/bt_play_mode.c \
../bt_audio_app_src/apps/hdmi_in_api.c \
../bt_audio_app_src/apps/hdmi_in_mode.c \
../bt_audio_app_src/apps/i2sin_mode.c \
../bt_audio_app_src/apps/linein_mode.c \
../bt_audio_app_src/apps/main_task.c \
../bt_audio_app_src/apps/media_play_api.c \
../bt_audio_app_src/apps/media_play_mode.c \
../bt_audio_app_src/apps/mode_switch_api.c \
../bt_audio_app_src/apps/otg_device_audio.c \
../bt_audio_app_src/apps/radio_mode.c \
../bt_audio_app_src/apps/spdif_mode.c \
../bt_audio_app_src/apps/standby_mode.c \
../bt_audio_app_src/apps/usb_audio_mode.c 

OBJS += \
./bt_audio_app_src/apps/bt_hf_api.o \
./bt_audio_app_src/apps/bt_hf_mode.o \
./bt_audio_app_src/apps/bt_play_api.o \
./bt_audio_app_src/apps/bt_play_mode.o \
./bt_audio_app_src/apps/hdmi_in_api.o \
./bt_audio_app_src/apps/hdmi_in_mode.o \
./bt_audio_app_src/apps/i2sin_mode.o \
./bt_audio_app_src/apps/linein_mode.o \
./bt_audio_app_src/apps/main_task.o \
./bt_audio_app_src/apps/media_play_api.o \
./bt_audio_app_src/apps/media_play_mode.o \
./bt_audio_app_src/apps/mode_switch_api.o \
./bt_audio_app_src/apps/otg_device_audio.o \
./bt_audio_app_src/apps/radio_mode.o \
./bt_audio_app_src/apps/spdif_mode.o \
./bt_audio_app_src/apps/standby_mode.o \
./bt_audio_app_src/apps/usb_audio_mode.o 

C_DEPS += \
./bt_audio_app_src/apps/bt_hf_api.d \
./bt_audio_app_src/apps/bt_hf_mode.d \
./bt_audio_app_src/apps/bt_play_api.d \
./bt_audio_app_src/apps/bt_play_mode.d \
./bt_audio_app_src/apps/hdmi_in_api.d \
./bt_audio_app_src/apps/hdmi_in_mode.d \
./bt_audio_app_src/apps/i2sin_mode.d \
./bt_audio_app_src/apps/linein_mode.d \
./bt_audio_app_src/apps/main_task.d \
./bt_audio_app_src/apps/media_play_api.d \
./bt_audio_app_src/apps/media_play_mode.d \
./bt_audio_app_src/apps/mode_switch_api.d \
./bt_audio_app_src/apps/otg_device_audio.d \
./bt_audio_app_src/apps/radio_mode.d \
./bt_audio_app_src/apps/spdif_mode.d \
./bt_audio_app_src/apps/standby_mode.d \
./bt_audio_app_src/apps/usb_audio_mode.d 


# Each subdirectory must supply rules for building sources it contributes
bt_audio_app_src/apps/%.o: ../bt_audio_app_src/apps/%.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -DFUNC_OS_EN=1 -DCFG_APP_CONFIG -DHAVE_CONFIG_H -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/audio" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai/md5" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai/speex/include/speex" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai/speex/include" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai/speex/libspeex" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai/speex" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/inc/otg" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/rtc/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/fatfs/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/user/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/bluetooth/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/audio/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/mv_utils/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/lrc/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/rtos/rtos_api" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/rtos/freertos/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/flashfs/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/display" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/user" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/cec/inc" -Og -mcmodel=medium -g3 -Wall -mcpu=d1088-spu -c -fmessage-length=0 -ldsp -mext-dsp -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '


