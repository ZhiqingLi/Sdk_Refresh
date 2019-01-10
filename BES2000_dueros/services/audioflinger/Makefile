
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
	-Iservices/fs/fat \
	-Iservices/fs/sd \
	-Iservices/fs/fat/ChaN \
	-Iservices/bt_profiles/inc \
	-Iutils/uarthci \
	-Iutils/cqueue \
	-Iplatform/drivers/codec  \
	-Iplatform/drivers/ana

ifeq ($(AUDIO_CODEC_ASYNC_CLOSE),1)
CFLAGS_audioflinger.o += -D__CODEC_ASYNC_CLOSE__
endif

ifeq ($(CODEC_PLAY_BEFORE_CAPTURE),1)
CFLAGS_audioflinger.o += -DCODEC_PLAY_BEFORE_CAPTURE
endif

ifeq ($(AUDIO_OUTPUT_SMALL_GAIN_ATTN),1)
CFLAGS_audioflinger.o += -DAUDIO_OUTPUT_SMALL_GAIN_ATTN
endif

ifeq ($(AUDIO_OUTPUT_SW_GAIN),1)
CFLAGS_audioflinger.o += -DAUDIO_OUTPUT_SW_GAIN
endif

ifeq ($(AUDIO_OUTPUT_DC_CALIB),1)
CFLAGS_audioflinger.o += -DAUDIO_OUTPUT_DC_CALIB
endif
ifeq ($(AUDIO_OUTPUT_DC_CALIB_ANA),1)
CFLAGS_audioflinger.o += -DAUDIO_OUTPUT_DC_CALIB_ANA
endif

ifeq ($(AUDIO_OUTPUT_GAIN_M60DB_CHECK),1)
CFLAGS_audioflinger.o += -DAUDIO_OUTPUT_GAIN_M60DB_CHECK
endif

#-------------------------------
# Audioflinger device selection
#-------------------------------
# Internal codec default on
AF_DEVICE_INT_CODEC ?= 1

# BT PCM default on for BT applications
ifeq ($(RTOS),1)
AF_DEVICE_BT_PCM ?= 1
endif

# Individual device macro
ifeq ($(AF_DEVICE_EXT_CODEC),1)
CFLAGS_audioflinger.o += -DAF_DEVICE_EXT_CODEC
endif
ifeq ($(AF_DEVICE_I2S),1)
CFLAGS_audioflinger.o += -DAF_DEVICE_I2S
endif
ifeq ($(AF_DEVICE_INT_CODEC),1)
CFLAGS_audioflinger.o += -DAF_DEVICE_INT_CODEC
endif
ifeq ($(AF_DEVICE_SPDIF),1)
CFLAGS_audioflinger.o += -DAF_DEVICE_SPDIF
endif
ifeq ($(AF_DEVICE_BT_PCM),1)
CFLAGS_audioflinger.o += -DAF_DEVICE_BT_PCM
endif
ifeq ($(AF_DEVICE_DPD_RX),1)
CFLAGS_audioflinger.o += -DAF_DEVICE_DPD_RX
endif
ifeq ($(AUDIO_ANC_FB_MC),1)
CFLAGS_audioflinger.o += -DAF_DEVICE_MC
endif

#-------------------------------
# End
#-------------------------------

