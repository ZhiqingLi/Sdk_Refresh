/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：BT MANAGER 专用软定时器创建单发定时器接口。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    创建单发应用级定时器。
 * \param[in]    attrb 定时器属性，属于另一种分类，分 control 和 ui 类
 * \param[in]    timeout 定时器周期，以 1ms 为单位
 * \param[in]    func_proc 定时器服务例程
 * \param[out]   none
 * \return       int8
 * \retval           id 申请成功，返回定时器ID，范围为 0 ~ g_app_timer_count - 1。
 * \retval           -1 申请失败，或定时器资源用完，或 timeout 为0，或 proc 为NULL。
 * \ingroup      app_timer
 * \par          exmaple code
 * \code
 例子1：创建 5s 超时定时器，用于启动某个事件
 //5s 超时定时器 ISR，置标志 g_timeout_flagd
 static void __timeout_handle(void)
 {
 g_timeout_flagd = TRUE;
 }

 bool g_timeout_flagd = FALSE;//全局变量
 int8 timeout_timer = -1;
 //创建 control类 应用级定时器
 timeout_timer = set_single_shot_app_timer(APP_TIMER_ATTRB_UI, 5000, __timeout_handle);
 if(timeout_timer == -1)
 {
 创建失败，原因为定时器已被申请完
 }

 //删除有效应用级定时器，如果能够确保单发定时器会执行到，那么就不需要执行删除动作了
 if(timeout_timer != -1)
 {
 kill_app_timer(timeout_timer);//如果在此之前单发定时器已自行删除，会返回错误，但是不会崩溃
 timeout_timer = -1;
 }
 * \endcode
 * \note
 * \li  单发应用级定时器在唯一一次 ISR 执行时就被自动销毁，用户可以不用自己销毁。
 * \li  但是这种定时器也有问题，就是用户不能完全把握定时器的生命周期，不能确定定时器
 *      是否被销毁了，这样就有可能重复创建相同的定时器。
 * \li  timeout不能为0，func_proc不能为空，否则返回失败。
 *******************************************************************************/
int8 set_single_shot_app_timer_btmamager(uint32 attrb_tagh, uint16 timeout, timer_proc func_proc)
{
    timer_attrb_e attrb = APP_TIMER_ATTRB_CONTROL;
    timer_type_e type = TIMER_TYPE_SINGLE_SHOT;
    uint8 tag = APP_TIMER_TAG_SYS;

    app_timer_t *tm;
    int8 timer_id = -1;
    int8 i;

    //检索符合条件的可用软定时器
    for (i = 0; i < COM_APP_TIMER_MAX; i++)
    {
        if (g_com_app_timer_vct[i].state == TIMER_STATE_NOUSED)
        {
            tm = g_com_app_timer_vct + i;
            //高4bit置为0100表示COM软定时器
            timer_id = 0x40 | i;

            //初始化软定时器
            tm->timeout = timeout;
            tm->timeout_off = 0;
            tm->timeout_expires = sys_get_ab_timer() + timeout;
            tm->func_proc = func_proc;
            tm->state = TIMER_STATE_RUNNING;
            tm->attrb = attrb;
            tm->type = type;
            tm->twinkle = FALSE;
            tm->on_off = FALSE;
            tm->tag = tag;
            tm->count = 0;

            break;
        }
    }

    if (timer_id == -1)
    {
        PRINT_ERR("set app timer fail!!");
        while (1)
        {
            ; //nothing
        }
    }

    return timer_id;
}

/*! \endcond */
