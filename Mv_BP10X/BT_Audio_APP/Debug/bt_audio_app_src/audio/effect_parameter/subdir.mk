################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bt_audio_app_src/audio/effect_parameter/DianYin.c \
../bt_audio_app_src/audio/effect_parameter/HanMai.c \
../bt_audio_app_src/audio/effect_parameter/HunXiang.c \
../bt_audio_app_src/audio/effect_parameter/MoYin.c \
../bt_audio_app_src/audio/effect_parameter/NanBianNv.c \
../bt_audio_app_src/audio/effect_parameter/NvBianNan.c \
../bt_audio_app_src/audio/effect_parameter/WaWaYin.c 

OBJS += \
./bt_audio_app_src/audio/effect_parameter/DianYin.o \
./bt_audio_app_src/audio/effect_parameter/HanMai.o \
./bt_audio_app_src/audio/effect_parameter/HunXiang.o \
./bt_audio_app_src/audio/effect_parameter/MoYin.o \
./bt_audio_app_src/audio/effect_parameter/NanBianNv.o \
./bt_audio_app_src/audio/effect_parameter/NvBianNan.o \
./bt_audio_app_src/audio/effect_parameter/WaWaYin.o 

C_DEPS += \
./bt_audio_app_src/audio/effect_parameter/DianYin.d \
./bt_audio_app_src/audio/effect_parameter/HanMai.d \
./bt_audio_app_src/audio/effect_parameter/HunXiang.d \
./bt_audio_app_src/audio/effect_parameter/MoYin.d \
./bt_audio_app_src/audio/effect_parameter/NanBianNv.d \
./bt_audio_app_src/audio/effect_parameter/NvBianNan.d \
./bt_audio_app_src/audio/effect_parameter/WaWaYin.d 


# Each subdirectory must supply rules for building sources it contributes
bt_audio_app_src/audio/effect_parameter/%.o: ../bt_audio_app_src/audio/effect_parameter/%.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -DFUNC_OS_EN=1 -DCFG_APP_CONFIG -DHAVE_CONFIG_H -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/audio" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai/md5" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai/speex/include/speex" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai/speex/include" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai/speex/libspeex" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai/speex" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/ai" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/inc/otg" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/rtc/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/fatfs/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/user/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/bluetooth/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/audio/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/mv_utils/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/lrc/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/rtos/rtos_api" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/rtos/freertos/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/flashfs/inc" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/display" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/BT_Audio_APP/bt_audio_app_src/user" -I"/cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/middleware/cec/inc" -Og -mcmodel=medium -g3 -Wall -mcpu=d1088-spu -c -fmessage-length=0 -ldsp -mext-dsp -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '


