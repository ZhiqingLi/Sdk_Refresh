/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：音量管理接口，包括音频输出切换接口，喇叭与耳机切换。
 * 作者：cailizhen
 ********************************************************************************/

#include "psp_includes.h"
#include "common_func.h"

//切换音频输出，发送同步消息让解码暂停一下
static bool SPEAKER_OUT_SWITCH_SEND_MSG(bool start_or_stop)
{
    bool bret = TRUE;

    //发送同步消息
    if (get_engine_type() != ENGINE_NULL)
    {
        msg_apps_t msg;

        if (start_or_stop == TRUE)
        {
            msg.type = MSG_SWITCH_AOUT_START_SYNC;
        }
        else
        {
            msg.type = MSG_SWITCH_AOUT_STOP_SYNC;
        }

        bret = (bool) (send_sync_msg(APP_ID_THEENGINE, &msg, NULL, 0) == ERR_NO_ERR);
    }

    return bret;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    SPEAKER 输出切换
 * \param[in]    hp_in 0表示Headphone拔出切换输出，1表示Headphone插入切换输出
 * \param[out]   none
 * \return       none
 *******************************************************************************/
void SPEAKER_OUT_SWITCH(uint8 hp_in)
{
    bool outer_pa_on_bk = g_app_info_state.outer_pa_on;

    PRINT_INFO("start swtich aout");

    if (SPEAKER_OUT_SWITCH_SEND_MSG(TRUE) == FALSE)
    {
        PRINT_ERR("aout switch error!!");
    }

    if (hp_in == 1)
    {
        if (g_app_info_state.aout_mode == AO_SOURCE_I2S)
        {
            disable_pa(AO_SOURCE_I2S);
        }

        if (g_app_info_state.outer_pa_on == TRUE)
        {
            SPEAKER_OFF();
        }
        SPEAKER_CONTROL_DISABLE();

        if (g_app_info_state.aout_mode == AO_SOURCE_I2S)
        {//切换为DAC Headphone输出
            uint8 pa_mode = (uint8) com_get_config_default(SETTING_AUDIO_INNER_PA_SET_MODE);
            enable_pa(pa_mode, 0, AO_SOURCE_DAC);
        }

        g_app_info_state.aout_mode = AO_SOURCE_DAC;

        //禁用DAE
        com_set_dae_onoff(FALSE);

        //更新DAE参数后，需要重新设置音量
        com_reset_sound_volume(0);

        g_app_info_state.outer_pa_on = outer_pa_on_bk;
    }
    else
    {
        g_app_info_state.aout_mode = (aout_type_e) com_get_config_default(SETTING_AUDIO_OUTPUT_CHANNAL);

        if (g_app_info_state.aout_mode == AO_SOURCE_I2S)
        {
            disable_pa(AO_SOURCE_DAC);
        }

        SPEAKER_CONTROL_ENABLE();

        if (g_app_info_state.aout_mode == AO_SOURCE_I2S)
        {//切换为I2S输出
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
            //DEMO IC:A0,A1,A2,VRO_S
            enable_pa(3, 0, AO_SOURCE_I2S);
#elif (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825)
            //DVB:B7,B8,B9,A6
            enable_pa(4, 0, AO_SOURCE_I2S);
#elif (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823)
            //DVB:AOUTL,AOUTR,VRO,VROS
            enable_pa(0, 0, AO_SOURCE_I2S);
#else
            //DEMO IC:A0,A1,A2,A3
            enable_pa(1, 0, AO_SOURCE_I2S);
#endif
        }

        if (outer_pa_on_bk == TRUE)
        {
            SPEAKER_ON();
        }

        //恢复DAE
        com_set_dae_onoff(TRUE);

        //更新DAE参数后，需要重新设置音量
        com_reset_sound_volume(0);
    }

    if (SPEAKER_OUT_SWITCH_SEND_MSG(FALSE) == FALSE)
    {
        PRINT_ERR("aout switch error!!");
    }

    PRINT_INFO("finish swtich aout");
}
