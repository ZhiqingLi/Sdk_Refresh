obj-y := audioflinger/ bt_profiles/ fs/ bt_app/ overlay/ resources/ ../utils/crc32/ ../utils/kfifo/

ifeq ($(BLE),1)
obj-y += ble_app/ bridge/ ble_stack/ ../utils/retention_ram/ ble_profiles/
endif
ifeq ($(A2DP_SCALABLE_ON),1)
obj-y += scalable/
endif

obj-y += nv_section/

ifneq ($(FPGA),1)
obj-y += nvrecord/
endif

ifneq ($(filter 1, $(HW_FIR_EQ_PROCESS) $(HW_IIR_EQ_PROCESS) $(SW_IIR_EQ_PROCESS) $(HW_DAC_IIR_EQ_PROCESS) $(AUDIO_DRC)),)
obj-y += audio_eq/
endif

ifeq ($(VOICE_RECOGNITION),1)
obj-y += voice_recognition/
endif

#ifeq ($(SMARTVOICE),1)
obj-y += ai_voice/
#endif

obj-y += communication/