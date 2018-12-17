/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：COMMON 一些公共功能接口相关的宏定义，枚举类型，结构体定义，外部接口声明等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _common_func_H_
#define _common_func_H_

#include "psp_includes.h"
#include "case_independent.h"
#include "enhanced.h"
#include "applib.h"
#include "setting_common.h"
#include "tts_interface.h"
#include "common_rcp.h"
#include "config_id.h"
#include "common_btmanager.h"
#include "common_discrete_led.h"
#include "common_time_alarm.h"
#include "app_view.h"
#include "common_tts.h"
#include "common_keytone.h"
#include "common_speaker.h"
#include "common_aset_interface.h"
#include "common_ota.h"

/*! 音量映射表总数 */
#define VOLUME_TABLE_MAX            9

/*! 音量最大值 31，总共 32 级 */
#define VOLUME_VALUE_MAX            31

/*! IC 内部 PA 音量级别 41 级 */
#define ANA_PA_VOLUME_MAX           41

/*! 信号能量大小默认值，单位为0.1db */
#define SIGNAL_ENERGY_DEFAULT       (-60)

//ESD参数在vram结构
typedef struct
{
    uint16 magic; //VM 变量魔数
    bool esd_flag;
    uint8 ap_id; //关机的应用
    uint8 engine_state;
    uint8 reserve[1];
} g_config_var_t;


typedef struct
{
    uint8 vol_pa;
    uint8 vol_da;
} vol_hard_t;

/*! 循环切换应用数目 */
#define MAX_FUNCTON_CYCLE   7

/*! 后台打电话状态*/
typedef enum
{
    CALL_BACK_STATUS_NULL = 0, //无打电话状态
    CALL_BACK_STATUS_IN = 1, //切换到后台打电话
    CALL_BACK_STATUS_BACK = 2, //从后台打电话切换回来
} call_back_status_e;

/*! 前台应用切换相关全局变量 */
typedef struct
{
    /*! 用于指示是否支持功能（应用模式），最多支持32个功能（应用模式） */
    uint32 app_func_support;
    //对应config.txt中SETTING_APP_SWITCH_SEQUENCE项的中括号里面的数字
    uint8 app_switch_seq[MAX_FUNCTON_CYCLE];
    //对应config.txt中SETTING_APP_SWITCH_SEQUENCE项的中括号前面的数字
    uint8 app_default_func_id;

    /*! 是否从后台打电话切换回来，1表示正进入后台打电话，2表示从后台打电话切换回去，0表示无 */
    uint8 call_background_status;
    /*! 闹钟闹铃目录扫描返回 */
    app_result_e result_alarm_ring_scan_backup;

    /*!进入APK ALARM*/
    uint8 apk_alarm_flag;
    uint8 apk_alarm_last_func;
    void* apk_alarm_backup_cb;
    void* apk_alarm_backup_global_cb;

    /*! 是否闹钟开机 */
    bool rtcalarm_poweron;
    /*! 是否闹钟完毕后关机 */
    bool rtcalarm_poweroff;

    /*! 是否要禁止回连 */
    bool forbid_auto_connect;

    /*! 是否通过APK/APP切换到收音机应用 */
    bool switch_to_radio_by_apkapp;

    /*! 从S2唤醒返回 */
    bool wake_from_s2;
    /*! 从S2唤醒方式 */
    uint8 wake_ret_from_s2;
    /*!是否插拔过card music加载引擎时使用*/
    uint8 card_in_out_flag;
    /*!是否插拔过hostmusic加载引擎时使用*/
    uint8 uhost_in_out_flag;

    /*!是否是tws电话来断开副箱*/
    uint8 tws_phon_come_flag;



    /*!S3BT NOR备份区是否已擦写*/
    //bool s3bt_nor_erase_flag;
} ap_switch_var_t;

//common key处理接口声明
extern bool com_gui_msg_hook(input_gui_msg_t *input_msg) __FAR__;
extern void com_filter_key_hold(void) __FAR__;
extern void com_filter_key_up(void) __FAR__;
extern void com_filter_key_itself(void) __FAR__;

//声音输出管理接口声明
extern bool com_set_sound_out(soundout_state_e state) __FAR__;
extern void com_switch_volume_mode(volume_mode_e mode) __FAR__;
extern void com_set_volume_gain(uint8 gain) __FAR__;
#define SET_VOLUME_SYNC_TO_REMOTE  (1<<0) //同步到手机等远程设备
extern bool com_set_sound_volume(uint8 set_vol, uint8 set_mode) __FAR__;
extern uint8 com_get_sound_volume(void) __FAR__;
extern bool com_reset_sound_volume(uint8 set_mode) __FAR__;

extern void com_set_phy_volume(uint8 set_vol) __FAR__;
extern void com_update_volume_limit(int8 vol_limit) __FAR__;
extern bool com_set_mute(bool mute) __FAR__;
#ifdef ENABLE_TRUE_WIRELESS_STEREO
extern bool com_switch_mute(uint8 op1,uint8 op2,bool op3)__FAR__;
#else
extern bool com_switch_mute(void) __FAR__;
#endif

typedef enum
{
    /*! 普通 */
    EQ_NOR = 0,
    /*!爵士 */
    EQ_JAZZ,
    /*! 流行*/
    EQ_POP,
    /*!经典 */
    EQ_CLASSIC,
    /*!柔和 */
    EQ_SOFT,
    /*! 重低音*/
    EQ_DBB,
    /*!摇滚 */
    EQ_ROCK,
    /*!用户自定义EQ*/
    EQ_USR_DEF,
} com_eqtype_e;

#define SETEQ_TYPE  0 //指定EQ类型
#define SETEQ_NEXT  1 //向下切EQ
#define SETEQ_PREV  2 //向上切EQ
extern bool com_set_sound_eq(uint8 mode, com_eqtype_e eq_type, uint8 *eq_para) __FAR__;
extern void com_eq_next(void) __FAR__;
extern void com_eq_prev(void) __FAR__;
extern void com_eq_set(void) __FAR__;

#define DAE_MODE_NONE   0 //BYPASS模式
#define DAE_MODE_EQ     1 //图形EQ模式（后续可合并到DAE模式：GEQ）
#define DAE_MODE_DAE    2 //DAE模式
#define DAE_OPTION_VBASS      0
#define DAE_OPTION_TREBLE     1
#define DAE_OPTION_VSURROUND  2
#define DAE_OPTION_SFR        3
#define DAE_OPTION_PEQ        4
#define DAE_OPTION_MDRC       5
#define DAE_OPTION_DAE_ENH    6
#define DAE_OPTION_DAE_WKN    7

extern void com_load_dae_config(comval_t *setting_comval) __FAR__; //该接口仅仅在升级后第一次开机调用一次
extern void com_reset_dae_config(comval_t *setting_comval) __FAR__; //该接口仅仅在CONFIG开机时调用一次
extern void com_init_dae_config(comval_t *setting_comval) __FAR__; //该接口仅仅在CONFIG开机时调用一次
extern bool com_set_dae_config(daemode_param_t *daemode_param) __FAR__; //用户设置DAE配置参数
extern bool com_set_dae_config_dynamic(void) __FAR__; //动态设置DAE配置参数
extern bool com_set_dae_onoff(bool onoff) __FAR__;
extern bool com_set_dae_enable(bool enable) __FAR__;
extern bool com_set_dae_chan(bool aux_flag, bool variable_mode) __FAR__;
extern bool com_load_mdrc_config(dae_config_t *p_dae_cfg, bool load_bin, bool update_signal_detect) __FAR__;
extern void com_update_dae_config(dae_config_t *p_dae_cfg) __FAR__;
extern void com_detect_energy_realtime(void) __FAR__;
void __section__(".text.bank") update_peq_para(comval_t *setting_comval,uint16 enable_id,uint16 para_id)__FAR__;

extern void com_volume_add(bool tts_flag) __FAR__;
extern void com_volume_sub(bool tts_flag) __FAR__;
#define SET_VOLUME_VIEW  (1<<0) //显示VOL值
#define SET_VOLUME_TTS   (1<<1) //最大声音和最小声音 TTS 提示
extern void com_volume_set(uint8 set_vol, uint8 set_mode) __FAR__;
extern void dynamic_switch_waves_bin_para(uint8 bin_number)__FAR__;
extern uint8 get_dae_bypass_status(void)__FAR__;

/*!
 *  \brief
 *  this_filter_key_value 过滤掉按键长按动作，要过滤的按键
 */
extern uint8 this_filter_key_value;

/*!
 *  \brief
 *  sys_counter_timer_id 系统定时功能（包括背光，屏保，省电关机，定时关机，等）计时器 timer id
 */
extern int8 sys_counter_timer_id;

/*!
 *  \brief
 *  peripheral_detect_timer_id 外设检测（包括USB，UHOST，CARD，AUX）定时器 timer id
 */
extern int8 peripheral_detect_timer_id;

/*!
 *  \brief
 *  是否需要忽略切换静音
 */
extern bool g_ignore_switch_mute;

extern bool g_detect_card_force_flag;

//杂项功能接口声明（详细说明见接口定义）
extern app_result_e com_app_sleep(uint32 sleep_timer) __FAR__;
extern void sys_timer_init(void) __FAR__;
extern void sys_timer_exit(void) __FAR__;
extern void sys_counter_handle(void) __FAR__;
extern void peripheral_detect_handle(void) __FAR__;
extern void com_setting_comval_init(comval_t *setting_comval) __FAR__;
extern void com_battery_charge_deal(void) __FAR__;
extern void com_app_msg_hook(private_msg_t *private_msg) __FAR__;

//for ap switch
extern ap_switch_var_t g_ap_switch_var;
extern void com_ap_switch_deal(app_result_e app_result) __FAR__;
extern uint8 com_ap_switch_ask_next_func(app_result_e app_result) __FAR__;
extern uint32 com_check_ap_func_index_valid(void) __FAR__;
//屏保
extern void com_set_display_onoff(bool onoff) __FAR__;

//加载公共数据
#define COM_SHARE_DATA_RING_FLODER_ROOT    0
#define COM_SHARE_DATA_RING_FOLDER_SUB     1
#define COM_SHARE_DATA_RING_BUILDIN        2
#define COM_SHARE_DATA_RING_TYPE           3
#define COM_SHARE_DATA_REC_FOLDER          4
extern int com_load_share_data(int data_id, uint8 *data_buf, int data_len, int param) __FAR__;

//standby相关宏定义和外部接口
//S2
#define WAKE_TYPE_ONOFF_KEY                (1 << 0)
#define WAKE_TYPE_PHY_KEY                  (1 << 1)
#define WAKE_TYPE_IR_KEY                   (1 << 2)
#define WAKE_TYPE_I2C_KEY                  (1 << 3)
#define WAKE_TYPE_BT_RX                    (1 << 4)
#define WAKE_TYPE_AUX_DETECT               (1 << 5)
#define WAKE_TYPE_CARD_DETECT              (1 << 6)
#define WAKE_TYPE_UHOST_DETECT             (1 << 7)

//唤醒结果
#define WAKE_NOSLEEP       (0)
#define WAKE_BY_USB        (1)
#define WAKE_BY_KEY        (2)
#define WAKE_BY_BT         (3)
#define WAKE_BY_ALARM      (4)
#define WAKE_BY_AUX_DET    (5)
#define WAKE_BY_CARD_DET   (6)
#define WAKE_BY_UHOST_DET  (7)
#define WAKE_BY_LOWPOWER   (8)

typedef struct
{
    uint32 bak_24MPLL_CTL;
    uint32 bak_CORE_PLL_CTL;
    uint32 bak_CMU_SYSCLK;
    uint32 bak_DAC_ANACTL;
    uint32 bak_AUDIO_PLL_CTL;
    uint8  high_freq_flag; //高频标志，0表示低频，1表示高频
    uint8  i2c_key_timer; //触摸屏检测周期
    uint8  card_detect_timer; //命令卡检测周期
} low_power_var_t;

extern low_power_var_t g_low_power_var;
extern app_result_e com_standby(uint32 wake_type);

//S3
typedef void (*standby_enter_hook)(void);
typedef void (*standby_exit_hook)(void);
extern standby_enter_hook g_standby_enter_cb;
extern standby_exit_hook g_standby_exit_cb;
extern bool g_standby_exit_flag;
extern void com_standby_register(standby_enter_hook enter_cb, standby_exit_hook exit_cb);

//ESD RESTART
extern bool g_esd_restart_flag;

extern bool g_need_auto_connect_timeout;
extern bool g_need_reset_controller_timeout;
extern bool g_neednot_tts_play_timeout;
extern bool g_neednot_tts_play_reset;

extern bool g_neednot_tts_play_timeout1;
extern uint8 g_prev_next_ch_start;
extern int8 prev_next_ch_timer_id;


extern g_config_var_t g_config_var;
extern bool g_esd_cardreader_flag;
#ifdef __ESD_MODE_
extern uint8 g_end_autoconnt ;
#endif
extern int com_math_exp_fixed(int db) __FAR__;

#endif
