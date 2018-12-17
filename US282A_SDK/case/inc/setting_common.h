/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：系统功能配置相关的宏定义，枚举类型，结构体定义等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _SETTING_COMMON_H
#define _SETTING_COMMON_H

#include "psp_includes.h"
#include "case_type.h"
#include "mmm_dae_def.h"

typedef enum
{
    VOLUME_MODE_NORMAL = 0,
    VOLUME_MODE_CALL = 1,
    VOLUME_MODE_MAX = 2,
} volume_mode_e;

typedef enum
{
    LOW_POWER_MODE_NONE = 0,
    LOW_POWER_MODE_S2 = 1,
    LOW_POWER_MODE_S3 = 2,
} low_power_mode_e;

typedef struct
{
    /*! 默认数值 */
    uint16 default_value;
    /*! 有效值个数 */
    uint16 total;
    /*! 有效值数组，可变长数组 */
    peq_band_t eq_data;
} peq_config_t;

typedef struct
{
    /*! 默认数值 */
    uint16 default_value;
    /*! 有效值个数 */
    uint16 total;
    /*! 滤波器阶数 */
    short filter_order;
    /*! 左声道滤波器Q值 */
    short filter_QvalLeft;
    /*! 右声道滤波器Q值 */
    short filter_QvalRight;
} spk_compensation_config_t;

typedef struct
{
    short threshold;
    short attack_time;
    short release_time;
    short crossover_freq;
} mdrc_band_config_t;

typedef struct
{
    /*! 默认数值 */
    uint16 default_value;
    /*! 有效值个数 */
    uint16 total;
    /*! 有效值数组，可变长数组 */
    mdrc_band_config_t mdrc_data;
} mdrc_config_t;

typedef struct
{
    /*! 截止频率，单位Hz */
    uint16 cutoff;
    /*! 保留参数 */
    short  reserve_param;
    /*! 开始音量，单位为 0.1db */
    short  rel_vol_start;
    /*! 保持音量，单位为 0.1db */
    short  rel_vol_keep;
    /*! 最大增益，单位为 0.1db */
    short  gain;
} dew_band_t;

typedef struct
{
    /*! 默认数值 */
    uint16 default_value;
    /*! 有效值个数 */
    uint16 total;
    /*! 音效增强减弱设置 */
    dew_band_t dew_band_setting;
} dew_config_t;

typedef struct
{
    //统计周期时长，以1ms为单位
    short period;
    //有效周期数
    short period_count;
    //信号大小降低较多才启动预调机制，以0.1db为单位
    short preadjust_db;
    //开始预调降低信号大小的周期数，然后每个周期降低相应比例
    short preadjust_count;
    //信号大小有效最小值，以0.1db为单位
    short signal_energy_min;
} my_signal_config_t;

typedef struct
{
    /*! 默认数值 */
    uint16 default_value;
    /*! 有效值个数 */
    uint16 total;
    /*! 信号检测参数 */
    my_signal_config_t signal_cfg;
} enhanced_signal_config_t;

/*********为兼容标准音效模型增加的DAE参数**********/
typedef struct
{
    /*! 默认数值 */
    uint16 default_value;
    /*! 有效值个数 */
    uint16 total;
    /*! 信号检测参数 */
    dae_compressor_standard_mode_t compressor;
} dae_compressor_temp_standard_mode_t;

typedef struct
{
    /*! 默认数值 */
    uint16 default_value;
    /*! 有效值个数 */
    uint16 total;
    /*! 信号检测参数 */
    int16 vol_adjust;
    mdrc_extend_para_standard_mode_t mdrc;
} mdrc_extend_para_temp_standard_mode_t;

typedef struct
{
    /*! 默认数值 */
    uint16 default_value;
    /*! 有效值个数 */
    uint16 total;
    /*! 信号检测参数 */
    dae_mdrc_band_standard_mode_t mdrc_band;
} mdrc_para_temp_standard_mode_t;

typedef struct
{
    /*! 默认数值 */
    uint16 default_value;
    /*! 有效值个数 */
    uint16 total;
    /*! 信号检测参数 */
    mdrc_peak_detect_standard_mode_t mdrc_peak_detect;
} mdrc_peak_detect_temp_standard_mode_t;

typedef struct
{
    /*! 默认数值 */
    uint16 default_value;
    /*! 有效值个数 */
    uint16 total;
    /*! 信号检测参数 */
    vol_dae_para_sm_t vol_dae_para_sm;
} vol_dae_para_temp_standard_mode_t;

/**************************************************/

typedef struct
{
    short enable;				            //default: 200ms,step 0.01ms, set 200*100 = 20000    
    short DownExp_attack_time;				//default: 200ms,step 0.01ms, set 200*100 = 20000
	short DownExp_release_time;				//default: 10ms,step 1ms, set 10
	short DownExp_set0_threshold;		    //default: 1ms, step 0.01ms, set 100;
	short DownExp_threshold;			    //default: -80dB(min), step 0.1dB, set -800
	short DownExp_ratio;				    //default: 3(greater than 1)  
}noise_reduction_t;

typedef struct
{
    /*! 默认数值 */
    uint16 default_value;
    /*! 有效值个数 */
    uint16 total;

    noise_reduction_t noise_reduction;
}noise_reduction_config_t;

/*!
 * \brief
 *  comval_t 系统公共变量组合结构体
 */
typedef struct
{
    /*! magic，值为 0x55AA 时有效 */
    uint16 magic;

    /*! 支持卡选择  0:不支持  1:支持 */
    uint8 support_cardplay;
    /*!支持u盘播放选择 0:不支持 1:支持*/
    uint8 support_uhostplay;
    /*!支持linein选择 0:不支持 1:支持*/
    uint8 support_linein;
    /*! 支持FM选择  0:不支持  1:支持 */
    uint8 support_fmradio;
    /*! 支持后台蓝牙 0:不支持  1:支持 */
    uint8 support_bt_inback;
    /*! 支持蓝牙推歌 0:不支持  1:支持 */
    uint8 support_btplay;
    /*! 支持蓝牙免提 0:不支持  1:支持 */
    uint8 support_btcall;
    /*! 支持USB音箱 0:不支持  1:支持 */
    uint8 support_usbbox;
    /*! 支持大容量磁盘 0:不支持  1:支持 */
    uint8 support_usbmsc;
    /*! 支持OTA 0:不支持  1:支持 */
    uint8 support_ota;    
    /*! 支持OTA歌曲测试 0:不支持  1:支持 */
    uint8 support_ota_song_test;    
    /*! 低功耗模式 */
    low_power_mode_e lowpower_mode;
    /*! 进入低功耗时间 */
    uint8 lowpower_timer;
    /*! 省电关机时间 以0.5秒为单位，0表示无省电关机 */
    uint8 poweroff_timer;
    /*! 蓝牙省电关机时间，以分钟为单位；该配置项在“省电关机时间”不为0时有效 */
    uint8 poweroff_bt_waitpair_timer;

    /*! 双击按键开关*/
    uint8 double_click_enable;
    /*! 双击按键延迟时间*/
    uint16 double_click_delay;

    /*! 按键音开关*/
    uint8 key_tone;
    /*! 按键音叠加开关*/
    uint8 key_tone_overlay;
    /*! 状态语音播报开关*/
    uint8 tts_enable;
    /*! 当前TTS语言类型*/
    uint8 tts_language;
    /*! 按键音和TTS最小音量，0表示在音量为0时没有按键音和TTS*/
    uint8 kt_tts_min_volume;
    /*! 按键音和TTS最大音量 */
    uint8 kt_tts_max_volume;
    /*! 按键音和TTS信号衰减，百分比 */
    uint8 kt_tts_percent;

    /*! 当前逻辑音量值：0~31 */
    uint8 volume_current;
    /*! 当前已经设置到PA的音量值：0~31，0xff表示无效 */
    uint8 volume_current_phy;
    /*! 当前音量模式 */
    volume_mode_e volume_mode;
    /*! 各种音量模式的音量 */
    uint8 volume[VOLUME_MODE_MAX];
    /*! 当前音量负增益（衰减），0 ~ 12 表示 0db ~ -12db */
    uint8 volume_gain;
    /*! 当前音量相对于最大音量的偏移值，单位为 0.1db，取值<=0 */
    int16 volume_relative;
    /*! 当前音量限制，相比于最大音量的衰减值，单位为 0.1db，取值<=0 */
    int8 volume_limit;

    /*! 是否支持LED数码管显示 */
    uint8 support_led_display;
    /*! LED数码管背光时间 以10秒为单位，0表示常亮 */
    uint8 light_timer;

    /*! 电池充电方式 */
    uint8 bat_charge_mode;
    /*! 充电满是否TTS提示 */
    uint8 bat_charge_full_tts;

    /*! 闹钟配置项 */
    uint8 snooze_timer;
    uint8 snooze_max;
    uint8 alarm_overtime;

    /*! 硬件层配置项 */
    uint8 hard_support_card;
    uint8 hard_support_uhost;
    uint8 hard_support_headphone;

    /*! 测试调试选项 */
    uint8 stub_enable;
    uint8 stub_print;

    /*! CASE 层自定义 Graphics EQ 类型*/
    uint8 eq_enable;
    uint8 eq_type;
    /*! MDRC音量微调 */
    int8 mdrc_vol_adjust;
    /*! mdrc音量补偿，需对信号进行衰减，百分比 */
    uint8 mdrc_percent;
    /*! DAE对任何频段提升最大只能到该峰值，超过部分会被截掉 */
    uint8 dae_mdrc_peak_gain;
    /*! DAE配置环境已初始化好的标志 */
    bool dae_inited_flag;
    bool default_vbass_enable; //默认虚拟低音使能开关
    bool set_dae_by_app_flag;  //通过APP设置音效标志
    bool w_input_enable;
    /*! 音效增强效果开关 */
    uint8 dae_enhanced_enable;
    /*! 音效减弱效果开关 */
    uint8 dae_weaken_enable;
    /*! 高通滤波器截止频率，0表示没有高通滤波器 */
    uint8 highpass_cutoff;
    /*! 音效增强尖峰比率，需要综合考虑增强PEQ点相互影响程度，以及低音增强和高音增强，单位为0.1 */
    uint8 dae_enhanced_peak_ratio;

    bool aux_flag; //LINEIN场景标志
    bool signal_variable_mode; //信号可变模式，比如AUX通道，不支持音量同步的蓝牙推歌等
    bool dae_cfg_variable; //DAE参数是否可变，如果打开mdrc，或音效增强减弱，那么就是可变的

    /*! 解码后音频信号能量检测使能 */
    uint8 signal_energy_enable;
    short signal_energy_used; //已经使用了的有效信号能量，最大值为0db，单位为0.1db
    short signal_energy; //当前有效信号能量
    short signal_energy_min; //有效信号能量最小值

   /*********为兼容标准音效模型增加的DAE参数**********/
    //bool audiopp_type;                      //音效库类型
    int8 mdrc_vol_adjust_standard_mode;     //音量微调
    
   /**************************************************/

    /*! DAE配置项 */
    dae_config_t dae_cfg;
} comval_t;

#define OTA_UPGRADE_ENABLE		(0x7c8a)
/*!
 * \brief
 *  otaval_t OTA变量组合结构体
 */
typedef struct
{
    /*! magic，值为 0x55AA 时有效 */
    uint16 magic;
	
    /*! 升级标志，值为 OTA_UPGRADE_ENABLE(0x7c8a) 时有效 */
	uint16 ota_upgrade_flag;
} otaval_t;

/*!
 * \brief
 *  testval_t test mode进入标志
 */
typedef struct
{
    /*! magic，值为 0x55AA 时有效 */
    uint16 magic;
	
    /*! test mode标志,0x01:FT mode,0x02:bqbmode */
	uint8 api_flag;

	uint16 usb_cardreader_magic;

	uint16 usb_sound_magic;

	uint32 usb_cardreader_low;

	uint32 usb_cardreader_high;
	
	uint32 usb_sound_low;

	uint32 usb_sound_high;

} testval_t;


/*!
 *  \brief
 *  sys_comval 系统参数指针，通过 com_set_sys_comval 接口传递给 common 并保存在全局指针
 */
extern comval_t *sys_comval;

#endif //_SETTING_COMMON_H
