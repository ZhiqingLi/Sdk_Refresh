/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：APPLIB 模块宏定义，结构体定义，外部接口声明等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _applib_H_
#define _applib_H_

#include "psp_includes.h"
#include "case_independent.h"
#include "case_type.h"
#include "led_driver.h"
#include "key_interface.h"

#include "gui_msg.h"
#include "app_msg.h"
#include "app_timer.h"
#include "common_config.h"

/*! 方案最多支持 4 个应用同时运行 */
#define MAX_APP_COUNT           4

/*! 验证APK/APP状态*/
#define VERIFICATION_NULL       0x00
#define VERIFICATION_START      0x01
#define VERIFICATION_PASS       0x02
#define VERIFICATION_FAIL       0x03

//需要重新扫描U盘里面的音乐文件
#define UHOST_NEED_SCAN     (1 << 4)
//需要上报U盘里面的文件列表给app/apk
#define UHOST_NEED_UPLOAD   (1 << 5)
#define UHOST_NEED_MASK     (UHOST_NEED_SCAN | UHOST_NEED_UPLOAD)

//需要重新扫描卡里面的音乐文件
#define CARD_NEED_SCAN     (1 << 0)
//需要上报卡里面的文件列表给app/apk
#define CARD_NEED_UPLOAD   (1 << 1)
#define CARD_NEED_MASK     (CARD_NEED_SCAN | CARD_NEED_UPLOAD)

/*! 系统工作状态：根据各种状态总结出来，只在 sys_counter 中使用 */
typedef enum
{
    SYS_STATUS_IDLE      = 0,
    SYS_STATUS_WAIT_LINK = 1,
    SYS_STATUS_WORKING   = 2,
} sys_status_e;

#define UI_IDLE_TIMER    14 //空闲状态持续7秒钟就调低频率
/*! 前台用户交互状态 */
typedef enum
{
    UI_STATUS_IDLE         = 0, //空闲状态持续7秒钟后就调低频率
    UI_STATUS_IDLE_LOWFREQ = 1, //空闲且已调低频率状态
    UI_STATUS_BUSY         = 2, //表示正在进行初始化，切歌等短暂需要加速的事情
} ui_status_e;

/*!
 * \brief
 *  app_type_e 应用（进程）类型枚举类型，对应 g_app_info_vector 数组
 */
typedef enum
{
    /*! 进程管理器 */
    APP_TYPE_PMNG = 0,
    /*! 前台应用（gui进程） */
    APP_TYPE_GUI = 1,
    /*! 后台应用（console进程） */
    APP_TYPE_CONSOLE = 2,
    /*! 蓝牙协议栈 */
    APP_TYPE_BTSTACK = 3,
} app_type_e;

/*!
 * \brief
 *  app_info_t 应用信息结构体
 */
typedef struct
{
    /*! 结构体使用标志，1表示已被使用，0表示未被使用 */
    uint8 used;
    /*! 进程ID号，见 case_type.h 中定义的列表 */
    uint8 app_id;
    /*! 应用类型，见app_type_e 定义 */
    uint8 app_type;
    /*! 进程私有消息队列ID，见mq_id_e 定义 */
    uint8 mq_id;
} app_info_t;

/*!
 * \brief
 *  engine_type_e 引擎类型枚举类型，用于应答用户请求后台引擎类型
 */
typedef enum
{
    /*! 没有引擎 */
    ENGINE_NULL = 0,
    /*! music引擎 */
    ENGINE_MUSIC = 1,
    /*! fm引擎 */
    ENGINE_RADIO = 2,
    /*! LINEIN引擎 */
    ENGINE_LINEIN = 3,
    /*! 蓝牙推歌引擎 */
    ENGINE_BTPLAY = 4,
    /*! 蓝牙打电话引擎 */
    ENGINE_BTCALL = 5,
    /*! usb音箱引擎 */
    ENGINE_USOUND = 6,
} engine_type_e;

/*!
 * \brief
 *  engine_state_e 引擎状态枚举类型
 */
typedef enum
{
    /*! 没有引擎 */
    ENGINE_STATE_NULL = 0,
    /*! 引擎正在播放 */
    ENGINE_STATE_PLAYING = 1,
    /*! 引擎处于暂停状态 */
    ENGINE_STATE_PAUSE = 2,
} engine_state_e;

/*!
 * \brief
 *  app_state_e 前台应用工作状态枚举类型
 */
typedef enum
{
    /*! 前台AP处于非播放状态 */
    APP_STATE_NO_PLAY = 0,
    /*! 前台AP处于播放状态 */
    APP_STATE_PLAYING = 1,
    /*! 前台AP处于播放状态，且希望背光常亮 */
    APP_STATE_PLAYING_ALWAYS = 2,
} app_state_e;

/*!
 * \brief
 *  card_state_e 卡状态枚举类型
 */
typedef enum
{
    /*! 没卡 */
    CARD_STATE_CARD_NULL = 0,
    /*! 有卡但没播放 */
    CARD_STATE_CARD_IN = 1,
    /*! 正在播放UHOST，暂时不使用 */
    CARD_STATE_CARD_PLAY = 2,
} card_state_e;

/*!
 * \brief
 *  uhost_state_e uhost状态枚举类型
 */
typedef enum
{
    /*! 没UHOST */
    UHOST_STATE_UHOST_NULL = 0,
    /*! 有UHOST但没播放 */
    UHOST_STATE_UHOST_IN = 1,
    /*! 正在播放UHOST，暂时不使用 */
    UHOST_STATE_UHOST_PLAY = 2,
} uhost_state_e;

/*!
 * \brief
 *  linein_state_e linein状态枚举类型
 */
typedef enum
{
    /*! 没linein */
    LINEIN_STATE_LINEIN_NULL = 0,
    /*! 有linein但没播放 */
    LINEIN_STATE_LINEIN_IN = 1,
    /*! 正在播放linein，暂时不使用 */
    LINEIN_STATE_LINEIN_PLAY = 2,
} linein_state_e;

/*!
 * \brief
 *  play_disk_type_e 播放磁盘类型枚举类型
 */
typedef enum
{
    /*! 空闲 */
    PLAY_DISK_NULL = 0,
    /*! UHOST播放 */
    PLAY_DISK_UHOST = 1,
    /*! CARD播放 */
    PLAY_DISK_CARD = 2,
    /*! SD区播放 */
    PLAY_DISK_SDFILE = 3,
    /*! LINEIN播放 */
    PLAY_DISK_LINEIN = 4,
    /*! 录音播放 */
    PLAY_RECORD_FILE = 5,
} play_disk_type_e;

/*!
 * \brief
 *  cable_state_e 线缆状态枚举类型
 */
typedef enum
{
    /*! 没插 USB 线 */
    CABLE_STATE_CABLE_NULL = 0,
    /*! 有插 USB 线 */
    CABLE_STATE_CABLE_IN = 1,
    /*! 有插 ADAPTOR 线 */
    CABLE_STATE_ADAPTOR_IN = 2,
} cable_state_e;

typedef enum
{
    /*! 没插耳机（天线） */
    HEADPHONE_STATE_NULL = 0,
    /*! 有插耳机（天线） */
    HEADPHONE_STATE_IN = 1,
} headphone_state_e;

/*!
 * \brief
 *  backlight_state_e 背光状态枚举类型
 */
typedef enum
{
    /*! 背光正常状态，亮着 */
    BACKLIGHT_STATE_NORMAL = 0,
    /*! 背光变暗状态，暗着 */
    BACKLIGHT_STATE_DARK = 1,
    /*! 背光关闭状态，关着 */
    BACKLIGHT_STATE_OFF = 2,
} backlight_state_e;

/*!
 * \brief
 *  soundout_state_e 声音输出状态枚举类型
 */
typedef enum
{
    /*! 停止输出声音，关掉pa */
    SOUND_OUT_STOP = 0,
    /*! 开始输出声音，开启pa */
    SOUND_OUT_START = 1,
    /*! 暂停输出声音，不关pa */
    SOUND_OUT_PAUSE = 2,
    /*! 恢复输出声音，不开pa */
    SOUND_OUT_RESUME = 3,
} soundout_state_e;

/*!
 * \brief
 *  apk_status_e APK状态枚举类型
 */
typedef enum
{
    /*APK尚未连接，APK退出时发送*/
    APK_UNLINK = 0,
    /*APK正在操作，5秒钟发送一次，BUSY是一种特殊的FRONTGROUND，处理完之后变为FRONTGROUND*/
    APK_BUSY = 1,
    /*APK从后台切换到前台*/
    APK_FRONTGROUND = 2,
    /*APK从前台切换到后台*/
    APK_BACKGROUND = 3,
} apk_status_e;

/*!
 * \brief
 *  app_info_state_all_t 系统当前状态结构体，所有应用程序都需要访问
 */
typedef struct
{
    /*! 当前运行的引擎类型，见 engine_type_e 定义 */
    uint8 engine_type;
    /*! 引擎当前状态，正在播放还是暂停，见 engine_state_e 定义 */
    uint8 engine_state;
    /*! 前台应用工作状态，不播放和播放，见 app_state_e 定义 */
    uint8 app_state;
    /*! 前台UI工作状态，详见 ui_status_e */
    uint8 ui_status;

    /*-- 以上4个成员BT STACK的APPLIB库需要访问，不允许修改 --*/

    /*!播tts不发送卸载命令*/
    uint8 g_no_support_tts_flag;
    uint8 g_send_exit_mmm_tts;
    uint8 power_on_flag:4;
    uint8 stub_mode_flag:4;

    /*! 音频输出状态，见 soundout_state_e */
    uint8 sound_state;
    
    uint8 fix_sample_rate_flag;
    
    /*! 蓝牙电量同步全局变量 */
    uint8 battery_report_enable;

    /*! 能量检测相关全局变量 */
    bool energy_available_flag;
    short *p_energy_value;
    
#if (SUPPORT_MULTI_FREQ_MULTI_BAND == 1)    
    short *p_energy_value_bands;
    short *p_energy_value_freqs;
#endif
    //该变量用途:使用WAVES音效时，为了让中间件知道当前是调试模式还是normal模式而加的
    uint8 stub_pc_tools_type;
    uint8 stub_phy_type;  //0:USB 1:UART

    uint8 bin_number:3;   //WAVES音效normal模式，使用第几套音效参数  
    uint8 waves_dae_para_update_flag:1;///WAVES音效normal模式下，更新音效参数组标志
    uint8 reserve:4; 
    
} app_info_state_all_t;

/*!
 * \brief
 *  app_info_state_t 系统当前状态结构体，只有前台应用需要访问
 */
typedef struct
{
    /*! 静音状态 */
    bool mute_state;
    /*! 内/外部PA */
    bool pa_mode;    
    /*! 音频输出方式 */
    uint8 aout_mode;
    /*! 背光状态，亮，暗，和关，见 backlight_state_e 定义 */
    uint8 backlight_state;
    /*! 卡状态，没卡，有卡，有卡且播放，见 card_state_e 定义 */
    uint8 card_state;
    /*! UHOST状态，没UHOST，有UHOST，有UHOST且播放，见 uhost_state_e 定义 */
    uint8 uhost_state;
    /*! linein状态，没linein，有linein，有linein且播放，见 linein_state_e 定义 */
    uint8 linein_state;
    /*! 线缆状态，没插 USB 线，有插 USB 线，见 cable_state_e 定义 */
    uint8 cable_state;
    /*! 耳机（天线）状态 */
    bool hp_state;
    /*! 充电状态，见 battery_status_e 定义 */
    uint8 charge_state;
    /*! 电池电量，取值 0 ~ 9 */
    uint8 bat_value;
    /*! 是否在闹钟响中，如果是将忽略部分系统消息 */
    bool ring_alarm;
    /*! stub工具类型 */
    uint8 stub_tools_type:7;
    uint8 stub_phy_type:1;  //0:USB 1:UART
    /*! auto test mode */
    uint8 autotest_mode;

    /*! BT patch download flag */
    bool patch_download_flag;
    /*! BT CONTROLLER POWER ON/OFF */
    bool bt_power_on;
    /*! 内部PA IC初始化是否完成，完成后才能播放声音 */
    bool inner_pa_inited;
    /*! 外部PA ON/OFF 状态 */
    bool outer_pa_on;
    /*!当前应用功能序号,即：ap_switch_info数组中的下标*/
    uint8 cur_func_index;
    /*! 当前应用功能实际序号*/
    uint8 cur_func_id;
    /*! 当前手机APK状态，见 apk_status_e 定义 */
    uint8 apk_status;
    /*! 验证APK/APP状态机 */
    uint8 verification_status;
    /*! 强制APK更新播放列表. 参考CARD_NEED_SCAN等的说明 */
    uint8 need_update_playlist;
    /*! 外部充电方案的电池开关标志 */
    uint8 extern_bat_in;
} app_info_state_t;

/*!
 * \brief
 *  app_last_state_t 系统最近状态结构体，用于自动检测状态变化并处理
 */
typedef struct
{
    /*! 最近播放磁盘类型，见 play_disk_type_e 定义 */
    uint8 last_disk;
    /*! 最近应用功能序号*/
    uint8 last_func_index;
    /*! 最近系统工作状态，见 sys_status_e 定义 */
    uint8 last_sys_status;
    /*! 最近前台UI工作状态，见 ui_status_e 定义 */
    uint8 last_ui_status;
} app_last_state_t;

/*!
 *  \brief
 *  g_app_info_vector 全局应用信息结构体数组，存放在系统全局空间
 *  NOTE!!! 必须确保 app_info_t 及 g_app_info_vector 都不发生改变
 */
extern app_info_t g_app_info_vector[MAX_APP_COUNT];

/*!
 *  \brief
 *  config_fp （全局）应用配置脚本文件句柄，存放在系统全局空间
 */
extern sd_handle config_fp;

/*!
 *  \brief
 *  g_app_info_state_all 全局系统当前状态结构体，存放在系统全局空间
 *  NOTE!!! 必须确保 app_info_state_all_t 前面部分成员不变
 */
extern app_info_state_all_t g_app_info_state_all;

/*!
 *  \brief
 *  g_app_last_state 全局系统最近状态结构体
 */
extern app_last_state_t g_app_last_state;

/*!
 *  \brief
 *  g_app_info_state 全局系统当前状态结构体
 */
extern app_info_state_t g_app_info_state;

/*!
 *  \brief
 *  g_this_app_info 当前应用信息结构体指针
 */
extern const app_info_t *g_this_app_info;

/*!
 *  \brief
 *  thread_mutex 线程互斥信号量指针
 */
extern os_event_t *thread_mutex;

extern uint32 g_customer_state;

///获取后台引擎类型
static inline engine_type_e get_engine_type(void)
{
    return g_app_info_state_all.engine_type;
}

///修改后台引擎类型
static inline void change_engine_type(engine_type_e engine_type)
{
    g_app_info_state_all.engine_type = engine_type;
}

///获取后台引擎状态
static inline engine_state_e get_engine_state(void)
{
    return g_app_info_state_all.engine_state;
}

///修改后台引擎状态
static inline void change_engine_state(engine_state_e engine_state)
{
    g_app_info_state_all.engine_state = engine_state;
}

///获取音频输出状态，见soundout_state_e
static inline soundout_state_e get_soundout_state(void)
{
    return g_app_info_state_all.sound_state;
}

///修改音频输出状态，见soundout_state_e
static inline void change_soundout_state(soundout_state_e sound_state)
{
    g_app_info_state_all.sound_state = sound_state;
}

///获取静音与否，TRUE表示静音，FALSE表示非静音
static inline bool get_mute_enable(void)
{
    return g_app_info_state.mute_state;
}

///修改静音与否，TRUE表示静音，FALSE表示非静音
static inline void change_mute_enable(bool mute)
{
    g_app_info_state.mute_state = mute;
}

///修改前台UI工作状态
static inline void change_ui_status(ui_status_e ui_status)
{
    g_app_info_state_all.ui_status = ui_status;
}

///获取背光状态，TRUE表示背光亮，FALSE表示背光灭
static inline backlight_state_e get_backlight_state(void)
{
    return g_app_info_state.backlight_state;
}

///修改背光状态
static inline void change_backlight_state(backlight_state_e state)
{
    g_app_info_state.backlight_state = state;
}

///获取前台应用 UI 状态，分正在播放UI和非播放UI
static inline app_state_e get_app_state(void)
{
    return g_app_info_state_all.app_state;
}

///修改前台应用 UI 状态，分正在播放UI和非播放UI
static inline void change_app_state(app_state_e state)
{
    g_app_info_state_all.app_state = state;
}

///获取软定时器场景标志
static inline uint8 get_app_timer_tag(void)
{
    return g_this_app_timer_tag;
}

///修改软定时器场景标志
static inline void change_app_timer_tag(uint8 tag)
{
    g_this_app_timer_tag = tag;
}

///获取系统卡状态
static inline card_state_e get_card_state(void)
{
    return g_app_info_state.card_state;
}

///修改系统卡状态
static inline void change_card_state(card_state_e card_state)
{
    g_app_info_state.card_state = card_state;
}

///获取系统UHOST状态
static inline uhost_state_e get_uhost_state(void)
{
    return g_app_info_state.uhost_state;
}

///修改系统UHOST状态
static inline void change_uhost_state(uhost_state_e uhost_state)
{
    g_app_info_state.uhost_state = uhost_state;
}

///获取系统LINEIN状态
static inline linein_state_e get_linein_state(void)
{
    return g_app_info_state.linein_state;
}

///修改系统LINEIN状态
static inline void change_linein_state(linein_state_e linein_state)
{
    g_app_info_state.linein_state = linein_state;
}

///获取系统 USB 线缆状态
static inline cable_state_e get_cable_state(void)
{
    return g_app_info_state.cable_state;
}

///修改系统 USB 线缆状态
static inline void change_cable_state(cable_state_e cable_state)
{
    g_app_info_state.cable_state = cable_state;
}

///获取耳机（天线）状态
static inline headphone_state_e get_headphone_state(void)
{
    return g_app_info_state.hp_state;
}

///修改耳机（天线）状态
static inline void change_headphone_state(headphone_state_e hp_state)
{
    g_app_info_state.hp_state = hp_state;
}

///获取充电状态
static inline battery_status_e get_charge_state(void)
{
    static uint8 lsat_state = 0xff;
    static uint8 ret_state = 0xff; 
    static uint32 last_time_ab = 0;
    uint32 os_cur_time_ab = sys_get_ab_timer();
    if(ret_state == 0xff)//初始化
    {
       ret_state = g_app_info_state.charge_state;
    }
    
    if(g_app_info_state.charge_state == lsat_state)
    {
        if((last_time_ab + (1000*5)) < os_cur_time_ab)//保持5秒
        {
            ret_state = g_app_info_state.charge_state;
        }
    }
    else
    {
        last_time_ab = os_cur_time_ab;
        lsat_state = g_app_info_state.charge_state;
    }
    return ret_state;
}

///获取电池电量
static inline uint8 get_bat_value(void)
{
    return g_app_info_state.bat_value;
}

///修改电池电量
static inline void change_bat_value(uint8 value)
{
    g_app_info_state.bat_value = value;
}

///获取最近播放磁盘类型
static inline play_disk_type_e get_play_disk_type(void)
{
    return g_app_last_state.last_disk;
}

///设置最近播放磁盘类型
static inline void set_play_disk_type(play_disk_type_e play_disk)
{
    g_app_last_state.last_disk = play_disk;
}

//获取上一个前台AP fuction index
static inline uint8 get_last_func_index(void)
{
    return g_app_last_state.last_func_index;
}

///设置上一个前台AP fuction index， 即：ap_switch_info数组中的下标
static inline void set_last_func_index(uint8 index_num)
{
    g_app_last_state.last_func_index = index_num;
}

///获取当前前台AP fuction index， 即：ap_switch_info数组中的下标
static inline uint8 get_cur_func_index(void)
{
    return g_app_info_state.cur_func_index;
}

///设置当前前台AP fuction index
static inline void set_cur_func_index(uint8 index_num)
{
    g_app_info_state.cur_func_index = index_num;
}

///获取当前前台AP funciton id
static inline uint8 get_cur_func_id(void)
{
    return g_app_info_state.cur_func_id;
}

///设置当前前台AP function id
static inline void set_cur_func_id(uint8 func_id)
{
    g_app_info_state.cur_func_id = func_id;
}

///获取当前APK状态
static inline apk_status_e get_apk_status(void)
{
    return g_app_info_state.apk_status;
}
static inline bool get_apk_available(void)
{
    return ((g_app_info_state.apk_status == APK_FRONTGROUND) || (g_app_info_state.apk_status == APK_BUSY));
}

///设置当前APK状态
static inline void set_apk_status(apk_status_e status)
{
    g_app_info_state.apk_status = status;
}

//应用（进程）管理模块接口声明（详细说明见接口定义）
extern bool applib_init(uint8 app_id, app_type_e type) __FAR__;
extern bool applib_quit(void) __FAR__;

extern engine_type_e get_engine_type_by_appid(uint8 app_id) __FAR__;
extern uint8 get_engine_appid_by_type(engine_type_e engine_type) __FAR__;

#endif
