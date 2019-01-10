cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)

CFLAGS_codec_best1000.o += $(CODEC_INT_CFG_FLAGS)

ifeq ($(CODEC_PLAY_BEFORE_CAPTURE),1)
CFLAGS_codec_best1000.o += -DCODEC_PLAY_BEFORE_CAPTURE
endif

