/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：闹钟闹铃响播放流程功能接口实现。
 * 作者：cailizhen
 ********************************************************************************/

#include "app_alarm.h"

//打开音乐引擎
bool alarm_open_engine(uint8 disk_type)
{
    bool bret = TRUE;
    msg_apps_t msg;
    msg_reply_t temp_reply;
    //无消息内容
    msg.content.data[0] = APP_ID_MENGINE;
    if (disk_type == DISK_H)
    {
        msg.content.data[1] = (uint8) (ENTER_ALARM | ENTER_MUSIC_DISK_H);
    }
    else if (disk_type == DISK_SD)
    {
        msg.content.data[1] = (uint8) (ENTER_ALARM | ENTER_MUSIC_DISK_C);
    }
    else
    {
        msg.content.data[1] = (uint8) (ENTER_ALARM | ENTER_MUSIC_DISK_U);
    }
    //消息类型(同步创建应用程序消息)
    msg.type = MSG_CREAT_APP_SYNC;
    //发送同步消息
    bret = (bool) (send_sync_msg(APP_ID_MANAGER, &msg, &temp_reply, 0) == ERR_NO_ERR);
    return bret;
}

//关闭音乐引擎
bool alarm_close_engine(engine_type_e engine_type)
{
    msg_apps_t msg;
    bool bret = TRUE;

    if (engine_type != ENGINE_NULL)
    {
        msg.type = MSG_KILL_APP_SYNC;
        msg.content.data[0] = APP_ID_THEENGINE;
        //发送同步消息
        bret = (bool) (send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0) == ERR_NO_ERR);
    }

    return bret;
}

//设置闹钟音乐来源
bool alarm_set_filepath(file_path_info_t *filepath)
{
    bool bret = TRUE;
    msg_apps_t msg;
    //设置播放文件的路径信息
    msg.content.addr = filepath;
    //消息类型(即消息名称)
    msg.type = MSG_MENGINE_SET_FILEPATH_SYNC;
    //发送同步消息
    bret = (bool) (send_sync_msg(APP_ID_MENGINE, &msg, NULL, 0) == ERR_NO_ERR);
    return bret;
}

//设置播放模式
bool alarm_set_playmode(uint8 playmode)
{
    bool bret = TRUE;
    msg_apps_t msg;
    //设置循环模式&shuffle模式
    msg.content.addr = &playmode;
    //消息类型(即消息名称)
    msg.type = MSG_MENGINE_SET_PLAYMODE_SYNC;
    //发送同步消息
    bret = (bool) (send_sync_msg(APP_ID_MENGINE, &msg, NULL, 0) == ERR_NO_ERR);
    return bret;
}

//开始播放音乐
bool alarm_play(void)
{
    bool bret = TRUE;
    msg_apps_t msg;
    //消息类型(即消息名称)开始播放音乐
    msg.type = MSG_MENGINE_PLAY_SYNC;
    //发送同步消息
    bret = (bool) (send_sync_msg(APP_ID_MENGINE, &msg, NULL, 0) == ERR_NO_ERR);
    return bret;
}

//停止音乐播放
bool alarm_stop(void)
{
    bool bret = TRUE;
    msg_apps_t msg;

    //消息类型(即消息名称)停止播放音乐
    msg.type = MSG_MENGINE_STOP_SYNC;
    //发送同步消息
    bret = (bool) (send_sync_msg(APP_ID_MENGINE, &msg, NULL, 0) == ERR_NO_ERR);

    return bret;
}

//获取音乐播放状态
bool alarm_get_status(mengine_info_t* p_mengine_info)
{
    if (sys_share_query_read(APP_ID_MENGINE, p_mengine_info) == -1)
    {
        PRINT_ERR("music share query not exist!");
        return FALSE;
    }

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    TOOLS 应用闹钟播放循环检查播放状态
 * \param[in]    none
 * \param[out]   none
 * \return       bool TRUE表示播放正常，FALSE表示播放发生错误
 * \ingroup
 * \note
 *******************************************************************************/
bool alarm_check_status(void)
{
    mengine_info_t tmp_music_info;

    //等待MUSIC状态可用
    while (alarm_get_status(&tmp_music_info) == FALSE)
    {
        sys_os_time_dly(1);
    }
    if (tmp_music_info.eg_status.err_status != EG_ERR_NONE)
    {
        return FALSE;
    }
    if (tmp_music_info.eg_status.play_status == StopSta) //循环播放
    {
        alarm_play();
    }
    return TRUE;
}

bool __section__(".bank") set_SDD_file_IO(uint8 mode,bool playing)
{
    storage_io_t io; 
    msg_apps_t msg;
    bool ret ;
    if(mode == 16)
    {    
        libc_memcpy(g_sdd_open_parm.file_name, "zen.mp3", 11);
        if(g_sdd_open_parm.id == 16)
        {
            return TRUE;
        }
        g_sdd_open_parm.id = 16;
    }
    else if(mode == 17)
    {
        libc_memcpy(g_sdd_open_parm.file_name, "tones.mp3", 11);
        if(g_sdd_open_parm.id == 17)
        {
            return TRUE;
        }
        g_sdd_open_parm.id = 17;
    }
    else if(mode == 18)
    {
        libc_memcpy(g_sdd_open_parm.file_name, "ocean.mp3", 11);
        if(g_sdd_open_parm.id == 18)
        {
            return TRUE;
        }
        g_sdd_open_parm.id = 18;
    }
    else if(mode == 19)
    {
        libc_memcpy(g_sdd_open_parm.file_name, "storm.mp3", 11);
        if(g_sdd_open_parm.id == 19)
        {
            return TRUE;
        }
        g_sdd_open_parm.id = 19;
    }
    else if(mode == 20)
    {
        libc_memcpy(g_sdd_open_parm.file_name, "nature.mp3", 11);
        if(g_sdd_open_parm.id == 20)
        {
            return TRUE;
        }
        g_sdd_open_parm.id = 20;
    }
    else if(mode == 21)
    {
        libc_memcpy(g_sdd_open_parm.file_name, "air.mp3", 11);
        if(g_sdd_open_parm.id == 21)
        {
            return TRUE;
        }
        g_sdd_open_parm.id = 21;
    }
    else if(mode == 22)
    {
        libc_memcpy(g_sdd_open_parm.file_name, "focus.mp3", 11);
        if(g_sdd_open_parm.id == 22)
        {
            return TRUE;
        }
        g_sdd_open_parm.id = 22;
    }
    else if(mode == 23)
    {
        libc_memcpy(g_sdd_open_parm.file_name, "quiet.mp3", 11);
        if(g_sdd_open_parm.id == 23)
        {
            return TRUE;
        }
        g_sdd_open_parm.id = 23;
    }
    else if(mode == 24)
    {
        libc_memcpy(g_sdd_open_parm.file_name, "peace.mp3", 11);
        if(g_sdd_open_parm.id == 24)
        {
            return TRUE;
        }
        g_sdd_open_parm.id = 24;
    }
    else if(mode == 25)
    {
        libc_memcpy(g_sdd_open_parm.file_name, "heart.mp3", 11);
        if(g_sdd_open_parm.id == 25)
        {
            return TRUE;
        }
        g_sdd_open_parm.id = 25;
    }                      
    else
    {
        g_sdd_open_parm.id = 0;
    }
    g_sdd_open_parm.file_name = g_alarm_path.file_path.dirlocation.file_info.file_name_info.file_name;
    g_sdd_open_parm.loop_interval_ms = 0;//为0是无缝播放
    io.open_arg = (void*)&g_sdd_open_parm;
    io.file_type = FILE_SOURCE_SD;
    io.read = sdd_read;
    io.tell = sdd_tell;
    io.seek = sdd_seek;
    io.open = sdd_open;
    io.handel = NULL;
    if(g_sdd_open_parm.id != 0)
    {
        g_display_sec = 0;
        if(playing == TRUE)
        {
            alarm_stop();
        }
        msg.type = MSG_MENGINE_SET_FIO_SYNC;
        msg.content.addr = &io;
        //发送同步消息
        ret = (bool)send_sync_msg(APP_ID_MENGINE, &msg, NULL, 0);
        libc_print("set-sdd-io(f-id&ret):", (g_sdd_open_parm.id<<8)+ret, 2);
        if(playing == TRUE)
        {
            alarm_play();
        }
    }
    else
    {
        g_display_sec = 0;
        if(playing == TRUE)
        {
            alarm_stop();
        }
        libc_memcpy(g_sdd_open_parm.file_name, "alarm1.mp3", 11);
        msg.type = MSG_MENGINE_SET_FIO_SYNC;
        msg.content.addr = &io;
        //发送同步消息
        ret = (bool)send_sync_msg(APP_ID_MENGINE, &msg, NULL, 0);
        libc_print("set-sdd-io(f-id&ret):", (g_sdd_open_parm.id<<8)+ret, 2);
        if(playing == TRUE)
        {
            alarm_play();
        }
    }
    return ret;
}

uint8 choice_ring_song(void)
{
    uint8 ret = 1;
    uint16 cnt = 0;
    input_gui_msg_t gui_msg;
    sys_os_time_dly(5);
    /*while(get_gui_msg(&gui_msg) == TRUE)
    {
        sys_os_time_dly(1);
        g_display_sec = 0;
    }*/
    while(++cnt < 200)
    {
        g_display_sec = 0;
        if(get_gui_msg(&gui_msg) == TRUE)
        {
            if(gui_msg.data.kmsg.val == KEY_NEXT)//
            {
                ret = 16;
                break;
            }
            if(gui_msg.data.kmsg.val == KEY_PREV)//
            {
                ret = 17;
                break;
            }
            if(gui_msg.data.kmsg.val == KEY_MODE)
            {
                ret = 1;
                break;
            }
        }
        if(com_view_loop() > RESULT_COMMON_RESERVE)
        {
            break;
        }
        sys_os_time_dly(1);
    }
    set_SDD_file_IO(ret, TRUE);
    return ret;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    TOOLS 应用闹钟播放初始化
 * \param[in]    none
 * \param[out]   none
 * \return       bool TRUE表示初始化成功，FALSE表示初始化失败
 * \ingroup
 * \note
 *******************************************************************************/
bool alarm_play_init(alarm_ring_var_t *alarm_ring_var)
{

    alarm_alarm_t *p_alarm_alarm = &(alarm_ring_var->cur_alarm_record.alarm);
    engine_type_e engine_type;
    uint8 disk_type = DISK_SD;
    uint8 tmp_ring_type = p_alarm_alarm->ring_type;
    uint16 tmp_file_seq = (uint16) (p_alarm_alarm->sound.ring_seq);
    
    engine_type = get_engine_type();
    //关闭引擎
    if (engine_type != ENGINE_NULL)
    {
        alarm_close_engine(engine_type);
    }
    tmp_ring_type = RING_TYPE_BUILTIN;

    if ((tmp_ring_type >= RING_TYPE_FOLDER1) && (tmp_ring_type <= RING_TYPE_FOLDER4))
    {
        //卡盘不存在或者闹铃找不到，播放内置闹铃第一首
        tmp_ring_type = RING_TYPE_BUILTIN;
        tmp_file_seq = 1;
    }


    //播放内置闹铃
    if (tmp_ring_type == RING_TYPE_BUILTIN)
    {
        sd_handle alarm_handle;
        file_location_t *plocation;
        uint8 tmp_ring_file[12];
        
        com_load_share_data(COM_SHARE_DATA_RING_BUILDIN, tmp_ring_file, 12, 0);
        alarm_handle = sys_sd_fopen(tmp_ring_file);
        if (alarm_handle != NULL)
        {
            uint8 song_seq = 1;
            g_alarm_path.file_source = FSEL_TYPE_SDFILE;
            plocation = &(g_alarm_path.file_path.dirlocation);
            sys_sd_fclose(alarm_handle);
            libc_memcpy(plocation->file_info.file_extend_info.file_ext, tmp_ring_file, 12);//默认文件名
            goto alarm_fsel_init_ok;
        }
    }

    //文件选择失败，返回 FALSE
    return FALSE;

    alarm_fsel_init_ok:
    //打开引擎
    alarm_open_engine(disk_type);
    
    //设置音乐文件路径
    alarm_set_filepath(&g_alarm_path);
    
    //设置音量
    com_set_mute(FALSE);
    
    com_reset_sound_volume(0);

    //设置循环模式
    alarm_set_playmode(FSEL_MODE_LOOPONE);
    sys_os_time_dly(20);
    //choice_ring_song();
    set_SDD_file_IO(0, TRUE);
    //播放闹钟音乐
    alarm_play();
    /*set_play_disk_type(PLAY_DISK_NULL);
    {
        input_gui_msg_t gui_msg;
        sys_os_time_dly(20);
        while(get_gui_msg(&gui_msg) == TRUE)//clean key
        {
            sys_os_time_dly(2);
        }
    }*/
    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    TOOLS 应用闹钟播放关闭
 * \param[in]    none
 * \param[out]   none
 * \return       bool TRUE表示关闭成功，FALSE表示关闭失败
 * \ingroup
 * \note
 *******************************************************************************/
bool alarm_play_deinit(void)
{
    engine_type_e engine_type;

    engine_type = get_engine_type();

    //if (g_alarm_mute_backup == TRUE)
    //{
    //    com_set_mute(TRUE);
    //}

    if (engine_type == ENGINE_MUSIC)
    {
        //g_app_info_state.ring_alarm = FALSE;
        //停止播放
        alarm_stop();
        //关闭引擎
        alarm_close_engine(engine_type);
    }

    return TRUE;
}
