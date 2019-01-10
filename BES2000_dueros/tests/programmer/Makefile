cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)

obj-y += ../../utils/hexdump/

obj-y += ../../platform/drivers/norflash/ ../../platform/drivers/ana/

ifeq ($(CHIP_HAS_USB),1)
obj-y += ../../platform/drivers/usb/usb_dev/
CFLAGS_sys_api_programmer.o += -Iplatform/drivers/usb/usb_dev/inc
endif

#ccflags-y += -I$(obj)/../../utils/hexdump -I$(obj)/../../utils/boot_struct -I$(obj)/../../tests/rom
ccflags-y += -Iutils/hexdump -Iutils/boot_struct -Itests/rom

CFLAGS_sys_api_programmer.o += -Iplatform/drivers/norflash -Iplatform/drivers/ana

ifneq ($(DEBUG_PORT),)
CFLAGS_sys_api_programmer.o += -DDEBUG_PORT=$(DEBUG_PORT)
endif

ifeq ($(PROGRAMMER_LOAD_SIMU),1)
CFLAGS_sys_api_programmer.o += -DPROGRAMMER_LOAD_SIMU
endif

ifeq ($(NO_UART_IOMUX),1)
CFLAGS_sys_api_programmer.o += -DNO_UART_IOMUX
endif

