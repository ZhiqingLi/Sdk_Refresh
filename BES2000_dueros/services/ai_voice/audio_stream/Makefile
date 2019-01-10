cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))

obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

subdir-ccflags-y += -Iplatform/drivers/ana \
					-Iplatform/hal \
					-Iutils/cqueue \
					-Iservices/bt_profiles \
					-Iservices/bt_profiles/inc \
					-Iservices/bt_profiles/besport \
					-Iapps/key \
					-Iservices/audio_eq \
					-Iservices/overlay \
					-Iservices/nvrecord \
					-Iservices/resources \
					-Iapps/main \
					-Iapps/common \
					-Iapps/audioplayers \
					-Iapps/factory \
					-Iservices/bt_app \
					-Ithirdparty/userapi \
					-Iservices/multimedia/opus121/include \
					-Iservices/ai_voice/algorithm \
					-Iservices/ai_voice/smartvoice \
					-Iservices/ai_voice/ama/ama_manager \
					-Iservices/ai_voice/dma/dma_manager \
					-Iservices/multimedia/audio_dump/include \
					-Iservices/communication \
					-Iservices/ai_voice/ama/ama_stream \
					-Ithirdparty/knowles_uart_lib \
					-Iservices/multimedia/speech/process/lc_mmse_ns/include \
					-Iservices/multimedia/speech/process/ns_and_ec/include \
					-Iservices/multimedia/speech/process/common/include

ifeq ($(AUDIO_RESAMPLE),1)
ANC_USB_CFG_FLAGS += -D__AUDIO_RESAMPLE__
endif
ifeq ($(SW_CAPTURE_RESAMPLE),1)
ANC_USB_CFG_FLAGS += -DSW_CAPTURE_RESAMPLE
endif

