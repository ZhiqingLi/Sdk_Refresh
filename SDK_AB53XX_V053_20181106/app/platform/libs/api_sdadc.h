#ifndef _API_SDADC_H
#define _API_SDADC_H

#define CHANNEL_L           0x0F
#define CHANNEL_R           0xF0

#define ADC2DAC_EN          0x01        //ADC-->DAC
#define ADC2SRC_EN          0x02        //ADC-->SRC

typedef void (*pcm_callback_t)(u8 *ptr, u32 samples, int ch_mode);

typedef struct {
    u8 channel;
    u8 sample_rate;
    u16 gain;                   //低5bit为sdadc digital gain, 其它bit为模拟gain控制
    u8 out_ctrl;
    u8 res;
    u16 samples;
    pcm_callback_t callback;
} sdadc_cfg_t;

enum {
    SPR_48000,
    SPR_44100,
    SPR_38000,
    SPR_32000,
    SPR_24000,
    SPR_22050,
    SPR_16000,
    SPR_12000,
    SPR_11025,
    SPR_8000,
};

void set_mic_analog_gain(u8 level, u8 channel);         //level: 0~31, step 3DB
void set_aux_analog_gain(u8 level, u8 channel);         //level: 0~4, 000(-6DB), 001(-3DB), 010(0DB), 011(+3DB), 100(+6DB)
void sdadc_dummy(u8 *ptr, u32 samples, int ch_mode);
void sdadc_pcm_2_dac(u8 *ptr, u32 samples, int ch_mode);
void sdadc_var_init(void);

int sdadc_init(const sdadc_cfg_t *p_cfg);
int sdadc_start(u8 channel);
int sdadc_exit(u8 channel);

//AUX直通PA或LPF。 aux2lpf_en: 0 -> AUX2PA, 1 -> AUX2LPF
//AUX单声道直通，自动选择“单进单出”与“单进双出”配置。
int analog_aux2pa_start(u8 channel, u8 aux2lpf_en);
void analog_aux2pa_exit(u8 channel);
void analog_aux2pa_dig2anl_enable(u8 aux2lpf_en);
void analog_aux2pa_dig2anl_disable(u8 aux2lpf_en);

//MIC直通PA、LPF、AUX。 mic2lpf_en: 0 -> MIC2PA, 1 -> MIC2LPF, 2 -> MIC2AUXL, 3 -> MIC2AUXR
void analog_mic2pa_start(u8 channel, u8 mic2lpf_en);
void analog_mic2pa_exit(u8 channel);

#endif //_API_SDADC_H
