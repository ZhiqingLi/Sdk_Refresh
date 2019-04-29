ROOT := .
PROJECT ?= Demo
OUT_DIR ?= ../../out
CROSS-COMPLILE := ba-elf-

TARGET := $(OUT_DIR)/$(PROJECT)/$(PROJECT).elf

CC := $(CROSS-COMPLILE)gcc
AR := $(CROSS-COMPLILE)ar

SRC_DIRS := $(ROOT)/host/port/usb/src/test/msd  \
			$(ROOT)/host/port/usb/src/systems/none/afs  \
			$(ROOT)/host/port/usb/src/systems/none  \
			$(ROOT)/host/port/usb/src/lib  \
			$(ROOT)/host/port/usb/src/functions/trans_fn  \
			$(ROOT)/host/port/usb/src/examples/msd  \
			$(ROOT)/host/port/usb/src/drivers/msd  \
			$(ROOT)/host/port/usb/src/cd   \
			$(ROOT)/host/port/common/bluetooth  \
			$(ROOT)/host/port/common/audio_out_interface  \
			$(ROOT)/host/port/common   \
			$(ROOT)/host/port/beken_no_os/pts  \
			$(ROOT)/host/port/beken_no_os  \
			$(ROOT)/host/port/beken_driver  \
			$(ROOT)/host/port/beken_app   \
			$(ROOT)/host/libs/Mp3Lib  \
			$(ROOT)/host/libs/FatLib  \
			$(ROOT)/controller/hal/beken/sys  \
			$(ROOT)/controller/core/le/include  \
			$(ROOT)/controller/core/le/ai_voice/common/utils/print  \
			$(ROOT)/controller/core/le/ai_voice/common/utils/libc  \
			$(ROOT)/controller/core/le/ai_voice/common/utils/cqueue  \
			$(ROOT)/controller/core/le/ai_voice/common/linkplay  \
			$(ROOT)/controller/core/le

SRC_FILES  = $(foreach dir,	$(SRC_DIRS), $(wildcard $(dir)/*.[csS]))
OBJ_FILES = $(patsubst %.c, $(OUT_DIR)/$(PROJECT)/%.o, $(filter %.c,$(SRC_FILES)))
OBJ_FILES += $(patsubst %.S, $(OUT_DIR)/$(PROJECT)/%.o, $(filter %.S,$(SRC_FILES)))
DEP_FILES = $(patsubst %.o,%.d,$(OBJ_FILES))

C_INCLUDE = -I$(ROOT)/host -I$(ROOT)/host/include -I$(ROOT)/host/port/include -I$(ROOT)/host/port/beken_driver -I$(ROOT)/host/port/beken_no_os \
			-I$(ROOT)/host/config -I$(ROOT)/controller/core/sys/include -I$(ROOT)/controller/hal/beken/sys/include -I$(ROOT)/controller/hal/hw/radio/include \
			-I$(ROOT)/controller/core/hc/include -I$(ROOT)/controller/hal/beken/hw/include -I$(ROOT)/host/port/beken_app \
			-I$(ROOT)/host/bluetooth/include -I$(ROOT)/host/port/include/bluetooth -I$(ROOT)/host/port/common -I$(ROOT)/host/pkg/sbc \
			-I$(ROOT)/host/libs/Mp3Lib -I$(ROOT)/host/libs/FatLib -I$(ROOT)/host/port/common/bluetooth -I$(ROOT)/controller/core/le/include \
			-I$(ROOT)/controller/core/lc/uslc/include -I$(ROOT)/controller/core/transport/include -I$(ROOT)/host/libs/opus \
			-I$(ROOT)/controller/core/lmp/include -I$(ROOT)/controller/core/lc/include -I$(ROOT)/controller/core/bt/include \
			-I$(ROOT)/controller/core/hw/include -I$(ROOT)/controller/core/lc/dl/include -I$(ROOT)/controller/hal/hw/include \
			-I$(ROOT)/controller/core/lc/lslc/include -I$(ROOT)/controller/core/tc/include -I$(ROOT)/controller/core/lmp/include \
			-I$(ROOT)/controller/core/le/ai_voice/common/utils/print -I$(ROOT)/host/port/include/os -I$(ROOT)/host/jos \
			-I$(ROOT)/host/bluetooth/protocol/avctp -I$(ROOT)/host/port/usb/src/systems/none -I$(ROOT)/host/port/usb/src/systems/none/afs \
			-I$(ROOT)/host/port/usb/include -I$(ROOT)/host/port/usb/include/class -I$(ROOT)/host/port/usb/src/drivers/msd \
			-I$(ROOT)/controller/core/le/ai_voice/mdd -I$(ROOT)/controller/core/le/ai_voice/pdd

C_FLAGS = -DBA22_DEee -D_BEKEN_PLATFROM 
C_OPTIONS = -O1 -flto -G 100000 -g3 -Wall -c -fmessage-length=0  -mno-hard-float -ffast-math -march=ba2 -mle -mabi=3 -mtj-rodata -msingle-float -mdsp -mmac -MMD -MP
LD_OPTIONS = -nostartfiles -flto -Os -G 100000 -march=ba2 -mle -T "$(ROOT)/ram.ld"
LD_INCLUDE = -L$(ROOT)/host/libs -L$(ROOT)/host/libs/opus -L$(ROOT)/controller/core/le/ai_voice/mdd/libs -L$(ROOT)/controller/core/le/ai_voice/pdd/libs 
LD_LIBRARY = -lm -lBK3266_DM_OTA_Full_Func_lib -lpdd -lmdd -lopus
.PHONY : all

all : pre-build $(TARGET) post-build
	@echo "build finish"

$(TARGET) : $(OBJ_FILES)
	@mkdir -p $(dir $@)
	$(CC) $(LD_INCLUDE) $(LD_OPTIONS) -o $@ $^ $(LD_LIBRARY)

$(OUT_DIR)/$(PROJECT)/%.o : %.c
	@echo "compile $^"
	@mkdir -p $(dir $@)
	$(CC) $(C_INCLUDE) $(C_FLAGS) $(C_OPTIONS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$^"

$(OUT_DIR)/$(PROJECT)/%.o : %.S
	@echo "compile $^"
	@mkdir -p $(dir $@)
	$(CC) $(C_INCLUDE) $(C_FLAGS) $(C_OPTIONS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$^"

pre-build:
	@rm -rf $(OUT_DIR)/$(PROJECT)/*.dmp $(OUT_DIR)/$(PROJECT)/*.map $(OUT_DIR)/$(PROJECT)/*.bin $(OUT_DIR)/$(PROJECT)/*.elf
	@echo ' '
	
post-build : $(TARGET)
	@ba-elf-objdump -d $(TARGET) > $(TARGET:%.elf=%.dmp)
	@ba-elf-nm $(TARGET) > $(TARGET:%.elf=%.map)
	@ba-elf-objcopy -O binary $(TARGET) $(TARGET:%.elf=%.bin)
	@tool/encrypt $(TARGET) 00000000
	@ba-elf-size -B $(TARGET)

clean :
	@rm -rf $(OUT_DIR)/$(PROJECT)
	