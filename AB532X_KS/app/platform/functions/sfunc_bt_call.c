#include "include.h"
#include "func.h"
#include "func_bt.h"

#define MAX_DIG_VAL             32767
#define AEC_DIG_P0DB            (MAX_DIG_VAL * 1.000000)
#define AEC_DIG_P1DB            (MAX_DIG_VAL * 1.122018)
#define AEC_DIG_P2DB            (MAX_DIG_VAL * 1.258925)
#define AEC_DIG_P3DB            (MAX_DIG_VAL * 1.412538)
#define AEC_DIG_P4DB            (MAX_DIG_VAL * 1.584893)
#define AEC_DIG_P5DB            (MAX_DIG_VAL * 1.778279)
#define AEC_DIG_P6DB            (MAX_DIG_VAL * 1.995262)
#define AEC_DIG_P7DB            (MAX_DIG_VAL * 2.238721)
#define AEC_DIG_P8DB            (MAX_DIG_VAL * 2.511886)
#define AEC_DIG_P9DB            (MAX_DIG_VAL * 2.818383)

typedef struct {
    //AEC
    u8 aec_en       :   1;
    u8 aec_echo_level;
    u8 aec_far_offset;
    //ALC
    u8 alc_en       :   1;
    u8 alc_fade_in_step;
    u8 alc_fade_out_step;
    u8 alc_fade_in_delay;
    u8 alc_fade_out_delay;
    s32 alc_far_voice_thr;
    //WNR
    u8 wnr_en       :   1;
    //降噪
    u16 noise_threshoid;
} bt_voice_cfg_t;

static bt_voice_cfg_t bt_voice_cfg AT(.sco_data);
void bt_voice_init(bt_voice_cfg_t *p);
void bt_voice_exit(void);
void bt_sco_rec_exit(void);
////库调用，设置MIC的增益（算法之后）
//AT(.com_text.aec)
//int sco_set_mic_gain_after_aec(void)
//{
//    return AEC_DIG_P9DB;
//}

#if FUNC_BT_EN
void sco_audio_init(void)
{
    memset(&bt_voice_cfg, 0, sizeof(bt_voice_cfg_t));
    bt_voice_cfg.noise_threshoid = BT_NOISE_THRESHOID;
#if BT_AEC_EN || BT_ALC_EN
    if (xcfg_cb.bt_aec_en) {
#if BT_AEC_EN
        bt_voice_cfg.aec_en = 1;
        bt_voice_cfg.aec_echo_level = BT_ECHO_LEVEL;
        bt_voice_cfg.aec_far_offset = BT_FAR_OFFSET;
#endif // BT_AEC_EN
    } else if (xcfg_cb.bt_alc_en) {
#if BT_ALC_EN
        bt_voice_cfg.alc_en = 1;
        bt_voice_cfg.alc_fade_in_delay = BT_ALC_FADE_IN_DELAY;
        bt_voice_cfg.alc_fade_in_step = BT_ALC_FADE_IN_STEP;
        bt_voice_cfg.alc_fade_out_delay = BT_ALC_FADE_OUT_DELAY;
        bt_voice_cfg.alc_fade_out_step = BT_ALC_FADE_OUT_STEP;
        bt_voice_cfg.alc_far_voice_thr = BT_ALC_VOICE_THR;
#endif // BT_ALC_EN
    }
#endif

#if BT_HFP_REC_EN
    func_bt_sco_rec_init();
#endif

#if SYS_KARAOK_EN
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
    sys_cb.hfp_karaok_en = BT_HFP_CALL_KARAOK_EN;       //通话是否支持KARAOK
    plugin_hfp_karaok_configure();
    if (sys_cb.hfp_karaok_en) {
        bsp_karaok_init(AUDIO_PATH_BTMIC, FUNC_BT);
        kara_sco_start();
    } else
#endif
    {
        bt_voice_init(&bt_voice_cfg);
        dac_set_anl_offset(1);
        audio_path_init(AUDIO_PATH_BTMIC);
        audio_path_start(AUDIO_PATH_BTMIC);
        bsp_change_volume(sys_cb.hfp_vol * sys_cb.hfp2sys_mul);
        dac_fade_in();
    }
}

void sco_audio_exit(void)
{
#if BT_HFP_REC_EN
    sfunc_record_stop();
    bt_sco_rec_exit();
#endif
#if SYS_KARAOK_EN
    if (sys_cb.hfp_karaok_en) {
        kara_sco_stop();
        bsp_karaok_exit(AUDIO_PATH_BTMIC);
        sys_cb.hfp_karaok_en = 0;
    } else
#endif
    {
        dac_fade_out();
        dac_set_anl_offset(0);
        bsp_change_volume(sys_cb.vol);
        audio_path_exit(AUDIO_PATH_BTMIC);
        bt_voice_exit();
    }

#if SYS_KARAOK_EN
    bsp_karaok_init(AUDIO_PATH_KARAOK, FUNC_BT);
#endif

#if BT_REC_EN && BT_HFP_REC_EN
    bt_music_rec_init();
#endif
}

static void sfunc_bt_call_process(void)
{
    func_process();
    func_bt_status();
}

static void sfunc_bt_call_enter(void)
{
}

static void sfunc_bt_call_exit(void)
{
}

AT(.text.func.bt)
void sfunc_bt_call(void)
{
    printf("%s\n", __func__);

    sfunc_bt_call_enter();
#if DAC_DNR_EN
    u8 sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif

    while ((f_bt.disp_status >= BT_STA_OUTGOING) && (func_cb.sta == FUNC_BT)) {
        sfunc_bt_call_process();
        sfunc_bt_call_message(msg_dequeue());
        func_bt_display();
    }

#if DAC_DNR_EN
    dac_dnr_set_sta(sta);
#endif
    sfunc_bt_call_exit();
}
#else

void sco_audio_init(void){}
void sco_audio_exit(void){}

#endif //FUNC_BT_EN
