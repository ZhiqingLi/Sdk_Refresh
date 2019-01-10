#ifndef _API_SARADC_H
#define _API_SARADC_H

void saradc_init(u16 adc_ch);
//adc_ch需要延时采样的通道.
//delay_clk 采样建立时间(采样延时)取值范围[0~3] //0:0us //1:2us //2: 4us  //3: 8us
void saradc_setup_time_set(u32 adc_ch,  u32 delay_clk);
void saradc_kick_start(u16 adc_ch);
bool is_saradc_convert_finish(void);
void saradc_exit(u16 adc_ch);
void lefmic_zero_detect_do(u8 *ptr, u32 samples);
void saradc_kick_convert(u16 adc_ch, u8 auto_anl_en);

void saradc_baud_set(u16 div);
#define saradc_kick_start(x)    saradc_kick_convert(x, USER_ADKEY_MUX_LED)

#endif
