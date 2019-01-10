cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

src_obj := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c $(cur_dir)src/*.S))
src_obj := $(src_obj:.c=.o)
src_obj := $(src_obj:.S=.o)

cfg_obj := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)cfg/*.c $(cur_dir)cfg/*.S))
cfg_obj := $(cfg_obj:.c=.o)
cfg_obj := $(cfg_obj:.S=.o)

USB_DEV_LIB_NAME := libusbdev

ccflags-y += -Iplatform/drivers/usb/usb_dev/inc

ifeq ($(USB_HIGH_SPEED),1)
ccflags-y += -DUSB_HIGH_SPEED
endif

CFLAGS_usb_cdc.o += -Itests/programmer

CFLAGS_usb_audio.o += -Iutils/hwtimer_list

CFLAGS_usb_audio_sync.o += -Iplatform/drivers/ana

USB_AUDIO_NO_START_ERR ?= 1
ifeq ($(USB_AUDIO_NO_START_ERR),1)
CFLAGS_usb_audio.o += -DUSB_AUDIO_NO_START_ERR
endif

TARGET_TO_MAX_DIFF ?= 0
ifeq ($(TARGET_TO_MAX_DIFF),1)
CFLAGS_usb_audio_sync.o += -DTARGET_TO_MAX_DIFF
endif

ifneq ($(USB_CDC_VENDOR_ID),)
CFLAGS_usb_dev_desc.o += -DUSB_CDC_VENDOR_ID=$(USB_CDC_VENDOR_ID)
endif

ifneq ($(USB_CDC_PRODUCT_ID),)
CFLAGS_usb_dev_desc.o += -DUSB_CDC_PRODUCT_ID=$(USB_CDC_PRODUCT_ID)
endif

ifneq ($(USB_AUDIO_VENDOR_ID),)
CFLAGS_usb_dev_desc.o += -DUSB_AUDIO_VENDOR_ID=$(USB_AUDIO_VENDOR_ID)
endif

ifneq ($(USB_AUDIO_PRODUCT_ID),)
CFLAGS_usb_dev_desc.o += -DUSB_AUDIO_PRODUCT_ID=$(USB_AUDIO_PRODUCT_ID)
endif

ifeq ($(USB_LPM),1)
CFLAGS_usb_dev_desc.o += -DUSB_LPM
ifeq ($(USB_LPM_DEEP_BESL),1)
CFLAGS_usb_dev_desc.o += -DUSB_LPM_DEEP_BESL
endif
endif

UAUD_CFG_FLAGS :=

ifeq ($(USB_AUDIO_DYN_CFG),1)

ifeq ($(USB_AUDIO_DYN_CFG_16BIT),1)
USB_DEV_LIB_NAME := $(USB_DEV_LIB_NAME)_dyn16
UAUD_CFG_FLAGS += -DUSB_AUDIO_DYN_CFG_16BIT
ifeq ($(USB_AUDIO_192K),1)
UAUD_CFG_FLAGS += -DUSB_AUDIO_192K
endif
else
ifeq ($(USB_AUDIO_DYN_CFG_24BIT),1)
ifeq ($(USB_AUDIO_32BIT),1)
USB_DEV_LIB_NAME := $(USB_DEV_LIB_NAME)_dyn32
UAUD_CFG_FLAGS += -DUSB_AUDIO_DYN_CFG_24BIT -DUSB_AUDIO_32BIT
else
USB_DEV_LIB_NAME := $(USB_DEV_LIB_NAME)_dyn24
UAUD_CFG_FLAGS += -DUSB_AUDIO_DYN_CFG_24BIT
endif
else
ifeq ($(USB_AUDIO_32BIT),1)
USB_DEV_LIB_NAME := $(USB_DEV_LIB_NAME)_dyn16-32
UAUD_CFG_FLAGS += -DUSB_AUDIO_32BIT
else
USB_DEV_LIB_NAME := $(USB_DEV_LIB_NAME)_dyn
endif
ifeq ($(USB_AUDIO_192K),1)
UAUD_CFG_FLAGS += -DUSB_AUDIO_192K
endif
endif # USB_AUDIO_DYN_CFG_24BIT != 1
endif # USB_AUDIO_DYN_CFG_16BIT != 1
UAUD_CFG_FLAGS += -DUSB_AUDIO_DYN_CFG

else # USB_AUDIO_DYN_CFG != 1

ifeq ($(USB_AUDIO_192K),1)
USB_DEV_LIB_NAME := $(USB_DEV_LIB_NAME)_192k
UAUD_CFG_FLAGS += -DUSB_AUDIO_192K
else
ifeq ($(USB_AUDIO_96K),1)
USB_DEV_LIB_NAME := $(USB_DEV_LIB_NAME)_96k
UAUD_CFG_FLAGS += -DUSB_AUDIO_96K
else
ifeq ($(USB_AUDIO_44_1K),1)
USB_DEV_LIB_NAME := $(USB_DEV_LIB_NAME)_44p1k
UAUD_CFG_FLAGS += -DUSB_AUDIO_44_1K
endif
ifeq ($(USB_AUDIO_16K),1)
USB_DEV_LIB_NAME := $(USB_DEV_LIB_NAME)_16k
UAUD_CFG_FLAGS += -DUSB_AUDIO_16K
endif
endif
endif

ifeq ($(USB_AUDIO_24BIT),1)
USB_DEV_LIB_NAME := $(USB_DEV_LIB_NAME)_24b
UAUD_CFG_FLAGS += -DUSB_AUDIO_24BIT
else
ifeq ($(USB_AUDIO_32BIT),1)
USB_DEV_LIB_NAME := $(USB_DEV_LIB_NAME)_32b
UAUD_CFG_FLAGS += -DUSB_AUDIO_32BIT
endif
endif

endif # USB_AUDIO_DYN_CFG != 1

ifeq ($(USB_AUDIO_MIC_MONO),1)
USB_DEV_LIB_NAME := $(USB_DEV_LIB_NAME)_mono
UAUD_CFG_FLAGS += -DUSB_AUDIO_MIC_MONO
endif

CFLAGS_usb_audio.o += $(UAUD_CFG_FLAGS)
CFLAGS_usb_dev_desc.o += $(UAUD_CFG_FLAGS)

ifeq ($(SIMU_UAUD_MAX_PKT),1)
CFLAGS_usb_audio.o += -DSIMU_UAUD_MAX_PKT
endif

$(USB_DEV_LIB_NAME)-y := $(src_obj)

obj-y := $(USB_DEV_LIB_NAME).a $(cfg_obj)

