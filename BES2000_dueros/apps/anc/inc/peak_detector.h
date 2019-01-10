#ifndef __PEAK_DETECTOR_H__
#define __PEAK_DETECTOR_H__

#include "plat_types.h"
#include "hal_aud.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    enum AUD_SAMPRATE_T fs;
    enum AUD_BITS_T bits;
    float factor_up;
    float factor_down;
    float reduce_dB;
} PEAK_DETECTOR_CFG_T;

void peak_detector_init(void);
void peak_detector_setup(PEAK_DETECTOR_CFG_T *cfg);
void peak_detector_run(uint8_t *buf, uint32_t len, float vol_multiple);

#ifdef __cplusplus
}
#endif
#endif
