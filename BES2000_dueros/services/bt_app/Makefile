
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))

obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y +=app_voice/

ccflags-y += \
	-Iservices/audio_eq \
	-Iservices/fs/fat \
	-Iservices/fs/sd \
	-Iservices/fs/fat/ChaN \
	-Iservices/bt_profiles/inc \
	-Iservices/bt_profiles/inc/sys \
	-Iservices/overlay \
	-Iservices/nvrecord \
	-Iservices/resources \
	-Iservices/multimedia/rbcodec \
	-Iservices/multimedia/audio/process/resample/include \
	-Iservices/multimedia/audio/process/filters/include \
	-Iservices/multimedia/audio/process/drc/include \
	-Iservices/multimedia/audio/process/anc/include\
	-Iservices/multimedia/speech/process/speech_drc/include \
	-Iservices/nv_section/aud_section \
    -Iservices/nv_section/include   \
	-Iplatform/drivers/uarthci \
	-Iplatform/drivers/ana \
	-Iplatform/drivers/bt \
	-Iutils/cqueue \
	-Iservices/audioflinger \
	-Iutils/lockcqueue \
	-Iutils/intersyshci \
	-Iapps/anc/inc \
	-Iapps/key \
	-Iapps/main \
	-Iapps/common \
	-Iapps/audioplayers \
	-Iapps/battery \
	-Iapps/common \
	-Iapps/factory \
	-Iservices/resources \
	-Iservices/ble_app \
	-Iutils/hwtimer_list \
	-Iservices/ble_stack/ble_ip \
	-Iservices/ble_stack/hl/api \
	-Iservices/ble_stack/app/api/  \
	-Iservices/ble_stack/common/api/ \
	-Iservices/ble_stack/hl/inc/ \
	-Iservices/ble_stack/ke/api \
	-Iservices/nvrecord \
	-Iservices/ai_voice/audio_stream \
	-Ithirdparty/userapi \
	-Iservices/bt_app/app_voice/app_bt_ama \
	-Iservices/ai_voice/dma/dma_manager \
	-Iapps/app_anker/device_drivers/bes_hal \
	-Iservices/ai_voice/ama/ama_manager \
	-Iservices/multimedia/speech/process/common/include \
	-Iservices/ble_app/app_main

# add begin by cai.zhong 20190103 for rfcomm
ifeq ($(DMA_VOICE),1)	
ccflags-y += -Iservices/bt_app/app_voice/app_bt_dma
endif
# add end by cai.zhong 20190103 for rfcomm
	

CFLAGS_app_bt_stream.o += -DAUDIO_OUTPUT_VOLUME_DEFAULT=$(AUDIO_OUTPUT_VOLUME_DEFAULT)

ifeq ($(BT_ONE_BRING_TWO),1)
ccflags-y += -D__BT_ONE_BRING_TWO__
endif

ifeq ($(VOICE_PROMPT),1)
ccflags-y += -DMEDIA_PLAYER_SUPPORT
endif

ifeq ($(AUDIO_QUEUE_SUPPORT),1)
CFLAGS_app_bt_media_manager.o += -D__AUDIO_QUEUE_SUPPORT__
endif

ifeq ($(BTADDR_GEN),1)
CFLAGS_besmain.o += -DRANDOM_BTADDR_GEN
endif

ifeq ($(SPEECH_TX_AEC),1)
CFLAGS_app_bt_stream.o += -DSPEECH_TX_AEC
CFLAGS_app_hfp.o += -DSPEECH_TX_AEC
endif

ifeq ($(SPEECH_TX_NS),1)
CFLAGS_app_bt_stream.o += -DSPEECH_TX_NS
CFLAGS_app_hfp.o += -DSPEECH_TX_NS
endif

ifeq ($(AUDIO_SCO_BTPCM_CHANNEL),1)
ccflags-y += -D_SCO_BTPCM_CHANNEL_
endif

ifeq ($(AUDIO_INPUT_MONO),1)
CFLAGS_app_bt_stream.o += -D__AUDIO_INPUT_MONO_MODE__
endif

ifeq ($(AUDIO_OUTPUT_MONO),1)
CFLAGS_app_bt_stream.o += -D__AUDIO_OUTPUT_MONO_MODE__
endif

ifeq ($(AUDIO_RESAMPLE),1)
CFLAGS_app_bt_stream.o += -D__AUDIO_RESAMPLE__
endif
ifeq ($(SW_PLAYBACK_RESAMPLE),1)
CFLAGS_app_bt_stream.o += -DSW_PLAYBACK_RESAMPLE
endif
ifeq ($(SW_CAPTURE_RESAMPLE),1)
CFLAGS_app_bt_stream.o += -DSW_CAPTURE_RESAMPLE
endif
ifeq ($(SW_SCO_RESAMPLE),1)
CFLAGS_app_bt_stream.o += -DSW_SCO_RESAMPLE
endif
ifeq ($(NO_SCO_RESAMPLE),1)
CFLAGS_app_bt_stream.o += -DNO_SCO_RESAMPLE
endif
ifeq ($(RESAMPLE_ANY_SAMPLE_RATE),1)
CFLAGS_app_bt_stream.o += -DRESAMPLE_ANY_SAMPLE_RATE
endif

ifeq ($(BT_XTAL_SYNC),1)
CFLAGS_app_bt_stream.o += -DBT_XTAL_SYNC
endif

ifeq ($(HW_FIR_EQ_PROCESS),1)
CFLAGS_app_bt_stream.o += -D__HW_FIR_EQ_PROCESS__
endif

ifeq ($(HW_IIR_EQ_PROCESS),1)
CFLAGS_app_bt_stream.o += -D__HW_IIR_EQ_PROCESS__
endif

ifeq ($(SW_IIR_EQ_PROCESS),1)
CFLAGS_app_bt_stream.o += -D__SW_IIR_EQ_PROCESS__
endif

ifeq ($(AUDIO_DRC),1)
CFLAGS_app_bt_stream.o += -D__AUDIO_DRC__
endif
ifeq ($(HW_DAC_IIR_EQ_PROCESS),1)
CFLAGS_app_bt_stream.o += -D__HW_DAC_IIR_EQ_PROCESS__
ifeq ($(ANC_APP),1)
$(error "error: ANC and Hardware codec iir can't be used in the same")
endif
endif

ifeq ($(A2DP_EQ_24BIT),1)
CFLAGS_app_bt_stream.o += -DA2DP_EQ_24BIT
endif

ifeq ($(APP_MUSIC_26M),1)
CFLAGS_app_bt_stream.o += -DAPP_MUSIC_26M
endif

