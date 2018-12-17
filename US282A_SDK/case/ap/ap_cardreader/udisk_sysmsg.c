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

#include "ap_udisk.h"

/******************************************************************************/
/*! app_result_e ud_unstick_msg_deal(void *ev_param)
 * \par  Description:
 * \ 系统消息MSG_USB_UNSTICK处理
 * \param[in]    ev_param
 * \param[out]   none
 * \return       app_result_e
 * \ingroup      udisk_sysmsg.c
 * \note
 *******************************************************************************/
app_result_e ud_unstick_msg_deal(void *ev_param)
{
    ev_param = ev_param;//avoid warning

    return RESULT_APP_QUIT;
}

/******************************************************************************/
/* ! app_result_e ud_sdin_msg_deal(void *ev_param)
 * \par  Description:
 * \ 系统消息SD_IN处理
 * \param[in]    ev_param
 * \param[out]   none
 * \return       app_result_e
 * \ingroup      udisk_sysmsg.c
 * \note
 *******************************************************************************/
app_result_e ud_sdin_msg_deal(void *ev_param)
{
    ev_param = ev_param;//avoid warning

    ud_set_cmd(SET_CARD_INOUT, 1);

    return RESULT_NULL;
}

/******************************************************************************/
/*! app_result_e ud_sdout_msg_deal(void *ev_param)
 * \par  Description:
 * \ 系统消息MSG_SD_OUT处理
 * \param[in]    ev_param
 * \param[out]   none
 * \return       app_result_e
 * \ingroup      udisk_sysmsg.c
 * \note
 *******************************************************************************/
app_result_e ud_sdout_msg_deal(void *ev_param)
{
    ev_param = ev_param;//avoid warning

    ud_set_cmd(SET_CARD_INOUT, 0);

    return RESULT_NULL;
}

/******************************************************************************/
/*! app_result_e ud_poweroff_msg_deal(void *ev_param)
 * \par  Description:
 * \ 系统消息MSG_power_off处理
 * \param[in]    ev_param
 * \param[out]   none
 * \return       app_result_e
 * \ingroup      udisk_sysmsg.c
 * \note
 *******************************************************************************/
#if (SUPPORT_OUTER_CHARGE == 1)
app_result_e ud_poweroff_msg_deal(void *ev_param)
{
    if ((IS_EXTERN_CAHREGE_IN() == 0) && (IS_EXTERN_BATTERY_IN() == 0))
    {
        return RESULT_NULL;
    }
    else
    {
        return RESULT_POWER_OFF;
    }

}
#endif

