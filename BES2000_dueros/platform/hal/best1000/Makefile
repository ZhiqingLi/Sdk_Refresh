cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj-y := $(obj-y:.c=.o)

CFLAGS_hal_cmu_best1000.o += $(CMU_CFG_FLAGS)

CFLAGS_hal_codec_best1000.o += $(CODEC_CFG_FLAGS)

CFLAGS_hal_iomux_best1000.o += $(IOMUX_CFG_FLAGS)

