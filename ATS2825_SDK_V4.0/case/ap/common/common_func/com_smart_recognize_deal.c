/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙推歌前台应用用户交互实现，包括按键事件和系统事件处理。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"


typedef int (*mmm_cmd_func)(void *, mmm_bp_cmd_e, unsigned int);


#if defined(SMART_BOX_OPUS_MODE) || defined(SMART_BOX_BUF_MODE)

void smart_tws_sync_cmd_send(uint8 val)
{

    bt_stack_info_t *p_bt_stack_cur_info = (bt_stack_info_t *) sys_shm_mount(SHARE_MEM_ID_BTSTACK_STATUS);

    if(p_bt_stack_cur_info->dev_role != 0) {
        msg_apps_t msg;
        msg.type = MSG_BTSTACK_TWS_SYNC_CMD;
        msg.content.data[0] = val;
        send_sync_msg_btmanager(NULL, &msg, NULL, 0);
    }
}

void smart_set_ainout_param(mmm_com_ainout_setting_t *set_ain_out, uint8 smart_mode)
{
    set_ain_out->input_type = 0;
    set_ain_out->adc_insel = (uint8) com_get_config_default(SETTING_AUDIO_INPUT_CHANNAL);

    if(smart_mode == SMART_MODE_BUF) {
        set_ain_out->asrc_index = K_OUT0_U0_IN_U1;
        set_ain_out->dac_sample = 16;
        set_ain_out->dac_chanel = 0;
        set_ain_out->asrc_mode_sel = 1;
        set_ain_out->adc_sample = 16;
    } else {
        set_ain_out->asrc_index = K_OUT1_P2_IN_U1;
#if defined(BOARD_DPI_7C370_FAR) || defined(BOARD_JL_36410_FAR)
        set_ain_out->dac_sample = 16;           //set i2s samplerate to 16KHz
#else
        set_ain_out->dac_sample = 48;
#endif
        set_ain_out->dac_chanel = 1;
        if(set_ain_out->adc_insel == AI_SOURCE_I2S) {
            set_ain_out->asrc_mode_sel = 3;
#if defined(BOARD_DPI_7C370_FAR) || defined(BOARD_JL_36410_FAR)
            set_ain_out->adc_sample = 16;       //set i2s samplerate to 16KHz
#else
            set_ain_out->adc_sample = 48;
#endif
        } else {
            set_ain_out->asrc_mode_sel = 2;
            set_ain_out->adc_sample = 16;
        }
    }

    set_ain_out->ain_gain = 7;
    set_ain_out->adc_gain = 1;
    set_ain_out->output_chan = (uint8) com_get_config_default(VOICE_RECONGNITION_OUTPUT_CHANNEL_CONFIG);
}

void smart_enter_speech_mode(uint8 *speech_ctl_flag)
{
    if(speech_ctl_flag != NULL) {
        *speech_ctl_flag = TRUE;
    }

}

void smart_exit_speech_mode(uint8 *speech_ctl_flag)
{
    if(speech_ctl_flag != NULL) {
        *speech_ctl_flag = FALSE;
    }
}

app_result_e smart_process_message_deal(private_msg_t *pri_msg, void *mmm_hdl, mmm_cmd_func func_ptr,
                                        uint8 *tws_speech_flag)
{

    mmm_cmd_func mmm_cmd_deal = func_ptr;
    switch(pri_msg->msg.type) {
        case MSG_PLAYEREG_RECOGNIZE_PAUSE_MUSIC:
            PRINT_INFO("recognize pause music");
            smart_enter_speech_mode(tws_speech_flag);
            {
                if(mmm_cmd_deal(mmm_hdl, MMM_SPEECH_PAUSE_MUSIC_PLAY, NULL) == 0) {
                    pri_msg->reply->type = MSG_REPLY_SUCCESS;
                } else {
                    pri_msg->reply->type = MSG_REPLY_FAILED;
                }
                //for tell slave to stop play and prepare replay enviroment
                smart_tws_sync_cmd_send(TWS_MASTER_SMART_MODE);
            }
            break;

        case MSG_PLAYEREG_RECOGNIZE_RESUME_MUSIC:
            PRINT_INFO("recognize resume music");
            {
                mmm_cmd_deal(mmm_hdl, MMM_SPEECH_RESUME_MUSIC_PLAY, NULL);
            }
            smart_exit_speech_mode(tws_speech_flag);
            break;

        //for voice recognize deal
        case MSG_PLAYEREG_RECOGNIZE_START:
            PRINT_INFO("recognize start");

            //smart_enter_speech_mode(tws_speech_flag);
            {
                mmm_com_ainout_setting_t param_ain_out;
                smart_set_ainout_param(&param_ain_out, pri_msg->msg.content.data[0]);
                param_ain_out.output_chan = pri_msg->msg.content.data[1];
                if(param_ain_out.adc_insel == AI_SOURCE_I2S) {
                    i2s_mfp_cfg();
                }
                mmm_cmd_deal(mmm_hdl, MMM_SPEECH_RECONGNITION_START, (unsigned int) &param_ain_out);
            }

            break;
        case MSG_PLAYEREG_RECOGNIZE_STOP:
            PRINT_INFO("recognize stop");
            {
                uint32 recognize_mode;
                uint8 adc_sel;
                if(pri_msg->msg.content.data[0] != SMART_MODE_BUF) {
                    recognize_mode = 0;
                    adc_sel = (uint8) com_get_config_default(SETTING_AUDIO_INPUT_CHANNAL);
                    if(adc_sel == AI_SOURCE_I2S) {
                        recognize_mode = 1;
                    }
                } else {
                    recognize_mode = 2;
                }
                smart_tws_sync_cmd_send(TWS_MASTER_EXIT_SMART_MODE);
                if(mmm_cmd_deal(mmm_hdl, MMM_SPEECH_RECONGNITION_STOP, (unsigned int)recognize_mode) == 0) {
                    pri_msg->reply->type = MSG_REPLY_SUCCESS;
                } else {
                    pri_msg->reply->type = MSG_REPLY_FAILED;
                }
            }
            //smart_exit_speech_mode(tws_speech_flag);

            break;
#ifdef SMART_BOX_BUF_MODE
        case MSG_PLAYEREG_RECOGNIZE_DOING:
            mmm_cmd_deal(mmm_hdl, MMM_SPEECH_REC_BUF_TRANS, (unsigned int)NULL);
            break;

#endif //#ifdef SMART_BOX_BUF_MODE 

    }
    return RESULT_NULL;



}


#endif //#if defined(SMART_BOX_OPUS_MODE) || defined(SMART_BOX_BUF_MODE)


