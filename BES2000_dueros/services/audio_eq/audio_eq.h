#ifndef __AUDIO_EQ_H__
#define __AUDIO_EQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "iir_process.h"
#include "fir_process.h"
#include "audio_drc.h"
#include "speech_drc.h"

typedef enum {
    AUDIO_EQ_TYPE_SW_IIR = 0,
    AUDIO_EQ_TYPE_HW_FIR,
    AUDIO_EQ_TYPE_HW_DAC_IIR,
    AUDIO_EQ_TYPE_HW_IIR,
} AUDIO_EQ_TYPE_T;

typedef enum {
    AUDIO_DRC_TYPE_FIRST = 0,
    AUDIO_DRC_TYPE_SECOND,
} AUDIO_DRC_TYPE_T;


int audio_eq_init(void);
int audio_eq_open(enum AUD_SAMPRATE_T sample_rate, enum AUD_BITS_T sample_bits,enum AUD_CHANNEL_NUM_T ch_num, void *eq_buf, uint32_t len);
int audio_eq_run(uint8_t *buf, uint32_t len);
int audio_eq_close(void);
int audio_eq_set_cfg(const FIR_CFG_T *fir_cfg,const IIR_CFG_T *iir_cfg,AUDIO_EQ_TYPE_T audio_eq_type);
int audio_drc_set_cfg(const drc_settings_t* drc1_cfg,const drc_2_settings_t *drc2_cfg,AUDIO_DRC_TYPE_T audio_drc_type);

#ifdef __cplusplus
}
#endif

#endif
