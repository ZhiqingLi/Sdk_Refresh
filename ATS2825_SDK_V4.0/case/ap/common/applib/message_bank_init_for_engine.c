/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：引擎应用和BT STACK消息初始化接口，会清空私有消息队列；如果是BT STACK，还要
 *       将BT MANAGER的事件队列也清空。
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
bool applib_message_init(void)
{
    //清空 应用消息队列
    sys_mq_flush(g_this_app_info->mq_id);

    return TRUE;
}

/*! \endcond */
