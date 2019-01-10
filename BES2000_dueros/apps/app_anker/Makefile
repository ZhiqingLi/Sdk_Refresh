obj-y := device_drivers/ thirdparty/


ifeq ($(CONEXANT_CX20921_ENABLE),1)
subdir-ccflags-y += -Iapps/app_anker/conexant_cx20921
endif

ifeq ($(CODEC_AK7755_ENABLE),1)
subdir-ccflags-y += -Iapps/app_anker/ak7755
endif

subdir-ccflags-y += -Iapps/apptester \
					-Iapps/audioplayers \
					-Iapps/common \
					-Iapps/main \
					-Iapps/key \
					-Iapps/pwl \
					-Iapps/battery \
					-Iservices/multimedia/opus121/include \
					-Iapps/app_anker/device_drivers/bes_hal \
					-Iapps/app_anker/device_drivers/light_ctrl \
					-Iutils/list
		
