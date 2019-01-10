#ifndef __HAL_CMU_BEST3001_H__
#define __HAL_CMU_BEST3001_H__

#ifdef __cplusplus
extern "C" {
#endif

enum HAL_CMU_MOD_ID_T {
    // HCLK/HRST
    HAL_CMU_MOD_H_MCU = 0,      // 0
    HAL_CMU_MOD_H_ROM,          // 1
    HAL_CMU_MOD_H_RAM0,         // 2
    HAL_CMU_MOD_H_RAM1,         // 3
    HAL_CMU_MOD_H_AHB0,         // 4
    HAL_CMU_MOD_H_AHB1,         // 5
    HAL_CMU_MOD_H_GDMA,         // 6
    HAL_CMU_MOD_H_EXTMEM,       // 7
    HAL_CMU_MOD_H_FLASH,        // 8
    HAL_CMU_MOD_H_USBC,         // 9
    HAL_CMU_MOD_H_CODEC,        // 10
    HAL_CMU_MOD_H_I2C_SLAVE,    // 11
    HAL_CMU_MOD_H_USBH,         // 12
    HAL_CMU_MOD_H_FFT,          // 13
    // PCLK/PRST
    HAL_CMU_MOD_P_CMU,          // 14
    HAL_CMU_MOD_P_GPIO,         // 15
    HAL_CMU_MOD_P_GPIO_INT,     // 16
    HAL_CMU_MOD_P_WDT,          // 17
    HAL_CMU_MOD_P_PWM,          // 18
    HAL_CMU_MOD_P_TIMER0,       // 19
    HAL_CMU_MOD_P_TIMER1,       // 20
    HAL_CMU_MOD_P_I2C0,         // 21
    HAL_CMU_MOD_P_SPI,          // 22
    HAL_CMU_MOD_P_SPI_ITN,      // 23
    HAL_CMU_MOD_P_UART0,        // 24
    HAL_CMU_MOD_P_UART1,        // 25
    HAL_CMU_MOD_P_I2S,          // 26
    HAL_CMU_MOD_P_SPDIF0,       // 27
    HAL_CMU_MOD_P_IOMUX,        // 28
    HAL_CMU_MOD_P_GLOBAL,       // 29
    // OCLK/ORST
    HAL_CMU_MOD_O_SLEEP,        // 30
    HAL_CMU_MOD_O_FLASH,        // 31
    HAL_CMU_MOD_O_USB,          // 32
    HAL_CMU_MOD_O_GPIO,         // 33
    HAL_CMU_MOD_O_WDT,          // 34
    HAL_CMU_MOD_O_TIMER0,       // 35
    HAL_CMU_MOD_O_TIMER1,       // 36
    HAL_CMU_MOD_O_I2C0,         // 37
    HAL_CMU_MOD_O_SPI,          // 38
    HAL_CMU_MOD_O_SPI_ITN,      // 39
    HAL_CMU_MOD_O_UART0,        // 40
    HAL_CMU_MOD_O_UART1,        // 41
    HAL_CMU_MOD_O_I2S,          // 42
    HAL_CMU_MOD_O_SPDIF0,       // 43
    HAL_CMU_MOD_O_USB32K,       // 44
    HAL_CMU_MOD_O_IOMUX,        // 45
    HAL_CMU_MOD_O_PWM0,         // 46
    HAL_CMU_MOD_O_PWM1,         // 47
    HAL_CMU_MOD_O_PWM2,         // 48
    HAL_CMU_MOD_O_PWM3,         // 49
    HAL_CMU_MOD_O_CODEC,        // 50
    HAL_CMU_MOD_O_CODECIIR,     // 51
    HAL_CMU_MOD_O_CODECRS,      // 52
    HAL_CMU_MOD_O_CODECADC,     // 53
    HAL_CMU_MOD_O_CODECAD0,     // 54
    HAL_CMU_MOD_O_CODECAD1,     // 55
    HAL_CMU_MOD_O_CODECDA0,     // 56
    HAL_CMU_MOD_O_CODECDA1,     // 57

    HAL_CMU_MOD_QTY,

    HAL_CMU_MOD_GLOBAL = HAL_CMU_MOD_P_GLOBAL,
};

enum HAL_CMU_CLOCK_OUT_ID_T {

    HAL_CMU_CLOCK_OUT_QTY,
};

enum HAL_CMU_I2S_MCLK_ID_T {

    HAL_CMU_I2S_MCLK_QTY,
};

void hal_cmu_codec_reset_set(void);

void hal_cmu_codec_reset_clear(void);

void hal_cmu_codec_iir_enable(uint32_t speed);

void hal_cmu_codec_iir_disable(void);

int hal_cmu_codec_iir_set_div(uint32_t div);

#ifdef __cplusplus
}
#endif

#endif

