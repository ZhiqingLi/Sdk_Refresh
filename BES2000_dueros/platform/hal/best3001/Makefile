cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj-y := $(obj-y:.c=.o)

ifeq ($(USB_HIGH_SPEED),1)
CMU_CFG_FLAGS += -DUSB_HIGH_SPEED
endif

ifeq ($(ANA_26M_X4_ENABLE),1)
CMU_CFG_FLAGS += -DANA_26M_X4_ENABLE
endif

ifeq ($(OSC_26M_X4_AUD2BB),1)
CMU_CFG_FLAGS += -DOSC_26M_X4_AUD2BB
endif

USB_CLK_SRC_24M_X2 ?= 1
ifeq ($(USB_CLK_SRC_24M_X2),1)
CMU_CFG_FLAGS += -DUSB_CLK_SRC_24M_X2
endif

ifeq ($(DAC_CLASSG_ENABLE),1)
CODEC_CFG_FLAGS += -DDAC_CLASSG_ENABLE
endif

ifeq ($(DAC_DRE_ENABLE),1)
CODEC_CFG_FLAGS += -DDAC_DRE_ENABLE
endif

ifeq ($(JTAG_TDI_TDO_PIN),1)
IOMUX_CFG_FLAGS += -DJTAG_TDI_TDO_PIN
endif

ifeq ($(JTAG_RESET_PIN),1)
IOMUX_CFG_FLAGS += -DJTAG_RESET_PIN
endif

CFLAGS_hal_cmu_best3001.o += $(CMU_CFG_FLAGS)

CFLAGS_hal_codec_best3001.o += $(CODEC_CFG_FLAGS)

CFLAGS_hal_iomux_best3001.o += $(IOMUX_CFG_FLAGS)

