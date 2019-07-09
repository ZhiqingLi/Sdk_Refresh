#include "include.h"

typedef struct {
    u8  anl2pa      :   1,          //AUX模拟直通
        aux2lpf_en  :   1,          //是否AUX模拟直通LPF
        dig2anl_dis :   1;          //是否关闭DAC digital to pa通路, 模拟直通时关闭
} bsp_aux_cb_t;
static bsp_aux_cb_t bsp_aux_cb AT(.buf.aux);

//AUX模拟直通start
AT(.text.bsp.aux)
void bsp_aux_anl2pa_start(u8 channel, u8 aux_anl_gain, u8 aux2lpf_en)
{
    bsp_aux_cb_t *s = &bsp_aux_cb;

    analog_aux2pa_start(channel, aux2lpf_en);
    set_aux_analog_gain(aux_anl_gain, channel);         //set aux analog gain
    s->dig2anl_dis = 1;
#if SYS_KARAOK_EN
    analog_aux2pa_dig2anl_enable(aux2lpf_en);           //KARAOK时不能关数字DAC
    s->dig2anl_dis = 0;
#endif
    s->anl2pa = 1;                                      //模拟直通start
    s->aux2lpf_en = aux2lpf_en;
}

//AUX模拟直通stop
AT(.text.bsp.aux)
void bsp_aux_anl2pa_stop(u8 channel)
{
    bsp_aux_cb_t *s = &bsp_aux_cb;

    analog_aux2pa_exit(channel);
    s->anl2pa = 0;
    s->aux2lpf_en = 0;
    s->dig2anl_dis = 0;
}


AT(.text.bsp.aux)
void bsp_aux_start(u8 channel, u8 aux_anl_gain, u8 aux2adc)
{
    u8 path = aux2adc & 0x3f;

 //   printf("%s\n", __func__);
    dac_fade_out();
    dac_fade_wait();                                            //等待淡出完成

    if (aux2adc & BIT(7)) {                                     //AUX to SDADC
        audio_path_init(path);
        audio_path_start(path);
    } else {
        bsp_aux_anl2pa_start(channel, aux_anl_gain, 0);         //AUX直通
        if (aux2adc & BIT(6)) {                                 //模拟直通，需要同时开启SDADC用于录音
            audio_path_init(path);
            aux2pa_sdadc_enable();
        }
    }
    dac_fade_in();
}

AT(.text.bsp.aux)
void bsp_aux_stop(u8 channel, u8 aux2adc)
{
    u8 path = aux2adc & 0x3f;

 //   printf("%s\n", __func__);
    dac_fade_out();
    dac_fade_wait();
    if (aux2adc & BIT(7)) {
        audio_path_exit(path);
    } else {
        bsp_aux_anl2pa_stop(channel);
        if (aux2adc & BIT(6)) {
            audio_path_exit(path);
        }
    }
}

//处理AUX直通时播放WAV提示音
AT(.text.bsp.aux)
void bsp_aux_piano_start(void)
{
    bsp_aux_cb_t *s = &bsp_aux_cb;
    if (s->dig2anl_dis) {
        analog_aux2pa_dig2anl_enable(s->aux2lpf_en);
    }
}

AT(.text.bsp.aux)
void bsp_aux_piano_exit(void)
{
    bsp_aux_cb_t *s = &bsp_aux_cb;
    if (s->dig2anl_dis) {
        analog_aux2pa_dig2anl_disable(s->aux2lpf_en);
    }
}

AT(.text.bsp.aux)
void aux_var_init(void)
{
    memset(&bsp_aux_cb, 0, sizeof(bsp_aux_cb_t));
}



