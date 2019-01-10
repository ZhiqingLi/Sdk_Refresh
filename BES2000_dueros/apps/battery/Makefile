cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

subdir-ccflags-y += -Iplatform/drivers/ana \
					-Iapps/app_anker/device_drivers/bes_hal \
					-Iutils/hwtimer_list 

ifeq ($(VOICE_PROMPT),1)
CFLAGS_app_battery.o += -DMEDIA_PLAYER_SUPPORT
endif

