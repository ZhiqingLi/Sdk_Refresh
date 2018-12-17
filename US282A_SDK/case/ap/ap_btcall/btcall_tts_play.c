/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *        brief   处理tts播报
 *      <author>       <time>
 *       Wekan   2015-3-27
 *******************************************************************************/

#include  "ap_btcall.h"

/******************************************************************************
 * \par  Description:   播报tts
 * \param[in]
 * \param[out]   null
 * \return       null
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/
void btcall_tts_play(void)
{

    //switch (g_btcall_cur_info.status)
    switch (g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.hfp_active_id].hfp_status)
    {

        case BTCALL_IDLE:

        if (g_standby_exit_flag == FALSE)
        {
            // PRINT_INFO("T7 ");
            if (g_last_btcall_status == BTCALL_IDLE)
            {
                //语音播报等待连接
                if (g_bt_stack_cur_info.conn_status == CONN_STATUS_WAIT_PAIR)
                {
                    com_tts_state_play((uint16) TTS_MODE_ONLYONE, (void*) TTS_WAIT_BT_CON);
                }
                //PRINT_INFO("T2 ");
            }
            else
            {
                //蓝牙断开开启tts播报
                com_tts_state_play_control(TTS_CTRL_CANCEL_IGNORE);
                //语音播报蓝牙断开&等待连接
                com_tts_state_play((uint16) TTS_MODE_ONLYONE, (void*) TTS_BT_DISCON);
                com_tts_state_play((uint16) TTS_MODE_ONLYONE, (void*) TTS_WAIT_BT_CON);
            }
        }
        break;

        default:
        if (g_last_btcall_status == BTCALL_IDLE)
        {
            //蓝牙连接成功
            com_tts_state_play((uint16) TTS_MODE_ONLYONE, (void*) TTS_BT_CON_OK);
        }
        break;
    }
}

