cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

obj-y += $(CHIP)/

subdir-ccflags-y += -Iservices/nvrecord -Iutils/cqueue

ifeq ($(BT_XTAL_SYNC),1)
subdir-ccflags-y += -DBT_XTAL_SYNC
endif

export BT_DRV_UART_BRIDGE_FLAGS :=
ifneq ($(DEBUG_PORT),)
BT_DRV_UART_BRIDGE_FLAGS += -DDEBUG_PORT=$(DEBUG_PORT)
endif

export BT_DRV_FLAGS :=
ifeq ($(NO_SLEEP),1)
BT_DRV_FLAGS += -DNO_SLEEP
endif

export BT_DRV_PATCH_FLAGS :=
ifeq ($(SPEECH_RX_PLC),1)
BT_DRV_PATCH_FLAGS += -DSPEECH_RX_PLC
endif

export BT_DRV_CONFIG_FLAGS :=
ifeq ($(AUDIO_SCO_BTPCM_CHANNEL),1)
BT_DRV_CONFIG_FLAGS += -D_SCO_BTPCM_CHANNEL_
endif

ifeq ($(INTERSYS_NO_THREAD),1)
BT_DRV_CONFIG_FLAGS += -D_INTERSYS_NO_THREAD_
endif

