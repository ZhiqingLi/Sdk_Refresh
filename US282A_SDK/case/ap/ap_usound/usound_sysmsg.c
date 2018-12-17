/*******************************************************************************
 *                              US212A
 *                            Module: music ui
 *                 Copyright(c) 2003-2012 Actions Semiconductor
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *     fiona.yang   2011-09-14          1.0              create this file
 *******************************************************************************/

#include "ap_usound.h"


/******************************************************************************/
/*! app_result_e ud_unstick_msg_deal(void *ev_param)
 * \par  Description:
 * \ 系统消息MSG_USB_UNSTICK处理
 * \param[in]    ev_param
 * \param[out]   none
 * \return       app_result_e
 * \ingroup      usound_sysmsg.c
 * \note
 *******************************************************************************/
app_result_e ud_unstick_msg_deal(void *ev_param)
{
    ev_param = ev_param;//avoid warning
 #ifdef __ESD_MODE_
    return RESULT_NULL;
 #else
    return RESULT_NEXT_FUNCTION;
 #endif
}

