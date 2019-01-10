#include "plat_addr_map.h"
#include "reg_iomux_best2300.h"
#include "hal_iomux.h"
#include "hal_trace.h"
#include "hal_gpio.h"
#include "pmu.h"

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

#ifdef I2C0_VOLTAGE_VMEM
#define I2C0_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define I2C0_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef I2C1_VOLTAGE_VMEM
#define I2C1_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define I2C1_VOLTAGE_SEL                    HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifdef CLKOUT_VOLTAGE_VMEM
#define CLKOUT_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define CLKOUT_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifndef I2S_IOMUX_INDEX
#define I2S_IOMUX_INDEX                     0
#endif

#ifndef SPDIF0_IOMUX_INDEX
#define SPDIF0_IOMUX_INDEX                  0
#endif

#ifndef DIG_MIC2_CK_IOMUX_INDEX
#define DIG_MIC2_CK_IOMUX_INDEX             0
#endif

#ifndef DIG_MIC3_CK_IOMUX_INDEX
#define DIG_MIC3_CK_IOMUX_INDEX             0
#endif

#ifndef DIG_MIC_CK_IOMUX_INDEX
#define DIG_MIC_CK_IOMUX_INDEX              0
#endif

#ifndef DIG_MIC_D0_IOMUX_INDEX
#define DIG_MIC_D0_IOMUX_INDEX              1
#endif

#ifndef DIG_MIC_D1_IOMUX_INDEX
#define DIG_MIC_D1_IOMUX_INDEX              2
#endif

#ifndef DIG_MIC_D2_IOMUX_INDEX
#define DIG_MIC_D2_IOMUX_INDEX              3
#endif

#ifndef SPI_IOMUX_INDEX
#define SPI_IOMUX_INDEX                     0
#endif

#ifndef SPILCD_IOMUX_INDEX
#define SPILCD_IOMUX_INDEX                  0
#endif

#ifndef I2C1_IOMUX_INDEX
#define I2C1_IOMUX_INDEX                    0
#endif

#ifndef CLKOUT_IOMUX_INDEX
#define CLKOUT_IOMUX_INDEX                  0
#endif

#define IOMUX_FUNC_VAL_GPIO                 0

#define IOMUX_ALT_FUNC_NUM                  6

// Other func values: 0 -> gpio, 6 -> rf_ana, 7 -> jtag/btdm, 9 -> clk_req, 10 -> ana_test
static const uint8_t index_to_func_val[IOMUX_ALT_FUNC_NUM] = { 1, 2, 3, 4, 5, 8, };

static const enum HAL_IOMUX_FUNCTION_T pin_func_map[HAL_IOMUX_PIN_NUM][IOMUX_ALT_FUNC_NUM] = {
    // P0_0
    { HAL_IOMUX_FUNC_I2S_SDI0, HAL_IOMUX_FUNC_UART2_RX, HAL_IOMUX_FUNC_PCM_DI, HAL_IOMUX_FUNC_SPILCD_DI0,
      HAL_IOMUX_FUNC_PDM0_CK, HAL_IOMUX_FUNC_SPILCD_DCN, },
    // P0_1
    { HAL_IOMUX_FUNC_I2S_SDO, HAL_IOMUX_FUNC_UART2_TX, HAL_IOMUX_FUNC_PCM_DO, HAL_IOMUX_FUNC_SPILCD_DIO,
      HAL_IOMUX_FUNC_PDM0_D, HAL_IOMUX_FUNC_NONE, },
    // P0_2
    { HAL_IOMUX_FUNC_I2S_WS, HAL_IOMUX_FUNC_I2C_M1_SCL, HAL_IOMUX_FUNC_PCM_FSYNC, HAL_IOMUX_FUNC_SPILCD_CS0,
      HAL_IOMUX_FUNC_PDM1_D, HAL_IOMUX_FUNC_NONE, },
    // P0_3
    { HAL_IOMUX_FUNC_I2S_SCK, HAL_IOMUX_FUNC_I2C_M1_SDA, HAL_IOMUX_FUNC_PCM_CLK, HAL_IOMUX_FUNC_SPILCD_CLK,
      HAL_IOMUX_FUNC_PDM2_D, HAL_IOMUX_FUNC_NONE, },
    // P0_4
    { HAL_IOMUX_FUNC_SDMMC_DATA7, HAL_IOMUX_FUNC_SPI_DI0, HAL_IOMUX_FUNC_I2S_MCLK, HAL_IOMUX_FUNC_CLK_OUT,
      HAL_IOMUX_FUNC_PDM1_CK, HAL_IOMUX_FUNC_SPI_DCN, },
    // P0_5
    { HAL_IOMUX_FUNC_SDMMC_DATA6, HAL_IOMUX_FUNC_SPI_CLK, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_PDM1_D, HAL_IOMUX_FUNC_NONE, },
    // P0_6
    { HAL_IOMUX_FUNC_SDMMC_DATA5, HAL_IOMUX_FUNC_SPI_CS0, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_PDM0_D, HAL_IOMUX_FUNC_NONE, },
    // P0_7
    { HAL_IOMUX_FUNC_SDMMC_DATA4, HAL_IOMUX_FUNC_SPI_DIO, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_PDM2_D, HAL_IOMUX_FUNC_NONE, },
    // P1_0
    { HAL_IOMUX_FUNC_SDMMC_DATA2, HAL_IOMUX_FUNC_I2S_SCK, HAL_IOMUX_FUNC_SPILCD_CLK, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, },
    // P1_1
    { HAL_IOMUX_FUNC_SDMMC_DATA3, HAL_IOMUX_FUNC_I2S_WS, HAL_IOMUX_FUNC_SPILCD_CS0, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, },
    // P1_2
    { HAL_IOMUX_FUNC_SDMMC_CMD, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SPILCD_CS1, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, },
    // P1_3
    { HAL_IOMUX_FUNC_SDMMC_CLK, HAL_IOMUX_FUNC_I2S_MCLK, HAL_IOMUX_FUNC_SPILCD_DCN, HAL_IOMUX_FUNC_CLK_OUT,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, },
    // P1_4
    { HAL_IOMUX_FUNC_SDMMC_DATA0, HAL_IOMUX_FUNC_I2S_SDI0, HAL_IOMUX_FUNC_SPILCD_DI0, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, },
    // P1_5
    { HAL_IOMUX_FUNC_SDMMC_DATA1, HAL_IOMUX_FUNC_I2S_SDO, HAL_IOMUX_FUNC_SPILCD_DIO, HAL_IOMUX_FUNC_I2S_MCLK,
      HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_NONE, },
    // P1_6
    { HAL_IOMUX_FUNC_UART0_RX, HAL_IOMUX_FUNC_I2C_SCL, HAL_IOMUX_FUNC_BT_UART_RX, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, },
    // P1_7
    { HAL_IOMUX_FUNC_UART0_TX, HAL_IOMUX_FUNC_I2C_SDA, HAL_IOMUX_FUNC_BT_UART_TX, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, },
    // P2_0
    { HAL_IOMUX_FUNC_UART1_RX, HAL_IOMUX_FUNC_I2C_M0_SCL, HAL_IOMUX_FUNC_BT_UART_RX, HAL_IOMUX_FUNC_SPDIF0_DI,
      HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_FUNC_I2S_MCLK, },
    // P2_1
    { HAL_IOMUX_FUNC_UART1_TX, HAL_IOMUX_FUNC_I2C_M0_SDA, HAL_IOMUX_FUNC_BT_UART_TX, HAL_IOMUX_FUNC_SPDIF0_DO,
      HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_FUNC_CLK_OUT, },
    // P2_2
    { HAL_IOMUX_FUNC_I2C_M1_SCL, HAL_IOMUX_FUNC_UART2_RX, HAL_IOMUX_FUNC_UART1_CTS, HAL_IOMUX_FUNC_BT_UART_CTS,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_I2S_MCLK, },
    // P2_3
    { HAL_IOMUX_FUNC_I2C_M1_SDA, HAL_IOMUX_FUNC_UART2_TX, HAL_IOMUX_FUNC_UART1_RTS, HAL_IOMUX_FUNC_BT_UART_RTS,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_CLK_OUT, },
    // P2_4
    { HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_FUNC_CLK_REQ_OUT, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, },
    // P2_5
    { HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_FUNC_CLK_REQ_IN, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, },
    // P2_6
    { HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_FUNC_SPILCD_DI1, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SPDIF0_DI,
      HAL_IOMUX_FUNC_CLK_32K_IN, HAL_IOMUX_FUNC_NONE, },
    // P2_7
    { HAL_IOMUX_FUNC_PWM3, HAL_IOMUX_FUNC_SPILCD_CS1, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SPDIF0_DO,
      HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_NONE, },
    // P3_0
    { HAL_IOMUX_FUNC_SPILCD_DI2, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_PDM0_D, HAL_IOMUX_FUNC_NONE, },
    // P3_1
    { HAL_IOMUX_FUNC_SPILCD_CS2, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_PDM1_D, HAL_IOMUX_FUNC_NONE, },
    // P3_2
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_PDM2_D, HAL_IOMUX_FUNC_NONE, },
    // P3_3
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_PDM2_CK, HAL_IOMUX_FUNC_NONE, },
    // P3_4
    { HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_FUNC_SPI_DI1, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SPILCD_DI0,
      HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_SPILCD_DCN, },
    // P3_5
    { HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_FUNC_SPI_CS1, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SPILCD_DIO,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, },
    // P3_6
    { HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_FUNC_SPI_DI2, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SPILCD_CS0,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, },
    // P3_7
    { HAL_IOMUX_FUNC_PWM3, HAL_IOMUX_FUNC_SPI_CS2, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SPILCD_CLK,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, },
};

static struct IOMUX_T * const iomux = (struct IOMUX_T *)IOMUX_BASE;

#ifdef ANC_PROD_TEST
#define OPT_TYPE
#else
#define OPT_TYPE                        const
#endif

static OPT_TYPE uint8_t digmic_ck_index = DIG_MIC_CK_IOMUX_INDEX;

static OPT_TYPE uint8_t digmic_d0_index = DIG_MIC_D0_IOMUX_INDEX;
static OPT_TYPE uint8_t digmic_d1_index = DIG_MIC_D1_IOMUX_INDEX;
static OPT_TYPE uint8_t digmic_d2_index = DIG_MIC_D2_IOMUX_INDEX;

void hal_iomux_set_default_voltage(void)
{
}

uint32_t hal_iomux_check(const struct HAL_IOMUX_PIN_FUNCTION_MAP *map, uint32_t count)
{
    uint32_t i = 0;
    for (i = 0; i < count; ++i) {
    }
    return 0;
}

uint32_t hal_iomux_init(const struct HAL_IOMUX_PIN_FUNCTION_MAP *map, uint32_t count)
{
    uint32_t i;
    uint32_t ret;

    for (i = 0; i < count; ++i) {
        ret = hal_iomux_set_function(map[i].pin, map[i].function, HAL_IOMUX_OP_CLEAN_OTHER_FUNC_BIT);
        if (ret) {
            return (i << 8) + 1;
        }
		ret = hal_iomux_set_io_voltage_domains(map[i].pin, map[i].volt);
        if (ret) {
            return (i << 8) + 2;
        }
		ret = hal_iomux_set_io_pull_select(map[i].pin, map[i].pull_sel);
        if (ret) {
            return (i << 8) + 3;
        }
    }

    return 0;
}

uint32_t hal_iomux_set_function(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_FUNCTION_T func, enum HAL_IOMUX_OP_TYPE_T type)
{
    int i;
    uint8_t val;
    __IO uint32_t *reg;
    uint32_t shift;

    if (pin >= HAL_IOMUX_PIN_NUM) {
        return 1;
    }
    if (func >= HAL_IOMUX_FUNC_END) {
        return 2;
    }

    if (pin == HAL_IOMUX_PIN_P1_6 || pin == HAL_IOMUX_PIN_P1_7) {
        if (func ==  HAL_IOMUX_FUNC_I2C_SCL || func == HAL_IOMUX_FUNC_I2C_SDA) {
            // Enable analog I2C slave
            iomux->REG_050 &= ~IOMUX_GPIO_I2C_MODE;
            // Set mcu GPIO func
            iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P16_SEL_MASK | IOMUX_GPIO_P17_SEL_MASK)) |
                IOMUX_GPIO_P16_SEL(IOMUX_FUNC_VAL_GPIO) | IOMUX_GPIO_P17_SEL(IOMUX_FUNC_VAL_GPIO);
            return 0;
        } else {
            iomux->REG_050 |= IOMUX_GPIO_I2C_MODE;
            // Continue to set the alt func
        }
    } else if (pin == HAL_IOMUX_PIN_P0_2) {
        if (func ==  HAL_IOMUX_FUNC_SPDIF0_DI) {
            iomux->REG_004 = SET_BITFIELD(iomux->REG_004, IOMUX_GPIO_P02_SEL, 6);
            return 0;
        }
    } else if (pin == HAL_IOMUX_PIN_P0_3) {
        if (func ==  HAL_IOMUX_FUNC_SPDIF0_DO) {
            iomux->REG_004 = SET_BITFIELD(iomux->REG_004, IOMUX_GPIO_P03_SEL, 6);
            return 0;
        }
    } else if (pin == HAL_IOMUX_PIN_P2_0) {
        if (func ==  HAL_IOMUX_FUNC_CLK_REQ_OUT) {
            iomux->REG_00C = SET_BITFIELD(iomux->REG_00C, IOMUX_GPIO_P20_SEL, 9);
            return 0;
        }
    } else if (pin == HAL_IOMUX_PIN_P2_1) {
        if (func == HAL_IOMUX_FUNC_CLK_REQ_IN) {
            iomux->REG_00C = SET_BITFIELD(iomux->REG_00C, IOMUX_GPIO_P21_SEL, 9);
            return 0;
        }
    } else if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        ASSERT(func == HAL_IOMUX_FUNC_GPIO, "Bad func=%d for IOMUX pin=%d", func, pin);
        return 0;
    }

    if (func == HAL_IOMUX_FUNC_GPIO) {
        val = IOMUX_FUNC_VAL_GPIO;
    } else {
        for (i = 0; i < IOMUX_ALT_FUNC_NUM; i++) {
            if (pin_func_map[pin][i] == func) {
                break;
            }
        }

        if (i == IOMUX_ALT_FUNC_NUM) {
            return 3;
        }
        val = index_to_func_val[i];
    }

    reg = &iomux->REG_004 + pin / 8;
    shift = (pin % 8) * 4;

    *reg = (*reg & ~(0xF << shift)) | (val << shift);

    return 0;
}

enum HAL_IOMUX_FUNCTION_T hal_iomux_get_function(enum HAL_IOMUX_PIN_T pin)
{
    return HAL_IOMUX_FUNC_NONE;
}

uint32_t hal_iomux_set_io_voltage_domains(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt)
{
    return 0;
}

uint32_t hal_iomux_set_io_pull_select(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_PULL_SELECT_T pull_sel)
{
    if (pin >= HAL_IOMUX_PIN_NUM) {
        return 1;
    }

    if (pin < HAL_IOMUX_PIN_LED1) {
        iomux->REG_02C &= ~(1 << pin);
        iomux->REG_030 &= ~(1 << pin);
        if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENALBE) {
            iomux->REG_02C |= (1 << pin);
        } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENALBE) {
            iomux->REG_030 |= (1 << pin);
        }
    } else if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        pmu_led_set_pull_select(pin, pull_sel);
    }

    return 0;
}

void hal_iomux_set_sdmmc_dt_n_out_group(int enable)
{
}

void hal_iomux_set_uart0(void)
{
    uint32_t mask;

    // Disable analog I2C slave & master
    iomux->REG_050 |= IOMUX_GPIO_I2C_MODE | IOMUX_I2C0_M_SEL_GPIO;
    // Set uart0 func
    iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P16_SEL_MASK | IOMUX_GPIO_P17_SEL_MASK)) |
        IOMUX_GPIO_P16_SEL(1) | IOMUX_GPIO_P17_SEL(1);

    mask = (1 << HAL_IOMUX_PIN_P1_6) | (1 << HAL_IOMUX_PIN_P1_7);
    // Set vio voltage
    iomux->REG_070 |= IOMUX_GPIO_P1_PWS(2);
    // Setup pullup
    iomux->REG_02C |= (1 << HAL_IOMUX_PIN_P1_6);
    iomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P1_7);
    // Clear pulldown
    iomux->REG_030 &= ~mask;
}

void hal_iomux_set_uart1(void)
{
    uint32_t mask;

    // Set uart1 func
    iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P20_SEL_MASK | IOMUX_GPIO_P21_SEL_MASK)) |
        IOMUX_GPIO_P20_SEL(1) | IOMUX_GPIO_P21_SEL(1);

    mask = (1 << HAL_IOMUX_PIN_P2_0) | (1 << HAL_IOMUX_PIN_P2_1);
    // Setup pullup
    iomux->REG_02C |= (1 << HAL_IOMUX_PIN_P2_0);
    iomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P2_1);
    // Clear pulldown
    iomux->REG_030 &= ~mask;
}

void hal_iomux_set_analog_i2c(void)
{
    uint32_t mask;

    // Disable analog I2C master
    iomux->REG_050 |= IOMUX_GPIO_I2C_MODE | IOMUX_I2C0_M_SEL_GPIO;
    // Set mcu GPIO func
    iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P16_SEL_MASK | IOMUX_GPIO_P17_SEL_MASK)) |
        IOMUX_GPIO_P16_SEL(0) | IOMUX_GPIO_P17_SEL(0);
    // Enable analog I2C slave
    iomux->REG_050 &= ~IOMUX_GPIO_I2C_MODE;

    mask = (1 << HAL_IOMUX_PIN_P1_6) | (1 << HAL_IOMUX_PIN_P1_7);
    // Set vio voltage
    iomux->REG_070 |= IOMUX_GPIO_P1_PWS(2);
    // Setup pullup
    iomux->REG_02C |= mask;
    // Clear pulldown
    iomux->REG_030 &= ~mask;
}

void hal_iomux_set_jtag(void)
{
    uint32_t mask;
    uint32_t val;

    // Workaroud: P00_SEL also affects the functions of P01~P03

    // SWCLK/TCK, SWDIO/TMS
    mask = IOMUX_GPIO_P00_SEL_MASK | IOMUX_GPIO_P01_SEL_MASK | IOMUX_GPIO_P02_SEL_MASK;
    val = IOMUX_GPIO_P00_SEL(7) | IOMUX_GPIO_P01_SEL(7) | IOMUX_GPIO_P02_SEL(7);
    // TDI, TDO
#ifdef JTAG_TDI_TDO_PIN
    mask |= IOMUX_GPIO_P00_SEL_MASK | IOMUX_GPIO_P03_SEL_MASK;
    val |= IOMUX_GPIO_P00_SEL(7) | IOMUX_GPIO_P03_SEL(7);
#endif
    iomux->REG_004 = (iomux->REG_004 & ~mask) | val;

    // RESET
#ifdef JTAG_RESET_PIN
    iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_GPIO_P20_SEL_MASK)) | IOMUX_GPIO_P20_SEL(7);
#endif

    mask = (1 << HAL_IOMUX_PIN_P0_1) | (1 << HAL_IOMUX_PIN_P0_2);
#ifdef JTAG_TDI_TDO_PIN
    mask |= (1 << HAL_IOMUX_PIN_P0_0) | (1 << HAL_IOMUX_PIN_P0_3);
#endif
#ifdef JTAG_RESET_PIN
    mask |= (1 << HAL_IOMUX_PIN_P2_0);
#endif
    // Clear pullup
    iomux->REG_02C &= ~mask;
    // Clear pulldown
    iomux->REG_030 &= ~mask;
}

enum HAL_IOMUX_ISPI_ACCESS_T hal_iomux_ispi_access_enable(enum HAL_IOMUX_ISPI_ACCESS_T access)
{
    uint32_t v;

    v = iomux->REG_044;
    iomux->REG_044 |= access;

    return v;
}

enum HAL_IOMUX_ISPI_ACCESS_T hal_iomux_ispi_access_disable(enum HAL_IOMUX_ISPI_ACCESS_T access)
{
    uint32_t v;

    v = iomux->REG_044;
    iomux->REG_044 &= ~access;

    return v;
}

void hal_iomux_ispi_access_init(void)
{
    // Disable bt spi access ana interface
    hal_iomux_ispi_access_disable(HAL_IOMUX_ISPI_BT_ANA);
}

void hal_iomux_set_i2s(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2s[] = {
#if (I2S_IOMUX_INDEX == 14)
        {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_I2S_SDI0, I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_I2S_SDO,  I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_I2S_WS,   I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_I2S_SCK,  I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_I2S_SDI0, I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_I2S_SDO,  I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_I2S_WS,   I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_I2S_SCK,  I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_i2s, ARRAY_SIZE(pinmux_i2s));
}

void hal_iomux_clear_i2s(void)
{
}

void hal_iomux_set_spdif0(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spdif[] = {
#if (SPDIF0_IOMUX_INDEX == 20)
        {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_SPDIF0_DI, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_SPDIF0_DO, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPDIF0_IOMUX_INDEX == 26)
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_SPDIF0_DI, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_SPDIF0_DO, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_SPDIF0_DI, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_SPDIF0_DO, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_spdif, ARRAY_SIZE(pinmux_spdif));
}

void hal_iomux_set_spdif1(void)
{
}

void hal_iomux_set_dig_mic(uint32_t map)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic_clk[] = {
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_PDM0_CK, DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic0[] = {
        {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_PDM0_D,  DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic1[] = {
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_PDM1_D,  DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic2[] = {
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_PDM2_D,  DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };

    if (digmic_ck_index == 0) {
        pinmux_digitalmic_clk[0].pin = HAL_IOMUX_PIN_P0_0;
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_PDM0_CK;
    } else if (digmic_ck_index == 4) {
        pinmux_digitalmic_clk[0].pin = HAL_IOMUX_PIN_P0_4;
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_PDM1_CK;
    } else if (digmic_ck_index == 33) {
        pinmux_digitalmic_clk[0].pin = HAL_IOMUX_PIN_P3_3;
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_PDM2_CK;
    }

    if (digmic_d0_index == 1) {
        pinmux_digitalmic0[0].pin = HAL_IOMUX_PIN_P0_1;
    } else if (digmic_d0_index == 6) {
        pinmux_digitalmic0[0].pin = HAL_IOMUX_PIN_P0_6;
    } else if (digmic_d0_index == 30) {
        pinmux_digitalmic0[0].pin = HAL_IOMUX_PIN_P3_0;
    }
    
    if (digmic_d1_index == 2) {
        pinmux_digitalmic1[0].pin = HAL_IOMUX_PIN_P0_2;
    } else if (digmic_d1_index == 5) {
        pinmux_digitalmic1[0].pin = HAL_IOMUX_PIN_P0_5;
    } else if (digmic_d1_index == 31) {
        pinmux_digitalmic1[0].pin = HAL_IOMUX_PIN_P3_1;
    }

    if (digmic_d2_index == 3) {
        pinmux_digitalmic2[0].pin = HAL_IOMUX_PIN_P0_3;
    } else if (digmic_d2_index == 7) {
        pinmux_digitalmic2[0].pin = HAL_IOMUX_PIN_P0_7;
    } else if (digmic_d2_index == 32) {
        pinmux_digitalmic2[0].pin = HAL_IOMUX_PIN_P3_2;
    }

    if (map & 0xF) {
        hal_iomux_init(pinmux_digitalmic_clk, ARRAY_SIZE(pinmux_digitalmic_clk));
    }
    if (map & (1 << 0)) {
        hal_iomux_init(pinmux_digitalmic0, ARRAY_SIZE(pinmux_digitalmic0));
    }
    if (map & (1 << 1)) {
        hal_iomux_init(pinmux_digitalmic1, ARRAY_SIZE(pinmux_digitalmic1));
    }
    if (map & (1 << 2)) {
        hal_iomux_init(pinmux_digitalmic2, ARRAY_SIZE(pinmux_digitalmic2));
    }

}

void hal_iomux_set_spi(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi_3wire[3] = {
        {HAL_IOMUX_PIN_P0_5, HAL_IOMUX_FUNC_SPI_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_6, HAL_IOMUX_FUNC_SPI_CS0,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_SPI_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
#ifdef SPI_IOMUX_4WIRE
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi_4wire[1] = {
        {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_SPI_DI0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
#endif

    hal_iomux_init(pinmux_spi_3wire, ARRAY_SIZE(pinmux_spi_3wire));
#ifdef SPI_IOMUX_4WIRE
    hal_iomux_init(pinmux_spi_4wire, ARRAY_SIZE(pinmux_spi_4wire));
#endif
}

void hal_iomux_clear_spi(void)
{
}

void hal_iomux_set_spilcd(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spilcd_3wire[] = {
#if (SPILCD_IOMUX_INDEX == 10)
        {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SPILCD_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SPILCD_CS0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_SPILCD_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPILCD_IOMUX_INDEX == 35)
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_SPILCD_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_SPILCD_CS0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_SPILCD_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_SPILCD_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_SPILCD_CS0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_SPILCD_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };
#ifdef SPILCD_IOMUX_4WIRE
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spilcd_4wire[] = {
#if (SPILCD_IOMUX_INDEX == 14)
        {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_SPILCD_DI0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (SPILCD_IOMUX_INDEX == 34)
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_SPILCD_DI0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_SPILCD_DI0, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };
#endif

    hal_iomux_init(pinmux_spilcd_3wire, ARRAY_SIZE(pinmux_spilcd_3wire));
#ifdef SPILCD_IOMUX_4WIRE
    hal_iomux_init(pinmux_spilcd_4wire, ARRAY_SIZE(pinmux_spilcd_4wire));
#endif
}

void hal_iomux_set_i2c0(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2c[] = {
        {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_I2C_M0_SCL, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_I2C_M0_SDA, I2C0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
    };

    hal_iomux_init(pinmux_i2c, ARRAY_SIZE(pinmux_i2c));
    iomux->REG_050 |= IOMUX_I2C0_M_SEL_GPIO;
}

void hal_iomux_set_i2c1(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2c[] = {
#if (I2C1_IOMUX_INDEX == 22)
        {HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_I2C_M1_SCL, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_I2C_M1_SDA, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
#else
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_I2C_M1_SCL, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_I2C_M1_SDA, I2C1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
#endif
    };

    hal_iomux_init(pinmux_i2c, ARRAY_SIZE(pinmux_i2c));
    iomux->REG_050 |= IOMUX_I2C1_M_SEL_GPIO;
}

void hal_iomux_set_clock_out(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_clkout[] = {
#if (CLKOUT_IOMUX_INDEX == 13)
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 15)
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 21)
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 23)
        {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 27)
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLKOUT_IOMUX_INDEX == 34)
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_clkout, ARRAY_SIZE(pinmux_clkout));
}

void hal_iomux_set_mcu_clock_out(void)
{
}

void hal_iomux_set_bt_clock_out(void)
{
}

void hal_iomux_set_clock_12m(void)
{
}

void hal_iomux_clear_clock_12m(void)
{
}

void hal_iomux_set_bt_tport(void)
{
    // P1_0 ~ P1_5
    iomux->REG_008 = (iomux->REG_008 & ~(IOMUX_GPIO_P10_SEL_MASK | IOMUX_GPIO_P11_SEL_MASK | IOMUX_GPIO_P12_SEL_MASK |
        IOMUX_GPIO_P13_SEL_MASK | IOMUX_GPIO_P14_SEL_MASK | IOMUX_GPIO_P15_SEL_MASK)) |
        IOMUX_GPIO_P10_SEL(0xA) | IOMUX_GPIO_P11_SEL(0xA) | IOMUX_GPIO_P12_SEL(0xA) | IOMUX_GPIO_P13_SEL(0xA) |
        IOMUX_GPIO_P14_SEL(0xA) | IOMUX_GPIO_P15_SEL(0xA);
    // ANA TEST DIR
    iomux->REG_014 = 0x3F00;
    // ANA TEST SEL
    iomux->REG_018 = IOMUX_ANA_TEST_SEL(5);
}

int WEAK hal_pwrkey_set_irq(enum HAL_PWRKEY_IRQ_T type)
{
    return 0;
}

bool WEAK hal_pwrkey_pressed(void)
{
    return 0;
}

bool hal_pwrkey_startup_pressed(void)
{
    return hal_pwrkey_pressed();
}

enum HAL_PWRKEY_IRQ_T WEAK hal_pwrkey_get_irq_state(void)
{
    enum HAL_PWRKEY_IRQ_T state = HAL_PWRKEY_IRQ_NONE;

    return state;
}

