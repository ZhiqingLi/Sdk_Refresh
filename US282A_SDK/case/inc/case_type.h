/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：CASE 应用和功能级别的宏定义，枚举类型，结构体定义等。
 * 作者：cailizhen
 ********************************************************************************/

#include "psp_includes.h"

#ifndef _case_type_H_
#define _case_type_H_

#define FRAME_SIZE 1152

/*!
 * \brief
 *  方案应用ID，最多支持32个应用，ID对应 app_name_ram 中的名字。
 */
#define APP_ID_MUSIC        0x00
#define APP_ID_LINEIN       0x01
#define APP_ID_BTPLAY       0x02
#define APP_ID_BTCALL       0x03
#define APP_ID_RADIO        0x04
#define APP_ID_USOUND       0x05
#define APP_ID_UDISK        0x06
#define APP_ID_RECORD       0x07
#define APP_ID_USER1        0x08
#define APP_ID_USER2        0x09
#define APP_ID_CONFIG       0x0a
#define APP_ID_MANAGER      0x0b
#define APP_ID_UPGRADE      0x0c
#define APP_ID_BTSTACK      0x0d
#define APP_ID_MENGINE      0x0e
#define APP_ID_LINEIN_EG    0x0f
#define APP_ID_BTPLAYEG     0x10
#define APP_ID_BTCALLEG     0x11
#define APP_ID_FMENGINE     0x12
#define APP_ID_UENGINE      0x13
#define APP_ID_OTA_TEST     0x14
// #define APP_ID_USER4        0x15
#define APP_ID_OTA_UPGRADE  0x15
#define APP_ID_ALARM        0x16
#define APP_ID_BTSTACK_OTA  0x17
#define APP_ID_MAX          0x18


#define APP_ID_THEENGINE    0x80 //泛指引擎
#define APP_ID_BT_MANAGER   0x81 //BT MANAGER独占一个事件队列

//app func id for config 最多8个bit 支持64个功能
#define APP_FUNC_BTPLAY      0

#define APP_FUNC_PLAYCARD    1

#define APP_FUNC_PLAYUHOST   2

#define APP_FUNC_PLAYLINEIN  3

#define APP_FUNC_RADIO       4

#define APP_FUNC_USOUND      5

#define APP_FUNC_CONFIG      6

#define APP_FUNC_UDISK       7

#define APP_FUNC_CLOCK       8

//播放udisk录音文件 APP_FUNC_PLAYURECORD
#define APP_FUNC_PLAYURECORD 9
//播放卡录音文件
#define APP_FUNC_PLAYCRECORD 10

#define APP_FUNC_LINREC_TOC  11

#define APP_FUNC_FMREC_TOC   12

#define APP_FUNC_MICREC_TOC  13

#define APP_FUNC_BTCALL      14

#define APP_FUNC_UPGRADE     15

#define APP_FUNC_CHARGING    16

//17 no used
//18 no used

#define APP_FUNC_LINREC_TOU  19

#define APP_FUNC_FMREC_TOU   20

#define APP_FUNC_MICREC_TOU  21

#define APP_FUNC_IDLE_MODE   30 //外部充电方案的空闲模式

#define APP_FUNC_OTA_TEST    31

//无效的APP_FUNC ID
#define APP_FUNC_INVALID    (63)

/*!
 * \brief
 *  应用后缀名 bitmap，bit<31> = 0h。
 * \note
 * \li  与后缀名数组对应关系：bit[30] 对应后缀名数组 rom_ext_name 首元素，即 MP3。
 */
#define MUSIC_BITMAP        0x76000c00

//#define MUSIC_BITMAP        0x30000000 //only wav & wma

#define AUDIBLE_BITMAP      0x00003000
#define VIDEO_BITMAP        0x01004000
#define PICTURE_BITMAP      0x00780000
#define PICTURE_BITMAP1     0x00600000//后台音乐播放时的图片文件名过滤
#define TEXT_BITMAP         0x00800000
#define RECORD_BITMAP       0x50000000
#define ALLALL_BITMAP       0x00000004
#define ALLFILE_BITMAP      0x00000002
#define ALLDIR_BITMAP       0x00000001

#define RECORD_WAV_BITMAP   0x10000000
#define RECORD_MP3_BITMAP   0x40000000
//附上 rom_ext_name 列表，放在 fs_common_func.c 中。
//const char rom_ext_name[31][4] =
//{
//    "MP3",//对应 bit[30]
//    "WMA",
//    "WAV",
//    "AAC",
//    "FLAC",
//    "APE",
//    "AVI",//对应 bit[24]
//    "TXT",
//    "JPG",
//    "JPEG",
//    "BMP",
//    "GIF",
//    "LRC",
//    "ASF",
//    "OGG",//对应 bit[16]
//    "WMV",
//    "AMV",
//    "AAX",
//    "AA",
//    "MP1",
//    "MP2",
//    "M4A",
//    "** ",//对应 bit[8]
//    "** ",
//    "** ",
//    "** ",
//    "** ",
//    "** ",
//    "**",
//    "*f",
//    "*d",//对应 bit[0]
//};

/*! 无符号32位整型数 -1 值宏定义 */
#define V_U32_INVALID   0xffffffff
/*! 无符号16位整型数 -1 值宏定义 */
#define V_U16_INVALID   0xffff
/*! 无符号8 位整型数 -1 值宏定义 */
#define V_U8_INVALID    0xff

/*! 文件系统扇区大小 */
#define FILE_SECTOR     512

//数字输入最大个数限制
#define MAX_DIGITAL_COUNT 4

#define ASQT_STUB_TEST_EN  0/*ASQT调试宏,不能跟ASET同时打开*/
#define ASET_STUB_TEST_EN  0 /*ASET调试宏不能跟ASQT同时打开*/

#define TK_STUB_TEST_EN  0   /*tk调试宏，要把后台BT关掉*/

/*!
 * \brief
 *  app_param_e 进入应用传递参数类型枚举类型
 */
//有时候会跟PARAM_S3_EXIT组成一个字节，
//所以要考虑组合后，还能不能知道本类型变量的值是多少的问题。
//所以本类型变量的值是多少，是有讲究的，不能无限制增加。
typedef enum
{
    /*! 空参数 */
    PARAM_NULL = 0x0,
    /*! 从 udisk ap 进入 */
    PARAM_FROM_UDISK,
    /*! 从alarm ap进入卡录音*/
    PARAM_FROM_ALARM,
    /*! 从 radio ap 进入卡录音 */
    PARAM_FROM_RADIO_TOC,
    /*! 从 radio ap 进入u盘录音 */
    PARAM_FROM_RADIO_TOU,
    /*! 从linein进入卡录音*/
    PARAM_FROM_LINEIN_TOC,
    /*! 从linein进入u盘录音*/
    PARAM_FROM_LINEIN_TOU,
    /*进入mic录音进入卡录音*/
    PARAM_FROM_MIC_TOC,
    /*进入mic录音进入u盘录音*/
    PARAM_FROM_MIC_TOU,
    /*!播放uhost*/
    PARAM_MUSIC_UHOST,
    /*!播放card*/
    PARAM_MUSIC_SDCARD,
    /*!进入cdisk录音文件播放*/
    PARAM_RECORD_CPLAY,
    /*!进入udisk录音文件播放*/
    PARAM_RECORD_UPLAY,

} app_param_e;

#define PARAM_S3_EXIT  0x80

/*!
 * \brief
 *  app_result_e 应用层接口返回结果枚举类型
 */
typedef enum
{
    /*! 没有任何需要通知调用者的情况返回，调用者不需要做任何特殊处理 */
    RESULT_NULL = 0x00,
    RESULT_IGNORE = 0x01,
    /*! 没有任何事件，用于 com_view_loop 返回，表示没有按键消息，系统消息，BT事件等 */
    RESULT_NONE_EVENT = 0x02,
    
    RESULT_RCP_LC_WAIT = 0x03,

    /*! 无关紧要的结果，一般不会引起场景和应用切换 */
    RESULT_COMMON_RESERVE = 0x20,

    /*! 其他错误返回 */
    RESULT_ERROR = 0x80,
    /*! 退出当前场景，回到场景分发循环 */
    RESULT_BACK_SCENE_DISPATCH = 0x81,
    /*! 场景调度退出 */
    RESULT_SCENE_EXIT = 0x82,
    /*! 按键事件在COMMON重新处理 */
    RESULT_KEY_EVENT_REDEAL_BY_COMMON = 0x83,
    /*! 系统消息在COMMON重新处理 */
    RESULT_SYS_MSG_REDEAL_BY_COMMON = 0x84,
    /*! 强制终止TTS */
    RESULT_FORCE_TTS_STOP = 0x85,
    
    /*! OTA升级 */
    RESULT_ENTER_OTA_UPGRADE = 0xa1,
    
    /*! 退出自动化测试 */
    RESULT_TESTAP_QUIT = 0xff, //NOTE!!! 不可更改

    /*! 应用无条件退出 */
    RESULT_APP_QUIT = 0x100, //NOTE!!! 不可更改

    /*! 音乐文件播放 */
    RESULT_MUSIC_PLAY = 0x101,
    /*! 音乐文件播放卡文件 */
    RESULT_MUSIC_CPLAY = 0x102,
    /*! 音乐文件播放u盘文件*/
    RESULT_MUSIC_UPLAY = 0x103,
    /*! 进入line in */
    RESULT_ENTER_LINEIN = 0x104,
    /*!RADIO进入消息*/
    RESULT_RADIO = 0x105,
    /*! 进入蓝牙音箱 */
    RESULT_BLUETEETH_SOUND = 0x106,
    /*! 进入蓝牙免提*/
    RESULT_BLUETEETH_CALL = 0x107,
    /*! 后台方式进入蓝牙免提 */
    RESULT_BLUETEETH_CALL_BACKGROUND = 0x108,
    /*! 进入usb音箱 */
    RESULT_USB_SOUND = 0x109,
    /*! 关机 */
    RESULT_POWER_OFF = 0x10a,
    /*! 电量低 */
    RESULT_LOW_POWER = 0x10b,
    /*! 进入大容量磁盘 */
    RESULT_USB_TRANS = 0x10c,
    /*! 进入录音文件播放 */
    RESULT_RECORD_CPLAY = 0x10d,
    /*! 进入录音文件播放 */
    RESULT_RECORD_UPLAY = 0x10e,
    /*!录音进入消息录制到卡盘*/
    RESULT_RECORD_LINEIN_TOC = 0x10f,
    /*!录音进入消息录制到卡盘*/
    RESULT_RECORD_FM_TOC = 0x110,
    /*!录音进入消息录制到卡盘*/
    RESULT_RECORD_MIC_TOC = 0x111,
    /*!录音进入消息录制到U盘*/
    RESULT_RECORD_LINEIN_TOU = 0x112,
    /*!录音进入消息录制到U盘*/
    RESULT_RECORD_FM_TOU = 0x113,
    /*!录音进入消息录制到U盘*/
    RESULT_RECORD_MIC_TOU = 0x114,
    /*!上一次播放应用 */
    RESULT_LASTPLAY = 0x115,
    /*!下一个功能选项*/
    RESULT_NEXT_FUNCTION = 0x116,
    /*! 进入upgrade*/
    RESULT_ENTER_UPGRADE = 0x117,
    /*! 回到默认应用 */
    RESULT_MAIN_APP = 0x118,
    /*! 闹钟响 */
    RESULT_ENTER_ALARM = 0x119,
    /*! 闹钟响SNOOZE停止 */
    RESULT_ALARM_SNOOZE = 0x11a,
    /*! 闹钟响关闭停止 */
    RESULT_ALARM_CLOSE = 0x11b,
    /*! 闹钟闹铃目录扫描 */
    RESULT_ALARM_RING_FOLDER_SCAN = 0x11c,
    /*! FM自动搜台 */
    RESULT_AUTO_SEARCH = 0x11d,
    /*! FM递增手动搜台 */
    RESULT_MANUAL_SEARCH_UP = 0x11e,
    /*! FM递减手动搜台 */
    RESULT_MANUAL_SEARCH_DOWN = 0x11f,
    /*! FM停止搜台 */
    RESULT_STOP_SEARCH = 0x120,
    /*! 进入CONFIG 充电界面 */
    RESULT_CONFIG_CHARGE = 0x121,
    /*! 退出CONFIG 充电界面 */
    RESULT_CONFIG_CHARGE_QUIT = 0x122,
    /*! ASQT免提调试 */
    RESULT_ENTER_ASQT_HF_TEST = 0x123,
    /*! USER1应用 */
    RESULT_ENTER_USER1 = 0x124,
    /*! 进入超低功耗模式S3 */
    RESULT_SYSTEM_ENTER_S3 = 0x125,
    /*! 退出超低功耗模式S3，仅在CONFIG使用 */
    RESULT_SYSTEM_EXIT_S3 = 0x126,
    /*! 进入stub调试模式 */
    RESULT_ENTER_STUB_DEBUG = 0x127,
    /*! ESD 重启 */
    RESULT_ESD_RESTART = 0x128,
    /*! 空闲模式，仅外部充电方案有用 */
    RESULT_IDLE_MODE,
    /*!进入OTA歌曲测试 */
    RESULT_ENTER_OTA_SONG_TEST,
    
    RESULT_PLAY_SDD_FID16,//播放SDD扩展区16的文件OTA测试使用

    RESULT_PLAY_SDD_FID17,//播放SDD扩展区17的文件

    RESULT_PLAY_SDD_FID18,

    RESULT_PLAY_SDD_FID19,
        
    RESULT_PLAY_SDD_FID20,

    RESULT_PLAY_SDD_FID21,
    
    RESULT_PLAY_SDD_FID22,

    RESULT_PLAY_SDD_FID23,
    
    RESULT_PLAY_SDD_FID24,

    RESULT_PLAY_SDD_FID25,

    
    /*! APK/APP 按钮返回值定义 */
    RESULT_DIALOG_OK = 0x200,
    RESULT_DIALOG_CANCEL = 0x201,
    RESULT_DIALOG_ABORT = 0x202,
    RESULT_DIALOG_RETRY = 0x203,
    RESULT_DIALOG_IGNORE = 0x204,
    RESULT_DIALOG_YES = 0x205,
    RESULT_DIALOG_NO = 0x206,
    RESULT_DIALOG_CANCEL_AUTO = 0x207, //自动取消
    RESULT_DIALOG_USB_SOUND = 0x208, //进入USB音箱
    RESULT_DIALOG_ONLY_CHARGE = 0x209, //仅充电

    RESULT_HID_SEND_KEY_EVENT = 0x20a,//拍照键值发送事件

} app_result_e;

typedef enum
{
    VDD_MODE_LDO = 0, //电源配置为LDO模式
    VDD_MODE_DCDC = 1, //电源配置为DCDC
    VDD_MODE_UVLO = 2, //DCDC/LDO自动切换
    VDD_MODE_DC2VOV = 3
//过压自动切换
} vdd_mode_e;

/*!
 * \brief
 *  sys_counter_t 系统功能计时器数据结构，各计时器均以0.5 s为单位
 */
typedef struct
{
    /*! 省电关机计时器 */
    uint16 poweroff_counter;
    /*! 背光变暗或关闭计时器 */
    uint16 light_counter;
    /*! 充电满每分钟播报一次充电已满 */
    uint16 charge_full_cnt;
    /*! 每3s允许UI电量改变一次 */
    uint16 bat_change_cnt;
    /*! 电量低每分钟播报一次电量不足请充电 */
    uint16 bat_low_cnt;
    /*! 等待APK/APP验证计时器 */
    uint8 apk_app_counter;
    /*! 判断APK avialable与否计时器 */
    uint8 apk_counter;
    /*! 前台UI空闲计时器 */
    uint8 ui_idle_counter;
} sys_counter_t;

typedef struct
{
    uint8 ap_id;
    uint8 param;
    bool engine_do;
    app_result_e result_val;
    uint8 app_func_id;
} ap_switch_info_t;


typedef struct
{
    uint8 freq_mode; // 0 = 32.768k, 1 = 13M
    uint8 gpio_level; //输出13M驱动能力0~7
    uint16 rev;
} FM_init_parm_t;

/*!
 *  \brief
 *  g_sys_counter （全局）系统功能计时器，存放在系统全局空间
 */
extern sys_counter_t g_sys_counter;

/************************state for customer**************************/
/*预留，变量g_customer_state的每个bit的定义，针对不同客户修改*/

#define CUSTOMER_ESD_WAKEUP                    (1 << 6)//esd唤醒

/************************end**************************/
/*****************************************************

Note: 以下寄存器需要在使用之后刷密码!!!
PMU:
Offset	Register Name	电压域
0x4c	WKEN_CTL	RTCVDD需要刷密码

GPIO
Offset	Register Name	电压域
0x3C	GPIOB0_CTL	RTCVDD需要刷密码

CMU_ANALOG
Offset	Register Name	电压域
0x00	HOSC_CTL	RTCVDD需要刷密码
0x04	HCL_CTL	RTCVDD需要刷密码

RTC
Offset	Register Name	电压域
0x00	RTC_CTL	RTCVDD需要刷密码
0x08	RTC_DHMSALM	RTCVDD需要刷密码
0x0C	RTC_DHMS	RTCVDD需要刷密码
0x10	RTC_YMD	RTCVDD需要刷密码
0x18	RTC_ACCESS	RTCVDD需要刷密码

Offset	Register Name	电压域
0x30	RTC_BAK0	RTCVDD需要刷密码
0x34	RTC_BAK1	RTCVDD需要刷密码
0x38	RTC_BAK2	RTCVDD需要刷密码
0x3C	RTC_BAK3	RTCVDD需要刷密码

BT
Offset	Register Name	电压域
0x00	BT_CFG	RTCVDD需要刷密码
*************************************************/

typedef enum
{
    APP_ATT_FUNC_ID_BTPLAY,
    APP_ATT_FUNC_ID_BTCALL,
    APP_ATT_FUNC_ID_CARDPLAY,
    APP_ATT_FUNC_ID_LINEIN,
    APP_ATT_FUNC_ID_RADIO
}att_func_id_e;

#endif
