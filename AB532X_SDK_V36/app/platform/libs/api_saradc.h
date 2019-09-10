#ifndef _API_SARADC_H
#define _API_SARADC_H

void saradc_init(u16 adc_ch);
void saradc_kick_start(u16 adc_ch, u8 auto_anl_en);
bool is_saradc_convert_finish(void);
void saradc_exit(u16 adc_ch);
void lefmic_zero_detect_do(u8 *ptr, u32 samples);
void saradc_setup_time_set(u32 adc_ch,  u32 delay_clk);
void saradc_baud_set(u16 div);
#endif
