#include "reg_codec_best3001.h"
#include "hal_codec.h"
#include "hal_chipid.h"
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

#define FIXED_CODEC_ADC_VOL

#define NO_DAC_RESET

#define HAL_CODEC_TX_FIFO_TRIGGER_LEVEL     (3)
#define HAL_CODEC_RX_FIFO_TRIGGER_LEVEL     (4)

#define MAX_DIG_DAC_DBVAL                   (50)
#define ZERODB_DIG_DAC_DBVAL                (0)
#define MIN_DIG_DAC_DBVAL                   (-99)

#ifndef CODEC_DIGMIC_PHASE
#define CODEC_DIGMIC_PHASE                  7
#endif

#define MAX_DIG_MIC_CH_NUM                  8

#define MAX_ADC_CH_NUM                      8

#define VALID_MIC_MAP                       (AUD_CHANNEL_MAP_ALL & ~(AUD_CHANNEL_MAP_CH5 | AUD_CHANNEL_MAP_CH6 | AUD_CHANNEL_MAP_CH7))
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

static const uint32_t pdm_mux[MAX_ANA_MIC_CH_NUM] = {
    CODEC_09_PDM_ADC_SEL_CH0,
    CODEC_09_PDM_ADC_SEL_CH1,
    CODEC_09_PDM_ADC_SEL_CH2,
    CODEC_09_PDM_ADC_SEL_CH3,
    CODEC_09_PDM_ADC_SEL_CH4,
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
#ifndef DIG_MIC_WORKAROUND
static bool adc_enabled;
#endif
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

static void hal_codec_set_adc_gain_value(enum AUD_CHANNEL_MAP_T map, uint32_t val);
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

        codec->REG32B_10 = SET_BITFIELD(codec->REG32B_10, CODEC_10_CLASSG_THD2, config->thd2);
        codec->REG32B_10 = SET_BITFIELD(codec->REG32B_10, CODEC_10_CLASSG_THD1, config->thd1);
        codec->REG32B_10 = SET_BITFIELD(codec->REG32B_10, CODEC_10_CLASSG_THD0, config->thd0);

        codec->REG32B_11 = SET_BITFIELD(codec->REG32B_11, CODEC_11_CLASSG_WINDOW, config->wind_width);

        if (config->lr)
            codec->REG32B_10 |= CODEC_10_CLASSG_LR;
        else
            codec->REG32B_10 &= ~CODEC_10_CLASSG_LR;

        if (config->step_4_3n)
            codec->REG32B_10 |= CODEC_10_CLASSG_STEP_4_3N;
        else
            codec->REG32B_10 &= ~CODEC_10_CLASSG_STEP_4_3N;

        if (config->quick_down)
            codec->REG32B_10 |= CODEC_10_CLASSG_QUICK_DOWN;
        else
            codec->REG32B_10 &= ~CODEC_10_CLASSG_QUICK_DOWN;

        codec->REG32B_10 |= CODEC_10_CLASSG_EN;
    } else {
        codec->REG32B_10 &= ~CODEC_10_CLASSG_EN;
    }
}
#endif

#ifdef AUDIO_OUTPUT_DC_CALIB
static void hal_codec_dac_dc_offset_enable(void)
{
    codec->REG32B_57 &= (CODEC_57_DAC_CH0_GAIN_UPDATE | CODEC_57_DAC_CH1_GAIN_UPDATE);
    hal_codec_reg_update_delay();
    codec->REG32B_57 = CODEC_57_DAC_CH0_DC(dac_dc_l) | CODEC_57_DAC_CH1_DC(dac_dc_r);
    codec->REG32B_57 |= (CODEC_57_DAC_CH0_GAIN_UPDATE | CODEC_57_DAC_CH1_GAIN_UPDATE);
}
#endif

static int hal_codec_int_open(enum HAL_CODEC_OPEN_T type)
{
#ifdef ANC_APP
    enum HAL_CODEC_OPEN_T old_type;

    old_type = open_type;
    open_type |= type;
    if (old_type) {
        return 0;
    }
#endif

#ifdef __AUDIO_RESAMPLE__
    bool first_open = !codec_init;
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

    codec->CMU_CTRL |= CODEC_CMU_EN_CLK_DAC | CODEC_CMU_EN_CLK_ADC_ALL | CODEC_CMU_EN_CLK_ADC_ANA_ALL;
    codec->CMU_CTRL2 |= CODEC_CMU2_SOFT_RSTN_IIR | CODEC_CMU2_SOFT_RSTN_DAC | CODEC_CMU2_SOFT_RSTN_ADC_ALL;
    codec->CONFIG &= ~(CODEC_CFG_ADC_ENABLE | CODEC_CFG_DAC_ENABLE |
        CODEC_CFG_DMACTRL_TX | CODEC_CFG_DMACTRL_RX | CODEC_CFG_MC_ENABLE);
    codec->FIFO_FLUSH |= (CODEC_FIFO_FLUSH_TX_FIFO_FLUSH | CODEC_FIFO_FLUSH_RX_FIFO_FLUSH_ALL);
    hal_codec_reg_update_delay();
    codec->FIFO_FLUSH &= ~(CODEC_FIFO_FLUSH_TX_FIFO_FLUSH | CODEC_FIFO_FLUSH_RX_FIFO_FLUSH_ALL);
    codec->CONFIG |= CODEC_CFG_CODEC_IF_EN;

#ifdef AUDIO_ANC_FB_MC
    codec->MC_CONFIG = MC_CONFIG_MC_DELAY(10) | MC_CONFIG_DMACTRL_MC;
#endif

    // ANC zero-crossing
    codec->REG32B_07 |= CODEC_07_MUTE_GAIN_PASS0_CH0 | CODEC_07_MUTE_GAIN_PASS0_CH1;

    // Enable ADC zero-crossing gain adjustment
    codec->REG32B_09 |= CODEC_09_ADC_GAIN_SEL;

#ifdef AUDIO_ANC_FB_MC
    // Enable ADC + music cancel.
    codec->REG32B_51 |= CODEC_FB_CH_CHECK_KEEP_SEL;
    codec->REG32B_52 |= CODEC_FB_CH_CHECK_KEEP_SEL;
#endif

#ifdef FIXED_CODEC_ADC_VOL
    hal_codec_set_adc_gain_value(0xFF, CODEC_SADC_VOL);
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
        hal_codec_set_dig_dac_gain(ZERODB_DIG_DAC_DBVAL);
    }
#else
    // Enable DAC zero-crossing gain adjustment
    codec->REG32B_09 |= CODEC_09_DAC_GAIN_SEL;
#endif

#ifdef __AUDIO_RESAMPLE__
    if (first_open) {
        static const struct HAL_CODEC_CONFIG_T cfg = {
            .sample_rate = AUD_SAMPRATE_192000,
            .set_flag = HAL_CODEC_CONFIG_SAMPLE_RATE,
        };

        // Init h/w resample memory
        hal_codec_setup_stream(HAL_CODEC_ID_0, AUD_STREAM_PLAYBACK, &cfg);
        codec->CONFIG &= ~CODEC_CFG_DAC_ENABLE;
        codec->REG32B_04 |= CODEC_04_DAC_EN | CODEC_04_DAC_EN_CH1 | CODEC_04_DAC_EN_CH0;
        osDelay(2);
        codec->REG32B_04 &= ~(CODEC_04_DAC_EN | CODEC_04_DAC_EN_CH1 | CODEC_04_DAC_EN_CH0);
    }
#endif

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

    codec->CONFIG &= ~CODEC_CFG_CODEC_IF_EN;
    codec->CMU_CTRL2 &= ~(CODEC_CMU2_SOFT_RSTN_IIR | CODEC_CMU2_SOFT_RSTN_DAC | CODEC_CMU2_SOFT_RSTN_ADC_ALL);
    codec->CMU_CTRL &= ~(CODEC_CMU_EN_CLK_DAC | CODEC_CMU_EN_CLK_ADC_ALL | CODEC_CMU_EN_CLK_ADC_ANA_ALL);

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
    codec->REG32B_24 = (codec->REG32B_24 & ~(DRE_DAC_THD_DB_OFFSET_SIGN | DRE_DAC_DRE_DELAY_MASK |
            DRE_DAC_INI_ANA_GAIN_MASK | DRE_DAC_THD_DB_OFFSET_MASK | DRE_DAC_STEP_MODE_MASK)) |
        DRE_DAC_THD_DB_OFFSET_SIGN | DRE_DAC_DRE_DELAY(dac_dre_cfg.dre_delay) |
        DRE_DAC_INI_ANA_GAIN(0) | DRE_DAC_THD_DB_OFFSET(dac_dre_cfg.thd_db_offset) |
        DRE_DAC_STEP_MODE(dac_dre_cfg.step_mode) | DRE_DAC_DRE_ENABLE;
    codec->REG32B_25 = (codec->REG32B_25 & ~(DRE_WIN_DRE_WINDOW_MASK | DRE_WIN_AMP_HIGH_MASK)) |
        DRE_WIN_DRE_WINDOW(dac_dre_cfg.dre_win) | DRE_WIN_AMP_HIGH(dac_dre_cfg.amp_high);
    codec->REG32B_26 = (codec->REG32B_26 & ~(DRE_DAC_THD_DB_OFFSET_SIGN | DRE_DAC_DRE_DELAY_MASK |
            DRE_DAC_INI_ANA_GAIN_MASK | DRE_DAC_THD_DB_OFFSET_MASK | DRE_DAC_STEP_MODE_MASK)) |
        DRE_DAC_THD_DB_OFFSET_SIGN | DRE_DAC_DRE_DELAY(dac_dre_cfg.dre_delay) |
        DRE_DAC_INI_ANA_GAIN(0) | DRE_DAC_THD_DB_OFFSET(dac_dre_cfg.thd_db_offset) |
        DRE_DAC_STEP_MODE(dac_dre_cfg.step_mode) | DRE_DAC_DRE_ENABLE;
    codec->REG32B_27 = (codec->REG32B_27 & ~(DRE_WIN_DRE_WINDOW_MASK | DRE_WIN_AMP_HIGH_MASK)) |
        DRE_WIN_DRE_WINDOW(dac_dre_cfg.dre_win) | DRE_WIN_AMP_HIGH(dac_dre_cfg.amp_high);
}

static void hal_codec_dac_dre_disable(void)
{
    codec->REG32B_24 &= ~DRE_DAC_DRE_ENABLE;
    codec->REG32B_26 &= ~DRE_DAC_DRE_ENABLE;
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

    if (codec->REG32B_24 & DRE_DAC_DRE_ENABLE) {
        dac_vol = nominal_vol;
    } else {
        codec->REG32B_24 = SET_BITFIELD(codec->REG32B_24, DRE_DAC_INI_ANA_GAIN, ini_ana_gain);
        codec->REG32B_26 = SET_BITFIELD(codec->REG32B_26, DRE_DAC_INI_ANA_GAIN, ini_ana_gain);
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

    if (!codec_opened || (codec->REG32B_04 & CODEC_04_DAC_EN) == 0) {
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
        ini_ana_gain = GET_BITFIELD(codec->REG32B_24, DRE_DAC_INI_ANA_GAIN);
        digdac_gain_offset_nr = ((0xF - ini_ana_gain) * 11 + 0xF / 2) / 0xF;
        ini_ana_gain = 0xF;
    } else {
        ini_ana_gain = 0xF - (digdac_gain_offset_nr * 0xF + 11 / 2) / 11;
        digdac_gain_offset_nr = 0;
    }

    codec->REG32B_24 = SET_BITFIELD(codec->REG32B_24, DRE_DAC_INI_ANA_GAIN, ini_ana_gain);
    codec->REG32B_26 = SET_BITFIELD(codec->REG32B_26, DRE_DAC_INI_ANA_GAIN, ini_ana_gain);

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

    codec->REG32B_04 &= ~(CODEC_04_DAC_EN | CODEC_04_DAC_EN_CH0 | CODEC_04_DAC_EN_CH1);

#ifdef DAC_DRE_ENABLE
    hal_codec_dac_dre_disable();
#endif

#if defined(DAC_CLASSG_ENABLE)
    hal_codec_classg_enable(false);
#endif

#ifndef NO_DAC_RESET
    // Reset DAC
    // Avoid DAC outputing noise after it is disabled
    codec->CMU_CTRL2 &= ~CODEC_CMU2_SOFT_RSTN_DAC;
    codec->CMU_CTRL2 |= CODEC_CMU2_SOFT_RSTN_DAC;
#endif
}

void hal_codec_start_playback_stream(enum HAL_CODEC_ID_T id)
{
#ifndef NO_DAC_RESET
    // Reset DAC
    codec->CMU_CTRL2 &= ~CODEC_CMU2_SOFT_RSTN_DAC;
    codec->CMU_CTRL2 |= CODEC_CMU2_SOFT_RSTN_DAC;
#endif

#ifdef DAC_DRE_ENABLE
    if ((codec->CONFIG & CODEC_CFG_MODE_16BIT_DAC) == 0
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
        codec->REG32B_04 |= CODEC_04_DAC_EN_CH0;
    } else {
        codec->REG32B_04 &= ~CODEC_04_DAC_EN_CH0;
    }
    if (codec_dac_ch_map & AUD_CHANNEL_MAP_CH1) {
        codec->REG32B_04 |= CODEC_04_DAC_EN_CH1;
    } else {
        codec->REG32B_04 &= ~CODEC_04_DAC_EN_CH1;
    }

#if (defined(AUDIO_OUTPUT_DC_CALIB_ANA) || defined(AUDIO_OUTPUT_DC_CALIB)) && (!defined(__TWS__) || defined(__BT_ANC__))
#if 0
    uint32_t cfg_en;
    uint32_t anc_ff_gain, anc_fb_gain;

    cfg_en = codec->CONFIG & CODEC_CFG_DAC_ENABLE;
    anc_ff_gain = codec->REG32B_14;
    anc_fb_gain = codec->REG32B_15;

    if (cfg_en) {
        codec->CONFIG &= ~cfg_en;
    }
    if (anc_ff_gain) {
        codec->REG32B_14 = 0;
    }
    if (anc_fb_gain) {
        codec->REG32B_15 = 0;
    }
#endif
#endif

    codec->REG32B_04 |= CODEC_04_DAC_EN;

#if (defined(AUDIO_OUTPUT_DC_CALIB_ANA) || defined(AUDIO_OUTPUT_DC_CALIB)) && (!defined(__TWS__) || defined(__BT_ANC__))
#if 0
    // At least delay 4ms for 8K-sample-rate mute data to arrive at DAC PA
    osDelay(5);

    if (cfg_en) {
        codec->CONFIG |= cfg_en;
    }
    if (anc_ff_gain) {
        codec->REG32B_14 = anc_ff_gain;
    }
    if (anc_fb_gain) {
        codec->REG32B_15 = anc_fb_gain;
    }
#endif

    // Enable PA
    analog_aud_codec_speaker_enable(true);

#ifdef AUDIO_ANC_FB_MC
    if (mc_enabled) {
        // MC FIFO and DAC FIFO must be started at the same time
        codec->CONFIG |= CODEC_CFG_MC_ENABLE | CODEC_CFG_DAC_ENABLE;
    }
#endif
#endif
}

#ifdef DIG_MIC_WORKAROUND
static bool codec_started[AUD_STREAM_NUM];

void hal_codec_dig_mic_activate(void)
{
    codec->REG32B_09 |= CODEC_09_PDM_ENABLE;
    codec->REG32B_04 |= CODEC_04_ADC_EN;
    hal_codec_reg_update_delay();
    // Reset ADC free running clock to get a fixed phase
    codec->CMU_CTRL2 &= ~(CODEC_CMU2_SOFT_RSTN_ADC_FREE | CODEC_CMU2_SOFT_RSTN_ADC_ANA);
    codec->CMU_CTRL2 |= (CODEC_CMU2_SOFT_RSTN_ADC_FREE | CODEC_CMU2_SOFT_RSTN_ADC_ANA);
    hal_codec_reg_update_delay();
}

void hal_codec_dig_mic_deactivate(void)
{
    codec->REG32B_04 &= ~CODEC_04_ADC_EN;
}
#endif

int hal_codec_start_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream)
{
#ifdef DIG_MIC_WORKAROUND
    codec_started[stream] = true;
    if (stream == AUD_STREAM_PLAYBACK) {
        if (!codec_started[AUD_STREAM_CAPTURE]) {
            hal_codec_dig_mic_activate();
        }
    } else {
        if (!codec_started[AUD_STREAM_PLAYBACK]) {
            hal_codec_dig_mic_activate();
        }
    }
#endif

    if (stream == AUD_STREAM_PLAYBACK) {
        // Reset and start DAC
        hal_codec_start_playback_stream(id);
    } else {
#ifndef DIG_MIC_WORKAROUND
#ifdef AUDIO_ANC_FB_MC
        adc_enabled = true;
        if (!mc_enabled)
#endif
        {
            // Reset ADC free running clock and ADC ANA
            codec->CMU_CTRL2 &= ~(CODEC_CMU2_SOFT_RSTN_ADC_FREE | CODEC_CMU2_SOFT_RSTN_ADC_ANA);
            codec->CMU_CTRL2 |= (CODEC_CMU2_SOFT_RSTN_ADC_FREE | CODEC_CMU2_SOFT_RSTN_ADC_ANA);
            codec->REG32B_04 |= CODEC_04_ADC_EN;
        }
#endif
    }

    return 0;
}

int hal_codec_stop_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream)
{
#ifdef DIG_MIC_WORKAROUND
    codec_started[stream] = false;
    if (!(codec_started[AUD_STREAM_PLAYBACK] || codec_started[AUD_STREAM_CAPTURE])) {
        hal_codec_dig_mic_deactivate();
    }
#endif

    if (stream == AUD_STREAM_PLAYBACK) {
        // Stop and reset DAC
        hal_codec_stop_playback_stream(id);
    } else {
#ifndef DIG_MIC_WORKAROUND
#ifdef AUDIO_ANC_FB_MC
        adc_enabled = false;
        if (!mc_enabled)
#endif
        {
            codec->REG32B_04 &= ~CODEC_04_ADC_EN;
        }
#endif
    }

    return 0;
}

int hal_codec_start_interface(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream, int dma)
{
    if (stream == AUD_STREAM_PLAYBACK) {
        codec->FIFO_FLUSH |= CODEC_FIFO_FLUSH_TX_FIFO_FLUSH;
        hal_codec_reg_update_delay();
        codec->FIFO_FLUSH &= ~CODEC_FIFO_FLUSH_TX_FIFO_FLUSH;
        if (dma) {
            codec->FIFO_THRESHOLD = SET_BITFIELD(codec->FIFO_THRESHOLD, CODEC_FIFO_THRESH_TX, HAL_CODEC_TX_FIFO_TRIGGER_LEVEL);
            codec->CONFIG |= CODEC_CFG_DMACTRL_TX;
            // Delay a little time for DMA to fill the TX FIFO before sending
            for (volatile int i = 0; i < 50; i++);
        }
#ifdef AUDIO_ANC_FB_MC
        if (codec_dac_ch_map == (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
            codec->MC_CONFIG |= MC_CONFIG_DUAL_CHANNEL_MC;
        } else {
            codec->MC_CONFIG &= ~MC_CONFIG_DUAL_CHANNEL_MC;
        }
        if (codec->CONFIG & CODEC_CFG_MODE_16BIT_DAC) {
            codec->MC_CONFIG |= MC_CONFIG_MODE_16BIT_MC;
        } else {
            codec->MC_CONFIG &= ~MC_CONFIG_MODE_16BIT_MC;
        }
#ifndef DIG_MIC_WORKAROUND
        if (!adc_enabled) {
            // Reset ADC free running clock and ADC ANA
            codec->CMU_CTRL2 &= ~(CODEC_CMU2_SOFT_RSTN_ADC_FREE | CODEC_CMU2_SOFT_RSTN_ADC_ANA);
            codec->CMU_CTRL2 |= (CODEC_CMU2_SOFT_RSTN_ADC_FREE | CODEC_CMU2_SOFT_RSTN_ADC_ANA);
            codec->REG32B_04 |= CODEC_04_ADC_EN;
        }
#endif
        // If codec function has been enabled, start FIFOs directly;
        // otherwise, start FIFOs after PA is enabled
        if (codec->REG32B_04 & CODEC_04_DAC_EN) {
            // MC FIFO and DAC FIFO must be started at the same time
            codec->CONFIG |= CODEC_CFG_MC_ENABLE | CODEC_CFG_DAC_ENABLE;
        }
        mc_enabled = true;
#else
        codec->CONFIG |= CODEC_CFG_DAC_ENABLE;
#endif
    } else {
        for (int i = 0; i < MAX_ADC_CH_NUM; i++) {
            if (codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
                if ((codec->REG32B_04 & (CODEC_04_ADC_EN_CH0 << i)) == 0) {
                    // Reset ADC channel
                    codec->CMU_CTRL2 &= ~CODEC_CMU2_SOFT_RSTN_ADC(i);
                    codec->CMU_CTRL2 |= CODEC_CMU2_SOFT_RSTN_ADC(i);
                    codec->REG32B_04 |= (CODEC_04_ADC_EN_CH0 << i);
                }
                codec->CONFIG |= (CODEC_CFG_ADC_ENABLE_CH0 << i);
            }
        }
        codec->FIFO_FLUSH |= CODEC_FIFO_FLUSH_RX_FIFO_FLUSH_ALL;
        hal_codec_reg_update_delay();
        codec->FIFO_FLUSH &= ~CODEC_FIFO_FLUSH_RX_FIFO_FLUSH_ALL;
        if (dma) {
            codec->FIFO_THRESHOLD = SET_BITFIELD(codec->FIFO_THRESHOLD, CODEC_FIFO_THRESH_RX, HAL_CODEC_RX_FIFO_TRIGGER_LEVEL);
            codec->CONFIG |= CODEC_CFG_DMACTRL_RX;
        }
        codec->CONFIG |= CODEC_CFG_ADC_ENABLE;
    }

    return 0;
}

int hal_codec_stop_interface(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream)
{
    if (stream == AUD_STREAM_PLAYBACK) {
#ifdef AUDIO_ANC_FB_MC
        mc_enabled = false;
        codec->CONFIG &= ~CODEC_CFG_MC_ENABLE;
#ifndef DIG_MIC_WORKAROUND
        if (!adc_enabled) {
            codec->REG32B_04 &= ~CODEC_04_ADC_EN;
        }
#endif
#endif
#ifdef __AUDIO_RESAMPLE__
        codec->REG32B_18 &= ~CODEC_18_RESAMPLE_SLOW_EN;
        codec->REG32B_18 = SET_BITFIELD(codec->REG32B_18, CODEC_18_RESAMPLE_SLOW_VAL, 0);
#endif
        codec->CONFIG &= ~CODEC_CFG_DAC_ENABLE;
        codec->CONFIG &= ~CODEC_CFG_DMACTRL_TX;
        codec->FIFO_FLUSH |= CODEC_FIFO_FLUSH_TX_FIFO_FLUSH;
        hal_codec_reg_update_delay();
        codec->FIFO_FLUSH &= ~CODEC_FIFO_FLUSH_TX_FIFO_FLUSH;
        // Cancel dac sync request
        hal_codec_sync_dac_disable();
#if defined(NO_DAC_RESET) || defined(__AUDIO_RESAMPLE__)
        // Clean up DAC and resample intermediate states
        osDelay(dac_delay_ms);
#endif
    } else {
        codec->CONFIG &= ~(CODEC_CFG_ADC_ENABLE | CODEC_CFG_ADC_ENABLE_CH_ALL);
        codec->CONFIG &= ~CODEC_CFG_DMACTRL_RX;
        for (int i = 0; i < MAX_ADC_CH_NUM; i++) {
            if ((codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) &&
                    (anc_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) == 0) {
                codec->REG32B_04 &= ~(CODEC_04_ADC_EN_CH0 << i);
            }
        }
        codec->FIFO_FLUSH |= CODEC_FIFO_FLUSH_RX_FIFO_FLUSH_ALL;
        hal_codec_reg_update_delay();
        codec->FIFO_FLUSH &= ~CODEC_FIFO_FLUSH_RX_FIFO_FLUSH_ALL;
        // Cancel adc sync request
        hal_codec_sync_adc_disable();
    }

    return 0;
}

static void hal_codec_set_dac_gain_value(uint32_t val)
{
    codec->REG32B_33 &= ~CODEC_33_DAC_CH0_GAIN_UPDATE;
    codec->REG32B_34 &= ~CODEC_34_DAC_CH1_GAIN_UPDATE;
    hal_codec_reg_update_delay();
    codec->REG32B_33 = SET_BITFIELD(codec->REG32B_33, CODEC_33_DAC_CH0_GAIN, val);
    codec->REG32B_34 = SET_BITFIELD(codec->REG32B_34, CODEC_34_DAC_CH1_GAIN, val);
    codec->REG32B_33 |= CODEC_33_DAC_CH0_GAIN_UPDATE;
    codec->REG32B_34 |= CODEC_34_DAC_CH1_GAIN_UPDATE;
}

void hal_codec_get_dac_gain(float *left_gain,float *right_gain)
{
    uint32_t left_val;
    uint32_t right_val;

    left_val  = GET_BITFIELD(codec->REG32B_33, CODEC_33_DAC_CH0_GAIN);
    right_val = GET_BITFIELD(codec->REG32B_34, CODEC_34_DAC_CH1_GAIN);

    *left_gain = left_val;
    *right_gain = right_val;

    *left_gain /= (1 << 12);
    *right_gain /= (1 << 12);
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

    if (db == ZERODB_DIG_DAC_DBVAL) {
        coef = 1;
    } else if (db <= MIN_DIG_DAC_DBVAL) {
        coef = 0;
    } else {
        if (db > MAX_DIG_DAC_DBVAL) {
            db = MAX_DIG_DAC_DBVAL;
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

    val = (uint32_t)(coef * (1 << 12));
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
    if (map & AUD_CHANNEL_MAP_CH0) {
        codec->REG32B_05 = SET_BITFIELD(codec->REG32B_05, CODEC_05_S_ADC_VOLUME_CH0, val);
    }
    if (map & AUD_CHANNEL_MAP_CH1) {
        codec->REG32B_09 = SET_BITFIELD(codec->REG32B_09, CODEC_09_S_ADC_VOLUME_CH1, val);
    }
    for (int i = 0; i < MAX_ADC_CH_NUM - 2; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << (2 + i))) {
            codec->REG32B_12 = (codec->REG32B_12 & ~CODEC_12_S_ADC_VOLUME_CH_MASK(i)) |
                CODEC_12_S_ADC_VOLUME_CH(i, val);
        }
    }
}

static int hal_codec_set_dac_hbf_bypass_cnt(uint32_t cnt)
{
    uint32_t bypass = 0;
    uint32_t bypass_mask = CODEC_04_DAC_HBF1_BYPASS | CODEC_04_DAC_HBF2_BYPASS | CODEC_04_DAC_HBF3_BYPASS;

    if (cnt == 0) {
    } else if (cnt == 1) {
        bypass = CODEC_04_DAC_HBF3_BYPASS;
    } else if (cnt == 2) {
        bypass = CODEC_04_DAC_HBF2_BYPASS | CODEC_04_DAC_HBF3_BYPASS;
    } else if (cnt == 3) {
        bypass = CODEC_04_DAC_HBF1_BYPASS | CODEC_04_DAC_HBF2_BYPASS | CODEC_04_DAC_HBF3_BYPASS;
    } else {
        ASSERT(false, "%s: Invalid dac bypass cnt: %u", __FUNCTION__, cnt);
    }

    // OSR is fixed to 512
    //codec->REG32B_06 = SET_BITFIELD(codec->REG32B_06, CODEC_06_S_DAC_OSR_SEL, 2);

    codec->REG32B_04 = (codec->REG32B_04 & ~bypass_mask) | bypass;
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
    codec->REG32B_04 = SET_BITFIELD(codec->REG32B_04, CODEC_04_DAC_UP_SEL, sel);
    return 0;
}

static uint32_t POSSIBLY_UNUSED hal_codec_get_dac_up(void)
{
    uint32_t sel;

    sel = GET_BITFIELD(codec->REG32B_04, CODEC_04_DAC_UP_SEL);
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
    uint32_t reg_val;

    if (val == 3) {
        sel = 0;
    } else if (val == 6) {
        sel = 1;
    } else if (val == 1) {
        sel = 2;
    } else {
        ASSERT(false, "%s: Invalid adc down: %u", __FUNCTION__, val);
    }
    reg_val = codec->REG32B_03;
    for (int i = 0; i < MAX_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            reg_val = (reg_val & ~CODEC_03_ADC_DOWN_SEL_CH_MASK(i)) | CODEC_03_ADC_DOWN_SEL_CH(i, sel);
        }
    }
    codec->REG32B_03 = reg_val;
    return 0;
}

static int hal_codec_set_adc_hbf_bypass_cnt(enum AUD_CHANNEL_MAP_T map, uint32_t cnt)
{
    uint32_t bypass = 0;
    uint32_t bypass_ch0 = 0;

    if (cnt == 0) {
        //bypass_ch0 = 0;
    } else if (cnt == 1) {
        bypass_ch0 = CODEC_06_ADC_HBF3_BYPASS_CH(0);
    } else if (cnt == 2) {
        bypass_ch0 = CODEC_06_ADC_HBF2_BYPASS_CH(0) | CODEC_06_ADC_HBF3_BYPASS_CH(0);
    } else if (cnt == 3) {
        bypass_ch0 = CODEC_06_ADC_HBF1_BYPASS_CH(0) | CODEC_06_ADC_HBF2_BYPASS_CH(0) | CODEC_06_ADC_HBF3_BYPASS_CH(0);
    } else {
        ASSERT(false, "%s: Invalid bypass cnt: %u", __FUNCTION__, cnt);
    }
    for (int i = 0; i < MAX_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            bypass |= bypass_ch0 << (3 * i);
        }
    }
    codec->REG32B_06 = (codec->REG32B_06 & ~CODEC_06_ADC_HBF_BYPASS_CH_ALL) | bypass;
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

static void hal_codec_dac_compensate(enum AUD_SAMPRATE_T sample_rate)
{
    // Frequency response eq
    const static uint32_t coef[][8] = {
        // 48K
        { 0x00000, 0x00000, 0x00000, 0x00002, 0xFFFF8, 0x0002A, 0xFFF2E, 0x3FE4E, },
        // 192K
        { 0xFFF1A, 0x0014E, 0xFFE0E, 0x00303, 0xFFB0F, 0x008B2, 0xFEF6D, 0x4126A, },
    };

    uint32_t index = ARRAY_SIZE(coef);

    if (sample_rate == AUD_SAMPRATE_44100 || sample_rate == AUD_SAMPRATE_48000) {
        index = 0;
    } else if (sample_rate == AUD_SAMPRATE_176400 || sample_rate == AUD_SAMPRATE_192000) {
        index = 1;
    }

    if (index < ARRAY_SIZE(coef)) {
        // Channel 0
        codec->REG32B_35 = CODEC_FIR_COMP_COEF(coef[index][0]);
        codec->REG32B_36 = CODEC_FIR_COMP_COEF(coef[index][1]);
        codec->REG32B_37 = CODEC_FIR_COMP_COEF(coef[index][2]);
        codec->REG32B_38 = CODEC_FIR_COMP_COEF(coef[index][3]);
        codec->REG32B_39 = CODEC_FIR_COMP_COEF(coef[index][4]);
        codec->REG32B_40 = CODEC_FIR_COMP_COEF(coef[index][5]);
        codec->REG32B_41 = CODEC_FIR_COMP_COEF(coef[index][6]);
        codec->REG32B_42 = CODEC_FIR_COMP_COEF(coef[index][7]);
        // Channel 1
        codec->REG32B_43 = CODEC_FIR_COMP_COEF(coef[index][0]);
        codec->REG32B_44 = CODEC_FIR_COMP_COEF(coef[index][1]);
        codec->REG32B_45 = CODEC_FIR_COMP_COEF(coef[index][2]);
        codec->REG32B_46 = CODEC_FIR_COMP_COEF(coef[index][3]);
        codec->REG32B_47 = CODEC_FIR_COMP_COEF(coef[index][4]);
        codec->REG32B_48 = CODEC_FIR_COMP_COEF(coef[index][5]);
        codec->REG32B_49 = CODEC_FIR_COMP_COEF(coef[index][6]);
        codec->REG32B_50 = CODEC_FIR_COMP_COEF(coef[index][7]);
    } else {
        codec->REG32B_35 = CODEC_DAC_COMPENSATE_BYPASS_CH0;
        codec->REG32B_43 = CODEC_DAC_COMPENSATE_BYPASS_CH1;
    }

}

static void hal_codec_enable_dig_mic(enum AUD_CHANNEL_MAP_T mic_map)
{
    uint32_t phase = 0;
    uint32_t line_map = 0;

    phase = codec->REG32B_19;
    for (int i = 0; i < MAX_DIG_MIC_CH_NUM; i++) {
        if (mic_map & (AUD_CHANNEL_MAP_DIGMIC_CH0 << i)) {
            line_map |= (1 << (i / 2));
        }
        phase = (phase & ~CODEC_19_PDM_CAP_PHASE_CH_MASK(i)) |
            CODEC_19_PDM_CAP_PHASE_CH(i, CODEC_DIGMIC_PHASE);
    }
    codec->REG32B_19 = phase;
    codec->REG32B_09 |= CODEC_09_PDM_ENABLE;
    hal_iomux_set_dig_mic(line_map);
}

#ifndef DIG_MIC_WORKAROUND
static void hal_codec_disable_dig_mic(void)
{
    codec->REG32B_09 &= ~CODEC_09_PDM_ENABLE;
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
                codec->REG32B_09 &= ~CODEC_09_DAC_SDM_SWAP;
                codec->CONFIG |= CODEC_CFG_DUAL_CHANNEL_DAC;
                codec_dac_ch_map = AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1;
            } else {
                ASSERT(cfg->channel_map == AUD_CHANNEL_MAP_CH0 || cfg->channel_map == AUD_CHANNEL_MAP_CH1,
                    "%s: Bad play mono ch map: 0x%X", __func__, cfg->channel_map);
                // Hw resample only supports mono channel on channel L
                if (cfg->channel_map == AUD_CHANNEL_MAP_CH1) {
                    codec->REG32B_09 |= CODEC_09_DAC_SDM_SWAP;
                } else {
                    codec->REG32B_09 &= ~CODEC_09_DAC_SDM_SWAP;
                }
                codec->CONFIG &= ~CODEC_CFG_DUAL_CHANNEL_DAC;
                codec_dac_ch_map = AUD_CHANNEL_MAP_CH0;
            }
        }

        if (HAL_CODEC_CONFIG_BITS & cfg->set_flag) {
            if (cfg->bits == AUD_BITS_16) {
                codec->CONFIG |= CODEC_CFG_MODE_16BIT_DAC;
                codec->MC_CONFIG &= ~MC_CONFIG_MODE_24BIT_DAC;
            } else {
                codec->CONFIG &= ~CODEC_CFG_MODE_16BIT_DAC;
                codec->MC_CONFIG |= MC_CONFIG_MODE_24BIT_DAC;
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

            mask = CODEC_10_RESAMPLE_ENABLE | CODEC_10_RESAMPLE_DUAL_CH | CODEC_10_RESAMPLE_441K;
            val = 0;
            if (hal_cmu_get_audio_resample_status() && codec_dac_sample_rate[rate_idx].codec_freq != CODEC_FREQ_26M) {
                val |= CODEC_10_RESAMPLE_ENABLE;
                if (codec_dac_sample_rate[rate_idx].codec_freq == CODEC_FREQ_44_1K_SERIES) {
                    val |= CODEC_10_RESAMPLE_441K;
                }
                if (codec->CONFIG & CODEC_CFG_DUAL_CHANNEL_DAC) {
                    val |= CODEC_10_RESAMPLE_DUAL_CH;
                }
            }
            codec->REG32B_10 = (codec->REG32B_10 & ~mask) | val;

            // 8K -> 12ms, 16K -> 6ms, ...
            dac_delay_ms = 12 / ((cfg->sample_rate + AUD_SAMPRATE_8000 / 2) / AUD_SAMPRATE_8000);
#else
            // 8K -> 4ms, 16K -> 2ms, ...
            dac_delay_ms = 4 / ((cfg->sample_rate + AUD_SAMPRATE_8000 / 2) / AUD_SAMPRATE_8000);
#endif
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
                analog_aud_pll_set_dig_div(ana_dig_div);
                hal_cmu_codec_dac_set_div(codec_dac_sample_rate[rate_idx].cmu_div);
            }
            hal_codec_set_dac_up(codec_dac_sample_rate[rate_idx].dac_up);
            hal_codec_set_dac_hbf_bypass_cnt(codec_dac_sample_rate[rate_idx].bypass_cnt);

            hal_codec_dac_compensate(cfg->sample_rate);
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

#ifdef DIG_MIC_WORKAROUND
            ASSERT((codec_mic_ch_map & ~AUD_CHANNEL_MAP_DIGMIC_ALL) == 0, "%s: Cap cannot use analog mic: 0x%04X", __func__, mic_map);
#endif

            if (codec_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                hal_codec_enable_dig_mic(mic_map);
            } else if ((anc_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) == 0) {
#ifndef DIG_MIC_WORKAROUND
                hal_codec_disable_dig_mic();
#endif
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
                    codec->REG32B_08 = (codec->REG32B_08 & ~CODEC_08_PDM_MUX_CH_MASK(0)) |
                        CODEC_08_PDM_MUX_CH(0, ch_idx);
                    codec->REG32B_09 |= pdm_mux[0];
                } else {
                    codec->REG32B_17 = SET_BITFIELD(codec->REG32B_17, CODEC_17_ADC_IN_SEL_CH0, ch_idx);
                    codec->REG32B_09 &= ~pdm_mux[0];
                }
            } else if (ANC_FF_MIC_CH_L & AUD_CHANNEL_MAP_ALL) {
                reserv_map |= AUD_CHANNEL_MAP_CH0;
            }
            if (mic_map & ANC_FF_MIC_CH_R) {
                codec_adc_ch_map |= AUD_CHANNEL_MAP_CH1;
                mic_map &= ~ANC_FF_MIC_CH_R;
                ch_idx = get_msb_pos(ANC_FF_MIC_CH_R);
                if (ANC_FF_MIC_CH_R & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                    codec->REG32B_08 = (codec->REG32B_08 & ~CODEC_08_PDM_MUX_CH_MASK(1)) |
                        CODEC_08_PDM_MUX_CH(1, ch_idx);
                    codec->REG32B_09 |= pdm_mux[1];
                } else {
                    codec->REG32B_17 = SET_BITFIELD(codec->REG32B_17, CODEC_17_ADC_IN_SEL_CH1, ch_idx);
                    codec->REG32B_09 &= ~pdm_mux[1];
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
                    codec->REG32B_08 = (codec->REG32B_08 & ~CODEC_08_PDM_MUX_CH_MASK(2)) |
                        CODEC_08_PDM_MUX_CH(2, ch_idx);
                    codec->REG32B_09 |= pdm_mux[2];
                } else {
                    codec->REG32B_17 = SET_BITFIELD(codec->REG32B_17, CODEC_17_ADC_IN_SEL_CH2, ch_idx);
                    codec->REG32B_09 &= ~pdm_mux[2];
                }
            } else if (ANC_FB_MIC_CH_L & AUD_CHANNEL_MAP_ALL) {
                reserv_map |= AUD_CHANNEL_MAP_CH2;
            }
            if (mic_map & ANC_FB_MIC_CH_R) {
                codec_adc_ch_map |= AUD_CHANNEL_MAP_CH3;
                mic_map &= ~ANC_FB_MIC_CH_R;
                ch_idx = get_msb_pos(ANC_FB_MIC_CH_R);
                if (ANC_FB_MIC_CH_R & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                    codec->REG32B_08 = (codec->REG32B_08 & ~CODEC_08_PDM_MUX_CH_MASK(3)) |
                        CODEC_08_PDM_MUX_CH(3, ch_idx);
                    codec->REG32B_09 |= pdm_mux[3];
                } else {
                    codec->REG32B_17 = SET_BITFIELD(codec->REG32B_17, CODEC_17_ADC_IN_SEL_CH3, ch_idx);
                    codec->REG32B_09 &= ~pdm_mux[3];
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
                        codec->REG32B_08 = (codec->REG32B_08 & ~CODEC_08_PDM_MUX_CH_MASK(i)) |
                            CODEC_08_PDM_MUX_CH(i, ch_idx);
                        if (i < ARRAY_SIZE(pdm_mux)) {
                            codec->REG32B_09 |= pdm_mux[i];
                        }
                    } else {
                        codec->REG32B_17 = (codec->REG32B_17 & ~(CODEC_17_ADC_IN_SEL_CH0_MASK << 3 * i)) |
                            (CODEC_17_ADC_IN_SEL_CH0(ch_idx) << 3 * i);
                        if (i < ARRAY_SIZE(pdm_mux)) {
                            codec->REG32B_09 &= ~pdm_mux[i];
                        }
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
                cfg_set_mask = CODEC_CFG_MODE_16BIT_ADC_CH0;
            } else {
                cfg_clr_mask = CODEC_CFG_MODE_16BIT_ADC_CH0;
            }
        }

        cnt = 0;
        for (i = 0; i < MAX_ADC_CH_NUM; i++) {
            if (codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
                cnt++;
                codec->CONFIG |= (cfg_set_mask << i);
                codec->CONFIG &= ~(cfg_clr_mask << i);
            } else {
                codec->CONFIG &= ~(CODEC_CFG_ADC_ENABLE_CH0 << i);
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
            if (hal_cmu_get_audio_resample_status()) {
                ASSERT(codec_adc_sample_rate[rate_idx].codec_freq == CODEC_FREQ_26M,
                    "%s: capture sample rate %u is not for resample", __func__, cfg->sample_rate);
            } else
#endif
            {
#ifdef __AUDIO_RESAMPLE__
                ASSERT(codec_adc_sample_rate[rate_idx].codec_freq != CODEC_FREQ_26M,
                    "%s: capture sample rate %u is for resample only", __func__, cfg->sample_rate);
#endif
                analog_aud_freq_pll_config(codec_adc_sample_rate[rate_idx].codec_freq, codec_adc_sample_rate[rate_idx].codec_div);
                analog_aud_pll_set_dig_div(ana_dig_div);
                hal_cmu_codec_adc_set_div(codec_adc_sample_rate[rate_idx].cmu_div);
            }
            hal_codec_set_adc_down(codec_adc_ch_map, codec_adc_sample_rate[rate_idx].adc_down);
            hal_codec_set_adc_hbf_bypass_cnt(codec_adc_ch_map, codec_adc_sample_rate[rate_idx].bypass_cnt);
        }

#ifndef FIXED_CODEC_ADC_VOL
        if (HAL_CODEC_CONFIG_VOL & cfg->set_flag) {
            hal_codec_set_adc_gain_value(codec_adc_ch_map, cfg->vol);
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
                codec->REG32B_08 = (codec->REG32B_08 & ~CODEC_08_PDM_MUX_CH_MASK(0)) |
                    CODEC_08_PDM_MUX_CH(0, ch_idx);
                codec->REG32B_09 |= pdm_mux[0];
            } else {
                codec->REG32B_17 = SET_BITFIELD(codec->REG32B_17, CODEC_17_ADC_IN_SEL_CH0, ch_idx);
                codec->REG32B_09 &= ~pdm_mux[0];
            }
            map |= AUD_CHANNEL_MAP_CH0;
            mic_map |= ANC_FF_MIC_CH_L;
        }
        if (ANC_FF_MIC_CH_R) {
            ch_idx = get_msb_pos(ANC_FF_MIC_CH_R);
            if (ANC_FF_MIC_CH_R & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                codec->REG32B_08 = (codec->REG32B_08 & ~CODEC_08_PDM_MUX_CH_MASK(1)) |
                    CODEC_08_PDM_MUX_CH(1, ch_idx);
                codec->REG32B_09 |= pdm_mux[1];
            } else {
                codec->REG32B_17 = SET_BITFIELD(codec->REG32B_17, CODEC_17_ADC_IN_SEL_CH1, ch_idx);
                codec->REG32B_09 &= ~pdm_mux[1];
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
                codec->REG32B_08 = (codec->REG32B_08 & ~CODEC_08_PDM_MUX_CH_MASK(2)) |
                    CODEC_08_PDM_MUX_CH(2, ch_idx);
                codec->REG32B_09 |= pdm_mux[2];
            } else {
                codec->REG32B_17 = SET_BITFIELD(codec->REG32B_17, CODEC_17_ADC_IN_SEL_CH2, ch_idx);
                codec->REG32B_09 &= ~pdm_mux[2];
            }
            map |= AUD_CHANNEL_MAP_CH2;
            mic_map |= ANC_FB_MIC_CH_L;
        }
        if (ANC_FB_MIC_CH_R) {
            ch_idx = get_msb_pos(ANC_FB_MIC_CH_R);
            if (ANC_FB_MIC_CH_R & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                codec->REG32B_08 = (codec->REG32B_08 & ~CODEC_08_PDM_MUX_CH_MASK(3)) |
                    CODEC_08_PDM_MUX_CH(3, ch_idx);
                codec->REG32B_09 |= pdm_mux[3];
            } else {
                codec->REG32B_17 = SET_BITFIELD(codec->REG32B_17, CODEC_17_ADC_IN_SEL_CH3, ch_idx);
                codec->REG32B_09 &= ~pdm_mux[3];
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

#ifdef DIG_MIC_WORKAROUND
    ASSERT((anc_mic_ch_map & ~AUD_CHANNEL_MAP_DIGMIC_ALL) == 0, "%s: ANC cannot use analog mic: 0x%04X", __func__, anc_adc_ch_map);
#endif

    if (anc_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) {
        hal_codec_enable_dig_mic(anc_mic_ch_map);
    }

    for (int i = 0; i < MAX_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            if ((codec->REG32B_04 & (CODEC_04_ADC_EN_CH0 << i)) == 0) {
                // Reset ADC channel
                codec->CMU_CTRL2 &= ~CODEC_CMU2_SOFT_RSTN_ADC(i);
                codec->CMU_CTRL2 |= CODEC_CMU2_SOFT_RSTN_ADC(i);
                codec->REG32B_04 |= (CODEC_04_ADC_EN_CH0 << i);
            }
        }
    }

#ifdef DAC_DRE_ENABLE
    if (anc_adc_ch_map && (codec->REG32B_04 & CODEC_04_DAC_EN)) {
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
#ifndef DIG_MIC_WORKAROUND
        hal_codec_disable_dig_mic();
#endif
    }

    for (int i = 0; i < MAX_ADC_CH_NUM; i++) {
        if ((map & (AUD_CHANNEL_MAP_CH0 << i)) && ((codec->CONFIG & CODEC_CFG_ADC_ENABLE) == 0 ||
                (codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) == 0)) {
            codec->REG32B_04 &= ~(CODEC_04_ADC_EN_CH0 << i);
        }
    }

#ifdef DAC_DRE_ENABLE
    if (anc_adc_ch_map == 0 && (codec->REG32B_04 & CODEC_04_DAC_EN) &&
            (codec->CONFIG & CODEC_CFG_MODE_16BIT_DAC) == 0) {
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
        codec->REG32B_18 = SET_BITFIELD(codec->REG32B_18, DRE_ADC_INI_ANA_GAIN, 0xF);
        codec->REG32B_20 = SET_BITFIELD(codec->REG32B_20, DRE_ADC_INI_ANA_GAIN, 0xF);
        codec->REG32B_22 = SET_BITFIELD(codec->REG32B_22, DRE_ADC_INI_ANA_GAIN, 0xF);
        codec->REG32B_29 = SET_BITFIELD(codec->REG32B_29, DRE_ADC_INI_ANA_GAIN, 0xF);
        codec->REG32B_31 = SET_BITFIELD(codec->REG32B_31, DRE_ADC_INI_ANA_GAIN, 0xF);
    } else {
        codec->REG32B_18 = SET_BITFIELD(codec->REG32B_18, DRE_ADC_INI_ANA_GAIN, 0);
        codec->REG32B_20 = SET_BITFIELD(codec->REG32B_20, DRE_ADC_INI_ANA_GAIN, 0);
        codec->REG32B_22 = SET_BITFIELD(codec->REG32B_22, DRE_ADC_INI_ANA_GAIN, 0);
        codec->REG32B_29 = SET_BITFIELD(codec->REG32B_29, DRE_ADC_INI_ANA_GAIN, 0);
        codec->REG32B_31 = SET_BITFIELD(codec->REG32B_31, DRE_ADC_INI_ANA_GAIN, 0);
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
    // DC calib values are based on 16-bit, but hardware compensation is based on 20-bit
    dac_dc_l = dc_l << 4;
    dac_dc_r = dc_r << 4;
}
#endif

void hal_codec_set_dac_reset_callback(HAL_CODEC_DAC_RESET_CALLBACK callback)
{
    //dac_reset_callback = callback;
}

void hal_codec_sync_dac_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
#if defined(__TWS__) && defined(__BT_ANC__)
    codec->SYNC_PLAY = SET_BITFIELD(codec->SYNC_PLAY, SYNC_DAC_ENABLE_SEL, type) | SYNC_STAMP_CLR_USED;
#else
    codec->SYNC_PLAY = SET_BITFIELD(codec->SYNC_PLAY, SYNC_CODEC_DAC_ENABLE_SEL, type) | SYNC_STAMP_CLR_USED;
#endif
}

void hal_codec_sync_dac_disable(void)
{
#if defined(__TWS__) && defined(__BT_ANC__)
    codec->SYNC_PLAY = SET_BITFIELD(codec->SYNC_PLAY, SYNC_DAC_ENABLE_SEL, HAL_CODEC_SYNC_TYPE_NONE);
#else
    codec->SYNC_PLAY = SET_BITFIELD(codec->SYNC_PLAY, SYNC_CODEC_DAC_ENABLE_SEL, HAL_CODEC_SYNC_TYPE_NONE);
#endif
}

void hal_codec_sync_adc_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
    codec->SYNC_PLAY = SET_BITFIELD(codec->SYNC_PLAY, SYNC_CODEC_ADC_ENABLE_SEL, type) | SYNC_STAMP_CLR_USED;
}

void hal_codec_sync_adc_disable(void)
{
    codec->SYNC_PLAY = SET_BITFIELD(codec->SYNC_PLAY, SYNC_CODEC_ADC_ENABLE_SEL, HAL_CODEC_SYNC_TYPE_NONE);
}

int hal_codec_dac_reset_set(void)
{
    int ret;

    if (codec_opened) {
        ret = !!(codec->CMU_CTRL2 & CODEC_CMU2_SOFT_RSTN_DAC);
        codec->CMU_CTRL2 &= ~CODEC_CMU2_SOFT_RSTN_DAC;
    } else {
        ret = 0;
    }

    return ret;
}

int hal_codec_dac_reset_clear(void)
{
    int ret;

    if (codec_opened) {
        ret = !!(codec->CMU_CTRL2 & CODEC_CMU2_SOFT_RSTN_DAC);
        codec->CMU_CTRL2 |= CODEC_CMU2_SOFT_RSTN_DAC;
    } else {
        ret = 0;
    }

    return ret;
}

void hal_codec_tune_resample_rate(enum AUD_STREAM_T stream, float factor)
{
#ifdef __AUDIO_RESAMPLE__
    enum RESAMPLE_SLOW_VAL_T {
        RESAMPLE_SLOW_NONE          = 0,

        RESAMPLE_SLOW_44P1K_118PPM  = 1,
        RESAMPLE_SLOW_44P1K_237PPM  = 2,
        RESAMPLE_SLOW_44P1K_355PPM  = 3,
        RESAMPLE_SLOW_44P1K_473PPM  = 4,
        RESAMPLE_SLOW_44P1K_592PPM  = 5,
        RESAMPLE_SLOW_44P1K_710PPM  = 6,
        RESAMPLE_SLOW_44P1K_829PPM  = 7,

        RESAMPLE_SLOW_48K_57PPM     = 1,
        RESAMPLE_SLOW_48K_113PPM    = 2,
        RESAMPLE_SLOW_48K_170PPM    = 3,
        RESAMPLE_SLOW_48K_226PPM    = 4,
        RESAMPLE_SLOW_48K_283PPM    = 5,
        RESAMPLE_SLOW_48K_340PPM    = 6,
        RESAMPLE_SLOW_48K_396PPM    = 7,
    };

    enum RESAMPLE_SLOW_VAL_T val;
    static const float factor_44p1k_tbl[7] = {
        0.999881629, 0.999763258, 0.999644886, 0.999526515, 0.999408144, 0.999289773, 0.999171402,
    };
    static const float factor_48k_tbl[7] = {
        0.999943388, 0.999886775, 0.999830163, 0.999773551, 0.999716938, 0.999660326, 0.999603714,
    };
    const float *tbl;

    if (!codec_opened) {
        return;
    }

    if (stream != AUD_STREAM_PLAYBACK) {
        return;
    }

    if ((codec->REG32B_10 & CODEC_10_RESAMPLE_ENABLE) == 0) {
        return;
    }

    if (codec->REG32B_10 & CODEC_10_RESAMPLE_441K) {
        tbl = factor_44p1k_tbl;
    } else {
        tbl = factor_48k_tbl;
    }

    if (factor >= 1.0) {
        val = RESAMPLE_SLOW_NONE;
    } else if (factor >= (tbl[0] + tbl[1]) / 2) {
        val = RESAMPLE_SLOW_48K_57PPM;
    } else if (factor >= (tbl[1] + tbl[2]) / 2) {
        val = RESAMPLE_SLOW_48K_113PPM;
    } else if (factor >= (tbl[2] + tbl[3]) / 2) {
        val = RESAMPLE_SLOW_48K_170PPM;
    } else if (factor >= (tbl[3] + tbl[4]) / 2) {
        val = RESAMPLE_SLOW_48K_226PPM;
    } else if (factor >= (tbl[4] + tbl[5]) / 2) {
        val = RESAMPLE_SLOW_48K_283PPM;
    } else if (factor >= (tbl[5] + tbl[6]) / 2) {
        val = RESAMPLE_SLOW_48K_340PPM;
    } else {
        val = RESAMPLE_SLOW_48K_396PPM;
    }

    if (val == RESAMPLE_SLOW_NONE) {
        codec->REG32B_18 &= ~CODEC_18_RESAMPLE_SLOW_EN;
    } else {
        codec->REG32B_18 |= CODEC_18_RESAMPLE_SLOW_EN;
    }
    codec->REG32B_18 = SET_BITFIELD(codec->REG32B_18, CODEC_18_RESAMPLE_SLOW_VAL, val);
#endif
}

