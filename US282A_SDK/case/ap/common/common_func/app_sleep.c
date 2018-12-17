/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：前台应用睡眠接口，睡眠过程中可通过按键提前终止。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    应用层休眠，可以按键中断，并且该按键消息在退出 app_sleep 后被接收到。
 * \param[in]    sleep_timer 睡眠时间，以ms为单位
 * \param[out]   none
 * \return       app_result_e
 * \retval           RESULT_NULL 睡眠时间到时返回，无效
 * \ingroup      misc_func
 * \par          exmaple code
 * \code
 例子1：等待2秒钟，按任意按键退出
 app_result_e result;
 result = com_app_sleep(2000);

 * \endcode
 * \note
 *******************************************************************************/
app_result_e com_app_sleep(uint32 sleep_timer)
{
    input_gui_msg_t input_msg;
    uint32 sleep_100ms_timer;

    //计算 100ms 计时器
    sleep_100ms_timer = sleep_timer / 100;
    if (sleep_100ms_timer == 0)
    {
        return RESULT_NULL;
    }
    while (1)
    {
        //获取gui消息
        while (get_gui_msg(&input_msg) == TRUE) //有gui消息
        {
            //按任意按键返回
            libc_memcpy(&g_buffer_gui_msg, &input_msg, sizeof(input_gui_msg_t)); //备份缓冲消息
            return RESULT_NULL;
        }

        //挂起100ms，多任务调度
        sys_os_time_dly(10);

        //睡眠时间计时器递减
        sleep_100ms_timer--;
        //睡眠时间已到，退出
        if (sleep_100ms_timer == 0)
        {
            return RESULT_NULL;
        }
    }
}

/*! \endcond */
