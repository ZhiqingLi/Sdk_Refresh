#include "include.h"

volatile int pwrkey_detect_flag;            //pwrkey 820K用于复用检测的标志。

#if (!WARING_MAXVOL_MP3)
    #if (WARING_MAXVOL_FORMAT & WARING_WAV)
    wavres_cb_t wavres_cb;    //播放wav提示音时需要用到
    #endif
#endif

#if SYS_KARAOK_EN
//{第一级混响幅度[0, 256]， 每级混响衰减系数[0, 256]}
const u16 echo_level_gain_16[16 + 1][2] = {
    {0, 0},
    {45, 20},   {60, 25},   {65, 30},   {65, 51},
    {79, 51},   {79, 72},   {79, 85},   {79, 101},
    {85, 131},  {85, 142},  {86, 147},  {88, 160},
    {88, 163},  {88, 167},  {95, 171},  {170, 150},
};

//{第一级混响幅度[0, 256]， 每级混响衰减系数[0, 256]}
const u16 echo_level_gain_12[12 + 1][2] = {
    {0, 0},
    {45, 20},   {65, 30},   {65, 51},    {79, 51},
    {79, 72},   {79, 85},   {85, 131},   {86, 147},
    {88, 160},  {88, 167},  {95, 171},   {170, 150},
};
#endif

void plugin_init(void)
{
#if (LANG_SELECT == LANG_EN_ZH)
    multi_lang_init(sys_cb.lang_id);
#endif
}

void plugin_var_init(void)
{
    pwrkey_detect_flag = 0;
    xcfg_cb.pwrkey_config_en = xcfg_cb.pwrkey_config_en & xcfg_cb.user_pwrkey_en;
    xcfg_cb.adkey_config_en  = xcfg_cb.adkey_config_en & xcfg_cb.user_adkey_en;
    xcfg_cb.adkey2_config_en = xcfg_cb.adkey2_config_en & xcfg_cb.user_adkey2_en;
    xcfg_cb.iokey_config_en  = xcfg_cb.iokey_config_en & xcfg_cb.user_iokey_en;
    xcfg_cb.key_double_config_en = xcfg_cb.key_double_config_en & xcfg_cb.user_key_double_en;

#if FUNC_AUX_EN
    if (!xcfg_cb.func_aux_en) {
        xcfg_cb.linein_2_pwrdown_en = 0;
        xcfg_cb.linein_det_iosel = 0;
        xcfg_cb.auxl_sel = 0;
        xcfg_cb.auxr_sel = 0;
        xcfg_cb.mode_2_aux_en = 0;
    }
#endif // FUNC_AUX_EN

    if (!xcfg_cb.led_disp_en) {
        xcfg_cb.bled_io_sel = 0;
        xcfg_cb.charge_full_bled = 0;
        xcfg_cb.led_pwron_config_en   =  0;
        xcfg_cb.led_pwroff_config_en  =  0;
        xcfg_cb.led_btpair_config_en  =  0;
        xcfg_cb.led_btconn_config_en  =  0;
        xcfg_cb.led_btmusic_config_en =  0;
        xcfg_cb.led_btring_config_en  =  0;
        xcfg_cb.led_btcall_config_en  =  0;
        xcfg_cb.led_lowbat_config_en  =  0;
    }

    if (!xcfg_cb.func_music_en) {
        xcfg_cb.music_wav_support = 0;
        xcfg_cb.music_wma_support = 0;
        xcfg_cb.music_ape_support = 0;
        xcfg_cb.music_flac_support = 0;
        xcfg_cb.music_udisk_en = 0;
        xcfg_cb.music_sdcard_en = 0;
    }

    if (!xcfg_cb.music_sdcard_en) {
        xcfg_cb.sddet_iosel = 0;
    }

    if (!EARPHONE_DETECT_EN) {
        xcfg_cb.earphone_det_iosel = 0;
    }

    if (!LINEIN_DETECT_EN) {
        xcfg_cb.linein_det_iosel = 0;
    }

    if (!xcfg_cb.led_pwr_en) {
        xcfg_cb.rled_io_sel = 0;
    }

#if LED_DISP_EN
	led_func_init();
#endif // LED_DISP_EN
}

AT(.com_text.plugin)
void plugin_tmr5ms_isr(void)
{
#if ENERGY_LED_EN
    energy_led_level_calc();
#endif

#if PWM_TIMER_EN
	func_led_pwm_scan();
#endif
}

AT(.com_text.plugin)
void plugin_tmr1ms_isr(void)
{
#if ENERGY_LED_EN
    energy_led_scan();
#endif
}

void plugin_saradc_init(u16 *adc_ch)
{
#if USER_KEY_KNOB_EN
    *adc_ch |= BIT(ADCCH_PE5);       //旋钮1
    *adc_ch |= BIT(ADCCH_PE6);       //旋钮2
#endif

#if USER_ADC_DETECT_EN
	*adc_ch |= BIT(ADCCH_PE6);       //ADC
#endif
}

#if USER_ADC_DETECT_EN
const uint16_t tbl_adc_detect_val[5][2] = {
    {0x02, EVT_ADC_DET_LOW},
    {0x05, EVT_ADC_DET_LOW}, 
    {0x3f, EVT_ADC_DET_MID},
    {0x7f, EVT_ADC_DET_HIG},
    {0xff, EVT_ADC_DET_HIG},
};

//每5Ms检测一次
AT(.com_text.port.key)
void adc_detect_process(uint16_t *adc_event, uint8_t adc_val)
{
	//水泵电流是交流峰值
    #define ADC_JITTER_VAL (5000/5)   //峰值判
    uint8_t adc_index = 0, val_index = 0;
    static u16 jitter_cnt = 0;
	static u16 adc_max[5] = {0,0,0,0,0};
	static u16 adc_val_cnt[5] = {0,0,0,0,0};

	if ((func_idle_work_state_get() >= 1) && (func_idle_work_state_get() <= 3)) {
		if (jitter_cnt++ < ADC_JITTER_VAL) {
			//数据按照大到小
			for (adc_index = 0; adc_index < 5; adc_index++) {
	        	if (adc_max[adc_index] <= adc_val) {
	        		break;
	        	}
	        }
	        //数据放入队列
	        if ((adc_index < 5) && (adc_max[adc_index] != adc_val)) {
	        	val_index = adc_index;

	        	for (adc_index = 4; adc_index    > val_index; adc_index--) {
	        		adc_max[adc_index] = adc_max[adc_index-1];
	        		adc_val_cnt[adc_index] = adc_val_cnt[adc_index-1];
	        	}
	            adc_max[val_index] = adc_val;
	            adc_val_cnt[val_index] = 0;
	        }

	        for (adc_index = 0; adc_index < 5; adc_index++) {
	        	if (adc_max[adc_index] == adc_val) {
	        		adc_val_cnt[adc_index]++;
	        		break;
	        	}
	        }
	        
			//printf ("adc value = %d; \n", adc_val);
	    } 
	    else {
	    	//查找最多数据的
	    	for (adc_index = 0; adc_index < 4; adc_index++) {
				if (adc_val_cnt[adc_index] >= ADC_JITTER_VAL/5) {
					val_index = adc_index;
					break;
				}
	        } 

	        for (adc_index = 0; adc_index < 5; adc_index++) {
				printf ("adc_max[%d]= %d; adc_val_cnt[%d] = %d;\n",
				adc_index, adc_max[adc_index], adc_index, adc_val_cnt[adc_index]);
			}

			for (adc_index = 0; adc_index < 5; adc_index++) {
				if (adc_max[val_index] < (u8)tbl_adc_detect_val[adc_index][0]) {
					break;
				}
			}
			
			*adc_event = tbl_adc_detect_val[adc_index][1];
	        memset(adc_max, 0x00, sizeof(adc_max));
	        memset(adc_val_cnt, 0x00, sizeof(adc_val_cnt));
	        jitter_cnt = 0;
	    }
	}
	else {
		memset(adc_max, 0x00, sizeof(adc_max));
		memset(adc_val_cnt, 0x00, sizeof(adc_val_cnt));
		jitter_cnt = 0;
		*adc_event = EVT_ADC_DET_IDE;
	}
}
#endif


#if USER_KEY_KNOB_EN
//16级旋钮
AT(.com_rodata.port.key)
const u8 tbl_knob_level_16[16 + 1] = {
    0x02, 0x13, 0x24, 0x36, 0x47, 0x59, 0x6A, 0x7B,
    0x8B, 0x9A, 0xA0, 0xB2, 0xC3, 0xD5, 0xE6, 0xF8,
    0xFF,
};
#endif

AT(.com_text.port.key)
void plugin_saradc_sel_channel(u16 *adc_ch)
{
#if USER_KEY_KNOB_EN
    u8 mic_vol_val = (u8)(adc_cb.sfr[ADCCH_PE5] >> 2);
    u8 echo_level_val = (u8)(adc_cb.sfr[ADCCH_PE6] >> 2);

    static u8 echo_level = USER_KEY_KNOB_LEVEL, mic_vol = USER_KEY_KNOB_LEVEL;

    key_knob_process(echo_level_val, tbl_knob_level_16, &echo_level);
    if (echo_level != sys_cb.echo_level) {
        sys_cb.echo_level = echo_level;
        msg_enqueue(EVT_ECHO_LEVEL);
    }

    key_knob_process(mic_vol_val, tbl_knob_level_16, &mic_vol);
    if (mic_vol != sys_cb.mic_vol) {
        sys_cb.mic_vol = mic_vol;
        msg_enqueue(EVT_MIC_VOL);
    }
#endif

#if USER_ADC_DETECT_EN
	static uint16_t prev_detect_event = EVT_ADC_DET_IDE, detect_event = EVT_ADC_DET_IDE;

	adc_detect_process(&detect_event, (u8)(adc_cb.sfr[ADCCH_PE6] >> 2));
	
	if (detect_event != prev_detect_event) {
		printf ("adc detect event = %x;\n", detect_event);
		prev_detect_event = detect_event;
		msg_enqueue(detect_event);
	}
#endif
}

#if (MUSIC_SDCARD_EN || MUSIC_SDCARD1_EN)
u8 get_sddet_io_num(u8 iosel)
{
    u8 io_num;
    if (iosel <= IO_PF5) {
        io_num = iosel;
    } else if (iosel == IO_MUX_SDCLK) {
        io_num = SDCLK_IO;
    } else if (iosel == IO_MUX_SDCMD) {
        io_num = SDCMD_IO;
    } else {
        io_num = IO_NONE;
    }
    return io_num;
}
#endif // MUSIC_SDCARD_EN

//蓝牙休眠时，唤醒IO配置
void sleep_wakeup_config(void)
{
#if USER_ADKEY
    if (xcfg_cb.user_adkey_en) {
        adcch_io_wakeup_config(ADKEY_CH);           //配置ADKEY IO下降沿唤醒。
    }
#endif // USER_ADKEY

#if USER_ADKEY2
    if (xcfg_cb.user_adkey2_en) {
        adcch_io_wakeup_config(ADKEY2_CH);          //配置ADKEY1 IO下降沿唤醒。
    }
#endif // USER_ADKEY2

#if USER_ADKEY_MUX_SDCLK
    if (xcfg_cb.user_adkey_mux_sdclk_en) {
        adcch_io_wakeup_config(SDCLK_AD_CH);      //IO下降沿唤醒。
    }
#endif // USER_ADKEY_MUX_SDCLK

#if USER_IOKEY
    if (xcfg_cb.user_iokey_en) {
        if (xcfg_cb.iokey_config_en) {
            wakeup_gpio_config(xcfg_cb.iokey_io0, 1);   //配置IO下降沿唤醒。
            wakeup_gpio_config(xcfg_cb.iokey_io1, 1);
            wakeup_gpio_config(xcfg_cb.iokey_io2, 1);
            wakeup_gpio_config(xcfg_cb.iokey_io3, 1);
            wakeup_gpio_config(xcfg_cb.iokey_io4, 1);
        } else {
            //不用工具配置IOKEY时，根据实际使用的IOKEY IO进行修改。
            wakeup_gpio_config(IO_PF0, 1);
            wakeup_gpio_config(IO_PF1, 1);
            wakeup_gpio_config(IO_PF3, 1);
        }
    }
#endif // USER_IOKEY

#if USER_PWRKEY
    if (xcfg_cb.user_pwrkey_en) {
        wakeup_wko_config();
    }
#endif // USER_PWRKEY

#if MUSIC_SDCARD_EN
    //sdcard insert/remove wakeup
    if (xcfg_cb.music_sdcard_en) {
        u8 edge;
        if (dev_is_online(DEV_SDCARD)) {
            edge = 2;                           //sdcard remove wakeup(rising edge)
        } else {
            edge = 1;                           //sdcard insert wakeup(falling edge)
        }
        wakeup_gpio_config(get_sddet_io_num(xcfg_cb.sddet_iosel), edge);
    }
#endif // MUSIC_SDCARD_EN

#if MUSIC_SDCARD1_EN
    //sdcard1 insert/remove wakeup
    if (xcfg_cb.music_sdcard_en) {
        u8 edge;
        if (dev_is_online(DEV_SDCARD1)) {
            edge = 2;                           //sdcard1 remove wakeup(rising edge)
        } else {
            edge = 1;                           //sdcard1 insert wakeup(falling edge)
        }
        wakeup_gpio_config(get_sddet_io_num(xcfg_cb.sd1det_iosel), edge);
    }
#endif // MUSIC_SDCARD1_EN

#if MUSIC_UDISK_EN
    //udisk insert/remove wakeup
    if (xcfg_cb.music_udisk_en) {
        if (dev_is_online(DEV_UDISK)) {
            wakeup_udisk_config(1);             //udisk remove wakeup(falling edge)
        } else {
            wakeup_udisk_config(0);             //udisk insert wakeup(rising edge)
        }
    }
#endif // MUSIC_UDISK_EN
}

AT(.com_text.bsp.sys)
void lefmic_zero_detect(u8 *ptr, u32 samples)
{
#if MICL_MUX_DETECT_LINEIN
    lefmic_zero_detect_do(ptr, samples);
#endif // MICL_MUX_DETECT_LINEIN
}

bool bt_hfp_ring_number_en(void)
{
    return xcfg_cb.bt_hfp_ring_number_en;
}

bool is_func_fmrx_en(void)
{
    return xcfg_cb.func_fmrx_en;
}

AT(.com_text.bsp.sys)
bool is_sd_support(void)
{
#if (SD0_MAPPING == SD0MAP_G2) && EQ_DBG_IN_UART
    if (xcfg_cb.eq_dgb_uart_en && xcfg_cb.eq_uart_sel == 1) {
        return false;
    }
#endif
    return (MUSIC_SDCARD_EN * xcfg_cb.music_sdcard_en);
}

AT(.com_text.bsp.sys)
bool is_sd1_support(void)
{
#if (SD1_MAPPING == SD0MAP_G2) && EQ_DBG_IN_UART
    if (xcfg_cb.eq_dgb_uart_en && xcfg_cb.eq_uart_sel == 1) {
        return false;
    }
#endif
    return (MUSIC_SDCARD1_EN * xcfg_cb.music_sdcard_en);
}

AT(.com_text.bsp.sys)
bool is_usb_support(void)
{
#if EQ_DBG_IN_UART
    if (xcfg_cb.eq_dgb_uart_en && xcfg_cb.eq_uart_sel == 2) {
        return false;
    }
#endif
    return ((MUSIC_UDISK_EN * xcfg_cb.music_udisk_en) | FUNC_USBDEV_EN);
}

bool is_sleep_dac_off_enable(void)
{
    if (xcfg_cb.linein_det_iosel == IO_MUX_MICL) {
        return false;
    }

    return true;
}

//设置WAV RES提示音播放的数字音量 (0 ~ 0x7fff)
u32 get_piano_digvol(void)
{
    return 0x6000;
}

//设置WAV RES提示音播放的数字音量 (0 ~ 0x7fff)
u32 get_wav_res_digvol(void)
{
    return 0x5000;
}

void maxvol_tone_play(void)
{
#if WARING_MAXVOL_MP3
    if (func_cb.mp3_res_play) {
        func_cb.mp3_res_play(RES_BUF_MAX_VOL_MP3, RES_LEN_MAX_VOL_MP3);
        if (sys_cb.maxvol_fade) {
            sys_cb.maxvol_fade = 2;
            dac_fade_out();     //抬键的时候再淡入
        }
    }
#else
    #if SYS_KARAOK_EN
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
    #endif

    #if (WARING_MAXVOL_FORMAT & WARING_TONE)
    //tone提示音
    bsp_piano_warning_play(WARNING_TONE, TONE_MAX_VOL);

    #elif (WARING_MAXVOL_FORMAT & WARING_WAV)
    //wav提示音
    wav_res_play(RES_BUF_MAX_VOL_WAV, RES_LEN_MAX_VOL_WAV,&wavres_cb);
    #endif

    #if SYS_KARAOK_EN
    bsp_karaok_init(AUDIO_PATH_KARAOK, func_cb.sta);
    #endif

#endif
}

void minvol_tone_play(void)
{

}

void plugin_playmode_warning(void)
{

}

void plugin_lowbat_vol_reduce(void)
{
#if LOWPWR_REDUCE_VOL_EN
    music_src_set_volume(0x50c0);       //设置音乐源音量达到整体降低系统音量 (范围：0~0x7fff)
#endif // LOWPWR_REDUCE_VOL_EN
}

void plugin_lowbat_vol_recover(void)
{
#if LOWPWR_REDUCE_VOL_EN
    music_src_set_volume(0x7fff);       //还原音量
#endif // LOWPWR_REDUCE_VOL_EN
}

//用于调DAC音质接口函数
void plugin_music_eq(void)
{
    bsp_eq_init();
    sys_cb.eq_mode = 0;
    music_set_eq_by_num(sys_cb.eq_mode);
}

AT(.com_text.port.vbat)
void plugin_vbat_filter(u32 *vbat)
{
#if  VBAT_FILTER_USE_PEAK
    //电源波动比较大的音箱方案, 取一定时间内的电量"最大值"或"次大值",更能真实反应电量.
    #define VBAT_MAX_TIME  (3000/5)   //电量峰值判断时间 3S
    static u16 cnt = 0;
	static u16 vbat_max_cnt = 0;
    static u32 vbat_max[2] = {0,0};
    static u32 vbat_ret = 0;
    u32 vbat_cur = *vbat;
    if (cnt++  < VBAT_MAX_TIME) {
        if (vbat_max[0] < vbat_cur) {
            vbat_max[1] = vbat_max[0];  //vbat_max[1] is less max (次大值)
            vbat_max[0] = vbat_cur;     //vbat_max[0] is max(最大值)
            vbat_max_cnt = 0;
        } else if (vbat_max[0] == vbat_cur) {
            vbat_max_cnt ++;
        }
    } else {
        if (vbat_max_cnt >= VBAT_MAX_TIME/5) {  //总次数的(1/5)都采到最大值,则返回最大值.
            vbat_ret = vbat_max[0];
        } else if (vbat_max[1] != 0) {   //最大值次数较少,则返回次大值(舍弃最大值)
            vbat_ret = vbat_max[1];
        }
        vbat_max[0] = 0;
        vbat_max[1] = 0;
        vbat_max_cnt = 0;
        cnt = 0;
    }
    //返回值
    if (vbat_ret != 0) {
        *vbat = vbat_ret;
    }
#endif
}

//初始化完成, 各方案可能还有些不同参数需要初始化,预留接口到各方案
void plugin_sys_init_finish_callback(void)
{
#if ENERGY_LED_EN
    energy_led_init();
#endif
}

bool plugin_func_idle_enter_check(void)
{
    //可以在这里决定否需要进入idle
    return true;
}

void plugin_hfp_karaok_configure(void)
{
    //android用于通话，iphone用于K歌
#if BT_HFP_CALL_KARAOK_EN
    if (bt_is_ios_device()) {
        sys_cb.hfp_karaok_en = 1;
    } else {
        sys_cb.hfp_karaok_en = 0;
    }
#endif
}

//用于karaok初始化
void plugin_karaok_init(void)
{
#if SYS_KARAOK_EN
    sys_cb.echo_delay = SYS_ECHO_DELAY;
#if !USER_KEY_KNOB_EN   //没有旋钮的话就固定一个值
    sys_cb.music_vol = USER_KEY_KNOB_LEVEL - 1;
    sys_cb.echo_level = USER_KEY_KNOB_LEVEL - 1;
    sys_cb.mic_vol = USER_KEY_KNOB_LEVEL - 1;
#endif
#endif
}

#if SYS_MAGIC_VOICE_EN
const u8 magic_table[4][2] = {
    {0, 5},
    {0, 2},
    {1, 1},
    {1, 4},
};

void magic_voice_switch(void)
{
    static int magic_sel = 0;
    magic_sel++;
    if (magic_sel > 4) {
        magic_sel = 0;
    }

    sys_cb.magic_type = magic_table[magic_sel - 1][0];
    sys_cb.magic_level = magic_table[magic_sel - 1][1];
    //printf("set mav:%d %d %d\n", magic_sel, sys_cb.magic_type, sys_cb.magic_level);
    karaok_set_mav(sys_cb.magic_type, sys_cb.magic_level);
}
#endif

#if MAGIC_REC_PLAY
#define AUCON0          SFR_RW (SFR1_BASE + 0x20*4)
#define AUDMAADR        SFR_RW (SFR1_BASE + 0x24*4)
#define MP3_TYPE    2
extern u32 dac_obuf[576 + 512] ;

typedef struct {
    volatile u8  sta;
    volatile u8  nslot;             //用于统计main线程是否有时间跑
    u8  type;
    u8  sysclk;                    //保存解码前的系统时钟
    u16 total_time;
    u16 cur_time_ms;
} music_cb_t;
extern music_cb_t music_cb;

void obuf_put_samples_w(void *buf, uint len);

//魔音化
static s16 buf_01[1536] AT(.echo_buf);
static s16 buf_02[128] AT(.echo_buf);
static s16 buf_03[128] AT(.echo_buf);
static s16 buf_04[512] AT(.echo_buf);
static s16 buf_05[64] AT(.echo_buf);
static s16 buf_06[64] AT(.echo_buf);
static char buf_07[384] AT(.echo_buf);

typedef struct {
    u8 res[2];
    s16 *buf_used;
    s16 *inbuf;
    s16 *outbuf;
    s16 *cache;
    s16 *mav_out;
    s16 *out1_buf;
    char *s8_fdata;
}mav_cfg_test;

mav_cfg_test mav_cfg_te = {
    .res[0]         = 0,
    .res[1]         = 0,
    .buf_used           = buf_01,
    .inbuf              = buf_02,
    .outbuf             = buf_03,
    .cache              = buf_04,
    .mav_out            = buf_05,
    .out1_buf           = buf_06,
    .s8_fdata           = buf_07,
};

void magic_voice_init(void *cfg);
void mav_kick_start(void);
void magic_voice_set_level(u8 type, u8 level);

void magic_config(void)
{
   magic_voice_init(&mav_cfg_te);
}

const u8 magic_table[4][2] = {
    {0, 5},
    {0, 2},
    {1, 1},
    {1, 4},
};

//切换魔音
void magic_voice_switch_ex(void)
{
    //设置魔音级别
    u8 magic_type,magic_level;
    static int magic_sel = 0;
    magic_sel++;
    if (magic_sel > 4) {
        magic_sel = 0;
    }

    magic_type = magic_table[magic_sel - 1][0];
    magic_level = magic_table[magic_sel - 1][1];
    magic_voice_set_level(magic_type, magic_level);
}


AT(.mp3dec.dac)
void mpeg_pcm_output(void)
{
    if (1) {
        if (!(AUCON0 & BIT(26))) {
            AUCON0 |= BIT(26);                      //MPEG DMA to RAM
            AUDMAADR = DMA_ADR(&dac_obuf[576]);
        } else {
            u16 frame_len;
            if (music_cb.type == MP3_TYPE) {
                frame_len = 576;
            } else {
                frame_len = 384;
            }

            if(func_cb.sta == FUNC_EXSPIFLASH_MUSIC)
            {
                #if SPIFALSH_MUSIC_PLAY_REC
                frame_len = 384;
                #endif // SPIFALSH_MUSIC_PLAY_REC
                  for(u8 count=0;count<((frame_len<<2)>>8);count++)
                  {
                      memcpy(mav_cfg_te.inbuf, &dac_obuf[576+(64*count)], 128*2);
                      memcpy(&dac_obuf[576+(64*count)], mav_cfg_te.outbuf, 128*2);
                      mav_kick_start(); //魔音开始
                  }
            }
            obuf_put_samples_w(&dac_obuf[576], frame_len);
        }
    } else {
        AUCON0 &= ~BIT(26);
    }
}
#endif // MAGIC_REC_PLAY
