#ifndef __HAL_CODEC_H__
#define __HAL_CODEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "hal_aud.h"

#define DB_TO_QDB(n)                        ((n) * 4)
#define QDB_TO_DB(n)                        ((n) / 4)

enum HAL_CODEC_ID_T {
    HAL_CODEC_ID_0 = 0,
    HAL_CODEC_ID_NUM,
};

struct HAL_CODEC_CONFIG_T {
    enum AUD_BITS_T bits;
    enum AUD_SAMPRATE_T sample_rate;
    enum AUD_CHANNEL_NUM_T channel_num;
    enum AUD_CHANNEL_MAP_T channel_map;

    uint32_t use_dma:1;
    uint32_t vol:5;

    enum AUD_IO_PATH_T io_path;

    uint32_t set_flag;
};

struct dac_classg_cfg {
    uint8_t thd2;
    uint8_t thd1;
    uint8_t thd0;
    uint8_t lr;
    uint8_t step_4_3n;
    uint8_t quick_down;
    uint16_t wind_width;
};

enum HAL_CODEC_CONFIG_FLAG_T{
    HAL_CODEC_CONFIG_NULL = 0x00,

    HAL_CODEC_CONFIG_BITS = 0x01,
    HAL_CODEC_CONFIG_SAMPLE_RATE = 0x02,
    HAL_CODEC_CONFIG_CHANNEL_NUM = 0x04,
    HAL_CODEC_CONFIG_CHANNEL_MAP = 0x08,
    HAL_CODEC_CONFIG_VOL = 0x10,

    HAL_CODEC_CONFIG_ALL = 0xff,
};

enum HAL_CODEC_DAC_RESET_STAGE_T {
    HAL_CODEC_DAC_PRE_RESET,
    HAL_CODEC_DAC_POST_RESET,
};

enum HAL_CODEC_SYNC_TYPE_T {
    HAL_CODEC_SYNC_TYPE_NONE,
    HAL_CODEC_SYNC_TYPE_GPIO,
    HAL_CODEC_SYNC_TYPE_BT,
    HAL_CODEC_SYNC_TYPE_WIFI,
};

enum HAL_CODEC_PERF_TEST_POWER_T {
    HAL_CODEC_PERF_TEST_30MW,
    HAL_CODEC_PERF_TEST_10MW,
    HAL_CODEC_PERF_TEST_5MW,
    HAL_CODEC_PERF_TEST_M60DB,

    HAL_CODEC_PERF_TEST_QTY
};

typedef void (*HAL_CODEC_DAC_RESET_CALLBACK)(enum HAL_CODEC_DAC_RESET_STAGE_T stage);
typedef void (*HAL_CODEC_SW_OUTPUT_COEF_CALLBACK)(float coef);

int hal_codec_open(enum HAL_CODEC_ID_T id);
int hal_codec_close(enum HAL_CODEC_ID_T id);
void hal_codec_stop_playback_stream(enum HAL_CODEC_ID_T id);
void hal_codec_start_playback_stream(enum HAL_CODEC_ID_T id);
int hal_codec_start_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream);
int hal_codec_stop_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream);
int hal_codec_start_interface(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream, int dma);
int hal_codec_stop_interface(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream);
void hal_codec_dac_mute(enum HAL_CODEC_ID_T id);
int hal_codec_setup_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream, const struct HAL_CODEC_CONFIG_T *cfg);
int hal_codec_playback_sample_rate_trim(enum HAL_CODEC_ID_T id, enum AUD_SAMPRATE_T samp_rate, float trim);
int hal_codec_anc_adc_enable(enum ANC_TYPE_T type);
int hal_codec_anc_adc_disable(enum ANC_TYPE_T type);
enum AUD_SAMPRATE_T hal_codec_anc_convert_rate(enum AUD_SAMPRATE_T rate);
int hal_codec_anc_dma_enable(enum HAL_CODEC_ID_T id);
int hal_codec_anc_dma_disable(enum HAL_CODEC_ID_T id);
int hal_codec_aux_mic_dma_enable(enum HAL_CODEC_ID_T id);
int hal_codec_aux_mic_dma_disable(enum HAL_CODEC_ID_T id);
uint32_t hal_codec_get_alg_dac_shift(void);
void hal_codec_apply_dac_gain_offset(enum HAL_CODEC_ID_T id, int16_t offset);
void hal_codec_anc_init_speedup(bool enable);
void hal_codec_set_dac_reset_callback(HAL_CODEC_DAC_RESET_CALLBACK callback);
void hal_codec_set_sw_output_coef_callback(HAL_CODEC_SW_OUTPUT_COEF_CALLBACK callback);
void hal_codec_dac_gain_m60db_check(enum HAL_CODEC_PERF_TEST_POWER_T type);
void hal_codec_set_noise_reduction(bool enable);
void hal_codec_classg_config(const struct dac_classg_cfg *cfg);
void hal_codec_set_dac_dc_gain_attn(float attn);
void hal_codec_set_dac_dc_offset(int16_t dc_l, int16_t dc_r);
void hal_codec_sync_dac_enable(enum HAL_CODEC_SYNC_TYPE_T type);
void hal_codec_sync_dac_disable(void);
void hal_codec_sync_adc_enable(enum HAL_CODEC_SYNC_TYPE_T type);
void hal_codec_sync_adc_disable(void);
int hal_codec_dac_reset_set(void);
int hal_codec_dac_reset_clear(void);
void hal_codec_tune_resample_rate(enum AUD_STREAM_T stream, float factor);
void hal_codec_get_dac_gain(float *left_gain,float *right_gain);
int hal_codec_select_clock_out(uint32_t cfg);
void hal_codec_setup_mc(enum AUD_CHANNEL_NUM_T channel_num, enum AUD_BITS_T bits);

#ifdef __cplusplus
}
#endif

#endif
