cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ccflags-y += \
	-Iapps/common \
	-Iplatform/drivers/usb/usb_dev/inc \
	-Iplatform/drivers/ana \
	-Iutils/crc32 \
	-Iservices/bt_app \
	-Iservices/multimedia/audio_dump/include \
	-Iutils/kfifo

