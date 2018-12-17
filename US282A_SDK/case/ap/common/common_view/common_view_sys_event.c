/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：前台应用系统事件分发入口，应用自定义系统事件优先分发，然后再COMMON系统
 *       事件分发；也支持仅COMMON系统事件分发。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"
#include "common_func.h"
#include "app_view.h"

extern const sys_event_map_t com_se_maplist[];

int com_view_sys_event(sys_event_t *p_se, const sys_event_map_t *app_se_maplist, sys_event_ret_t *p_ret, bool only_com)
{
    const sys_event_map_t *this_se_map;

    if ((app_se_maplist != NULL) && (only_com == FALSE))
    {
        this_se_map = app_se_maplist;
        for (;; this_se_map++)
        {
            if (MSG_NULL == this_se_map->se.msg_type)
            {
                break;
            }

            if (this_se_map->se.msg_type == p_se->msg_type)
            {
                p_ret->ret_cb = this_se_map->cb;
                p_ret->ret_option = this_se_map->se.msg_option;
                return 0;
            }
        }
    }

    this_se_map = com_se_maplist;
    for (;; this_se_map++)
    {
        if (MSG_NULL == this_se_map->se.msg_type)
        {
            break;
        }

        if (this_se_map->se.msg_type == p_se->msg_type)
        {
            p_ret->ret_cb = this_se_map->cb;
            p_ret->ret_option = this_se_map->se.msg_option;
            return 0;
        }
    }

    return -1;
}
