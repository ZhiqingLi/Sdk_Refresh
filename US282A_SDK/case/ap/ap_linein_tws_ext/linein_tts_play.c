/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：line in tws推歌前台应用TTS相关接口实现。
 * 作者：
 ********************************************************************************/

#include  "ap_linein.h"

#ifdef ENABLE_TRUE_WIRELESS_STEREO
//当从单箱变成副箱后就利用该该函数负责播报tts
void linein_tws_tts_play(void)
{
    //tws打电话的时候副箱连上不播报组对成功
    if((g_bt_stack_cur_info.pair==1)&&(flag2==1))
    {
        //g_app_info_state_all.g_send_exit_mmm_tts = 1;
        //蓝牙组对成功
        com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_PAIR_SUCCEED);
        flag2=0;

    }
    else if(g_bt_stack_cur_info.tws_dev_discon == 1)
    {
        //语音播报蓝牙断开&等待连接
        com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_BT_DISCON);
        com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_WAIT_BT_CON);
        flag_con_ok_one=0;
        PRINT_INFO("dis2 ");
    }
    else
    {
        //do nothing
    }
}

#endif

