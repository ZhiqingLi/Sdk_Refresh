cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

DMA_PROTOBUF_LIB_NAME := libdma_protobuf

$(DMA_PROTOBUF_LIB_NAME)-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y += $(DMA_PROTOBUF_LIB_NAME).a

ccflags-y += \
		-Iservices/ai_voice/dma \
		-Iservices/ai_voice/dma/dma_stream

