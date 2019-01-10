
add_if_exists = $(foreach d,$(1),$(if $(wildcard $(srctree)/$(d)),$(d) ,))

# -------------------------------------------
# Root Option Dependencies
# -------------------------------------------

ifeq ($(BT_ANC),1)
export ANC_APP ?= 1
endif

# -------------------------------------------
# CHIP selection
# -------------------------------------------

export CHIP

ifneq (,)
else ifeq ($(CHIP),best3001)
KBUILD_CPPFLAGS += -DCHIP_BEST3001
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 0
export CHIP_HAS_SDIO := 0
export CHIP_HAS_PSRAM := 0
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 0
export CHIP_HAS_SPIPHY := 0
export CHIP_HAS_I2C := 1
export CHIP_HAS_UART := 2
export CHIP_HAS_DMA := 1
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_EXT_PMU := 0
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_FLASH_CTRL_VER := 1
export CHIP_SPI_VER := 3
else ifeq ($(CHIP),best2300)
KBUILD_CPPFLAGS += -DCHIP_BEST2300
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_SDIO := 0
export CHIP_HAS_PSRAM := 0
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 1
export CHIP_HAS_SPIPHY := 1
export CHIP_HAS_I2C := 2
export CHIP_HAS_UART := 3
export CHIP_HAS_DMA := 2
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_EXT_PMU := 1
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_FLASH_CTRL_VER := 2
export CHIP_SPI_VER := 2
else ifeq ($(CHIP),best2000)
KBUILD_CPPFLAGS += -DCHIP_BEST2000
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_SDIO := 1
export CHIP_HAS_PSRAM := 1
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 1
export CHIP_HAS_SPIPHY := 1
export CHIP_HAS_I2C := 1
export CHIP_HAS_UART := 3
export CHIP_HAS_DMA := 2
export CHIP_HAS_SPDIF := 2
export CHIP_HAS_TRANSQ := 1
export CHIP_HAS_EXT_PMU := 0
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_FLASH_CTRL_VER := 1
export CHIP_SPI_VER := 1
else ifeq ($(CHIP),best1200)
KBUILD_CPPFLAGS += -DCHIP_BEST1200
export CHIP_HAS_FPU := 0
export CHIP_HAS_USB := 0
export CHIP_HAS_USBPHY := 0
export CHIP_HAS_SDMMC := 0
export CHIP_HAS_SDIO := 0
export CHIP_HAS_PSRAM := 0
export CHIP_HAS_SPI := 0
export CHIP_HAS_SPILCD := 0
export CHIP_HAS_SPIPHY := 0
export CHIP_HAS_I2C := 1
export CHIP_HAS_UART := 1
export CHIP_HAS_DMA := 1
export CHIP_HAS_SPDIF := 0
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_EXT_PMU := 0
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_FLASH_CTRL_VER := 1
export CHIP_SPI_VER := 1
else
ifneq ($(CHIP),best1000)
$(error Invalid CHIP: $(CHIP))
endif
KBUILD_CPPFLAGS += -DCHIP_BEST1000
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 0
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_SDIO := 1
export CHIP_HAS_PSRAM := 1
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 1
export CHIP_HAS_SPIPHY := 0
export CHIP_HAS_I2C := 1
export CHIP_HAS_UART := 2
export CHIP_HAS_DMA := 2
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_EXT_PMU := 0
export CHIP_HAS_AUDIO_CONST_ROM := 1
export CHIP_FLASH_CTRL_VER := 1
export CHIP_SPI_VER := 1
endif

export FLASH_SIZE ?= 0x400000
PSRAM_SIZE ?= 0x400000

KBUILD_CPPFLAGS += -DCHIP_HAS_UART=$(CHIP_HAS_UART)
KBUILD_CPPFLAGS += -DCHIP_HAS_I2C=$(CHIP_HAS_I2C)

ifeq ($(CHIP_HAS_USB),1)
KBUILD_CPPFLAGS += -DCHIP_HAS_USB
endif

ifneq ($(CHIP_HAS_SPDIF),0)
KBUILD_CPPFLAGS += -DCHIP_HAS_SPDIF=$(CHIP_HAS_SPDIF)
endif

ifneq ($(CHIP_HAS_TRANSQ),0)
KBUILD_CPPFLAGS += -DCHIP_HAS_TRANSQ=$(CHIP_HAS_TRANSQ)
endif

ifeq ($(CHIP_HAS_AUDIO_CONST_ROM),1)
KBUILD_CPPFLAGS += -DCHIP_HAS_AUDIO_CONST_ROM
endif

ifeq ($(MCU_SLEEP_POWER_DOWN),1)
KBUILD_CPPFLAGS += -DMCU_SLEEP_POWER_DOWN
endif

ifeq ($(USB_AUDIO_APP),1)
ifneq ($(BTUSB_AUDIO_MODE),1)
NO_OVERLAY ?= 1
endif
endif
export NO_OVERLAY
ifeq ($(NO_OVERLAY),1)
CPPFLAGS_${LDS_FILE} += -DNO_OVERLAY
endif

ifneq ($(ROM_SIZE),)
KBUILD_CPPFLAGS += -DROM_SIZE=$(ROM_SIZE)
endif

ifneq ($(RAM_SIZE),)
KBUILD_CPPFLAGS += -DRAM_SIZE=$(RAM_SIZE)
endif

ifneq ($(FLASH_REGION_BASE),)
CPPFLAGS_${LDS_FILE} += -DFLASH_REGION_BASE=$(FLASH_REGION_BASE)
endif

ifneq ($(FLASH_REGION_SIZE),)
CPPFLAGS_${LDS_FILE} += -DFLASH_REGION_SIZE=$(FLASH_REGION_SIZE)
endif

ifeq ($(ANC_APP),1)
ifeq ($(CHIP),best1000)
AUD_SECTION_SIZE ?= 0x8000
else
AUD_SECTION_SIZE ?= 0x10000
endif
ifeq ($(ANC_FB_CHECK),1)
KBUILD_CPPFLAGS += -DANC_FB_CHECK
endif
else
AUD_SECTION_SIZE ?= 0
endif

ifeq ($(TWS),1)
LARGE_RAM ?= 1
endif

USERDATA_SECTION_SIZE ?= 0x1000

FACTORY_SECTION_SIZE ?= 0x1000

CUSTOM_PARAMETER_SECTION_SIZE ?= 0x1000

CPPFLAGS_${LDS_FILE} += \
	-DLINKER_SCRIPT \
	-DBUILD_INFO_MAGIC=0xBE57341D \
	-DFLASH_SIZE=$(FLASH_SIZE) \
	-DPSRAM_SIZE=$(PSRAM_SIZE) \
	-DAUD_SECTION_SIZE=$(AUD_SECTION_SIZE) \
	-DUSERDATA_SECTION_SIZE=$(USERDATA_SECTION_SIZE) \
	-DFACTORY_SECTION_SIZE=$(FACTORY_SECTION_SIZE) \
	-DCUSTOM_PARAMETER_SECTION_SIZE=$(CUSTOM_PARAMETER_SECTION_SIZE) \
	-Iplatform/hal

ifeq ($(LARGE_RAM),1)
KBUILD_CPPFLAGS += -DLARGE_RAM
endif

# -------------------------------------------
# Standard C library
# -------------------------------------------

export NOSTD

ifeq ($(NOSTD),1)

ifeq ($(MBED),1)
$(error Invalid configuration: MBED needs standard C library support)
endif
ifeq ($(RTOS),1)
$(error Invalid configuration: RTOS needs standard C library support)
endif

ifneq ($(NO_LIBC),1)
core-y += utils/libc/
endif

SPECS_CFLAGS :=

LIB_LDFLAGS := $(filter-out -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys,$(LIB_LDFLAGS))

KBUILD_CPPFLAGS += -nostdinc -ffreestanding -Iutils/libc/inc

CFLAGS_IMAGE += -nostdlib

CPPFLAGS_${LDS_FILE} += -DNOSTD

else # NOSTD != 1

SPECS_CFLAGS := --specs=nano.specs

LIB_LDFLAGS += -lm -lc -lgcc -lnosys

endif # NOSTD != 1

# -------------------------------------------
# RTOS library
# -------------------------------------------

export RTOS

ifeq ($(RTOS),1)

core-y += rtos/

KBUILD_CPPFLAGS += -DRTOS -D__CORTEX_M4F

KBUILD_CPPFLAGS += \
	-Irtos/rtos \
	-Irtos/rtx/TARGET_CORTEX_M \

ifeq ($(TWS),1)
OS_TASKCNT ?= 12
OS_SCHEDULERSTKSIZE ?= 768
OS_IDLESTKSIZE ?= 512
else
OS_TASKCNT ?= 10
OS_SCHEDULERSTKSIZE ?= 512
OS_IDLESTKSIZE ?= 256
endif
OS_CLOCK ?= 32000
OS_FIFOSZ ?= 24

export OS_TASKCNT
export OS_SCHEDULERSTKSIZE
export OS_IDLESTKSIZE
export OS_CLOCK
export OS_FIFOSZ

endif

# -------------------------------------------
# MBED library
# -------------------------------------------

export MBED

ifeq ($(MBED),1)

core-y += mbed/

KBUILD_CPPFLAGS += -DMBED

KBUILD_CPPFLAGS += \
	-Imbed/api \
	-Imbed/common \

endif

# -------------------------------------------
# DEBUG functions
# -------------------------------------------

export DEBUG

ifeq ($(DEBUG),1)

KBUILD_CFLAGS	+= -O2
KBUILD_CPPFLAGS	+= -DDEBUG

else

KBUILD_CFLAGS	+= -Os
KBUILD_CPPFLAGS	+= -DNDEBUG

endif

# -------------------------------------------
# SIMU functions
# -------------------------------------------

export SIMU

ifeq ($(SIMU),1)

KBUILD_CPPFLAGS += -DSIMU

endif

# -------------------------------------------
# FPGA functions
# -------------------------------------------

export FPGA

ifeq ($(FPGA),1)

KBUILD_CPPFLAGS += -DFPGA

endif

# -------------------------------------------
# ROM_BUILD functions
# -------------------------------------------

export ROM_BUILD

ifeq ($(ROM_BUILD),1)

KBUILD_CPPFLAGS += -DROM_BUILD

endif

# Limit the length of REVISION_INFO if ROM_BUILD or using rom.lds
ifneq ($(filter 1,$(ROM_BUILD))$(filter rom.lds,$(LDS_FILE)),)
ifeq ($(CHIP),best1000)
REVISION_INFO := x
else
REVISION_INFO := $(GIT_REVISION)
endif
endif

# -------------------------------------------
# PROGRAMMER functions
# -------------------------------------------

export PROGRAMMER

ifeq ($(PROGRAMMER),1)

KBUILD_CPPFLAGS += -DPROGRAMMER

endif

# -------------------------------------------
# Features
# -------------------------------------------

export DEBUG_PORT ?= 1

export AUD_SECTION_STRUCT_VERSION ?= 1

ifneq ($(AUD_SECTION_STRUCT_VERSION),)
KBUILD_CPPFLAGS += -DAUD_SECTION_STRUCT_VERSION=$(AUD_SECTION_STRUCT_VERSION)
endif

export FLASH_CHIP
ifneq ($(FLASH_CHIP),)
VALID_FLASH_CHIP_LIST := ALL GD25Q32C GD25Q80C GD25LQ32C GD25LQ32D EN25S80B P25Q80H P25Q32L
ifneq ($(filter-out $(VALID_FLASH_CHIP_LIST),$(FLASH_CHIP)),)
$(error Invalid FLASH_CHIP: $(filter-out $(VALID_FLASH_CHIP_LIST),$(FLASH_CHIP)))
endif
endif

NV_REC_DEV_VER ?= 2

export NO_SLEEP ?= 0

export FAULT_DUMP ?= 1

export CRASH_BOOT ?= 0

export OSC_26M_X4_AUD2BB ?= 0
ifeq ($(OSC_26M_X4_AUD2BB),1)
export ANA_26M_X4_ENABLE ?= 1
export FLASH_LOW_SPEED ?= 0
endif

export BTADDR_GEN ?= 1

export BT_ONE_BRING_TWO ?= 0

ifeq ($(BLE),0)				
export INTERSYS_NO_THREAD ?= 1
else
export INTERSYS_NO_THREAD ?= 0
endif

export SBC_FUNC_IN_ROM ?= 0

export VOICE_DETECT ?= 0

export VOICE_PROMPT ?= 1

export AUDIO_QUEUE_SUPPORT ?= 1

export VOICE_RECOGNITION ?= 0

export ENGINEER_MODE ?= 1
ifeq ($(ENGINEER_MODE),1)
FACTORY_MODE := 1
endif

export AUDIO_SCO_BTPCM_CHANNEL ?= 1

export AUDIO_CODEC_ASYNC_CLOSE ?= 0

# Enable the workaround for BEST1000 version C & earlier chips
export CODEC_PLAY_BEFORE_CAPTURE ?= 0

export AUDIO_INPUT_CAPLESSMODE ?= 0

export AUDIO_INPUT_LARGEGAIN ?= 0

export AUDIO_INPUT_MONO ?= 0

export AUDIO_OUTPUT_MONO ?= 0

export AUDIO_OUTPUT_VOLUME_DEFAULT ?= 10

export BTUSB_AUDIO_MODE ?= 0
ifeq ($(BTUSB_AUDIO_MODE),1)
KBUILD_CPPFLAGS += -DBTUSB_AUDIO_MODE
endif

ifeq ($(CHIP),best1000)
AUDIO_OUTPUT_DIFF ?= 1
AUDIO_OUTPUT_DC_CALIB ?= $(AUDIO_OUTPUT_DIFF)
AUDIO_OUTPUT_SMALL_GAIN_ATTN ?= 1
AUDIO_OUTPUT_SW_GAIN ?= 1
ANC_L_R_MISALIGN_WORKAROUND ?= 1
else ifeq ($(CHIP),best2000)
ifeq ($(USB_AUDIO_APP),1)
export VCODEC_VOLT ?= 2.5V
else
export VCODEC_VOLT ?= 1.6V
endif
AUDIO_OUTPUT_DIFF ?= 0
ifeq ($(VCODEC_VOLT),2.5V)
AUDIO_OUTPUT_DC_CALIB ?= 0
AUDIO_OUTPUT_DC_CALIB_ANA ?= 1
else
AUDIO_OUTPUT_DC_CALIB ?= 1
AUDIO_OUTPUT_DC_CALIB_ANA ?= 0
endif
ifneq ($(AUDIO_OUTPUT_DIFF),1)
DAC_CLASSG_ENABLE ?= 1
endif
else ifeq ($(CHIP),best2300)
AUDIO_OUTPUT_DC_CALIB ?= 0
AUDIO_OUTPUT_DC_CALIB_ANA ?= 1
endif

ifeq ($(AUDIO_OUTPUT_DC_CALIB)-$(AUDIO_OUTPUT_DC_CALIB_ANA),1-1)
$(error AUDIO_OUTPUT_DC_CALIB and AUDIO_OUTPUT_DC_CALIB_ANA cannot be enabled at the same time)
endif

export AUDIO_OUTPUT_DIFF

export AUDIO_OUTPUT_DC_CALIB

export AUDIO_OUTPUT_DC_CALIB_ANA

export AUDIO_OUTPUT_SMALL_GAIN_ATTN

export AUDIO_OUTPUT_SW_GAIN

export ANC_L_R_MISALIGN_WORKAROUND

export DAC_CLASSG_ENABLE

ifeq ($(ANC_APP),1)
export ANC_FF_ENABLED ?= 1
endif

export AUDIO_RESAMPLE ?= 0
ifeq ($(AUDIO_RESAMPLE),1)
ifeq ($(CHIP),best1000)
export SW_PLAYBACK_RESAMPLE ?= 1
export SW_CAPTURE_RESAMPLE ?= 1
export NO_SCO_RESAMPLE ?= 1
endif # CHIP is best1000
ifeq ($(CHIP),best2000)
export SW_PLAYBACK_RESAMPLE ?= 1
export SW_CAPTURE_RESAMPLE ?= 0
export SW_SCO_RESAMPLE ?= 0
export NO_SCO_RESAMPLE ?= 1
endif # CHIP is best2000
ifeq ($(BT_ANC),1)
ifeq ($(NO_SCO_RESAMPLE),1)
$(error BT_ANC and NO_SCO_RESAMPLE cannot be enabled at the same time)
endif
endif # BT_ANC
endif # AUDIO_RESAMPLE

export HW_FIR_EQ_PROCESS ?= 0

export SW_IIR_EQ_PROCESS ?= 0

export HW_DAC_IIR_EQ_PROCESS ?= 0

export AUDIO_DRC ?= 0

ifeq ($(USB_AUDIO_APP),1)
ifneq ($(BTUSB_AUDIO_MODE),1)
NO_PWRKEY ?= 1
NO_GROUPKEY ?= 1
endif
endif

export NO_PWRKEY

export NO_GROUPKEY

export APP_TEST_AUDIO ?= 0

export APP_TEST_MODE ?= 0
ifeq ($(APP_TEST_MODE),1)
KBUILD_CPPFLAGS += -DAPP_TEST_MODE
endif

ifeq ($(CHIP),best1000)

ifeq ($(AUD_PLL_DOUBLE),1)
KBUILD_CPPFLAGS += -DAUD_PLL_DOUBLE
endif

ifeq ($(DUAL_AUX_MIC),1)
ifeq ($(AUDIO_INPUT_MONO),1)
$(error Invalid talk mic configuration)
endif
KBUILD_CPPFLAGS += -D_DUAL_AUX_MIC_
endif

endif # best1000

ifeq ($(CAPTURE_ANC_DATA),1)
KBUILD_CPPFLAGS += -DCAPTURE_ANC_DATA
endif

ifeq ($(AUDIO_ANC_FB_MC),1)
KBUILD_CPPFLAGS += -DAUDIO_ANC_FB_MC
endif

ifeq ($(BT_ANC),1)
KBUILD_CPPFLAGS += -D__BT_ANC__
endif

ifeq ($(HEAR_THRU_PEAK_DET),1)
KBUILD_CPPFLAGS += -D__HEAR_THRU_PEAK_DET__
endif

ifeq ($(FACTORY_MODE),1)
KBUILD_CPPFLAGS += -D__FACTORY_MODE_SUPPORT__
endif

ifeq ($(WATCHER_DOG),1)
KBUILD_CPPFLAGS += -D__WATCHER_DOG_RESET__
endif

ifeq ($(ULTRA_LOW_POWER),1)
export FLASH_LOW_SPEED ?= 1
KBUILD_CPPFLAGS += -DULTRA_LOW_POWER
endif

ifeq ($(SIMPLE_TASK_SWITCH),1)
KBUILD_CPPFLAGS += -DSIMPLE_TASK_SWITCH
endif

ifeq ($(ASSERT_SHOW_FILE_FUNC),1)
KBUILD_CPPFLAGS += -DASSERT_SHOW_FILE_FUNC
else
ifeq ($(ASSERT_SHOW_FILE),1)
KBUILD_CPPFLAGS += -DASSERT_SHOW_FILE
else
ifeq ($(ASSERT_SHOW_FUNC),1)
KBUILD_CPPFLAGS += -DASSERT_SHOW_FUNC
endif
endif
endif

ifeq ($(MAX_DAC_OUTPUT),-60db)
MAX_DAC_OUTPUT_FLAGS := -DMAX_DAC_OUTPUT_M60DB
else
ifeq ($(MAX_DAC_OUTPUT),3.75mw)
MAX_DAC_OUTPUT_FLAGS := -DMAX_DAC_OUTPUT_3P75MW
else
ifeq ($(MAX_DAC_OUTPUT),5mw)
MAX_DAC_OUTPUT_FLAGS := -DMAX_DAC_OUTPUT_5MW
else
ifeq ($(MAX_DAC_OUTPUT),10mw)
MAX_DAC_OUTPUT_FLAGS := -DMAX_DAC_OUTPUT_10MW
else
ifneq ($(MAX_DAC_OUTPUT),30mw)
ifneq ($(MAX_DAC_OUTPUT),)
$(error Invalid MAX_DAC_OUTPUT value: $(MAX_DAC_OUTPUT) (MUST be one of: -60db 3.75mw 5mw 10mw 30mw))
endif
endif
endif
endif
endif
endif
export MAX_DAC_OUTPUT_FLAGS

ifeq ($(SBC_FUNC_IN_ROM),1)
# Best1000 Ver-G or future chips
KBUILD_CPPFLAGS += -D__SBC_FUNC_IN_ROM__
endif

ifeq ($(HFP_1_6_ENABLE),1)
KBUILD_CPPFLAGS += -DHFP_1_6_ENABLE
ifeq ($(MSBC_PLC_ENABLE),1)
KBUILD_CPPFLAGS += -DMSBC_PLC_ENABLE
endif
ifeq ($(MSBC_16K_SAMPLE_RATE),1)
KBUILD_CPPFLAGS += -DMSBC_16K_SAMPLE_RATE
endif
endif

export A2DP_AAC_ON ?= 0
ifeq ($(A2DP_AAC_ON),1)
KBUILD_CPPFLAGS += -DA2DP_AAC_ON
endif

ifeq ($(USER_SECURE_BOOT),1)
KBUILD_CPPFLAGS += -DUSER_SECURE_BOOT
endif

ifeq ($(USER_SECURE_BOOT),1)
core-y += utils/user_secure_boot/ \
               utils/system_info/
endif

export A2DP_SCALABLE_ON ?= 0
ifeq ($(A2DP_SCALABLE_ON),1)
KBUILD_CPPFLAGS += -DA2DP_SCALABLE_ON
endif

export A2DP_AAC_DIRECT_TRANSFER ?= 0
ifeq ($(A2DP_AAC_DIRECT_TRANSFER),1)
KBUILD_CPPFLAGS += -DA2DP_AAC_DIRECT_TRANSFER
endif

export BLE ?= 0
ifeq ($(BLE),1)
core-y += utils/jansson/
KBUILD_CPPFLAGS += -DIAG_BLE_INCLUDE
endif

export SPEECH_TX_NS ?= 0
ifeq ($(SPEECH_TX_NS),1)
KBUILD_CPPFLAGS += -DSPEECH_TX_NS
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
endif

export SPEECH_TX_NS2 ?= 0
ifeq ($(SPEECH_TX_NS2),1)
KBUILD_CPPFLAGS += -DSPEECH_TX_NS2
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
endif

export SPEECH_TX_NS2FLOAT ?= 0
ifeq ($(SPEECH_TX_NS2FLOAT),1)
KBUILD_CPPFLAGS += -DSPEECH_TX_NS2FLOAT
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
export DSP_LIB ?= 1
endif

export SPEECH_TX_2MIC_NS2 ?= 0
ifeq ($(SPEECH_TX_2MIC_NS2),1)
KBUILD_CPPFLAGS += -DSPEECH_TX_2MIC_NS2
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
KBUILD_CPPFLAGS += -DSPEECH_CAPTURE_TWO_CHANNEL
endif

export SPEECH_TX_AEC ?= 0
ifeq ($(SPEECH_TX_AEC),1)
KBUILD_CPPFLAGS += -DSPEECH_TX_AEC
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
endif

export SPEECH_TX_AEC2 ?= 0
ifeq ($(SPEECH_TX_AEC2),1)
KBUILD_CPPFLAGS += -DSPEECH_TX_AEC2
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
endif

export SPEECH_TX_AEC2FLOAT ?= 0
ifeq ($(SPEECH_TX_AEC2FLOAT),1)
KBUILD_CPPFLAGS += -DSPEECH_TX_AEC2FLOAT
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
endif

export UNALIGNED_ACCESS  ?= 1

export SPEECH_TX_2MIC_NS ?= 0
ifeq ($(SPEECH_TX_2MIC_NS),1)
KBUILD_CPPFLAGS += -DSPEECH_TX_2MIC_NS
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
KBUILD_CPPFLAGS += -DSPEECH_CAPTURE_TWO_CHANNEL
endif

export SPEECH_TX_2MIC_NS3 ?= 0
ifeq ($(SPEECH_TX_2MIC_NS3),1)
KBUILD_CPPFLAGS += -DSPEECH_TX_2MIC_NS3
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
KBUILD_CPPFLAGS += -DSPEECH_CAPTURE_TWO_CHANNEL
endif

export SPEECH_TX_AGC ?= 0
ifeq ($(SPEECH_TX_AGC),1)
KBUILD_CPPFLAGS += -DSPEECH_TX_AGC
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
endif

export SPEECH_TX_EQ ?= 0
ifeq ($(SPEECH_TX_EQ),1)
KBUILD_CPPFLAGS += -DSPEECH_TX_EQ
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
endif

export SPEECH_RX_NS ?= 0
ifeq ($(SPEECH_RX_NS),1)
KBUILD_CPPFLAGS += -DSPEECH_RX_NS
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
endif

export SPEECH_RX_NS2 ?= 0
ifeq ($(SPEECH_RX_NS2),1)
KBUILD_CPPFLAGS += -DSPEECH_RX_NS2
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
endif

export SPEECH_RX_NS2FLOAT ?= 0
ifeq ($(SPEECH_RX_NS2FLOAT),1)
KBUILD_CPPFLAGS += -DSPEECH_RX_NS2FLOAT
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
export DSP_LIB ?= 1
endif

export SPEECH_RX_AGC ?= 0
ifeq ($(SPEECH_RX_AGC),1)
KBUILD_CPPFLAGS += -DSPEECH_RX_AGC
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
endif

export SPEECH_RX_EQ ?= 0
ifeq ($(SPEECH_RX_EQ),1)
KBUILD_CPPFLAGS += -DSPEECH_RX_EQ
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
endif

export SPEECH_RX_PLC ?= 1

export SPEECH_CODEC_FRAME_MS 	?= 16

export SPEECH_SCO_FRAME_MS 		?= 15

export APP_LINEIN_A2DP_SOURCE ?= 0
ifeq ($(APP_LINEIN_A2DP_SOURCE),1)
KBUILD_CPPFLAGS += -DAPP_LINEIN_A2DP_SOURCE
endif

export HSP_ENABLE ?=0
ifeq ($(HSP_ENABLE),1)
KBUILD_CPPFLAGS += -D__HSP_ENABLE__
endif
export APP_I2S_A2DP_SOURCE ?= 0
ifeq ($(APP_I2S_A2DP_SOURCE),1)
KBUILD_CPPFLAGS += -DAPP_I2S_A2DP_SOURCE
endif

export ANKER_UI ?= 0
ifeq ($(ANKER_UI),1)
KBUILD_CPPFLAGS += -D__ANKER_UI__
endif

export CONEXANT_CX20921_ENABLE ?= 0
ifeq ($(CONEXANT_CX20921_ENABLE),1)
KBUILD_CPPFLAGS += -DVOICE_INPUT_MODULE_CX20921
endif

export CODEC_AK7755_ENABLE ?= 0
ifeq ($(CODEC_AK7755_ENABLE),1)
KBUILD_CPPFLAGS += -DEXTRAL_CODEC_AK7755_ENABLE
endif

export SMARTVOICE ?= 0
ifeq ($(SMARTVOICE),1)
KBUILD_CPPFLAGS += -D__SMARTVOICE__
endif

export AMA_VOICE ?= 0
ifeq ($(AMA_VOICE),1)
KBUILD_CPPFLAGS += -D__AMA_VOICE__
endif

export DMA_VOICE ?= 0
ifeq ($(DMA_VOICE),1)
KBUILD_CPPFLAGS += -D__DMA_VOICE__
KBUILD_CPPFLAGS += -D__BES__
KBUILD_CPPFLAGS += -DEXPORT_FN_ROM_CUST=1
KBUILD_CPPFLAGS += -DBAIDU_DATA_SN_LEN=16
KBUILD_CPPFLAGS += -DFLOW_CONTROL_ON_UPLEVEL=1
KBUILD_CPPFLAGS += -DASSAM_PKT_ON_UPLEVEL=1
KBUILD_CPPFLAGS += -DNVREC_BAIDU_DATA_SECTION=1
KBUILD_CPPFLAGS += -DBAIDU_DATA_RAND_LEN=8
KBUILD_CPPFLAGS += -DCLOSE_BLE_ADV_WHEN_VOICE_CALL=1
KBUILD_CPPFLAGS += -DCLOSE_BLE_ADV_WHEN_SPP_CONNECTED=1
KBUILD_CPPFLAGS += -DBAIDU_RFCOMM_DIRECT_CONN=1
KBUILD_CPPFLAGS += -DBYPASS_SLOW_ADV_MODE=1
endif

export KNOWLES_I2S_INTERFACE ?= 0
ifeq ($(KNOWLES_I2S_INTERFACE),1)
KBUILD_CPPFLAGS += -DKNOWLES_I2S_INTERFACE
endif

export OTA_ENABLED ?= 0
ifeq ($(OTA_ENABLED), 1)
KBUILD_CPPFLAGS += -DOTA_ENABLED
endif

KBUILD_CPPFLAGS += -DSPEECH_CODEC_FRAME_MS=$(SPEECH_CODEC_FRAME_MS)
KBUILD_CPPFLAGS += -DSPEECH_SCO_FRAME_MS=$(SPEECH_SCO_FRAME_MS)
KBUILD_CPPFLAGS += -DCOH_FRAME_LENGTH=$(COH_FRAME_LENGTH)
KBUILD_CPPFLAGS += -DLC_MMSE_FRAME_LENGTH=$(LC_MMSE_FRAME_LENGTH)
KBUILD_CPPFLAGS += -D__RTX_CPU_STATISTICS__=1
KBUILD_CPPFLAGS += -D_CPU_STATISTICS_PEROID_=6000

# -------------------------------------------
# General
# -------------------------------------------

export LIBC_ROM

ifneq ($(NO_CONFIG),1)
core-y += config/
endif

ifneq ($(NO_BOOT_STRUCT),1)
core-y += $(call add_if_exists,utils/boot_struct/)
endif

export DEFAULT_CFG_SRC ?= _default_cfg_src_

ifneq ($(wildcard $(srctree)/config/$(T)/tgt_hardware.h $(srctree)/config/$(T)/res/),)
KBUILD_CPPFLAGS += -Iconfig/$(T)
endif
KBUILD_CPPFLAGS += -Iconfig/$(DEFAULT_CFG_SRC)

CPU_CFLAGS := -mcpu=cortex-m4 -mthumb

export UNALIGNED_ACCESS
ifeq ($(UNALIGNED_ACCESS),1)
KBUILD_CPPFLAGS += -DUNALIGNED_ACCESS
else
CPU_CFLAGS += -mno-unaligned-access
endif

ifeq ($(CHIP_HAS_FPU),1)
CPU_CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
else
CPU_CFLAGS += -mfloat-abi=soft
endif

ifeq ($(A2DP_AAC_ON),1)
KBUILD_CPPFLAGS +=               -D__ACC_FRAGMENT_COMPATIBLE__
endif
KBUILD_CPPFLAGS += $(CPU_CFLAGS) $(SPECS_CFLAGS) -DTOOLCHAIN_GCC -DTARGET_BEST1000
LINK_CFLAGS += $(CPU_CFLAGS) $(SPECS_CFLAGS)
CFLAGS_IMAGE += $(CPU_CFLAGS) $(SPECS_CFLAGS)

