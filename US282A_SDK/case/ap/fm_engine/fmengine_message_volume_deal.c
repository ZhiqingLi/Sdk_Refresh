#include "app_fmengine.h"

app_result_e fmengine_set_eq(private_msg_t* msg_ptr)
{
    //dae_config_t *p_dae_cfg = (dae_config_t *)(msg_ptr->msg.content.addr);
    adjust_freq_add_value(0, g_dae_cfg_shm->run_freq);

    if ((g_fm_chanel == 1)&&(NULL != g_mmm_pp_handle))
    {
        //设置当前eq到解码器
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_SET_EQ, (unsigned int) NULL);
    }

    //返回成功
    msg_ptr->reply->type = MSG_REPLY_SUCCESS;
    //回复同步消息(发送信号量)
    fm_eg_reply_msg(msg_ptr);

    return RESULT_IGNORE;
}
