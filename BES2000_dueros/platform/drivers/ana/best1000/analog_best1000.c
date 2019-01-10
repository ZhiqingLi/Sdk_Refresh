#include <stdint.h>
#include "stdbool.h"
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "tgt_hardware.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_cmu.h"
#include "hal_codec.h"
#include "hal_chipid.h"
#include "analog.h"
#include "pmu.h"

#define ANALOG_TRACE(s,...)                         //TRACE(s, ##__VA_ARGS__)

#ifdef _BEST1000_QUAL_DCDC_
//#define __ADC_ACITVE_USE_VCODEC__
#define __DAC_ACITVE_USE_VCODEC__
#define __DAC_ACITVE_USE_LVOLTAGE__
#endif

#define TX_PA_GAIN_0DB                              0x13

#ifdef MAX_DAC_OUTPUT_M60DB
#define DEFAULT_TX_PA_GAIN                          0x00
#elif defined(MAX_DAC_OUTPUT_3P75MW) // -9dB
#define DEFAULT_TX_PA_GAIN                          0x07
#elif defined(MAX_DAC_OUTPUT_5MW)
#define DEFAULT_TX_PA_GAIN                          0x09
#elif defined(MAX_DAC_OUTPUT_10MW)
#define DEFAULT_TX_PA_GAIN                          0x0d
#else // 30mW
#define DEFAULT_TX_PA_GAIN                          TX_PA_GAIN_0DB
#endif

#define DEFAULT_ANC_FF_ADC_GAIN_DB                  30
#define DEFAULT_VOICE_ADC_GAIN_DB                   30 //17.25

#ifndef ANALOG_ADC_A_GAIN_DB
#ifdef ANC_APP
#define ANALOG_ADC_A_GAIN_DB                        DEFAULT_ANC_FF_ADC_GAIN_DB
#else
#define ANALOG_ADC_A_GAIN_DB                        DEFAULT_VOICE_ADC_GAIN_DB
#endif
#endif

#ifndef ANALOG_ADC_B_GAIN_DB
#ifdef ANC_APP
#define ANALOG_ADC_B_GAIN_DB                        DEFAULT_ANC_FF_ADC_GAIN_DB
#else
#define ANALOG_ADC_B_GAIN_DB                        DEFAULT_VOICE_ADC_GAIN_DB
#endif
#endif

#ifndef ANALOG_ADC_C_GAIN_DB
#ifdef _DUAL_AUX_MIC_
#define ANALOG_ADC_C_GAIN_DB                        42
#else
#define ANALOG_ADC_C_GAIN_DB                        0
#endif
#endif

#ifndef ANALOG_ADC_D_GAIN_DB
#ifdef _DUAL_AUX_MIC_
#define ANALOG_ADC_D_GAIN_DB                        18
#else
#define ANALOG_ADC_D_GAIN_DB                        0
#endif
#endif

#ifndef CFG_HW_AUD_INPUT_PATH_LINEIN_DEV
#define CFG_HW_AUD_INPUT_PATH_LINEIN_DEV            0
#endif

#ifndef CFG_HW_AUD_MICKEY_DEV
#define CFG_HW_AUD_MICKEY_DEV                       (AUD_VMIC_MAP_VMIC1)
#endif

#ifndef CFG_HW_AUD_INPUT_PATH_ANC_DEV
#define CFG_HW_AUD_INPUT_PATH_ANC_DEV               (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_VMIC_MAP_VMIC1)
#endif

#define ANALOG_REG31_OFFSET                         0x31
#define ANALOG_REG31_CODEC_RXEN_ADCA                (1 << 0)
#define ANALOG_REG31_CODEC_RXEN_ADCB                (1 << 1)
#define ANALOG_REG31_CODEC_RXEN_ADCC                (1 << 2)
#define ANALOG_REG31_CODEC_RXEN_ADCD                (1 << 3)
#define ANALOG_REG31_CODEC_TXEN_LDAC                (1 << 4)
#define ANALOG_REG31_CODEC_TXEN_RDAC                (1 << 5)
#define ANALOG_REG31_CODEC_CH0_MUTE                 (1 << 6)
#define ANALOG_REG31_CODEC_CH1_MUTE                 (1 << 7)
#define ANALOG_REG31_AUDPLL_FREQ_EN                 (1 << 8)
#define ANALOG_REG31_USBPLL_FREQ_EN                 (1 << 9)
#define ANALOG_REG31_CODEC_PGA1A_EN                 (1 << 10)
#define ANALOG_REG31_CODEC_PGA2A_EN                 (1 << 11)
#define ANALOG_REG31_CODEC_PGA1B_EN                 (1 << 12)
#define ANALOG_REG31_CODEC_PGA2B_EN                 (1 << 13)
#define ANALOG_REG31_CODEC_PGAC_EN                  (1 << 14)
#define ANALOG_REG31_CODEC_PGAD_EN                  (1 << 15)

#define ANALOG_REG31_ENABLE_ALL_DAC                 (ANALOG_REG31_CODEC_TXEN_LDAC | ANALOG_REG31_CODEC_TXEN_RDAC)

#define ANALOG_REG31_ENABLE_MIC_A                   (ANALOG_REG31_CODEC_RXEN_ADCA | ANALOG_REG31_CODEC_PGA1A_EN | ANALOG_REG31_CODEC_PGA2A_EN)
#define ANALOG_REG31_ENABLE_MIC_B                   (ANALOG_REG31_CODEC_RXEN_ADCB | ANALOG_REG31_CODEC_PGA1B_EN | ANALOG_REG31_CODEC_PGA2B_EN)
#define ANALOG_REG31_ENABLE_MIC_C                   (ANALOG_REG31_CODEC_RXEN_ADCC | ANALOG_REG31_CODEC_PGAC_EN)
#define ANALOG_REG31_ENABLE_MIC_D                   (ANALOG_REG31_CODEC_RXEN_ADCD | ANALOG_REG31_CODEC_PGAD_EN)

#define ANALOG_REG31_ENABLE_ALL_ADC                 (ANALOG_REG31_ENABLE_MIC_A    | \
                                                    ANALOG_REG31_ENABLE_MIC_B    | \
                                                    ANALOG_REG31_ENABLE_MIC_C    | \
                                                    ANALOG_REG31_ENABLE_MIC_D)

#define ANALOG_REG32_OFFSET                         0x32
#define ANALOG_REG32_CFG_PGA1A_GAIN_SHIFT           0
#define ANALOG_REG32_CFG_PGA1A_GAIN_MASK            (0x7 << ANALOG_REG32_CFG_PGA1A_GAIN_SHIFT)
#define ANALOG_REG32_CFG_PGA1A_GAIN(n)              BITFIELD_VAL(ANALOG_REG32_CFG_PGA1A_GAIN, n)
#define ANALOG_REG32_CFG_PGA2A_GAIN_SHIFT           3
#define ANALOG_REG32_CFG_PGA2A_GAIN_MASK            (0x7 << ANALOG_REG32_CFG_PGA2A_GAIN_SHIFT)
#define ANALOG_REG32_CFG_PGA2A_GAIN(n)              BITFIELD_VAL(ANALOG_REG32_CFG_PGA2A_GAIN, n)
#define ANALOG_REG32_CFG_PGA1B_GAIN_SHIFT           6
#define ANALOG_REG32_CFG_PGA1B_GAIN_MASK            (0x7 << ANALOG_REG32_CFG_PGA1B_GAIN_SHIFT)
#define ANALOG_REG32_CFG_PGA1B_GAIN(n)              BITFIELD_VAL(ANALOG_REG32_CFG_PGA1B_GAIN, n)
#define ANALOG_REG32_CFG_PGA2B_GAIN_SHIFT           9
#define ANALOG_REG32_CFG_PGA2B_GAIN_MASK            (0x7 << ANALOG_REG32_CFG_PGA2B_GAIN_SHIFT)
#define ANALOG_REG32_CFG_PGA2B_GAIN(n)              BITFIELD_VAL(ANALOG_REG32_CFG_PGA2B_GAIN, n)
#define ANALOG_REG32_CFG_PGA1A_LARGEGAIN            (1 << 12)
#define ANALOG_REG32_CFG_PGA1B_LARGEGAIN            (1 << 13)
#define ANALOG_REG32_CFG_PGAC_LARGEGAIN             (1 << 14)
#define ANALOG_REG32_CFG_PGAD_LARGEGAIN             (1 << 15)

// REG33
#define ANALOG_REG33_CFG_PGA_C_GAIN_SHIFT           0
#define ANALOG_REG33_CFG_PGA_C_GAIN_MASK            (0x7 << ANALOG_REG33_CFG_PGA_C_GAIN_SHIFT)
#define ANALOG_REG33_CFG_PGA_C_GAIN(n)              BITFIELD_VAL(ANALOG_REG33_CFG_PGA_C_GAIN, n)
#define ANALOG_REG33_CFG_PGA_D_GAIN_SHIFT           3
#define ANALOG_REG33_CFG_PGA_D_GAIN_MASK            (0x7 << ANALOG_REG33_CFG_PGA_D_GAIN_SHIFT)
#define ANALOG_REG33_CFG_PGA_D_GAIN(n)              BITFIELD_VAL(ANALOG_REG33_CFG_PGA_D_GAIN, n)
#define ANALOG_REG33_CFG_TX_PA_GAIN_SHIFT           6
#define ANALOG_REG33_CFG_TX_PA_GAIN_MASK            (0x1F << ANALOG_REG33_CFG_TX_PA_GAIN_SHIFT)
#define ANALOG_REG33_CFG_TX_PA_GAIN(n)              BITFIELD_VAL(ANALOG_REG33_CFG_TX_PA_GAIN, n)

#define ANALOG_REG34_OFFSET                         0x34
#define ANALOG_REG34_CFG_ADC_A                      (1 << 0)
#define ANALOG_REG34_CFG_ADC_A_DR                   (1 << 1)
#define ANALOG_REG34_CFG_ADC_B                      (1 << 2)
#define ANALOG_REG34_CFG_ADC_B_DR                   (1 << 3)
#define ANALOG_REG34_CFG_RESETN                     (1 << 4)
#define ANALOG_REG34_CFG_RESETN_DR                  (1 << 5)
#define ANALOG_REG34_CFG_RX                         (1 << 6)
#define ANALOG_REG34_CFG_RX_DR                      (1 << 7)
#define ANALOG_REG34_CFG_SARAD_RSTNC                (1 << 8)
#define ANALOG_REG34_CFG_SARAD_RSTNC_DR             (1 << 9)
#define ANALOG_REG34_CFG_SARAD_RSTND                (1 << 10)
#define ANALOG_REG34_CFG_SARAD_RSTND_DR             (1 << 11)
#define ANALOG_REG34_CFG_BBPLL1_PU                  (1 << 12)
#define ANALOG_REG34_CFG_BBPLL1_PU_DR               (1 << 13)
#define ANALOG_REG34_CFG_BBPLL2_PU                  (1 << 14)
#define ANALOG_REG34_CFG_BBPLL2_PU_DR               (1 << 15)

// REG37
#define ANALOG_REG37_CFG_BBPLL1_DIV_34_32_SHIFT     13
#define ANALOG_REG37_CFG_BBPLL1_DIV_34_32_MASK      (0x7 << ANALOG_REG37_CFG_BBPLL1_DIV_34_32_SHIFT)
#define ANALOG_REG37_CFG_BBPLL1_DIV_34_32(n)        BITFIELD_VAL(ANALOG_REG37_CFG_BBPLL1_DIV_34_32, n)

#define ANALOG_REG3A_OFFSET                         0x3A
#define ANALOG_REG3A_CFG_ADC_IBSEL_SHIFT            (0)
#define ANALOG_REG3A_CFG_ADC_IBSEL_MASK             (0x7)
#define ANALOG_REG3A_CFG_ADC_IBSEL(n)               BITFIELD_VAL(ANALOG_REG3A_CFG_ADC_IBSEL, n)
#define ANALOG_REG3A_CFG_ADC_VCM_CLOSE              (1 << 3)
#define ANALOG_REG3A_CFG_ADC_VREF_SEL_SHIFT         (4)
#define ANALOG_REG3A_CFG_ADC_VREF_SEL_MASK          (0xf)
#define ANALOG_REG3A_CFG_BBOLL1_IQCAL_TTG_EN        (1 << 8)
#define ANALOG_REG3A_CFG_BBPLL1_CLKGEN_EN           (1 << 9)
#define ANALOG_REG3A_CFG_BBPLL1_CLKGEN_DIV_SHIFT    10
#define ANALOG_REG3A_CFG_BBPLL1_CLKGEN_DIV_MASK     (0xF << ANALOG_REG3A_CFG_BBPLL1_CLKGEN_DIV_SHIFT)
#define ANALOG_REG3A_CFG_BBPLL1_CLKGEN_DIV(n)       BITFIELD_VAL(ANALOG_REG3A_CFG_BBPLL1_CLKGEN_DIV, n)
#define ANALOG_REG3A_CFG_TXEN_LPA                   (1 << 14)
#define ANALOG_REG3A_CFG_TXEN_RPA                   (1 << 15)
#define ANALOG_REG3A_ENABLE_ALL_DAC_PA              (ANALOG_REG3A_CFG_TXEN_LPA | ANALOG_REG3A_CFG_TXEN_RPA)

#define ANALOG_REG3B_OFFSET                         0x3B
#define ANALOG_REG3B_CFG_TX_EN_LPPA                 (1 << 0)
#define ANALOG_REG3B_CFG_TX_EN_LPPA_DR              (1 << 1)
#define ANALOG_REG3B_CFG_TX_EN_S1PA                 (1 << 2)
#define ANALOG_REG3B_CFG_TX_EN_S1PA_DR              (1 << 3)
#define ANALOG_REG3B_CFG_TX_EN_S2PA                 (1 << 4)
#define ANALOG_REG3B_CFG_TX_EN_S2PA_DR              (1 << 5)
#define ANALOG_REG3B_CFG_TX_EN_S3PA                 (1 << 6)
#define ANALOG_REG3B_CFG_TX_EN_S3PA_DR              (1 << 7)
#define ANALOG_REG3B_CODEC_TX_PA_DIFFMODE           (1 << 8)
#define ANALOG_REG3B_CODEC_TX_PA_DOUBLEBIAS         (1 << 9)
#define ANALOG_REG3B_CODEC_TX_PA_SOFTSTART_SHIFT    (10)
#define ANALOG_REG3B_CODEC_TX_PA_SOFTSTART_MASK     (0x3f << ANALOG_REG3B_CODEC_TX_PA_SOFTSTART_SHIFT)
#define ANALOG_REG3B_CODEC_TX_PA_SOFTSTART(n)       BITFIELD_VAL(ANALOG_REG3B_CODEC_TX_PA_SOFTSTART, n)

#define ANALOG_REGA1_OFFSET                         0xA1
#define ANALOG_REGA1_PGAC_CAPMODE                   (1 << 0)
#define ANALOG_REGA1_PGAC_FASTSETTLE                (1 << 1)
#define ANALOG_REGA1_PGAD_CAPMODE                   (1 << 2)
#define ANALOG_REGA1_PGAD_FASTSETTLE                (1 << 3)
#define ANALOG_REGA1_PGA2A_LOWNOISE                 (1 << 4)
#define ANALOG_REGA1_EN_VCMLPF                      (1 << 5)
#define ANALOG_REGA1_CFG_TX_EN_S3PA                 (1 << 6)
#define ANALOG_REGA1_CFG_TX_EN_S3PA_DR              (1 << 7)
#define ANALOG_REGA1_CFG_DIV_100K_SHIFT             (8)
#define ANALOG_REGA1_CFG_DIV_100K_MASK              (0xff << ANALOG_REGA1_CFG_DIV_100K_SHIFT)
#define ANALOG_REGA1_CFG_DIV_100K(n)                BITFIELD_VAL(ANALOG_REGA1_CFG_DIV_100K, n)

#define ANALOG_REGB0_OFFSET                         0xB0
#define ANALOG_REGB0_CODEC_RX_IBSEL_SHIFT           (0)
#define ANALOG_REGB0_CODEC_RX_IBSEL_MASK            (0x0f << ANALOG_REGB0_CODEC_RX_IBSEL_SHIFT)
#define ANALOG_REGB0_CODEC_RX_IBSEL(n)              BITFIELD_VAL(ANALOG_REGB0_CODEC_RX_IBSEL, n)
#define ANALOG_REGB0_CODEC_TX_PA_IBSEL_SHIFT        (14)
#define ANALOG_REGB0_CODEC_TX_PA_IBSEL_MASK         (0x03 << ANALOG_REGB0_CODEC_TX_PA_IBSEL_SHIFT)
#define ANALOG_REGB0_CODEC_TX_PA_IBSEL(n)           BITFIELD_VAL(ANALOG_REGB0_CODEC_TX_PA_IBSEL, n)

#define ANALOG_REGB4_OFFSET                                 0xB4
#define ANALOG_REGB4_LCHAN_OFFSET_CORRECT_CODE_SHIFT        (9)
#define ANALOG_REGB4_LCHAN_OFFSET_CORRECT_CODE_MASK         (0xf << ANALOG_REGB4_LCHAN_OFFSET_CORRECT_CODE_SHIFT)
#define ANALOG_REGB4_LCHAN_OFFSET_CORRECT_CODE(n)           BITFIELD_VAL(ANALOG_REGB4_LCHAN_OFFSET_CORRECT_CODE, n)
#define ANALOG_REGB4_RCHAN_OFFSET_CORRECT_CODE0             (1 << 14)

#define ANALOG_REGAE_OFFSET                                 0xAE
#define ANALOG_REGAE_RCHAN_OFFSET_CORRECT_CODE_SHIFT        (3)
#define ANALOG_REGAE_RCHAN_OFFSET_CORRECT_CODE_MASK         (0x7 << ANALOG_REGAE_RCHAN_OFFSET_CORRECT_CODE_SHIFT)
#define ANALOG_REGAE_RCHAN_OFFSET_CORRECT_CODE(n)           BITFIELD_VAL(ANALOG_REGAE_RCHAN_OFFSET_CORRECT_CODE, n)

#define ANA_EFFUSE_DCCALIB_R_SHIFT                  (0)
#define ANA_EFFUSE_DCCALIB_R_MASK                   (0xf << ANA_EFFUSE_DCCALIB_R_SHIFT)
#define ANA_EFFUSE_DCCALIB_L_SHIFT                  (4)
#define ANA_EFFUSE_DCCALIB_L_MASK                   (0xf << ANA_EFFUSE_DCCALIB_L_SHIFT)
#define ANA_EFFUSE_DCCALIB_R_CHECK_SHIFT            (8)
#define ANA_EFFUSE_DCCALIB_R_CHECK_MASK             (0xf << ANA_EFFUSE_DCCALIB_R_CHECK_SHIFT)
#define ANA_EFFUSE_DCCALIB_L_CHECK_SHIFT            (12)
#define ANA_EFFUSE_DCCALIB_L_CHECK_MASK             (0xf << ANA_EFFUSE_DCCALIB_L_CHECK_SHIFT)

#define DB_TO_MAIN_ADC_GAIN(n)                      ((n) * 4)
#define QDB_TO_MAIN_ADC_GAIN(n)                     (n)

#define DB_TO_AUX_ADC_GAIN(n)                       ((n) / 6)

#define DAC_GAIN_TO_QDB(n)                          ((n) * 3)
#define QDB_TO_DAC_GAIN(n)                          ((n) / 3)

#define CAPLESS_MAIN_ADC_GAIN                       0xFFFF
#define LINEIN_MAIN_ADC_GAIN_DB                     0

enum ANA_CODEC_USER_T {
    ANA_CODEC_USER_AUDIO,
    ANA_CODEC_USER_ANC,
    ANA_CODEC_USER_MICKEY,
    ANA_CODEC_USER_DAC_ACT,

    ANA_CODEC_USER_QTY
};

typedef uint8_t CODEC_USER_MAP;
STATIC_ASSERT(sizeof(CODEC_USER_MAP) * 8 >= ANA_CODEC_USER_QTY, "Invalid codec user map type size");

struct ANALOG_PLL_CFG_T {
    uint32_t freq;
    uint8_t div;
    uint16_t val[3];
    int32_t total_delta;
};

static uint16_t dac_gain;
#ifdef ANC_APP
static int16_t dac_gain_offset;
static int16_t main_adc_gain_offset[2];
#endif

static bool ana_spk_req;
static bool ana_spk_muted;
static bool ana_spk_enabled;

static CODEC_USER_MAP adc_a_map;
static CODEC_USER_MAP adc_b_map;
static CODEC_USER_MAP adc_c_map;
static CODEC_USER_MAP adc_d_map;
static CODEC_USER_MAP vmic_1_map;
static CODEC_USER_MAP vmic_2_map;
static CODEC_USER_MAP rx_cfg_map;
static CODEC_USER_MAP vcodec_map;

static uint8_t ana_aud_pll_map;
STATIC_ASSERT(ANA_AUD_PLL_USER_QTY <= sizeof(ana_aud_pll_map) * 8, "Too many ANA AUD PLL users");

static bool vcmlpf_enabled;

// Max allowed total delta is +/-10M offset (50000ppm for 200M, or 25000ppm for 400M)
#define MAX_ALLOWED_TOTAL_DELTA             ((uint64_t)(1 << 28) * 10) / 26

static struct ANALOG_PLL_CFG_T ana_pll_cfg[2];
static int pll_cfg_idx;

void analog_aud_freq_pll_config(uint32_t freq, uint32_t div)
{
    // CODEC_FREQ is likely 24.576M (48K series) or 22.5792M (44.1K series)
    // PLL_nominal = CODEC_FREQ * CODEC_DIV
    // PLL_cfg_val = ((CODEC_FREQ * CODEC_DIV) / 26M) * (1 << 28)

    uint64_t PLL_cfg_val;
    uint16_t val;
    int i, j;

    if (pll_cfg_idx < ARRAY_SIZE(ana_pll_cfg) &&
            ana_pll_cfg[pll_cfg_idx].freq == freq &&
            ana_pll_cfg[pll_cfg_idx].div == div) {
        return;
    }

    j = ARRAY_SIZE(ana_pll_cfg);
    for (i = 0; i < ARRAY_SIZE(ana_pll_cfg); i++) {
        if (ana_pll_cfg[i].freq == freq && ana_pll_cfg[i].div == div) {
            break;
        }
        if (j == ARRAY_SIZE(ana_pll_cfg) && ana_pll_cfg[i].freq == 0) {
            j = i;
        }
    }

    if (i < ARRAY_SIZE(ana_pll_cfg)) {
        pll_cfg_idx = i;
    } else {
        if (j < ARRAY_SIZE(ana_pll_cfg)) {
            pll_cfg_idx = j;
        } else {
            j = 0;
        }

        ana_pll_cfg[pll_cfg_idx].freq = freq;
        ana_pll_cfg[pll_cfg_idx].div = div;
        ana_pll_cfg[pll_cfg_idx].total_delta = 0;

        PLL_cfg_val = ((uint64_t)(1 << 28) * freq * div + 26000000 / 2) / 26000000;
        ana_pll_cfg[pll_cfg_idx].val[0] = PLL_cfg_val & 0xFFFF;
        ana_pll_cfg[pll_cfg_idx].val[1] = (PLL_cfg_val >> 16) & 0xFFFF;
#ifdef AUD_PLL_DOUBLE
        ana_pll_cfg[pll_cfg_idx].val[2] = (PLL_cfg_val >> 32) & 0xFFFF;
#endif
    }

#ifdef AUD_PLL_DOUBLE
    analog_read(0x37, &val);
    val = SET_BITFIELD(val, ANALOG_REG37_CFG_BBPLL1_DIV_34_32, ana_pll_cfg[pll_cfg_idx].val[2]);
    analog_write(0x37, val);
#endif

    analog_write(0x36, ana_pll_cfg[pll_cfg_idx].val[1]);
    analog_write(0x35, ana_pll_cfg[pll_cfg_idx].val[0]);

    analog_read(ANALOG_REG3A_OFFSET, &val);
    val = SET_BITFIELD(val, ANALOG_REG3A_CFG_BBPLL1_CLKGEN_DIV, div);
    analog_write(ANALOG_REG3A_OFFSET, val);

    analog_read(ANALOG_REG31_OFFSET, &val);
    analog_write(ANALOG_REG31_OFFSET,val&(~ANALOG_REG31_AUDPLL_FREQ_EN));
    analog_write(ANALOG_REG31_OFFSET,val|ANALOG_REG31_AUDPLL_FREQ_EN);
}

void analog_aud_pll_reset_cur_config(void)
{
    uint32_t freq, div;

    if (pll_cfg_idx < ARRAY_SIZE(ana_pll_cfg) && ana_pll_cfg[pll_cfg_idx].freq != 0) {
        freq = ana_pll_cfg[pll_cfg_idx].freq;
        div = ana_pll_cfg[pll_cfg_idx].div;
        ana_pll_cfg[pll_cfg_idx].freq = 0;
        ana_pll_cfg[pll_cfg_idx].div = 0;
        analog_aud_freq_pll_config(freq, div);
    }
}

void analog_aud_set_pll_tune_handler(ANA_AUD_PLL_TUNE_HANDLER hdlr)
{
}

void analog_aud_pll_tune(uint32_t rate, int32_t sample, uint32_t ms)
{
    // CODEC_FREQ is likely 24.576M (48K series) or 22.5792M (44.1K series)
    // PLL_nominal = CODEC_FREQ * CODEC_DIV
    // PLL_cfg_val = ((CODEC_FREQ * CODEC_DIV) / 26M) * (1 << 28)
    // Delta = ((SampleDiff / Fs) / TimeDiff) * PLL_cfg_val

    //---------------------------------------------------------
    // DAC
    //---------------------------------------------------------
    // OSR = CODEC_FREQ * CODEC_DIV / CMU_DAC_DIV / (Fs * DAC_UP) = (128, 256, or 512)
    // Delta = (1 << 28) * SampleDiff * CMU_DAC_DIV * DAC_UP * OSR / (MS_TimeDiff / 1000) / 26M

    //---------------------------------------------------------
    // ADC
    //---------------------------------------------------------
    // 512 = CODEC_FREQ * CODEC_DIV / CMU_ADC_DIV / (Fs * ADC_DOWN)
    // Delta = (1 << 28) * SampleDiff * CMU_ADC_DIV * ADC_DOWN * 512 / (MS_TimeDiff / 1000) / 26M

    int64_t delta, total_delta, new_pll;
    uint16_t high, low;
    uint16_t new_high, new_low;
    uint16_t update;
#ifdef AUD_PLL_DOUBLE
    uint16_t val, bit34_32, new_bit34_32;
#endif

    if (pll_cfg_idx >= ARRAY_SIZE(ana_pll_cfg) ||
            ana_pll_cfg[pll_cfg_idx].freq == 0) {
        TRACE("%s: WARNING: aud pll config cache invalid. Skip tuning", __FUNCTION__);
        return;
    }

    // Avoid uint16_t->int (integer promotions in C)
    delta = ((int64_t)
#ifdef AUD_PLL_DOUBLE
        ((uint64_t)ana_pll_cfg[pll_cfg_idx].val[2] << 32) |
#endif
        ((uint32_t)ana_pll_cfg[pll_cfg_idx].val[1] << 16) |
        (uint32_t)ana_pll_cfg[pll_cfg_idx].val[0]) * sample * 1000 / ms / rate;

#if 0
    {
        uint32_t div;
        int64_t delta1;
        if (0) {
            uint32_t adc_down;

            adc_down = hal_codec_get_adc_down(HAL_CODEC_ID_0);
            div = hal_cmu_codec_adc_get_div();

            delta1 = (int64_t)(1 << 28) * sample * div * adc_down * 512 * 1000 / ms / 26000000;
        } else {
            uint32_t osr, dac_up;

            osr = hal_codec_get_dac_osr(HAL_CODEC_ID_0);
            // DAC_UP is 1 for Fs >= 44.1K
            dac_up = hal_codec_get_dac_up(HAL_CODEC_ID_0);
            div = hal_cmu_codec_dac_get_div();

            delta1 = (int64_t)(1 << 28) * sample * div * dac_up * osr * 1000 / ms / 26000000;
        }
        TRACE("%s: delta=%d delta1=%d", __FUNCTION__, (int)delta, (int)delta1);
    }
#endif

    total_delta = delta + ana_pll_cfg[pll_cfg_idx].total_delta;

    if (ABS(total_delta) > MAX_ALLOWED_TOTAL_DELTA) {
        TRACE("\n------\nWARNING: TUNE: total_delta=%d too large, delta=%d is cut\n------\n", (int)total_delta, (int)delta);
        if (total_delta > 0) {
            total_delta = MAX_ALLOWED_TOTAL_DELTA;
        } else {
            total_delta = -MAX_ALLOWED_TOTAL_DELTA;
        }
        delta = total_delta - ana_pll_cfg[pll_cfg_idx].total_delta;
    }
    ana_pll_cfg[pll_cfg_idx].total_delta = total_delta;

    TRACE("%s: rate=%u sample=%d ms=%u delta=%d", __FUNCTION__, rate, sample, ms, (int)delta);

    if (delta) {
        low = ana_pll_cfg[pll_cfg_idx].val[0];
        high = ana_pll_cfg[pll_cfg_idx].val[1];
        // Avoid uint16_t->int (integer promotions in C)
#ifdef AUD_PLL_DOUBLE
        bit34_32 = ana_pll_cfg[pll_cfg_idx].val[2];
        new_pll = (int64_t)(((uint64_t)bit34_32 << 32) | ((uint32_t)high << 16) | (uint32_t)low) - delta;
#else
        new_pll = (int64_t)(((uint32_t)high << 16) | (uint32_t)low) - delta;
#endif

        new_high = (new_pll >> 16) & 0xFFFF;
        new_low = new_pll & 0xFFFF;

#ifdef AUD_PLL_DOUBLE
        new_bit34_32 = (new_pll >> 32) & 0xFFFF;
        if (bit34_32 != new_bit34_32) {
            ana_pll_cfg[pll_cfg_idx].val[2] = new_bit34_32;
            analog_read(0x37, &val);
            val = SET_BITFIELD(val, ANALOG_REG37_CFG_BBPLL1_DIV_34_32, new_bit34_32);
            analog_write(0x37, val);
        }
#endif

        if (new_high != high) {
            ana_pll_cfg[pll_cfg_idx].val[1] = new_high;
            analog_write(0x36, new_high);
        }

        if (new_low != low) {
            ana_pll_cfg[pll_cfg_idx].val[0] = new_low;
            analog_write(0x35, new_low);
        }

        analog_read(ANALOG_REG31_OFFSET, &update);
        analog_write(ANALOG_REG31_OFFSET,update&(~ANALOG_REG31_AUDPLL_FREQ_EN));
        analog_write(ANALOG_REG31_OFFSET,update|ANALOG_REG31_AUDPLL_FREQ_EN);
    }
}

void analog_aud_pll_open(enum ANA_AUD_PLL_USER_T user)
{
    unsigned short val;

    if (user >= ANA_AUD_PLL_USER_QTY) {
        return;
    }

#ifdef __AUDIO_RESAMPLE__
    if (user == ANA_AUD_PLL_USER_CODEC &&
            hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2 &&
            hal_cmu_get_audio_resample_status()) {
        analog_read(ANALOG_REGB4_OFFSET, &val);
        val |= (1 << 5);
        analog_write(ANALOG_REGB4_OFFSET, val);
    } else
#endif
    {
        if (ana_aud_pll_map == 0) {
            analog_read(ANALOG_REG3A_OFFSET, &val);
            val |= ANALOG_REG3A_CFG_BBPLL1_CLKGEN_EN;
            analog_write(ANALOG_REG3A_OFFSET, val);

            hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_AUD);
        }
        ana_aud_pll_map |= (1 << user);
    }
}

void analog_aud_pll_close(enum ANA_AUD_PLL_USER_T user)
{
    unsigned short val;

    if (user >= ANA_AUD_PLL_USER_QTY) {
        return;
    }

#ifdef __AUDIO_RESAMPLE__
    if (user == ANA_AUD_PLL_USER_CODEC &&
            hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2 &&
            hal_cmu_get_audio_resample_status()) {
        analog_read(ANALOG_REGB4_OFFSET, &val);
        val &= ~(1 << 5);
        analog_write(ANALOG_REGB4_OFFSET, val);
    } else
#endif
    {
        ana_aud_pll_map &= ~(1 << user);
        if (ana_aud_pll_map == 0) {
            hal_cmu_pll_disable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_AUD);

            analog_read(ANALOG_REG3A_OFFSET, &val);
            val &= ~ANALOG_REG3A_CFG_BBPLL1_CLKGEN_EN;
            analog_write(ANALOG_REG3A_OFFSET, val);
        }
    }
}

//Note: Can be used to open many dacs, also can be used to close all dacs
static void analog_aud_enable_dac(uint16_t dac)
{
    unsigned short readVal;

    ANALOG_TRACE("[%s] dac = 0x%x", __func__, dac);
    analog_read(ANALOG_REG31_OFFSET, &readVal);

    //clear all dac bit
    readVal &= ~ANALOG_REG31_ENABLE_ALL_DAC;

    if(dac & AUD_CHANNEL_MAP_CH0)
    {
        readVal |= ANALOG_REG31_CODEC_TXEN_LDAC;
    }

    if(dac & AUD_CHANNEL_MAP_CH1)
    {
        readVal |= ANALOG_REG31_CODEC_TXEN_RDAC;
    }

    analog_write(ANALOG_REG31_OFFSET, readVal);
}

static void analog_aud_enable_dac_pa(uint32_t dac)
{
    unsigned short readVal;

    analog_read(ANALOG_REG3A_OFFSET, &readVal);

    //clear all dac bit
    readVal &= ~ANALOG_REG3A_ENABLE_ALL_DAC_PA;
    if(dac & AUD_CHANNEL_MAP_CH0)
    {
        readVal |= ANALOG_REG3A_CFG_TXEN_LPA;
    }

    if(dac & AUD_CHANNEL_MAP_CH1)
    {
        readVal |= ANALOG_REG3A_CFG_TXEN_RPA;
    }

    analog_write(ANALOG_REG3A_OFFSET, readVal);
}

static void analog_aud_vcmlpf_enable(bool en)
{
    uint16_t readVal;

    if (vcmlpf_enabled == en) {
        return;
    }
    vcmlpf_enabled = en;

    if (en) {
        // Make sure both VMIC and ADC are stable
        osDelay(3);
    }

    analog_read(ANALOG_REGA1_OFFSET, &readVal);
    if (en) {
        // Improve vmic psrr
        readVal |= ANALOG_REGA1_EN_VCMLPF;
    } else {
        // Speedup codec startup time
        readVal &= ~ANALOG_REGA1_EN_VCMLPF;
    }
    analog_write(ANALOG_REGA1_OFFSET, readVal);
}

static void analog_aud_enable_adc(enum ANA_CODEC_USER_T user, enum AUD_CHANNEL_MAP_T dev)
{
    uint32_t lock;
    uint16_t val;
    CODEC_USER_MAP old_map;
    bool set = false;

    ANALOG_TRACE("[%s] user=%d dev=0x%x", __func__, user, dev);

    lock = int_lock();

    if (dev & AUD_CHANNEL_MAP_CH0) {
        if (adc_a_map == 0) {
            set = true;
        }
        adc_a_map |= (1 << user);
    } else {
        old_map = adc_a_map;
        adc_a_map &= ~(1 << user);
        if (old_map != 0 && adc_a_map == 0) {
            set = true;
        }
    }

    if (dev & AUD_CHANNEL_MAP_CH1) {
        if (adc_b_map == 0) {
            set = true;
        }
        adc_b_map |= (1 << user);
    } else {
        old_map = adc_b_map;
        adc_b_map &= ~(1 << user);
        if (old_map != 0 && adc_b_map == 0) {
            set = true;
        }
    }

    if (dev & AUD_CHANNEL_MAP_CH2) {
        if (adc_c_map == 0) {
            set = true;
        }
        adc_c_map |= (1 << user);
    } else {
        old_map = adc_c_map;
        adc_c_map &= ~(1 << user);
        if (old_map != 0 && adc_c_map == 0) {
            set = true;
        }
    }

    if (dev & AUD_CHANNEL_MAP_CH3) {
        if (adc_d_map == 0) {
            set = true;
        }
        adc_d_map |= (1 << user);
    } else {
        old_map = adc_d_map;
        adc_d_map &= ~(1 << user);
        if (old_map != 0 && adc_d_map == 0) {
            set = true;
        }
    }

    int_unlock(lock);

    if (set) {
        analog_read(ANALOG_REG31_OFFSET, &val);
        //clear all adc bit
        val &= ~ANALOG_REG31_ENABLE_ALL_ADC;
        if(adc_a_map) {
            val |= ANALOG_REG31_ENABLE_MIC_A;
        }
        if(adc_b_map) {
            val |= ANALOG_REG31_ENABLE_MIC_B;
        }
        if(adc_c_map) {
            val |= ANALOG_REG31_ENABLE_MIC_C;
        }
        if(adc_d_map) {
            val |= ANALOG_REG31_ENABLE_MIC_D;
        }
        if (adc_a_map == 0 && (adc_b_map || adc_c_map || adc_d_map)) {
            // Enable ADCA to enable clock, but disable PGA1A and PGA2A to save power
            val |= ANALOG_REG31_CODEC_RXEN_ADCA;
        }
        analog_write(ANALOG_REG31_OFFSET, val);

        // REG3A IBSEL affects ADCA and ADCB
        if (adc_a_map || adc_b_map) {
            analog_read(ANALOG_REG3A_OFFSET, &val);
            val = SET_BITFIELD(val, ANALOG_REG3A_CFG_ADC_IBSEL, 4);
            analog_write(ANALOG_REG3A_OFFSET, val);
        } else if (adc_c_map || adc_d_map) {
            // Save power when only ADCC and/or ADCD are used.
            analog_read(ANALOG_REG3A_OFFSET, &val);
            val = SET_BITFIELD(val, ANALOG_REG3A_CFG_ADC_IBSEL, 0);
            analog_write(ANALOG_REG3A_OFFSET, val);
        }

        if ((adc_a_map || adc_b_map || adc_c_map || adc_d_map) &&
                ((vmic_1_map & (ANA_CODEC_USER_AUDIO | ANA_CODEC_USER_ANC)) ||
                    (vmic_2_map & (ANA_CODEC_USER_AUDIO | ANA_CODEC_USER_ANC)))) {
            // If any ADC is in use with VMIC
            analog_aud_vcmlpf_enable(true);
        } else {
            // If no ADC is in use with VMIC
            analog_aud_vcmlpf_enable(false);
        }
    }
}

static void analog_aud_set_main_adc_gain(uint16_t gain_a, uint16_t gain_b)
{
    uint16_t val;
    uint8_t pga1a, pga2a, pga1b, pga2b;

    if (gain_a == (uint16_t)CAPLESS_MAIN_ADC_GAIN) {
        pga1a = 0;
        pga2a = 0;
    } else {
#ifdef ANC_APP
        if (main_adc_gain_offset[0] < 0 && -main_adc_gain_offset[0] > gain_a) {
            gain_a = 0;
        } else {
            gain_a += main_adc_gain_offset[0];
        }
#endif

        pga1a = gain_a / DB_TO_MAIN_ADC_GAIN(6) + 1;
        pga2a = (gain_a % DB_TO_MAIN_ADC_GAIN(6)) / 3;
    }

    if (gain_b == (uint16_t)CAPLESS_MAIN_ADC_GAIN) {
        pga1b = 0;
        pga2b = 0;
    } else {
#ifdef ANC_APP
        if (main_adc_gain_offset[1] < 0 && -main_adc_gain_offset[1] > gain_b) {
            gain_b = 0;
        } else {
            gain_b += main_adc_gain_offset[1];
        }
#endif

        pga1b = gain_b / DB_TO_MAIN_ADC_GAIN(6) + 1;
        pga2b = (gain_b % DB_TO_MAIN_ADC_GAIN(6)) / 3;
    }

#ifdef ANC_APP
    //TRACE("ana: set main adc gain: %u / %u (offset=%d)", main_adc_gain, gain, main_adc_gain_offset);
#endif

    // disable large gain
    val = ANALOG_REG32_CFG_PGA1A_GAIN(pga1a) | ANALOG_REG32_CFG_PGA2A_GAIN(pga2a) |
        ANALOG_REG32_CFG_PGA1B_GAIN(pga1b) | ANALOG_REG32_CFG_PGA2B_GAIN(pga2b);
    analog_write(ANALOG_REG32_OFFSET, val);
}

static void analog_aud_set_adc_gain(enum AUD_IO_PATH_T input_path)
{
    uint8_t pga_c, pga_d;
    uint16_t val;
    uint16_t main_gain_a, main_gain_b;

    if (input_path == AUD_INPUT_PATH_LINEIN) {
        main_gain_a = DB_TO_MAIN_ADC_GAIN(LINEIN_MAIN_ADC_GAIN_DB);
        main_gain_b = DB_TO_MAIN_ADC_GAIN(LINEIN_MAIN_ADC_GAIN_DB);
    } else {
#ifdef _MIC_CAPLESSMODE_
        main_gain_a = CAPLESS_MAIN_ADC_GAIN;
        main_gain_b = CAPLESS_MAIN_ADC_GAIN;
#else
        main_gain_a = DB_TO_MAIN_ADC_GAIN(ANALOG_ADC_A_GAIN_DB);
        main_gain_b = DB_TO_MAIN_ADC_GAIN(ANALOG_ADC_B_GAIN_DB);
#endif
    }

    analog_aud_set_main_adc_gain(main_gain_a, main_gain_b);

    pga_c = DB_TO_AUX_ADC_GAIN(ANALOG_ADC_C_GAIN_DB);
    pga_d = DB_TO_AUX_ADC_GAIN(ANALOG_ADC_D_GAIN_DB);;

    analog_read(0x33, &val);
    val = (val & ~(ANALOG_REG33_CFG_PGA_C_GAIN_MASK | ANALOG_REG33_CFG_PGA_D_GAIN_MASK)) |
        ANALOG_REG33_CFG_PGA_C_GAIN(pga_c) | ANALOG_REG33_CFG_PGA_D_GAIN(pga_d);
    analog_write(0x33,val);
}

#ifdef ANC_APP
void analog_aud_apply_anc_adc_gain_offset(enum ANC_TYPE_T type, int16_t offset_l, int16_t offset_r)
{
    if (main_adc_gain_offset[0] == offset_l && main_adc_gain_offset[1] == offset_r) {
        return;
    }

    TRACE("ana: apply adc gain offset: %d/d", offset_l, offset_r);

    main_adc_gain_offset[0] = offset_l;
    main_adc_gain_offset[1] = offset_r;
    analog_aud_set_adc_gain(AUD_INPUT_PATH_MAINMIC);
}
#endif

static void analog_codec_tx_pa_gain_sel(uint32_t v)
{
    unsigned short val = 0;
    analog_read(0x33, &val);
    val = SET_BITFIELD(val, ANALOG_REG33_CFG_TX_PA_GAIN, v);
    analog_write(0x33, val);
}

void analog_aud_set_dac_gain(int32_t v)
{
    if (v < 0) {
        v = DEFAULT_TX_PA_GAIN;
    }

    dac_gain = v;

#ifdef ANC_APP
    if (v) {
        if (dac_gain_offset < 0 && -dac_gain_offset > v) {
            v = 0;
        } else {
            v += dac_gain_offset;
            if (v > (ANALOG_REG33_CFG_TX_PA_GAIN_MASK >> ANALOG_REG33_CFG_TX_PA_GAIN_SHIFT)) {
                v = (ANALOG_REG33_CFG_TX_PA_GAIN_MASK >> ANALOG_REG33_CFG_TX_PA_GAIN_SHIFT);
            }
        }
    }

    //TRACE("ana: set dac gain: %u / %u (offset=%d)", dac_gain, v, dac_gain_offset);
#endif

    analog_codec_tx_pa_gain_sel(v);
}

uint32_t analog_codec_get_dac_gain(void)
{
    unsigned short val = 0;
    analog_read(0x33, &val);
    val = GET_BITFIELD(val, ANALOG_REG33_CFG_TX_PA_GAIN);
    return val;
}

uint32_t analog_codec_dac_gain_to_db(int32_t gain)
{
    if (gain < 0) {
        gain = DEFAULT_TX_PA_GAIN;
    }
    return (DAC_GAIN_TO_QDB(gain) + 2) / 4;
}

int32_t analog_codec_dac_max_attn_db(void)
{
    return -(DAC_GAIN_TO_QDB(TX_PA_GAIN_0DB) + 2) / 4;
}

#ifdef ANC_APP
void analog_aud_apply_anc_dac_gain_offset(int16_t offset)
{
    int16_t new_offset;

    new_offset = QDB_TO_DAC_GAIN(offset);

    if (dac_gain_offset == new_offset) {
        return;
    }

    TRACE("ana: apply dac gain offset: %d", new_offset);

    dac_gain_offset = new_offset;
    analog_aud_set_dac_gain(dac_gain);

    // Compensate the offset in digital DAC
    hal_codec_apply_dac_gain_offset(HAL_CODEC_ID_0, -offset);
}
#endif

static int POSSIBLY_UNUSED dc_calib_checksum_valid(uint16_t efuse)
{
    int i;
    uint32_t cnt = 0;

    for (i = 0; i < 12; i++) {
        if (efuse & (1 << i)) {
            cnt++;
        }
    }

    return (((~cnt) & 0xF) == ((efuse >> 12) & 0xF));
}

void analog_aud_get_dc_calib_value(int16_t *dc_l, int16_t *dc_r)
{
    uint16_t val;
    uint16_t efuse;
    static const uint16_t DC_VALUE_MASK = 0x07FF;
    static const uint16_t DC_VALUE_SIGN = 0x0800;

    pmu_get_efuse(PMU_EFUSE_PAGE_DCCALIB_L, &efuse);
#ifdef ANA_OLD_DC_CALIB
    if (efuse == 0) {
        goto _old_calib;
    }
#endif
    val = efuse & DC_VALUE_MASK;
    if (efuse & DC_VALUE_SIGN) {
        *dc_l = val;
    } else {
        *dc_l = -val;
    }
    if (dc_calib_checksum_valid(efuse)) {
        TRACE("Dc calib L OK: 0x%04x", efuse);
    } else {
        TRACE("Warning: Bad dc calib efuse L: 0x%04x", efuse);
        *dc_l = 0;
    }

    pmu_get_efuse(PMU_EFUSE_PAGE_DCCALIB_R, &efuse);
#ifdef ANA_OLD_DC_CALIB
    if (efuse == 0) {
        goto _old_calib;
    }
#endif
    val = efuse & DC_VALUE_MASK;
    if (efuse & DC_VALUE_SIGN) {
        *dc_r = val;
    } else {
        *dc_r = -val;
    }
    if (dc_calib_checksum_valid(efuse)) {
        TRACE("Dc calib R OK: 0x%04x", efuse);
    } else {
        TRACE("Warning: Bad dc calib efuse R: 0x%04x", efuse);
        *dc_r = 0;
    }

_got_values:
    TRACE("ANA: DC CALIB L=%d R=%d", *dc_l, *dc_r);

#if defined(ANA_DC_CALIB_L) || defined(ANA_DC_CALIB_R)
#ifdef ANA_DC_CALIB_L
    *dc_l = ANA_DC_CALIB_L;
#endif
#ifdef ANA_DC_CALIB_R
    *dc_r = ANA_DC_CALIB_R;
#endif
    TRACE("ANA: OVERRIDE DC CALIB L=%d R=%d", *dc_l, *dc_r);
#endif

    return;

#ifdef ANA_OLD_DC_CALIB
_old_calib:
    static const uint16_t DC_VALUE_MASK_ALT = 0x00FF;
    static const uint16_t DC_VALUE_SIGN_ALT = 0x0100;

    pmu_get_efuse(PMU_EFUSE_PAGE_DCCALIB2_L, &efuse);
    val = efuse & DC_VALUE_MASK_ALT;
    if (efuse & DC_VALUE_SIGN_ALT) {
        *dc_l = val;
    } else {
        *dc_l = -val;
    }

    pmu_get_efuse(PMU_EFUSE_PAGE_DCCALIB2_R, &efuse);
    val = efuse & DC_VALUE_MASK_ALT;
    if (efuse & DC_VALUE_SIGN_ALT) {
        *dc_r = val;
    } else {
        *dc_r = -val;
    }
#endif

    goto _got_values;
}

void analog_open(void)
{
    uint16_t readVal;

    // -------------------------------------
    // Direct analog register initialization
    // -------------------------------------

    readVal = 0;
    if (hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2) {
        readVal = (readVal & ~0xFF) | ((1 << 4) | (0x3 << 2));
    }
#if defined(_BEST1000_QUAL_DCDC_) || !defined(CODEC_HIGH_QUALITY)
    readVal = (readVal & ~(0x3 << 13));
#else
    // For codec voltage larger than 2.3V
    readVal = (readVal & ~(0x3 << 13)) | (0x1 << 13);
#endif
    analog_write(ANALOG_REGB4_OFFSET, readVal);

    // Init audio PLL (22.5792M * 9 = 203.2128M)
    analog_write(0x37,0x0000);
    analog_write(0x36,0x7D0D);
    analog_write(0x35,0xD4F6);
    analog_read(ANALOG_REG31_OFFSET, &readVal);
    analog_write(ANALOG_REG31_OFFSET, readVal&(~ANALOG_REG31_AUDPLL_FREQ_EN));
    analog_write(ANALOG_REG31_OFFSET, readVal|ANALOG_REG31_AUDPLL_FREQ_EN);
    analog_write(0xA7,0x7F3F);

#ifdef AUD_PLL_DOUBLE
    // codec_bbpll1_vco_vres/codec_bbpll1_ldo_out are set to max
    analog_write(0xA8,0x36FF);
    // codec_bbpll1_vco_ngmc/codec_bbpll1_vco_gmc are set to max
    analog_write(0xAA,0x48BF);
#endif

    // Init 0xA2 / 0xA3
    analog_read(0xA2, &readVal);
    readVal &= ~(1 << 15);
    analog_write(0xA2, readVal);
    analog_write(0xA3, readVal);

#if defined(_BEST1000_QUAL_DCDC_) || !defined(CODEC_HIGH_QUALITY)
    // Set micbias to vcodec
    analog_read(0xA0, &readVal);
    readVal=(readVal&(~(0xf<<12)))|(0xf<<12);
    readVal=(readVal&(~(0xf<<7)))|(0xf<<7);
    analog_write(0xA0, readVal);
#endif

    // Init ANALOG_REGAE_RCHAN_OFFSET_CORRECT_CODE to 0
    if (hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4) {
        analog_write(ANALOG_REGAE_OFFSET, 0x48C3);
    }

    //DAC INIT
    analog_write(0x33,0xFC00);
#if defined(_BEST1000_QUAL_DCDC_) || !defined(CODEC_HIGH_QUALITY)
    if (hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_3) {
        analog_write(0x3C,0x6859);
    } else
#endif
    {
        uint16_t efuse;

        pmu_get_efuse(PMU_EFUSE_PAGE_PROD_TEST, &efuse);
        if (efuse & 0x100) {
            readVal = 0x6959;
        } else if (efuse & 0x200) {
            readVal = 0x5859;
        } else {
            readVal = 0x4859;
        }
        analog_write(0x3C, readVal);
    }

    // DAC diff mode
    readVal = 0;
#ifdef AUDIO_OUTPUT_DIFF
    analog_write(0xA4,0x0000);
    analog_write(0xA5,0x0000);
    readVal |= ANALOG_REG3B_CODEC_TX_PA_DIFFMODE;
#else
    analog_write(0xA4,0x1000);
    analog_write(0xA5,0x1000);
    //dac rise ramp
    readVal |= ANALOG_REG3B_CODEC_TX_PA_SOFTSTART(63);
#endif
#ifdef CODEC_HIGH_QUALITY
    readVal |= ANALOG_REG3B_CODEC_TX_PA_DOUBLEBIAS;
#endif
    analog_write(ANALOG_REG3B_OFFSET, readVal);

    readVal = 0x0108;
#ifdef CODEC_HIGH_THD_WORKAROUND
    // Improve THD (especially for high frequency tones)
    // Current consumption will be increased by 1~2 mA, and DAC DC offset will be changed as well.
    readVal = SET_BITFIELD(readVal, ANALOG_REGB0_CODEC_TX_PA_IBSEL, 3);
#else
    // DAC DC offset calib in production test uses value 1
    readVal = SET_BITFIELD(readVal, ANALOG_REGB0_CODEC_TX_PA_IBSEL, 1);
#endif
    analog_write(ANALOG_REGB0_OFFSET, readVal);

#ifdef __AUDIO_OUTPUT_VOLT_STAY_HIGH__
    if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2)
    {
        analog_read(ANALOG_REG3B_OFFSET, &readVal);
        readVal |= (63<<ANALOG_REG3B_CODEC_TX_PA_SOFTSTART_SHIFT);
        analog_write(ANALOG_REG3B_OFFSET, readVal);
    }
#endif

    // Set supplement ADC half rate
    analog_write(0xB2,0x2000);

//Note: capmode using largegain mode can get better noise performance
//Note: enable largegain and (pga1x = 0x06) have same gain
#ifdef _MIC_CAPLESSMODE_
    analog_read(ANALOG_REG32_OFFSET, &readVal);
#ifdef _MIC_LARGEGAIN_
    readVal |= ANALOG_REG32_CFG_PGA1A_LARGEGAIN;
    analog_write(ANALOG_REG32_OFFSET, readVal);  // enable largegain
#else
    readVal |= 0;
    analog_write(ANALOG_REG32_OFFSET, readVal);  // enable largegaindd
#endif
    // enable pga(a and b) capmode
    analog_read(0xA0, &readVal);
    readVal |= (1 << 0)|(1 << 3);
    readVal |= (1 << 6)|(1 << 11);
    analog_write(0xA0, readVal);
    // enable pga(c and d) capmode
    analog_read(ANALOG_REGA1_OFFSET, &readVal);
    analog_write(ANALOG_REGA1_OFFSET, readVal|ANALOG_REGA1_PGAC_CAPMODE|ANALOG_REGA1_PGAD_CAPMODE);
#else // !_MIC_CAPLESSMODE_
    analog_read(0xA0, &readVal);
    readVal &= ~((1 << 0)|(1 << 3));
    readVal |= (1 << 6)|(1 << 11);
    analog_write(0xA0, readVal);

    analog_read(ANALOG_REGA1_OFFSET, &readVal);
    analog_write(ANALOG_REGA1_OFFSET, readVal&(~(ANALOG_REGA1_PGAC_CAPMODE|ANALOG_REGA1_PGAD_CAPMODE)));
#endif // !_MIC_CAPLESSMODE_

    // default set adc(a, b, c, d) mic mode
    analog_write(0x3F,0x0AAA);

    //close all adc, dac, pll and so on.
    analog_write(ANALOG_REG31_OFFSET, 0);

    // ----------------------------------------------------
    // Further initialization with analog setting functions
    // ----------------------------------------------------

    analog_codec_tx_pa_gain_sel(0);

    analog_aud_vcmlpf_enable(false);

#ifdef ANC_INIT_SPEEDUP
    analog_aud_codec_anc_enable(ANC_FEEDFORWARD, true);
    osDelay(500);
#endif

#ifdef CFG_HW_AUD_OUTPUT_POP_SWITCH
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_output_switch =
        {CFG_HW_AUD_OUTPUT_POP_SWITCH, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};
    hal_iomux_init(&pinmux_output_switch, 1);
    hal_gpio_pin_set_dir(CFG_HW_AUD_OUTPUT_POP_SWITCH, HAL_GPIO_DIR_OUT, 0);
#endif
}

void analog_sleep(void)
{
    // Vcodec and related LDOs cannot work in sleep mode
#if 0
    uint16_t readVal;

    if (rx_cfg_map) {
        analog_read(ANALOG_REG34_OFFSET,&readVal);
        readVal &= ~(ANALOG_REG34_CFG_RX|ANALOG_REG34_CFG_RX_DR);
        analog_write(ANALOG_REG34_OFFSET,readVal);
    }
#endif
}

void analog_wakeup(void)
{
    // Vcodec and related LDOs cannot work in sleep mode
#if 0
    uint16_t readVal;

    if (rx_cfg_map) {
        analog_read(ANALOG_REG34_OFFSET,&readVal);
        readVal |= ANALOG_REG34_CFG_RX|ANALOG_REG34_CFG_RX_DR;
        analog_write(ANALOG_REG34_OFFSET,readVal);
    }
#endif
}

static void analog_aud_vcodec_enable(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (vcodec_map == 0) {
            set = true;
        }
        vcodec_map |= (1 << user);
    } else {
        vcodec_map &= ~(1 << user);
        if (vcodec_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        pmu_codec_config(!!vcodec_map);
    }
}

static void analog_aud_enable_rx_cfg(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    uint16_t val;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (rx_cfg_map == 0) {
            set = true;
        }
        rx_cfg_map |= (1 << user);
    } else {
        rx_cfg_map &= ~(1 << user);
        if (rx_cfg_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        analog_read(ANALOG_REG34_OFFSET, &val);
        if (rx_cfg_map) {
            val |= ANALOG_REG34_CFG_RX|ANALOG_REG34_CFG_RX_DR;
        } else {
            val &= ~(ANALOG_REG34_CFG_RX|ANALOG_REG34_CFG_RX_DR);
        }
        analog_write(ANALOG_REG34_OFFSET, val);
    }
}

static void analog_aud_enable_vmic(enum ANA_CODEC_USER_T user, uint32_t dev)
{
    uint32_t lock;
    CODEC_USER_MAP old_map;
    bool set = false;

    lock = int_lock();
    if (dev & AUD_VMIC_MAP_VMIC1) {
        if (vmic_1_map == 0) {
            set = true;
        }
        vmic_1_map |= (1 << user);
    } else {
        old_map = vmic_1_map;
        vmic_1_map &= ~(1 << user);
        if (old_map != 0 && vmic_1_map == 0) {
            set = true;
        }
    }

    if (dev & AUD_VMIC_MAP_VMIC2) {
        if (vmic_2_map == 0) {
            set = true;
        }
        vmic_2_map |= (1 << user);
    } else {
        old_map = vmic_2_map;
        vmic_2_map &= ~(1 << user);
        if (old_map != 0 && vmic_2_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        uint16_t val;

        analog_read(0x3F, &val);
        val &= ~(3 << 13);
        if (vmic_1_map) {
            val |= (1 << 13);
        }
        if (vmic_2_map) {
            val |= (1 << 14);
        }
        analog_write(0x3F, val);
    }
}

void analog_aud_codec_anc_enable(enum ANC_TYPE_T type, bool en)
{
    if (en) {
        analog_aud_vcodec_enable(ANA_CODEC_USER_ANC, true);
        analog_aud_enable_rx_cfg(ANA_CODEC_USER_ANC, true);
        analog_aud_enable_vmic(ANA_CODEC_USER_ANC, CFG_HW_AUD_INPUT_PATH_ANC_DEV);
        analog_aud_enable_adc(ANA_CODEC_USER_ANC, (CFG_HW_AUD_INPUT_PATH_ANC_DEV & AUD_CHANNEL_MAP_ALL));

        analog_aud_set_adc_gain(AUD_INPUT_PATH_MAINMIC);
    } else {
        analog_aud_enable_adc(ANA_CODEC_USER_ANC, 0);
        analog_aud_enable_vmic(ANA_CODEC_USER_ANC, 0);
        analog_aud_enable_rx_cfg(ANA_CODEC_USER_ANC, false);
        analog_aud_vcodec_enable(ANA_CODEC_USER_ANC, false);
    }
}

void analog_aud_codec_anc_init_close(void)
{
    analog_aud_codec_anc_enable(ANC_FEEDFORWARD, false);
}

void analog_aud_mickey_enable(bool en)
{
    if (en) {
        analog_aud_vcodec_enable(ANA_CODEC_USER_MICKEY, true);
        analog_aud_enable_rx_cfg(ANA_CODEC_USER_MICKEY, true);
        analog_aud_enable_vmic(ANA_CODEC_USER_MICKEY, CFG_HW_AUD_MICKEY_DEV);
    } else {
        analog_aud_enable_vmic(ANA_CODEC_USER_MICKEY, 0);
        analog_aud_enable_rx_cfg(ANA_CODEC_USER_MICKEY, false);
        analog_aud_vcodec_enable(ANA_CODEC_USER_MICKEY, false);
    }
}

void analog_aud_codec_adc_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    uint32_t dev;

    if(en)
    {
        if (input_path == AUD_INPUT_PATH_MAINMIC) {
            dev = CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV;
        } else if (input_path == AUD_INPUT_PATH_LINEIN) {
            dev = CFG_HW_AUD_INPUT_PATH_LINEIN_DEV;
        } else {
            ASSERT(false, "%s: Bad input path: %d", __func__, input_path);
        }

        analog_aud_enable_vmic(ANA_CODEC_USER_AUDIO, dev);

        analog_aud_set_adc_gain(input_path);

#ifdef _AUTO_SWITCH_POWER_MODE__
#ifdef _MIC_CAPLESSMODE_
        unsigned short readVal_a0;
#endif
        unsigned short readVal_34;
        //force disable ADC
        analog_read(ANALOG_REG34_OFFSET,&readVal_34);
        readVal_34 |= ANALOG_REG34_CFG_ADC_A|ANALOG_REG34_CFG_ADC_A_DR|ANALOG_REG34_CFG_ADC_B|ANALOG_REG34_CFG_ADC_B_DR;
        analog_write(ANALOG_REG34_OFFSET,readVal_34);
#ifdef __ADC_ACITVE_USE_VCODEC__
        pmu_codec_adc_pre_start();
#endif
        osDelay(1);
#ifdef _MIC_CAPLESSMODE_
        // disable pga(a and b) capmode
        analog_read(0xA0, &readVal_a0);
        readVal_a0 &= ~(1<<0);
        analog_write(0xA0,readVal_a0);
        osDelay(2);
#endif
#endif

        analog_aud_enable_adc(ANA_CODEC_USER_AUDIO, ch_map);

#ifdef _AUTO_SWITCH_POWER_MODE__
#ifdef __ADC_ACITVE_USE_VCODEC__
        pmu_codec_adc_post_start(osDelay);
#endif
#ifdef _MIC_CAPLESSMODE_
        // enable pga(a and b) capmode
        readVal_a0 |= (1<<0);
        analog_write(0xA0,readVal_a0);
#endif
        osDelay(5);
        //resume ADC status
        readVal_34 &= ~(ANALOG_REG34_CFG_ADC_A|ANALOG_REG34_CFG_ADC_A_DR|ANALOG_REG34_CFG_ADC_B|ANALOG_REG34_CFG_ADC_B_DR);
        analog_write(ANALOG_REG34_OFFSET,readVal_34);
#endif
    }
    else
    {
        analog_aud_enable_adc(ANA_CODEC_USER_AUDIO, 0);

        analog_aud_enable_vmic(ANA_CODEC_USER_AUDIO, 0);
    }
}

static void analog_aud_codec_config_speaker(void)
{
    bool en;

    if (ana_spk_req && !ana_spk_muted) {
        en = true;
    } else {
        en = false;
    }

    if (ana_spk_enabled != en) {
        ana_spk_enabled = en;
        if (en) {
            analog_aud_enable_dac_pa(CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV);
#ifdef CFG_HW_AUD_OUTPUT_POP_SWITCH
            hal_sys_timer_delay_us(5);
            hal_gpio_pin_set(CFG_HW_AUD_OUTPUT_POP_SWITCH);
#endif
        } else {
            analog_aud_enable_dac_pa(0);
#ifdef CFG_HW_AUD_OUTPUT_POP_SWITCH
            hal_gpio_pin_clr(CFG_HW_AUD_OUTPUT_POP_SWITCH);
#endif
        }
    }
}

void analog_aud_codec_speaker_enable(bool en)
{
    ana_spk_req = en;
    analog_aud_codec_config_speaker();
}

void analog_aud_codec_dac_enable(bool en)
{
    if(en)
    {
#ifndef __DAC_ACITVE_USE_LVOLTAGE__
#ifdef _AUTO_SWITCH_POWER_MODE__
#ifdef __DAC_ACITVE_USE_VCODEC__
        analog_aud_enable_rx_cfg(ANA_CODEC_USER_DAC_ACT, true);
        pmu_codec_dac_pre_start();
#endif
#endif
#endif

        analog_aud_enable_dac(CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV);

#ifndef __DAC_ACITVE_USE_LVOLTAGE__
#ifdef _AUTO_SWITCH_POWER_MODE__
#ifdef __DAC_ACITVE_USE_VCODEC__
        pmu_codec_dac_post_start(osDelay);
        analog_aud_enable_rx_cfg(ANA_CODEC_USER_DAC_ACT, false);
#endif
#endif
#endif

#ifndef AUDIO_OUTPUT_DC_CALIB
        osDelay(4);
        analog_aud_codec_speaker_enable(true);
#endif
    }
    else
    {
#ifndef AUDIO_OUTPUT_DC_CALIB
        analog_aud_codec_speaker_enable(false);
        osDelay(1);
#endif

        analog_aud_enable_dac(0);
    }
}

void analog_aud_codec_open(void)
{
    //analog_codec_tx_pa_gain_sel(0);

    analog_aud_vcodec_enable(ANA_CODEC_USER_AUDIO, true);

#ifdef _AUTO_SWITCH_POWER_MODE__
    pmu_mode_change(PMU_POWER_MODE_DIG_DCDC);
#endif

    // NOTE:
    // If DELAY_STREAM_OPEN is enabled and CFG_HW_AUD_OUTPUT_POP_SWITCH is disabled,
    // vcodec might need 120ms to be fully stable.
    // (PA output spur in 30ms is about 20% larger than that in 120ms)
    osDelay(30);
}

void analog_aud_codec_close(void)
{
    unsigned short POSSIBLY_UNUSED readVal;

    //analog_codec_tx_pa_gain_sel(0);

    analog_aud_codec_speaker_enable(false);
    osDelay(1);
    analog_aud_codec_dac_enable(false);

    analog_aud_codec_adc_enable(AUD_IO_PATH_NULL, 0, false);

#ifdef _AUTO_SWITCH_POWER_MODE__
    pmu_mode_change(PMU_POWER_MODE_ANA_DCDC);
#endif

    analog_aud_vcodec_enable(ANA_CODEC_USER_AUDIO, false);
}

void analog_aud_codec_mute(void)
{
#ifndef AUDIO_OUTPUT_DC_CALIB
    //analog_codec_tx_pa_gain_sel(0);
#endif

    ana_spk_muted = true;
    analog_aud_codec_config_speaker();

#if 0
#ifndef AUDIO_OUTPUT_DC_CALIB
    unsigned short readVal;
    analog_read(ANALOG_REG31_OFFSET, &readVal);
    analog_write(ANALOG_REG31_OFFSET,readVal |
                                     ANALOG_REG31_CODEC_CH0_MUTE|
                                     ANALOG_REG31_CODEC_CH1_MUTE);
#endif
#endif
}

void analog_aud_codec_nomute(void)
{
#ifndef AUDIO_OUTPUT_DC_CALIB
#if 0
    unsigned short readVal;
    analog_read(ANALOG_REG31_OFFSET, &readVal);
    analog_write(ANALOG_REG31_OFFSET,readVal &
                                     ~(ANALOG_REG31_CODEC_CH0_MUTE|
                                     ANALOG_REG31_CODEC_CH1_MUTE));
#endif
#endif

    ana_spk_muted = false;
    analog_aud_codec_config_speaker();

#ifndef AUDIO_OUTPUT_DC_CALIB
    //analog_aud_set_dac_gain(dac_gain);
#endif
}

