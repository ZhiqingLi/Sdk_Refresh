#include "plat_types.h"
#include "plat_addr_map.h"
#include "reg_codecip_best1000.h"
#include "hal_codecip_best1000.h"
#include "hal_codec.h"
#include "hal_uart.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_cmu.h"
#include "hal_aud.h"
#include "hal_chipid.h"
#include "analog.h"
#include "hal_iomux.h"
#include "tgt_hardware.h"
#if !defined(NOSTD) && defined(CODEC_ACCURATE_SW_GAIN)
#include <math.h>
#endif

#define HAL_CODEC_TRACE()    //TRACE("%s:%d\n", __func__, __LINE__)

#define FIXED_CODEC_ADC_VOL

#define HAL_CODEC_TX_FIFO_TRIGGER_LEVEL     (3)
#define HAL_CODEC_RX_FIFO_TRIGGER_LEVEL     (CODECIP_FIFO_DEPTH/2)

#define HAL_CODEC_YES 1
#define HAL_CODEC_NO 0

#define MAX_DIG_DAC_DBVAL                   (MAX_DIG_DAC_REGVAL - ZERODB_DIG_DAC_REGVAL)
#define ZERODB_DIG_DAC_DBVAL                (0)
#define MIN_DIG_DAC_DBVAL                   (-99)

#define MAX_DIG_DAC_REGVAL                  (31)
#ifndef ZERODB_DIG_DAC_REGVAL
#define ZERODB_DIG_DAC_REGVAL               (26)
#endif
#define MIN_DIG_DAC_REGVAL                  (0)
#define ALG_DAC_DBVAL_PER_SHIFT             (6)

static bool codec_opened = false;
static uint16_t digdac_gain;
#ifdef ANC_APP
static int16_t digdac_gain_offset_anc;
#endif
#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
static bool codec_nr_enabled;
static int8_t digdac_gain_offset_nr;
#endif
#ifdef AUDIO_OUTPUT_DC_CALIB
static float dac_dc_gain_attn;
#endif
static uint8_t algdac_shift;
static uint8_t prev_anadac_gain;

#ifdef AUDIO_OUTPUT_SW_GAIN
static HAL_CODEC_SW_OUTPUT_COEF_CALLBACK sw_output_coef_callback;
#endif

static HAL_CODEC_DAC_RESET_CALLBACK dac_reset_callback;

static const char * const invalid_id = "Invalid CODEC ID: %d\n";
//static const char * const invalid_ch = "Invalid CODEC CH: %d\n";

struct CODEC_DAC_SAMPLE_RATE_T{
    enum AUD_SAMPRATE_T sample_rate;
    uint32_t codec_freq;
    uint8_t codec_div;
    uint8_t cmu_div;
    uint8_t dac_up;
    uint16_t sdac_osr;
};

static const struct CODEC_DAC_SAMPLE_RATE_T codec_dac_sample_rate[]={
#ifdef __AUDIO_RESAMPLE__
    {AUD_SAMPRATE_8463,   CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV, 6, 512},
    {AUD_SAMPRATE_16927,  CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV, 3, 512},
    {AUD_SAMPRATE_50781,  CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV, 1, 512},
#endif
#ifdef AUD_PLL_DOUBLE
    {AUD_SAMPRATE_88200,  CODEC_FREQ_44_1K_SERIES*2, CODEC_PLL_DIV, CODEC_PLL_DIV, 1, 512},
    {AUD_SAMPRATE_96000,  CODEC_FREQ_48K_SERIES*2,   CODEC_PLL_DIV, CODEC_PLL_DIV, 1, 512},
    {AUD_SAMPRATE_192000, CODEC_FREQ_48K_SERIES*2,   CODEC_PLL_DIV, CODEC_PLL_DIV, 1, 256},
    {AUD_SAMPRATE_384000, CODEC_FREQ_48K_SERIES*2,   CODEC_PLL_DIV, CODEC_PLL_DIV, 1, 128},
#else
    {AUD_SAMPRATE_8000,   CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV, 6, 512},
    {AUD_SAMPRATE_16000,  CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV, 3, 512},
    {AUD_SAMPRATE_22050,  CODEC_FREQ_44_1K_SERIES,   CODEC_PLL_DIV, CODEC_PLL_DIV, 2, 512},
    {AUD_SAMPRATE_24000,  CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV, 2, 512},
    {AUD_SAMPRATE_44100,  CODEC_FREQ_44_1K_SERIES,   CODEC_PLL_DIV, CODEC_PLL_DIV, 1, 512},
    {AUD_SAMPRATE_48000,  CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV, 1, 512},
    {AUD_SAMPRATE_88200,  CODEC_FREQ_44_1K_SERIES,   CODEC_PLL_DIV, CODEC_PLL_DIV, 1, 256},
    {AUD_SAMPRATE_96000,  CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV, 1, 256},
    {AUD_SAMPRATE_176400, CODEC_FREQ_44_1K_SERIES,   CODEC_PLL_DIV, CODEC_PLL_DIV, 1, 128},
    {AUD_SAMPRATE_192000, CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV, 1, 128},
#endif
};

struct CODEC_ADC_SAMPLE_RATE_T {
    enum AUD_SAMPRATE_T sample_rate;
    uint32_t codec_freq;
    uint8_t codec_div;
    uint8_t cmu_div;
    uint8_t adc_down;
    uint8_t clk_x4;
};

static const struct CODEC_ADC_SAMPLE_RATE_T codec_adc_sample_rate[]={
#ifdef __AUDIO_RESAMPLE__
    {AUD_SAMPRATE_8463,   CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV,   6, 0},
    {AUD_SAMPRATE_16927,  CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV,   3, 0},
    {AUD_SAMPRATE_50781,  CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV,   1, 0},
#endif
#ifdef AUD_PLL_DOUBLE
    {AUD_SAMPRATE_96000,  CODEC_FREQ_48K_SERIES*2,   CODEC_PLL_DIV, CODEC_PLL_DIV,   1, 0},
    {AUD_SAMPRATE_176400, CODEC_FREQ_44_1K_SERIES*2, CODEC_PLL_DIV, CODEC_PLL_DIV/2, 1, 0},
    {AUD_SAMPRATE_192000, CODEC_FREQ_48K_SERIES*2,   CODEC_PLL_DIV, CODEC_PLL_DIV/2, 1, 0},
    {AUD_SAMPRATE_384000, CODEC_FREQ_48K_SERIES*2,   CODEC_PLL_DIV, CODEC_PLL_DIV,   1, 1},
    {AUD_SAMPRATE_768000, CODEC_FREQ_48K_SERIES*2,   CODEC_PLL_DIV, CODEC_PLL_DIV/2, 1, 1},
#else
    {AUD_SAMPRATE_8000,   CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV,   6, 0},
    {AUD_SAMPRATE_16000,  CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV,   3, 0},
    {AUD_SAMPRATE_44100,  CODEC_FREQ_44_1K_SERIES,   CODEC_PLL_DIV, CODEC_PLL_DIV,   1, 0},
    {AUD_SAMPRATE_48000,  CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV,   1, 0},
    {AUD_SAMPRATE_88200,  CODEC_FREQ_44_1K_SERIES,   CODEC_PLL_DIV, CODEC_PLL_DIV/2, 1, 0},
    {AUD_SAMPRATE_96000,  CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV/2, 1, 0},
    {AUD_SAMPRATE_176400, CODEC_FREQ_44_1K_SERIES,   CODEC_PLL_DIV, CODEC_PLL_DIV,   1, 1},
    {AUD_SAMPRATE_192000, CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV,   1, 1},
    {AUD_SAMPRATE_352800, CODEC_FREQ_44_1K_SERIES,   CODEC_PLL_DIV, CODEC_PLL_DIV/2, 1, 1},
    {AUD_SAMPRATE_384000, CODEC_FREQ_48K_SERIES,     CODEC_PLL_DIV, CODEC_PLL_DIV/2, 1, 1},
#endif
};

static int hal_codec_set_dac_osr(enum HAL_CODEC_ID_T id, uint32_t val);
static uint32_t hal_codec_get_dac_osr(enum HAL_CODEC_ID_T id);
static int hal_codec_set_dac_up(enum HAL_CODEC_ID_T id, uint32_t val);
static uint32_t hal_codec_get_dac_up(enum HAL_CODEC_ID_T id);
static int hal_codec_set_adc_down(enum HAL_CODEC_ID_T id, uint32_t val);
static uint32_t hal_codec_get_adc_down(enum HAL_CODEC_ID_T id);
static const struct CODEC_DAC_VOL_T *hal_codec_get_dac_volume(uint32_t index);
static void hal_codec_set_dig_dac_gain(uint32_t reg_base, int32_t gain);
#ifdef AUDIO_OUTPUT_SW_GAIN
static void hal_codec_set_sw_gain(int32_t gain);
#endif

static inline uint32_t _codec_get_reg_base(enum HAL_CODEC_ID_T id)
{
    ASSERT(id < HAL_CODEC_ID_NUM, invalid_id, id);
    switch(id) {
        case HAL_CODEC_ID_0:
        default:
            return CODEC_BASE;
            break;
    }
    return 0;
}

static inline uint32_t _codec_get_fir_reg_base(enum HAL_CODEC_ID_T id)
{
    ASSERT(id < HAL_CODEC_ID_NUM, invalid_id, id);
    switch(id) {
        case HAL_CODEC_ID_0:
        default:
            return CODEC_BASE + 0x8000;
            break;
    }
    return 0;
}

int hal_codec_open(enum HAL_CODEC_ID_T id)
{
    uint32_t reg_base;

    analog_aud_pll_open(ANA_AUD_PLL_USER_CODEC);

    hal_cmu_codec_clock_enable();
    hal_cmu_clock_enable(HAL_CMU_MOD_P_CODEC);
    hal_cmu_reset_clear(HAL_CMU_MOD_P_CODEC);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_CODEC_DA);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_CODEC_AD);

    codec_opened = true;

    reg_base = _codec_get_reg_base(id);

    codecipbus_w_ff_fb_disable(reg_base, HAL_CODEC_YES);
    codecipbus_w_dac_if_en(reg_base, HAL_CODEC_NO);
    codecipbus_w_adc_if_en(reg_base, HAL_CODEC_NO);
    codecipbus_w_enable_tx_dma(reg_base, HAL_CODEC_NO);
    codecipbus_w_enable_rx_dma(reg_base, HAL_CODEC_NO);
    codecipbus_w_tx_fifo_reset(reg_base, HAL_CODEC_YES);
    codecipbus_w_tx_fifo_reset(reg_base, HAL_CODEC_NO);
    codecipbus_w_rx_fifo_reset(reg_base, HAL_CODEC_YES);
    codecipbus_w_rx_fifo_reset(reg_base, HAL_CODEC_NO);

    codecipbus_w_en(reg_base, HAL_CODEC_YES);
    codecipbus_w_codec_if_en(reg_base, HAL_CODEC_YES);

#ifdef FIXED_CODEC_ADC_VOL
    codecipbus_w_sadc_vol_ch0_sel(reg_base, CODEC_SADC_VOL);
    codecipbus_w_sadc_vol_ch1_sel(reg_base, CODEC_SADC_VOL);
#endif

#ifdef AUDIO_OUTPUT_SW_GAIN
    const struct CODEC_DAC_VOL_T *vol_tab_ptr;

    // Init gain settings
    vol_tab_ptr = hal_codec_get_dac_volume(0);
    if (vol_tab_ptr) {
        analog_aud_set_dac_gain(vol_tab_ptr->tx_pa_gain);
        codecipbus_w_sdm_gain_sel(reg_base, vol_tab_ptr->sdm_gain);
        hal_codec_set_dig_dac_gain(reg_base, ZERODB_DIG_DAC_DBVAL);
    }
#else
    // Enable zero-crossing gain adjustment
    codecip_w_dac_gain_sel(reg_base, HAL_CODEC_YES);
#endif

    return 0;
}

int hal_codec_close(enum HAL_CODEC_ID_T id)
{
    uint32_t reg_base;

    reg_base = _codec_get_reg_base(id);

    codecipbus_w_en(reg_base, HAL_CODEC_NO);
    codecipbus_w_codec_if_en(reg_base, HAL_CODEC_NO);

    codecipbus_w_dac_if_en(reg_base, HAL_CODEC_NO);
    codecipbus_w_adc_if_en(reg_base, HAL_CODEC_NO);
    codecipbus_w_enable_tx_dma(reg_base, HAL_CODEC_NO);
    codecipbus_w_enable_rx_dma(reg_base, HAL_CODEC_NO);
    codecipbus_w_tx_fifo_reset(reg_base, HAL_CODEC_YES);
    codecipbus_w_tx_fifo_reset(reg_base, HAL_CODEC_NO);
    codecipbus_w_rx_fifo_reset(reg_base, HAL_CODEC_YES);
    codecipbus_w_rx_fifo_reset(reg_base, HAL_CODEC_NO);

    codec_opened = false;

    hal_cmu_reset_set(HAL_CMU_MOD_O_CODEC_AD);
    hal_cmu_reset_set(HAL_CMU_MOD_O_CODEC_DA);
    // NEVER reset CODEC registers, for the CODEC driver expects that last configurations
    // still exist in the next stream setup
    //hal_cmu_reset_set(HAL_CMU_MOD_P_CODEC);
    hal_cmu_clock_disable(HAL_CMU_MOD_P_CODEC);
    hal_cmu_codec_clock_disable();

    analog_aud_pll_close(ANA_AUD_PLL_USER_CODEC);

    return 0;
}

#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
void hal_codec_set_noise_reduction(bool enable)
{
    uint32_t ana_gain;

    if (codec_nr_enabled == enable) {
        // Avoid corrupting prev_anadac_gain or using an invalid one
        return;
    }

    codec_nr_enabled = enable;

    if (!codec_opened) {
        return;
    }

    if (enable) {
        prev_anadac_gain = analog_codec_get_dac_gain();
        // Distinguish -60dB DAC output mode and noise reduction mode
        if (prev_anadac_gain > 0) {
            ana_gain = 1;
        } else {
            ana_gain = 0;
        }
        digdac_gain_offset_nr = analog_codec_dac_gain_to_db(prev_anadac_gain - ana_gain);
    } else {
        ana_gain = prev_anadac_gain;
        digdac_gain_offset_nr = 0;
    }
    analog_aud_set_dac_gain(ana_gain);

#ifdef AUDIO_OUTPUT_SW_GAIN
    hal_codec_set_sw_gain(digdac_gain);
#else
    uint32_t reg_base = _codec_get_reg_base(HAL_CODEC_ID_0);
    hal_codec_set_dig_dac_gain(reg_base, digdac_gain);
#endif
}
#endif

void hal_codec_stop_playback_stream(enum HAL_CODEC_ID_T id)
{
    uint32_t reg_base;
    volatile uint32_t *fir_reg;
    uint32_t fir_val[16];
    int fir_valid;
    int i;

    reg_base = _codec_get_reg_base(id);
    fir_reg = (volatile uint32_t *)_codec_get_fir_reg_base(id);

    if (dac_reset_callback) {
        dac_reset_callback(HAL_CODEC_DAC_PRE_RESET);
    }

    fir_valid = hal_cmu_anc_get_status(HAL_CMU_ANC_CLK_USER_QTY);
    if (fir_valid) {
        // Save fir regs
        for (i = 0; i < ARRAY_SIZE(fir_val); i++) {
            fir_val[i] = fir_reg[i];
        }
    }

    // Disable DAC
    codecip_w_dac_en(reg_base, HAL_CODEC_NO);

    // Reset DAC
    // Avoid DAC outputing noise after it is disabled
    hal_cmu_reset_pulse(HAL_CMU_MOD_O_CODEC_DA);

    if (fir_valid) {
        // Restore fir regs
        for (i = 0; i < ARRAY_SIZE(fir_val); i++) {
            fir_reg[i] = fir_val[i];
        }
    }

    if (dac_reset_callback) {
        dac_reset_callback(HAL_CODEC_DAC_POST_RESET);
    }
}
void hal_codec_start_playback_stream(enum HAL_CODEC_ID_T id)
{
    uint32_t reg_base;

    reg_base = _codec_get_reg_base(id);

    // Enable DAC
    codecip_w_dac_en(reg_base, HAL_CODEC_YES);
}
int hal_codec_start_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream)
{
    uint32_t reg_base;

    reg_base = _codec_get_reg_base(id);

    if (stream == AUD_STREAM_PLAYBACK) {
        // Reset DAC
        hal_codec_stop_playback_stream(id);
        // Start DAC
        hal_codec_start_playback_stream(id);
    }
    else {
        hal_cmu_reset_pulse(HAL_CMU_MOD_O_CODEC_AD);
        codecip_w_adc_en(reg_base, HAL_CODEC_YES);
    }

    return 0;
}
int hal_codec_stop_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream)
{
    uint32_t reg_base;

    reg_base = _codec_get_reg_base(id);

    if (stream == AUD_STREAM_PLAYBACK) {
        // Stop and reset DAC
        hal_codec_stop_playback_stream(id);
    }
    else {
        codecip_w_adc_en(reg_base, HAL_CODEC_NO);
    }

    return 0;
}
int hal_codec_start_interface(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream, int dma)
{
    uint32_t reg_base;

    reg_base = _codec_get_reg_base(id);

    if (stream == AUD_STREAM_PLAYBACK) {
        codecipbus_w_tx_fifo_reset(reg_base, HAL_CODEC_YES);
        codecipbus_w_tx_fifo_reset(reg_base, HAL_CODEC_NO);
        if (dma) {
            codecipbus_w_tx_fifo_threshold(reg_base, HAL_CODEC_TX_FIFO_TRIGGER_LEVEL);
            codecipbus_w_enable_tx_dma(reg_base, HAL_CODEC_YES);
            // Delay a little time for DMA to fill the TX FIFO before sending
            for (volatile int i = 0; i < 50; i++);
        }
        codecipbus_w_dac_if_en(reg_base, HAL_CODEC_YES);
    }
    else {
        codecipbus_w_rx_fifo_reset(reg_base, HAL_CODEC_YES);
        codecipbus_w_rx_fifo_reset(reg_base, HAL_CODEC_NO);
        if (dma) {
            codecipbus_w_rx_fifo_threshold(reg_base, HAL_CODEC_RX_FIFO_TRIGGER_LEVEL);
            codecipbus_w_enable_rx_dma(reg_base, HAL_CODEC_YES);
        }
        codecipbus_w_adc_if_en(reg_base, HAL_CODEC_YES);
    }

    return 0;
}
int hal_codec_stop_interface(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream)
{
    uint32_t reg_base;

    reg_base = _codec_get_reg_base(id);

    if (stream == AUD_STREAM_PLAYBACK) {
        codecipbus_w_dac_if_en(reg_base, HAL_CODEC_NO);
        codecipbus_w_enable_tx_dma(reg_base, HAL_CODEC_NO);
        codecipbus_w_tx_fifo_reset(reg_base, HAL_CODEC_YES);
        codecipbus_w_tx_fifo_reset(reg_base, HAL_CODEC_NO);
    }
    else {
        codecipbus_w_adc_if_en(reg_base, HAL_CODEC_NO);
        codecipbus_w_enable_rx_dma(reg_base, HAL_CODEC_NO);
        codecipbus_w_rx_fifo_reset(reg_base, HAL_CODEC_YES);
        codecipbus_w_rx_fifo_reset(reg_base, HAL_CODEC_NO);
    }

    return 0;
}

void hal_codec_dac_mute(enum HAL_CODEC_ID_T id)
{
#ifdef AUDIO_OUTPUT_SW_GAIN
    hal_codec_set_sw_gain(MIN_DIG_DAC_DBVAL);
#else
    uint32_t reg_base;

    reg_base = _codec_get_reg_base(id);

    codecipbus_w_sdac_lvolume_sel(reg_base, MIN_DIG_DAC_REGVAL);
    if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2) {
        codecipbus_w_sdac_rvolume_sel(reg_base, MIN_DIG_DAC_REGVAL);
    }
#endif
}

static const struct CODEC_DAC_VOL_T *hal_codec_get_dac_volume(uint32_t index)
{
    if (index < ARRAY_SIZE(codec_dac_vol)) {
        return &codec_dac_vol[index];
    } else {
        return NULL;
    }
}

static void hal_codec_set_dig_dac_gain(uint32_t reg_base, int32_t gain)
{
    uint32_t val;
    int32_t remains;

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

#ifdef MAX_DAC_OUTPUT_M60DB
    // Minus 1 to ensure the output signal is below -60 dBV
    gain = -60 - analog_codec_dac_max_attn_db() - 1;
#endif

    if (gain >= ZERODB_DIG_DAC_DBVAL) {
        if (gain > MAX_DIG_DAC_DBVAL) {
            gain = MAX_DIG_DAC_DBVAL;
        }
        val = ZERODB_DIG_DAC_REGVAL + (gain - ZERODB_DIG_DAC_DBVAL);
        algdac_shift = 0;
    } else if (gain <= MIN_DIG_DAC_DBVAL) {
        val = MIN_DIG_DAC_REGVAL;
        // Keep the original algdac_shift
    } else {
        remains = ZERODB_DIG_DAC_REGVAL - (ZERODB_DIG_DAC_DBVAL - gain);
        if (remains >= 0) {
            val = remains;
            algdac_shift = 0;
        } else {
            val = -remains;
            algdac_shift = val / ALG_DAC_DBVAL_PER_SHIFT + 1;
            val = val % ALG_DAC_DBVAL_PER_SHIFT;
            val = ALG_DAC_DBVAL_PER_SHIFT - val;
        }
    }

    //TRACE("hal_codec: set digdac gain=%d shift=%u val=%u", gain, algdac_shift, val);

    codecipbus_w_sdac_lvolume_sel(reg_base, val);
    if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2) {
        codecipbus_w_sdac_rvolume_sel(reg_base, val);
    }
}

#ifdef AUDIO_OUTPUT_SW_GAIN
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

#ifdef MAX_DAC_OUTPUT_M60DB
    // Minus 1 to ensure the output signal is below -60 dBV
    gain = -60 - analog_codec_dac_max_attn_db() - 1;
#endif

    coef = db_to_amplitude_ratio(gain);

#ifdef AUDIO_OUTPUT_DC_CALIB
    coef *= dac_dc_gain_attn;
#endif

#if (ZERODB_DIG_DAC_REGVAL >= 26)
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

static int hal_codec_set_dac_osr(enum HAL_CODEC_ID_T id, uint32_t val)
{
    uint32_t sel = 0;
    uint32_t reg_base;
    reg_base = _codec_get_reg_base(id);

    if (val == 128) {
        sel = 0;
    } else if (val == 256) {
        sel = 1;
    } else if (val == 512) {
        sel = 2;
    } else {
        ASSERT(false, "%s: Invalid dac osr: %u", __FUNCTION__, val);
    }
    codecipbus_w_sdac_osr_sel(reg_base, sel);
    return 0;
}

static uint32_t POSSIBLY_UNUSED hal_codec_get_dac_osr(enum HAL_CODEC_ID_T id)
{
    uint32_t sel;
    uint32_t reg_base;
    reg_base = _codec_get_reg_base(id);

    sel = codecipbus_r_sdac_osr_sel(reg_base);
    if (sel == 0) {
        return 128;
    } else if (sel == 1) {
        return 256;
    } else {
        return 512;
    }
}

static int hal_codec_set_dac_up(enum HAL_CODEC_ID_T id, uint32_t val)
{
    uint32_t sel = 0;
    uint32_t reg_base;
    reg_base = _codec_get_reg_base(id);

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
    codecipbus_w_dac_up_sel(reg_base, sel);
    return 0;
}

static uint32_t POSSIBLY_UNUSED hal_codec_get_dac_up(enum HAL_CODEC_ID_T id)
{
    uint32_t sel;
    uint32_t reg_base;
    reg_base = _codec_get_reg_base(id);

    sel = codecipbus_r_dac_up_sel(reg_base);
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

static int hal_codec_set_adc_down(enum HAL_CODEC_ID_T id, uint32_t val)
{
    uint32_t sel = 0;
    uint32_t reg_base;
    reg_base = _codec_get_reg_base(id);

    if (val == 3) {
        sel = 0;
    } else if (val == 6) {
        sel = 1;
    } else if (val == 1) {
        sel = 2;
    } else {
        ASSERT(false, "%s: Invalid adc down: %u", __FUNCTION__, val);
    }
    codecip_w_adc_down_sel(reg_base, sel);
    return 0;
}

static uint32_t POSSIBLY_UNUSED hal_codec_get_adc_down(enum HAL_CODEC_ID_T id)
{
    uint32_t sel;
    uint32_t reg_base;
    reg_base = _codec_get_reg_base(id);

    sel = codecip_r_adc_down_sel(reg_base);
    if (sel == 0) {
        return 3;
    } else if (sel == 1) {
        return 6;
    } else {
        return 1;
    }
}

int hal_codec_setup_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream, const struct HAL_CODEC_CONFIG_T *cfg)
{
    uint32_t reg_base;

    reg_base = _codec_get_reg_base(id);

    if (stream == AUD_STREAM_PLAYBACK)
    {
        //set playback channel num
        //Note: best1000 channel num is 2
        if(HAL_CODEC_CONFIG_CHANNEL_NUM & cfg->set_flag)
        {
            HAL_CODEC_TRACE();
            if (cfg->channel_num == AUD_CHANNEL_NUM_2)
            {
                codecipbus_w_dual_dac_en(reg_base, HAL_CODEC_YES);  //select dual channel dac
                codecip_w_dual_dac_en(reg_base, HAL_CODEC_YES);     //enable dual channel dac
            }
            else
            {
                codecipbus_w_dual_dac_en(reg_base, HAL_CODEC_NO);   //select signal channel dac
                codecip_w_dual_dac_en(reg_base, HAL_CODEC_NO);      //disable dual channel dac
            }
        }

        //set playback bits
        //Note: best1000 just use AUD_BITS_16
        if(HAL_CODEC_CONFIG_BITS & cfg->set_flag)
        {
            HAL_CODEC_TRACE();
            if (cfg->bits == AUD_BITS_16)
            {
                codecipbus_w_dac_16bit_en(reg_base, HAL_CODEC_YES);
            }
            else
            {
                codecipbus_w_dac_16bit_en(reg_base, HAL_CODEC_NO);
            }
        }

        //set playback sample rate
        if(HAL_CODEC_CONFIG_SAMPLE_RATE & cfg->set_flag)
        {
            uint8_t i;

#if (CODEC_PLL_DIV == 8)
            ASSERT(hal_get_chip_metal_id() != HAL_CHIP_METAL_ID_2, "[%s] VER D chips cannot support div 8", __func__);
#endif
            for(i = 0; i < ARRAY_SIZE(codec_dac_sample_rate); i++)
            {
                if(codec_dac_sample_rate[i].sample_rate == cfg->sample_rate)
                {
                    break;
                }
            }
            ASSERT(i < ARRAY_SIZE(codec_dac_sample_rate), "%s: Invalid playback sample rate: %d", __func__, cfg->sample_rate);

            TRACE("[%s] stream=%d sample_rate=%d", __func__, stream, cfg->sample_rate);
            analog_aud_freq_pll_config(codec_dac_sample_rate[i].codec_freq, codec_dac_sample_rate[i].codec_div);
            hal_cmu_codec_dac_set_div(codec_dac_sample_rate[i].cmu_div);
            hal_codec_set_dac_up(id, codec_dac_sample_rate[i].dac_up);
            hal_codec_set_dac_osr(id, codec_dac_sample_rate[i].sdac_osr);
        }

        //set playback volume
        if(HAL_CODEC_CONFIG_VOL & cfg->set_flag)
        {
            HAL_CODEC_TRACE();

            const struct CODEC_DAC_VOL_T *vol_tab_ptr;

            vol_tab_ptr = hal_codec_get_dac_volume(cfg->vol);
            if (vol_tab_ptr)
            {
#ifdef AUDIO_OUTPUT_SW_GAIN
                hal_codec_set_sw_gain(vol_tab_ptr->sdac_volume);
#else
                analog_aud_set_dac_gain(vol_tab_ptr->tx_pa_gain);
                codecipbus_w_sdm_gain_sel(reg_base, vol_tab_ptr->sdm_gain);
                hal_codec_set_dig_dac_gain(reg_base, vol_tab_ptr->sdac_volume);
#endif
            }
        }
    }
    else
    {
        //set capture channel num
        if(HAL_CODEC_CONFIG_CHANNEL_NUM & cfg->set_flag)
        {
            HAL_CODEC_TRACE();
            if (cfg->channel_num == AUD_CHANNEL_NUM_2)
            {
                codecipbus_w_dual_mic_en(reg_base, HAL_CODEC_YES);  //select dual channel adc
                codecip_w_dual_mic_en(reg_base, HAL_CODEC_YES);     //enable dual channel adc
            }
            else
            {
                codecipbus_w_dual_mic_en(reg_base, HAL_CODEC_NO);
                codecip_w_dual_mic_en(reg_base, HAL_CODEC_NO);
            }
        }

        //select capture input path
        if(HAL_CODEC_CONFIG_CHANNEL_MAP & cfg->set_flag)
        {
            uint32_t v;

            HAL_CODEC_TRACE();
            if (cfg->channel_map & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                hal_iomux_set_dig_mic(0);
                v = HAL_CODEC_YES;
            } else {
                v = HAL_CODEC_NO;
            }
            codecipbus_w_pdm_en(reg_base, v);
            if (cfg->channel_map & AUD_CHANNEL_MAP_DIGMIC_CH0) {
                v = HAL_CODEC_YES;
            } else {
                v = HAL_CODEC_NO;
            }
            codecipbus_w_adc_ch0_sel_pdm_path(reg_base, v);
            if (cfg->channel_map & AUD_CHANNEL_MAP_DIGMIC_CH1) {
                v = HAL_CODEC_YES;
            } else {
                v = HAL_CODEC_NO;
            }
            codecipbus_w_adc_ch1_sel_pdm_path(reg_base, v);
        }

        //set capture sample rate
        if(HAL_CODEC_CONFIG_SAMPLE_RATE & cfg->set_flag)
        {
            uint8_t i;

            for(i = 0; i < ARRAY_SIZE(codec_adc_sample_rate); i++)
            {
                if(codec_adc_sample_rate[i].sample_rate == cfg->sample_rate)
                {
                    break;
                }
            }
            ASSERT(i < ARRAY_SIZE(codec_adc_sample_rate), "%s: Invalid capture sample rate: %d", __func__, cfg->sample_rate);

            TRACE("[%s] stream=%d sample_rate=%d", __func__, stream, cfg->sample_rate);
            analog_aud_freq_pll_config(codec_adc_sample_rate[i].codec_freq, codec_adc_sample_rate[i].codec_div);
            if (codec_adc_sample_rate[i].codec_div != codec_adc_sample_rate[i].cmu_div &&
                    codec_adc_sample_rate[i].codec_div != 2 * codec_adc_sample_rate[i].cmu_div &&
                    codec_adc_sample_rate[i].codec_div != 4 * codec_adc_sample_rate[i].cmu_div) {
                ASSERT(false, "[%s] Invalid ADC div cfg: %u / %u", __func__,
                    codec_adc_sample_rate[i].codec_div, codec_adc_sample_rate[i].cmu_div);
            }
            hal_cmu_codec_adc_set_div(codec_adc_sample_rate[i].cmu_div);
            codecipbus_w_enable_sadc_clk_x2_mode(reg_base,
                (codec_adc_sample_rate[i].codec_div == codec_adc_sample_rate[i].cmu_div * 2));
            hal_codec_set_adc_down(id, codec_adc_sample_rate[i].adc_down);
            if (codec_adc_sample_rate[i].clk_x4) {
                hal_codec_anc_dma_enable(id);
            } else {
                hal_codec_anc_dma_disable(id);
            }
        }

#ifndef FIXED_CODEC_ADC_VOL
        //set capture volume
        if(HAL_CODEC_CONFIG_VOL & cfg->set_flag)
        {
            HAL_CODEC_TRACE();
            codecipbus_w_sadc_vol_ch0_sel(reg_base, cfg->vol);
            codecipbus_w_sadc_vol_ch1_sel(reg_base, cfg->vol);
        }
#endif
    }

    return 0;
}

int hal_codec_playback_sample_rate_trim(enum HAL_CODEC_ID_T id, enum AUD_SAMPRATE_T samp_rate, float trim)
{
    uint8_t i;

    for(i = 0; i < ARRAY_SIZE(codec_dac_sample_rate); i++)
    {
        if(codec_dac_sample_rate[i].sample_rate == samp_rate)
        {
            break;
        }
    }
    ASSERT(i < ARRAY_SIZE(codec_dac_sample_rate), "%s: Invalid playback sample rate: %d", __func__, samp_rate);

    analog_aud_freq_pll_config((uint32_t)(codec_dac_sample_rate[i].codec_freq * (1 + trim)), codec_dac_sample_rate[i].codec_div);

    return 0;
}

enum AUD_SAMPRATE_T hal_codec_anc_convert_rate(enum AUD_SAMPRATE_T rate)
{
    return rate;
}

int hal_codec_anc_dma_enable(enum HAL_CODEC_ID_T id)
{
    uint32_t reg_base;
    reg_base = _codec_get_reg_base(id);

    codecipbus_w_anc_ch0_dma_en(reg_base, HAL_CODEC_YES);
    codecipbus_w_anc_ch1_dma_en(reg_base, HAL_CODEC_YES);
    return 0;
}

int hal_codec_anc_dma_disable(enum HAL_CODEC_ID_T id)
{
    uint32_t reg_base;
    reg_base = _codec_get_reg_base(id);

    codecipbus_w_anc_ch0_dma_en(reg_base, HAL_CODEC_NO);
    codecipbus_w_anc_ch1_dma_en(reg_base, HAL_CODEC_NO);
    return 0;
}

int hal_codec_aux_mic_dma_enable(enum HAL_CODEC_ID_T id)
{
    uint32_t reg_base;
    reg_base = _codec_get_reg_base(id);

    codecipbus_w_adc_ch2_dma_en(reg_base, HAL_CODEC_YES);
    codecipbus_w_adc_ch3_dma_en(reg_base, HAL_CODEC_YES);
    return 0;
}

int hal_codec_aux_mic_dma_disable(enum HAL_CODEC_ID_T id)
{
    uint32_t reg_base;
    reg_base = _codec_get_reg_base(id);

    codecipbus_w_adc_ch2_dma_en(reg_base, HAL_CODEC_NO);
    codecipbus_w_adc_ch3_dma_en(reg_base, HAL_CODEC_NO);
    return 0;
}

uint32_t hal_codec_get_alg_dac_shift(void)
{
    return algdac_shift;
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
    uint32_t reg_base = _codec_get_reg_base(id);
    hal_codec_set_dig_dac_gain(reg_base, digdac_gain);
#endif
}
#endif

#ifdef AUDIO_OUTPUT_DC_CALIB
void hal_codec_set_dac_dc_gain_attn(float attn)
{
    dac_dc_gain_attn = attn;
}
#endif

void hal_codec_set_dac_reset_callback(HAL_CODEC_DAC_RESET_CALLBACK callback)
{
    dac_reset_callback = callback;
}

void hal_codec_dac_gain_m60db_check(enum HAL_CODEC_PERF_TEST_POWER_T type)
{
    uint8_t anadac_gain;
    int8_t gain;
    bool update = false;

    anadac_gain = analog_codec_get_dac_gain();
#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
    if (codec_nr_enabled) {
        if (anadac_gain != 1 && !(anadac_gain == 0 && prev_anadac_gain == 0)) {
            prev_anadac_gain = anadac_gain;
            update = true;
            codec_nr_enabled = false;
            hal_codec_set_noise_reduction(true);
        }
    } else
#endif
    {
        if (prev_anadac_gain != anadac_gain) {
            prev_anadac_gain = anadac_gain;
            update = true;
        }
    }

    if (update) {
        if (anadac_gain == 0) {
            // Minus 1 to ensure the output signal is below -60 dBV
            gain = -60 - analog_codec_dac_max_attn_db() - 1;
        } else {
            gain = 0;
        }
#ifdef AUDIO_OUTPUT_SW_GAIN
        hal_codec_set_sw_gain(gain);
#else
        hal_codec_set_dig_dac_gain(_codec_get_reg_base(HAL_CODEC_ID_0), gain);
#endif
    }
}

void hal_codec_tune_resample_rate(enum AUD_STREAM_T stream, float factor)
{
}

