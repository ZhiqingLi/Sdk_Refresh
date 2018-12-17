#include "include.h"

#if SYS_KARAOK_EN

#define FIX_BIT(x)          ((int)(x * (1 << 28)))
#define ECHO_MAX_LEVEL      (12)

karaok_vol_t karaok_vol;

static s16 echo_cache_0[0x2C00] AT(.bram_echo);
static s16 echo_cache_1[0x2400] AT(.cram_echo);

#if SYS_MAGIC_VOICE_EN
static s16 buf_1[2048];
static s16 buf_2[128];
static s16 buf_3[128];
static s16 buf_4[512];
static s16 buf_5[64];
static s16 buf_6[64];
static char buf_7[512];
#endif

AT(.text.bsp.dac.table)
const u16 karaok_dvol_table_12[12 + 1] = {
    DIG_N60DB,  DIG_N43DB,  DIG_N32DB,  DIG_N26DB,  DIG_N22DB,  DIG_N18DB,  DIG_N14DB,  DIG_N12DB,  DIG_N10DB,
    DIG_N6DB,   DIG_N4DB,   DIG_N2DB,   DIG_N0DB,
};

AT(.text.bsp.dac.table)
const u16 karaok_dvol_table_16[16 + 1] = {
    DIG_N60DB,  DIG_N43DB,  DIG_N32DB,  DIG_N26DB,  DIG_N24DB,  DIG_N22DB,  DIG_N20DB,  DIG_N18DB, DIG_N16DB,
    DIG_N14DB,  DIG_N12DB,  DIG_N10DB,  DIG_N8DB,   DIG_N6DB,   DIG_N4DB,   DIG_N2DB,   DIG_N0DB,
};

//低通滤波器参数， 3KHz截至频率
const s32 echo_filter_coef[2][8] = {
    {   //44.1KHz 3K
        //分子
        FIX_BIT(0.006644395788557),
        FIX_BIT(0.019933187365671),
        FIX_BIT(0.019933187365671),
        FIX_BIT(0.006644395788557),
        //分母
        FIX_BIT(1.000000000000000),
        FIX_BIT(-2.150944711976045),
        FIX_BIT(1.626545208636282),
        FIX_BIT(-0.422445330351779),
    }, {//48KHz
        //分子
        FIX_BIT(0.005300409794526),
        FIX_BIT(0.015901229383577),
        FIX_BIT(0.015901229383577),
        FIX_BIT(0.005300409794526),

        //分母
        FIX_BIT(1.000000000000000),
        FIX_BIT(-2.219168618311665),
        FIX_BIT(1.715117830033402),
        FIX_BIT(-0.453545933365530),
    }
};


////低通滤波器参数， 4KHz截至频率
//const s32 echo_filter_coef[2][8] = {
//    {   //44.1KHz 4K
//        //分子
//        FIX_BIT(0.014099708769044),
//        FIX_BIT(0.042299126307133),
//        FIX_BIT(0.042299126307133),
//        FIX_BIT(0.014099708769044),
//        //分母
//        FIX_BIT(1.000000000000000),
//        FIX_BIT(-1.873027248422390),
//        FIX_BIT(1.300326954651049),
//        FIX_BIT(-0.314502036076304),
//    }, {//48KHz
//        //分子
//        FIX_BIT(0.011324865405187),
//        FIX_BIT(0.033974596215561),
//        FIX_BIT(0.033974596215561),
//        FIX_BIT(0.011324865405187),
//        //分母
//        FIX_BIT(1.000000000000000),
//        FIX_BIT(-1.962990915244728),
//        FIX_BIT(1.400000000000000),
//        FIX_BIT(-0.346410161513776),
//    }
//};

//混响间隔：(echo_delay[0, 87] * 128 / 44100)ms
const u16 echo_delay_12[12 + 1] = {
    0,
    30,       35,        38,         40,
    45,       48,        55,         60,
    75,       80,        85,         87,
};

//混响间隔：(echo_delay[0, 87] * 128 / 44100)ms
const u16 echo_delay_16[16 + 1] = {
     0,
     3,          5,        10,        15,
    20,         25,        40,        48,
    55,         60,        65,        70,
    74,         78,        83,        87,
};

#if SYS_MAGIC_VOICE_EN
mav_cfg_t mav_cfg = {
    .res[0]         = 0,
    .res[1]         = 0,
    .buf_1          = buf_1,
    .buf_2          = buf_2,
    .buf_3          = buf_3,
    .buf_4          = buf_4,
    .buf_5          = buf_5,
    .buf_6          = buf_6,
    .buf_7          = buf_7,
};
#endif

kara_cfg_t kara_cfg = {
    .lp_coef        = &echo_filter_coef,
#if SYS_MAGIC_VOICE_EN
    .mav            = &mav_cfg,
#endif
    .res[0]         = 0,
    .res[1]         = 0,
    .echo_en        = SYS_ECHO_EN,
    .mic_dnr        = MIC_DNR_EN,
    .rec_en         = KARAOK_REC_EN,
    .magic_voice    = SYS_MAGIC_VOICE_EN,
    .rec_no_echo_en = KARAOK_REC_NO_ECHO_EN,
    .howling_en     = SYS_HOWLING_EN,
};

AT(.text.bsp.karaok)
void bsp_echo_set_level(void)
{
    echo_set_level(karaok_vol.echo_level_gain[sys_cb.echo_level]);
    sys_cb.show_type = 1;
}

AT(.text.bsp.karaok)
void bsp_echo_set_delay(void)
{
    echo_set_delay(karaok_vol.echo_delay[sys_cb.echo_delay]);
}

AT(.text.bsp.karaok)
void bsp_karaok_set_mic_volume(void)
{
    karaok_set_mic_volume(karaok_vol.micvol_table[sys_cb.mic_vol]);
    sys_cb.show_type = 2;
}

AT(.text.bsp.karaok)
void bsp_karaok_set_music_volume(void)
{
    karaok_set_music_volume(karaok_vol.dvol_table[sys_cb.music_vol]);
    sys_cb.show_type = 3;
}

AT(.text.bsp.karaok)
void bsp_karaok_echo_reset_buf(u8 func)
{
    if (func == FUNC_NULL) {
        return;
    }
    if (func == FUNC_BT || func == FUNC_BTHID) {
#if KARAOK_REC_EN
        rec_cb_t *rec = &rec_cb;
        if (func == FUNC_BT && rec->sta == REC_STARTING) {
            kara_cfg.echo_cache = NULL;
            kara_cfg.res[0] &= ~0x03;
        } else
#endif
        {
            if (kara_cfg.echo_cache == echo_cache_1) {
                return;
            }
            kara_cfg.echo_cache = echo_cache_1;
            kara_cfg.res[0] = (kara_cfg.res[0] & ~0x03) | BIT(0);
        }
    } else {
        if (kara_cfg.echo_cache == echo_cache_0) {
            return;
        }
        kara_cfg.echo_cache = echo_cache_0;
        kara_cfg.res[0] = (kara_cfg.res[0] & ~0x03) | BIT(1);
    }
    echo_reset_buf(&kara_cfg);
    echo_set_delay(karaok_vol.echo_delay[sys_cb.echo_delay]);
    echo_set_level(karaok_vol.echo_level_gain[sys_cb.echo_level]);
}

AT(.text.bsp.karaok)
void bsp_karaok_init(u8 path, u8 func)
{
    if (func == FUNC_NULL) {
        return;
    }
    if ((func == FUNC_BT) || (func == FUNC_BTHID)) {
        kara_cfg.echo_cache = echo_cache_1;
        kara_cfg.res[0] = (kara_cfg.res[0] & ~0x03) | BIT(0);
    } else {
        kara_cfg.echo_cache = echo_cache_0;
        kara_cfg.res[0] = (kara_cfg.res[0] & ~0x03) | BIT(1);
    }

#if MIC_DNR_EN
    mic_dnr_init(2, 0x800, 100, 0x400); //连续超过2次大于0x800就认为有声，连续超过100次低于0x400就认为无声
#endif

    sys_cb.echo_delay = SYS_ECHO_DELAY;
#if (USER_KEY_KNOB_LEVEL == 12)
    karaok_vol.echo_delay = echo_delay_12;
    karaok_vol.echo_level_gain = echo_level_gain_12;
    karaok_vol.dvol_table = karaok_dvol_table_12;
    karaok_vol.micvol_table = karaok_dvol_table_12;
#elif (USER_KEY_KNOB_LEVEL == 16)
    karaok_vol.echo_delay = echo_delay_16;
    karaok_vol.echo_level_gain = echo_level_gain_16;
    karaok_vol.dvol_table = karaok_dvol_table_16;
    karaok_vol.micvol_table = karaok_dvol_table_16;
#endif
    plugin_karaok_init();

    audio_path_init(path);
    karaok_init(&kara_cfg);
#if SYS_KARAOK_BUFMODE
    karaok_bufmode(1);                    //使用降采样来加倍混响深度
#endif

    echo_set_delay(karaok_vol.echo_delay[sys_cb.echo_delay]);
    echo_set_level(karaok_vol.echo_level_gain[sys_cb.echo_level]);
	
    //karaok_enable_mic_first(0x2000);
    //mic_first_init(2, 10000, 30, 4000); //连续超过2次大于10000就开启mic_first，连续超过100次低于4000就关闭mic_first
    //karaok_mic_first_music_vol_fade_enable(100, 1000);   //淡入步进100, 淡出步进1000.   (数字音量最大值为0xFFFF_65535)

	#if SYS_KARAOK_EN && DUAL_MIC_CHANNEL_EN
    karaok_set_mic_left_volume(karaok_vol.micvol_table[sys_cb.mic_left_vol]);
    karaok_set_mic_right_volume(karaok_vol.micvol_table[sys_cb.mic_right_vol]);
	#endif
    karaok_set_mic_volume(karaok_vol.micvol_table[sys_cb.mic_vol]);
    karaok_set_music_volume(karaok_vol.dvol_table[sys_cb.music_vol]);

#if MIC_DETECT_EN
    if (!dev_is_online(DEV_MIC)) {
        karaok_mic_mute();
    }
#endif
    audio_path_start(path);

#if SYS_MAGIC_VOICE_EN
    magic_voice_set_level(sys_cb.magic_type, sys_cb.magic_level);
#endif // SYS_MAGIC_VOICE_EN

#ifdef RES_BUF_EQ_MIC_NORMAL_EQ
    mic_set_eq_by_res(&RES_BUF_EQ_MIC_NORMAL_EQ, &RES_LEN_EQ_MIC_NORMAL_EQ);
#endif
}

AT(.text.bsp.karaok)
void bsp_karaok_exit(u8 path)
{
    karaok_exit();
    audio_path_exit(path);
}
#if FUNC_REC_EN
AT(.text.bsp.karaok)
void bsp_karaok_rec_init(void)
{
    karaok_rec_init(rec_cb.src->nchannel & 0x0f);
    bsp_karaok_echo_reset_buf(func_cb.sta);
}

AT(.text.bsp.karaok)
void bsp_karaok_rec_exit(void)
{
    bsp_karaok_echo_reset_buf(func_cb.sta);
}
#endif


#if DUAL_MIC_CHANNEL_EN

void dual_mic_channel_select(u8 left_ch,u8 right_ch)
{
    u8 lr_ch = 0;
    if (left_ch) {
        lr_ch |= BIT(0);
    }
    if (right_ch) {
        lr_ch |= BIT(1);
    }
    sys_cb.dual_mic_lr = lr_ch;
}

AT(.text.karaok)
void karaok_set_mic_left_volume(u16 vol) //双MIC时有效
{
    sys_cb.micl_digital_vol = vol;
}

AT(.text.karaok)
void karaok_set_mic_right_volume(u16 vol) //双MIC时有效
{
    sys_cb.micr_digital_vol = vol;
}

AT(.com_text.sdadc)
s16 pcm_gain_calc(s16 pcm, u16 gain)
{
    s32 tmp;
    tmp = ((s32)pcm* gain)>>15;   //32767
    if(tmp < -32768){
        tmp = -32768;
    }else if(tmp > 32767){
        tmp = 32767;
    }
    return tmp;
}

#endif  //DUAL_MIC_CHANNEL_EN

AT(.com_text.sdadc)
void karaok_sdadc_process_callback(u8 *ptr, u32 samples, int ch_mode)  //目前这个函数主要处理双MIC。
{
    #if DUAL_MIC_CHANNEL_EN
    int i;
    s16* ppcm = (s16*)ptr;
    if(0x03 == (sys_cb.dual_mic_lr & 0x03)){
        if (ch_mode){  //MIC 双声道时
            for (i = 0; i < samples; i++) {
                ppcm[i] = (pcm_gain_calc(ppcm[i*2],sys_cb.micl_digital_vol) +  pcm_gain_calc(ppcm[i*2 + 1],sys_cb.micr_digital_vol))/2;
            }
        }
    }else if(BIT(0) == sys_cb.dual_mic_lr){
        for (i = 0; i < samples; i++) {
            ppcm[i] = pcm_gain_calc(ppcm[i*2],sys_cb.micl_digital_vol);
        }
    }else if(BIT(1) == sys_cb.dual_mic_lr){
        for (i = 0; i < samples; i++){
            ppcm[i] = pcm_gain_calc(ppcm[i*2 + 1],sys_cb.micr_digital_vol);
        }
    }else{
        if (ch_mode){  //双MIC未选通道，声音清0.
            for (i = 0; i < samples; i++) {
                ppcm[i] = pcm_gain_calc(ppcm[i*2],0) +  pcm_gain_calc(ppcm[i*2 + 1],0);
            }
        }
    }
    #endif
}




#endif
