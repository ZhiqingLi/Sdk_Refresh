#ifndef __PMU_BEST3001_H__
#define __PMU_BEST3001_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ISPI_PMU_REG(reg)                   (((reg) & 0xFFF) | 0x0000)

void pmu_codec_hppa_enable(int enable);

void pmu_codec_mic_bias_enable(uint32_t map);

#ifdef __cplusplus
}
#endif

#endif

