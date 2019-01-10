cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

#obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y :=

ifeq ($(ROM_BUILD),1)
obj-y += ../../tests/rom/startup_ARMCM4.S
else
obj-y += startup_main.S
endif

ifeq ($(FACTORY_SUITE),1)
obj-y += ../../tests/factory_suite/
else
ifeq ($(RELIABILITY_TEST),1)
obj-y += ../../tests/reliability_test/
else
ifeq ($(NOSTD),1)
asflags-y += -DNOSTD
obj-y += nostd_main.c
else
ifeq ($(NOAPP),1)
obj-y += noapp_main.cpp
else
obj-y += main.cpp
endif
endif
endif
endif

ifneq ($(filter 1,$(USB_SERIAL_TEST) $(USB_AUDIO_TEST) $(AF_TEST) $(MEM_TEST)),)
ccflags-y += -Itests/hwtest

ifneq ($(filter 1,$(USB_SERIAL_TEST) $(USB_AUDIO_TEST)),)
obj-y += ../../tests/hwtest/usb_dev_test.c
ifeq ($(USB_SERIAL_TEST),1)
ccflags-y += -DUSB_SERIAL_TEST
else
ccflags-y += -DUSB_AUDIO_TEST
endif
endif

ifeq ($(AF_TEST),1)
obj-y += ../../services/audioflinger/  ../../tests/hwtest/af_test.c
ccflags-y += -Iservices/audioflinger -DAF_TEST

ifeq ($(AF_INT_CODEC_TEST),1)
CFLAGS_af_test.o += -DAF_INT_CODEC_TEST
obj-y += ../../platform/drivers/codec/
export AF_DEVICE_INT_CODEC ?= 1
export AF_DEVICE_I2S ?= 0
else
export AF_DEVICE_INT_CODEC ?= 0
export AF_DEVICE_I2S ?= 1
endif

ifneq ($(CHAN_NUM_CAPTURE),)
CFLAGS_af_test.o += -DCHAN_NUM_CAPTURE=$(CHAN_NUM_CAPTURE)
endif
ifneq ($(CHAN_SEP_BUF_CAPTURE),)
CFLAGS_af_test.o += -DCHAN_SEP_BUF_CAPTURE=$(CHAN_SEP_BUF_CAPTURE)
endif
ifeq ($(FIXED_BUF_CAPTURE),1)
CFLAGS_af_test.o += -DFIXED_BUF_CAPTURE
endif
endif # AF_TEST

ifeq ($(MEM_TEST),1)
obj-y += ../../tests/hwtest/mem_test.c
ccflags-y += -DMEM_TEST
endif

endif # hwtest cases

obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

obj-y += ../../utils/hwtimer_list/

ccflags-y +=  \
	-Iutils/hwtimer_list \
	-Iplatform/drivers/ana \
	-Iplatform/drivers/usb/usb_dev/inc \
	-Iapps/main \
	-Iapps/factory \
	-Iutils/list  \
	-Iapps/audioplayers  \
	-Iservices/bt_app  \
	-Iapps/common  \
	-Iservices/bt_profiles/inc \
	-Iplatform/drivers/norflash \
	-Iapps/app_anker/device_drivers/bes_hal

ifneq ($(FLASH_FILL),)
ccflags-y += -DFLASH_FILL=$(FLASH_FILL)
endif

ifneq ($(DEBUG_PORT),)
ccflags-y += -DDEBUG_PORT=$(DEBUG_PORT)
endif

ifneq ($(FLASH_REGION_BASE),)
CFLAGS_main.o += -DFLASH_REGION_BASE=$(FLASH_REGION_BASE)
endif

ifneq ($(FLASH_SIZE),)
CFLAGS_main.o += -DFLASH_SIZE=$(FLASH_SIZE)
endif

