/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：前台应用接收按键消息接口实现，该接口仅在前台应用中调用。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    接收 gui 消息，即输入型消息。
 * \param[in]    none
 * \param[out]   input_msg：返回接收到的 gui 消息
 * \return       bool
 * \retval           TRUE 接收 gui 消息成功。
 * \retval           FALSE 接收 gui 消息失败。
 * \ingroup      applib_msg
 * \par          exmaple code
 * \code
 例子1：gui 消息（事件）循环
 input_gui_msg_t input_msg;
 msg_apps_type_e gui_event;
 bool ret;

 ret = get_gui_msg(&input_msg);
 if(ret == TRUE)
 {
 if(com_key_mapping(&input_msg, &gui_event, key_map_list) == TRUE)
 {
 switch(gui_event)
 {
 case EVENT_DO_SOMETHING:
 break;

 default:
 result = com_message_box(gui_event);
 break;
 }
 }
 }
 else
 {
 gui 消息（事件）处理完毕，接下来可以处理应用私有消息了
 }
 * \endcode
 * \note
 *******************************************************************************/
bool get_gui_msg(input_gui_msg_t *input_msg)
{
    uint32 tmp_cur_ab_timer = sys_get_ab_timer();
    int key_cnt;
    bool ret;

    if (g_buffer_gui_msg.data.kmsg.val != KEY_NULL)
    {
        //缓冲消息，直接拷贝
        libc_memcpy(input_msg, &g_buffer_gui_msg, sizeof(input_gui_msg_t));
        g_buffer_gui_msg.data.kmsg.val = KEY_NULL;
        return TRUE;
    }

    key_cnt = sys_mq_traverse(MQ_ID_GUI, NULL, 0);
    if (key_cnt <= 0)
    {
        if ((uint16) ((uint16) tmp_cur_ab_timer - g_key_infor.last_detect_timestamp) > 500)
        {
            g_key_infor.status = KEY_STATUS_NULL;
            g_key_infor.dck_flag = FALSE;
            g_key_infor.dck_fail = FALSE;
        }
        return FALSE;
    }
    else
    {
        g_key_infor.last_detect_timestamp = (uint16) tmp_cur_ab_timer;
    }

    if (g_key_infor.dck_flag == TRUE)
    {
        if (com_need_cont_dck_deal((uint8) key_cnt) == FALSE)
        {
            return FALSE;
        }
    }

    if (com_phy_key_fsm(input_msg) == FALSE)
    {
        return FALSE;
    }

    //收到gui消息，表示有用户操作，进行一些系统处理，比如：
    //0.长按按键后过滤掉后续按键
    //1.背光关闭计时，屏幕保护计时，省电关机计时，返回正在播放计时 清0
    //2.按键音处理
    //3.按键锁处理
    ret = g_this_gui_msg_hook(input_msg);

    return ret;
}

/*! \endcond */
