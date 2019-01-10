cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))


obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_main/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_htp/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_datapath/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_hrps/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_hid/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_sec/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_batt/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_smartvoice/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_amazon/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_dueros/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_ota/*.c))
obj-y := $(obj_c:.c=.o)

subdir-ccflags-y += \
	-Iservices/audio_eq \
	-Iservices/fs/fat \
	-Iservices/fs/sd \
	-Iservices/fs/fat/ChaN \
	-Iservices/bt_profiles/inc \
	-Iservices/bt_profiles/inc/sys \
	-Iservices/overlay \
	-Iservices/nvrecord \
	-Iservices/resources \
	-Iplatform/drivers/uarthci \
	-Iplatform/drivers/ana \
	-Iplatform/drivers/bt \
	-Iutils/cqueue \
	-Iutils/retention_ram \
	-Iservices/audioflinger \
	-Iutils/lockcqueue \
	-Iutils/intersyshci \
	-Iapps/key \
	-Iapps/main \
	-Iapps/common \
	-Iapps/audioplayers \
	-Iapps/factory \
	-Iservices/resources \
	-Iservices/ble_app \
	-Iservices/ble_stack/ble_ip \
	-Iservices/ble_stack/hl/api \
	-Iservices/ble_stack/app/api/  \
	-Iservices/ble_stack/common/api/ \
	-Iservices/ble_stack/hl/inc/ \
	-Iservices/ble_stack/ke/api \
	-Iservices/bridge/ \
	-Iservices/bt_profiles/inc/ \
	-Iservices/bt_profiles/inc/sys/ \
	-Iservices/ble_stack/common/api/ \
	-Iservices/ble_stack/hl/inc/ \
	-Iservices/ble_stack/hl/api/ \
	-Iservices/ble_stack/hl/src/gap/ \
	-Iservices/ble_stack/hl/src/gap/gapc/ \
	-Iservices/ble_stack/hl/src/gap/gapm/ \
	-Iservices/ble_stack/hl/src/gap/smpc/ \
	-Iservices/ble_stack/ke/api/ \
	-Iservices/ble_stack/ke/src/ \
	-Iservices/ble_stack/hl/src/gatt/attc/ \
	-Iservices/ble_stack/hl/src/gatt/attm/ \
	-Iservices/ble_stack/hl/src/gatt/atts/ \
	-Iservices/ble_stack/hl/src/gatt/gattc/ \
	-Iservices/ble_stack/hl/src/gatt/gattm/ \
	-Iservices/ble_stack/hl/src/gatt/ \
	-Iservices/ble_stack/hl/src/l2c/l2cc/ \
	-Iservices/ble_stack/hl/src/l2c/l2cm/ \
	-Iservices/ble_stack/hci/api/ \
	-Iservices/ble_stack/hci/src/ \
	-Iservices/ble_stack/ble_ip/  \
	-Iservices/ble_stack/app/api/  \
	-Iservices/ble_stack/app/src/  \
	-Iservices/ble_profiles/htp/htpt/api/  \
	-Iservices/ble_profiles/htp/htpt/src/  \
	-Iservices/ble_profiles/htp/	\
	-Iservices/ble_profiles/datapath/datapathps/api/  \
	-Iservices/ble_app/app_main/ \
	-Iservices/ble_app/app_htp/	\
	-Iservices/ble_app/app_datapath/	\
	-Iservices/ble_app/app_hrps/	\
	-Iservices/ble_profiles/hrp/hrps/api/ \
	-Iservices/ble_profiles/hrp/hrps/src \
	-Iservices/ble_profiles/hrp/ \
	-Iservices/ble_app/app_sec/ \
	-Iservices/ble_app/app_batt/ \
	-Iservices/ble_app/app_hid/ \
	-Iservices/ble_profiles/bas/bass/api/ \
	-Iservices/ble_profiles/hogp/hogpd/api/ \
	-Iservices/ble_profiles/hogp/ \
	-Iservices/multimedia/speech/codec/codecs/adpcm \
	-Iservices/ai_voice/audio_stream \
	-Iservices/ai_voice/smartvoice \
	-Iservices/overlay \
	-Iservices/ble_profiles/smartvoice \
	-Iservices/ble_profiles/amazon \
	-Iservices/ble_profiles/dueros \
	-Iservices/ble_app/app_smartvoice \
	-Iservices/ble_app/app_amazon \
	-Iservices/ble_app/app_sec \
	-Iservices/ai_voice/ama/ama_manager \
	-Iservices/ai_voice/dma/dma_manager \
	-Iservices/ble_app/app_dueros \
	-Iservices/ble_profiles/ota \
	-Iservices/ble_app/app_ota \
	-Iapps/ota \
	-Iservices/bt_app
	
	

ifeq ($(AUDIO_RESAMPLE),1)
CFLAGS_voice_over_ble.o += -D__AUDIO_RESAMPLE__
endif
ifeq ($(SW_PLAYBACK_RESAMPLE),1)
CFLAGS_voice_over_ble.o += -DSW_PLAYBACK_RESAMPLE
endif
ifeq ($(SW_CAPTURE_RESAMPLE),1)
CFLAGS_voice_over_ble.o += -DSW_CAPTURE_RESAMPLE
endif

