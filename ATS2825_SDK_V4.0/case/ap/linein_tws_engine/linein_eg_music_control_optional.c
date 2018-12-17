/********************************************************************************
 *                            USDK(US282A_171204_GL5116_DVB_SVN13422_for_opus)
 *                            Module: SYSTEM
 *                 Copyright(c) 2003-2017 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>      <time>                      <version >          <desc>
 *      wuyufan   2017年12月4日-下午2:00:34             1.0             build this file
 ********************************************************************************/
/*!
 * \file     btplay_eg_music_control_force.c
 * \brief    强制装载中间件
 * \author
 * \version  1.0
 * \date  2017年12月4日-下午2:00:34
 *******************************************************************************/

#include "linein_engine.h"

#if 0
void a2dp_set_codec_type(uint32 media_type)
{
    mmm_bp_set_type_param_t type_pram;

    if(media_type == A2DP_CODEC_SBC) {
        type_pram.music_type = TYPE_SBC;

#ifdef ENABLE_TRUE_WIRELESS_STEREO
        type_pram.work_mode = (g_p_bt_stack_cur_info->dev_role | 0xf0);
#else
        type_pram.work_mode = 0;
#endif
        type_pram.play_delay_start = btplay_gl_var.sbc_delay_start;
        type_pram.play_delay_lowest = btplay_gl_var.sbc_delay_lowest;
    } else if(media_type == A2DP_CODEC_AAC) {
        type_pram.music_type = TYPE_AAC;
        type_pram.play_delay_start = btplay_gl_var.aac_delay_start;
        type_pram.play_delay_lowest = btplay_gl_var.aac_delay_lowest;
    } else if(media_type == A2DP_CODEC_MPEG12) {
        type_pram.music_type = TYPE_MP3;
        type_pram.play_delay_start = 161;
        type_pram.play_delay_lowest = 161;
    } else {
        PRINT_ERR("btplay engine error media type!!");
        while(1) {
            ; //nothing for QAC
        }
    }

    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);
#ifdef ENABLE_TRUE_WIRELESS_STEREO
    if(g_p_bt_stack_cur_info->dev_role != 0) {
        if(type_pram.music_type == TYPE_SBC) {
            adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL8, FREQ_LEVEL6);
            PRINT_INFO("level8:");
        } else if(type_pram.music_type == TYPE_AAC) {
            adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL8, FREQ_LEVEL3);
        } else {
            //TYPE_MP3
            adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL8, FREQ_LEVEL3);
        }
    } else
#endif
    {
        if(type_pram.music_type == TYPE_SBC) {
            adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL5, FREQ_LEVEL3);
        } else if(type_pram.music_type == TYPE_AAC) {
            adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL5, FREQ_LEVEL3);
        } else { //TYPE_MP3
            adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL5, FREQ_LEVEL3);
        }
    }

    adjust_freq_add_value(0, g_dae_cfg_shm->run_freq);

    mmm_bp_cmd(mp_handle, MMM_BP_SET_MUSIC_TYPE, (unsigned int) &type_pram);
}



void a2dp_decode_init(void)
{
    int result;

    com_set_sound_out(SOUND_OUT_START);

    a2dp_set_codec_type(btplay_gl_var.media_type);

    //设置这个标志表示需要重对齐，这样就需要重新加载dsp库
    if(btplay_gl_var.codec_flag == FALSE) {
#ifdef WAVES_ASET_TOOLS
        //发送播放命令
        g_waves.bin_number = g_app_info_state_all.bin_number;
        result = mmm_bp_cmd(mp_handle, MMM_BP_PLAY, (unsigned int) &g_waves);
#else
        //发送播放命令
        result = mmm_bp_cmd(mp_handle, MMM_BP_PLAY, (unsigned int) NULL);
#endif

        if(result == 0) {
            PRINT_INFO("btplay mmm init");
#ifdef ENABLE_TRUE_WIRELESS_STEREO
            uint32 temp_flag;
            temp_flag = sys_local_irq_save();
            g_p_tws_info->mmm_init_flag = 1;
            sys_local_irq_restore(temp_flag);
#endif
            mmm_bp_cmd(mp_handle, MMM_BP_GET_ENERGY_INFO, (unsigned int) & (g_app_info_state_all.p_energy_value));
            g_app_info_state_all.energy_available_flag = TRUE;
        }

        btplay_gl_var.codec_flag = TRUE;
//      g_p_btplay_info->status = BTPLAY_PAUSE;
    }

    PRINT_INFO("btplay mmm open");
}


void a2dp_decode_quit(void)
{
    uint32 temp_flag;

    com_set_sound_out(SOUND_OUT_STOP);

    btplay_gl_var.codec_flag = FALSE;

#ifdef ENABLE_TRUE_WIRELESS_STEREO
    //卸载中间件,提前关闭bt wake host中断
    if(g_p_bt_stack_cur_info->dev_role != 0) {
        temp_flag = sys_local_irq_save();

        act_writel(act_readl(INTC_MSK) & 0xfffffffe, INTC_MSK);
        act_writel(act_readl(BIAS_CTL) & 0xfffffffb, BIAS_CTL);
        act_writel(0x00000005, WAKE_PD); //clear bt onoff pending
        g_p_tws_info->mmm_init_flag = 0;
        g_p_tws_info->start_flag = 0;
        sys_local_irq_restore(temp_flag);
    }
#endif
#ifdef WAVES_ASET_TOOLS
    mmm_bp_cmd(mp_handle, MMM_BP_STOP, (unsigned int) &g_waves);
#else
    mmm_bp_cmd(mp_handle, MMM_BP_STOP, (unsigned int) NULL);
#endif

    PRINT_INFO_INT("btplay mmm close", __builtin_return_address(0));
}
#endif

