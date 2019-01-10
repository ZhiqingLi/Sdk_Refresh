#ifndef __PMU_BEST2000_H__
#define __PMU_BEST2000_H__

#ifdef __cplusplus
extern "C" {
#endif

void pmu_codec_hppa_enable(int enable);

void pmu_codec_mic_bias_enable(uint32_t map);

// TODO: To be removed
typedef int (*PMU_EFUSE_GET_HANDLER)(int page, unsigned short *efuse);

void pmu_override_efuse_get_handler(PMU_EFUSE_GET_HANDLER hdlr);

#ifdef __cplusplus
}
#endif

#endif

