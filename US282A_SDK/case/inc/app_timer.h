/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：软定时器相关宏定义，枚举类型，结构体定义，外部接口声明等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _app_timer_H_
#define _app_timer_H_

#include "psp_includes.h"
#include "case_type.h"

/*! 公共定时器数目，用于系统功能定时，USB检测切换 */
#define COMMON_TIMER_COUNT      4

/*! 
 * \brief
 *  应用级定时器所属场景 tag id，用于把定时器捆绑于某个场景，更好地管理应用下所有定时器。
 */
/*! AP 主场景 tag */
#define APP_TIMER_TAG_MAIN          0x00
/*! 系统场景 tag，虚拟场景，任何场景下都会执行 */
#define APP_TIMER_TAG_SYS           0xff

/*! 
 * \brief
 *  timer_type_e 软定时器类型枚举结构
 */
typedef enum
{
    /*! 普通模式的定时器，自动周期发送 */
    TIMER_TYPE_NORMAL,
    /*! 单发定时器，只定时一次后进入TIMER_STATE_STOPED 状态 */
    TIMER_TYPE_SINGLE_SHOT,
} timer_type_e;

/*! 
 * \brief
 *  timer_state_e 软定时器状态枚举结构
 */
typedef enum
{
    /*! 标志可用状态，可用来创建定时器 */
    TIMER_STATE_NOUSED,
    /*! 正常计时状态 */
    TIMER_STATE_RUNNING,
    /*! 停止计时状态，只能通过restart重新开始计时 */
    TIMER_STATE_STOPED,
} timer_state_e;

/*! 
 * \brief
 *  timer_attrb_e 软定时器属性枚举结构，用来对软定时器资源进行初始分配
 */
typedef enum
{
    /*! UI 类软定时器，在背光熄灭后不再执行 ISR */
    APP_TIMER_ATTRB_UI,
    /*! control 类软定时器，任何时候都会执行 ISR */
    APP_TIMER_ATTRB_CONTROL,
    /*! UI 类软定时器，在背光熄灭后不再执行 ISR */
    APP_TIMER_ATTRB_COM_UI,
    /*! control 类软定时器，任何时候都会执行 ISR */
    APP_TIMER_ATTRB_COM_CONTROL,
} timer_attrb_e;

/*! 
 * \brief
 *  timer_proc 软定时器服务例程 
 */
typedef void (*timer_proc)(void);

/*! 
 * \brief
 *  app_timer_t 软定时器管理结构体
 */
typedef struct
{
    /*! 软定时器状态，见timer_state_e 定义 */
    uint8 state :2;
    /*! 软定时器特性，见timer_attrb_e 定义 */
    uint8 attrb :2;
    /*! 软定时器类型，见timer_type_e 定义 */
    uint8 type :1;
    /*! twinkle 标志，twinkle类定时器type为TIMER_TYPE_NORMAL */
    uint8 twinkle :1;
    /*! 软定时器场景标志 tag */
    /*! twinkle 状态，1表示亮周期，0表示灭周期 */
    uint8 on_off :1;
    uint8 tag;
    /*! 只对twinkle类定时器有效：循环周期数目，0表示循环停止，-1表示一直循环直到重新设置 */
    uint8 count;
    uint8 reserve;
    /*! 软定时器 定时周期，单位为 1ms */
    /*! 亮时间，闪烁时有效，单位为1ms */
    uint16 timeout;
    /*! 灭时间，闪烁时有效，单位为1ms */
    uint16 timeout_off;
    /*! 软定时器 下一次超时绝对时间点，单位为 1ms */
    uint32 timeout_expires;
    /*! 软定时器 服务例程 */
    timer_proc func_proc;
} app_timer_t;

typedef struct
{
    /*! 循环周期数目，0表示循环停止，-1表示一直循环直到重新设置 */
    uint8 count;
    /*! 初始亮灭状态 */
    uint8 on_off;
    /*! 亮时间，闪烁时有效，单位为1ms */
    uint16 timeout_on;
    /*! 灭时间，闪烁时有效，单位为1ms */
    uint16 timeout_off;
    /*! 软定时器 服务例程 */
    timer_proc func_proc;
} twinkle_app_timer_param_t;

/*! 
 *  \brief
 *  g_app_timer_vector 应用级定时器数组指针，指向应用空间的定时器数组
 */
extern app_timer_t *g_app_timer_vector;

/*! 
 *  \brief
 *  g_app_timer_count 应用级软定时器数目
 */
extern uint8 g_app_timer_count;
extern uint8 g_app_timer_id;

/*! 
 *  \brief
 *  g_this_app_timer_tag 软定时器场景标志，AP场景为0
 */
extern uint8 g_this_app_timer_tag;

#define COM_APP_TIMER_MAX  (8)
extern app_timer_t g_com_app_timer_vct[COM_APP_TIMER_MAX];


//软定时器管理模块接口声明
extern int8 set_app_timer(uint32 attrb_tagh, uint16 timeout, timer_proc func_proc) __FAR__;
extern int8 set_single_shot_app_timer(uint32 attrb_tagh, uint16 timeout, timer_proc func_proc) __FAR__;
extern int8 set_single_shot_app_timer_btmamager(uint32 attrb_tagh, uint16 timeout, timer_proc func_proc) __FAR__;
extern int8 set_twinkle_app_timer(uint32 attrb_tagh, twinkle_app_timer_param_t *twinkle_param) __FAR__;
extern bool modify_app_timer(int8 timer_id, uint16 timeout) __FAR__;
extern bool stop_app_timer(int8 timer_id) __FAR__;
extern bool restart_app_timer(int8 timer_id) __FAR__;
extern bool kill_app_timer(int8 timer_id) __FAR__;
extern bool init_app_timers(app_timer_t *timers, uint8 count) __FAR__;
extern void standby_restart_all_app_timer(app_timer_t *timers, uint8 count) __FAR__;
extern void handle_timers(app_timer_t *timers, uint8 count) __FAR__;
extern bool kill_app_timer_safe(int8 timer_id, timer_proc time_handle)__FAR__;

#endif
