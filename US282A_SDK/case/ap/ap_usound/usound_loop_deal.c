/*******************************************************************************
 *                              US212A
 *                            Module: MainMenu
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/

#include "ap_usound.h"

const key_event_map_t __section__(".rodata.ke_maplist") usound_ke_maplist[] =
{
    /*! 短按按next切换下一曲事件 */
    { { KEY_NEXT_VOLADD, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL}, _key_next_deal},
    /*! 短按按prev切换上一曲事件 */
    { { KEY_PREV_VOLSUB, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL}, _key_prev_deal},
    /*! 短按按next切换下一曲事件 */
    { { KEY_NEXT, 0, KEY_TYPE_SHORT_UP | KEY_TYPE_LONG_UP, KEY_DEAL_NULL}, _key_next_deal},
    /*! 短按按prev切换上一曲事件 */
    { { KEY_PREV, 0, KEY_TYPE_SHORT_UP | KEY_TYPE_LONG_UP, KEY_DEAL_NULL}, _key_prev_deal},
    /*!短按play播放/暂停事件 */
    { { KEY_PLAY, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL}, _key_play_deal},

    /*! 音量加 */
    { { KEY_VADD, 0, KEY_TYPE_DOWN | KEY_TYPE_LONG | KEY_TYPE_HOLD, KEY_DEAL_FILTER_UP}, _key_vol_add_deal},
    /*! 音量加 */
    { { KEY_NEXT_VOLADD, 0, KEY_TYPE_LONG | KEY_TYPE_HOLD, KEY_DEAL_FILTER_UP}, _key_vol_add_deal},

    /*! 音量减 */
    { { KEY_VSUB, 0, KEY_TYPE_DOWN | KEY_TYPE_LONG | KEY_TYPE_HOLD, KEY_DEAL_FILTER_UP}, _key_vol_sub_deal},
    /*! 音量减 */
    { { KEY_PREV_VOLSUB, 0, KEY_TYPE_LONG | KEY_TYPE_HOLD, KEY_DEAL_FILTER_UP}, _key_vol_sub_deal},

    /*! 结束标志 */
    { { KEY_NULL, 0, KEY_TYPE_NULL, KEY_DEAL_NULL}, NULL},
};

const sys_event_map_t __section__(".rodata.se_maplist") usound_se_maplist[] =
{
    //防止漏检

    { { MSG_USB_UNSTICK, SYSMSG_STOP_TTS_YES}, ud_unstick_msg_deal},

    /*! 结束标志 */
    { { MSG_NULL, 0}, NULL},
};

/******************************************************************************/
/*!
 * \par  Description:
 *      MainMenu 应用的入口函数和场景管理器模块
 * \param[in]    PARAM_FROM_XXX
 * \param[out]   none
 * \return       the result
 * \retval       0 sucess
 * \retval       1 failed
 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 *******************************************************************************/
app_result_e get_message_loop(void)
{
    app_result_e result = RESULT_NULL;
    uint8 NTRIRQ_count = 0;
    uint8 NO_NTRIRQ_count = 0;

    g_volume_share = (uengine_sync_volume_t *) sys_shm_mount(SHARE_MEM_ID_USOUND);
    if (g_volume_share == NULL)
    {
        PRINT_ERR("usound shm not exist!!");
        while (1)
        {
            ; //nothing for QAC
        }
    }

    while (1)
    {

        //get status
        usound_get_status(&g_play_status);
        
        //读取NTRIRQ判断是否为充电器      
        if((act_readl(Usbirq_hcusbirq) & (0x01 << 6)) != 0)
        {
            act_writel((act_readl(Usbirq_hcusbirq)|(0x01<<6)),Usbirq_hcusbirq);
            NTRIRQ_count ++;
            NO_NTRIRQ_count = 0;
        }
        else
        {
            NO_NTRIRQ_count ++;
        }
        if(NTRIRQ_count >= 5)
        {
#ifndef __ESD_MODE_            
            result = RESULT_NEXT_FUNCTION;
            libc_print("charger in",0,0);
            break;
#endif            
        }
        if(NO_NTRIRQ_count >= 200)
        {
            NTRIRQ_count = 0;
            NO_NTRIRQ_count = 0;
        }

        if (g_volume_share != NULL)
        {
            if (g_volume_share->vol_flag != 0)
            {
                g_volume_share->vol_flag = 0;
                
                PRINT_INFO_INT("usond_set_vol",g_volume_share->vol_val);
                com_volume_set((uint8) g_volume_share->vol_val, 1);

                g_pc_syn_flag = 1;                
            }
        }

        if (g_play_status.line_status == 1)
        {
            //esd可能会让line_status状态不正常，导致切换应用，因此，ESD不走此分支
 #ifndef __ESD_MODE_
            result = RESULT_NEXT_FUNCTION;
            break;
 #endif
            
        }
        
        if (g_play_status.switch_flag == 1)
        {
            libc_print("switch stub ok", 0, 0);

            //进入stub应用程序
            result = RESULT_ENTER_STUB_DEBUG;

            break;
        }

        result = com_view_loop();

        if (result > RESULT_COMMON_RESERVE)
        {
            break;
        }

        if (g_play_key_timer != -1)
        {
            g_play_key_timer++;
        }

        if (g_play_key_timer >= 3000)
        {
            g_play_key_timer = -1;
        }

        //挂起10ms，多任务调度
        sys_os_time_dly(1);

    }

    return result;
}

