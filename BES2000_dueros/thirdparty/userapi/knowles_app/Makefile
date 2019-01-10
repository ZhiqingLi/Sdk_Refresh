cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)



ccflags-y += \
	-Iapps/common \
	-Iplatform/drivers/ana \
	-Iutils/crc32 \
	-Iapps/key \
	-Ithirdparty/knowles_uart_lib \
	-Ithirdparty/userapi \
	-Iservices/ai_voice/ama/ama_stream \
	-Iservices/ai_voice/dma/dma_stream
	



