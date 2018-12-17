/*******************************************************************************
 *                              US211A
 *                            Module: music engine
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>        <time>           <version >            <desc>
 *     fiona.yang     2011-09-06 15:00     1.0             build this file
 *******************************************************************************/
#include "app_uengine.h"

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e uengine_set_volume(private_msg_t* cur_msg,uint32 volume)
 * \处理音量调节
 * \param[in]    private_msg_t* cur_msg
 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNOR 忽略
 * \retval       RESULT_APP_QUIT 退出app
 * \note         uengine_event_deal.c
 */
/*******************************************************************************/
app_result_e uengine_sync_volume(uint32 volume,uint8 mode)
{
    app_result_e ret_value = RESULT_NULL;

    uint32 set_vol;
    if(mode == 1)
    {
        set_vol = volume;
        goto set_true_vol;
    }

    //PRINT_INFO_INT("volsync", volume);

    //syn volume
    if ((volume >= 0xe3a0) && (volume <= 0xfff0))
    {
        set_vol = ((volume - 0xe3a0) / 226);
    }
    else if (volume < 0xe3a0)
    {
        set_vol = 0;
    }
    else
    {
        set_vol = VOLUME_VALUE_MAX;
    }
    set_true_vol:    
    if (set_vol > VOLUME_VALUE_MAX)
    {
        set_vol = VOLUME_VALUE_MAX;
    }

    PRINT_INFO_INT("volset", set_vol);
    //send asyn volume
    g_cur_play_status_p->change_vol_val = set_vol;

    //更新共享内存
    g_cur_play_status_p = sys_share_query_update(APP_ID_UENGINE);

    g_share_ptr->vol_flag = 1;

    g_share_ptr->vol_val = set_vol;

    return ret_value;

}


/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e uengine_set_volume(private_msg_t* cur_msg,uint32 volume)
 * \处理音量调节
 * \param[in]    private_msg_t* cur_msg
 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNOR 忽略
 * \retval       RESULT_APP_QUIT 退出app
 * \note         uengine_event_deal.c
 */
/*******************************************************************************/
app_result_e uengine_switch_deal(void)
{
    app_result_e ret_value = RESULT_NULL;

    
    //send asyn volume
    g_cur_play_status_p->switch_flag = 1;

    //更新共享内存
    g_cur_play_status_p = sys_share_query_update(APP_ID_UENGINE);
   
    return ret_value;

}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e uengine_set_eq(private_msg_t* cur_msg)
 * \处理eq 设置
 * \param[in]    private_msg_t* cur_msg
 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNOR 忽略
 * \retval       RESULT_APP_QUIT 退出app
 * \note         uengine_event_deal.c
 */
/*******************************************************************************/
app_result_e uengine_set_eq(private_msg_t* cur_msg)
{
    app_result_e ret_value = RESULT_NULL;

    //private_msg_t* data_ptr = (private_msg_t*) cur_msg;

    adjust_freq_add_value(0, g_dae_cfg_shm->run_freq);

    if (g_mmm_pp_handle != NULL)
    {
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_SET_EQ, (unsigned int) NULL);
    }

    return ret_value;

}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e uengine_play_pause(void)
 * \播放暂停
 * \param[in]
 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNOR 忽略
 * \retval       RESULT_APP_QUIT 退出app
 * \note         uengine_event_deal.c
 */
/*******************************************************************************/
app_result_e uengine_play_pause(void)
{

    if ((g_stop_by_tts == 0) && (g_status_data.start_play == 0))
    {
        //libc_print("no tts enter",0,0);

        uegine_player_close();
        g_ain_out_setting.dac_sample = (int32) g_status_data.sample_rate;

        g_ain_out_setting.adc_sample = (int32) g_status_data.sample_rate;

        //need set namual stop flag
        ud_set_cmd(SET_PLAY_FLAG, 1);

        uegine_player_open();
    }

    //hid report change is ok
    ud_set_cmd(SET_HID_OPERS, 0x08);

    //cur status
    /*if (g_status_data.start_play == 0)
     {
     //need clear namual stop flag
     ud_set_cmd(SET_PLAY_FLAG, 0);
     }
     else
     {

     //need set namual stop flag
     ud_set_cmd(SET_PLAY_FLAG, 1);
     }*/

    g_stop_by_tts = 0;

    return RESULT_NULL;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e uengine_next_song(void)
 * \下一曲
 * \param[in]
 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNOR 忽略
 * \retval       RESULT_APP_QUIT 退出app
 * \note         uengine_event_deal.c
 */
/*******************************************************************************/
app_result_e uengine_next_song(void)
{
    //hid report change is ok
    ud_set_cmd(SET_HID_OPERS, 0x10);

    g_stop_by_tts = 0;
    
    g_auto_play_count = 0;

    if(g_switch_song_flag != 0xff)
    {
        g_switch_song_flag = 0;
    }
    
    g_auto_play_flag = 1;

    return RESULT_NULL;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e uengine_prev_song(void)
 * \上一曲
 * \param[in]
 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNOR 忽略
 * \retval       RESULT_APP_QUIT 退出app
 * \note         uengine_event_deal.c
 */
/*******************************************************************************/
app_result_e uengine_prev_song(void)
{
    //hid report change is ok
    ud_set_cmd(SET_HID_OPERS, 0x20);

    g_stop_by_tts = 0;
    
    g_auto_play_count = 0;
    
    if(g_switch_song_flag != 0xff)
    {    
        g_switch_song_flag = 0;
    }
    
    g_auto_play_flag = 1;

    return RESULT_NULL;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e uengine_vol_add(void)
 * \调大音量
 * \param[in]
 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNOR 忽略
 * \retval       RESULT_APP_QUIT 退出app
 * \note         uengine_event_deal.c
 */
/*******************************************************************************/
app_result_e uengine_vol_add(void)
{

    //hid report change is ok
    ud_set_cmd(SET_HID_OPERS, 0x01);
    
    ud_hid_deal();

    g_stop_by_tts = 0;

    return RESULT_NULL;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e uengine_vol_sub(void)
 * \调小音量
 * \param[in]
 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNOR 忽略
 * \retval       RESULT_APP_QUIT 退出app
 * \note         uengine_event_deal.c
 */
/*******************************************************************************/
app_result_e uengine_vol_sub(void)
{

    //hid report change is ok
    ud_set_cmd(SET_HID_OPERS, 0x02);

    ud_hid_deal();
    
    g_stop_by_tts = 0;

    return RESULT_NULL;
}
/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e uengine_vol_sub(void)
 * \音量同步
 * \param[in]
 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNOR 忽略
 * \retval       RESULT_APP_QUIT 退出app
 * \note         uengine_event_deal.c
 */
/*******************************************************************************/
app_result_e uengine_vol_sync(uint8 vol)
{
    int8 vol_sync_cnt = 0;
    uint8 symbol = 0;
    uint8 i = 0;

    //libc_print("vol__",vol,2);
    
    if((vol == 0) || (vol == 31))
    {
        g_force_equ = 1;
    }
    #if 0
    if(vol == 0)
    {        
        for (i = 0;i < 25;i ++)
        {
            ud_set_cmd(SET_HID_OPERS, 0x02);
            
            ud_hid_deal();
            
            ud_set_cmd(SET_HID_OPERS, 0x00);
            
            ud_hid_deal();
        }
    }
    else if(vol == 31)
    {
        for (i = 0;i < 25;i ++)
        {
            ud_set_cmd(SET_HID_OPERS, 0x01);
            
            ud_hid_deal();
            
            ud_set_cmd(SET_HID_OPERS, 0x00);
            
            ud_hid_deal();
        }
    }
    else
    #endif
    {
        vol_sync_cnt = ((int8)vol - (int8)g_cur_play_status_p->change_vol_val);
        if(vol_sync_cnt < 0)
        {
            symbol = 1;
            vol_sync_cnt = 0 - vol_sync_cnt;
        }
        //if(vol_sync_cnt >= 15)
        {
            vol_sync_cnt = vol_sync_cnt/2;
        }
        if(vol_sync_cnt == 0)
        {
            vol_sync_cnt = 1;
        }
        //libc_print("adjust cnt:",vol_sync_cnt,2);
        if(symbol == 0)
        {
            for (i = 0;i < ( vol_sync_cnt);i ++)
            {
                ud_set_cmd(SET_HID_OPERS, 0x01);
                
                ud_hid_deal();
                
                ud_set_cmd(SET_HID_OPERS, 0x00);
                
                ud_hid_deal();
            }
        }
        else
        {
            for (i = 0;i < (vol_sync_cnt);i ++)
            {
                ud_set_cmd(SET_HID_OPERS, 0x02);
                
                ud_hid_deal();
                
                ud_set_cmd(SET_HID_OPERS, 0x00);
                
                ud_hid_deal();
            }
        }
    }
    
    g_vol_sync_timer = sys_get_ab_timer();
    
    return RESULT_NULL;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e uengine_tts_start(void)
 * \开始tts,需要做好互斥
 * \param[in]
 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNOR 忽略
 * \retval       RESULT_APP_QUIT 退出app
 * \note         uengine_event_deal.c
 */
/*******************************************************************************/
app_result_e uengine_tts_start(void)
{

    ud_get_status(&g_status_data);
    if(1 == g_uspeaker_24bit)
    {
        if(g_pa_type == 1)
        {
            ccd_i2s_pa_set_clock(44);
        }

        ud_set_cmd(SET_USPEAKER_24BIT,USPEAKER24_TTS_START);
    }

    ud_set_cmd(SET_TTS_FLAG,1);

    //stop mmm player
    uegine_player_close();

    //if play stop usound play
    if (g_status_data.start_play == 1)
    {
        uint8 i = 0;
        ud_set_cmd(SET_HID_OPERS, 0x08);
        //make sure pc receive the stop cmd
        while (i < 4)
        {
            ud_hid_deal();
            i++;
            sys_os_time_dly(1);
        }
    }

    g_stop_by_tts = 1;
    
    g_switch_song_flag = 0xff;
    
    return RESULT_NULL;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e uengine_tts_stop(void)
 * \tts结束,继续运行
 * \param[in]
 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNOR 忽略
 * \retval       RESULT_APP_QUIT 退出app
 * \note         uengine_event_deal.c
 */
/*******************************************************************************/
app_result_e uengine_tts_stop(void)
{

    g_ain_out_setting.dac_sample = (int32) g_status_data.sample_rate;

    g_ain_out_setting.adc_sample = (int32) g_status_data.sample_rate;
    
    if(1 == g_uspeaker_24bit)
    {
        g_ain_out_setting.dac_sample = (int32) g_status_data.sample_rate;
                
        g_ain_out_setting.adc_sample = (int32) g_status_data.sample_rate;
        
        if(g_pa_type == 1)
        {
            ccd_i2s_pa_set_clock(g_status_data.sample_rate);
        }

        ud_set_cmd(SET_USPEAKER_24BIT,USPEAKER24_SAMPLE_RATE);

        ud_set_cmd(SET_USPEAKER_24BIT,USPEAKER24_TTS_STOP);
    }
    ud_set_cmd(SET_TTS_FLAG,0);

    //manual stop flag
    ud_set_cmd(SET_PLAY_FLAG, 1);
    //hid report change is ok
    uegine_player_open();

    g_switch_song_flag = 0;
    
    return RESULT_NULL;
}
