/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：前台应用消息初始化接口，会清空GUI消息队列和私有消息队列。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    初始化 applib 消息通信管理模块。
 * \param[in]    none
 * \param[out]   none
 * \return       bool
 * \retval           TRUE 初始化消息通信管理模块成功。
 * \retval           FALSE 初始化消息通信管理模块失败
 * \ingroup      applib_msg
 * \par          exmaple code
 * \code
 applib_message_init();
 * \endcode
 * \note
 *******************************************************************************/
extern bool com_gui_msg_hook(input_gui_msg_t *input_msg) __FAR__;
extern void com_app_msg_hook(private_msg_t *private_msg) __FAR__;
bool applib_message_init(void)
{
    //清空 应用消息队列
    sys_mq_flush(g_this_app_info->mq_id);

    g_this_gui_msg_hook = com_gui_msg_hook;
    g_this_app_msg_hook = com_app_msg_hook;

    return TRUE;
}

/*! \endcond */
