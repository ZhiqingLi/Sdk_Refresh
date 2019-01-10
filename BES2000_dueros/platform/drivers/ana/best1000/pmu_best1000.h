#ifndef __PMU_BEST1000_H__
#define __PMU_BEST1000_H__

#ifdef __cplusplus
extern "C" {
#endif

void pmu_codec_adc_pre_start(void);

void pmu_codec_adc_post_start(void (*delay_ms)(uint32_t));

void pmu_codec_dac_pre_start(void);

void pmu_codec_dac_post_start(void (*delay_ms)(uint32_t));

void pmu_fm_config(int enable);

#ifdef __cplusplus
}
#endif

#endif

