/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：COMMON 时间闹钟服务相关的宏定义，枚举类型，结构体定义，外部接口声明等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _COMMON_TIME_ALARM_H
#define _COMMON_TIME_ALARM_H

#include "psp_includes.h"
#include "enhanced.h"
#include "common_func.h"
#include "vm_def.h"

#define RCP_ALARM_MAX                (4) //闹钟数目，最多可以配置为10个（time_alarm_var_t在512字节以内）
#define BUILDIN_RING_MAX             (1) //默认铃声数目

#define RING_FOLDER_FILENUM_OFFSET   (4) //该offset对应time_alarm_var_t中uint8 ring_folder_filenum[]
#define RCP_RING_FOLDER_MAX          (4) //自定义闹钟闹铃目录数目
#define RING_FOLDER_FILENAME_LEN     (32)//闹铃目录文件名长度
#define RING_FOLDER_FILENUM_PER_SEC  (512/RING_FOLDER_FILENAME_LEN)//一个扇区可以容纳的闹铃目录文件名个数
#define RING_FOLDER_FILENUM_MAX      (64)//闹铃目录文件总数，受VRAM大小限制

/*! 闹钟铃声类型 */
typedef enum
{
    RING_TYPE_UHOST = 0, //U盘当前列表
    RING_TYPE_SDCARD = 1, //SDCARD当前列表
    RING_TYPE_RESERVE = 2, //保留
    RING_TYPE_BUILTIN = 3, //内置铃声
    /*根目录下闹钟目录下的闹钟目录，该顺序与QUERY_ID_RING_FOLDER命令指定目录顺序一致*/
    RING_TYPE_FOLDER1 = 4, //自定义闹铃目录
    RING_TYPE_FOLDER2 = 5, //自定义闹铃目录
    RING_TYPE_FOLDER3 = 6, //自定义闹铃目录
    RING_TYPE_FOLDER4 = 7, //自定义闹铃目录
} ring_type_e;

typedef struct
{
    /*! 闹钟名字 */
    uint8 alarm_name[32];
    /*! 闹钟ID，从1~9为有效ID */
    uint8 alarm_id;
    /*! 闹钟占用标志，1表示已占用 */
    uint8 alarm_used;
    /*! 闹钟开关 */
    uint8 alarm_onoff;
    /*! 闹钟星期，bit0 ~ bit6 分别表示周日 ~ 周六 */
    uint8 alarm_day;
    /*! 闹钟时间 {uint8 hour;uint8 minute;uint8 second;} */
    time_t alarm_time;
    /*! 闹铃类型，详见 ring_type_e 定义 */
    uint8 ring_type;
    /*! 闹铃序号 */
    union
    {
        uint32 u_song_seq; /*RINT_TYPE_UHOST   = 0, U盘当前列表*/
        uint32 sd_song_seq; /*RINT_TYPE_SDCARD  = 1, SDCARD当前列表*/
        uint32 ring_seq; /*RING_TYPE_BUILTIN = 3, 内置铃声 及 RING_TYPE_FOLDERX 闹铃目录*/
    } sound;
} alarm_alarm_t;

//设置日历状态机
typedef enum
{
    /*! 设置年份 */
    calendar_year,
    /*! 设置月份 */
    calendar_month,
    /*! 设置日期 */
    calendar_day,
    /*! 设置小时 */
    calendar_hour,
    /*! 设置分钟 */
    calendar_minute,
    /*! 设置完成 */
    calendar_finish
} calendar_state_e;

//设置闹钟状态机
typedef enum
{
    /*! 设置开/关 */
    alarm_onoff,
    /*! 设置小时 */
    alarm_hour,
    /*! 设置分钟 */
    alarm_minute,
    /*! 设置完成 */
    alarm_finish,
} alarm_state_e;

typedef struct
{
    /*! 闹钟记录 */
    alarm_alarm_t alarm;
    /*! 当前闹钟时间 */
    time_t last_time;
    /*! 闹钟snooze次数 */
    uint8 snooze;
} alarm_record_t;

/*!
 * \brief
 *  time & alarm的VM全局变量.
 */
typedef struct
{
    /*! VM 变量魔数*/
    uint16 magic;
    /*! 当前闹钟记录INDEX，0xff表示无效 */
    uint8 alarm_idx;
    uint8 reserve[1];
    /*! 自定义闹钟闹铃目录文件数 */
    uint8 ring_folder_filenum[RCP_RING_FOLDER_MAX];
    /*! 闹钟记录列表 */
    alarm_record_t alarm_records[RCP_ALARM_MAX];
} time_alarm_var_t;

#define RECORD_LIST_READ_BYINDEX     0 //读取指定INDEX闹钟记录，param1 返回闹钟记录指针，param2 闹钟INDEX
#define RECORD_LIST_SEARCH_BYID      1 //读取指定ID闹钟记录，param1 返回闹钟记录指针，param2 闹钟ID
#define RECORD_LIST_READ_CUR         2 //读取当前活动ID闹钟记录，param1 返回闹钟记录指针，param2 0
#define RECORD_LIST_UPDATE_ALARM     3 //更新闹钟时间，param1 更新模式，param2 0
#define RECORD_LIST_GET_NEXT_ALARM   4 //获取最近闹钟时间，排除当前正在闹的闹钟，param1 返回闹钟时间，param2 0
#define RECORD_LIST_SET_NEXT_ALARM   5 //设置下一个闹钟，并将当前闹钟关掉，param1 先一个闹钟INDEX，param2 0
#define RECORD_LIST_ADD_BYINDEX      6 //添加或更新闹钟记录，param1 返回闹钟记录指针，param2 闹钟INDEX
#define RECORD_LIST_DELETE_BYINDEX   7 //删除闹钟记录，param1 0，param2 闹钟INDEX
#define RECORD_LIST_VALID_TOTAL      8 //获取有效闹钟数目，param1 0，param2 0
#define RECORD_LIST_SEARCH_NOUSED    9 //检索空白记录，param1 0，param2 0
#define RECORD_LIST_READ_FOLDERNUM   10//读取闹铃目录下各个子目录下的文件数目
#define RECORD_LIST_WRITE_FOLDERNUM  11//保存闹铃目录下各个子目录下的文件数目
extern int time_alarm_record_list_operate(uint8 op, void *param1, void *param2) __FAR__;

typedef enum
{
    ALARM_RING_ENTER,
    ALARM_RING_CLOSE,
    ALARM_RING_SNOOZE,
    ALARM_RING_ENTER_CLOSE_AUTO, //在不合适响闹钟的场景下使用，会将闹钟关闭然后马上退出
} alarm_ring_mode_e;

extern app_result_e tm_alarm_alarm_ring_deal(alarm_ring_mode_e alarm_ring_mode) __FAR__;

typedef struct
{
    date_t date;
    time_t time;
    bool twinkling_flag;
    int8 twinkling_timer_id;
} clock_show_var_t;

typedef struct
{
    date_t date;
    time_t time;
    uint16 *setting_num16;
    uint8 *setting_num8;
    uint8 setting_digits;
    uint8 now_digits;
    bool digit_flag;
    bool write_date_falg;
    bool write_time_falg;
    calendar_state_e calendar_state;
    bool twinkling_flag;
    int8 twinkling_timer_id;
} set_calendar_var_t;

extern app_result_e set_calendar_switch_deal(set_calendar_var_t *set_calendar_var) __FAR__;
extern app_result_e set_calendar_sub_deal(set_calendar_var_t *set_calendar_var) __FAR__;
extern app_result_e set_calendar_add_deal(set_calendar_var_t *set_calendar_var) __FAR__;
extern void sys_write_rtc(set_calendar_var_t *set_calendar_var) __FAR__;

typedef struct
{
    time_t time;
    uint16 *setting_num16;
    uint8 *setting_num8;
    uint8 setting_digits;
    uint8 now_digits;
    bool digit_flag;
    alarm_state_e alarm_state;
    bool twinkling_flag;
    int8 twinkling_timer_id;
    alarm_record_t cur_alarm_record;
    uint8 alarm_onoff_bk;
} set_alarm_var_t;

extern app_result_e set_alarm_switch_deal(set_alarm_var_t *set_alarm_var) __FAR__;
extern app_result_e set_alarm_sub_deal(set_alarm_var_t *set_alarm_var) __FAR__;
extern app_result_e set_alarm_add_deal(set_alarm_var_t *set_alarm_var) __FAR__;

typedef struct
{
    /*! 闹铃类型，详见 ring_type_e 定义 */
    uint8 ring_type;
    uint8 reserve[2];
    /*! 文件名称编码类型：0表示ANSI，1表示UNICODE little endian，2表示UNICODE bit endian，3表示UTF-8 */
    uint8 code_type;
    /*! 铃声音乐文件名称 */
    uint8 song_name[60];
} ring_song_t;

/*闹铃目录信息结构体*/
typedef struct
{
    /*! 闹铃目录（类型）ID */
    uint8 ring_type;
    /*! 闹铃目录名字编码类型：0表示ANSI，1表示UNICODE little endian，2表示UNICODE bit endian，3表示UTF-8 */
    uint8 code_type;
    /*! 特殊目录名字 */
    uint8 folder_name[30];
} ring_folder_infor_t;

/*闹钟参数*/
typedef struct
{
    /*! snooze时间，1-30分钟，0表示不开启snooze功能 */
    uint8 snooze_time;
    /*! snooze次数，1-10次，0表示无限次 */
    uint8 snooze_max_count;
    /*! 超时时间，1-30分钟，0表示不超时一直闹*/
    uint8 overtime;
    uint8 reserve;
} alarm_param_t;

#define UPDATE_ALARM_ADD        0 //添加或修改闹钟记录
#define UPDATE_ALARM_DEL        1 //删除闹钟记录
#define UPDATE_ALARM_SNOOZE     2 //延时
#define UPDATE_ALARM_CLOSE      3 //自动关闭
#define UPDATE_ALARM_GET_NEXT   4 //获取最近闹钟时间，排除掉当前正在闹的闹钟
#define UPDATE_ALARM_JUST_CLOSE 5 //仅仅关闭当前闹钟
#define UPDATE_ALARM_CLOSE_SAME 6 //关闭所有相同时间的闹钟
extern uint8 time_alarm_update_alarm(uint8 mode) __FAR__;

extern clock_show_var_t *g_p_clock_show;
extern void tm_alarm_create_clock_view(void) __FAR__;

extern set_calendar_var_t *g_p_set_calendar;
extern void tm_alarm_create_set_calendar_view(void) __FAR__;

extern set_alarm_var_t *g_p_set_alarm;
extern void tm_alarm_create_set_alarm_view(void) __FAR__;

#endif
