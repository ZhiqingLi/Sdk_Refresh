#include "app_mengine.h"

typedef enum _SPECIAL_SWITCH_MODE
{
    SWITCH_BY_NO, //数字点歌
    SWITCH_NEXT_DIR = 0x80, //下一个目录
    SWITCH_PRE_DIR = 0x80, //上一个目录
}
SPECIAL_SWITCH_MODE;

//切换到上下曲
static app_result_e mengine_play_switch(void *msg_ptr, switch_mode_e switch_mode)
{
    bool bret;
    //play_mode_e play_mode;//保存是否正在播放
    switch_result_e switch_ret; //切换是否成功

    //升频
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL10, FREQ_NULL);

    if (g_eg_status_p->play_status != PlaySta) //正在播放
    {
        //清除断点
        libc_memset(&(g_eg_cfg_p->bk_infor), 0, sizeof(mmm_mp_bp_info_t));
    }

    switch_ret = mengine_file_switch(STOP_NORMAL, switch_mode, PLAY_NO_PLAY);

    if (switch_ret == SWITCH_NO_ERR)
    {
        bret = TRUE;
    }
    else
    {
        bret = FALSE;
    }

    mengine_reply_msg(msg_ptr, bret);

    //降频
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);

    return RESULT_IGNORE;

}

//切换到下一曲
app_result_e mengine_play_next(void* msg_ptr)
{
    if (g_mengine_enter_mode == ENTER_RECOD_PLAY)
    {
        return mengine_play_switch(msg_ptr, NORMAL_SWITCH_NEXT);
    }
    else
    {
        return mengine_play_switch(msg_ptr, FORCE_SWITCH_NEXT);
    }
}

//切换到上一曲
app_result_e mengine_play_prev(void* msg_ptr)
{
    play_status_e play_status = g_eg_status_p->play_status;
    bool bret = TRUE;

    if ((g_music_replay_by_prev_key == TRUE) && (g_eg_playinfo_p->cur_time > 5000))
    {
        g_eg_playinfo_p->cur_time = 0;

        //清除断点
        libc_memset(&(g_eg_cfg_p->bk_infor), 0, sizeof(mmm_mp_bp_info_t));

        if (play_status == PlaySta)
        {
            //停止播放
            bret = stop(STOP_NORMAL);

            //从头开始播放
            if (bret == TRUE)
            {
                //play(PLAY_NORMAL);
            }
            g_eg_playinfo_p->cur_file_switch = 1;
        }
        if (bret == TRUE)
        {
            mengine_reply_msg(msg_ptr, bret);
            return RESULT_IGNORE;
        }
    }

    //return mengine_play_switch(msg_ptr, FORCE_SWITCH_PREV);
    if (g_mengine_enter_mode == ENTER_RECOD_PLAY)
    {
        return mengine_play_switch(msg_ptr, NORMAL_SWITCH_PREV);
    }
    else
    {
        return mengine_play_switch(msg_ptr, FORCE_SWITCH_PREV);
    }
}

//实现文件切换, 根据文件号、或者目录切换
bool mengine_special_switch(uint8 direct, uint16 dest_num)
{
    bool ret;

    //停止当前歌曲播放
    ret = stop(STOP_NORMAL);
    if (ret == FALSE)
    {
        return ret;
    }

    //检测磁盘是否存在
    ret = mengine_check_disk_in();
    //如果已经拔出磁盘
    if (ret == FALSE)
    {
        //保存错误状态
        g_eg_status_p->err_status = EG_ERR_DISK_OUT;

        return ret;
    }

    //升频
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL10, FREQ_NULL);

    //是否可以选择列表
    if ((g_list_access_allow == TRUE) && (g_mengine_enter_mode != ENTER_RECOD_PLAY))
    {
        //只设置一次
        fsel_set_seltype(FSEL_TYPE_LISTDIR, fsel_get_fileno());
        g_list_access_allow = FALSE;
    }

    //强制切换
    fsel_set_mode(g_shuffle_flag | FSEL_MODE_LOOPALL);

    if (direct == SWITCH_BY_NO)
    {
        //数字点歌
        ret = fsel_get_byno((void*) g_eg_cfg_p->location.dirlocation.file_info.file_extend_info.file_ext, dest_num);
    }
    else if (direct == SWITCH_NEXT_DIR)
    {
        //目录向后
        ret = fsel_get_nextdir((void*) g_eg_cfg_p->location.dirlocation.file_info.file_extend_info.file_ext);
    }
    else
    {
        //目录向前
        ret = fsel_get_prevdir((void*) g_eg_cfg_p->location.dirlocation.file_info.file_extend_info.file_ext);
    }

    //恢复循环模式
    fsel_set_mode(g_shuffle_flag | (uint8)g_repeat_mode);

    //获取当前歌曲的location
    if (ret == TRUE)
    {
        fsel_get_location(&(g_eg_cfg_p->location.dirlocation));

        if (direct != 0)
        {
            g_eg_playinfo_p->cur_file_switch |= 0x01;
        }
        //清除断点信息
        libc_memset(&g_eg_cfg_p->bk_infor, 0, sizeof(mmm_mp_bp_info_t));

#ifdef MENGINE_ID3_SUPPORT
        mengine_getid3_done(&g_eg_cfg_p->location);
#endif
#ifdef DIR_NAME_SUPPORT
        fsel_get_dir_name(g_dir_name_buf, sizeof(g_dir_name_buf) / 2 - 2, &(g_eg_cfg_p->location.dirlocation));
#endif
        vfs_get_name(g_file_sys_id, g_eg_cfg_p->name_buf,
                sizeof(g_eg_cfg_p->name_buf) / 2 - 2);
#ifdef CHECK_BK_DIR
        fsel_get_dir_name(g_eg_cfg_p->dir_name, sizeof(g_eg_cfg_p->dir_name) / 2 - 2,
                &g_eg_cfg_p->location.dirlocation);
#endif
    }
    else
    {
        libc_memset(g_eg_cfg_p->name_buf, 0, sizeof(g_eg_cfg_p->name_buf));
#ifdef CHECK_BK_DIR
        libc_memset(g_eg_cfg_p->dir_name, 0, sizeof(g_eg_cfg_p->dir_name));
#endif
    }
    if (g_mengine_enter_mode != ENTER_RECOD_PLAY)
    {
        //正常切换也需要断点记忆,用于断电恢复
        mengine_vram_write(g_eg_cfg_p->location.dirlocation.disk);
    }

    //普通歌曲切换时当前时间先清零
    g_eg_playinfo_p->cur_time = 0;

    //if (direct == 0)
    //{
    //  //只要切换歌曲就会开始播放
    //  ret = play(PLAY_RESUME);
    //}

    //降频
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);

    return ret;
}

//数字点歌
app_result_e mengine_play_digital_song(void* msg_ptr)
{
    bool ret_val;
    //点歌数字的指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;
    uint16 song_number = *(uint16*) (data_ptr->msg.content.addr);
    ret_val = mengine_special_switch(SWITCH_BY_NO, song_number);

    //返回成功
    mengine_reply_msg(msg_ptr, ret_val);

    return RESULT_IGNORE;
}

//目录切换
app_result_e mengine_play_folder_next(void* msg_ptr)
{
    bool ret_val;

    ret_val = mengine_special_switch(SWITCH_NEXT_DIR, 1);

    //返回成功
    mengine_reply_msg(msg_ptr, ret_val);

    return RESULT_IGNORE;
}

//目录切换
app_result_e mengine_play_folder_prev(void* msg_ptr)
{
    bool ret_val;

    ret_val = mengine_special_switch(SWITCH_PRE_DIR, 1);

    //返回成功
    mengine_reply_msg(msg_ptr, ret_val);

    return RESULT_IGNORE;
}

