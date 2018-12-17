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
 * \int32 uegine_player_open(void)
 * \打开中间件
 * \param[in]    void
 * \param[out]   none
 * \return       none
 * \retval       none
 * \ingroup      uengine_control.c
 * \note
 */
/*******************************************************************************/
int32 uegine_player_open(void)
{
    int open_ret = 0;
    if(0 == g_uspeaker_24bit)
    {
        //bool init_fsel_ret;
        //char *mmm_name =
        //{ "mmm_pp.al" };

        //加载中间件驱动
        //sys_load_mmm(mmm_name, TRUE);//待完善

        if (g_mmm_pp_handle != NULL)
        {
            return 0;
        }

        adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL10, FREQ_LEVEL10);

        adjust_freq_add_value(0, g_dae_cfg_shm->run_freq);

        //初始化中间件库
        open_ret = mmm_pp_cmd(&g_mmm_pp_handle, MMM_PP_OPEN, (unsigned int) NULL);

        if(1 == g_app_info_state_all.fix_sample_rate_flag)
        {
            mmm_pp_cmd(&g_mmm_pp_handle, MMM_PP_FIX_SAMPLE_RATE, (unsigned int) NULL);
        }

        if (open_ret != -1)
        {
            open_ret = mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_AINOUT_OPEN, (unsigned int) &g_ain_out_setting);
        }

        //输出声道处理选择
        //#define OUTPUT_CHANNEL_NORMAL      0 //正常，中间件会初始化为正常
        //#define OUTPUT_CHANNEL_L_R_SWITCH  1 //左右声道互换 L = R0, R = L0
        //#define OUTPUT_CHANNEL_L_R_MERGE   2 //左右声道合并后输出到左声道和右声道 L = R = (L0 + R0) / 2
        //#define OUTPUT_CHANNEL_L_R_ALL_L   3 //左右声道都输出左声道 L = R = L0
        //#define OUTPUT_CHANNEL_L_R_ALL_R   4 //左右声道都输出右声道 L = R = R0
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_SET_OUTPUT_CHANNEL, (unsigned int) g_output_channel_config);

#ifdef WAVES_ASET_TOOLS
        g_waves.bin_number = g_app_info_state_all.bin_number;
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_PLAY, (unsigned int) &g_waves);
#else
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_PLAY, (unsigned int) NULL);
#endif

        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_GET_ENERGY_INFO, (unsigned int) &(g_app_info_state_all.p_energy_value));

        g_app_info_state_all.energy_available_flag = TRUE;

        //start get data timer
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_START_SEND, (unsigned int) NULL);

        //ud_set_cmd(SET_ADJUST_TIMER, 1);
    }
    else
    {
        ud_set_cmd(SET_USPEAKER_24BIT,USPEAKER24_SAMPLE_RATE);
        ud_set_cmd(SET_USPEAKER_24BIT,USPEAKER24_PLAY);
    }
    return open_ret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \void uegine_player_close(void)
 * \关闭中间件
 * \param[in]    void
 * \param[out]   none
 * \return       none
 * \retval       none
 * \ingroup      uengine_control.c
 * \note
 */
/*******************************************************************************/

void uegine_player_close(void)
{
    if(0 == g_uspeaker_24bit)
    {
        uint32 fade_out_use = 0;

        //ud_set_cmd(SET_ADJUST_TIMER, 0);

        if (g_mmm_pp_handle == NULL)
        {
            return;
        }

        g_app_info_state_all.energy_available_flag = FALSE;

        if (g_status_data.start_play == 0)
        {
            mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_FADE_OUT_TIME, (unsigned int) fade_out_use);
        }
#ifdef WAVES_ASET_TOOLS
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_STOP, (unsigned int) &g_waves);
#else
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_STOP, (unsigned int) NULL);
#endif
        //stop get data timer
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_STOP_SEND, (unsigned int) NULL);

        //关闭声音设备
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_AINOUT_CLOSE, (unsigned int) NULL);

        //关闭中间件设备
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_CLOSE, (unsigned int) NULL);

        g_mmm_pp_handle = NULL;

        adjust_freq_add_value(0, 0);

        adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_NULL, FREQ_NULL);

        //卸载解码驱动
        //sys_free_mmm(TRUE);
    }
    else
    { 
        ud_set_cmd(SET_USPEAKER_24BIT,USPEAKER24_PAUSE);
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 * \void uengine_status_deal(void)
 * \引擎状态处理,处理当前音乐播放状态
 * \param[in]    void  para1
 * \param[out]   none
 * \return       void the result
 * \retval       none
 * \ingroup      uengine_control.c
 * \note
 */
/*******************************************************************************/
void uengine_status_deal(void)
{
    ;//do nothing
}
/******************************************************************************/
/*!
 * \par  Description:
 * \void uegine_check_status(void)
 * \检测引擎状态定时器函数，设置标志位
 * \param[in]    void
 * \param[out]   none
 * \return       none
 * \retval       none
 * \ingroup      uengine_control.c
 * \note
 */
/*******************************************************************************/
void uegine_check_status(void)
{
    g_check_status_flag = TRUE;
}

