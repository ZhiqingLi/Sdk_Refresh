#include "include.h"

void bt_aec_process(u8 *ptr, u32 samples, int ch_mode);
void bt_sco_process(u8 *ptr, u32 samples, int ch_mode);
void bt_alc_process(u8 *ptr, u32 samples, int ch_mode);
void aux_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void speaker_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void usbmic_sdadc_process(u8 *ptr, u32 samples, int ch_mode);

#if FUNC_AUX_EN
    #define aux_sdadc_callback      aux_sdadc_process
#else
    #define aux_sdadc_callback      sdadc_dummy
#endif // FUNC_AUX_EN

#if FUNC_SPEAKER_EN
    #define speaker_sdadc_callback  speaker_sdadc_process
#else
    #define speaker_sdadc_callback  sdadc_dummy
#endif // FUNC_SPEAKER_EN

#if UDE_MIC_EN
    #define usbmic_sdadc_callback   usbmic_sdadc_process
#else
    #define usbmic_sdadc_callback   sdadc_dummy
#endif // UDE_MIC_EN

#if BT_AEC_EN
    #define bt_sdadc_callback    bt_aec_process
#elif BT_ALC_EN
    #define bt_sdadc_callback    bt_alc_process
#else
    #define bt_sdadc_callback    bt_sco_process
#endif //BT_AEC_EN

#if !FMRX_INSIDE_EN
    #define fmrx_sdadc_callback     fmrx_sdadc_process
#else
    #define fmrx_sdadc_callback     sdadc_dummy
#endif

//AUX analog gain: 000(-6DB), 001(-3DB), 010(0DB), 011(+3DB), 100(+6DB), 默认值010(0DB)
//MIC analog gain: 0~23(共24级), step 3DB (-6db ~ +63db)
//adadc digital gain: 0~31, step 3/32 DB, 0~3DB的细调, 保存在gain的低5bit
const sdadc_cfg_t rec_cfg_tbl[] = {
/*   通道,              采样率,     增益,        通路控制,     保留,   样点数,   回调函数*/
    {AUX_CHANNEL_CFG,   SPR_44100,  (2 << 5),    ADC2DAC_EN,     0,      256,    aux_sdadc_callback},            /* AUX     */
    {FMRX_CHANNEL_CFG,  SPR_44100,  (2 << 5),    ADC2DAC_EN,     0,      256,    fmrx_sdadc_callback},           /* FM      */
    {MIC_CHANNEL_CFG,   SPR_16000,  (14 << 5),   ADC2DAC_EN,     0,      240,    speaker_sdadc_callback},        /* SPEAKER */
    {MIC_CHANNEL_CFG,   SPR_8000,   (14 << 5),   ADC2DAC_EN,     0,      128,    bt_sdadc_callback},             /* BTMIC   */
    {MIC_CHANNEL_CFG,   SPR_48000,  (14 << 5),   ADC2DAC_EN,     0,      128,    usbmic_sdadc_callback},         /* USBMIC  */
    {MIC_CHANNEL_CFG,   SPR_16000,  (14 << 5),   ADC2SRC_EN,     0,      256,    sdadc_dummy},                   /* KARAOK  */
};

const u8 btmic_ch_tbl[] = {CH_MIC_PF2, CH_MIC_PF5};

void audio_path_init(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));

#if FUNC_AUX_EN
    if (path_idx == AUDIO_PATH_AUX) {
        cfg.channel = (xcfg_cb.auxr_sel << 4) | xcfg_cb.auxl_sel;
        cfg.gain = ((u16)xcfg_cb.aux_anl_gain << 5) | xcfg_cb.aux_dig_gain;
    }
#endif // FUNC_AUX_EN

#if FUNC_FMRX_EN
    if (path_idx == AUDIO_PATH_FM) {
        cfg.gain = ((u16)FMRX_AUX_ANL_GAIN << 5);
    }
#endif // FUNC_FMRX_EN

    if (path_idx == AUDIO_PATH_BTMIC) {
        cfg.channel = btmic_ch_tbl[xcfg_cb.bt_ch_mic];
        cfg.gain = ((u16)BT_ANL_GAIN << 5) | BT_DIG_GAIN;
#if BT_AEC_EN || BT_ALC_EN
        if (xcfg_cb.bt_aec_en) {
            cfg.callback = bt_aec_process;
        } else if (xcfg_cb.bt_alc_en) {
            cfg.callback = bt_alc_process;
        } else {
            cfg.callback = bt_sco_process;
        }
#endif // BT_AEC_EN
    }

    sdadc_init(&cfg);
}

void audio_path_start(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));
#if FUNC_AUX_EN
    if (path_idx == AUDIO_PATH_AUX) {
        cfg.channel = (xcfg_cb.auxr_sel << 4) | xcfg_cb.auxl_sel;
    }
#endif // FUNC_AUX_EN
    if (path_idx == AUDIO_PATH_BTMIC) {
        cfg.channel = btmic_ch_tbl[xcfg_cb.bt_ch_mic];
    }

#if MICL_MUX_DETECT_LINEIN
    if (((cfg.channel & CHANNEL_L) == CH_MIC_PF2) && (is_linein_det_mux_micl())) {
        sys_cb.micl_en = 1;
        micl2gnd_flag = 0;
        GPIOFDIR |= BIT(2);
        GPIOFDE &= ~BIT(2);
        GPIOFPU &= ~BIT(2);
    }
#endif // MICL_MUX_DETECT_LINEIN
    sdadc_start(cfg.channel);
}

void audio_path_exit(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));

#if FUNC_AUX_EN
    if (path_idx == AUDIO_PATH_AUX) {
        cfg.channel = (xcfg_cb.auxr_sel << 4) | xcfg_cb.auxl_sel;
    }
#endif // FUNC_AUX_EN

    if (path_idx == AUDIO_PATH_BTMIC) {
        cfg.channel = btmic_ch_tbl[xcfg_cb.bt_ch_mic];
    }

    sdadc_exit(cfg.channel);

#if MICL_MUX_DETECT_LINEIN
    if (((cfg.channel & CHANNEL_L) == CH_MIC_PF2) && (is_linein_det_mux_micl())) {
        GPIOFDIR |= BIT(2);
        GPIOFDE |= BIT(2);
        GPIOFPU |= BIT(2);
        delay_us(10);
        sys_cb.micl_en = 0;
        micl2gnd_flag = 0;
    }
#endif // MICL_MUX_DETECT_LINEIN

    adpll_spr_set(DAC_OUT_SPR);
}

u8 get_aux_channel_cfg(void)
{
#if FUNC_AUX_EN
    return ((xcfg_cb.auxr_sel << 4) | xcfg_cb.auxl_sel);    //工具配置AUX通路
#else
    return 0;
#endif // FUNC_AUX_EN
}



