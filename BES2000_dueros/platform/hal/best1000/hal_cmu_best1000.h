#ifndef __HAL_CMU_BEST1000_H__
#define __HAL_CMU_BEST1000_H__

#ifdef __cplusplus
extern "C" {
#endif

enum HAL_CMU_MOD_ID_T {
    // HCLK/HRST
    HAL_CMU_MOD_H_MCU,          // 0
    HAL_CMU_MOD_H_ROM,          // 1
    HAL_CMU_MOD_H_RAM0,         // 2
    HAL_CMU_MOD_H_RAM1,         // 3
    HAL_CMU_MOD_H_AHB0,         // 4
    HAL_CMU_MOD_H_AHB1,         // 5
    HAL_CMU_MOD_H_AH2H,         // 6
    HAL_CMU_MOD_H_AH2H_BT,      // 7
    HAL_CMU_MOD_H_ADMA,         // 8
    HAL_CMU_MOD_H_GDMA,         // 9
    HAL_CMU_MOD_H_PSRAM,        // 10
    HAL_CMU_MOD_H_FLASH,        // 11
    HAL_CMU_MOD_H_SDIO,         // 12
    HAL_CMU_MOD_H_SDMMC,        // 13
    HAL_CMU_MOD_H_USBC,         // 14
    HAL_CMU_MOD_H_DPDRX,        // 15
    HAL_CMU_MOD_H_DPDTX,        // 16
    // PCLK/PRST
    HAL_CMU_MOD_P_CMU,          // 17
    HAL_CMU_MOD_P_GPIO,         // 18
    HAL_CMU_MOD_P_GPIO_INT,     // 19
    HAL_CMU_MOD_P_WDT,          // 20
    HAL_CMU_MOD_P_PWM,          // 21
    HAL_CMU_MOD_P_TIMER0,       // 22
    HAL_CMU_MOD_P_I2C0,         // 23
    HAL_CMU_MOD_P_SPI,          // 24
    HAL_CMU_MOD_P_SLCD,         // 25
    HAL_CMU_MOD_P_UART0,        // 26
    HAL_CMU_MOD_P_UART1,        // 27
    HAL_CMU_MOD_P_CODEC,        // 28
    HAL_CMU_MOD_P_PCM,          // 29
    HAL_CMU_MOD_P_I2S,          // 30
    HAL_CMU_MOD_P_SPI_ITN,      // 31
    HAL_CMU_MOD_P_IOMUX,        // 32
    HAL_CMU_MOD_P_SPDIF0,       // 33
    HAL_CMU_MOD_P_GLOBAL,       // 34
    // OCLK/ORST
    HAL_CMU_MOD_O_SLEEP,        // 35
    HAL_CMU_MOD_O_FLASH,        // 36
    HAL_CMU_MOD_O_PSRAM,        // 37
    HAL_CMU_MOD_O_USB,          // 38
    HAL_CMU_MOD_O_SDMMC,        // 39
    HAL_CMU_MOD_O_SDIO,         // 40
    HAL_CMU_MOD_O_WDT,          // 41
    HAL_CMU_MOD_O_BT,           // 42
    HAL_CMU_MOD_O_TIMER0,       // 43
    HAL_CMU_MOD_O_SPI,          // 44
    HAL_CMU_MOD_O_SLCD,         // 45
    HAL_CMU_MOD_O_UART0,        // 46
    HAL_CMU_MOD_O_UART1,        // 47
    HAL_CMU_MOD_O_CODEC_AD,     // 48
    HAL_CMU_MOD_O_CODEC_DA,     // 49
    HAL_CMU_MOD_O_I2S,          // 50
    HAL_CMU_MOD_O_SPDIF0,       // 51
    HAL_CMU_MOD_O_PCM,          // 52
    HAL_CMU_MOD_O_I2C0,         // 53
    HAL_CMU_MOD_O_SPI_ITN,      // 54
    HAL_CMU_MOD_O_GPIO,         // 55
    HAL_CMU_MOD_O_PWM0,         // 56
    HAL_CMU_MOD_O_PWM1,         // 57
    HAL_CMU_MOD_O_PWM2,         // 58
    HAL_CMU_MOD_O_PWM3,         // 59
    HAL_CMU_MOD_O_BTCPU,        // 60

    HAL_CMU_MOD_QTY,

    HAL_CMU_MOD_GLOBAL = HAL_CMU_MOD_P_GLOBAL,
    HAL_CMU_MOD_BT = HAL_CMU_MOD_O_BT,
    HAL_CMU_MOD_BTCPU = HAL_CMU_MOD_O_BTCPU,
};

enum HAL_CMU_CLOCK_OUT_ID_T {
    HAL_CMU_CLOCK_OUT_32K,          // 0
    HAL_CMU_CLOCK_OUT_6M5,          // 1
    HAL_CMU_CLOCK_OUT_13M,          // 2
    HAL_CMU_CLOCK_OUT_26M,          // 3
    HAL_CMU_CLOCK_OUT_52M,          // 4
    HAL_CMU_CLOCK_OUT_SYSPLL,       // 5
    HAL_CMU_CLOCK_OUT_48M,          // 6
    HAL_CMU_CLOCK_OUT_60M,          // 7
    HAL_CMU_CLOCK_OUT_CODEC_ADC,    // 8
    HAL_CMU_CLOCK_OUT_CODEC_HIGH,   // 9
    HAL_CMU_CLOCK_OUT_HCLK,         // 10
    HAL_CMU_CLOCK_OUT_PCLK,         // 11
    HAL_CMU_CLOCK_OUT_CODEC_ADC_IN, // 12
    HAL_CMU_CLOCK_OUT_BT,           // 13

    HAL_CMU_CLOCK_OUT_QTY
};

enum HAL_CMU_ANC_CLK_USER_T {
    HAL_CMU_ANC_CLK_USER_ANC,
    HAL_CMU_ANC_CLK_USER_AUXMIC,
    HAL_CMU_ANC_CLK_USER_EQ,

    HAL_CMU_ANC_CLK_USER_QTY
};

enum HAL_CMU_FIR_USER_T {
    HAL_CMU_FIR_USER_ANC,
    HAL_CMU_FIR_USER_EQ,

    HAL_CMU_FIR_USER_QTY
};

void hal_cmu_anc_enable(enum HAL_CMU_ANC_CLK_USER_T user);

void hal_cmu_anc_disable(enum HAL_CMU_ANC_CLK_USER_T user);

int hal_cmu_anc_get_status(enum HAL_CMU_ANC_CLK_USER_T user);

void hal_cmu_fir_high_speed_enable(enum HAL_CMU_FIR_USER_T user);

void hal_cmu_fir_high_speed_disable(enum HAL_CMU_FIR_USER_T user);

void hal_cmu_force_bt_sleep(void);

#ifdef __cplusplus
}
#endif

#endif

