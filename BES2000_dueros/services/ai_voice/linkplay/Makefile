cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

DEV_ENC_LIB_NAME := libdev_encryption

$(DEV_ENC_LIB_NAME)-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y += $(DEV_ENC_LIB_NAME).a

ccflags-y +=  -Iplatform/hal \
              -Iplatform/drivers/norflash \
              -Iapps/main \
              -Iservices/nvrecord \
			  -Iservices/bt_app \
			  -Iservices/bt_profiles/inc \
			  -Iservices/ai_voice/linkplay \
			  -Iservices/ble_app/app_dueros \
			  -Iutils/crc32 \
			  -Iutils/cqueue
			  
	





