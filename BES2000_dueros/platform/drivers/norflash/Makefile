cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)

ccflags-y := -I$(obj)/inc -Itests/programmer

CFLAGS_norflash_drv.o += -DCHIP_FLASH_CTRL_VER=$(CHIP_FLASH_CTRL_VER)

ifeq ($(FLASH_CHIP),)
ifeq ($(PROGRAMMER),1)
FLASH_CHIP := ALL
else
FLASH_CHIP := GD25Q32C
endif
endif
ifeq ($(FLASH_CHIP),)
$(error *** Error: FLASH_CHIP is NOT defined!)
endif

CFLAGS_norflash_drv.o += $(foreach flash,$(FLASH_CHIP),-D__NORFLASH_$(flash)__)

