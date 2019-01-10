cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)

ifeq ($(VCODEC_OFF),1)
PMU_CFG_FLAGS += -DVCODEC_OFF
endif

CFLAGS_pmu_best1000.o += $(PMU_CFG_FLAGS)

CFLAGS_analog_best1000.o += $(ANA_CFG_FLAGS)

ifeq ($(CODEC_HIGH_THD_WORKAROUND),1)
CFLAGS_analog_best1000.o += -DCODEC_HIGH_THD_WORKAROUND
endif

ifeq ($(ANA_OLD_DC_CALIB),1)
CFLAGS_analog_best1000.o += -DANA_OLD_DC_CALIB
endif

