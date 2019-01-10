cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)

ifeq ($(ANC_APP),1)
export CODEC_HIGH_QUALITY ?= 1
endif
ifeq ($(CODEC_HIGH_QUALITY),1)
VCODEC_VOLT ?= 1.95V
else
VCODEC_VOLT ?= 1.7V
endif
ifneq ($(filter-out 1.6V 1.7V 1.8V 1.9V 1.95V,$(VCODEC_VOLT)),)
$(error Invalid VCODEC_VOLT=$(VCODEC_VOLT))
endif
ifneq ($(VCODEC_VOLT),)
VCODEC_STEM := $(subst .,P,$(VCODEC_VOLT))
ANA_CFG_FLAGS += -DVCODEC_$(VCODEC_STEM)
PMU_CFG_FLAGS += -DVCODEC_$(VCODEC_STEM)
ifneq ($(filter 1.6V 1.7V 1.8V 1.9V 1.95V,$(VCODEC_VOLT)),)
VCODEC_OFF ?= 1
ifeq ($(VHPPA_VOLT),)
VHPPA_VOLT := $(VCODEC_VOLT)
endif
ifneq ($(VCODEC_VOLT),$(VHPPA_VOLT))
$(error Incompatible VCODEC_VOLT=$(VCODEC_VOLT) with VHPPA_VOLT=$(VHPPA_VOLT))
endif
endif
endif

ifeq ($(VCODEC_OFF),1)
PMU_CFG_FLAGS += -DVCODEC_OFF
endif

ifeq ($(VHPPA2VCODEC),1)
PMU_CFG_FLAGS += -DVHPPA2VCODEC
endif

VHPPA_VOLT ?= 1.8V
ifneq ($(filter-out 1.6V 1.7V 1.8V 1.9V 1.95V,$(VHPPA_VOLT)),)
$(error Invalid VHPPA_VOLT=$(VHPPA_VOLT))
endif
ifneq ($(VHPPA_VOLT),)
VHPPA_STEM := $(subst .,P,$(VHPPA_VOLT))
PMU_CFG_FLAGS += -DVHPPA_$(VHPPA_STEM)
endif

VANA_VOLT ?= 1.3V
ifneq ($(filter-out 1.2V 1.3V 1.35V 1.4V 1.5V 1.6V,$(VANA_VOLT)),)
$(error Invalid VANA_VOLT=$(VANA_VOLT))
endif
ifneq ($(VANA_VOLT),)
VANA_STEM := $(subst .,P,$(VANA_VOLT))
PMU_CFG_FLAGS += -DVANA_$(VANA_STEM)
endif

LOW_POWER_ADC ?= 1
ifeq ($(LOW_POWER_ADC),1)
ANA_CFG_FLAGS += -DLOW_POWER_ADC
endif

CFLAGS_pmu_best2300.o += $(PMU_CFG_FLAGS)

CFLAGS_analog_best2300.o += $(ANA_CFG_FLAGS)

CFLAGS_usbphy_best2300.o += $(USBPHY_CFG_FLAGS)

