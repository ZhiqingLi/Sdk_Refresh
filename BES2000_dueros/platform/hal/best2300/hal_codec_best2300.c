#include "reg_codec_best2300.h"
#include "hal_codec.h"
#include "hal_cmu.h"
#include "hal_psc.h"
#include "hal_aud.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "analog.h"
#include "cmsis.h"
#include "tgt_hardware.h"
#if !defined(NOSTD) && defined(CODEC_ACCURATE_SW_GAIN)
#include <math.h>
#endif
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_chipid.h"

#define FIXED_CODEC_ADC_VOL

#define NO_DAC_RESET

#define ADC_MUX_WORKAROUND

#define HAL_CODEC_TX_FIFO_TRIGGER_LEVEL     (3)
#define HAL_CODEC_RX_FIFO_TRIGGER_LEVEL     (4)

#define MAX_DIG_DBVAL                       (50)
#define ZERODB_DIG_DBVAL                    (0)
#define MIN_DIG_DBVAL                       (-99)

#ifndef CODEC_DIGMIC_PHASE
#define CODEC_DIGMIC_PHASE                  7
#endif

#define MAX_DIG_MIC_CH_NUM                  5

#define MAX_ADC_CH_NUM                      5

#define VALID_MIC_MAP                       (AUD_CHANNEL_MAP_ALL & ~(AUD_CHANNEL_MAP_CH5 | AUD_CHANNEL_MAP_CH6 | AUD_CHANNEL_MAP_CH7 | \
                                                AUD_CHANNEL_MAP_DIGMIC_CH5 | AUD_CHANNEL_MAP_DIGMIC_CH6 | AUD_CHANNEL_MAP_DIGMIC_CH7))
#define VALID_SPK_MAP                       (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)

#if (CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV & ~VALID_MIC_MAP)
#error "Invalid CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV"
#endif
#if (CFG_HW_AUD_INPUT_PATH_LINEIN_DEV & ~VALID_MIC_MAP)
#error "Invalid CFG_HW_AUD_INPUT_PATH_LINEIN_DEV"
#endif
#if (CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV & ~VALID_SPK_MAP)
#error "Invalid CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV"
#endif

#define RSTN_ADC_FREE_RUNNING_CLK           CODEC_SOFT_RSTN_ADC(1 << MAX_ADC_CH_NUM)

struct CODEC_DAC_DRE_CFG_T {
    uint8_t dre_delay;
    uint8_t thd_db_offset;
    uint8_t step_mode;
    uint32_t dre_win;
    uint16_t amp_high;
};

struct CODEC_DAC_SAMPLE_RATE_T {
    enum AUD_SAMPRATE_T sample_rate;
    uint32_t codec_freq;
    uint8_t codec_div;
    uint8_t cmu_div;
    uint8_t dac_up;
    uint16_t bypass_cnt;
};

static const struct CODEC_DAC_SAMPLE_RATE_T codec_dac_sample_rate[] = {
#ifdef __AUDIO_RESAMPLE__
    {AUD_SAMPRATE_8463,          CODEC_FREQ_26M,               1,             1, 6, 0},
    {AUD_SAMPRATE_16927,         CODEC_FREQ_26M,               1,             1, 3, 0},
    {AUD_SAMPRATE_50781,         CODEC_FREQ_26M,               1,             1, 1, 0},
#endif
    {AUD_SAMPRATE_7350,   CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 6, 0},
    {AUD_SAMPRATE_8000,   CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 6, 0},
    {AUD_SAMPRATE_14700,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 3, 0},
    {AUD_SAMPRATE_16000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 3, 0},
    {AUD_SAMPRATE_22050,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 2, 0},
    {AUD_SAMPRATE_24000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 2, 0},
    {AUD_SAMPRATE_44100,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 0},
    {AUD_SAMPRATE_48000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 0},
    {AUD_SAMPRATE_88200,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 1},
    {AUD_SAMPRATE_96000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 1},
    {AUD_SAMPRATE_176400, CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 2},
    {AUD_SAMPRATE_192000, CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 2},
    {AUD_SAMPRATE_352800, CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 3},
    {AUD_SAMPRATE_384000, CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 3},
};

struct CODEC_ADC_SAMPLE_RATE_T {
    enum AUD_SAMPRATE_T sample_rate;
    uint32_t codec_freq;
    uint8_t codec_div;
    uint8_t cmu_div;
    uint8_t adc_down;
    uint8_t bypass_cnt;
};

static const struct CODEC_ADC_SAMPLE_RATE_T codec_adc_sample_rate[] = {
#ifdef __AUDIO_RESAMPLE__
    {AUD_SAMPRATE_8463,          CODEC_FREQ_26M,               1,             1, 6, 0},
    {AUD_SAMPRATE_16927,         CODEC_FREQ_26M,               1,             1, 3, 0},
    {AUD_SAMPRATE_50781,         CODEC_FREQ_26M,               1,             1, 1, 0},
#endif
    {AUD_SAMPRATE_7350,   CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 6, 0},
    {AUD_SAMPRATE_8000,   CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 6, 0},
    {AUD_SAMPRATE_14700,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 3, 0},
    {AUD_SAMPRATE_16000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 3, 0},
    {AUD_SAMPRATE_44100,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 0},
    {AUD_SAMPRATE_48000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 0},
    {AUD_SAMPRATE_88200,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 1},
    {AUD_SAMPRATE_96000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 1},
    {AUD_SAMPRATE_176400, CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 2},
    {AUD_SAMPRATE_192000, CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 2},
    {AUD_SAMPRATE_352800, CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 3},
    {AUD_SAMPRATE_384000, CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, CODEC_CMU_DIV, 1, 3},
};

const int8_t WEAK codec_adc_vol[TGT_ADC_VOL_LEVEL_QTY] = {
    -99, -12, -10, -8, -6, -4, -2, 0, 2, 4, 6, 8, 10, 12, 14, 16,
};

static struct CODEC_T * const codec = (struct CODEC_T *)CODEC_BASE;

static bool codec_init = false;
static bool codec_opened = false;
static uint16_t digdac_gain;

enum HAL_CODEC_OPEN_T {
    HAL_CODEC_OPEN_NORMAL       = (1 << 0),
    HAL_CODEC_OPEN_ANC_INIT     = (1 << 1),
};

#ifdef ANC_APP
static enum HAL_CODEC_OPEN_T open_type;
static int8_t digdac_gain_offset_anc;
#endif
#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
static bool codec_nr_enabled;
static int8_t digdac_gain_offset_nr;
#endif
#ifdef AUDIO_OUTPUT_DC_CALIB
static int16_t dac_dc_l;
static int16_t dac_dc_r;
static float dac_dc_gain_attn;
#endif
#ifdef __AUDIO_RESAMPLE__
static uint8_t rs_clk_map;
STATIC_ASSERT(sizeof(rs_clk_map) * 8 >= AUD_STREAM_NUM, "rs_clk_map size too small");

static uint8_t resample_rate_idx[AUD_STREAM_NUM];
static float resample_factor[AUD_STREAM_NUM] = { 1.0f, 1.0f, };
#endif

//static HAL_CODEC_DAC_RESET_CALLBACK dac_reset_callback;

static enum AUD_CHANNEL_MAP_T codec_dac_ch_map;
static enum AUD_CHANNEL_MAP_T codec_adc_ch_map;
static enum AUD_CHANNEL_MAP_T anc_adc_ch_map;
static enum AUD_CHANNEL_MAP_T codec_mic_ch_map;
static enum AUD_CHANNEL_MAP_T anc_mic_ch_map;

static uint8_t dac_delay_ms;

#if defined(AUDIO_ANC_FB_MC) && defined(__AUDIO_RESAMPLE__)
#error "Music cancel cannot work with audio resample"
#endif
#if defined(AUDIO_ANC_FB_MC) && defined(__TWS__)
#error "Music cancel cannot work with TWS")
#endif
#ifdef AUDIO_ANC_FB_MC
static bool mc_enabled;
static bool mc_dual_chan;
static bool mc_16bit;
static bool adc_enabled;
#endif

#ifdef PERF_TEST_POWER_KEY
static enum HAL_CODEC_PERF_TEST_POWER_T cur_perft_power;
#endif

#ifdef AUDIO_OUTPUT_SW_GAIN
static HAL_CODEC_SW_OUTPUT_COEF_CALLBACK sw_output_coef_callback;
#endif

#if defined(DAC_CLASSG_ENABLE)
static struct dac_classg_cfg _dac_classg_cfg = {
    .thd2 = 0xC0,
    .thd1 = 0x10,
    .thd0 = 0x10,
    .lr = 1,
    .step_4_3n = 0,
    .quick_down = 1,
    .wind_width = 0x400,
};
#endif

#ifdef DAC_DRE_ENABLE
static const struct CODEC_DAC_DRE_CFG_T dac_dre_cfg = {
    .dre_delay = 8,
    .thd_db_offset = 5,
    .step_mode = 0,
    .dre_win = 0x6000,
    .amp_high = 0x10,
};
#endif

static void hal_codec_set_dig_adc_gain(enum AUD_CHANNEL_MAP_T map, int32_t gain);
static const struct CODEC_DAC_VOL_T *hal_codec_get_dac_volume(uint32_t index);
static void hal_codec_set_dig_dac_gain(int32_t gain);

static void hal_codec_reg_update_delay(void)
{
    for (int i = 0; i < 10; i++) {
        asm volatile("nop");
    }
}

#if defined(DAC_CLASSG_ENABLE)
void hal_codec_classg_config(const struct dac_classg_cfg *cfg)
{
    _dac_classg_cfg = *cfg;
}

static void hal_codec_classg_enable(bool en)
{
    struct dac_classg_cfg *config;

    if (en) {
        config = &_dac_classg_cfg;

        codec->REG_0B4 = SET_BITFIELD(codec->REG_0B4, CODEC_CODEC_CLASSG_THD2, config->thd2);
        codec->REG_0B4 = SET_BITFIELD(codec->REG_0B4, CODEC_CODEC_CLASSG_THD1, config->thd1);
        codec->REG_0B4 = SET_BITFIELD(codec->REG_0B4, CODEC_CODEC_CLASSG_THD0, config->thd0);

        codec->REG_0B0 = SET_BITFIELD(codec->REG_0B0, CODEC_CODEC_CLASSG_WINDOW, config->wind_width);

        if (config->lr)
            codec->REG_0B0 |= CODEC_CODEC_CLASSG_LR;
        else
            codec->REG_0B0 &= ~CODEC_CODEC_CLASSG_LR;

        if (config->step_4_3n)
            codec->REG_0B0 |= CODEC_CODEC_CLASSG_STEP_3_4N;
        else
            codec->REG_0B0 &= ~CODEC_CODEC_CLASSG_STEP_3_4N;

        if (config->quick_down)
            codec->REG_0B0 |= CODEC_CODEC_CLASSG_QUICK_DOWN;
        else
            codec->REG_0B0 &= ~CODEC_CODEC_CLASSG_QUICK_DOWN;

        codec->REG_0B0 |= CODEC_CODEC_CLASSG_QUICK_DOWN;
    } else {
        codec->REG_0B0 &= ~CODEC_CODEC_CLASSG_QUICK_DOWN;
    }
}
#endif

#ifdef AUDIO_OUTPUT_DC_CALIB
static void hal_codec_dac_dc_offset_enable(void)
{
    codec->REG_0E0 &= CODEC_CODEC_DAC_DC_UPDATE_CH0;
    hal_codec_reg_update_delay();
    codec->REG_0E8 = SET_BITFIELD(codec->REG_0E8, CODEC_CODEC_DAC_DC_CH0, dac_dc_r);
    codec->REG_0E0 = SET_BITFIELD(codec->REG_0E0, CODEC_CODEC_DAC_DC_CH0, dac_dc_l) | CODEC_CODEC_DAC_DC_UPDATE_CH0;
}
#endif

static int hal_codec_int_open(enum HAL_CODEC_OPEN_T type)
{
    int i;

#ifdef ANC_APP
    enum HAL_CODEC_OPEN_T old_type;

    old_type = open_type;
    open_type |= type;
    if (old_type) {
        return 0;
    }
#endif

    analog_aud_pll_open(ANA_AUD_PLL_USER_CODEC);

    if (!codec_init) {
        // It will power down some modules first
        hal_psc_codec_enable();
        codec_init = true;
    }
    hal_cmu_codec_clock_enable();
    hal_cmu_codec_reset_clear();

    codec_opened = true;

    codec->REG_060 |= CODEC_EN_CLK_DAC | CODEC_EN_CLK_ADC_MASK | CODEC_EN_CLK_ADC_ANA_MASK | CODEC_POL_ADC_ANA_MASK;
    codec->REG_064 |= CODEC_SOFT_RSTN_32K | CODEC_SOFT_RSTN_IIR | CODEC_SOFT_RSTN_RS |
        CODEC_SOFT_RSTN_DAC | CODEC_SOFT_RSTN_ADC_MASK | CODEC_SOFT_RSTN_ADC_ANA_MASK;
    codec->REG_000 &= ~(CODEC_DMACTRL_TX | CODEC_DMACTRL_RX | CODEC_DAC_ENABLE |
        CODEC_ADC_ENABLE_CH0 | CODEC_ADC_ENABLE_CH1 | CODEC_ADC_ENABLE_CH2 | CODEC_ADC_ENABLE_CH3 |
        CODEC_ADC_ENABLE_CH4 | CODEC_ADC_ENABLE | CODEC_CODEC_IF_EN);
    codec->REG_04C &= ~CODEC_MC_ENABLE;
    codec->REG_004 |= CODEC_RX_FIFO_FLUSH_CH0 | CODEC_RX_FIFO_FLUSH_CH1 | CODEC_RX_FIFO_FLUSH_CH2 |
        CODEC_RX_FIFO_FLUSH_CH3 | CODEC_RX_FIFO_FLUSH_CH4 | CODEC_TX_FIFO_FLUSH | CODEC_TX_FIFO_FLUSH |
        CODEC_DSD_TX_FIFO_FLUSH | CODEC_MC_FIFO_FLUSH | CODEC_IIR_RX_FIFO_FLUSH | CODEC_IIR_TX_FIFO_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_004 &= ~(CODEC_RX_FIFO_FLUSH_CH0 | CODEC_RX_FIFO_FLUSH_CH1 | CODEC_RX_FIFO_FLUSH_CH2 |
        CODEC_RX_FIFO_FLUSH_CH3 | CODEC_RX_FIFO_FLUSH_CH4 | CODEC_TX_FIFO_FLUSH | CODEC_TX_FIFO_FLUSH |
        CODEC_DSD_TX_FIFO_FLUSH | CODEC_MC_FIFO_FLUSH | CODEC_IIR_RX_FIFO_FLUSH | CODEC_IIR_TX_FIFO_FLUSH);
    codec->REG_000 |= CODEC_CODEC_IF_EN;

#ifdef AUDIO_ANC_FB_MC
    codec->REG_04C = CODEC_MC_DELAY(250) | CODEC_DMA_CTRL_MC;
#endif

    // ANC zero-crossing
    codec->REG_0D4 |= CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH0 | CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH1;
    codec->REG_0D8 |= CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH0 | CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH1;

    // Enable ADC zero-crossing gain adjustment
    for (i = 0; i < MAX_ADC_CH_NUM; i++) {
        *(&codec->REG_084 + i) |= CODEC_CODEC_ADC_GAIN_SEL_CH0;
    }

    // DRE ini gain
    uint32_t dre_offset;
    dre_offset = analog_aud_get_max_dre_gain();
    if (dre_offset > 0xF) {
        dre_offset -= 0xF;
    } else {
        dre_offset = 0;
    }
    codec->REG_0C0 = CODEC_CODEC_DRE_INI_ANA_GAIN_CH0(0) | CODEC_CODEC_DRE_GAIN_OFFSET_CH0(dre_offset);
    codec->REG_0C8 = CODEC_CODEC_DRE_INI_ANA_GAIN_CH1(0) | CODEC_CODEC_DRE_GAIN_OFFSET_CH1(dre_offset);
    codec->REG_0E0 = CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH0(0);
    codec->REG_0E8 = CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH1(0);

#ifdef AUDIO_ANC_FB_MC
    // Enable ADC + music cancel.
    codec->REG_130 |= CODEC_CODEC_FB_CHECK_KEEP_SEL_CH0;
    codec->REG_134 |= CODEC_CODEC_FB_CHECK_KEEP_SEL_CH1;
#endif

#ifdef FIXED_CODEC_ADC_VOL
    if (CODEC_SADC_VOL < ARRAY_SIZE(codec_adc_vol)) {
        hal_codec_set_dig_adc_gain(0xFF, codec_adc_vol[CODEC_SADC_VOL]);
    }
#endif

#ifdef AUDIO_OUTPUT_DC_CALIB
    hal_codec_dac_dc_offset_enable();
#endif

#ifdef AUDIO_OUTPUT_SW_GAIN
    const struct CODEC_DAC_VOL_T *vol_tab_ptr;

    // Init gain settings
    vol_tab_ptr = hal_codec_get_dac_volume(0);
    if (vol_tab_ptr) {
        analog_aud_set_dac_gain(vol_tab_ptr->tx_pa_gain);
        hal_codec_set_dig_dac_gain(ZERODB_DIG_DBVAL);
    }
#else
    // Enable DAC zero-crossing gain adjustment
    codec->REG_09C |= CODEC_CODEC_DAC_GAIN_SEL_CH0;
    codec->REG_0A0 |= CODEC_CODEC_DAC_GAIN_SEL_CH1;
#endif

#ifdef __AUDIO_RESAMPLE__
    codec->REG_0E4 &= ~(CODEC_CODEC_RESAMPLE_DAC_ENABLE | CODEC_CODEC_RESAMPLE_ADC_ENABLE |
        CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE | CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE);
#endif
    if (hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_1) 
    {
        codec->REG_098 |= CODEC_CODEC_DAC_USE_HBF4;           
    }
    return 0;
}

int hal_codec_open(enum HAL_CODEC_ID_T id)
{
    return hal_codec_int_open(HAL_CODEC_OPEN_NORMAL);
}

int hal_codec_int_close(enum HAL_CODEC_OPEN_T type)
{
#ifdef ANC_APP
    open_type &= ~type;
    if (open_type) {
        return 0;
    }
#endif

    codec->REG_000 &= ~CODEC_CODEC_IF_EN;
    codec->REG_064 &= ~(CODEC_SOFT_RSTN_32K | CODEC_SOFT_RSTN_IIR | CODEC_SOFT_RSTN_RS |
        CODEC_SOFT_RSTN_DAC | CODEC_SOFT_RSTN_ADC_MASK | CODEC_SOFT_RSTN_ADC_ANA_MASK);
    codec->REG_060 &= ~(CODEC_EN_CLK_DAC | CODEC_EN_CLK_ADC_MASK | CODEC_EN_CLK_ADC_ANA_MASK | CODEC_POL_ADC_ANA_MASK);

    codec_opened = false;

    // NEVER reset or power down CODEC registers, for the CODEC driver expects that last configurations
    // still exist in the next stream setup
    //hal_cmu_codec_reset_set();
    hal_cmu_codec_clock_disable();
    //hal_psc_codec_disable();

    analog_aud_pll_close(ANA_AUD_PLL_USER_CODEC);

    return 0;
}

int hal_codec_close(enum HAL_CODEC_ID_T id)
{
    return hal_codec_int_close(HAL_CODEC_OPEN_NORMAL);
}

#ifdef DAC_DRE_ENABLE
static void hal_codec_dac_dre_enable(void)
{
    codec->REG_0C0 = (codec->REG_0C0 & ~(CODEC_CODEC_DRE_THD_DB_OFFSET_SIGN_CH0 | CODEC_CODEC_DRE_DELAY_CH0_MASK |
            CODEC_CODEC_DRE_INI_ANA_GAIN_CH0_MASK | CODEC_CODEC_DRE_THD_DB_OFFSET_CH0_MASK | CODEC_CODEC_DRE_STEP_MODE_CH0_MASK)) |
        CODEC_CODEC_DRE_THD_DB_OFFSET_SIGN_CH0 | DRE_DAC_DRE_DELAY(dac_dre_cfg.dre_delay) |
        CODEC_CODEC_DRE_INI_ANA_GAIN_CH0(0) | CODEC_CODEC_DRE_THD_DB_OFFSET_CH0(dac_dre_cfg.thd_db_offset) |
        CODEC_CODEC_DRE_THD_DB_OFFSET_CH0(dac_dre_cfg.step_mode) | CODEC_CODEC_DRE_ENABLE_CH0;
    codec->REG_0C4 = (codec->REG_0C4 & ~(CODEC_CODEC_DRE_WINDOW_CH0_MASK | CODEC_CODEC_DRE_AMP_HIGH_CH0_MASK)) |
        CODEC_CODEC_DRE_WINDOW_CH0(dac_dre_cfg.dre_win) | CODEC_CODEC_DRE_AMP_HIGH_CH0(dac_dre_cfg.amp_high);

    codec->REG_0C8 = (codec->REG_0C8 & ~(CODEC_CODEC_DRE_THD_DB_OFFSET_SIGN_CH1 | CODEC_CODEC_DRE_DELAY_CH1_MASK |
            CODEC_CODEC_DRE_INI_ANA_GAIN_CH1_MASK | CODEC_CODEC_DRE_THD_DB_OFFSET_CH1_MASK | CODEC_CODEC_DRE_STEP_MODE_CH1_MASK)) |
        CODEC_CODEC_DRE_THD_DB_OFFSET_SIGN_CH1 | DRE_DAC_DRE_DELAY(dac_dre_cfg.dre_delay) |
        CODEC_CODEC_DRE_INI_ANA_GAIN_CH1(0) | CODEC_CODEC_DRE_THD_DB_OFFSET_CH1(dac_dre_cfg.thd_db_offset) |
        CODEC_CODEC_DRE_THD_DB_OFFSET_CH1(dac_dre_cfg.step_mode) | CODEC_CODEC_DRE_ENABLE_CH1;
    codec->REG_0CC = (codec->REG_0CC & ~(CODEC_CODEC_DRE_WINDOW_CH1_MASK | CODEC_CODEC_DRE_AMP_HIGH_CH1_MASK)) |
        CODEC_CODEC_DRE_WINDOW_CH1(dac_dre_cfg.dre_win) | CODEC_CODEC_DRE_AMP_HIGH_CH1(dac_dre_cfg.amp_high);
}

static void hal_codec_dac_dre_disable(void)
{
    codec->REG_0C0 &= ~CODEC_CODEC_DRE_ENABLE_CH0;
    codec->REG_0C8 &= ~CODEC_CODEC_DRE_ENABLE_CH1;
}
#endif

#ifdef PERF_TEST_POWER_KEY
static void hal_codec_update_perf_test_power(void)
{
    int32_t nominal_vol;
    uint32_t ini_ana_gain;
    int32_t dac_vol;

    if (!codec_opened) {
        return;
    }

    dac_vol = 0;
    if (cur_perft_power == HAL_CODEC_PERF_TEST_30MW) {
        nominal_vol = 0;
        ini_ana_gain = 0;
    } else if (cur_perft_power == HAL_CODEC_PERF_TEST_10MW) {
        nominal_vol = -5;
        ini_ana_gain = 7;
    } else if (cur_perft_power == HAL_CODEC_PERF_TEST_5MW) {
        nominal_vol = -8;
        ini_ana_gain = 0xA;
    } else if (cur_perft_power == HAL_CODEC_PERF_TEST_M60DB) {
        nominal_vol = -60;
        ini_ana_gain = 0xF; // about -11 dB
        dac_vol = -49;
    } else {
        return;
    }

    if (codec->REG_0C0 & CODEC_CODEC_DRE_ENABLE_CH0) {
        dac_vol = nominal_vol;
    } else {
        codec->REG_0C0 = SET_BITFIELD(codec->REG_0C0, CODEC_CODEC_DRE_INI_ANA_GAIN_CH0, ini_ana_gain);
        codec->REG_0C8 = SET_BITFIELD(codec->REG_0C8, CODEC_CODEC_DRE_INI_ANA_GAIN_CH1, ini_ana_gain);
    }

#ifdef AUDIO_OUTPUT_SW_GAIN
    hal_codec_set_sw_gain(dac_vol);
#else
    hal_codec_set_dig_dac_gain(dac_vol);
#endif

#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
    if (codec_nr_enabled) {
        codec_nr_enabled = false;
        hal_codec_set_noise_reduction(true);
    }
#endif
}

void hal_codec_dac_gain_m60db_check(enum HAL_CODEC_PERF_TEST_POWER_T type)
{
    cur_perft_power = type;

    if (!codec_opened || (codec->REG_098 & CODEC_CODEC_DAC_EN) == 0) {
        return;
    }

    hal_codec_update_perf_test_power();
}
#endif

#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
void hal_codec_set_noise_reduction(bool enable)
{
    uint32_t ini_ana_gain;

    if (codec_nr_enabled == enable) {
        // Avoid corrupting digdac_gain_offset_nr or using an invalid one
        return;
    }

    codec_nr_enabled = enable;

    if (!codec_opened) {
        return;
    }

    // ini_ana_gain=0   -->   0dB
    // ini_ana_gain=0xF --> -11dB
    if (enable) {
        ini_ana_gain = GET_BITFIELD(codec->REG_0C0, CODEC_CODEC_DRE_INI_ANA_GAIN_CH0);
        digdac_gain_offset_nr = ((0xF - ini_ana_gain) * 11 + 0xF / 2) / 0xF;
        ini_ana_gain = 0xF;
    } else {
        ini_ana_gain = 0xF - (digdac_gain_offset_nr * 0xF + 11 / 2) / 11;
        digdac_gain_offset_nr = 0;
    }

    codec->REG_0C0 = SET_BITFIELD(codec->REG_0C0, CODEC_CODEC_DRE_INI_ANA_GAIN_CH0, ini_ana_gain);
    codec->REG_0C8 = SET_BITFIELD(codec->REG_0C8, CODEC_CODEC_DRE_INI_ANA_GAIN_CH1, ini_ana_gain);

#ifdef AUDIO_OUTPUT_SW_GAIN
    hal_codec_set_sw_gain(digdac_gain);
#else
    hal_codec_set_dig_dac_gain(digdac_gain);
#endif
}
#endif

void hal_codec_stop_playback_stream(enum HAL_CODEC_ID_T id)
{
#if (defined(AUDIO_OUTPUT_DC_CALIB_ANA) || defined(AUDIO_OUTPUT_DC_CALIB)) && (!defined(__TWS__) || defined(__BT_ANC__))
    // Disable PA
    analog_aud_codec_speaker_enable(false);
#endif

    codec->REG_098 &= ~(CODEC_CODEC_DAC_EN | CODEC_CODEC_DAC_EN_CH0 | CODEC_CODEC_DAC_EN_CH1);

#ifdef DAC_DRE_ENABLE
    hal_codec_dac_dre_disable();
#endif

#if defined(DAC_CLASSG_ENABLE)
    hal_codec_classg_enable(false);
#endif

#ifndef NO_DAC_RESET
    // Reset DAC
    // Avoid DAC outputing noise after it is disabled
    codec->REG_064 &= ~CODEC_SOFT_RSTN_DAC;
    codec->REG_064 |= CODEC_SOFT_RSTN_DAC;
#endif
}

void hal_codec_start_playback_stream(enum HAL_CODEC_ID_T id)
{
#ifndef NO_DAC_RESET
    // Reset DAC
    codec->REG_064 &= ~CODEC_SOFT_RSTN_DAC;
    codec->REG_064 |= CODEC_SOFT_RSTN_DAC;
#endif

#ifdef DAC_DRE_ENABLE
    if ((codec->REG_044 & CODEC_MODE_16BIT_DAC) == 0
#ifdef ANC_APP
            && anc_adc_ch_map == 0
#endif
            )
    {
        hal_codec_dac_dre_enable();
    }
#endif

#ifdef PERF_TEST_POWER_KEY
    hal_codec_update_perf_test_power();
#endif

#if defined(DAC_CLASSG_ENABLE)
    hal_codec_classg_enable(true);
#endif

    if (codec_dac_ch_map & AUD_CHANNEL_MAP_CH0) {
        codec->REG_098 |= CODEC_CODEC_DAC_EN_CH0;
    } else {
        codec->REG_098 &= ~CODEC_CODEC_DAC_EN_CH0;
    }
    if (codec_dac_ch_map & AUD_CHANNEL_MAP_CH1) {
        codec->REG_098 |= CODEC_CODEC_DAC_EN_CH1;
    } else {
        codec->REG_098 &= ~CODEC_CODEC_DAC_EN_CH1;
    }

#if (defined(AUDIO_OUTPUT_DC_CALIB_ANA) || defined(AUDIO_OUTPUT_DC_CALIB)) && (!defined(__TWS__) || defined(__BT_ANC__))
#if 0
    uint32_t cfg_en;
    uint32_t anc_ff_gain, anc_fb_gain;

    cfg_en = codec->REG_000 & CODEC_DAC_ENABLE;
    anc_ff_gain = codec->REG_0D4;
    anc_fb_gain = codec->REG_0D8;

    if (cfg_en) {
        codec->REG_000 &= ~cfg_en;
    }
    if (anc_ff_gain) {
        codec->REG_0D4 = CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH0 | CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH1;
        anc_ff_gain |= CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH0 | CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH1;
    }
    if (anc_fb_gain) {
        codec->REG_0D8 = CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH0 | CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH1;
        anc_fb_gain = CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH0 | CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH1;
    }
    osDelay(1);
#endif
#endif

    codec->REG_098 |= CODEC_CODEC_DAC_EN;

#if (defined(AUDIO_OUTPUT_DC_CALIB_ANA) || defined(AUDIO_OUTPUT_DC_CALIB)) && (!defined(__TWS__) || defined(__BT_ANC__))
#if 0
    // At least delay 4ms for 8K-sample-rate mute data to arrive at DAC PA
    osDelay(5);

    if (cfg_en) {
        codec->REG_000 |= cfg_en;
    }
    if (anc_ff_gain) {
        codec->REG_0D4 = anc_ff_gain;
    }
    if (anc_fb_gain) {
        codec->REG_0D8 = anc_fb_gain;
    }
#endif

    // Enable PA
    analog_aud_codec_speaker_enable(true);

#ifdef AUDIO_ANC_FB_MC
    if (mc_enabled) {
        uint32_t lock;
        lock = int_lock();
        // MC FIFO and DAC FIFO must be started at the same time
        codec->REG_04C |= CODEC_MC_ENABLE;
        codec->REG_000 |= CODEC_DAC_ENABLE;
        int_unlock(lock);
    }
#endif
#endif
}

int hal_codec_start_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream)
{
    if (stream == AUD_STREAM_PLAYBACK) {
        // Reset and start DAC
        hal_codec_start_playback_stream(id);
    } else {
#ifdef AUDIO_ANC_FB_MC
        adc_enabled = true;
        if (!mc_enabled)
#endif
        {
            // Reset ADC free running clock and ADC ANA
            codec->REG_064 &= ~(RSTN_ADC_FREE_RUNNING_CLK | CODEC_SOFT_RSTN_ADC_ANA_MASK);
            codec->REG_064 |= (RSTN_ADC_FREE_RUNNING_CLK | CODEC_SOFT_RSTN_ADC_ANA_MASK);
            codec->REG_080 |= CODEC_CODEC_ADC_EN;
        }
    }

    return 0;
}

int hal_codec_stop_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream)
{
    if (stream == AUD_STREAM_PLAYBACK) {
        // Stop and reset DAC
        hal_codec_stop_playback_stream(id);
    } else {
#ifdef AUDIO_ANC_FB_MC
        adc_enabled = false;
        if (!mc_enabled)
#endif
        {
            codec->REG_080 &= ~CODEC_CODEC_ADC_EN;
        }
    }

    return 0;
}

#ifdef __AUDIO_RESAMPLE__
void hal_codec_resample_clock_enable(enum AUD_STREAM_T stream)
{
    if (rs_clk_map == 0) {
        // 192K-24BIT requires 52M clock
        hal_cmu_codec_rs_enable(52000000);
    }
    rs_clk_map |= (1 << stream);
}

void hal_codec_resample_clock_disable(enum AUD_STREAM_T stream)
{
    if (rs_clk_map == 0) {
        return;
    }
    rs_clk_map &= ~(1 << stream);
    if (rs_clk_map == 0) {
        hal_cmu_codec_rs_disable();
    }
}
#endif

int hal_codec_start_interface(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream, int dma)
{
    uint32_t fifo_flush = 0;

    if (stream == AUD_STREAM_PLAYBACK) {
#ifdef __AUDIO_RESAMPLE__
        if (codec->REG_0E4 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE) {
            codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_ENABLE;
            hal_codec_resample_clock_enable(stream);
        }
#endif
#ifdef AUDIO_ANC_FB_MC
        fifo_flush |= CODEC_MC_FIFO_FLUSH;
#endif
        fifo_flush |= CODEC_TX_FIFO_FLUSH;
        codec->REG_004 |= fifo_flush;
        hal_codec_reg_update_delay();
        codec->REG_004 &= ~fifo_flush;
        if (dma) {
            codec->REG_008 = SET_BITFIELD(codec->REG_008, CODEC_CODEC_TX_THRESHOLD, HAL_CODEC_TX_FIFO_TRIGGER_LEVEL);
            codec->REG_000 |= CODEC_DMACTRL_TX;
            // Delay a little time for DMA to fill the TX FIFO before sending
            for (volatile int i = 0; i < 50; i++);
        }
#ifdef AUDIO_ANC_FB_MC
        if (mc_dual_chan) {
            codec->REG_04C |= CODEC_DUAL_CHANNEL_MC;
        } else {
            codec->REG_04C &= ~CODEC_DUAL_CHANNEL_MC;
        }
        if (mc_16bit) {
            codec->REG_04C |= CODEC_MODE_16BIT_MC;
        } else {
            codec->REG_04C &= ~CODEC_MODE_16BIT_MC;
        }
        if (!adc_enabled) {
            // Reset ADC free running clock and ADC ANA
            codec->REG_064 &= ~(RSTN_ADC_FREE_RUNNING_CLK | CODEC_SOFT_RSTN_ADC_ANA_MASK);
            codec->REG_064 |= (RSTN_ADC_FREE_RUNNING_CLK | CODEC_SOFT_RSTN_ADC_ANA_MASK);
            codec->REG_080 |= CODEC_CODEC_ADC_EN;
        }
        // If codec function has been enabled, start FIFOs directly;
        // otherwise, start FIFOs after PA is enabled
        if (codec->REG_098 & CODEC_CODEC_DAC_EN) {
            uint32_t lock;
            lock = int_lock();
            // MC FIFO and DAC FIFO must be started at the same time
            codec->REG_04C |= CODEC_MC_ENABLE;
            codec->REG_000 |= CODEC_DAC_ENABLE;
            int_unlock(lock);
        }
        mc_enabled = true;
#else
        codec->REG_000 |= CODEC_DAC_ENABLE;
#endif
    } else {
#ifdef __AUDIO_RESAMPLE__
        if (codec->REG_0E4 & CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE) {
            codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_ADC_ENABLE;
            hal_codec_resample_clock_enable(stream);
        }
#endif
        for (int i = 0; i < MAX_ADC_CH_NUM; i++) {
            if (codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
                if ((codec->REG_080 & (CODEC_CODEC_ADC_EN_CH0 << i)) == 0) {
                    // Reset ADC channel
                    codec->REG_064 &= ~CODEC_SOFT_RSTN_ADC(1 << i);
                    codec->REG_064 |= CODEC_SOFT_RSTN_ADC(1 << i);
                    codec->REG_080 |= (CODEC_CODEC_ADC_EN_CH0 << i);
                }
                codec->REG_000 |= (CODEC_ADC_ENABLE_CH0 << i);
            }
#ifdef ADC_MUX_WORKAROUND
            if (codec_mic_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
                if ((codec->REG_080 & (CODEC_CODEC_ADC_EN_CH0 << i)) == 0) {
                    // Reset ADC channel
                    codec->REG_064 &= ~CODEC_SOFT_RSTN_ADC(1 << i);
                    codec->REG_064 |= CODEC_SOFT_RSTN_ADC(1 << i);
                    codec->REG_080 |= (CODEC_CODEC_ADC_EN_CH0 << i);
                }
            }
#endif
        }
        fifo_flush |= CODEC_RX_FIFO_FLUSH_CH0 | CODEC_RX_FIFO_FLUSH_CH1 | CODEC_RX_FIFO_FLUSH_CH2 | CODEC_RX_FIFO_FLUSH_CH3 | CODEC_RX_FIFO_FLUSH_CH4;
        codec->REG_004 |= fifo_flush;
        hal_codec_reg_update_delay();
        codec->REG_004 &= ~fifo_flush;
        if (dma) {
            codec->REG_008 = SET_BITFIELD(codec->REG_008, CODEC_CODEC_RX_THRESHOLD, HAL_CODEC_RX_FIFO_TRIGGER_LEVEL);
            codec->REG_000 |= CODEC_DMACTRL_RX;
        }
        codec->REG_000 |= CODEC_ADC_ENABLE;
    }

    return 0;
}

int hal_codec_stop_interface(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream)
{
    uint32_t fifo_flush = 0;

    if (stream == AUD_STREAM_PLAYBACK) {
#ifdef AUDIO_ANC_FB_MC
        mc_enabled = false;
        codec->REG_04C &= ~CODEC_MC_ENABLE;
        if (!adc_enabled) {
            codec->REG_080 &= ~CODEC_CODEC_ADC_EN;
        }
        fifo_flush |= CODEC_MC_FIFO_FLUSH;
#endif
#ifdef __AUDIO_RESAMPLE__
        codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_ENABLE;
        hal_codec_resample_clock_disable(stream);
#endif
        codec->REG_000 &= ~CODEC_DAC_ENABLE;
        codec->REG_000 &= ~CODEC_DMACTRL_TX;
        fifo_flush |= CODEC_TX_FIFO_FLUSH;
        codec->REG_004 |= fifo_flush;
        hal_codec_reg_update_delay();
        codec->REG_004 &= ~fifo_flush;
        // Cancel dac sync request
        hal_codec_sync_dac_disable();
#ifdef NO_DAC_RESET
        // Clean up DAC intermediate states
        osDelay(dac_delay_ms);
#endif
    } else {
#ifdef __AUDIO_RESAMPLE__
        codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_ADC_ENABLE;
        hal_codec_resample_clock_disable(stream);
#endif
        codec->REG_000 &= ~(CODEC_ADC_ENABLE | CODEC_ADC_ENABLE_CH0 | CODEC_ADC_ENABLE_CH1 | CODEC_ADC_ENABLE_CH2 | CODEC_ADC_ENABLE_CH3 | CODEC_ADC_ENABLE_CH4);
        codec->REG_000 &= ~CODEC_DMACTRL_RX;
        for (int i = 0; i < MAX_ADC_CH_NUM; i++) {
#ifdef ADC_MUX_WORKAROUND
            if (((codec_adc_ch_map | codec_mic_ch_map) & (AUD_CHANNEL_MAP_CH0 << i)) &&
                    ((anc_adc_ch_map | anc_mic_ch_map) & (AUD_CHANNEL_MAP_CH0 << i)) == 0) {
                codec->REG_080 &= ~(CODEC_CODEC_ADC_EN_CH0 << i);
            }
#else
            if ((codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) &&
                    (anc_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) == 0) {
                codec->REG_080 &= ~(CODEC_CODEC_ADC_EN_CH0 << i);
            }
#endif
        }
        fifo_flush |= CODEC_RX_FIFO_FLUSH_CH0 | CODEC_RX_FIFO_FLUSH_CH1 | CODEC_RX_FIFO_FLUSH_CH2 | CODEC_RX_FIFO_FLUSH_CH3 | CODEC_RX_FIFO_FLUSH_CH4;
        codec->REG_004 |= fifo_flush;
        hal_codec_reg_update_delay();
        codec->REG_004 &= ~fifo_flush;
        // Cancel adc sync request
        hal_codec_sync_adc_disable();
    }

    return 0;
}

static void hal_codec_set_dac_gain_value(uint32_t val)
{
    codec->REG_09C &= ~CODEC_CODEC_DAC_GAIN_UPDATE;
    hal_codec_reg_update_delay();
    codec->REG_09C = SET_BITFIELD(codec->REG_09C, CODEC_CODEC_DAC_GAIN_CH0, val);
    codec->REG_0A0 = SET_BITFIELD(codec->REG_0A0, CODEC_CODEC_DAC_GAIN_CH1, val);
    codec->REG_09C |= CODEC_CODEC_DAC_GAIN_UPDATE;
}

void hal_codec_get_dac_gain(float *left_gain,float *right_gain)
{
    uint32_t left_val;
    uint32_t right_val;

    left_val  = GET_BITFIELD(codec->REG_09C, CODEC_CODEC_DAC_GAIN_CH0);
    right_val = GET_BITFIELD(codec->REG_0A0, CODEC_CODEC_DAC_GAIN_CH1);

    *left_gain = left_val;
    *right_gain = right_val;

    // Gain format: 6.14
    *left_gain /= (1 << 14);
    *right_gain /= (1 << 14);
}

void hal_codec_dac_mute(enum HAL_CODEC_ID_T id)
{
    hal_codec_set_dac_gain_value(0);
}

static const struct CODEC_DAC_VOL_T *hal_codec_get_dac_volume(uint32_t index)
{
    if (index < ARRAY_SIZE(codec_dac_vol)) {
        return &codec_dac_vol[index];
    } else {
        return NULL;
    }
}

static float db_to_amplitude_ratio(int32_t db)
{
    float coef;

    if (db == ZERODB_DIG_DBVAL) {
        coef = 1;
    } else if (db <= MIN_DIG_DBVAL) {
        coef = 0;
    } else {
        if (db > MAX_DIG_DBVAL) {
            db = MAX_DIG_DBVAL;
        }
#if !defined(NOSTD) && defined(CODEC_ACCURATE_SW_GAIN)
        // The math lib will consume 4K+ bytes of space
        coef = (float)pow(10, (float)db / 20);
#else
        static const float factor_m9db = 0.35481339;
        static const float factor_m3db = 0.70794578;
        static const float factor_m1db = 0.89125094;
        uint32_t cnt;
        uint32_t remain;
        int i;

        coef = 1;

        if (db < 0) {
            cnt = -db / 9;
            remain = -db % 9;
            for (i = 0; i < cnt; i++) {
                coef *= factor_m9db;
            }
            cnt = remain / 3;
            remain = remain % 3;
            for (i = 0; i < cnt; i++) {
                coef *= factor_m3db;
            }
            for (i = 0; i < remain; i++) {
                coef *= factor_m1db;
            }
        } else if (db > 0) {
            cnt = db / 9;
            remain = db % 9;
            for (i = 0; i < cnt; i++) {
                coef /= factor_m9db;
            }
            cnt = remain / 3;
            remain = remain % 3;
            for (i = 0; i < cnt; i++) {
                coef /= factor_m3db;
            }
            for (i = 0; i < remain; i++) {
                coef /= factor_m1db;
            }
        }
#endif
    }

    return coef;
}

static void hal_codec_set_dig_dac_gain(int32_t gain)
{
    uint32_t val;
    float coef;

#ifdef TGT_DIG_DAC_GAIN_OFFSET
    gain += TGT_DIG_DAC_GAIN_OFFSET;
#endif

    digdac_gain = gain;

#ifdef ANC_APP
    gain += digdac_gain_offset_anc;
#endif
#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
    gain += digdac_gain_offset_nr;
#endif

    coef = db_to_amplitude_ratio(gain);

#ifdef AUDIO_OUTPUT_DC_CALIB
    coef *= dac_dc_gain_attn;
#endif

#if 0
    static const float thd_attn = 0.982878873; // -0.15dB

    // Ensure that THD is good at max gain
    if (coef > thd_attn) {
        coef = thd_attn;
    }
#endif

    // Gain format: 6.14
    val = (uint32_t)(coef * (1 << 14));
    val = __USAT(val, 20);
    hal_codec_set_dac_gain_value(val);
}

#ifdef AUDIO_OUTPUT_SW_GAIN
static void hal_codec_set_sw_gain(int32_t gain)
{
    float coef;

#ifdef TGT_DIG_DAC_GAIN_OFFSET
    gain += TGT_DIG_DAC_GAIN_OFFSET;
#endif

    digdac_gain = gain;

#ifdef ANC_APP
    gain += digdac_gain_offset_anc;
#endif
#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
    gain += digdac_gain_offset_nr;
#endif

    if (sw_output_coef_callback == NULL) {
        return;
    }

    coef = db_to_amplitude_ratio(gain);

#ifdef AUDIO_OUTPUT_DC_CALIB
    coef *= dac_dc_gain_attn;
#endif

#if 0
    static const float thd_attn = 0.982878873; // -0.15dB

    // Ensure that THD is good at max gain
    if (coef > thd_attn) {
        coef = thd_attn;
    }
#endif

    sw_output_coef_callback(coef);
}

void hal_codec_set_sw_output_coef_callback(HAL_CODEC_SW_OUTPUT_COEF_CALLBACK callback)
{
    sw_output_coef_callback = callback;
}
#endif

static void hal_codec_set_adc_gain_value(enum AUD_CHANNEL_MAP_T map, uint32_t val)
{
    uint32_t gain_update = 0;

    for (int i = 0; i < MAX_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_GAIN_CH0, val);
            gain_update |= (CODEC_CODEC_ADC_GAIN_UPDATE_CH0 << i);
        }
    }
    codec->REG_09C &= ~gain_update;
    hal_codec_reg_update_delay();
    codec->REG_09C |= gain_update;
}

static void hal_codec_set_dig_adc_gain(enum AUD_CHANNEL_MAP_T map, int32_t gain)
{
    uint32_t val;
    float coef;

    coef = db_to_amplitude_ratio(gain);

    // Gain format: 8.12
    val = (uint32_t)(coef * (1 << 12));
    val = __USAT(val, 20);
    hal_codec_set_adc_gain_value(map, val);
}

static int hal_codec_set_dac_hbf_bypass_cnt(uint32_t cnt)
{
    uint32_t bypass = 0;
    uint32_t bypass_mask = CODEC_CODEC_DAC_HBF1_BYPASS | CODEC_CODEC_DAC_HBF2_BYPASS | CODEC_CODEC_DAC_HBF3_BYPASS;

    if (cnt == 0) {
    } else if (cnt == 1) {
        bypass = CODEC_CODEC_DAC_HBF3_BYPASS;
    } else if (cnt == 2) {
        bypass = CODEC_CODEC_DAC_HBF2_BYPASS | CODEC_CODEC_DAC_HBF3_BYPASS;
    } else if (cnt == 3) {
        bypass = CODEC_CODEC_DAC_HBF1_BYPASS | CODEC_CODEC_DAC_HBF2_BYPASS | CODEC_CODEC_DAC_HBF3_BYPASS;
    } else {
        ASSERT(false, "%s: Invalid dac bypass cnt: %u", __FUNCTION__, cnt);
    }

    // OSR is fixed to 512
    //codec->REG_098 = SET_BITFIELD(codec->REG_098, CODEC_CODEC_DAC_OSR_SEL, 2);

    codec->REG_098 = (codec->REG_098 & ~bypass_mask) | bypass;
    return 0;
}

static int hal_codec_set_dac_up(uint32_t val)
{
    uint32_t sel = 0;

    if (val == 2) {
        sel = 0;
    } else if (val == 3) {
        sel = 1;
    } else if (val == 4) {
        sel = 2;
    } else if (val == 6) {
        sel = 3;
    } else if (val == 1) {
        sel = 4;
    } else {
        ASSERT(false, "%s: Invalid dac up: %u", __FUNCTION__, val);
    }
    codec->REG_098 = SET_BITFIELD(codec->REG_098, CODEC_CODEC_DAC_UP_SEL, sel);
    return 0;
}

static uint32_t POSSIBLY_UNUSED hal_codec_get_dac_up(void)
{
    uint32_t sel;

    sel = GET_BITFIELD(codec->REG_098, CODEC_CODEC_DAC_UP_SEL);
    if (sel == 0) {
        return 2;
    } else if (sel == 1) {
        return 3;
    } else if (sel == 2) {
        return 4;
    } else if (sel == 3) {
        return 6;
    } else {
        return 1;
    }
}

static int hal_codec_set_adc_down(enum AUD_CHANNEL_MAP_T map, uint32_t val)
{
    uint32_t sel = 0;

    if (val == 3) {
        sel = 0;
    } else if (val == 6) {
        sel = 1;
    } else if (val == 1) {
        sel = 2;
    } else {
        ASSERT(false, "%s: Invalid adc down: %u", __FUNCTION__, val);
    }
    for (int i = 0; i < MAX_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_DOWN_SEL_CH0, sel);
        }
    }
    return 0;
}

static int hal_codec_set_adc_hbf_bypass_cnt(enum AUD_CHANNEL_MAP_T map, uint32_t cnt)
{
    uint32_t bypass = 0;
    uint32_t bypass_mask = CODEC_CODEC_ADC_HBF1_BYPASS_CH0 | CODEC_CODEC_ADC_HBF2_BYPASS_CH0 | CODEC_CODEC_ADC_HBF3_BYPASS_CH0;

    if (cnt == 0) {
    } else if (cnt == 1) {
        bypass = CODEC_CODEC_ADC_HBF3_BYPASS_CH0;
    } else if (cnt == 2) {
        bypass = CODEC_CODEC_ADC_HBF2_BYPASS_CH0 | CODEC_CODEC_ADC_HBF3_BYPASS_CH0;
    } else if (cnt == 3) {
        bypass = CODEC_CODEC_ADC_HBF1_BYPASS_CH0 | CODEC_CODEC_ADC_HBF2_BYPASS_CH0 | CODEC_CODEC_ADC_HBF3_BYPASS_CH0;
    } else {
        ASSERT(false, "%s: Invalid bypass cnt: %u", __FUNCTION__, cnt);
    }
    for (int i = 0; i < MAX_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            *(&codec->REG_084 + i) = (*(&codec->REG_084 + i) & ~bypass_mask) | bypass;;
        }
    }
    return 0;
}

static uint32_t POSSIBLY_UNUSED get_msb_pos(uint32_t val)
{
    uint32_t lead_zero;

    lead_zero = __CLZ(val);
    return (lead_zero >= 32) ? 32 : 31 - lead_zero;
}

static uint32_t POSSIBLY_UNUSED get_lsb_pos(uint32_t val)
{
    return __CLZ(__RBIT(val));
}

static void hal_codec_enable_dig_mic(enum AUD_CHANNEL_MAP_T mic_map)
{
    uint32_t phase = 0;
    uint32_t line_map = 0;

    phase = codec->REG_0A8;
    for (int i = 0; i < MAX_DIG_MIC_CH_NUM; i++) {
        if (mic_map & (AUD_CHANNEL_MAP_DIGMIC_CH0 << i)) {
            line_map |= (1 << (i / 2));
        }
        phase = (phase & ~(CODEC_CODEC_PDM_CAP_PHASE_CH0_MASK << (i * 2))) |
            (CODEC_CODEC_PDM_CAP_PHASE_CH0(CODEC_DIGMIC_PHASE) << (i * 2));
    }
    codec->REG_0A8 = phase;
    codec->REG_0A4 |= CODEC_CODEC_PDM_ENABLE;
    hal_iomux_set_dig_mic(line_map);
}

static void hal_codec_disable_dig_mic(void)
{
    codec->REG_0A4 &= ~CODEC_CODEC_PDM_ENABLE;
}

#ifdef __AUDIO_RESAMPLE__
static float get_playback_resample_phase(void)
{
    return (float)codec_dac_sample_rate[resample_rate_idx[AUD_STREAM_PLAYBACK]].codec_freq /
        (CODEC_FREQ_26M / (CODEC_FREQ_24P576M / CODEC_FREQ_48K_SERIES)) *
        resample_factor[AUD_STREAM_PLAYBACK];
}

static float get_capture_resample_phase(void)
{
    return (float)(CODEC_FREQ_26M / (CODEC_FREQ_24P576M / CODEC_FREQ_48K_SERIES)) /
        codec_adc_sample_rate[resample_rate_idx[AUD_STREAM_CAPTURE]].codec_freq /
        resample_factor[AUD_STREAM_CAPTURE];
}

static uint32_t resample_phase_float_to_value(float phase)
{
    if (phase >= 4.0) {
        return (uint32_t)-1;
    } else {
        // Phase format: 2.30
        return (uint32_t)(phase * (1 << 30));
    }
}

static float POSSIBLY_UNUSED resample_phase_value_to_float(uint32_t value)
{
    // Phase format: 2.30
    return (float)value / (1 << 30);
}
#endif

int hal_codec_setup_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream, const struct HAL_CODEC_CONFIG_T *cfg)
{
    int i;
    int rate_idx;
    uint32_t ana_dig_div;

    if (stream == AUD_STREAM_PLAYBACK) {
        if ((HAL_CODEC_CONFIG_CHANNEL_MAP | HAL_CODEC_CONFIG_CHANNEL_NUM) & cfg->set_flag) {
            if (cfg->channel_num == AUD_CHANNEL_NUM_2) {
                if (cfg->channel_map != (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
                    TRACE("\n!!! WARNING:%s: Bad play stereo ch map: 0x%X\n", __func__, cfg->channel_map);
                }
                codec->REG_0A0 &= ~CODEC_CODEC_DAC_OUT_SWAP;
                codec->REG_044 |= CODEC_DUAL_CHANNEL_DAC;
                codec_dac_ch_map = AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1;
            } else {
                ASSERT(cfg->channel_map == AUD_CHANNEL_MAP_CH0 || cfg->channel_map == AUD_CHANNEL_MAP_CH1,
                    "%s: Bad play mono ch map: 0x%X", __func__, cfg->channel_map);
                // Hw resample only supports mono channel on channel L
                if (cfg->channel_map == AUD_CHANNEL_MAP_CH1) {
                    codec->REG_0A0 |= CODEC_CODEC_DAC_OUT_SWAP;
                } else {
                    codec->REG_0A0 &= ~CODEC_CODEC_DAC_OUT_SWAP;
                }
                codec->REG_044 &= ~CODEC_DUAL_CHANNEL_DAC;
                codec_dac_ch_map = AUD_CHANNEL_MAP_CH0;
            }
        }

        if (HAL_CODEC_CONFIG_BITS & cfg->set_flag) {
            codec->REG_044 &= ~CODEC_MODE_32BIT_DAC;
            codec->REG_04C &= ~CODEC_MODE_32BIT_MC;
            if (cfg->bits == AUD_BITS_16) {
                codec->REG_044 |= CODEC_MODE_16BIT_DAC;
                codec->REG_04C |= CODEC_MODE_16BIT_MC;
            } else {
                codec->REG_044 &= ~CODEC_MODE_16BIT_DAC;
                codec->REG_04C &= ~CODEC_MODE_16BIT_MC;
            }
        }

        if (HAL_CODEC_CONFIG_SAMPLE_RATE & cfg->set_flag) {
            for (i = 0; i < ARRAY_SIZE(codec_dac_sample_rate); i++) {
                if (codec_dac_sample_rate[i].sample_rate == cfg->sample_rate) {
                    break;
                }
            }
            ASSERT(i < ARRAY_SIZE(codec_dac_sample_rate), "%s: Invalid playback sample rate: %u", __func__, cfg->sample_rate);
            rate_idx = i;
            ana_dig_div = codec_dac_sample_rate[rate_idx].codec_div / codec_dac_sample_rate[rate_idx].cmu_div;
            ASSERT(ana_dig_div * codec_dac_sample_rate[rate_idx].cmu_div == codec_dac_sample_rate[rate_idx].codec_div,
                "%s: Invalid playback div for rate %u: codec_div=%u cmu_div=%u", __func__, cfg->sample_rate,
                codec_dac_sample_rate[rate_idx].codec_div, codec_dac_sample_rate[rate_idx].cmu_div);

            TRACE("[%s] playback sample_rate=%d", __func__, cfg->sample_rate);

#ifdef __AUDIO_RESAMPLE__
            uint32_t mask, val;

            mask = CODEC_CODEC_RESAMPLE_DAC_L_ENABLE | CODEC_CODEC_RESAMPLE_DAC_R_ENABLE |
                CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;

            if (hal_cmu_get_audio_resample_status() && codec_dac_sample_rate[rate_idx].codec_freq != CODEC_FREQ_26M) {
                if ((codec->REG_0E4 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE) == 0 ||
                        resample_rate_idx[AUD_STREAM_PLAYBACK] != rate_idx) {
                    resample_rate_idx[AUD_STREAM_PLAYBACK] = rate_idx;
                    codec->REG_0E4 &= ~mask;
                    hal_codec_reg_update_delay();
                    codec->REG_0F4 = resample_phase_float_to_value(get_playback_resample_phase());
                    hal_codec_reg_update_delay();
                    codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
                    val = 0;
                    if (codec->REG_044 & CODEC_DUAL_CHANNEL_DAC) {
                        val |= CODEC_CODEC_RESAMPLE_DAC_L_ENABLE | CODEC_CODEC_RESAMPLE_DAC_R_ENABLE;
                    } else {
                        val |= CODEC_CODEC_RESAMPLE_DAC_L_ENABLE;
                    }
                    codec->REG_0E4 |= val;
                }
            } else {
                codec->REG_0E4 &= ~mask;
            }
#endif

            // 8K -> 4ms, 16K -> 2ms, ...
            dac_delay_ms = 4 / ((cfg->sample_rate + AUD_SAMPRATE_8000 / 2) / AUD_SAMPRATE_8000);
            if (dac_delay_ms < 2) {
                dac_delay_ms = 2;
            }

#ifdef __AUDIO_RESAMPLE__
            if (!hal_cmu_get_audio_resample_status())
#endif
            {
#ifdef __AUDIO_RESAMPLE__
                ASSERT(codec_dac_sample_rate[rate_idx].codec_freq != CODEC_FREQ_26M,
                    "%s: playback sample rate %u is for resample only", __func__, cfg->sample_rate);
#endif
                analog_aud_freq_pll_config(codec_dac_sample_rate[rate_idx].codec_freq, codec_dac_sample_rate[rate_idx].codec_div);
                hal_cmu_codec_dac_set_div(codec_dac_sample_rate[rate_idx].cmu_div);
            }
            hal_codec_set_dac_up(codec_dac_sample_rate[rate_idx].dac_up);
            hal_codec_set_dac_hbf_bypass_cnt(codec_dac_sample_rate[rate_idx].bypass_cnt);
        }

        if (HAL_CODEC_CONFIG_VOL & cfg->set_flag) {
            const struct CODEC_DAC_VOL_T *vol_tab_ptr;

            vol_tab_ptr = hal_codec_get_dac_volume(cfg->vol);
            if (vol_tab_ptr) {
#ifdef AUDIO_OUTPUT_SW_GAIN
                hal_codec_set_sw_gain(vol_tab_ptr->sdac_volume);
#else
                analog_aud_set_dac_gain(vol_tab_ptr->tx_pa_gain);
                hal_codec_set_dig_dac_gain(vol_tab_ptr->sdac_volume);
#endif
#ifdef PERF_TEST_POWER_KEY
                // Update performance test power after applying new dac volume
                hal_codec_update_perf_test_power();
#endif
            }
        }
    } else {
        enum AUD_CHANNEL_MAP_T mic_map;
        enum AUD_CHANNEL_MAP_T reserv_map;
        uint8_t cnt;
        uint8_t ch_idx;
        uint32_t cfg_set_mask;
        uint32_t cfg_clr_mask;

        mic_map = 0;
        if ((HAL_CODEC_CONFIG_CHANNEL_MAP | HAL_CODEC_CONFIG_CHANNEL_NUM) & cfg->set_flag) {
            codec_adc_ch_map = 0;
            codec_mic_ch_map = 0;
            mic_map = cfg->channel_map;
        }

        if (mic_map) {
            codec_mic_ch_map = mic_map;

            if (codec_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                hal_codec_enable_dig_mic(mic_map);
            } else if ((anc_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) == 0) {
                hal_codec_disable_dig_mic();
            }

            reserv_map = 0;

#ifdef ANC_APP
#if defined(ANC_FF_MIC_CH_L) && defined(ANC_FF_MIC_CH_R)
#ifdef ANC_PROD_TEST
            if ((ANC_FF_MIC_CH_L & ~VALID_MIC_MAP) || (ANC_FF_MIC_CH_L & (ANC_FF_MIC_CH_L - 1))) {
                ASSERT(false, "Invalid ANC_FF_MIC_CH_L: 0x%04X", ANC_FF_MIC_CH_L);
            }
            if ((ANC_FF_MIC_CH_R & ~VALID_MIC_MAP) || (ANC_FF_MIC_CH_R & (ANC_FF_MIC_CH_R - 1))) {
                ASSERT(false, "Invalid ANC_FF_MIC_CH_R: 0x%04X", ANC_FF_MIC_CH_R);
            }
            if (ANC_FF_MIC_CH_L & ANC_FF_MIC_CH_R) {
                ASSERT(false, "Conflicted ANC_FF_MIC_CH_L (0x%04X) and ANC_FF_MIC_CH_R (0x%04X)", ANC_FF_MIC_CH_L, ANC_FF_MIC_CH_R);
            }
#if defined(ANC_FB_MIC_CH_L) && defined(ANC_FB_MIC_CH_R)
            if ((ANC_FF_MIC_CH_L & ANC_FB_MIC_CH_L) || (ANC_FF_MIC_CH_L & ANC_FB_MIC_CH_R) ||
                    (ANC_FF_MIC_CH_R & ANC_FB_MIC_CH_L) || (ANC_FF_MIC_CH_R & ANC_FB_MIC_CH_R)) {
                ASSERT(false, "Conflicted FF MIC (0x%04X/0x%04X) and FB MIC (0x%04X/0x%04X)",
                    ANC_FF_MIC_CH_L, ANC_FF_MIC_CH_R, ANC_FB_MIC_CH_L, ANC_FB_MIC_CH_R);
            }
#endif
#else // !ANC_PROD_TEST
#if (ANC_FF_MIC_CH_L & ~VALID_MIC_MAP) || (ANC_FF_MIC_CH_L & (ANC_FF_MIC_CH_L - 1))
#error "Invalid ANC_FF_MIC_CH_L"
#endif
#if (ANC_FF_MIC_CH_R & ~VALID_MIC_MAP) || (ANC_FF_MIC_CH_R & (ANC_FF_MIC_CH_R - 1))
#error "Invalid ANC_FF_MIC_CH_R"
#endif
#if (ANC_FF_MIC_CH_L & ANC_FF_MIC_CH_R)
#error "Conflicted ANC_FF_MIC_CH_L and ANC_FF_MIC_CH_R"
#endif
#if defined(ANC_FB_MIC_CH_L) && defined(ANC_FB_MIC_CH_R)
#if (ANC_FF_MIC_CH_L & ANC_FB_MIC_CH_L) || (ANC_FF_MIC_CH_L & ANC_FB_MIC_CH_R) || \
        (ANC_FF_MIC_CH_R & ANC_FB_MIC_CH_L) || (ANC_FF_MIC_CH_R & ANC_FB_MIC_CH_R)
#error "Conflicted ANC_FF_MIC_CH_L and ANC_FF_MIC_CH_R, ANC_FB_MIC_CH_L, ANC_FB_MIC_CH_R"
#endif
#endif
#endif // !ANC_PROD_TEST
            if (mic_map & ANC_FF_MIC_CH_L) {
                codec_adc_ch_map |= AUD_CHANNEL_MAP_CH0;
                mic_map &= ~ANC_FF_MIC_CH_L;
                ch_idx = get_msb_pos(ANC_FF_MIC_CH_L);
                if (ANC_FF_MIC_CH_L & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                    codec->REG_0A8 = SET_BITFIELD(codec->REG_0A8, CODEC_CODEC_PDM_MUX_CH0, ch_idx);
                    codec->REG_0A4 |= CODEC_CODEC_PDM_ADC_SEL_CH0;
                } else {
                    codec->REG_084 = SET_BITFIELD(codec->REG_084, CODEC_CODEC_ADC_IN_SEL_CH0, ch_idx);
                    codec->REG_0A4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH0;
                }
            } else if (ANC_FF_MIC_CH_L & AUD_CHANNEL_MAP_ALL) {
                reserv_map |= AUD_CHANNEL_MAP_CH0;
            }
            if (mic_map & ANC_FF_MIC_CH_R) {
                codec_adc_ch_map |= AUD_CHANNEL_MAP_CH1;
                mic_map &= ~ANC_FF_MIC_CH_R;
                ch_idx = get_msb_pos(ANC_FF_MIC_CH_R);
                if (ANC_FF_MIC_CH_R & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                    codec->REG_0A8 = SET_BITFIELD(codec->REG_0A8, CODEC_CODEC_PDM_MUX_CH1, ch_idx);
                    codec->REG_0A4 |= CODEC_CODEC_PDM_ADC_SEL_CH1;
                } else {
                    codec->REG_088 = SET_BITFIELD(codec->REG_088, CODEC_CODEC_ADC_IN_SEL_CH1, ch_idx);
                    codec->REG_0A4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH1;
                }
            } else if (ANC_FF_MIC_CH_R & AUD_CHANNEL_MAP_ALL) {
                reserv_map |= AUD_CHANNEL_MAP_CH1;
            }
#endif

#if defined(ANC_FB_MIC_CH_L) && defined(ANC_FB_MIC_CH_R)
#ifdef ANC_PROD_TEST
            if ((ANC_FB_MIC_CH_L & ~VALID_MIC_MAP) || (ANC_FB_MIC_CH_L & (ANC_FB_MIC_CH_L - 1))) {
                ASSERT(false, "Invalid ANC_FB_MIC_CH_L: 0x%04X", ANC_FB_MIC_CH_L);
            }
            if ((ANC_FB_MIC_CH_R & ~VALID_MIC_MAP) || (ANC_FB_MIC_CH_R & (ANC_FB_MIC_CH_R - 1))) {
                ASSERT(false, "Invalid ANC_FB_MIC_CH_R: 0x%04X", ANC_FB_MIC_CH_R);
            }
            if (ANC_FB_MIC_CH_L & ANC_FB_MIC_CH_R) {
                ASSERT(false, "Conflicted ANC_FB_MIC_CH_L (0x%04X) and ANC_FB_MIC_CH_R (0x%04X)", ANC_FB_MIC_CH_L, ANC_FB_MIC_CH_R);
            }
#else // !ANC_PROD_TEST
#if (ANC_FB_MIC_CH_L & ~VALID_MIC_MAP) || (ANC_FB_MIC_CH_L & (ANC_FB_MIC_CH_L - 1))
#error "Invalid ANC_FB_MIC_CH_L"
#endif
#if (ANC_FB_MIC_CH_R & ~VALID_MIC_MAP) || (ANC_FB_MIC_CH_R & (ANC_FB_MIC_CH_R - 1))
#error "Invalid ANC_FB_MIC_CH_R"
#endif
#if (ANC_FB_MIC_CH_L & ANC_FB_MIC_CH_R)
#error "Conflicted ANC_FB_MIC_CH_L and ANC_FB_MIC_CH_R"
#endif
#endif // !ANC_PROD_TEST
            if (mic_map & ANC_FB_MIC_CH_L) {
                codec_adc_ch_map |= AUD_CHANNEL_MAP_CH2;
                mic_map &= ~ANC_FB_MIC_CH_L;
                ch_idx = get_msb_pos(ANC_FB_MIC_CH_L);
                if (ANC_FB_MIC_CH_L & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                    codec->REG_0A8 = SET_BITFIELD(codec->REG_0A8, CODEC_CODEC_PDM_MUX_CH2, ch_idx);
                    codec->REG_0A4 |= CODEC_CODEC_PDM_ADC_SEL_CH2;
                } else {
                    codec->REG_08C = SET_BITFIELD(codec->REG_08C, CODEC_CODEC_ADC_IN_SEL_CH2, ch_idx);
                    codec->REG_0A4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH2;
                }
            } else if (ANC_FB_MIC_CH_L & AUD_CHANNEL_MAP_ALL) {
                reserv_map |= AUD_CHANNEL_MAP_CH2;
            }
            if (mic_map & ANC_FB_MIC_CH_R) {
                codec_adc_ch_map |= AUD_CHANNEL_MAP_CH3;
                mic_map &= ~ANC_FB_MIC_CH_R;
                ch_idx = get_msb_pos(ANC_FB_MIC_CH_R);
                if (ANC_FB_MIC_CH_R & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                    codec->REG_0A8 = SET_BITFIELD(codec->REG_0A8, CODEC_CODEC_PDM_MUX_CH3, ch_idx);
                    codec->REG_0A4 |= CODEC_CODEC_PDM_ADC_SEL_CH3;
                } else {
                    codec->REG_090 = SET_BITFIELD(codec->REG_090, CODEC_CODEC_ADC_IN_SEL_CH3, ch_idx);
                    codec->REG_0A4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH3;
                }
            } else if (ANC_FB_MIC_CH_R & AUD_CHANNEL_MAP_ALL) {
                reserv_map |= AUD_CHANNEL_MAP_CH3;
            }
#endif
#endif // ANC_APP

            // Hardware IIR uses CHAN0 and CHAN1 only, so select chan number from CHAN0.
            i = 0;
            reserv_map |= codec_adc_ch_map;
            while (mic_map && i < MAX_ADC_CH_NUM) {
                ASSERT(i < MAX_ANA_MIC_CH_NUM || (mic_map & AUD_CHANNEL_MAP_DIGMIC_ALL),
                    "%s: Not enough ana cap chan: mic_map=0x%X adc_map=0x%X reserv_map=0x%X",
                    __func__, mic_map, codec_adc_ch_map, reserv_map);
                ch_idx = get_lsb_pos(mic_map);
                mic_map &= ~(1 << ch_idx);
                while ((reserv_map & (AUD_CHANNEL_MAP_CH0 << i)) && i < MAX_ADC_CH_NUM) {
                    i++;
                }
                if (i < MAX_ADC_CH_NUM) {
                    codec_adc_ch_map |= (AUD_CHANNEL_MAP_CH0 << i);
                    reserv_map |= codec_adc_ch_map;
                    if ((1 << ch_idx) & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                        codec->REG_0A8 = (codec->REG_0A8 & ~(CODEC_CODEC_PDM_MUX_CH0_MASK << (3 * i))) |
                            (CODEC_CODEC_PDM_MUX_CH0(ch_idx) << (3 * i));
                        codec->REG_0A4 |= CODEC_CODEC_PDM_ADC_SEL_CH0 << i;
                    } else {
                        *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_IN_SEL_CH0, ch_idx);
                        codec->REG_0A4 &= ~(CODEC_CODEC_PDM_ADC_SEL_CH0 << i);
                    }
                    i++;
                }
            }

            ASSERT(mic_map == 0, "%s: Bad cap chan map: 0x%X", __func__, mic_map);
        }

        cfg_set_mask = 0;
        cfg_clr_mask = 0;
        if (HAL_CODEC_CONFIG_BITS & cfg->set_flag) {
            if (cfg->bits == AUD_BITS_16) {
                codec->REG_040 &= ~(CODEC_MODE_24BIT_ADC | CODEC_MODE_32BIT_ADC);
                cfg_set_mask = CODEC_MODE_16BIT_ADC_CH0;
            } else {
                codec->REG_040 = (codec->REG_040 & ~CODEC_MODE_32BIT_ADC) | CODEC_MODE_24BIT_ADC;
                cfg_clr_mask = CODEC_MODE_16BIT_ADC_CH0;
            }
        }

        cnt = 0;
        for (i = 0; i < MAX_ADC_CH_NUM; i++) {
            if (codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
                cnt++;
                codec->REG_040 |= (cfg_set_mask << i);
                codec->REG_040 &= ~(cfg_clr_mask << i);
            } else {
                codec->REG_000 &= ~(CODEC_ADC_ENABLE_CH0 << i);
            }
        }
        ASSERT(cnt == cfg->channel_num, "%s: Invalid capture stream chan cfg: map=0x%X num=%u",
            __func__, codec_adc_ch_map, cfg->channel_num);

        if (HAL_CODEC_CONFIG_SAMPLE_RATE & cfg->set_flag) {
            for(i = 0; i < ARRAY_SIZE(codec_adc_sample_rate); i++) {
                if(codec_adc_sample_rate[i].sample_rate == cfg->sample_rate) {
                    break;
                }
            }
            ASSERT(i < ARRAY_SIZE(codec_adc_sample_rate), "%s: Invalid capture sample rate: %d", __func__, cfg->sample_rate);
            rate_idx = i;
            ana_dig_div = codec_adc_sample_rate[rate_idx].codec_div / codec_adc_sample_rate[rate_idx].cmu_div;
            ASSERT(ana_dig_div * codec_adc_sample_rate[rate_idx].cmu_div == codec_adc_sample_rate[rate_idx].codec_div,
                "%s: Invalid catpure div for rate %u: codec_div=%u cmu_div=%u", __func__, cfg->sample_rate,
                codec_adc_sample_rate[rate_idx].codec_div, codec_adc_sample_rate[rate_idx].cmu_div);

            TRACE("[%s] capture sample_rate=%d", __func__, cfg->sample_rate);

#ifdef __AUDIO_RESAMPLE__
            uint32_t mask, val;

            mask = CODEC_CODEC_RESAMPLE_ADC_DUAL_CH |
                CODEC_CODEC_RESAMPLE_ADC_CH0_SEL_MASK | CODEC_CODEC_RESAMPLE_ADC_CH1_SEL_MASK |
                CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;

            if (hal_cmu_get_audio_resample_status() && codec_adc_sample_rate[rate_idx].codec_freq != CODEC_FREQ_26M) {
                if ((codec->REG_0E4 & CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE) == 0 ||
                        resample_rate_idx[AUD_STREAM_CAPTURE] != rate_idx) {
                    resample_rate_idx[AUD_STREAM_CAPTURE] = rate_idx;
                    codec->REG_0E4 &= ~mask;
                    hal_codec_reg_update_delay();
                    codec->REG_0F8 = resample_phase_float_to_value(get_capture_resample_phase());
                    hal_codec_reg_update_delay();
                    codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
                    ASSERT(cfg->channel_num == AUD_CHANNEL_NUM_2 || cfg->channel_num == AUD_CHANNEL_NUM_1,
                        "%s: Invalid capture resample chan num: %d", __func__, cfg->channel_num);
                    cnt = 0;
                    val = 0;
                    for (i = 0; i < MAX_ADC_CH_NUM; i++) {
                        if (codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
                            if (cnt == 0) {
                                val |= CODEC_CODEC_RESAMPLE_ADC_CH0_SEL(i);
                            } else {
                                val |= CODEC_CODEC_RESAMPLE_ADC_CH1_SEL(i);
                            }
                            cnt++;
                        }
                    }
                    if (cfg->channel_num == AUD_CHANNEL_NUM_2) {
                        val |= CODEC_CODEC_RESAMPLE_ADC_DUAL_CH;
                    }
                    codec->REG_0E4 |= val;
                }
            } else {
                codec->REG_0E4 &= ~mask;
            }
#endif

#ifdef __AUDIO_RESAMPLE__
            if (!hal_cmu_get_audio_resample_status())
#endif
            {
#ifdef __AUDIO_RESAMPLE__
                ASSERT(codec_adc_sample_rate[rate_idx].codec_freq != CODEC_FREQ_26M,
                    "%s: capture sample rate %u is for resample only", __func__, cfg->sample_rate);
#endif
                analog_aud_freq_pll_config(codec_adc_sample_rate[rate_idx].codec_freq, codec_adc_sample_rate[rate_idx].codec_div);
                hal_cmu_codec_adc_set_div(codec_adc_sample_rate[rate_idx].cmu_div);
            }
            hal_codec_set_adc_down(codec_adc_ch_map, codec_adc_sample_rate[rate_idx].adc_down);
            hal_codec_set_adc_hbf_bypass_cnt(codec_adc_ch_map, codec_adc_sample_rate[rate_idx].bypass_cnt);
        }

#ifndef FIXED_CODEC_ADC_VOL
        if (HAL_CODEC_CONFIG_VOL & cfg->set_flag) {
            if (cfg->vol < ARRAY_SIZE(codec_adc_vol)) {
                hal_codec_set_dig_adc_gain(codec_adc_ch_map, codec_adc_vol[cfg->vol]);
            }
        }
#endif
    }

    return 0;
}

int hal_codec_playback_sample_rate_trim(enum HAL_CODEC_ID_T id, enum AUD_SAMPRATE_T samp_rate, float trim)
{
    uint8_t i;

    for (i = 0; i < ARRAY_SIZE(codec_dac_sample_rate); i++) {
        if (codec_dac_sample_rate[i].sample_rate == samp_rate) {
            break;
        }
    }
    ASSERT(i < ARRAY_SIZE(codec_dac_sample_rate), "%s: Invalid playback sample rate: %u", __func__, samp_rate);

    analog_aud_freq_pll_config((uint32_t)(codec_dac_sample_rate[i].codec_freq * (1 + trim)), codec_dac_sample_rate[i].codec_div);

    return 0;
}

int hal_codec_anc_adc_enable(enum ANC_TYPE_T type)
{
#ifdef ANC_APP
    enum AUD_CHANNEL_MAP_T map;
    enum AUD_CHANNEL_MAP_T mic_map;
    uint8_t ch_idx;

    map = 0;
    mic_map = 0;
    if (type == ANC_FEEDFORWARD) {
#if defined(ANC_FF_MIC_CH_L) && defined(ANC_FF_MIC_CH_R)
        if (ANC_FF_MIC_CH_L) {
            ch_idx = get_msb_pos(ANC_FF_MIC_CH_L);
            if (ANC_FF_MIC_CH_L & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                codec->REG_0A8 = SET_BITFIELD(codec->REG_0A8, CODEC_CODEC_PDM_MUX_CH0, ch_idx);
                codec->REG_0A4 |= CODEC_CODEC_PDM_ADC_SEL_CH0;
            } else {
                codec->REG_084 = SET_BITFIELD(codec->REG_084, CODEC_CODEC_ADC_IN_SEL_CH0, ch_idx);
                codec->REG_0A4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH0;
            }
            map |= AUD_CHANNEL_MAP_CH0;
            mic_map |= ANC_FF_MIC_CH_L;
        }
        if (ANC_FF_MIC_CH_R) {
            ch_idx = get_msb_pos(ANC_FF_MIC_CH_R);
            if (ANC_FF_MIC_CH_R & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                codec->REG_0A8 = SET_BITFIELD(codec->REG_0A8, CODEC_CODEC_PDM_MUX_CH1, ch_idx);
                codec->REG_0A4 |= CODEC_CODEC_PDM_ADC_SEL_CH1;
            } else {
                codec->REG_088 = SET_BITFIELD(codec->REG_088, CODEC_CODEC_ADC_IN_SEL_CH1, ch_idx);
                codec->REG_0A4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH1;
            }
            map |= AUD_CHANNEL_MAP_CH1;
            mic_map |= ANC_FF_MIC_CH_R;
        }
#else
        ASSERT(false, "No ana adc ff ch defined");
#endif
    } else if (type == ANC_FEEDBACK) {
#if defined(ANC_FB_MIC_CH_L) && defined(ANC_FB_MIC_CH_R)
        if (ANC_FB_MIC_CH_L) {
            ch_idx = get_msb_pos(ANC_FB_MIC_CH_L);
            if (ANC_FB_MIC_CH_L & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                codec->REG_0A8 = SET_BITFIELD(codec->REG_0A8, CODEC_CODEC_PDM_MUX_CH2, ch_idx);
                codec->REG_0A4 |= CODEC_CODEC_PDM_ADC_SEL_CH2;
            } else {
                codec->REG_08C = SET_BITFIELD(codec->REG_08C, CODEC_CODEC_ADC_IN_SEL_CH2, ch_idx);
                codec->REG_0A4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH2;
            }
            map |= AUD_CHANNEL_MAP_CH2;
            mic_map |= ANC_FB_MIC_CH_L;
        }
        if (ANC_FB_MIC_CH_R) {
            ch_idx = get_msb_pos(ANC_FB_MIC_CH_R);
            if (ANC_FB_MIC_CH_R & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                codec->REG_0A8 = SET_BITFIELD(codec->REG_0A8, CODEC_CODEC_PDM_MUX_CH3, ch_idx);
                codec->REG_0A4 |= CODEC_CODEC_PDM_ADC_SEL_CH3;
            } else {
                codec->REG_090 = SET_BITFIELD(codec->REG_090, CODEC_CODEC_ADC_IN_SEL_CH3, ch_idx);
                codec->REG_0A4 &= ~CODEC_CODEC_PDM_ADC_SEL_CH3;
            }
            map |= AUD_CHANNEL_MAP_CH3;
            mic_map |= ANC_FB_MIC_CH_R;
        }
#else
        ASSERT(false, "No ana adc fb ch defined");
#endif
    }
    anc_adc_ch_map |= map;
    anc_mic_ch_map |= mic_map;

    if (anc_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) {
        hal_codec_enable_dig_mic(anc_mic_ch_map);
    }

    for (int i = 0; i < MAX_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            if ((codec->REG_080 & (CODEC_CODEC_ADC_EN_CH0 << i)) == 0) {
                // Reset ADC channel
                codec->REG_064 &= ~CODEC_SOFT_RSTN_ADC(1 << i);
                codec->REG_064 |= CODEC_SOFT_RSTN_ADC(1 << i);
                codec->REG_080 |= (CODEC_CODEC_ADC_EN_CH0 << i);
            }
        }
#ifdef ADC_MUX_WORKAROUND
        if (mic_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            if ((codec->REG_080 & (CODEC_CODEC_ADC_EN_CH0 << i)) == 0) {
                // Reset ADC channel
                codec->REG_064 &= ~CODEC_SOFT_RSTN_ADC(1 << i);
                codec->REG_064 |= CODEC_SOFT_RSTN_ADC(1 << i);
                codec->REG_080 |= (CODEC_CODEC_ADC_EN_CH0 << i);
            }
        }
#endif
    }

#ifdef DAC_DRE_ENABLE
    if (anc_adc_ch_map && (codec->REG_098 & CODEC_CODEC_DAC_EN)) {
        hal_codec_dac_dre_disable();
    }
#endif
#endif

    return 0;
}

int hal_codec_anc_adc_disable(enum ANC_TYPE_T type)
{
#ifdef ANC_APP
    enum AUD_CHANNEL_MAP_T map;
    enum AUD_CHANNEL_MAP_T mic_map;

    map = 0;
    mic_map = 0;
    if (type == ANC_FEEDFORWARD) {
#if defined(ANC_FF_MIC_CH_L) && defined(ANC_FF_MIC_CH_R)
        if (ANC_FF_MIC_CH_L) {
            map |= AUD_CHANNEL_MAP_CH0;
            mic_map |= ANC_FF_MIC_CH_L;
        }
        if (ANC_FF_MIC_CH_R) {
            map |= AUD_CHANNEL_MAP_CH1;
            mic_map |= ANC_FF_MIC_CH_R;
        }
#endif
    } else if (type == ANC_FEEDBACK) {
#if defined(ANC_FB_MIC_CH_L) && defined(ANC_FB_MIC_CH_R)
        if (ANC_FB_MIC_CH_L) {
            map |= AUD_CHANNEL_MAP_CH2;
            mic_map |= ANC_FB_MIC_CH_L;
        }
        if (ANC_FB_MIC_CH_R) {
            map |= AUD_CHANNEL_MAP_CH3;
            mic_map |= ANC_FB_MIC_CH_R;
        }
#endif
    }
    anc_adc_ch_map &= ~map;
    anc_mic_ch_map &= ~mic_map;

    if ((anc_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) == 0 &&
            (codec_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) == 0) {
        hal_codec_disable_dig_mic();
    }

    for (int i = 0; i < MAX_ADC_CH_NUM; i++) {
#ifdef ADC_MUX_WORKAROUND
        if (((map | mic_map) & (AUD_CHANNEL_MAP_CH0 << i)) && ((codec->REG_000 & CODEC_ADC_ENABLE) == 0 ||
                ((codec_adc_ch_map | codec_mic_ch_map) & (AUD_CHANNEL_MAP_CH0 << i)) == 0)) {
            codec->REG_080 &= ~(CODEC_CODEC_ADC_EN_CH0 << i);
        }
#else
        if ((map & (AUD_CHANNEL_MAP_CH0 << i)) && ((codec->REG_000 & CODEC_ADC_ENABLE) == 0 ||
                (codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) == 0)) {
            codec->REG_080 &= ~(CODEC_CODEC_ADC_EN_CH0 << i);
        }
#endif
    }

#ifdef DAC_DRE_ENABLE
    if (anc_adc_ch_map == 0 && (codec->REG_098 & CODEC_CODEC_DAC_EN) &&
            (codec->REG_044 & CODEC_MODE_16BIT_DAC) == 0) {
        hal_codec_dac_dre_enable();
    }
#endif
#endif

    return 0;
}

enum AUD_SAMPRATE_T hal_codec_anc_convert_rate(enum AUD_SAMPRATE_T rate)
{
    if (hal_cmu_get_audio_resample_status()) {
        return AUD_SAMPRATE_50781;
    } else if (CODEC_FREQ_48K_SERIES / rate * rate == CODEC_FREQ_48K_SERIES) {
        return AUD_SAMPRATE_48000;
    } else /* if (CODEC_FREQ_44_1K_SERIES / rate * rate == CODEC_FREQ_44_1K_SERIES) */ {
        return AUD_SAMPRATE_44100;
    }
}

int hal_codec_anc_dma_enable(enum HAL_CODEC_ID_T id)
{
    return 0;
}

int hal_codec_anc_dma_disable(enum HAL_CODEC_ID_T id)
{
    return 0;
}

int hal_codec_aux_mic_dma_enable(enum HAL_CODEC_ID_T id)
{
    return 0;
}

int hal_codec_aux_mic_dma_disable(enum HAL_CODEC_ID_T id)
{
    return 0;
}

uint32_t hal_codec_get_alg_dac_shift(void)
{
    return 0;
}

#ifdef ANC_APP
void hal_codec_apply_dac_gain_offset(enum HAL_CODEC_ID_T id, int16_t offset)
{
    int16_t new_offset;

    offset += (offset > 0) ? 2 : -2;
    new_offset = QDB_TO_DB(offset);

    if (digdac_gain_offset_anc == new_offset) {
        return;
    }

    TRACE("hal_codec: apply dac gain offset: %d", new_offset);

    digdac_gain_offset_anc = new_offset;

#ifdef AUDIO_OUTPUT_SW_GAIN
    hal_codec_set_sw_gain(digdac_gain);
#else
    hal_codec_set_dig_dac_gain(digdac_gain);
#endif
}

void hal_codec_anc_init_speedup(bool enable)
{
    if (enable) {
        hal_codec_int_open(HAL_CODEC_OPEN_ANC_INIT);
        codec->REG_1C0 = SET_BITFIELD(codec->REG_1C0, CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH0, 0xF);
        codec->REG_1C8 = SET_BITFIELD(codec->REG_1C8, CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH1, 0xF);
        codec->REG_1D0 = SET_BITFIELD(codec->REG_1D0, CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH2, 0xF);
        codec->REG_1D8 = SET_BITFIELD(codec->REG_1D8, CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH3, 0xF);
    } else {
        codec->REG_1C0 = SET_BITFIELD(codec->REG_1C0, CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH0, 0);
        codec->REG_1C8 = SET_BITFIELD(codec->REG_1C8, CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH1, 0);
        codec->REG_1D0 = SET_BITFIELD(codec->REG_1D0, CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH2, 0);
        codec->REG_1D8 = SET_BITFIELD(codec->REG_1D8, CODEC_CODEC_ADC_DRE_INI_ANA_GAIN_CH3, 0);
        hal_codec_int_close(HAL_CODEC_OPEN_ANC_INIT);
    }
}
#endif

#ifdef AUDIO_OUTPUT_DC_CALIB
void hal_codec_set_dac_dc_gain_attn(float attn)
{
    dac_dc_gain_attn = attn;
}

void hal_codec_set_dac_dc_offset(int16_t dc_l, int16_t dc_r)
{
    // DC calib values are based on 16-bit, but hardware compensation is based on 24-bit
    dac_dc_l = dc_l << 8;
    dac_dc_r = dc_r << 8;
}
#endif

void hal_codec_set_dac_reset_callback(HAL_CODEC_DAC_RESET_CALLBACK callback)
{
    //dac_reset_callback = callback;
}

void hal_codec_sync_dac_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
#if defined(__TWS__) && defined(__BT_ANC__)
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_DAC_ENABLE_SEL, type) | CODEC_STAMP_CLR_USED;
#else
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_CODEC_DAC_ENABLE_SEL, type) | CODEC_STAMP_CLR_USED;
#endif
}

void hal_codec_sync_dac_disable(void)
{
#if defined(__TWS__) && defined(__BT_ANC__)
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_DAC_ENABLE_SEL, HAL_CODEC_SYNC_TYPE_NONE);
#else
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_CODEC_DAC_ENABLE_SEL, HAL_CODEC_SYNC_TYPE_NONE);
#endif
}

void hal_codec_sync_adc_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_CODEC_ADC_ENABLE_SEL, type) | CODEC_STAMP_CLR_USED;
}

void hal_codec_sync_adc_disable(void)
{
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_CODEC_ADC_ENABLE_SEL, HAL_CODEC_SYNC_TYPE_NONE);
}

int hal_codec_dac_reset_set(void)
{
    int ret;

    if (codec_opened) {
        ret = !!(codec->REG_064 & CODEC_SOFT_RSTN_DAC);
        codec->REG_064 &= ~CODEC_SOFT_RSTN_DAC;
    } else {
        ret = 0;
    }

    return ret;
}

int hal_codec_dac_reset_clear(void)
{
    int ret;

    if (codec_opened) {
        ret = !!(codec->REG_064 & CODEC_SOFT_RSTN_DAC);
        codec->REG_064 |= CODEC_SOFT_RSTN_DAC;
    } else {
        ret = 0;
    }

    return ret;
}

void hal_codec_tune_resample_rate(enum AUD_STREAM_T stream, float factor)
{
#ifdef __AUDIO_RESAMPLE__
    resample_factor[stream] = factor;

    if (!codec_opened) {
        return;
    }

    if (stream == AUD_STREAM_PLAYBACK) {
        if (codec->REG_0E4 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE) {
            codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
            hal_codec_reg_update_delay();
            codec->REG_0F4 = resample_phase_float_to_value(get_playback_resample_phase());
            hal_codec_reg_update_delay();
            codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
        }
    } else {
        if (codec->REG_0E4 & CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE) {
            codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
            hal_codec_reg_update_delay();
            codec->REG_0F8 = resample_phase_float_to_value(get_capture_resample_phase());
            hal_codec_reg_update_delay();
            codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
        }
    }
#endif
}

int hal_codec_select_clock_out(uint32_t cfg)
{
    uint32_t lock;
    int ret = 1;

    lock = int_lock();

    if (codec_opened) {
        codec->REG_060 = SET_BITFIELD(codec->REG_060, CODEC_CFG_CLK_OUT, cfg);
        ret = 0;
    }

    int_unlock(lock);

    return ret;
}

#ifdef AUDIO_ANC_FB_MC
void hal_codec_setup_mc(enum AUD_CHANNEL_NUM_T channel_num, enum AUD_BITS_T bits)
{
    if (channel_num == AUD_CHANNEL_NUM_2) {
        mc_dual_chan = true;
    } else {
        mc_dual_chan = false;
    }

    if (bits <= AUD_BITS_16) {
        mc_16bit = true;
    } else {
        mc_16bit = false;
    }
}
#endif

