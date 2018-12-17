/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：软定时器执行接口，需要常驻内存；仅引擎应用和BT STACK使用。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    应用级定时器 handle 处理，在应用消息循环中循环调用，循环扫描所有定时器，执行
 *    定时溢出的定时器的服务例程 proc，并根据定时器类型进行设置。
 * \param[in]    timers 应用软定时器数组，或者common全局软定时器数组
 * \param[in]    count 软定时器数组
 * \param[out]   none
 * \return       none
 * \ingroup      app_timer
 * \note
 *******************************************************************************/
void handle_timers(app_timer_t *timers, uint8 count)
{
    app_timer_t *tm;
    uint32 cur_time_ms = sys_get_ab_timer(); //如果没有执行软定时器服务例程，认为当前时间不变
    uint32 next_time_ms;
    uint16 next_timeout;
    uint8 i;
    bool need_proc;

    for (i = 0; i < count; i++)
    {
        tm = timers + i;

        //定时器正在运行才需要执行
        if (tm->state != TIMER_STATE_RUNNING)
        {
            continue;
        }

        //TAG场景检查
        if ((tm->tag != g_this_app_timer_tag) && (tm->tag != APP_TIMER_TAG_SYS))
        {
            continue;
        }

        g_app_timer_id = i;
        next_timeout = tm->timeout;
        if ((tm->twinkle == TRUE) && (tm->on_off == FALSE))
        {
            next_timeout = tm->timeout_off;
        }
        next_time_ms = cur_time_ms + next_timeout;
        need_proc = FALSE;
        if (cur_time_ms >= tm->timeout_expires) //定时时间已到
        {
            //单发软定时器一次执行完后 kill 掉
            if (tm->type == TIMER_TYPE_SINGLE_SHOT)
            {
                tm->state = TIMER_STATE_NOUSED;
            }
            need_proc = TRUE;

            //重设软定时器超时时间
            if (tm->timeout > (cur_time_ms - tm->timeout_expires))
            {
                //当前时间>=这次expires时间<下次expires时间
                tm->timeout_expires += next_timeout; //直接在expires上加定时周期，以确保多次定时时间准确
            }
            else
            {
                //当前时间>=下次expires时间
                //已经错过了一次expires(说明timeout太短了)
                //将下次expires时间设置到当前时间的timeout之后, 不然下次timeout立即就会到来
                tm->timeout_expires = next_time_ms;
            }

            if (tm->twinkle == TRUE)
            {
                if (tm->on_off == TRUE)
                {
                    tm->on_off = FALSE;
                }
                else
                {
                    tm->on_off = TRUE;

                    if (tm->count != 0xff)
                    {
                        tm->count--;
                    }
                    if (tm->count == 0)
                    {
                        tm->state = TIMER_STATE_NOUSED;
                        need_proc = FALSE;
                    }
                }
            }
        }

        if (next_time_ms <= tm->timeout_expires)
        {
            //如果系统时间因某种原因落在超时时间点后面，那么必须修复该时间点
            tm->timeout_expires = next_time_ms;
        }

        //调用软定时器服务例程
        if ((need_proc == TRUE) && (tm->func_proc != NULL))
        {
            tm->func_proc();
        }

        //执行过软定时器服务例程，重新加载当前时间，这样时间较为准确
        cur_time_ms = sys_get_ab_timer();
    }
}

/*! \endcond */
