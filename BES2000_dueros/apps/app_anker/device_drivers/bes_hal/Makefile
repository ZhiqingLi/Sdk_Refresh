cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ifeq ($(CONEXANT_CX20921_ENABLE),1)
subdir-ccflags-y += -Iapps/app_anker/device_drivers/voice_input/conexant_cx20921
endif

ifeq ($(CODEC_AK7755_ENABLE),1)
subdir-ccflags-y += -Iapps/app_anker/device_drivers/ak7755
endif

ccflags-y +=  \
	-Iplatform/hal/ \
	-Iplatform/hal/best2000 \
	-Iplatform/drivers/norflash \
	-Iplatform/drivers/ana \
	-Iservices/bt_profiles/inc \
	-Iservices/bt_profiles/inc/sys/ \
	-Iservices/bt_app \
	-Iservices/resources \
	-Iapps/common \
	-Iapps/key \
	-Iapps/audioplayers \
	-Iapps/app_anker/device_drivers/light_ctrl \
	-Iapps/app_anker/thirdparty/md5 \
	-Iutils/hwtimer_list \
	-Iutils/cqueue \