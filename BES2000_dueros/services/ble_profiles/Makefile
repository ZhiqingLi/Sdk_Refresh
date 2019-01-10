
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))


obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)datapath/datapathps/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hrp/hrps/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hogp/hogpbh/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hogp/hogpd/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hogp/hogrh/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)bas/bass/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)prf/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)smartvoice/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)amazon/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)dueros/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ota/*.c))
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
	-Iservices/ble_stack/profiles/htp/htpt/api/  \
	-Iservices/ble_stack/profiles/htp/htpt/src/  \
	-Iservices/ble_stack/profiles/htp/	\
	-Iservices/ble_profiles/datapath/datapathps/api/  \
	-Iservices/ble_app/app_main/ \
	-Iservices/ble_app/app_htp/	\
	-Iservices/ble_app/app_datapath/ \
	-Iservices/ble_profiles/hrp/hrps/src/ \
	-Iservices/ble_profiles/hrp/hrps/api \
	-Iservices/ble_profiles/hrp/ \
	-Iservices/ble_app/app_hrps/	\
	-Iservices/ble_profiles/hogp/ \
	-Iservices/ble_profiles/hogp/hogpd/api/ \
	-Iservices/ble_profiles/bas/bass/src/ \
	-Iservices/ble_profiles/bas/bass/api/

