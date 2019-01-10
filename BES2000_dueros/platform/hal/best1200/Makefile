cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj-y := $(obj-y:.c=.o)

obj-y += ../best1000/hal_cmu_best1000.o

CFLAGS_hal_iomux_best1000.o += $(IOMUX_CFG_FLAGS)

