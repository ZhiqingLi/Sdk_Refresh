/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：前台应用视图相关宏定义，枚举类型，结构体定义，外部接口声明等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _COMMON_VIEW_H
#define _COMMON_VIEW_H

#include "psp_includes.h"
#include "gui_msg.h"
#include "app_msg.h"
#include "case_type.h"

//APP TIMER TAG ID 也是使用同样的数值
#define APP_VIEW_ID_MAIN         0x00
#define COM_VIEW_ID_DISP_CLOCK   0x80
#define COM_VIEW_ID_SET_CALENDAR 0x81
#define COM_VIEW_ID_SET_ALARM    0x82
#define COM_VIEW_ID_SET_VOLUME   0x83
#define COM_VIEW_ID_SET_EQ       0x84

typedef enum
{
    VIEW_EVENT_KEY,
    VIEW_EVENT_SYS_EVENT,
    VIEW_EVENT_STATUS_CHANGE,
} view_event_e;

typedef struct
{
    view_event_e ev_type;
    /* 根据 event_type 而定
     * VIEW_EVENT_KEY -> key_event_t
     */
    void *ev_data;
} view_event_t;

typedef app_result_e (*key_event_cb)(void);
typedef app_result_e (*sys_event_cb)(void *ev_param);

typedef struct
{
    key_event_t ke;
    key_event_cb cb;
} key_event_map_t;

typedef struct
{
    sys_event_t se;
    sys_event_cb cb;
} sys_event_map_t;

typedef enum
{
    VIEW_UPDATE_CREATE, //com_view_add 时自动调用
    VIEW_UPDATE_DESTROY, //com_view_remove 时自动调用
    VIEW_UPDATE_REDRAW,
    VIEW_UPDATE_STATUS,
} view_update_e;

typedef void (*view_update_cb)(view_update_e mode);

typedef enum
{
    /*! 主视图 */
    VIEW_TYPE_MAIN,
    /*! 稳态子视图，可以嵌套其他子视图；有自己的按键事件处理；也可以延时一段时间就退出 */
    VIEW_TYPE_SUB,
    /*! 提示子视图，仅仅提示并延时一段时间就退出，不可以嵌套其他子视图，即提示子视图只能同时存在一个 */
    VIEW_TYPE_MSG,
} view_type_e;

#define UNKNOWN_KEY_IGNORE       0
#define UNKNOWN_KEY_QUIT_FILTER  1
#define UNKNOWN_KEY_QUIT_REDEAL  2
#define UNKNOWN_KEY_QUIT_IFMAIN  3

typedef struct
{
    view_type_e type; //视图类型
    uint8 unknown_key_deal; //稳态子视图下，不能识别的按键处理方式，0表示忽略，1表示退出并过滤，
                            //2表示退出并返回MAIN视图继续处理，
                            //3表示如果MAIN视图能够识别就退出并返回MAIN视图继续处理
    uint16 overtime; //稳态子视图和提示子视图超时时间，
                     //对于稳态子视图，为0表示不启用超时机制，
                     //对于提示子视图，为0表示无效，不会创建子视图就马上退出
    const key_event_map_t *ke_maplist; //主视图和稳态子视图的按键事件映射表
    const sys_event_map_t *se_maplist; //主视图系统消息映射表
    view_update_cb view_cb; //视图更新回调函数
} create_view_param_t;

typedef struct
{
    uint8 id; //id有2个作用，第一用来匹配视图，第二用来作为app_timer的tag号
    uint8 type; //视图类型
    uint8 unknown_key_deal; //稳态子视图下，不能识别的按键处理方式，0表示忽略，1表示退出并过滤，
                            //2表示退出并返回MAIN视图继续处理，
                            //3表示如果MAIN视图能够识别就退出并返回MAIN视图继续处理
    int8 timer_id; //VIEW_TYPE_SUB 和 VIEW_TYPE_MSG 超时定时器
    const key_event_map_t *ke_maplist; //主视图和稳态子视图的按键事件映射表
    const sys_event_map_t *se_maplist; //主视图系统消息映射表
    view_update_cb view_cb; //视图更新回调函数
} view_infor_t;

typedef struct
{
    uint8 top;
    view_infor_t *p_key_view_infor; //当前活动视图信息结构体
} view_manager_infor_t;

#define MVC_VIEW_DEEPTH   3
extern view_infor_t g_view_infor[MVC_VIEW_DEEPTH];
extern view_manager_infor_t g_view_manager_infor;
extern key_event_t * g_p_view_ke;

extern int com_view_manager_init(void) __FAR__;
extern int com_view_manager_exit(void) __FAR__;
extern int com_view_add(uint8 view_id, create_view_param_t *p_create_view_param) __FAR__;
extern int com_view_remove(uint8 view_id, bool redraw) __FAR__;
extern int com_view_update(uint8 view_id) __FAR__;
extern void com_view_remove_top_view(void) __FAR__;
extern void com_view_remove_type_view(view_type_e type) __FAR__;

extern int com_view_key_event(key_event_t *p_ke, const key_event_map_t *app_ke_maplist, key_event_cb *ret_cb,
        bool only_com) __FAR__;
extern void com_view_key_check_dck(const key_event_map_t *app_ke_maplist) __FAR__;

typedef struct
{
    sys_event_cb ret_cb;
    uint8 ret_option;
    uint8 reserve[3];
} sys_event_ret_t;
extern int com_view_sys_event(sys_event_t *p_se, const sys_event_map_t *app_se_maplist, sys_event_ret_t *p_ret,
        bool only_com) __FAR__;

extern app_result_e com_view_loop_key(input_gui_msg_t *p_gui_msg) __FAR__;
extern app_result_e com_view_loop_sys(private_msg_t *p_pri_msg, bool tts_flag) __FAR__;
extern app_result_e com_view_loop(void) __FAR__;

#endif
