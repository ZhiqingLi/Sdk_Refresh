cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

DMA_LIB_NAME := libdma

$(DMA_LIB_NAME)-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y += $(DMA_LIB_NAME).a

ccflags-y +=  -Iplatform/hal \
              -Iservices/nvrecord \
			  -Iservices/bt_app \
			  -Iservices/bt_profiles/inc \
	          -Iservices/ai_voice/dma/dma_protobuf \
			  -Iservices/ai_voice/linkplay/  \
			  -Iservices/ble_app/app_dueros \
			  -Iutils/cqueue
			  
	





