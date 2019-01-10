obj-y := audioplayers/ common/ main/ key/ pwl/ battery/ factory/

ifeq ($(APP_TEST_AUDIO),1)
obj-y += apptester/
endif

ifeq ($(BTUSB_AUDIO_MODE),1)
obj-y += usbaudio/
endif

ifeq ($(APP_TEST_SDMMC),1)
obj-y += sdmmc/
endif

ifeq ($(BT_ANC),1)
obj-y += anc/
endif

ifeq ($(OTA_ENABLED),1)
obj-y += ota/
endif

ifeq ($(ANKER_UI),1)
obj-y += app_anker/
endif

subdir-ccflags-y += -Iapps/apptester \
					-Iapps/audioplayers \
					-Iapps/common \
					-Iapps/sdmmc \
					-Iapps/main \
					-Iapps/key \
					-Iapps/pwl \
					-Iapps/battery \
					-Iservices/ai_voice/audio_stream \
					-Iservices/ai_voice/smartvoice \
					-Iutils/list
