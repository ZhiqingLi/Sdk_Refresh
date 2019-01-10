cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)

obj-y += $(CHIP)/

subdir-ccflags-y += -Iplatform/drivers/ana

ifeq ($(CHIP_HAS_SDMMC),1)
subdir-ccflags-y += -DCHIP_HAS_SDMMC
endif
ifeq ($(CHIP_HAS_SDIO),1)
subdir-ccflags-y += -DCHIP_HAS_SDIO
endif
ifeq ($(CHIP_HAS_PSRAM),1)
subdir-ccflags-y += -DCHIP_HAS_PSRAM
endif
ifeq ($(CHIP_HAS_SPI),1)
subdir-ccflags-y += -DCHIP_HAS_SPI
endif
ifeq ($(CHIP_HAS_SPILCD),1)
subdir-ccflags-y += -DCHIP_HAS_SPILCD
endif
ifeq ($(CHIP_HAS_EXT_PMU),1)
subdir-ccflags-y += -DCHIP_HAS_EXT_PMU
endif

ifeq ($(NO_LPU_26M),1)
CFLAGS_hal_cmu_common.o += -DNO_LPU_26M
endif
ifeq ($(MCU_HIGH_PERFORMANCE_MODE),1)
CFLAGS_hal_cmu_common.o += -DMCU_HIGH_PERFORMANCE_MODE
endif

CFLAGS_hal_dma.o += -DCHIP_HAS_DMA=$(CHIP_HAS_DMA)

ifeq ($(I2S_MCLK_FROM_SPDIF),1)
CFLAGS_hal_i2s.o += -DI2S_MCLK_FROM_SPDIF
endif

CFLAGS_hal_key.o += -Iutils/hwtimer_list

ifeq ($(NO_PWRKEY),1)
CFLAGS_hal_key.o += -DNO_PWRKEY
endif
ifeq ($(NO_GPIOKEY),1)
CFLAGS_hal_key.o += -DNO_GPIOKEY
endif
ifeq ($(NO_ADCKEY),1)
CFLAGS_hal_key.o += -DNO_ADCKEY
endif
ifeq ($(NO_GROUPKEY),1)
CFLAGS_hal_key.o += -DNO_GROUPKEY
endif

CFLAGS_hal_norflash.o += -Iplatform/drivers/norflash

ifeq ($(FLASH_LOW_SPEED),1)
CFLAGS_hal_cmu_common.o += -DFLASH_LOW_SPEED
CFLAGS_hal_norflash.o += -DFLASH_LOW_SPEED
endif
ifeq ($(SIMU_FAST_FLASH),1)
CFLAGS_hal_norflash.o += -DSIMU_FAST_FLASH
endif
ifeq ($(FLASH_DEEP_SLEEP),1)
CFLAGS_hal_norflash.o += -DFLASH_DEEP_SLEEP
endif
# add begin by cai.zhong 20190105 for unique id
ifeq ($(FLASH_UNIQUE_ID),1)
CFLAGS_hal_norflash.o += -DFLASH_UNIQUE_ID
endif
# add end by cai.zhong 20190105 for unique id
ifeq ($(OSC_26M_X4_AUD2BB),1)
CFLAGS_hal_norflash.o += -DOSC_26M_X4_AUD2BB
endif

CFLAGS_hal_norflash.o += -DCHIP_FLASH_CTRL_VER=$(CHIP_FLASH_CTRL_VER)
CFLAGS_hal_norflaship_v1.o += -Itests/programmer -DCHIP_FLASH_CTRL_VER=$(CHIP_FLASH_CTRL_VER)
CFLAGS_hal_norflaship_v2.o += -Itests/programmer -DCHIP_FLASH_CTRL_VER=$(CHIP_FLASH_CTRL_VER)

ifeq ($(NO_OVERLAY),1)
CFLAGS_hal_overlay.o += -DNO_OVERLAY
endif

CFLAGS_hal_sleep.o += -Iplatform/drivers/norflash

SLEEP_STATS_TRACE ?= 1
ifeq ($(SLEEP_STATS_TRACE),1)
CFLAGS_hal_sleep.o += -DSLEEP_STATS_TRACE
endif

ifeq ($(NO_SLEEP),1)
CFLAGS_hal_sleep.o += -DNO_SLEEP
endif
ifeq ($(NO_LIGHT_SLEEP),1)
CFLAGS_hal_sleep.o += -DNO_LIGHT_SLEEP
endif
ifeq ($(DEBUG_SLEEP_USER),1)
CFLAGS_hal_sleep.o += -DDEBUG_SLEEP_USER
endif

CFLAGS_hal_spi.o += -DCHIP_SPI_VER=$(CHIP_SPI_VER)

ifeq ($(SPI_ROM_ONLY),1)
CFLAGS_hal_spi.o += -DSPI_ROM_ONLY
endif
ifeq ($(CHIP_HAS_SPIPHY),1)
CFLAGS_hal_spi.o += -DCHIP_HAS_SPIPHY
endif

ifneq ($(TRACE_BUF_SIZE),)
CFLAGS_hal_trace.o += -DTRACE_BUF_SIZE=$(TRACE_BUF_SIZE)
endif
ifeq ($(FAULT_DUMP),1)
CFLAGS_hal_trace.o += -DFAULT_DUMP
endif
ifeq ($(CRASH_REBOOT),1)
CFLAGS_hal_trace.o += -DCRASH_REBOOT
endif
ifeq ($(FORCE_TRACE_UART1),1)
CFLAGS_hal_trace.o += -DFORCE_TRACE_UART1
endif

ifneq ($(UART_FREQ),)
CFLAGS_hal_uart.o += -DUART_FREQ=$(UART_FREQ)
endif

ifneq ($(DEBUG_PORT),)
CFLAGS_hal_uart.o += -DDEBUG_PORT=$(DEBUG_PORT)
endif

CFLAGS_hal_usb.o += -Iplatform/drivers/usb/usb_dev/inc -Iutils/hwtimer_list

ifeq ($(USB_ISO),1)
CFLAGS_hal_usb.o += -DUSB_ISO
endif

ifneq ($(ROM_BUILD),1)
ifneq ($(PROGRAMMER),1)
USB_SUSPEND ?= 1
PMU_USB_PIN_CHECK ?= 1
endif
endif
ifeq ($(USB_SUSPEND),1)
CFLAGS_hal_usb.o += -DUSB_SUSPEND
endif
ifeq ($(PMU_USB_PIN_CHECK),1)
CFLAGS_hal_usb.o += -DPMU_USB_PIN_CHECK
CFLAGS_hal_usbhost.o += -DPMU_USB_PIN_CHECK
endif

ifeq ($(USB_HIGH_SPEED),1)
CFLAGS_hal_usb.o += -DUSB_HIGH_SPEED
CFLAGS_hal_usbhost.o += -DUSB_HIGH_SPEED
endif

ifeq ($(USB_LPM),1)
CFLAGS_hal_usb.o += -DUSB_LPM
endif

ifeq ($(CHIP_HAS_USBPHY),1)
CFLAGS_hal_usb.o += -DCHIP_HAS_USBPHY
endif

CFLAGS_hal_usbhost.o += -Iutils/hwtimer_list

# CMU settings
export CMU_CFG_FLAGS :=

ifeq ($(AUDIO_RESAMPLE),1)
CMU_CFG_FLAGS += -D__AUDIO_RESAMPLE__
endif

ifeq ($(FLASH_LOW_SPEED),1)
CMU_CFG_FLAGS += -DFLASH_LOW_SPEED
endif

# PSC settings
export PSC_CFG_FLAGS :=

# CODEC settings
export CODEC_CFG_FLAGS :=

CODEC_CFG_FLAGS += $(MAX_DAC_OUTPUT_FLAGS)

ifeq ($(AUDIO_RESAMPLE),1)
CODEC_CFG_FLAGS += -D__AUDIO_RESAMPLE__
endif

ifeq ($(ANC_APP),1)
CODEC_CFG_FLAGS += -DANC_APP
endif

ifeq ($(AUDIO_OUTPUT_SW_GAIN),1)
CODEC_CFG_FLAGS += -DAUDIO_OUTPUT_SW_GAIN
endif

ifeq ($(AUDIO_OUTPUT_DC_CALIB),1)
CODEC_CFG_FLAGS += -DAUDIO_OUTPUT_DC_CALIB
endif
ifeq ($(AUDIO_OUTPUT_DC_CALIB_ANA),1)
CODEC_CFG_FLAGS += -DAUDIO_OUTPUT_DC_CALIB_ANA
endif

ifeq ($(NOSTD),1)
CODEC_CFG_FLAGS += -DNOSTD
endif

ifeq ($(CODEC_ACCURATE_SW_GAIN),1)
CODEC_CFG_FLAGS += -DCODEC_ACCURATE_SW_GAIN
endif

ifeq ($(NOISE_GATING),1)
CODEC_CFG_FLAGS += -DNOISE_GATING
endif

ifeq ($(NOISE_REDUCTION),1)
CODEC_CFG_FLAGS += -DNOISE_REDUCTION
endif

# IOMUX settings
export IOMUX_CFG_FLAGS :=

ifeq ($(IOMUX_DEFAULT_VOLTAGE_VMEM),1)
IOMUX_CFG_FLAGS += -DIOMUX_DEFAULT_VOLTAGE_VMEM
endif

ifeq ($(I2S_VOLTAGE_VMEM),1)
IOMUX_CFG_FLAGS += -DI2S_VOLTAGE_VMEM
endif

ifneq ($(I2S_IOMUX_INDEX),)
IOMUX_CFG_FLAGS += -DI2S_IOMUX_INDEX=$(I2S_IOMUX_INDEX)
endif

ifeq ($(SPDIF0_VOLTAGE_VMEM),1)
IOMUX_CFG_FLAGS += -DSPDIF0_VOLTAGE_VMEM
endif

ifneq ($(SPDIF0_IOMUX_INDEX),)
IOMUX_CFG_FLAGS += -DSPDIF0_IOMUX_INDEX=$(SPDIF0_IOMUX_INDEX)
endif

ifeq ($(SPDIF1_VOLTAGE_VMEM),1)
IOMUX_CFG_FLAGS += -DSPDIF1_VOLTAGE_VMEM
endif

ifneq ($(SPDIF1_IOMUX_INDEX),)
IOMUX_CFG_FLAGS += -DSPDIF1_IOMUX_INDEX=$(SPDIF1_IOMUX_INDEX)
endif

ifeq ($(DIGMIC_VOLTAGE_VMEM),1)
IOMUX_CFG_FLAGS += -DDIGMIC_VOLTAGE_VMEM
endif

ifneq ($(DIG_MIC_CK_IOMUX_INDEX),)
IOMUX_CFG_FLAGS += -DDIG_MIC_CK_IOMUX_INDEX=$(DIG_MIC_CK_IOMUX_INDEX)
endif

ifeq ($(SPI_VOLTAGE_VMEM),1)
IOMUX_CFG_FLAGS += -DSPI_VOLTAGE_VMEM
endif

ifeq ($(SPI_IOMUX_4WIRE),1)
IOMUX_CFG_FLAGS += -DSPI_IOMUX_4WIRE
endif

ifneq ($(SPI_IOMUX_INDEX),)
IOMUX_CFG_FLAGS += -DSPI_IOMUX_INDEX=$(SPI_IOMUX_INDEX)
endif

ifeq ($(SPILCD_VOLTAGE_VMEM),1)
IOMUX_CFG_FLAGS += -DSPILCD_VOLTAGE_VMEM
endif

export SPILCD_IOMUX_4WIRE ?= 1
ifeq ($(SPILCD_IOMUX_4WIRE),1)
IOMUX_CFG_FLAGS += -DSPILCD_IOMUX_4WIRE
endif

ifneq ($(SPILCD_IOMUX_INDEX),)
IOMUX_CFG_FLAGS += -DSPILCD_IOMUX_INDEX=$(SPILCD_IOMUX_INDEX)
endif

ifeq ($(I2C0_VOLTAGE_VMEM),1)
IOMUX_CFG_FLAGS += -DI2C0_VOLTAGE_VMEM
endif

ifneq ($(I2C0_IOMUX_INDEX),)
IOMUX_CFG_FLAGS += -DI2C0_IOMUX_INDEX=$(I2C0_IOMUX_INDEX)
endif

ifeq ($(I2C1_VOLTAGE_VMEM),1)
IOMUX_CFG_FLAGS += -DI2C1_VOLTAGE_VMEM
endif

ifneq ($(I2C1_IOMUX_INDEX),)
IOMUX_CFG_FLAGS += -DI2C1_IOMUX_INDEX=$(I2C1_IOMUX_INDEX)
endif

ifeq ($(CLKOUT_VOLTAGE_VMEM),1)
IOMUX_CFG_FLAGS += -DCLKOUT_VOLTAGE_VMEM
endif

ifneq ($(CLKOUT_IOMUX_INDEX),)
IOMUX_CFG_FLAGS += -DCLKOUT_IOMUX_INDEX=$(CLKOUT_IOMUX_INDEX)
endif

ifeq ($(CLK12M_VOLTAGE_VMEM),1)
IOMUX_CFG_FLAGS += -DCLK12M_VOLTAGE_VMEM
endif

ifneq ($(CLK12M_IOMUX_INDEX),)
IOMUX_CFG_FLAGS += -DCLK12M_IOMUX_INDEX=$(CLK12M_IOMUX_INDEX)
endif

ifneq ($(MCU_CLKOUT_IOMUX_INDEX),)
IOMUX_CFG_FLAGS += -DMCU_CLKOUT_IOMUX_INDEX=$(MCU_CLKOUT_IOMUX_INDEX)
endif

ifneq ($(BT_CLKOUT_IOMUX_INDEX),)
IOMUX_CFG_FLAGS += -DBT_CLKOUT_IOMUX_INDEX=$(BT_CLKOUT_IOMUX_INDEX)
endif

ccflags-y +=  -Iapps/key \
              -Iapps/app_anker/device_drivers/bes_hal
