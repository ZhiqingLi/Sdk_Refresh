/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：TTS 功能相关的宏定义，枚举类型，结构体定义，外部接口声明等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _common_tts_H_
#define _common_tts_H_

#include "psp_includes.h"
#include "case_independent.h"
#include "applib.h"
#include "setting_common.h"
#include "tts_interface.h"
#include "common_rcp.h"
#include "config_id.h"
#include "common_btmanager.h"
#include "app_view.h"
#include "common_tts_id.h"

#define TTS_LANGUAGE_TOTAL     2   //支持TTS语言类型总数

#define TTS_MODE_TYPE_MASK          (3 << 0)  //TTS类型选项
#define TTS_MODE_ONLYONE            (0 << 0)  //仅1个词条
#define TTS_MODE_SECLIST            (1 << 0)  //段序列，以0xff为结束标志
#define TTS_MODE_STRING             (2 << 0)  //字符串

#define TTS_MODE_MIN_VOLUME_MASK    (1 << 5)  //是否限制最低音量，即至少以某个音量来播放TTS
#define TTS_MODE_MIN_VOLUME_YES     (0 << 5)  //限制最低音量
#define TTS_MODE_MIN_VOLUME_NO      (1 << 5)  //不限制最低音量，如果当前音量为0，那么将不会播放TTS

#define TTS_MODE_NORECV_ALL_EV_MASK (1 << 6)  //不接收和处理所有事件选项，比如关机时TTS
#define TTS_MODE_NORECV_ALL_EV_NO   (0 << 6)  //接收和处理所有事件
#define TTS_MODE_NORECV_ALL_EV_YES  (1 << 6)  //不接收和处理所有事件，纯粹TTS

#define TTS_MODE_PB_OPTION_MASK     (1 << 7)  //电话簿播报选项
#define TTS_MODE_PB_NOPLAY          (0 << 7)  //非电话簿播报
#define TTS_MODE_PB_PLAY            (1 << 7)  //电话簿播报

#define TTS_MODE_RCP_OPTION_MASK    (1 << 8)  //TTS中RCP选项
#define TTS_MODE_RCP_BREAK          (0 << 8)  //终止TTS且响应，有些命令会在TTS中并行响应
#define TTS_MODE_RCP_NORECV         (1 << 8)  //不读取RCP命令

#define TTS_MODE_SYS_OPTION_MASK    (1 << 9)  //TTS中系统消息选项
#define TTS_MODE_SYS_BREAK          (0 << 9)  //终止TTS且响应
#define TTS_MODE_SYS_NORECV         (1 << 9)  //不读取系统消息

#define TTS_MODE_KEY_OPTION_MASK    (3 << 10) //TTS中按键选项
#define TTS_MODE_KEY_BREAK_REDEAL   (0 << 10) //终止TTS且响应
#define TTS_MODE_KEY_BREAK_FILTER   (1 << 10) //终止TTS且过滤
#define TTS_MODE_KEY_FILTER         (2 << 10) //直接过滤
#define TTS_MODE_KEY_NORECV         (3 << 10) //不读取按键消息

#define TTS_MODE_BLOCK_OPTION_MASK  (1 << 12) //阻塞选项
#define TTS_MODE_BLOCK              (0 << 12) //阻塞模式    以上选项有效
#define TTS_MODE_NOBLOCK            (1 << 12) //不阻塞模式  以上选项无效，由用户自己保证不会有解码冲突问题；
//不阻塞模式必须慎重使用，建议仅在开机时使用，让开机TTS与开机设置并行走

#define TTS_MODE_FIFO_OPTION_MASK   (1 << 15) //先放到FIFO后再播放，简化代码逻辑，并且支持多个TTS播报
#define TTS_MODE_NOFIFO             (0 << 15) //不使用FIFO，立即播放
#define TTS_MODE_USEFIFO            (1 << 15) //使用FIFO延缓播放

//状态语音播报
#define TTS_FIFO_DEPTH              (2)   //一些时序无关的提示TTS缓冲深度
#define TTS_SECTIONS_MAX            (16)  //一次语音播报最大片段数，最后1个为0xff表示结束，
                                          //比如数字1234 “一千二百三十四”，共7段

/*! TTS相关宏定义 */
#define IGNORE_PLAY_TTS             (1 << 7)
#define FORCE_STOP_TTS              (1 << 6)

/*! TTS相关宏定义 */
#define TTS_MIN_VOL                  8     //TTS播放时的最小音量

typedef enum
{
    TTS_CTRL_IGNORE = 0,
    TTS_CTRL_CANCEL_IGNORE = 1,
    TTS_CTRL_FORCE_STOP = 2,
    TTS_CTRL_CANCEL_FORCE_STOP = 3,
} tts_control_opr_e;

typedef enum
{
    TTS_STATUS_IDLE = 0, //表示空闲状态
    TTS_STATUS_READY = 1, //表示需要TTS播报
    TTS_STATUS_PLAYING = 2, //表示已初始化好，正在播放
    TTS_STATUS_PLAYOVER = 3, //表示已播放完成
} tts_status_e;

typedef struct
{
    uint16 cur_mode;
    uint8 count;
    /*! 播放TTS控制：[bit7] 忽略播放TTS；[bit6] 强制终止播放TTS */
    uint8 option;
    /*!TTS播报状态机*/
    tts_status_e status;
    uint8 section_ids[TTS_SECTIONS_MAX];
    uint8 tts_fifo[TTS_FIFO_DEPTH][TTS_SECTIONS_MAX];
    uint16 tts_mode[TTS_FIFO_DEPTH];
    void* tts_str;
} tts_play_info_t;

extern tts_play_info_t g_tts_play_info;

typedef enum
{
    TTS_PLAY_RET_NORMAL,
    TTS_PLAY_RET_BY_KEY,
    TTS_PLAY_RET_BY_SYS,
    TTS_PLAY_RET_BY_BTEV,
} tts_play_ret_e;

extern tts_play_ret_e com_tts_state_play(uint16 tts_mode, void *tts_info) __FAR__;
extern tts_play_ret_e com_tts_state_play_wait(void) __FAR__;
extern tts_play_ret_e com_tts_state_play_deal(void) __FAR__;
extern void com_tts_state_play_control(tts_control_opr_e opr) __FAR__;

#endif
