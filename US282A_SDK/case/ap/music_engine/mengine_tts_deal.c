#include "app_mengine.h"

//关闭中间件,销毁中间件线程
int32 close_mmm(void)
{
    if(g_mmm_opened == TRUE)
    {
        mmm_mp_cmd(g_mp_handle, MMM_MP_CLOSE, (unsigned int) NULL);
        g_mmm_opened = FALSE;
    }
    return 0;
}

int32 open_mmm(void)
{
    int32 open_ret;
    if(g_mmm_opened == TRUE)
    {
        return 0;
    }
    open_ret = mmm_mp_cmd(&g_mp_handle, MMM_MP_OPEN, (unsigned int) NULL);
    if((open_ret != -1)&&(1 == g_app_info_state_all.fix_sample_rate_flag))
    {
        mmm_mp_cmd(&g_mp_handle, MMM_MP_FIX_SAMPLE_RATE, NULL);
    }
    g_mmm_opened = TRUE;
    
    return open_ret;
}

app_result_e mengine_tts_start(void* msg_ptr)
{
    //设置播放类型的指针
    //private_msg_t* data_ptr = (private_msg_t*) msg_ptr;
    bool ret_vals = TRUE;

    if ((g_eg_status_p->play_status == PlayFast) 
        || (g_eg_status_p->play_status == PlaySta)
        || (g_eg_status_p->play_status == PlayAB))
    {
        //先暂停播放
        ret_vals = stop(STOP_PAUSE); //暂停播放
        if (ret_vals == TRUE)
        {
            g_stop_by_tts_flag = TRUE;
        }
    }
    close_mmm();

    mengine_reply_msg(msg_ptr, ret_vals);

    return RESULT_IGNORE;
}

app_result_e mengine_tts_stop(void* msg_ptr)
{
    //设置播放类型的指针
    //private_msg_t* data_ptr = (private_msg_t*) msg_ptr;
    bool ret_vals = TRUE;

    open_mmm();

    if (g_stop_by_tts_flag == TRUE)
    {

        if (g_eg_status_p->err_status == EG_ERR_NONE)
        {
            ret_vals = play(PLAY_RESUME); //续播
        }
        g_stop_by_tts_flag = FALSE;
    }

    mengine_reply_msg(msg_ptr, ret_vals);

    return RESULT_IGNORE;
}

//STANDBY命令响应
app_result_e mengine_standby(void* msg_ptr)
{
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;
    os_event_t *standby_sem = (os_event_t *)(data_ptr->msg.content.addr);

    //备份现场，准备进入睡眠模式
    PRINT_INFO("engine standby enter");

    //回复前台，然后接着跑就会睡眠
    mengine_reply_msg(data_ptr, TRUE);

    libc_sem_wait(standby_sem, 0);

    //从睡眠模式回来，恢复现场
    PRINT_INFO("engine standby exit");
    standby_restart_all_app_timer(g_app_timer_vector, g_app_timer_count);

    return RESULT_IGNORE;
}

