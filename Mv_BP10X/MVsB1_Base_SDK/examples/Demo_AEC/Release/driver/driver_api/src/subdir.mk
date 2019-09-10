################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
F:/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/adc_interface.c \
F:/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/backup_interface.c \
F:/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/dac_interface.c \
F:/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/fft_api.c \
F:/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/i2c_host.c \
F:/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/i2c_interface.c \
F:/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/i2s_interface.c \
F:/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/ppwm_interface.c \
F:/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/sadc_interface.c \
F:/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/sd_card.c \
F:/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/spdif_interface.c \
F:/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/spim_interface.c \
F:/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/sw_uart.c \
F:/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/uarts_interface.c 

OBJS += \
./driver/driver_api/src/adc_interface.o \
./driver/driver_api/src/backup_interface.o \
./driver/driver_api/src/dac_interface.o \
./driver/driver_api/src/fft_api.o \
./driver/driver_api/src/i2c_host.o \
./driver/driver_api/src/i2c_interface.o \
./driver/driver_api/src/i2s_interface.o \
./driver/driver_api/src/ppwm_interface.o \
./driver/driver_api/src/sadc_interface.o \
./driver/driver_api/src/sd_card.o \
./driver/driver_api/src/spdif_interface.o \
./driver/driver_api/src/spim_interface.o \
./driver/driver_api/src/sw_uart.o \
./driver/driver_api/src/uarts_interface.o 

C_DEPS += \
./driver/driver_api/src/adc_interface.d \
./driver/driver_api/src/backup_interface.d \
./driver/driver_api/src/dac_interface.d \
./driver/driver_api/src/fft_api.d \
./driver/driver_api/src/i2c_host.d \
./driver/driver_api/src/i2c_interface.d \
./driver/driver_api/src/i2s_interface.d \
./driver/driver_api/src/ppwm_interface.d \
./driver/driver_api/src/sadc_interface.d \
./driver/driver_api/src/sd_card.d \
./driver/driver_api/src/spdif_interface.d \
./driver/driver_api/src/spim_interface.d \
./driver/driver_api/src/sw_uart.d \
./driver/driver_api/src/uarts_interface.d 


# Each subdirectory must supply rules for building sources it contributes
driver/driver_api/src/adc_interface.o: /cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/adc_interface.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -mcmodel=medium -Wall -mcpu=d1088-spu -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '

driver/driver_api/src/backup_interface.o: /cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/backup_interface.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -mcmodel=medium -Wall -mcpu=d1088-spu -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '

driver/driver_api/src/dac_interface.o: /cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/dac_interface.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -mcmodel=medium -Wall -mcpu=d1088-spu -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '

driver/driver_api/src/fft_api.o: /cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/fft_api.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -mcmodel=medium -Wall -mcpu=d1088-spu -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '

driver/driver_api/src/i2c_host.o: /cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/i2c_host.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -mcmodel=medium -Wall -mcpu=d1088-spu -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '

driver/driver_api/src/i2c_interface.o: /cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/i2c_interface.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -mcmodel=medium -Wall -mcpu=d1088-spu -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '

driver/driver_api/src/i2s_interface.o: /cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/i2s_interface.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -mcmodel=medium -Wall -mcpu=d1088-spu -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '

driver/driver_api/src/ppwm_interface.o: /cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/ppwm_interface.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -mcmodel=medium -Wall -mcpu=d1088-spu -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '

driver/driver_api/src/sadc_interface.o: /cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/sadc_interface.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -mcmodel=medium -Wall -mcpu=d1088-spu -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '

driver/driver_api/src/sd_card.o: /cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/sd_card.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -mcmodel=medium -Wall -mcpu=d1088-spu -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '

driver/driver_api/src/spdif_interface.o: /cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/spdif_interface.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -mcmodel=medium -Wall -mcpu=d1088-spu -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '

driver/driver_api/src/spim_interface.o: /cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/spim_interface.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -mcmodel=medium -Wall -mcpu=d1088-spu -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '

driver/driver_api/src/sw_uart.o: /cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/sw_uart.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -mcmodel=medium -Wall -mcpu=d1088-spu -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '

driver/driver_api/src/uarts_interface.o: /cygdrive/F/Soft_sdk/Mv_BP10X/MVsB1_Base_SDK/driver/driver_api/src/uarts_interface.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -Os -mcmodel=medium -Wall -mcpu=d1088-spu -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '


