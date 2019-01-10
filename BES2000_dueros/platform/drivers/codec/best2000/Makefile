cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)

CFLAGS_codec_best2000.o += $(CODEC_INT_CFG_FLAGS)

ccflags-y += -Iapps/app_anker/device_drivers/bes_hal