#ifndef __ANC_PROCESS_H__
#define __ANC_PROCESS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "aud_section.h"
#include "hal_aud.h"

typedef enum {
    ANC_NO_ERR=0,
    ANC_TYPE_ERR,
    ANC_GAIN_ERR,
    ANC_OTHER_ERR,
    ANC_ERR_TOTAL,
}ANC_ERROR;

typedef enum {
    ANC_GAIN_NO_DELAY=0,
    ANC_GAIN_DELAY,
    ANC_GAIN_TOTAL,
}ANC_GAIN_TIME;


int anc_opened(enum ANC_TYPE_T anc_type);
int anc_open(enum ANC_TYPE_T anc_type);
void anc_close(enum ANC_TYPE_T anc_type);

int anc_disable(void);
int anc_enable(void);

int anc_set_cfg(const struct_anc_cfg * cfg,enum ANC_TYPE_T anc_type,ANC_GAIN_TIME anc_gain_delay);
int anc_set_gain(int32_t gain_ch_l, int32_t gain_ch_r,enum ANC_TYPE_T anc_type);
int anc_get_gain(int32_t *gain_ch_l, int32_t *gain_ch_r,enum ANC_TYPE_T anc_type);
int anc_get_cfg_gain(int32_t *gain_ch_l, int32_t *gain_ch_r,enum ANC_TYPE_T anc_type);

//void AncPduHardwarCh0(const short Coef[], short CoefLen);
//void AncPduHardwarCh1(const short Coef[], short CoefLen);
void AncPduHardwarCh0(const short Coef[], short CoefLen,short gain);
void AncPduHardwarCh1(const short Coef[], short CoefLen,short gain);

int anc_select_coef(enum AUD_SAMPRATE_T rate,enum ANC_INDEX index,enum ANC_TYPE_T anc_type,ANC_GAIN_TIME anc_gain_delay);
int anc_load_cfg(void);
enum ANC_INDEX anc_get_current_coef_index(void);
enum AUD_SAMPRATE_T anc_get_current_coef_samplerate(void);
int anc_mc_run_stereo(uint8_t *buf, int len,float left_gain,float right_gain,enum AUD_BITS_T sample_bit);
int anc_mc_run_mono(uint8_t *buf, int len,float left_gain,enum AUD_BITS_T sample_bit);
void anc_mc_run_init(enum AUD_SAMPRATE_T rate);
void anc_mc_run_setup(enum AUD_SAMPRATE_T rate);

#ifdef __cplusplus
}
#endif

#endif
