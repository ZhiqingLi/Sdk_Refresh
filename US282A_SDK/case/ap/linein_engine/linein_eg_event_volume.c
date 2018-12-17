#include "app_linein_eg.h"

app_result_e linein_eg_set_eq(void* msg_ptr)
{
    //private_msg_t * data_ptr = (private_msg_t*) msg_ptr;
    //dae_config_t *p_dae_cfg = (dae_config_t *)(data_ptr->msg.content.addr);
    adjust_freq_add_value(0, g_dae_cfg_shm->run_freq);

    if ((g_linein_channel_sel != LINEIN_CH_A)&&(NULL != g_mmm_pp_handle))
    {
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_SET_EQ, (unsigned int) NULL);
    }

    //·µ»Ø³É¹¦
    linein_eg_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}
