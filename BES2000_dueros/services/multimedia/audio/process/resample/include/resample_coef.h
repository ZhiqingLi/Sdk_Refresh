#ifndef __RESAMPLE_COEF_H__
#define __RESAMPLE_COEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "audio_resample_ex.h"

extern const struct RESAMPLE_COEF_T resample_coef_50p7k_to_48k;
extern const struct RESAMPLE_COEF_T resample_coef_50p7k_to_44p1k;
extern const struct RESAMPLE_COEF_T resample_coef_44p1k_to_48k;
extern const struct RESAMPLE_COEF_T resample_coef_44p1k_to_48k_16coef;
extern const struct RESAMPLE_COEF_T resample_coef_44p1k_to_48k_24coef;
extern const struct RESAMPLE_COEF_T resample_coef_44p1k_to_48k_28coef;
extern const struct RESAMPLE_COEF_T resample_coef_48k_to_44p1k;

extern const struct RESAMPLE_COEF_T resample_coef_32k_to_50p7k;
extern const struct RESAMPLE_COEF_T resample_coef_44p1k_to_50p7k;
extern const struct RESAMPLE_COEF_T resample_coef_48k_to_50p7k;
extern const struct RESAMPLE_COEF_T resample_coef_8k_to_8p4k;
extern const struct RESAMPLE_COEF_T resample_coef_8p4k_to_8k;

extern const struct RESAMPLE_COEF_T resample_coef_any_up64;
extern const struct RESAMPLE_COEF_T resample_coef_any_up256;
extern const struct RESAMPLE_COEF_T resample_coef_any_up512_32;
extern const struct RESAMPLE_COEF_T resample_coef_any_up512_36;

#ifdef __cplusplus
}
#endif

#endif
