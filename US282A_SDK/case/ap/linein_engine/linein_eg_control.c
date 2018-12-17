#include "app_linein_eg.h"

//引擎状态处理
void linein_eg_status_deal(void)
{
    if (g_linein_channel_sel == LINEIN_CH_A)
    {
    }
    else
    {
        mmm_pp_status_t play_status;
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_GET_STATUS, (unsigned int) &play_status); //获取后暂时没用
    }

    if (g_eg_status_p->play_status == linein_play_status_stop)
    {
        change_engine_state(ENGINE_STATE_PAUSE);
    }
    else
    {
        change_engine_state(ENGINE_STATE_PLAYING);
    }
}

//引擎的消息处理循环
app_result_e linein_eg_control_block(void)
{
    private_msg_t cur_msg_val;

    app_result_e linein_eg_retval = RESULT_NULL;

    //uint32 msg_type;

    //g_linein_eg_result = RESULT_NULL;

    while (1)
    {
        //更新共享查询
        g_eg_status_p = sys_share_query_update(APP_ID_LINEIN_EG);

        //获取中间件当前播放状态
        linein_eg_status_deal();

        //获取并处理详细
        if (get_app_msg(&cur_msg_val) == TRUE)
        {
            linein_eg_retval = linein_eg_message_done_bank(&cur_msg_val);
        }

        if (linein_eg_retval == RESULT_APP_QUIT)
        {
            if (g_linein_channel_sel == LINEIN_CH_A)
            {
                linein_eg_aa_stop();
            }
            else
            {
                //退出应用关闭处理
                stop();
            }
            break;
        }

        //挂起20ms 多任务交互
        sys_os_time_dly(2);
    }

    return linein_eg_retval;
}

void linein_eg_reply_msg(void* msg_ptr, bool ret_vals)
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
}

