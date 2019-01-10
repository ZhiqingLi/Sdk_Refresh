cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))

obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)


subdir-ccflags-y +=	-Iutils/cqueue \
					-Iutils/retention_ram \
					-Iservices/audioflinger \
					-Iutils/lockcqueue \
					-Iservices/bt_profiles/inc \
					-Iapps/audioplayers \
					-Iservices/multimedia/speech/application/Pcm8kCvsd/include \
					-Iservices/multimedia/speech/codec/codecs/adpcm \
					-Iservices/multimedia/opus121/include \
					-Iservices/multimedia/speech/process/lc_mmse_ns/include \
					-Iservices/multimedia/speech/process/ns_and_ec/include \
					-Iservices/multimedia/speech/process/agc/include \
					-Iservices/multimedia/speech/process/coherent_denoise/include \
					-Iservices/multimedia/speech/process/dual_mic_denoise/include \
					-Iservices/multimedia/speech/process/speech_drc/include \
                    -Iservices/multimedia/speech/process/far_field_speech_enhancement/include \
					-Iservices/multimedia/speech/process/plc/include \
					-Iservices/multimedia/speech/application/Pcm8kCvsd/include \
					-Iservices/multimedia/speech/process/common/include \
					-Iservices/bt_profiles/inc \
					-Iservices/bt_app \
					-Iservices/ai_voice/ama/ama_stream





