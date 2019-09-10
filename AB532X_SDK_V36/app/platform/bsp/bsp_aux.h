#ifndef _BSP_AUX_H
#define _BSP_AUX_H

void bsp_aux_anl2pa_start(u8 channel, u8 aux_anl_gain, u8 aux2lpf_en);
void bsp_aux_anl2pa_stop(u8 channel);
void bsp_aux_start(u8 channel, u8 aux_anl_gain, u8 aux2adc);
void bsp_aux_stop(u8 channel, u8 aux2adc);
void bsp_aux_piano_start(void);
void bsp_aux_piano_exit(void);
void bsp_aux_wav_res_start(void);
void bsp_aux_wav_res_exit(void);
void aux_var_init(void);
#endif
