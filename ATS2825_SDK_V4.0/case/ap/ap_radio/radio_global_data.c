#include "app_radio.h"

/*!
 *  \brief
 *  app_timer_vector：应用软定时器数组指针，指向应用空间的软定时器数组
 */
app_timer_t radio_app_timer_vector[COMMON_TIMER_COUNT + APP_TIMER_COUNT];

// for alarm_fm
// radio进入模式
uint8 g_radio_from_alarm = 0;
//alarm_fm的结束时间
time_t alarmfm_end_time;
// alarm_fm音量
uint8 alarmfm_volume;

/*****************************************************
 * 只读类全局变量
 ******************************************************/

//各波段最大频点
const uint32 max_freq[] =
{ MAX_US, MAX_JP, MAX_EU };

//各波段最小频点(单位kHz)
const uint32 min_freq[] =
{ MIN_US, MIN_JP, MIN_EU };

/*****************************************************
 * 配置类相关变量
 ******************************************************/

//系统全局变量
comval_t g_comval;

//radio ui 的配置项信息
radio_config_t g_radio_config;

//引擎状态
Engine_Status_t g_engine_status;

/*****************************************************
 * 全局buffer
 ******************************************************/

//自动搜台中用于保存有效电台的临时buffer
uint32 Auto_tab[MAX_STATION_COUNT];

/*****************************************************
 * 流程控制相关及其他
 ******************************************************/

/* 指向当前所播放的电台列表表首*/
uint32 *pfreq;

//标识当前所处应用场景
radio_scene_e g_radio_scene;

//备份进行半自动搜台前的频率
uint32 startfreq;

/*****************************************************
 * 用于数字选台
 ******************************************************/

//存储数字输入的buffer
uint8 g_digital_buf[MAX_DIGITAL_COUNT];

//当前数字健的个数
uint8 g_digital_count;

//搜台门限值 FMENGINE_AP_ID_SEEKTH
uint8 g_search_throd;

//是否从录音返回
app_param_e g_enter_param;

//KEY_NEXT_VOLADD 和 KEY_PREV_VOLSUB 长按调音量标志
bool g_next_prev_flag = FALSE;
//for RCP
//是否正在搜台中
bool g_searching_flag = FALSE;
//FM是否在播放
bool g_playing_flag = FALSE;

uint8 g_freq_tts_sec_ids[10];

uint32 g_search_current_freq;
bool g_digit_input_flag;
bool g_first_enter_play;

bool g_neednot_freq_tts_play;

//初始化阶段，可以过滤掉一些事件处理
bool g_radio_init_flag;

//for set release mute timer
int8 g_mute_set_timer = -1;
