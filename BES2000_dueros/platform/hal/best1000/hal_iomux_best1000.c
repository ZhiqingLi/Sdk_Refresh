#include "plat_types.h"
#include "reg_iomuxip_best1000.h"
#include "hal_iomuxip_best1000.h"
#include "reg_cmu_best1000.h"
#include "hal_cmu.h"
#include "hal_iomux.h"
#include "hal_uart.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_location.h"
#include "hal_chipid.h"
#include "cmsis.h"

/* i2c mode is specail : 0 means i2c, 1 means gpio */
#define I2C_IS_SPECIAL                      1

#define HAL_IOMUX_YES                       1
#define HAL_IOMUX_NO                        0

#define HAL_IOMUX_MAP_NUM                   6

#ifdef I2S_VOLTAGE_VMEM
#define I2S_VOLTAGE_SEL                     HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define I2S_VOLTAGE_SEL                     HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef SPDIF0_VOLTAGE_VMEM
#define SPDIF0_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define SPDIF0_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef DIGMIC_VOLTAGE_VMEM
#define DIGMIC_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define DIGMIC_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef SPI_VOLTAGE_VMEM
#define SPI_VOLTAGE_SEL                     HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define SPI_VOLTAGE_SEL                     HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef SPILCD_VOLTAGE_VMEM
#define SPILCD_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define SPILCD_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef CLKOUT_VOLTAGE_VMEM
#define CLKOUT_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define CLKOUT_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifndef I2S_IOMUX_INDEX
#ifdef FPGA
#define I2S_IOMUX_INDEX                     0
#else
#define I2S_IOMUX_INDEX                     1
#endif
#endif

#ifndef SPI_IOMUX_INDEX
#define SPI_IOMUX_INDEX                     0
#endif

#ifndef CLKOUT_IOMUX_INDEX
#define CLKOUT_IOMUX_INDEX                  0
#endif

struct pin_function_regbit_map {
    enum HAL_IOMUX_FUNCTION_T func;
    uint32_t reg_offset;
    uint32_t bit_mask;
};

/* see 192.168.1.3/dokuwiki/doku.php?id=projects:best1000#pinpad */
/* should put alt function in priority order (see rtl) */
static const struct pin_function_regbit_map _pfr_map[HAL_IOMUX_PIN_NUM][HAL_IOMUX_MAP_NUM+1] = {
    /* 0 0 */ {
                {HAL_IOMUX_FUNC_SDIO_CLK, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDIO_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 0 1 */ {
                {HAL_IOMUX_FUNC_SDIO_CMD, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDIO_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 0 2 */ {
                {HAL_IOMUX_FUNC_SDIO_DATA0, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDIO_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 0 3 */ {
                {HAL_IOMUX_FUNC_SDIO_DATA1, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDIO_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 0 4 */ {
                {HAL_IOMUX_FUNC_SDIO_DATA2, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDIO_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 0 5 */ {
                {HAL_IOMUX_FUNC_SDIO_DATA3, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDIO_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 0 6 */ {
                {HAL_IOMUX_FUNC_SDIO_INT_M, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDIO_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 0 7 */ {
                {HAL_IOMUX_FUNC_SDEMMC_RST_N, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDMMC_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_CLK_32K_IN, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_CLK32K_IN_MASK},
                {HAL_IOMUX_FUNC_SPI_CS, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPI2_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SMP0, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SMP_EN_MASK},
                {HAL_IOMUX_FUNC_RF_ANA_SWTX, IOMUXIP_MUX10_REG_OFFSET, IOMUXIP_MUX10_RF_ANA_SW2_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },

    /* 1 0 */ {
                {HAL_IOMUX_FUNC_I2S_WS, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_I2S1_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SPDIF0_DI, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPDIF1_DIN_EN_MASK},
                {HAL_IOMUX_FUNC_RF_ANA_SWTX, IOMUXIP_MUX10_REG_OFFSET, IOMUXIP_MUX10_RF_ANA_SW1_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 1 1 */ {
                {HAL_IOMUX_FUNC_I2S_SDI0, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_I2S1_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SPDIF0_DO, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPDIF1_DOUT_EN_MASK},
                {HAL_IOMUX_FUNC_RF_ANA_SWRX, IOMUXIP_MUX10_REG_OFFSET, IOMUXIP_MUX10_RF_ANA_SW1_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 1 2 */ {
                {HAL_IOMUX_FUNC_I2S_SDO, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_I2S1_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_PDM0_D, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_MIC_DIG_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 1 3 */ {
                {HAL_IOMUX_FUNC_I2S_SCK, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_I2S1_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_PDM0_CK, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_MIC_DIG_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 1 4 */ {
                {HAL_IOMUX_FUNC_SDMMC_CLK, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDMMC_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_I2S_WS, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_I2S2_GPIO_EN_MASK},
                /* TODO agpio 0 */
                {HAL_IOMUX_FUNC_SMP1, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SMP_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 1 5 */ {
                {HAL_IOMUX_FUNC_SDMMC_CMD, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDMMC_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_I2S_SDI0, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_I2S2_GPIO_EN_MASK},
                /* TODO agpio 1 */
                {HAL_IOMUX_FUNC_SMP2, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SMP_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 1 6 */ {
                {HAL_IOMUX_FUNC_SDMMC_DATA0, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDMMC_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_I2S_SDO, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_I2S2_GPIO_EN_MASK},
                /* TODO agpio 2 */
                {HAL_IOMUX_FUNC_SMP3, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SMP_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 1 7 */ {
                {HAL_IOMUX_FUNC_SDMMC_DATA1, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDMMC_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_I2S_SCK, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_I2S2_GPIO_EN_MASK},
                /* TODO agpio 3 */
                {HAL_IOMUX_FUNC_SMP4, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SMP_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },

    /* 2 0 */ {
                {HAL_IOMUX_FUNC_SDMMC_DATA2, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDMMC_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_PDM0_CK, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_MIC2_DIG_EN_MASK},
                {HAL_IOMUX_FUNC_SPI_CLK, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPI2_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SMP5, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SMP_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 2 1 */ {
                {HAL_IOMUX_FUNC_SDMMC_DATA3, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDMMC_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_PDM0_D, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_MIC2_DIG_EN_MASK},
                {HAL_IOMUX_FUNC_SPI_DIO, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPI2_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SMP6, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SMP_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 2 2 */ {
                {HAL_IOMUX_FUNC_SDMMC_DATA4, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDMMC_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SPILCD_CLK, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPILCD_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SPI_CLK, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPI_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_I2S_WS, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_I2S3_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 2 3 */ {
                {HAL_IOMUX_FUNC_SDMMC_DATA5, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDMMC_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SPILCD_CS, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPILCD_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SPI_CS, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPI_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_I2S_SDI0, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_I2S3_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 2 4 */ {
                {HAL_IOMUX_FUNC_SDMMC_DATA6, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDMMC_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SPILCD_DIO, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPILCD_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SPI_DIO, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPI_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_I2S_SDO, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_I2S3_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 2 5 */ {
                {HAL_IOMUX_FUNC_SDMMC_DATA7, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDMMC_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SPILCD_DCN, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPILCD_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SPI_DI, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPI_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_I2S_SCK, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_I2S3_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 2 6 */ {
                {HAL_IOMUX_FUNC_SDEMMC_DT_N, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDMMC_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_CLK_OUT, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_CLK_OUT_EN_MASK},
                {HAL_IOMUX_FUNC_SPI_DI, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPI2_GPIO4_EN_MASK},
                {HAL_IOMUX_FUNC_SMP7, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SMP_EN_MASK},
                {HAL_IOMUX_FUNC_RF_ANA_SWRX, IOMUXIP_MUX10_REG_OFFSET, IOMUXIP_MUX10_RF_ANA_SW2_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 2 7 */ {
                {HAL_IOMUX_FUNC_SDEMMC_WP, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SDMMC_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SPILCD_DI, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPILCD_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_CLK_OUT, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_CLK_OUT2_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },

    /* 3 0 */ {
                {HAL_IOMUX_FUNC_PWM0, IOMUXIP_MUX10_REG_OFFSET, IOMUXIP_MUX10_PWM_LED_EN_0_MASK},
                {HAL_IOMUX_FUNC_BT_UART_RX, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_UART2_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SPDIF0_DI, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPDIF_DIN_EN_MASK},
                {HAL_IOMUX_FUNC_UART0_RX, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_UART02_GPIO_EN_MASK},
                /* i2c is special, set i2c mode to 0 means enable i2c , 1 means gpio : other bits should be clear when used as i2c */
                {HAL_IOMUX_FUNC_I2C_SCL, IOMUXIP_MUX10_REG_OFFSET, IOMUXIP_MUX10_I2C_GPIO_MODE_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 3 1 */ {
                {HAL_IOMUX_FUNC_PWM1, IOMUXIP_MUX10_REG_OFFSET, IOMUXIP_MUX10_PWM_LED_EN_1_MASK},
                {HAL_IOMUX_FUNC_BT_UART_TX, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_UART2_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_SPDIF0_DO, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_SPDIF_DOUT_EN_MASK},
                {HAL_IOMUX_FUNC_UART0_TX, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_UART02_GPIO_EN_MASK},
                /* i2c is special, set i2c mode to 0 means enable i2c , 1 means gpio : other bits should be clear when used as i2c */
                {HAL_IOMUX_FUNC_I2C_SDA, IOMUXIP_MUX10_REG_OFFSET, IOMUXIP_MUX10_I2C_GPIO_MODE_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 3 2 */ {
                {HAL_IOMUX_FUNC_PWM2, IOMUXIP_MUX10_REG_OFFSET, IOMUXIP_MUX10_PWM_LED_EN_2_MASK},
                {HAL_IOMUX_FUNC_UART1_RX, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_UART1_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_CLK_REQ_IN, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_REQ_26M_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 3 3 */ {
                {HAL_IOMUX_FUNC_PWM3, IOMUXIP_MUX10_REG_OFFSET, IOMUXIP_MUX10_PWM_LED_EN_3_MASK},
                {HAL_IOMUX_FUNC_UART1_TX, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_UART1_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_PTA_TX_CONF, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_PTA_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 3 4 */ {
                {HAL_IOMUX_FUNC_UART0_CTS, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_UART0_FLOW_EN_MASK},
                {HAL_IOMUX_FUNC_BT_UART_CTS, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_UART2_FLOW_EN_MASK},
                {HAL_IOMUX_FUNC_PTA_STATUS, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_PTA_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 3 5 */ {
                {HAL_IOMUX_FUNC_UART0_RTS, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_UART0_FLOW_EN_MASK},
                {HAL_IOMUX_FUNC_BT_UART_RTS, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_UART2_FLOW_EN_MASK},
                {HAL_IOMUX_FUNC_PTA_RF_ACT, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_PTA_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 3 6 */ {
                {HAL_IOMUX_FUNC_UART0_RX, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_UART0_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_RF_ANA_SWTX, IOMUXIP_MUX10_REG_OFFSET, IOMUXIP_MUX10_RF_ANA_SW_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
    /* 3 7 */ {
                {HAL_IOMUX_FUNC_UART0_TX, IOMUXIP_MUX04_REG_OFFSET, IOMUXIP_MUX04_UART0_GPIO_EN_MASK},
                {HAL_IOMUX_FUNC_RF_ANA_SWRX, IOMUXIP_MUX10_REG_OFFSET, IOMUXIP_MUX10_RF_ANA_SW_EN_MASK},
                {HAL_IOMUX_FUNC_END, 0, 0},
              },
};

static uint32_t _iomux_set_function_as_gpio(enum HAL_IOMUX_PIN_T pin)
{
    uint32_t i = 0, v = 0;
    for (i = 0; i < HAL_IOMUX_MAP_NUM; ++i) {
        if (_pfr_map[pin][i].func == HAL_IOMUX_FUNC_END)
            break;

        v = iomuxip_read32(IOMUXIP_REG_BASE, _pfr_map[pin][i].reg_offset);
#if defined(I2C_IS_SPECIAL)
        if (_pfr_map[pin][i].func != HAL_IOMUX_FUNC_I2C_SCL && _pfr_map[pin][i].func != HAL_IOMUX_FUNC_I2C_SDA) {
#endif
            v &= ~(_pfr_map[pin][i].bit_mask);
#if defined(I2C_IS_SPECIAL)
        }
        else {
            v |= _pfr_map[pin][i].bit_mask;
        }
#endif
        iomuxip_write32(v, IOMUXIP_REG_BASE, _pfr_map[pin][i].reg_offset);
    }
    return 0;
}

static uint32_t _iomux_set_function_as_func(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_FUNCTION_T func)
{
    uint32_t i = 0, v = 0;
    /* i2c is special : i2c mode to 0 not 1 , 1 means gpio */
    for (i = 0; i < HAL_IOMUX_MAP_NUM; ++i) {
        if (_pfr_map[pin][i].func == HAL_IOMUX_FUNC_END)
            break;
        if (_pfr_map[pin][i].func == func) {
#if defined(I2C_IS_SPECIAL)
            if (func != HAL_IOMUX_FUNC_I2C_SCL && func != HAL_IOMUX_FUNC_I2C_SDA) {
#endif
                v = iomuxip_read32(IOMUXIP_REG_BASE, _pfr_map[pin][i].reg_offset);
                v |= _pfr_map[pin][i].bit_mask;
                iomuxip_write32(v, IOMUXIP_REG_BASE,_pfr_map[pin][i].reg_offset);
#if defined(I2C_IS_SPECIAL)
            }
            else {
                v = iomuxip_read32(IOMUXIP_REG_BASE, _pfr_map[pin][i].reg_offset);
                v &= ~(_pfr_map[pin][i].bit_mask);
                iomuxip_write32(v, IOMUXIP_REG_BASE,_pfr_map[pin][i].reg_offset);
            }
#endif
            break;
        }
    }
    return 0;
}

void hal_iomux_set_default_voltage(void)
{
    uint32_t v;

#ifdef IOMUX_DEFAULT_VOLTAGE_VMEM
    // Keep I2C/UART0 pins using VIO
    v = ~((1 << HAL_IOMUX_PIN_P3_0) | (1 << HAL_IOMUX_PIN_P3_1));
#else
    v = 0;
#endif
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX20_REG_OFFSET);
}

uint32_t hal_iomux_check(const struct HAL_IOMUX_PIN_FUNCTION_MAP *map, uint32_t count)
{
    uint32_t i = 0;
    /* check if one pin to multi function */
    /* check if one function to multi pin : have meanings for input pin */
    /* TODO */
    for (i = 0; i < count; ++i) {
    }
    return 0;
}

uint32_t hal_iomux_init(const struct HAL_IOMUX_PIN_FUNCTION_MAP *map, uint32_t count)
{
    uint32_t i = 0;
    for (i = 0; i < count; ++i) {
        hal_iomux_set_function(map[i].pin, map[i].function, HAL_IOMUX_OP_CLEAN_OTHER_FUNC_BIT);
		hal_iomux_set_io_voltage_domains(map[i].pin, map[i].volt);
		hal_iomux_set_io_pull_select(map[i].pin, map[i].pull_sel);
    }
    return 0;
}

uint32_t hal_iomux_set_function(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_FUNCTION_T func, enum HAL_IOMUX_OP_TYPE_T type)
{
    /* cancel all alt function is gpio function */
    if (func == HAL_IOMUX_FUNC_AS_GPIO || type == HAL_IOMUX_OP_CLEAN_OTHER_FUNC_BIT)
        _iomux_set_function_as_gpio(pin);

    if (func != HAL_IOMUX_FUNC_AS_GPIO)
        _iomux_set_function_as_func(pin, func);

    return 0;
}

enum HAL_IOMUX_FUNCTION_T hal_iomux_get_function(enum HAL_IOMUX_PIN_T pin)
{
    uint32_t v = 0, i = 0;
    enum HAL_IOMUX_FUNCTION_T func = HAL_IOMUX_FUNC_AS_GPIO;

    /* i2c is special : search for i2c func for this pin then read i2c mode */
    /* need _pfr_map put alt functions in priority order (see rtl) */

    for (i = 0; i < HAL_IOMUX_MAP_NUM; ++i) {
        if (_pfr_map[pin][i].func == HAL_IOMUX_FUNC_END)
            break;
        v = iomuxip_read32(IOMUXIP_REG_BASE, _pfr_map[pin][i].reg_offset);

#if defined(I2C_IS_SPECIAL)
        if (_pfr_map[pin][i].func == HAL_IOMUX_FUNC_I2C_SCL || _pfr_map[pin][i].func == HAL_IOMUX_FUNC_I2C_SDA) {
            /* v |0 == 0 means accorrding v bit is 0 */
            if ((v | ~(_pfr_map[pin][i].bit_mask)) == (~(_pfr_map[pin][i].bit_mask)))
                func = _pfr_map[pin][i].func;
        }
        else {
#endif
            /* v &1 == 1 means accorrding v bit is 1 */
            if (v & _pfr_map[pin][i].bit_mask)
                func = _pfr_map[pin][i].func;
#if defined(I2C_IS_SPECIAL)
        }
#endif
    }

    return func;
}

uint32_t hal_iomux_set_io_voltage_domains(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt)
{
    uint32_t v;

	v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX20_REG_OFFSET);

	if (volt)
		v |= (uint32_t)1 << pin;
	else
		v &= ~((uint32_t)1 << pin);
	iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX20_REG_OFFSET);

    return 0;
}

uint32_t hal_iomux_set_io_pull_select(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_PULL_SELECT_T pull_sel)
{
    uint32_t reg_08, reg_0C;

	reg_08 = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX08_REG_OFFSET);
	reg_08 &= ~((uint32_t)1 << pin);
	iomuxip_write32(reg_08, IOMUXIP_REG_BASE, IOMUXIP_MUX08_REG_OFFSET);

	reg_0C = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX0C_REG_OFFSET);
	reg_0C &= ~((uint32_t)1 << pin);
	iomuxip_write32(reg_0C, IOMUXIP_REG_BASE, IOMUXIP_MUX0C_REG_OFFSET);

    if (pin == HAL_IOMUX_PIN_P3_0 && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4) {
        uint32_t reg_10;

        reg_10 = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);
        if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENALBE) {
            reg_10 &= ~IOMUXIP_MUX10_P3_0_N_PULLUP_MASK;
        } else {
            reg_10 |= IOMUXIP_MUX10_P3_0_N_PULLUP_MASK;
        }
        iomuxip_write32(reg_10, IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);
    }

	if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENALBE){
		reg_08 |= (uint32_t)1 << pin;
		iomuxip_write32(reg_08, IOMUXIP_REG_BASE, IOMUXIP_MUX08_REG_OFFSET);
	}

	if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENALBE){
		reg_0C |= (uint32_t)1 << pin;
		iomuxip_write32(reg_0C, IOMUXIP_REG_BASE, IOMUXIP_MUX0C_REG_OFFSET);
	}

    return 0;
}

void hal_iomux_set_sdmmc_dt_n_out_group(int enable)
{
    uint32_t val = 0;
    val = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);
    if (enable)
        val |= IOMUXIP_MUX10_SDMMC_DT_N_OUT_GROUP_MASK;
    else
        val &= ~IOMUXIP_MUX10_SDMMC_DT_N_OUT_GROUP_MASK;
    iomuxip_write32(val, IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);
}

void hal_iomux_set_uart0(void)
{
    uint32_t v;

#ifdef CHIP_BEST1200

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);
    v |= IOMUXIP_MUX10_I2C_GPIO_MODE_MASK;
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX0C_REG_OFFSET);
    v &= ~0xFF;
    v |= (6 << 4) | (6 << 0);
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX0C_REG_OFFSET);

#else

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);
    v &= ~(IOMUXIP_MUX10_PWM_LED_EN_0_MASK | IOMUXIP_MUX10_PWM_LED_EN_1_MASK);
    v |= IOMUXIP_MUX10_I2C_GPIO_MODE_MASK;
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX04_REG_OFFSET);
#ifdef FPGA
    // MCU uses p3_6/7 pins so that BT can use p3_0/1 pins
    v |= IOMUXIP_MUX04_UART0_GPIO_EN_MASK;
    v &= ~IOMUXIP_MUX04_UART02_GPIO_EN_MASK;
#else
    v |= IOMUXIP_MUX04_UART02_GPIO_EN_MASK;
    v &= ~(IOMUXIP_MUX04_UART0_GPIO_EN_MASK | IOMUXIP_MUX04_UART2_GPIO_EN_MASK |
        IOMUXIP_MUX04_SPDIF_DIN_EN_MASK | IOMUXIP_MUX04_SPDIF_DOUT_EN_MASK);
#endif
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX04_REG_OFFSET);

#endif

    hal_iomux_set_io_voltage_domains(HAL_IOMUX_PIN_P3_0, HAL_IOMUX_PIN_VOLTAGE_VIO);
    hal_iomux_set_io_voltage_domains(HAL_IOMUX_PIN_P3_1, HAL_IOMUX_PIN_VOLTAGE_VIO);
    hal_iomux_set_io_pull_select(HAL_IOMUX_PIN_P3_0, HAL_IOMUX_PIN_PULLUP_ENALBE);
    hal_iomux_set_io_pull_select(HAL_IOMUX_PIN_P3_1, HAL_IOMUX_PIN_NOPULL);
}

void hal_iomux_set_uart1(void)
{
    uint32_t v;

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);
    v &= ~(IOMUXIP_MUX10_PWM_LED_EN_2_MASK | IOMUXIP_MUX10_PWM_LED_EN_3_MASK);
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX04_REG_OFFSET);
    v &= ~(IOMUXIP_MUX04_REQ_26M_EN_MASK | IOMUXIP_MUX04_PTA_EN_MASK);
    v |= IOMUXIP_MUX04_UART1_GPIO_EN_MASK;
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX04_REG_OFFSET);

    hal_iomux_set_io_voltage_domains(HAL_IOMUX_PIN_P3_2, HAL_IOMUX_PIN_VOLTAGE_VIO);
    hal_iomux_set_io_voltage_domains(HAL_IOMUX_PIN_P3_3, HAL_IOMUX_PIN_VOLTAGE_VIO);
    hal_iomux_set_io_pull_select(HAL_IOMUX_PIN_P3_2, HAL_IOMUX_PIN_PULLUP_ENALBE);
    hal_iomux_set_io_pull_select(HAL_IOMUX_PIN_P3_3, HAL_IOMUX_PIN_NOPULL);
}

void hal_iomux_set_analog_i2c(void)
{
    uint32_t v;

#ifdef CHIP_BEST1200

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);
    v &= ~IOMUXIP_MUX10_I2C_GPIO_MODE_MASK;
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX0C_REG_OFFSET);
    v &= ~0xFF;
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX0C_REG_OFFSET);

#else

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);
    v &= ~(IOMUXIP_MUX10_I2C_GPIO_MODE_MASK | IOMUXIP_MUX10_PWM_LED_EN_0_MASK |
        IOMUXIP_MUX10_PWM_LED_EN_1_MASK);
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX04_REG_OFFSET);
    v &= ~(IOMUXIP_MUX04_UART02_GPIO_EN_MASK | IOMUXIP_MUX04_UART2_GPIO_EN_MASK |
        IOMUXIP_MUX04_SPDIF_DIN_EN_MASK | IOMUXIP_MUX04_SPDIF_DOUT_EN_MASK);
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX04_REG_OFFSET);

#endif

    hal_iomux_set_io_voltage_domains(HAL_IOMUX_PIN_P3_0, HAL_IOMUX_PIN_VOLTAGE_VIO);
    hal_iomux_set_io_voltage_domains(HAL_IOMUX_PIN_P3_1, HAL_IOMUX_PIN_VOLTAGE_VIO);
    hal_iomux_set_io_pull_select(HAL_IOMUX_PIN_P3_0, HAL_IOMUX_PIN_PULLUP_ENALBE);
    hal_iomux_set_io_pull_select(HAL_IOMUX_PIN_P3_1, HAL_IOMUX_PIN_PULLUP_ENALBE);
}

void WEAK hal_iomux_set_jtag(void)
{
}

enum HAL_IOMUX_ISPI_ACCESS_T hal_iomux_ispi_access_enable(enum HAL_IOMUX_ISPI_ACCESS_T access)
{
    uint32_t v;

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX44_REG_OFFSET);
    iomuxip_write32(v | access, IOMUXIP_REG_BASE, IOMUXIP_MUX44_REG_OFFSET);
    return v;
}

enum HAL_IOMUX_ISPI_ACCESS_T hal_iomux_ispi_access_disable(enum HAL_IOMUX_ISPI_ACCESS_T access)
{
    uint32_t v;

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX44_REG_OFFSET);
    iomuxip_write32(v & ~access, IOMUXIP_REG_BASE, IOMUXIP_MUX44_REG_OFFSET);
    return v;
}

void hal_iomux_ispi_access_init(void)
{
    // Disable bt spi access ana interface
    hal_iomux_ispi_access_disable(HAL_IOMUX_ISPI_BT_ANA);
}

void hal_iomux_set_i2s(void)
{
#ifdef CHIP_BEST1000
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2s[] = {
#if (I2S_IOMUX_INDEX == 0)
        {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_I2S_WS,   I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_I2S_SDI0, I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_I2S_SDO,  I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_I2S_SCK,  I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (I2S_IOMUX_INDEX == 1)
        {HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_I2S_WS,   I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_I2S_SDI0, I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_I2S_SDO,  I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_5, HAL_IOMUX_FUNC_I2S_SCK,  I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_I2S_WS,   I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_I2S_SDI0, I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_I2S_SDO,  I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_7, HAL_IOMUX_FUNC_I2S_SCK,  I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_i2s, ARRAY_SIZE(pinmux_i2s));
#endif
}

void hal_iomux_set_spdif0(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spdif[] = {
        {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SPDIF0_DI, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SPDIF0_DO, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
    };

    hal_iomux_init(pinmux_spdif, ARRAY_SIZE(pinmux_spdif));
}

void hal_iomux_set_dig_mic(uint32_t map)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic[] = {
        {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_PDM0_CK, DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_PDM0_D,  DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };

    hal_iomux_init(pinmux_digitalmic, ARRAY_SIZE(pinmux_digitalmic));
}

void hal_iomux_set_spi(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi_3wire[3] = {
#if (SPI_IOMUX_INDEX == 0)
        {HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_SPI_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_SPI_CS,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_SPI_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_SPI_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_SPI_CS,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_SPI_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };
#ifdef SPI_IOMUX_4WIRE
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi_4wire[1] = {
#if (SPI_IOMUX_INDEX == 0)
        {HAL_IOMUX_PIN_P2_5, HAL_IOMUX_FUNC_SPI_DI, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_SPI_DI, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };
#endif

    uint32_t v;
    uint32_t mask;

#if (SPI_IOMUX_INDEX == 0)
    mask = IOMUXIP_MUX04_SPI_GPIO4_EN_MASK;
#else
    mask = IOMUXIP_MUX04_SPI2_GPIO4_EN_MASK;
#endif

    hal_iomux_init(pinmux_spi_3wire, ARRAY_SIZE(pinmux_spi_3wire));
#ifdef SPI_IOMUX_4WIRE
    hal_iomux_init(pinmux_spi_4wire, ARRAY_SIZE(pinmux_spi_4wire));
#endif

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX04_REG_OFFSET);
#ifdef SPI_IOMUX_4WIRE
    v |= mask;
#else
    v &= ~mask;
#endif
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX04_REG_OFFSET);
}


void hal_iomux_set_spilcd(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spilcd_3wire[] = {
        {HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_SPILCD_CLK, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_SPILCD_CS,  SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_4, HAL_IOMUX_FUNC_SPILCD_DIO, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_5, HAL_IOMUX_FUNC_SPILCD_DCN, SPILCD_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
#ifdef SPILCD_IOMUX_4WIRE
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spilcd_4wire[] = {
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_SPILCD_DI, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    };
#endif

    uint32_t v;

    hal_iomux_init(pinmux_spilcd_3wire, ARRAY_SIZE(pinmux_spilcd_3wire));
#ifdef SPILCD_IOMUX_4WIRE
    hal_iomux_init(pinmux_spilcd_4wire, ARRAY_SIZE(pinmux_spilcd_4wire));
#endif

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX04_REG_OFFSET);
#ifdef SPILCD_IOMUX_4WIRE
    v |= IOMUXIP_MUX04_SPILCD_GPIO4_EN_MASK;
#else
    v &= ~IOMUXIP_MUX04_SPILCD_GPIO4_EN_MASK;
#endif
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX04_REG_OFFSET);
}

void hal_iomux_set_clock_out(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_clkout[] = {
#if (CLKOUT_IOMUX_INDEX == 0)
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_clkout, ARRAY_SIZE(pinmux_clkout));
}

void hal_iomux_set_clock_12m(void)
{
    uint32_t v;

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);
    v |= IOMUXIP_MUX10_12M_EN_MASK;
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);
}

void hal_iomux_clear_clock_12m(void)
{
    uint32_t v;

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);
    v &= ~IOMUXIP_MUX10_12M_EN_MASK;
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);
}

void hal_iomux_set_bt_tport(void)
{
    uint32_t v;

    v = IOMUXIP_MUX10_ECO_I2C_EN_MASK;
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX10_REG_OFFSET);

    v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX04_REG_OFFSET);
    v |= IOMUXIP_MUX04_SMP_EN_MASK;
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX04_REG_OFFSET);

    v = 0;
    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX20_REG_OFFSET);
}

int hal_pwrkey_set_irq(enum HAL_PWRKEY_IRQ_T type)
{
    uint32_t v;

    if (type == HAL_PWRKEY_IRQ_NONE) {
        v = IOMUXIP_MUX40_PWRKEY_DOWN_INT_CLR_MASK | IOMUXIP_MUX40_PWRKEY_UP_INT_CLR_MASK;
    } else if (type == HAL_PWRKEY_IRQ_FALLING_EDGE) {
        v = IOMUXIP_MUX40_PWRKEY_DOWN_INT_EN_MASK | IOMUXIP_MUX40_PWRKEY_DOWN_INT_MASK_MASK;
    } else if (type == HAL_PWRKEY_IRQ_RISING_EDGE) {
        v = IOMUXIP_MUX40_PWRKEY_UP_INT_EN_MASK | IOMUXIP_MUX40_PWRKEY_UP_INT_MASK_MASK;
    } else if (type == HAL_PWRKEY_IRQ_BOTH_EDGE) {
        v = IOMUXIP_MUX40_PWRKEY_UP_INT_EN_MASK | IOMUXIP_MUX40_PWRKEY_UP_INT_MASK_MASK |
            IOMUXIP_MUX40_PWRKEY_DOWN_INT_EN_MASK | IOMUXIP_MUX40_PWRKEY_DOWN_INT_MASK_MASK;
    } else {
        return 1;
    }

    iomuxip_write32(v, IOMUXIP_REG_BASE, IOMUXIP_MUX40_REG_OFFSET);

    return 0;
}

bool hal_pwrkey_pressed(void)
{
    uint32_t v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX40_REG_OFFSET);
    return !!(v & IOMUXIP_MUX40_PWRKEY_VAL_MASK);
}

bool BOOT_TEXT_SRAM_LOC hal_pwrkey_startup_pressed(void)
{
    if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2)
    {
        return hal_pwrkey_pressed();
    }
    else
    {
        uint32_t v;
        struct CMU_T * const cmu = (struct CMU_T *)CMU_BASE;

        uint32_t lock = int_lock();
        uint32_t wakeup_cfg = cmu->WAKEUP_CLK_CFG;
        uint32_t sys_clk = cmu->SYS_CLK;
        cmu->WAKEUP_CLK_CFG = 0;
        hal_pwrkey_set_irq(HAL_PWRKEY_IRQ_BOTH_EDGE);
        // FLASH will be out of service, for SYS_CLK is reset
        hal_cmu_reset_pulse(HAL_CMU_MOD_O_SLEEP);
        hal_sys_timer_delay(MS_TO_TICKS(2));
        v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX40_REG_OFFSET);
        cmu->WAKEUP_CLK_CFG = 0x1000;
        hal_sys_timer_delay(MS_TO_TICKS(2));
        cmu->WAKEUP_CLK_CFG = wakeup_cfg;
        cmu->SYS_CLK = sys_clk;
        // FLASH is back to service now
        hal_sys_timer_delay(MS_TO_TICKS(2));
        int_unlock(lock);

        if (!(v & (IOMUXIP_MUX40_PWRKEY_DOWN_INT_MASK|IOMUXIP_MUX40_PWRKEY_UP_INT_MASK))){
            return false;
        }
        hal_pwrkey_set_irq(HAL_PWRKEY_IRQ_NONE);
        return true;
    }
}

enum HAL_PWRKEY_IRQ_T hal_pwrkey_get_irq_state(void)
{
    enum HAL_PWRKEY_IRQ_T state = HAL_PWRKEY_IRQ_NONE;
    uint32_t v = iomuxip_read32(IOMUXIP_REG_BASE, IOMUXIP_MUX40_REG_OFFSET);

    if (v & IOMUXIP_MUX40_PWRKEY_DOWN_INT_MASK) {
        state |= HAL_PWRKEY_IRQ_FALLING_EDGE;
    }

    if (v & IOMUXIP_MUX40_PWRKEY_UP_INT_MASK) {
        state |= HAL_PWRKEY_IRQ_RISING_EDGE;
    }

    return state;
}

