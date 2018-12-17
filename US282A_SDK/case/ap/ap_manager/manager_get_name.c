/*******************************************************************************
 *                              US212A
 *                            Module: Manager
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     manager_get_name.c
 * \brief    根据应用ID获取应用名称
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include  "manager.h"

const char app_name_ram[APP_ID_MAX][12] =
{
    "music.ap",    "linein.ap",   "btplay.ap",   "btcall.ap",    "radio.ap",    "usound.ap",  
    "udisk.ap",    "record.ap",   "tktest.ap",   "user2.ap",     "config.ap",   "manager.ap", 
    "upgrade.ap",  "btstack.ap",  "mengine.ap",  "lineineg.ap",  "btplayeg.ap", "btcalleg.ap", 
    "fmengine.ap", "uengine.ap",  "otatest.ap",    "ota.ap",     "alarm.ap",  "btstkota.ap",
};

/******************************************************************************/
/*!
 * \par  Description:
 *    根据ap的ID，获取ap的名称，系统创建ap的接口需要用到ap的名称
 * \param[in]    char *namebuf，存放应用名称的buffer地址
 * \param[in]    uint8 ap_id，应用ID
 * \param[out]   none
 * \return       none
 * \retval
 * \note
 *******************************************************************************/
void _get_app_name(char *namebuf, uint8 ap_id, uint8 type)
{   
    if(ap_id < APP_ID_MAX)
    {
        sys_os_sched_lock();
        
        libc_memcpy(namebuf, app_name_ram[ap_id], 12);

        sys_os_sched_unlock();
        
        if(type == APP_TYPE_GUI)
        {
            if(g_app_info_state.autotest_mode == TRUE)
            {
                g_p_test_share_info->front_ap_id = ap_id;
                g_p_test_share_info->ap_switch_flag = TRUE;
            }
        }
        
    }
    else
    {
        libc_print("invalid ap id:", ap_id, 2);
    }
}

