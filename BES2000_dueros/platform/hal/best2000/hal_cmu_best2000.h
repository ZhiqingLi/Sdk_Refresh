#ifndef __HAL_CMU_BEST2000_H__
#define __HAL_CMU_BEST2000_H__

#ifdef __cplusplus
extern "C" {
#endif

enum HAL_CMU_MOD_ID_T {
    // HCLK/HRST
    HAL_CMU_MOD_H_MCU,          // 0
    HAL_CMU_MOD_H_ROM,          // 1
    HAL_CMU_MOD_H_RAM0A,        // 2
    HAL_CMU_MOD_H_RAM1,         // 3
    HAL_CMU_MOD_H_AHB0,         // 4
    HAL_CMU_MOD_H_AHB1,         // 5
    HAL_CMU_MOD_H_AH2H_BT,      // 6
    HAL_CMU_MOD_H_AH2H_WF,      // 7
    HAL_CMU_MOD_H_ADMA,         // 8
    HAL_CMU_MOD_H_GDMA,         // 9
    HAL_CMU_MOD_H_CACHE,        // 10
    HAL_CMU_MOD_H_PSRAM,        // 11
    HAL_CMU_MOD_H_FLASH,        // 12
    HAL_CMU_MOD_H_SDIO,         // 13
    HAL_CMU_MOD_H_SDMMC,        // 14
    HAL_CMU_MOD_H_USBC,         // 15
    HAL_CMU_MOD_H_CAM,          // 16
    HAL_CMU_MOD_H_CODEC,        // 17
    HAL_CMU_MOD_H_FFT,          // 18
    HAL_CMU_MOD_H_DSDIF,        // 19
    HAL_CMU_MOD_H_I2C_SLAVE,    // 20
    HAL_CMU_MOD_H_RAM2,         // 21
    HAL_CMU_MOD_H_RAM3,         // 22
    HAL_CMU_MOD_H_ROM1,         // 23
    HAL_CMU_MOD_H_RAM0B,        // 24
    HAL_CMU_MOD_H_USBH,         // 25
    // PCLK/PRST
    HAL_CMU_MOD_P_CMU,          // 26
    HAL_CMU_MOD_P_GPIO,         // 27
    HAL_CMU_MOD_P_GPIO_INT,     // 28
    HAL_CMU_MOD_P_WDT,          // 29
    HAL_CMU_MOD_P_PWM,          // 30
    HAL_CMU_MOD_P_TIMER0,       // 31
    HAL_CMU_MOD_P_I2C0,         // 32
    HAL_CMU_MOD_P_SPI,          // 33
    HAL_CMU_MOD_P_SLCD,         // 34
    HAL_CMU_MOD_P_SPI_ITN,      // 35
    HAL_CMU_MOD_P_UART0,        // 36
    HAL_CMU_MOD_P_UART1,        // 37
    HAL_CMU_MOD_P_UART2,        // 38
    HAL_CMU_MOD_P_PCM,          // 39
    HAL_CMU_MOD_P_I2S,          // 40
    HAL_CMU_MOD_P_SPDIF0,       // 41
    HAL_CMU_MOD_P_SPDIF1,       // 42
    HAL_CMU_MOD_P_TRANSQ_WF,    // 43
    HAL_CMU_MOD_P_SPI_PHY,      // 44
    // OCLK/ORST
    HAL_CMU_MOD_O_SLEEP,        // 45
    HAL_CMU_MOD_O_FLASH,        // 46
    HAL_CMU_MOD_O_PSRAM,        // 47
    HAL_CMU_MOD_O_USB,          // 48
    HAL_CMU_MOD_O_SDMMC,        // 49
    HAL_CMU_MOD_O_SDIO,         // 50
    HAL_CMU_MOD_O_WDT,          // 51
    HAL_CMU_MOD_O_TIMER0,       // 52
    HAL_CMU_MOD_O_I2C0,         // 53
    HAL_CMU_MOD_O_SPI,          // 54
    HAL_CMU_MOD_O_SLCD,         // 55
    HAL_CMU_MOD_O_UART0,        // 56
    HAL_CMU_MOD_O_UART1,        // 57
    HAL_CMU_MOD_O_UART2,        // 58
    HAL_CMU_MOD_O_I2S,          // 59
    HAL_CMU_MOD_O_SPDIF0,       // 60
    HAL_CMU_MOD_O_SPDIF1,       // 61
    HAL_CMU_MOD_O_PCM,          // 62
    HAL_CMU_MOD_O_SPI_ITN,      // 63
    HAL_CMU_MOD_O_GPIO,         // 64
    HAL_CMU_MOD_O_PWM0,         // 65
    HAL_CMU_MOD_O_PWM1,         // 66
    HAL_CMU_MOD_O_PWM2,         // 67
    HAL_CMU_MOD_O_PWM3,         // 68
    HAL_CMU_MOD_O_CAMP,         // 69
    HAL_CMU_MOD_O_CAMC,         // 70
    HAL_CMU_MOD_O_USB32K,       // 71
    HAL_CMU_MOD_O_SPI_PHY,      // 72

    // AON ACLK/ARST
    HAL_CMU_AON_A_CMU,          // 73
    HAL_CMU_AON_A_GPIO,         // 74
    HAL_CMU_AON_A_GPIO_INT,     // 75
    HAL_CMU_AON_A_WDT,          // 76
    HAL_CMU_AON_A_PWM,          // 77
    HAL_CMU_AON_A_TIMER,        // 78
    HAL_CMU_AON_A_PSC,          // 79
    HAL_CMU_AON_A_IOMUX,        // 80
    HAL_CMU_AON_A_AHBC,         // 81
    HAL_CMU_AON_A_APBC,         // 82
    HAL_CMU_AON_A_SRAM,         // 83
    HAL_CMU_AON_A_H2H_MCU,      // 84
    HAL_CMU_AON_A_H2H_BT,       // 85
    HAL_CMU_AON_A_H2H_WF,       // 86
    // AON OCLK/ORST
    HAL_CMU_AON_O_WDT,          // 87
    HAL_CMU_AON_O_TIMER,        // 88
    HAL_CMU_AON_O_GPIO,         // 89
    HAL_CMU_AON_O_PWM0,         // 90
    HAL_CMU_AON_O_PWM1,         // 91
    HAL_CMU_AON_O_PWM2,         // 92
    HAL_CMU_AON_O_PWM3,         // 93
    HAL_CMU_AON_O_IOMUX,        // 94
    HAL_CMU_AON_O_SLEEP_32K,    // 95
    HAL_CMU_AON_O_SLEEP_26M,    // 96
    // AON SUBSYS
    HAL_CMU_AON_MCU,            // 97
    HAL_CMU_AON_CODEC,          // 98
    HAL_CMU_AON_WIFI,           // 99
    HAL_CMU_AON_BT,             // 100
    HAL_CMU_AON_MCUCPU,         // 101
    HAL_CMU_AON_WIFICPU,        // 102
    HAL_CMU_AON_BTCPU,          // 103
    HAL_CMU_AON_GLOBAL,         // 104

    HAL_CMU_MOD_QTY,

    HAL_CMU_MOD_GLOBAL = HAL_CMU_AON_GLOBAL,
    HAL_CMU_MOD_BT = HAL_CMU_AON_BT,
    HAL_CMU_MOD_BTCPU = HAL_CMU_AON_BTCPU,
    HAL_CMU_MOD_WIFI = HAL_CMU_AON_WIFI,
    HAL_CMU_MOD_WIFICPU = HAL_CMU_AON_WIFICPU,

    // TO BE REMOVED
    HAL_CMU_MOD_P_CODEC = HAL_CMU_MOD_QTY,
    HAL_CMU_MOD_O_CODEC_DA = HAL_CMU_MOD_QTY,
    HAL_CMU_MOD_O_CODEC_AD = HAL_CMU_MOD_QTY,
};

enum HAL_CMU_CLOCK_OUT_ID_T {
    HAL_CMU_CLOCK_OUT_AON_32K           = 0x00,
    HAL_CMU_CLOCK_OUT_AON_26M           = 0x01,
    HAL_CMU_CLOCK_OUT_AON_52M           = 0x02,
    HAL_CMU_CLOCK_OUT_AON_DIG_52M       = 0x03,
    HAL_CMU_CLOCK_OUT_AON_DIG_104M      = 0x04,
    HAL_CMU_CLOCK_OUT_AON_PER           = 0x05,
    HAL_CMU_CLOCK_OUT_AON_USB           = 0x06,
    HAL_CMU_CLOCK_OUT_AON_DCDC0         = 0x07,
    HAL_CMU_CLOCK_OUT_AON_DCDC1         = 0x08,
    HAL_CMU_CLOCK_OUT_AON_DCDC2         = 0x09,
    HAL_CMU_CLOCK_OUT_AON_MCU           = 0x0A,
    HAL_CMU_CLOCK_OUT_AON_MEM           = 0x0B,
    HAL_CMU_CLOCK_OUT_AON_FLASH         = 0x0C,
    HAL_CMU_CLOCK_OUT_AON_SPDIF0        = 0x0D,
    HAL_CMU_CLOCK_OUT_AON_SYS           = 0x0E,
    HAL_CMU_CLOCK_OUT_AON_CHCLK         = 0x0F,

    HAL_CMU_CLOCK_OUT_WF                = 0x20,

    HAL_CMU_CLOCK_OUT_BT                = 0x40,

    HAL_CMU_CLOCK_OUT_MCU_32K           = 0x60,
    HAL_CMU_CLOCK_OUT_MCU_SYS           = 0x61,
    HAL_CMU_CLOCK_OUT_MCU_MEM           = 0x62,
    HAL_CMU_CLOCK_OUT_MCU_FLASH         = 0x63,
    HAL_CMU_CLOCK_OUT_MCU_USB           = 0x64,
    HAL_CMU_CLOCK_OUT_MCU_PCLK          = 0x65,
    HAL_CMU_CLOCK_OUT_MCU_I2S           = 0x66,
    HAL_CMU_CLOCK_OUT_MCU_PCM           = 0x67,
    HAL_CMU_CLOCK_OUT_MCU_SPDIF0        = 0x68,
    HAL_CMU_CLOCK_OUT_MCU_SPDIF1        = 0x69,
    HAL_CMU_CLOCK_OUT_MCU_SDMMC         = 0x6A,
    HAL_CMU_CLOCK_OUT_MCU_SDIO          = 0x6B,
    HAL_CMU_CLOCK_OUT_MCU_SPI0          = 0x6C,
    HAL_CMU_CLOCK_OUT_MCU_SPI1          = 0x6D,
    HAL_CMU_CLOCK_OUT_MCU_SPI2          = 0x6E,
    HAL_CMU_CLOCK_OUT_MCU_CAMP          = 0x6F,

    HAL_CMU_CLOCK_OUT_CODEC             = 0x80,
};

enum HAL_CMU_ANC_CLK_USER_T {
    HAL_CMU_ANC_CLK_USER_ANC,

    HAL_CMU_ANC_CLK_USER_QTY
};

void hal_cmu_anc_enable(enum HAL_CMU_ANC_CLK_USER_T user);

void hal_cmu_anc_disable(enum HAL_CMU_ANC_CLK_USER_T user);

int hal_cmu_anc_get_status(enum HAL_CMU_ANC_CLK_USER_T user);

void hal_cmu_init_chip_feature(uint16_t feature);

void hal_cmu_dma_swap_enable(uint32_t index);

void hal_cmu_dma_swap_disable(uint32_t index);

void hal_cmu_codec_reset_set(void);

void hal_cmu_codec_reset_clear(void);

void hal_cmu_codec_iir_enable(uint32_t speed);

void hal_cmu_codec_iir_disable(void);

int hal_cmu_codec_iir_set_div(uint32_t div);

void hal_cmu_classg_clock_enable(void);

void hal_cmu_classg_clock_disable(void);

void hal_cmu_bt_clock_enable(void);

void hal_cmu_bt_clock_disable(void);

void hal_cmu_bt_reset_set(void);

void hal_cmu_bt_reset_clear(void);

void hal_cmu_bt_module_init(void);

#ifdef __cplusplus
}
#endif

#endif

