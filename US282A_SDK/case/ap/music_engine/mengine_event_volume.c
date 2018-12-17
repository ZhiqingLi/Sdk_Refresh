#include "app_mengine.h"
app_result_e mengine_set_eq(void* msg_ptr)
{
    //private_msg_t * data_ptr = (private_msg_t*) msg_ptr;
    //dae_config_t *p_dae_cfg = (dae_config_t *)(data_ptr->msg.content.addr);

    adjust_freq_add_value(0, g_dae_cfg_shm->run_freq);

    if (NULL != g_mp_handle)
    {
        mmm_mp_cmd(g_mp_handle, MMM_MP_SET_EQ, (unsigned int) NULL);
    }
    //·µ»Ø³É¹¦
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}
