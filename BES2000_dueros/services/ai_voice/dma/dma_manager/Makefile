cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_dma_info.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

src_obj := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_dma_handle.cpp))

LIB_NAME := libdma_manager
$(LIB_NAME)-y := $(src_obj:.cpp=.o)
obj-y += $(LIB_NAME).a

ccflags-y +=  -Iplatform/hal \
              -Iservices/ai_voice/dma/dma_stream \
			  -Iutils/cqueue \
			  -Iservices/ai_voice/dma/dma_protobuf \
			  -Iservices/ai_voice/algorithm \
			  -Iservices/multimedia/opus121/include \
			  -Iapps/audioplayers \
			  -Iservices/bt_profiles/inc/ \
	          -Iservices/bt_profiles/inc/sys/ \
			  -Iservices/bt_app \
	          -Iservices/resources \
			  -Iservices/ble_app/app_dueros \
			  -Iservices/ai_voice/audio_stream \
			  -Iutils/kfifo \
			  -Iservices/multimedia/speech/process/common/include \
			  -Itests/rom_cust \
			  -Iservices/ble_app/app_main \
			  -Iservices/nvrecord \
			  -Iservices/bt_app/app_voice/app_bt_dma
			  
	





