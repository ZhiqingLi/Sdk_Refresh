#include "app_linein_eg.h"

void linein_eg_vram_write(uint8 save_disk)
{
}

void linein_eg_vram_read(uint8 restore_disk)
{
}

app_result_e linein_eg_clear_error(void* msg_ptr)
{
    //返回成功
    linein_eg_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

//播报TTS, 暂停播放LineIn
app_result_e linein_eg_tts_start(void* msg_ptr)
{
    //设置播放类型的指针
    //private_msg_t* data_ptr = (private_msg_t*) msg_ptr;
    bool ret_vals = TRUE;

    if ((g_eg_status_p->play_status == linein_play_status_play))
    {
        //先暂停播放
        if (g_linein_channel_sel == LINEIN_CH_A)
        {
            ret_vals = linein_eg_aa_stop();
        }
        else
        {
            ret_vals = stop(); //暂停播放
        }

        //先暂停播放
        //ret_vals = stop();//暂停播放
        if (ret_vals == TRUE)
        {
            g_stop_by_tts_flag = TRUE;
        }
    }

    linein_eg_reply_msg(msg_ptr, ret_vals);

    return RESULT_IGNORE;
}

//播报TTS完成, 恢复播放LineIn
app_result_e linein_eg_tts_stop(void* msg_ptr)
{
    //设置播放类型的指针
    //private_msg_t* data_ptr = (private_msg_t*) msg_ptr;
    bool ret_vals = TRUE;

    if (g_stop_by_tts_flag == TRUE)
    {
        if (g_linein_channel_sel == LINEIN_CH_A)
        {
            ret_vals = linein_eg_aa_play();
        }
        else
        {
            ret_vals = play(); //续播
        }
        g_stop_by_tts_flag = FALSE;
    }

    linein_eg_reply_msg(msg_ptr, ret_vals);

    return RESULT_IGNORE;
}

//STANDBY命令响应
app_result_e linein_eg_standby(void * msg_ptr)
{
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;
    os_event_t *standby_sem = (os_event_t *)(data_ptr->msg.content.addr);

    //备份现场，准备进入睡眠模式
    PRINT_INFO("engine standby enter");

    //回复前台，然后接着跑就会睡眠
    linein_eg_reply_msg(data_ptr, TRUE);

    libc_sem_wait(standby_sem, 0);

    //从睡眠模式回来，恢复现场
    PRINT_INFO("engine standby exit");
    standby_restart_all_app_timer(g_app_timer_vector, g_app_timer_count);

    return RESULT_IGNORE;
}

