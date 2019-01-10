cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

subdir-ccflags-y += -Iplatform/hal \
							  -Iservices/ble_app/app_ota	\
					          -Iservices/bt_app \
					          -Iservices/bt_profiles/inc \
    								-Iservices/bt_profiles \
    								-Iservices/bt_app \
    								-Iservices/audioflinger \
    								-Iservices/nvrecord \
    								-Iservices/overlay \
    								-Iservices/resources \
									-Iapps/battery \
    								-Iutils/crc32 \
   	 								-Iplatform/drivers/bt \
    								-Iapps/audioplayers/rbplay \
    								-Iplatform/hal \
    								-Iplatform/hal/best2000 \
    								-Iservices/ble_stack/ble_ip \
   	 								-Iservices/ble_stack/hl/api \
    								-Iutils/jansson \
    								-Iutils/hwtimer_list \
    								-Iapps/key \
    								-Iservices/ble_app \
    								-Iservices/tws/inc    \
    								-Iplatform/drivers/ana \
	  								-Iservices/fs/sd \
									-Iservices/ble_stack/ke/api \
									-Iplatform/drivers/norflash

ifeq ($(RTOS),1)
ccflags-y += -DRTOS
endif
