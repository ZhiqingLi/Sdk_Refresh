#include "plat_addr_map.h"
#include "reg_iomux_best2000.h"
#include "hal_iomux.h"
#include "hal_trace.h"
#include "hal_gpio.h"
#include "pmu.h"
#include "tgt_hardware.h"

#ifdef __KNOWLES
#define I2S_VOLTAGE_VMEM
#define DIGMIC_VOLTAGE_VMEM
#endif

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

#ifdef SPDIF1_VOLTAGE_VMEM
#define SPDIF1_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define SPDIF1_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
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

#ifdef CLK12M_VOLTAGE_VMEM
#define CLK12M_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_MEM
#else
#define CLK12M_VOLTAGE_SEL                  HAL_IOMUX_PIN_VOLTAGE_VIO
#endif

#ifndef I2S_IOMUX_INDEX
#define I2S_IOMUX_INDEX                     0
#endif

#ifndef SPDIF1_DI_IOMUX_INDEX
#define SPDIF1_DI_IOMUX_INDEX               0
#endif

#ifndef SPDIF1_DO_IOMUX_INDEX
#define SPDIF1_DO_IOMUX_INDEX               0
#endif

#ifndef DIG_MIC_CK_IOMUX_INDEX
#define DIG_MIC_CK_IOMUX_INDEX              0
#endif

#ifndef SPI_IOMUX_INDEX
#define SPI_IOMUX_INDEX                     0
#endif

#ifndef CLKOUT_IOMUX_INDEX
#define CLKOUT_IOMUX_INDEX                  0
#endif

#ifndef MCU_CLKOUT_IOMUX_INDEX
#define MCU_CLKOUT_IOMUX_INDEX              0
#endif

#ifndef BT_CLKOUT_IOMUX_INDEX
#define BT_CLKOUT_IOMUX_INDEX               0
#endif

#ifndef CLK12M_IOMUX_INDEX
#define CLK12M_IOMUX_INDEX                  0
#endif

#define IOMUX_FUNC_VAL_SMP                  1
#define IOMUX_FUNC_VAL_JTAG                 2
#define IOMUX_FUNC_VAL_AO_GPIO              7
#define IOMUX_FUNC_VAL_GPIO                 15

#define IOMUX_ALT_FUNC_NUM                  7

// Other func values: 1 -> smp, 2 -> jtag, 7 -> ao_gpio, 15 -> gpio
static const uint8_t index_to_func_val[IOMUX_ALT_FUNC_NUM] = { 0, 3, 4, 5, 6, 8, 9, };

static const enum HAL_IOMUX_FUNCTION_T pin_func_map[HAL_IOMUX_PIN_NUM][IOMUX_ALT_FUNC_NUM] = {
    // P0_0
    { HAL_IOMUX_FUNC_BT_SYMTX2, HAL_IOMUX_FUNC_SDMMC_DATA0, HAL_IOMUX_FUNC_CAM_D0, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_WF_WAKE_HOST, HAL_IOMUX_FUNC_AO_PWM0, HAL_IOMUX_FUNC_CLK_12M, },
    // P0_1
    { HAL_IOMUX_FUNC_BT_SYMTX1, HAL_IOMUX_FUNC_SDMMC_DATA1, HAL_IOMUX_FUNC_CAM_D1, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_AO_PWM1, HAL_IOMUX_FUNC_WF_TXON, },
    // P0_2
    { HAL_IOMUX_FUNC_BT_SYMTX0, HAL_IOMUX_FUNC_SDMMC_DATA2, HAL_IOMUX_FUNC_CAM_D2, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_AO_PWM2, HAL_IOMUX_FUNC_NONE, },
    // P0_3
    { HAL_IOMUX_FUNC_BT_TXFLAG, HAL_IOMUX_FUNC_SDMMC_DATA3, HAL_IOMUX_FUNC_CAM_D3, HAL_IOMUX_FUNC_WF_WAKE1,
      HAL_IOMUX_FUNC_SPDIF1_DI, HAL_IOMUX_FUNC_AO_PWM3, HAL_IOMUX_FUNC_CLK_12M, },
    // P0_4
    { HAL_IOMUX_FUNC_BT_SYMRX2, HAL_IOMUX_FUNC_SDMMC_DATA4, HAL_IOMUX_FUNC_CAM_D4, HAL_IOMUX_FUNC_SPILCD_DCN,
      HAL_IOMUX_FUNC_WF_UART_TX, HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_FUNC_NONE, },
    // P0_5
    { HAL_IOMUX_FUNC_BT_SYMRX1, HAL_IOMUX_FUNC_SDMMC_DATA5, HAL_IOMUX_FUNC_CAM_D5, HAL_IOMUX_FUNC_SPILCD_CLK,
      HAL_IOMUX_FUNC_WF_UART_RTS, HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_FUNC_WF_TXON, },
    // P0_6
    { HAL_IOMUX_FUNC_BT_SYMRX0, HAL_IOMUX_FUNC_SDMMC_DATA6, HAL_IOMUX_FUNC_CAM_D6, HAL_IOMUX_FUNC_SPILCD_CS,
      HAL_IOMUX_FUNC_WF_UART_RX, HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_FUNC_NONE, },
    // P0_7
    { HAL_IOMUX_FUNC_BT_RXFLAG, HAL_IOMUX_FUNC_SDMMC_DATA7, HAL_IOMUX_FUNC_CAM_D7, HAL_IOMUX_FUNC_SPILCD_DIO,
      HAL_IOMUX_FUNC_WF_UART_CTS, HAL_IOMUX_FUNC_PWM3, HAL_IOMUX_FUNC_CLK_12M, },
    // P1_0
    { HAL_IOMUX_FUNC_BT_RATE0, HAL_IOMUX_FUNC_SDMMC_CLK, HAL_IOMUX_FUNC_CAM_MCLK, HAL_IOMUX_FUNC_WF_WAKE2,
      HAL_IOMUX_FUNC_SPI_DCN, HAL_IOMUX_FUNC_AO_PWM0, HAL_IOMUX_FUNC_WF_TXON, },
    // P1_1
    { HAL_IOMUX_FUNC_BT_RATE1, HAL_IOMUX_FUNC_SDMMC_CMD, HAL_IOMUX_FUNC_CAM_PCLK, HAL_IOMUX_FUNC_BT_CLK_OUT,
      HAL_IOMUX_FUNC_SPI_CLK, HAL_IOMUX_FUNC_AO_PWM1, HAL_IOMUX_FUNC_NONE, },
    // P1_2
    { HAL_IOMUX_FUNC_BT_SYNC_P, HAL_IOMUX_FUNC_UART2_RX, HAL_IOMUX_FUNC_UART0_CTS, HAL_IOMUX_FUNC_CAM_HREF,
      HAL_IOMUX_FUNC_SPI_CS, HAL_IOMUX_FUNC_AO_PWM2, HAL_IOMUX_FUNC_CLK_12M, },
    // P1_3
    { HAL_IOMUX_FUNC_BT_TXON, HAL_IOMUX_FUNC_UART2_TX, HAL_IOMUX_FUNC_UART2_CTS, HAL_IOMUX_FUNC_CAM_VSYNC,
      HAL_IOMUX_FUNC_SPI_DIO, HAL_IOMUX_FUNC_AO_PWM3, HAL_IOMUX_FUNC_NONE, },
    // P1_4
    { HAL_IOMUX_FUNC_BT_RXON, HAL_IOMUX_FUNC_WF_SDIO_CLK, HAL_IOMUX_FUNC_SPI_DCN, HAL_IOMUX_FUNC_SDIO_CLK,
      HAL_IOMUX_FUNC_CAM_MCLK, HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_FUNC_CLK_12M, },
    // P1_5
    { HAL_IOMUX_FUNC_BT_SPI_SENB, HAL_IOMUX_FUNC_WF_SDIO_CMD, HAL_IOMUX_FUNC_SPI_CLK, HAL_IOMUX_FUNC_SDIO_CMD,
      HAL_IOMUX_FUNC_CAM_PCLK, HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_FUNC_NONE, },
    // P1_6
    { HAL_IOMUX_FUNC_BT_SPI_CLK, HAL_IOMUX_FUNC_WF_SDIO_DATA0, HAL_IOMUX_FUNC_SPI_CS, HAL_IOMUX_FUNC_SDIO_DATA0,
      HAL_IOMUX_FUNC_CAM_D0, HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_FUNC_CLK_12M, },
    // P1_7
    { HAL_IOMUX_FUNC_BT_SPI_DO, HAL_IOMUX_FUNC_WF_SDIO_DATA1, HAL_IOMUX_FUNC_SPI_DIO, HAL_IOMUX_FUNC_SDIO_DATA1,
      HAL_IOMUX_FUNC_CAM_D1, HAL_IOMUX_FUNC_PWM3, HAL_IOMUX_FUNC_NONE, },
    // P2_0
    { HAL_IOMUX_FUNC_BT_SPI_DI, HAL_IOMUX_FUNC_WF_SDIO_DATA2, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SDIO_DATA2,
      HAL_IOMUX_FUNC_CAM_D2, HAL_IOMUX_FUNC_AO_PWM0, HAL_IOMUX_FUNC_WF_FEM_CNTL0, },
    // P2_1
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_WF_SDIO_DATA3, HAL_IOMUX_FUNC_MCU_CLK_OUT, HAL_IOMUX_FUNC_SDIO_DATA3,
      HAL_IOMUX_FUNC_CAM_D3, HAL_IOMUX_FUNC_AO_PWM1, HAL_IOMUX_FUNC_WF_FEM_CNTL1, },
    // P2_2
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_WF_WAKE_HOST, HAL_IOMUX_FUNC_PDM2_CK, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_CAM_D4, HAL_IOMUX_FUNC_AO_PWM2, HAL_IOMUX_FUNC_WF_FEM_CNTL2, },
    // P2_3
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_WF_WAKE1, HAL_IOMUX_FUNC_PDM3_CK, HAL_IOMUX_FUNC_MCU_CLK_OUT,
      HAL_IOMUX_FUNC_CAM_D5, HAL_IOMUX_FUNC_AO_PWM3, HAL_IOMUX_FUNC_WF_FEM_CNTL3, },
    // P2_4
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_UART1_CTS, HAL_IOMUX_FUNC_UART2_RX, HAL_IOMUX_FUNC_BT_UART_CTS,
      HAL_IOMUX_FUNC_CAM_D6, HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_FUNC_WF_FEM_CNTL4, },
    // P2_5
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_UART1_RTS, HAL_IOMUX_FUNC_UART2_TX, HAL_IOMUX_FUNC_BT_UART_RTS,
      HAL_IOMUX_FUNC_CAM_D7, HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_FUNC_WF_FEM_CNTL5, },
    // P2_6
    { HAL_IOMUX_FUNC_BT_TXEDR, HAL_IOMUX_FUNC_UART1_RX, HAL_IOMUX_FUNC_I2C_M_SCL, HAL_IOMUX_FUNC_BT_UART_RX,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_FUNC_NONE, },
    // P2_7
    { HAL_IOMUX_FUNC_BT_BLE_MODE, HAL_IOMUX_FUNC_UART1_TX, HAL_IOMUX_FUNC_I2C_M_SDA, HAL_IOMUX_FUNC_BT_UART_TX,
      HAL_IOMUX_FUNC_WIMAX_ACTIVE, HAL_IOMUX_FUNC_PWM3, HAL_IOMUX_FUNC_CLK_12M, },
    // P3_0
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_UART0_RX, HAL_IOMUX_FUNC_BT_UART_RX, HAL_IOMUX_FUNC_I2C_M_SCL,
      HAL_IOMUX_FUNC_USBTST_RX0, HAL_IOMUX_FUNC_AO_PWM0, HAL_IOMUX_FUNC_CLK_12M, },
    // P3_1
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_UART0_TX, HAL_IOMUX_FUNC_BT_UART_TX, HAL_IOMUX_FUNC_I2C_M_SDA,
      HAL_IOMUX_FUNC_USBTST_RX1, HAL_IOMUX_FUNC_AO_PWM1, HAL_IOMUX_FUNC_NONE, },
    // P3_2
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SPDIF0_DI, HAL_IOMUX_FUNC_PDM2_CK, HAL_IOMUX_FUNC_SPDIF1_DO,
      HAL_IOMUX_FUNC_USBTST_RX2, HAL_IOMUX_FUNC_AO_PWM2, HAL_IOMUX_FUNC_NONE, },
    // P3_3
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SPDIF0_DO, HAL_IOMUX_FUNC_PDM3_CK, HAL_IOMUX_FUNC_SPDIF1_DI,
      HAL_IOMUX_FUNC_USBTST_TX0, HAL_IOMUX_FUNC_AO_PWM3, HAL_IOMUX_FUNC_WF_TXON, },
    // P3_4
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SPDIF1_DI, HAL_IOMUX_FUNC_DSDIF_CK,
      HAL_IOMUX_FUNC_USBTST_TX1, HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_FUNC_WF_TXON, },
    // P3_5
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_SPDIF1_DO, HAL_IOMUX_FUNC_DSDIF_L,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_FUNC_NONE, },
    // P3_6
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PDM2_D, HAL_IOMUX_FUNC_BT_SWTX, HAL_IOMUX_FUNC_DSDIF_R,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_FUNC_NONE, },
    // P3_7
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PDM0_CK, HAL_IOMUX_FUNC_BT_SWRX, HAL_IOMUX_FUNC_SPDIF1_DO,
      HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PWM3, HAL_IOMUX_FUNC_CLK_12M, },
    // P4_0
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PDM0_D, HAL_IOMUX_FUNC_BT_CLK_OUT, HAL_IOMUX_FUNC_SPDIF1_DI,
      HAL_IOMUX_FUNC_CLK_REQ_OUT, HAL_IOMUX_FUNC_AO_PWM0, HAL_IOMUX_FUNC_WF_TXON, },
    // P4_1
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PDM1_D, HAL_IOMUX_FUNC_I2S_SDI3, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_CLK_REQ_IN, HAL_IOMUX_FUNC_AO_PWM1, HAL_IOMUX_FUNC_NONE, },
    // P4_2
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PDM1_CK, HAL_IOMUX_FUNC_I2S_SDI2, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_CLK_OUT, HAL_IOMUX_FUNC_AO_PWM2, HAL_IOMUX_FUNC_CLK_12M, },
    // P4_3
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_PDM3_D, HAL_IOMUX_FUNC_I2S_SDI1, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_CLK_32K_IN, HAL_IOMUX_FUNC_AO_PWM3, HAL_IOMUX_FUNC_NONE, },
    // P4_4
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_I2S_SDI0, HAL_IOMUX_FUNC_SPDIF1_DI, HAL_IOMUX_FUNC_NONE,
      HAL_IOMUX_FUNC_PCM_DI, HAL_IOMUX_FUNC_PWM0, HAL_IOMUX_FUNC_NONE, },
    // P4_5
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_I2S_SDO, HAL_IOMUX_FUNC_SPDIF1_DO, HAL_IOMUX_FUNC_WF_WAKE1,
      HAL_IOMUX_FUNC_PCM_DO, HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_FUNC_NONE, },
    // P4_6
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_I2S_WS, HAL_IOMUX_FUNC_PDM2_CK, HAL_IOMUX_FUNC_WF_WAKE2,
      HAL_IOMUX_FUNC_PCM_FSYNC, HAL_IOMUX_FUNC_PWM2, HAL_IOMUX_FUNC_NONE, },
    // P4_7
    { HAL_IOMUX_FUNC_NONE, HAL_IOMUX_FUNC_I2S_SCK, HAL_IOMUX_FUNC_PDM3_CK, HAL_IOMUX_FUNC_MCU_CLK_OUT,
      HAL_IOMUX_FUNC_PCM_CLK, HAL_IOMUX_FUNC_PWM3, HAL_IOMUX_FUNC_CLK_12M, },
};

static struct IOMUX_T * const iomux = (struct IOMUX_T *)IOMUX_BASE;

#ifdef ANC_PROD_TEST
#define OPT_TYPE
#else
#define OPT_TYPE                        const
#endif

static OPT_TYPE uint8_t digmic_ck_index = DIG_MIC_CK_IOMUX_INDEX;

void hal_iomux_set_default_voltage(void)
{
    uint32_t val_20;
    uint32_t val_4c;

#ifdef IOMUX_DEFAULT_VOLTAGE_VMEM
    // Keep I2C/UART0 pins using VIO
    val_20 = ~(1 << HAL_IOMUX_PIN_P3_0) | (1 << HAL_IOMUX_PIN_P3_1);
    val_4c = ~0;
#else
    val_20 = 0;
    val_4c = 0;
#endif
    iomux->REG_020 = val_20;
    iomux->REG_04C = SET_BITFIELD(iomux->REG_04C, IOMUX_PAD_VIO_SEL_P4, val_4c);
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

    if (pin == HAL_IOMUX_PIN_P3_0 || pin == HAL_IOMUX_PIN_P3_1) {
        if (func ==  HAL_IOMUX_FUNC_I2C_SCL || func == HAL_IOMUX_FUNC_I2C_SDA) {
            iomux->REG_050 &= ~IOMUX_ANALOG_I2C_DIS;
            iomux->REG_050 |= IOMUX_I2C_M_SEL_DIG_IO;
            iomux->REG_010 = (iomux->REG_010 & ~(IOMUX_FUNC_SEL_P3_0_MASK | IOMUX_FUNC_SEL_P3_1_MASK)) |
                IOMUX_FUNC_SEL_P3_0(IOMUX_FUNC_VAL_GPIO) | IOMUX_FUNC_SEL_P3_1(IOMUX_FUNC_VAL_GPIO);
            return 0;
        } else {
            iomux->REG_050 |= IOMUX_ANALOG_I2C_DIS | IOMUX_I2C_M_SEL_DIG_IO;
            // Continue to set the alt func
        }
    } else if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        ASSERT(func == HAL_IOMUX_FUNC_GPIO || func == HAL_IOMUX_FUNC_AO_GPIO,
            "Bad func=%d for IOMUX pin=%d", func, pin);
        return 0;
    }

    if (func == HAL_IOMUX_FUNC_GPIO) {
        val = IOMUX_FUNC_VAL_GPIO;
    } else if (func == HAL_IOMUX_FUNC_AO_GPIO) {
        val = IOMUX_FUNC_VAL_AO_GPIO;
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

    if (pin < HAL_IOMUX_PIN_P4_0) {
        reg = &iomux->REG_004 + pin / 8;
        shift = (pin % 8) * 4;
    } else {
        reg = &iomux->REG_048;
        shift = (pin - HAL_IOMUX_PIN_P4_0) * 4;
    }

    *reg = (*reg & ~(0xF << shift)) | (val << shift);

    return 0;
}

enum HAL_IOMUX_FUNCTION_T hal_iomux_get_function(enum HAL_IOMUX_PIN_T pin)
{
    return HAL_IOMUX_FUNC_NONE;
}

uint32_t hal_iomux_set_io_voltage_domains(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt)
{
    if (pin >= HAL_IOMUX_PIN_NUM) {
        return 1;
    }

    if (pin < HAL_IOMUX_PIN_P4_0) {
        if (volt == HAL_IOMUX_PIN_VOLTAGE_VIO) {
            iomux->REG_020 &= ~(1 << pin);
        } else {
            iomux->REG_020 |= (1 << pin);
        }
    } else if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        pmu_led_set_voltage_domains(pin, volt);
    } else {
        pin = pin - HAL_IOMUX_PIN_P4_0 + 16;

        if (volt == HAL_IOMUX_PIN_VOLTAGE_VIO) {
            iomux->REG_04C &= ~(1 << pin);
        } else {
            iomux->REG_04C |= (1 << pin);
        }
    }

    return 0;
}

uint32_t hal_iomux_set_io_pull_select(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_PULL_SELECT_T pull_sel)
{
    if (pin >= HAL_IOMUX_PIN_NUM) {
        return 1;
    }

    if (pin < HAL_IOMUX_PIN_P4_0) {
        iomux->REG_02C &= ~(1 << pin);
        iomux->REG_030 &= ~(1 << pin);
        if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENALBE) {
            iomux->REG_02C |= (1 << pin);
        } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENALBE) {
            iomux->REG_030 |= (1 << pin);
        }
    } else if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        pmu_led_set_pull_select(pin, pull_sel);
    } else {
        pin = pin - HAL_IOMUX_PIN_P4_0;
        iomux->REG_04C &= ~((1 << pin) | (1 << (pin + 8)));
        if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENALBE) {
            iomux->REG_04C |= (1 << pin);
        } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENALBE) {
            iomux->REG_04C |= (1 << (pin + 8));
        }
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
    iomux->REG_050 |= IOMUX_ANALOG_I2C_DIS | IOMUX_I2C_M_SEL_DIG_IO;
    // Set uart0 func
    iomux->REG_010 = (iomux->REG_010 & ~(IOMUX_FUNC_SEL_P3_0_MASK | IOMUX_FUNC_SEL_P3_1_MASK)) |
        IOMUX_FUNC_SEL_P3_0(3) | IOMUX_FUNC_SEL_P3_1(3);

    mask = (1 << HAL_IOMUX_PIN_P3_0) | (1 << HAL_IOMUX_PIN_P3_1);
    // Set vio voltage
    iomux->REG_020 &= ~mask;
    // Setup pullup
    iomux->REG_02C |= (1 << HAL_IOMUX_PIN_P3_0);
    iomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P3_1);
    // Clear pulldown
    iomux->REG_030 &= ~mask;
}

void hal_iomux_set_uart1(void)
{
    uint32_t mask;

    // Set uart1 func
    iomux->REG_00C = (iomux->REG_00C & ~(IOMUX_FUNC_SEL_P2_6_MASK | IOMUX_FUNC_SEL_P2_7_MASK)) |
        IOMUX_FUNC_SEL_P2_6(3) | IOMUX_FUNC_SEL_P2_7(3);

    mask = (1 << HAL_IOMUX_PIN_P2_6) | (1 << HAL_IOMUX_PIN_P2_7);
    // Set vio voltage
//#ifdef __KNOWLES
#if 1
	iomux->REG_020 |= mask;
#else
    iomux->REG_020 &= ~mask;
#endif    
    // Setup pullup
    iomux->REG_02C |= (1 << HAL_IOMUX_PIN_P2_6);
    iomux->REG_02C &= ~(1 << HAL_IOMUX_PIN_P2_7);
    // Clear pulldown
    iomux->REG_030 &= ~mask;
}

void hal_iomux_set_analog_i2c(void)
{
    uint32_t mask;

    // Disable analog I2C master
    iomux->REG_050 |= IOMUX_ANALOG_I2C_DIS | IOMUX_I2C_M_SEL_DIG_IO;
    // Set mcu GPIO func
    iomux->REG_010 = (iomux->REG_010 & ~(IOMUX_FUNC_SEL_P3_0_MASK | IOMUX_FUNC_SEL_P3_1_MASK)) |
        IOMUX_FUNC_SEL_P3_0(0xF) | IOMUX_FUNC_SEL_P3_1(0xF);
    // Enable analog I2C slave
    iomux->REG_050 &= ~IOMUX_ANALOG_I2C_DIS;

    mask = (1 << HAL_IOMUX_PIN_P3_0) | (1 << HAL_IOMUX_PIN_P3_1);
    // Set vio voltage
    iomux->REG_020 &= ~mask;
    // Setup pullup
    iomux->REG_02C |= mask;
    // Clear pulldown
    iomux->REG_030 &= ~mask;
}

void hal_iomux_set_jtag(void)
{
    uint32_t mask;
    uint32_t val;

    // SWCLK/TCK, SWDIO/TMS
    mask = IOMUX_FUNC_SEL_P0_1_MASK | IOMUX_FUNC_SEL_P0_2_MASK;
    val = IOMUX_FUNC_SEL_P0_1(2) | IOMUX_FUNC_SEL_P0_2(2);
    // TDI, TDO
#ifdef JTAG_TDI_TDO_PIN
    mask |= IOMUX_FUNC_SEL_P0_0_MASK | IOMUX_FUNC_SEL_P0_4_MASK;
    val |= IOMUX_FUNC_SEL_P0_0(2) | IOMUX_FUNC_SEL_P0_4(2);
#endif
    // RESET
#ifdef JTAG_RESET_PIN
    mask |= IOMUX_FUNC_SEL_P0_3_MASK;
    val |= IOMUX_FUNC_SEL_P0_3(2);
#endif
    // Set jtag func
    iomux->REG_004 = (iomux->REG_004 & ~mask) | val;

    mask = (1 << HAL_IOMUX_PIN_P0_1) | (1 << HAL_IOMUX_PIN_P0_2);
#ifdef JTAG_TDI_TDO_PIN
    mask |= (1 << HAL_IOMUX_PIN_P0_0) | (1 << HAL_IOMUX_PIN_P0_4);
#endif
#ifdef JTAG_RESET_PIN
    mask |= (1 << HAL_IOMUX_PIN_P0_3);
#endif
    // Set vio voltage
    iomux->REG_020 &= ~mask;
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
//#if (I2S_IOMUX_INDEX == 03)
#ifdef __THIRDPARTY
    // To set voltage and pull-up/down states
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2s[] = {
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_GPIO, I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},//SDI
        {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_GPIO, I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},//sck
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_GPIO, I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},//SDO
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_GPIO, I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},//WS
    };

    //ASSERT(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2, "No I2S on P0_0 ~ P0_3");
    iomux->REG_060 |= IOMUX_I2S_P0_0_3;
#else
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2s[] = {
        {HAL_IOMUX_PIN_P4_4, HAL_IOMUX_FUNC_I2S_SDI0, I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_5, HAL_IOMUX_FUNC_I2S_SDO,  I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_6, HAL_IOMUX_FUNC_I2S_WS,   I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P4_7, HAL_IOMUX_FUNC_I2S_SCK,  I2S_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
#endif

    hal_iomux_init(pinmux_i2s, ARRAY_SIZE(pinmux_i2s));
}

void hal_iomux_clear_i2s(void)
{
    iomux->REG_060 &= ~IOMUX_I2S_P0_0_3;
}

void hal_iomux_set_spdif0(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spdif[] = {
        {HAL_IOMUX_PIN_P3_2, HAL_IOMUX_FUNC_SPDIF0_DI, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P3_3, HAL_IOMUX_FUNC_SPDIF0_DO, SPDIF0_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
    };

    hal_iomux_init(pinmux_spdif, ARRAY_SIZE(pinmux_spdif));
}

void hal_iomux_set_spdif1(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spdif[] = {
#if (SPDIF1_DI_IOMUX_INDEX == 33)
        {HAL_IOMUX_PIN_P3_3, HAL_IOMUX_FUNC_SPDIF1_DI, SPDIF1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
#elif (SPDIF1_DI_IOMUX_INDEX == 34)
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_SPDIF1_DI, SPDIF1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
#elif (SPDIF1_DI_IOMUX_INDEX == 40)
        {HAL_IOMUX_PIN_P4_0, HAL_IOMUX_FUNC_SPDIF1_DI, SPDIF1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
#elif (SPDIF1_DI_IOMUX_INDEX == 44)
        {HAL_IOMUX_PIN_P4_4, HAL_IOMUX_FUNC_SPDIF1_DI, SPDIF1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
#else
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_SPDIF1_DI, SPDIF1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
#endif

#if (SPDIF1_DO_IOMUX_INDEX == 35)
        {HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_SPDIF1_DO, SPDIF1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
#elif (SPDIF1_DO_IOMUX_INDEX == 37)
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_SPDIF1_DO, SPDIF1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
#elif (SPDIF1_DO_IOMUX_INDEX == 45)
        {HAL_IOMUX_PIN_P4_5, HAL_IOMUX_FUNC_SPDIF1_DO, SPDIF1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
#else
        {HAL_IOMUX_PIN_P3_2, HAL_IOMUX_FUNC_SPDIF1_DO, SPDIF1_VOLTAGE_SEL, HAL_IOMUX_PIN_PULLUP_ENALBE},
#endif
    };

    hal_iomux_init(pinmux_spdif, ARRAY_SIZE(pinmux_spdif));
}

#ifdef ANC_PROD_TEST
void hal_iomux_set_dig_mic_clock_index(uint32_t index)
{
    digmic_ck_index = index;
}
#endif

void hal_iomux_set_dig_mic(uint32_t map)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic_clk[] = {
        {HAL_IOMUX_PIN_P4_7, HAL_IOMUX_FUNC_PDM3_CK, DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };

    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic0[] = {
        {HAL_IOMUX_PIN_P4_0, HAL_IOMUX_FUNC_PDM0_D,  DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic1[] = {
        {HAL_IOMUX_PIN_P4_1, HAL_IOMUX_FUNC_PDM1_D,  DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic2[] = {
        {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_PDM2_D,  DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_digitalmic3[] = {
        {HAL_IOMUX_PIN_P4_3, HAL_IOMUX_FUNC_PDM3_D,  DIGMIC_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };

    if (digmic_ck_index == 37) {
        pinmux_digitalmic_clk[0].pin = HAL_IOMUX_PIN_P3_7;
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_PDM0_CK;
    } else if (digmic_ck_index == 42) {
        pinmux_digitalmic_clk[0].pin = HAL_IOMUX_PIN_P4_2;
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_PDM1_CK;
    } else if (digmic_ck_index == 22) {
        pinmux_digitalmic_clk[0].pin = HAL_IOMUX_PIN_P2_2;
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_PDM2_CK;
    } else if (digmic_ck_index == 32) {
        pinmux_digitalmic_clk[0].pin = HAL_IOMUX_PIN_P3_2;
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_PDM2_CK;
    } else if (digmic_ck_index == 46) {
        pinmux_digitalmic_clk[0].pin = HAL_IOMUX_PIN_P4_6;
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_PDM2_CK;
    } else if (digmic_ck_index == 23) {
        pinmux_digitalmic_clk[0].pin = HAL_IOMUX_PIN_P2_3;
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_PDM3_CK;
    } else if (digmic_ck_index == 33) {
        pinmux_digitalmic_clk[0].pin = HAL_IOMUX_PIN_P3_3;
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_PDM3_CK;
    } else {
        pinmux_digitalmic_clk[0].pin = HAL_IOMUX_PIN_P4_7;
        pinmux_digitalmic_clk[0].function = HAL_IOMUX_FUNC_PDM3_CK;
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
    if (map & (1 << 3)) {
        hal_iomux_init(pinmux_digitalmic3, ARRAY_SIZE(pinmux_digitalmic3));
    }
}

void hal_iomux_set_spi(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi_3wire[3] = {
#if (SPI_IOMUX_INDEX == 15)
        {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_SPI_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_SPI_CS,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_7, HAL_IOMUX_FUNC_SPI_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SPI_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_SPI_CS,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_SPI_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };
#ifdef SPI_IOMUX_4WIRE
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi_4wire[1] = {
        // SPI_DI (and SPI_DIO is used as SPI_DO)
        {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_GPIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
#endif

    hal_iomux_init(pinmux_spi_3wire, ARRAY_SIZE(pinmux_spi_3wire));
#ifdef SPI_IOMUX_4WIRE
    hal_iomux_init(pinmux_spi_4wire, ARRAY_SIZE(pinmux_spi_4wire));
    hal_gpio_pin_set_dir(pinmux_spi_4wire[0].pin, HAL_GPIO_DIR_IN, 0);
    iomux->REG_060 |= IOMUX_SPI_4WIRE;
#else
    iomux->REG_060 &= ~IOMUX_SPI_4WIRE;
#endif
}

void hal_iomux_clear_spi(void)
{
    iomux->REG_060 &= ~IOMUX_SPI_4WIRE;
}

void hal_iomux_set_spilcd(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spilcd_3wire[] = {
        {HAL_IOMUX_PIN_P0_5, HAL_IOMUX_FUNC_SPILCD_CLK, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_6, HAL_IOMUX_FUNC_SPILCD_CS,  SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_SPILCD_DIO, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
#ifdef SPILCD_IOMUX_4WIRE
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spilcd_4wire[] = {
        // SPILCD_DO (and SPILCD_DIO is used as SPILCD_DI)
        {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_SPILCD_DCN, SPI_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };
#endif

    hal_iomux_init(pinmux_spilcd_3wire, ARRAY_SIZE(pinmux_spilcd_3wire));
#ifdef SPILCD_IOMUX_4WIRE
    hal_iomux_init(pinmux_spilcd_4wire, ARRAY_SIZE(pinmux_spilcd_4wire));
    iomux->REG_060 |= IOMUX_SPILCD_4WIRE;
#else
    iomux->REG_060 &= ~IOMUX_SPILCD_4WIRE;
#endif
}

void hal_iomux_set_clock_out(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_clkout[] = {
        {HAL_IOMUX_PIN_P4_2, HAL_IOMUX_FUNC_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
    };

    hal_iomux_init(pinmux_clkout, ARRAY_SIZE(pinmux_clkout));
}

void hal_iomux_set_mcu_clock_out(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_clkout[] = {
#if (MCU_CLKOUT_IOMUX_INDEX == 23)
        {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_MCU_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (MCU_CLKOUT_IOMUX_INDEX == 47)
        {HAL_IOMUX_PIN_P4_7, HAL_IOMUX_FUNC_MCU_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_MCU_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_clkout, ARRAY_SIZE(pinmux_clkout));
}

void hal_iomux_set_bt_clock_out(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_clkout[] = {
#if (BT_CLKOUT_IOMUX_INDEX == 40)
        {HAL_IOMUX_PIN_P4_0, HAL_IOMUX_FUNC_BT_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_BT_CLK_OUT, CLKOUT_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_clkout, ARRAY_SIZE(pinmux_clkout));
}

void hal_iomux_set_clock_12m(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_clk12m[] = {
#if (CLK12M_IOMUX_INDEX == 03)
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_CLK_12M, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 07)
        {HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_CLK_12M, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 12)
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_CLK_12M, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 14)
        {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_CLK_12M, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 16)
        {HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_CLK_12M, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 27)
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_CLK_12M, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 30)
        {HAL_IOMUX_PIN_P3_0, HAL_IOMUX_FUNC_CLK_12M, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 37)
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_CLK_12M, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 42)
        {HAL_IOMUX_PIN_P4_2, HAL_IOMUX_FUNC_CLK_12M, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 40)
        {HAL_IOMUX_PIN_P4_0, HAL_IOMUX_FUNC_CLK_12M, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_CLK_12M, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_clk12m, ARRAY_SIZE(pinmux_clk12m));
}

void hal_iomux_clear_clock_12m(void)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_clk12m[] = {
#if (CLK12M_IOMUX_INDEX == 03)
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_GPIO, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 07)
        {HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_GPIO, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 12)
        {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_GPIO, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 14)
        {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_GPIO, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 16)
        {HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_GPIO, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 27)
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_GPIO, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 30)
        {HAL_IOMUX_PIN_P3_0, HAL_IOMUX_FUNC_GPIO, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 37)
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_GPIO, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 42)
        {HAL_IOMUX_PIN_P4_2, HAL_IOMUX_FUNC_GPIO, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#elif (CLK12M_IOMUX_INDEX == 40)
        {HAL_IOMUX_PIN_P4_0, HAL_IOMUX_FUNC_GPIO, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#else
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_GPIO, CLK12M_VOLTAGE_SEL, HAL_IOMUX_PIN_NOPULL},
#endif
    };

    hal_iomux_init(pinmux_clk12m, ARRAY_SIZE(pinmux_clk12m));
}

void hal_iomux_set_bt_tport(void)
{
    // P0_0 ~ P0_7
    iomux->REG_004 = IOMUX_FUNC_SEL_P0_0(0xA) | IOMUX_FUNC_SEL_P0_1(0xA) | IOMUX_FUNC_SEL_P0_2(0xA) | IOMUX_FUNC_SEL_P0_3(0xA) |
        IOMUX_FUNC_SEL_P0_4(0xA) | IOMUX_FUNC_SEL_P0_5(0xA) | IOMUX_FUNC_SEL_P0_6(0xA) | IOMUX_FUNC_SEL_P0_7(0xA);
    // ANA TEST DIR
    iomux->REG_014 = 0xFF;
    // ANA TEST SEL
    iomux->REG_01C = IOMUX_ANA_TEST_SEL(5);
}

int hal_pwrkey_set_irq(enum HAL_PWRKEY_IRQ_T type)
{
    uint32_t v;

    if (type == HAL_PWRKEY_IRQ_NONE) {
        v = IOMUX_PWR_KEY_DOWN_INT_CLR | IOMUX_PWR_KEY_UP_INT_CLR;
    } else if (type == HAL_PWRKEY_IRQ_FALLING_EDGE) {
        v = IOMUX_PWR_KEY_DOWN_INT_EN | IOMUX_PWR_KEY_DOWN_INT_MASK;
    } else if (type == HAL_PWRKEY_IRQ_RISING_EDGE) {
        v = IOMUX_PWR_KEY_UP_INT_EN | IOMUX_PWR_KEY_UP_INT_MASK;
    } else if (type == HAL_PWRKEY_IRQ_BOTH_EDGE) {
        v = IOMUX_PWR_KEY_DOWN_INT_EN | IOMUX_PWR_KEY_DOWN_INT_MASK |
            IOMUX_PWR_KEY_UP_INT_EN | IOMUX_PWR_KEY_UP_INT_MASK;
    } else {
        return 1;
    }

    iomux->REG_040 = v;

    return 0;
}

bool hal_pwrkey_pressed(void)
{
    uint32_t v = iomux->REG_040;
    return !!(v & IOMUX_PWR_KEY_VAL);
}

bool hal_pwrkey_startup_pressed(void)
{
    return hal_pwrkey_pressed();
}

enum HAL_PWRKEY_IRQ_T hal_pwrkey_get_irq_state(void)
{
    enum HAL_PWRKEY_IRQ_T state = HAL_PWRKEY_IRQ_NONE;
    uint32_t v = iomux->REG_040;

    if (v & IOMUX_PWR_KEY_DOWN_INT_ST) {
        state |= HAL_PWRKEY_IRQ_FALLING_EDGE;
    }

    if (v & IOMUX_PWR_KEY_UP_INT_ST) {
        state |= HAL_PWRKEY_IRQ_RISING_EDGE;
    }

    return state;
}

