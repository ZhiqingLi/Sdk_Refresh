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

//这里增加能量检测是为了解决win7下大部分播放器在暂停时还会发送20多m左右的ISO数据过来
//导致小机没办法正常播放当前的播放状态，从而出现播放和播报混乱的问题
uint32 g_temp_conut;
uint16 g_energy_mean;
uint16 g_energy_max;
uint8 g_energy_same_count;

void __section__(".bank")auto_play_for_esd(uint8* check_time)
{
    
    if(g_status_data.start_play == 0)
    {
        ud_set_cmd(SET_HID_OPERS, 0x08);
    
        ud_hid_deal();

        //libc_print("ESD auto play",*check_time,2);
        (*check_time)++;

    }
    else
    {
        //libc_print("check_auto_time",*check_time,2);
        *check_time = 3;
    }

}
/*用于检测ESD后，DSP是否正常工作*/
void __section__(".bank")esd_check_dsp_excption(short *energy_mean,short *energy_max,uint32 prit_count)
{
    if((prit_count&0x3ff) == 0x280)
    {
        //libc_print("*energy_mean",*energy_mean,2);
        //libc_print("*energy_max",*energy_max,2);
    }
    if((g_energy_mean == *energy_mean)
        &&(g_energy_max == *energy_max)
        &&(g_energy_mean != 0))
    {
        g_energy_same_count++;
        //libc_print("g_energy_same_count",g_energy_same_count,2);
        if(g_energy_same_count == 10)
        {
            //libc_print("reboot!!!!!",0,0);
            ENABLE_WATCH_DOG(4);
            while(1)
            {
                ;//nothing
            }
        }
    }
    else
    {
        g_energy_same_count = 0;
        g_energy_mean = (uint16)*energy_mean;
        g_energy_max = (uint16)*energy_max;
    }
}
void check_energy_info(void)
{
    short *p_energy_mean;
    short *p_energy_max;

    if (g_app_info_state_all.energy_available_flag == TRUE)
    {
        p_energy_mean = g_app_info_state_all.p_energy_value;
        p_energy_max = g_app_info_state_all.p_energy_value + 1;
#ifdef __ESD_MODE_
        g_temp_conut ++;
        if((g_temp_conut & 0xff) == 0x80)
        {
            esd_check_dsp_excption(p_energy_mean,p_energy_max,g_temp_conut); 
        }
#endif        
        if ((*p_energy_mean == 0) && (*p_energy_max < 5))
        {
            energy_count++;
            if (energy_count == MAX_ENERGY_NUM)
            {
                //设置能量低标志，告知驱动当前处理暂停状态
                ud_set_cmd(SET_PLAY_FLAG, 3);
                //g_data_flag = 1;
            }
        }
        else
        {
            if (energy_count >= MAX_ENERGY_NUM)
            {
                //清除能量低标志，恢复正常状态处理
                ud_set_cmd(SET_PLAY_FLAG, 4);
                //g_data_flag = 0;
            }
            energy_count=0;
        }
        //libc_print("\n energy_mean:", *p_energy_mean,2);
        //libc_print("\n energy_max:", *p_energy_max,2);
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 * \void uengine_control_block(void)
 * \引擎功能处理函数
 * \param[in]    void  para1
 * \param[out]   none
 * \return       void the result
 * \retval       none
 * \ingroup      uengine_control.c
 * \note
 */
/*******************************************************************************/
app_result_e uengine_control_block(void)
{
    //mmm_pp_status_t play_status;

    private_msg_t cur_msg_val;

    uint32 hid_count = 0;

    app_result_e uengine_retval = RESULT_NULL;

    uint8 i = 0;
    
    int8 vol_dif = 0;

    //250ms处理hid 事务
    //g_hid_timer = set_app_timer(APP_TIMER_ATTRB_CONTROL, 500, uegine_deal_hid);
    
    uint32 flags;
#ifdef __ESD_MODE_
    uint8 check_auto_play_time = 0;   
    uint32 esd_count_time;
#endif
 
    while (1)
    {
        //查询当前播放状态
        ud_get_status(&g_status_data);
        if (g_status_data.start_play == 0)
        {
            g_stop_count++;

            if (g_stop_count >= 5)
            {
                g_cur_play_status_p->play_status = 0;
            }
#ifdef __ESD_MODE_
            if(g_stop_count == 500)
            {
                //libc_print("no_empty_fla",g_status_data.no_empty_fla,2);
                g_stop_count = 0;
                if(g_status_data.no_empty_fla==1)
                {
                    //libc_print("error!reset sys",0,0);
                    ENABLE_WATCH_DOG(4);
                    while(1)
                    {
                        ;//nothing
                    }
                }
            }
#endif            
        }
        else
        {
            g_stop_count = 0;

            g_cur_play_status_p->play_status = 1;

        }
        
        if(g_status_data.data_in_flag != 0)
        {    
            if(g_mute_cnt_flag == 0)
            {
                mute_cnt = sys_get_ab_timer();
                
                g_mute_cnt_flag = 1;
            }
        }

        if (((sys_get_ab_timer() - mute_cnt) >= MUTE_DELAY) && (g_mute_flag == 1) && (g_mute_cnt_flag == 1))
        {            
            ud_set_cmd(SET_MUTE_FLAG,0);
            
            g_mute_flag = 0;
        }

        g_cur_play_status_p->line_status = g_status_data.line_sta;

        //更新共享内存
        g_cur_play_status_p = sys_share_query_update(APP_ID_UENGINE);

        if (((g_volume_syn_cfg & 0x02) != 0) && (g_status_data.volume_chg != 0))
        {

            ud_set_cmd(SET_VOLUME_FLAG, 0);

            //change volume
            uengine_sync_volume(g_status_data.volume_value,0);

            //sys_os_time_dly(1);
        }
        
        if (g_status_data.switch_flag != 0)
        {
            ud_set_cmd(CLR_SWITCH_FLAG, 0);
              
            uengine_switch_deal();
        }

        //hid change

        hid_count++;

        if ((g_status_data.hid_idle_time != 0) && (g_status_data.hid_idle_time < (hid_count * 10)))
        {
            hid_count = 0;

            ud_set_cmd(SET_HID_CHANGE, 0);
        }

        //play flag need set sample rate
        if (g_status_data.set_sample == 1)
        {
            if (g_mmm_pp_handle != NULL)
            {
                flags = sys_local_irq_save();
                
                ud_get_status(&g_status_data);
                
                //clr flag
                ud_set_cmd(SET_SAMPLE_RATE, 0);
                
                sys_local_irq_restore(flags);
                //set adjust
                    //ud_set_cmd(SET_ADJUST_TIMER, 0);

                //mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_SET_PARAM, (unsigned int) &g_status_data.sample_rate);
                if(g_status_data.set_adc_sample == 1)
                {
                    mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_SET_ADC_RATE, (unsigned int) &g_status_data.adc_sample_rate);
                }
                
                if(g_status_data.set_dac_sample == 1)
                {
                    mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_SET_DAC_RATE, (unsigned int) &g_status_data.dac_sample_rate);
                }

                g_ain_out_setting.dac_sample = (int32) g_status_data.dac_sample_rate;

                g_ain_out_setting.adc_sample = (int32) g_status_data.adc_sample_rate;


                //ud_set_cmd(SET_ADJUST_TIMER, 1);
            }
            else if(1 == g_uspeaker_24bit)
            {
                if((g_ain_out_setting.dac_sample != (int32) g_status_data.sample_rate)&&(g_pa_type == 1))
                {
                    ccd_i2s_pa_set_clock(g_status_data.sample_rate);
                }
                
                g_ain_out_setting.dac_sample = (int32) g_status_data.sample_rate;
                
                g_ain_out_setting.adc_sample = (int32) g_status_data.sample_rate;

                //ccd_i2s_pa_set_clock(0xff);
                
                ud_set_cmd(SET_USPEAKER_24BIT,USPEAKER24_SAMPLE_RATE);
            }
            else
            {
                ;//for QAC
            }
        }

        //获取并处理详细
        if (get_app_msg(&cur_msg_val) == TRUE)
        {
            uengine_retval = uengine_message_deal(&cur_msg_val);
        }

        if (uengine_retval == RESULT_APP_QUIT)
        {
            break;
        }
        check_energy_info();
        if (i == 10)
        {
            ud_hid_deal();
            i = 0;
        }
        if ((g_vol_sync_timer != 0) && ((sys_get_ab_timer() - g_vol_sync_timer) >= 50))
        {
            vol_dif = (int8)(g_cur_play_status_p->change_vol_val - g_current_vol);

            if(g_pre_dif == vol_dif)
            {
                g_vol_no_chg ++;
            }
            
            g_pre_dif = vol_dif;
            
            if((g_vol_chg_cnt >= 10) || (g_vol_no_chg >= 2))   //防止死循环，兼容winxp
            {
                g_vol_sync_timer = 0;
                g_vol_chg_cnt = 0;
                g_vol_no_chg = 0;
                uengine_sync_volume(g_current_vol,1);
                sys_os_time_dly(1);
                continue;
            }
            
            if ((g_force_equ == 0) && ((vol_dif == 1) || ((0 - vol_dif) == 1) || (vol_dif == 0)))
            {
                g_vol_sync_timer = 0;
                g_vol_chg_cnt = 0;
                g_vol_no_chg = 0;
            }
            else if (vol_dif != 0)
            {
                uengine_vol_sync((uint8)g_current_vol);
            }
            else    //vol_dif == 0
            {
                g_vol_sync_timer = 0;
                g_vol_chg_cnt = 0;
                g_vol_no_chg = 0;
            }
            
            g_vol_chg_cnt ++;
        }
        
        if (g_auto_play_count >= AUTO_PLAY_DELAY)
        {
            if((g_status_data.start_play == 0) && (g_auto_play_flag == 1) && (g_data_flag == 0))
            {
                ud_set_cmd(SET_HID_OPERS, 0x08);
                
                ud_hid_deal();

                libc_print("auto play",0,0);
            }
            
            g_auto_play_count = 0;
            
            g_auto_play_flag = 0;
            
            g_switch_song_flag = 0x01;
        }
        //挂起10ms 多任务交互
        sys_os_time_dly(1);
        
        if((g_switch_song_flag == 0)&&(g_auto_play_flag == 1))
        {
            g_auto_play_count++;
        }
        else
        {
            g_auto_play_count = 0;
        }
        
        i++;
/*#ifdef __ESD_MODE_
        if(g_auto_play_flag == 0 && check_auto_play_time <=2 )
        {
            esd_count_time++;
            if(esd_count_time >= 500)
            {
                auto_play_for_esd(&check_auto_play_time);
                esd_count_time = 0;
            }
        }
#endif*/
    }

    //kill_app_timer(g_hid_timer);

    return uengine_retval;
}
/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e uengine_reply_msg(void* msg_ptr, bool ret_vals)
 * \回复同步消息
 * \param[in]    void  msg_ptr 消息结构体
 * \param[in]    bool  ret_vals 相关事件处理结果 TRUE 成功 FALSE 失败
 * \param[out]   none
 * \return       app_result_E
 * \retval       RESULT_IGNOR 忽略
 * \ingroup      uengine_control.c
 * \note  回复消息时，根据ret_vals的结果，设置应答成功或失败
 */
/*******************************************************************************/
void uengine_reply_msg(void* msg_ptr, bool ret_vals)
{
    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    if (ret_vals == TRUE)
    {
        data_ptr->reply->type = MSG_REPLY_SUCCESS;
    }
    else
    {
        data_ptr->reply->type = MSG_REPLY_FAILED;
    }

    //回复同步消息(发送信号量)
    libc_sem_post(data_ptr->sem);

    //return;
}

