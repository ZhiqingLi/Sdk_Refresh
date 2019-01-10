cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S $(cur_dir)*.cpp))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)
obj-y := $(obj-y:.cpp=.o)

ifeq ($(DSP_LIB),1)
obj-y += DSP_Lib/
endif

ccflags-y := -I$(obj)/inc
ccflags-y := -I$(obj)/inc \
	-Iplatform/drivers/ana


USAGE_FAULT ?= 1
ifeq ($(USAGE_FAULT),1)
CFLAGS_system_ARMCM4.o += -DUSAGE_FAULT
endif
BUS_FAULT ?= 1
ifeq ($(BUS_FAULT),1)
CFLAGS_system_ARMCM4.o += -DBUS_FAULT
endif
MEM_FAULT ?= 1
ifeq ($(MEM_FAULT),1)
CFLAGS_system_ARMCM4.o += -DMEM_FAULT
endif

