//
//      播放窗口
//
//1.主视图
//2.处理上/下/快进/快退/播放/暂停/静音/模式
//  上音量减/下音量加/目录加/目录减
//  (音量加/音量减/EQ由common去处理)
//  闹钟/POWER/录音由common去处理，跟本应用完全无关
//3.处理REPEAT键(弹出REPEATE窗口，并将这个按键消息也发送给窗口)
//4.处理AB键(弹出AB窗口，并将这个按键消息也发送给窗口)
//5.处理数字键(弹出数字选歌窗口, 并将这个按键消息也发送给窗口)
//6.处理异常：一首歌格式错误/全部错误/卡U拔出
//7.处理切盘消息
//
//备注：1.
//      2.
#include "music.h"
#include "music_scan_cmd.h"

//快进/快退的步长(单位:毫秒)
#define FF_FB_STEP      (5000)

//timer的时间间隔(单位:毫秒)
#define T1_INTERVAL     (250)

//连续错误歌曲数
uint16 g_error_num = 0;
//上次错误的歌曲号
uint16 g_error_file_num = 0;
//向前切(如果这一曲刚好错误，要跳到上一个还是下一个)
uint16 g_switch_prev = 0;
bool g_neednot_song_tts_play;
//切歌之前的状态
play_status_e g_last_play_status = StopSta;
//文件路径来源信息保存，播放时候用
file_path_info_t g_file_path_info;
//当前播放状态信息；当前播放时间和比特率信息
mengine_info_t g_music_info;
//music总时间信息
mmm_mp_file_info_t g_music_fileinfo;
//music的配置项信息
music_config_t g_music_config;
//在提示后，是否退出本窗口
bool g_quit_from_hint = FALSE;
//刷新标志
uint8 g_play_refresh_flag = PLAY_REFRESH_ALL;
static int8 music_sound_mode_timer_id = -1;
    
//前一次记录的播放时间(单位:秒)
MUS_STATIC uint32 s_prev_time;
MUS_STATIC play_status_e s_status_old = StopSta;
//按下快进/快退键后，还没有抬起
MUS_STATIC bool s_fast = FALSE;
//引擎的ERROR状态
MUS_STATIC eg_err_e s_eg_err_status = EG_ERR_NONE;

//消息-事件表

MUS_STATIC app_result_e _play_key_ff(void);
MUS_STATIC app_result_e _play_key_fb(void);
MUS_STATIC app_result_e _play_stop_ffb(void);
MUS_STATIC app_result_e _play_key_ab(void);
MUS_STATIC app_result_e _play_key_repeat(void);
MUS_STATIC app_result_e _play_key_mute(void) __FAR__;
MUS_STATIC app_result_e _play_key_folderadd(void) __FAR__;
MUS_STATIC app_result_e _play_key_foldersub(void) __FAR__;
MUS_STATIC app_result_e _play_key_number(void);
MUS_STATIC app_result_e _play_disk_out(void *ev_param) __FAR__;
MUS_STATIC app_result_e _play_card_out(void *ev_param) __FAR__;
MUS_STATIC app_result_e _play_uhost_out(void *ev_param) __FAR__;

MUS_STATIC app_result_e get_message_loop(void);
MUS_STATIC void _play_create_view(void);

MUS_STATIC  app_result_e _audiopp_update(void *ev_param);
MUS_STATIC __section__(".text.BANK20") void update_sound_mode(void);
void __section__(".text.BANK20")  update_waves_bin_para(void);

MUS_STATIC const key_event_map_t __section__(".rodata.ke_maplist") music_play_key_map_list[] =
{
    /*! 短按按next切换下一曲 */
    { { KEY_NEXT_VOLADD, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, play_key_next },
    /*! 短按按prev切换上一曲 */
    { { KEY_PREV_VOLSUB, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, play_key_prev },
    /*! 长按next快进 */
    { { KEY_NEXT, 0, KEY_TYPE_LONG, KEY_DEAL_NULL }, _play_key_ff },
    { { KEY_NEXT, 0, KEY_TYPE_LONG_UP, KEY_DEAL_NULL }, _play_stop_ffb },
    /*! 长按prev快退 */
    { { KEY_PREV, 0, KEY_TYPE_LONG, KEY_DEAL_NULL }, _play_key_fb },
    { { KEY_PREV, 0, KEY_TYPE_LONG_UP, KEY_DEAL_NULL }, _play_stop_ffb },
    /*! 短按按next切换下一曲 */
    { { KEY_NEXT, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, play_key_next },
    /*! 短按按prev切换上一曲 */
    { { KEY_PREV, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, play_key_prev },
    /*!短按play播放/暂停事件 */
    { { KEY_PLAY, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, play_key_play },
    { { KEY_PHONEPLAY, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, play_key_play },
    #ifdef AB_SUPPORT
    { { KEY_ABREC, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, _play_key_ab },
    #endif
    /*! 循环模式设置事件 */
    { { KEY_REPEAT, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, _play_key_repeat },
    /*! mute开关事件 */
    { { KEY_MUTE, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, _play_key_mute },
    /*! forder加事件 */
    { { KEY_FOLDER_ADD, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, _play_key_folderadd },
    /*! forder减事件 */
    { { KEY_FOLDER_SUB, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, _play_key_foldersub },
    /*! 数字点歌事件 */
    { { KEY_NUMBER, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, _play_key_number },

    /*! 结束标志 */
    { { KEY_NULL, 0, KEY_TYPE_NULL, KEY_DEAL_NULL }, NULL },
};

MUS_STATIC const sys_event_map_t __section__(".rodata.se_maplist") music_play_se_map_list[] =
{
    { { MSG_SD_OUT, SYSMSG_STOP_TTS_YES }, _play_card_out, },
    { { MSG_UH_OUT, SYSMSG_STOP_TTS_YES }, _play_uhost_out, },

    { { MSG_MUSIC_SONG_NUM_TTS, SYSMSG_STOP_TTS_YES }, music_song_num_tts, },

    /*! 结束标志 */
    { { MSG_NULL, 0 }, NULL },
};

bool __section__(".rcode") play_get_status(mengine_info_t *p_music_info)
{
    if (sys_share_query_read(APP_ID_MENGINE, p_music_info) == -1)
    {
        PRINT_ERR("music share query not exist!\n");
        return FALSE;
    }

    return TRUE;
}

MUS_STATIC bool __section__(".text.BANK2") _play_set_ffb_step(void)
{
    bool bret;
    uint32 step_len = FF_FB_STEP;
    bret = music_send_msg((void *) &step_len, NULL, MSG_MENGINE_SET_FFBSTEP_SYNC);
    return bret;
}

app_result_e music_scene_play(void)
{
    app_result_e ret_vals = RESULT_NULL;
    uint8 disk_in_play;

    _play_create_view();
    //连续错误歌曲数
    g_error_num = 0;
    g_error_file_num = 0;
    g_manul_switch_song_seq = 0;

    ret_vals = play_init();
    if (ret_vals > RESULT_COMMON_RESERVE)
    {
        //调用场景退出函数
        play_exit();
        return ret_vals;
    }

    //正常播放,没有错误
    if (g_music_info.eg_status.err_status == EG_ERR_NONE)
    {
        music_create_song_num_view(VIEW_TYPE_MSG, FALSE, PLAY_RESUME);
    }

    disk_in_play = g_file_path_info.file_path.dirlocation.disk;

    //启动扫描线程
    music_open_scan(disk_in_play);

    //修改播放盘的状态,common使用
    if (disk_in_play == DISK_H)
    {
        set_play_disk_type(PLAY_DISK_CARD);
    }
    else
    {
        set_play_disk_type(PLAY_DISK_UHOST);
    }

    //场景循环
    ret_vals = get_message_loop();

    //关闭扫描线程
    music_close_scan();

    //调用场景退出函数
    play_exit();
    return ret_vals;
}

MUS_STATIC void _play_check_status(void)
{
    app_result_e sta_ret = RESULT_NULL;
    static bool play_ok_falg = FALSE;//是否已经正常播放，区分空卡和出错后识别为空卡
    play_get_status(&g_music_info); //获取状态信息

    if (1 == sys_comval->dae_cfg.audiopp_type_update_flag)
    {
        update_sound_mode();
    }

    if (1 == g_app_info_state_all.waves_dae_para_update_flag)
    {
        update_waves_bin_para();
    }
    
    //正常播放,没有错误
    if (g_music_info.eg_status.err_status == EG_ERR_NONE)
    {
        play_ok_falg = TRUE;
        if ((g_music_info.eg_status.play_status != StopSta) && (g_music_info.eg_playinfo.cur_time > 0))
        {
            //连续错误歌曲数
            g_error_num = 0;
            g_error_file_num = 0;
            if (g_music_info.eg_status.play_status != PlayFast)
            {
                //清除向前切换标识
                g_switch_prev = 0;
            }
        }
        s_eg_err_status = EG_ERR_NONE;

        //是否切歌
        if ((g_music_info.eg_playinfo.cur_file_switch & 0x01) != 0)
        {
            //清除文件切换标志
            if (g_file_path_info.file_path.dirlocation.file_num != g_manul_switch_song_seq)
            {
                music_create_song_num_view((s_fast == TRUE) ? VIEW_TYPE_MSG : VIEW_TYPE_SUB, FALSE, PLAY_NORMAL);
            }
            else
            {
                music_clear_fileswitch();
            }
            g_manul_switch_song_seq = 0;
        }

        if (s_prev_time != (g_music_info.eg_playinfo.cur_time / 1000))
        {
            s_prev_time = g_music_info.eg_playinfo.cur_time / 1000;
            g_play_refresh_flag |= PLAY_REFRESH_TIME;
            com_view_update(APP_VIEW_ID_MAIN);
        }
    }
    else if (s_eg_err_status != g_music_info.eg_status.err_status)
    {
        //错误处理
        s_eg_err_status = g_music_info.eg_status.err_status;
        if((play_ok_falg == FALSE) && (s_eg_err_status == EG_ERR_NO_FILE))
        {
            sta_ret = RESULT_NEXT_FUNCTION;//空卡
        }
        else if(s_eg_err_status == EG_ERR_RAED_CARD_TIMEOUT)
        {
            sta_ret = RESULT_NEXT_FUNCTION;//读卡超时
        }
        else
        {
            sta_ret = play_err_deal(g_music_info.eg_status.err_status);
        }
        
        if (sta_ret == RESULT_NEXT_FUNCTION)
        {
            //停止播放，不然，如果在显示ER03时又快速插入，会有这样的现象：
            //播放一小段时间，然后退出music
            music_stop();
            create_view_hint(g_music_info.eg_status.err_status);
        }
        else if (sta_ret == RESULT_NULL)
        {
            s_eg_err_status = EG_ERR_NONE;
            music_create_song_num_view(VIEW_TYPE_MSG, FALSE, PLAY_NORMAL);
        }
        else
        {
        }
    }
    else
    {
        //错误已经得到处理了
    }

    if (g_music_info.eg_status.play_status != s_status_old)
    {
        s_status_old = g_music_info.eg_status.play_status;
        g_play_refresh_flag |= PLAY_REFRESH_STATUS;
        com_view_update(APP_VIEW_ID_MAIN);
    }
}

MUS_STATIC app_result_e get_message_loop(void)
{
    uint8 scan_flag = 0;
    app_result_e ret_vals = RESULT_NULL;
    while (1)
    {
        if (g_quit_from_hint == TRUE)
        {
            ret_vals = RESULT_NEXT_FUNCTION;
            break;
        }

        _play_check_status(); //从引擎获取当前状态信息

        //检查是否扫描完成
        if (g_scan_handle != NULL)
        {
            music_scan_cmd(g_scan_handle, MUSIC_SCAN_GET_STATUS, (unsigned int) &g_scan_flag);
            //关闭扫描线程
            if ((g_scan_flag & 0x81) != 0)
            {
                //出错或正常结束
                music_close_scan();
            }
            //正确扫描结束
            if ((g_scan_flag & 0x81) == 1)
            {
                PRINT_DBG("scan over!");
                update_dir_list_random();//更新随机数
                if((g_scan_flag & 0x02) != 0)
                {
                    //文件系统有改动,刷新apk播放列表
                    music_upload_music_plist();
                }
                //发送消息给引擎
                music_send_msg(NULL, NULL, MSG_MENGINE_LIST_ACCESS_ALLOW_SYNC);
            }
            else if((g_scan_flag & 0x80) == 0x80)
            {
                //出错了
                PRINT_ERR("scan err!");
            }
            else
            {
                ;//QAC
            }
        }

        ret_vals = com_view_loop();
        if (ret_vals> RESULT_COMMON_RESERVE)
        {
            break;
        }

#ifdef SUPPORT_ASET_TEST
        if ((g_app_info_state.stub_tools_type == STUB_PC_TOOL_WAVES_ASET_MODE) 
            || (g_app_info_state.stub_tools_type == STUB_PC_TOOL_ASET_EQ_MODE))
        {
            aset_test_loop_deal();
        }
#endif
        sys_os_time_dly(1);
    } //end of while

    return ret_vals;
}

MUS_STATIC app_result_e __section__(".text.BANK2") _play_key_ff(void)
{
    if (g_musicplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    if (g_music_info.eg_status.play_status != PlayFast)
    {
        //设置初始步长
        _play_set_ffb_step();
        if(g_music_info.eg_status.play_status == PauseSta)
        {
            music_resume(PLAY_FFON);
        }
        else
        {
            music_send_msg(NULL, NULL, MSG_MENGINE_FFWD_SYNC);
        }
        s_fast = TRUE;
        g_switch_prev = 0;
    }

    return RESULT_NULL;
}

MUS_STATIC app_result_e __section__(".text.BANK2") _play_key_fb(void)
{
    if (g_musicplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    if (g_music_info.eg_status.play_status != PlayFast)
    {
        //设置初始步长
        _play_set_ffb_step();
        if(g_music_info.eg_status.play_status == PauseSta)
        {
            music_resume(PLAY_FBON);
        }
        else
        {
            music_send_msg(NULL, NULL, MSG_MENGINE_FBWD_SYNC);
        }
        s_fast = TRUE;
        g_switch_prev = 1;
    }

    return RESULT_NULL;
}

MUS_STATIC app_result_e __section__(".text.BANK2") _play_stop_ffb(void)
{
    if (g_musicplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    //清消息队列，不处理过多的按键消息
    s_fast = FALSE;
    sys_mq_flush(MQ_ID_GUI);

    g_switch_prev = 0;

    //快进退时取消快进退
    if (g_music_info.eg_status.play_status == PlayFast)
    {
        music_send_msg(NULL, NULL, MSG_MENGINE_CANCEL_FFB_SYNC);
        //执行到这里时，引擎可能还没来得及更新file_switch标志，所以这里主动清除一下标志
        music_clear_fileswitch();
        g_play_refresh_flag |= PLAY_REFRESH_STATUS;
        com_view_update(APP_VIEW_ID_MAIN);
    }

    return RESULT_NULL;
}
MUS_STATIC app_result_e __section__(".text.BANK2") _play_key_number(void)
{
    if (g_musicplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    //ToDo
    //create_view_digit_song(g_p_view_ke);
    return RESULT_NULL;
}

#ifdef AB_SUPPORT
MUS_STATIC app_result_e __section__(".text.BANK2") _play_key_ab(void)
{
    if (g_musicplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    com_filter_key_hold();
    return create_view_ab();
}
#endif

MUS_STATIC app_result_e __section__(".text.BANK2") _play_key_repeat(void)
{
    if (g_musicplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    return create_view_loop_mode();
}

MUS_STATIC void __section__(".text.BANK2") _play_create_view(void)
{
    create_view_param_t param;

    param.type = VIEW_TYPE_MAIN;
    param.unknown_key_deal = UNKNOWN_KEY_IGNORE;
    param.overtime = 0;
    param.ke_maplist = music_play_key_map_list;
    param.se_maplist = music_play_se_map_list;
    param.view_cb = play_view_handle;
    com_view_add(APP_VIEW_ID_MAIN, &param);
}

__section__(".text.BANK2") MUS_STATIC app_result_e _play_key_mute(void)
{
    if (g_musicplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    if (get_mute_enable() == FALSE)
    {
        //只有在正常播放时进行设置mute
        if (g_music_info.eg_status.play_status == PlaySta)
        {
            com_set_mute(TRUE);
        }
    }
    else
    {
        com_set_mute(FALSE);
    }

    return RESULT_NULL;

}

__section__(".text.BANK2") MUS_STATIC app_result_e _play_key_folderadd(void)
{
    play_mode_e play_mode;

    if (g_musicplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    if((g_music_info.eg_status.play_status != StopSta)
            && (g_music_info.eg_status.play_status != PauseSta))
    {
        play_mode = PLAY_NORMAL;
    }
    else
    {
        play_mode = PLAY_NO_PLAY;
    }

    //切换文件夹
    //发送同步消息
    music_send_msg(NULL, NULL, MSG_MENGINE_FOLFER_NEXT_SYNC);
    g_switch_prev = 0;

    //语音播报
    music_switch_song(TTS_SONG_SEQ, play_mode);

    return RESULT_NULL;
}

MUS_STATIC __section__(".text.BANK2") app_result_e _play_key_foldersub(void)
{
    play_mode_e play_mode;

    if (g_musicplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    if((g_music_info.eg_status.play_status != StopSta)
            && (g_music_info.eg_status.play_status != PauseSta))
    {
        play_mode = PLAY_NORMAL;
    }
    else
    {
        play_mode = PLAY_NO_PLAY;
    }

    //切换文件夹
    music_send_msg(NULL, NULL, MSG_MENGINE_FOLFER_PREV_SYNC);
    //为出错处理使用
    g_switch_prev = 1;

    //语音播报
    music_switch_song(TTS_SONG_SEQ, play_mode);

    return RESULT_NULL;
}

MUS_STATIC __section__(".text.BANK2") app_result_e _play_card_out(void *ev_param)
{
    //app_result_e ret;
    //U盘模式下拔卡，不做处理
    if (g_file_path_info.file_path.dirlocation.disk == DISK_U)
    {
        return RESULT_NULL;
    }
	
    if ((g_musicplay_init_flag == TRUE)&&(g_file_path_info.file_path.dirlocation.disk == DISK_H))
    {
        return RESULT_NEXT_FUNCTION;
    }

    _play_disk_out(ev_param);

    return RESULT_NULL;
}

MUS_STATIC __section__(".text.BANK2") app_result_e _play_uhost_out(void *ev_param)
{
    //app_result_e ret;
    //卡模式下拔U盘，不做处理
    if (g_file_path_info.file_path.dirlocation.disk == DISK_H)
    {
        return RESULT_NULL;
    }
	
    if ((g_musicplay_init_flag == TRUE)&&(g_file_path_info.file_path.dirlocation.disk == DISK_U))
    {
        return RESULT_NEXT_FUNCTION;
    }

    _play_disk_out(ev_param);

    return RESULT_NULL;
}

MUS_STATIC __section__(".text.BANK2") app_result_e _play_disk_out(void *ev_param)
{
    app_result_e ret;

    /*if ((g_music_info.eg_status.play_status != StopSta)
            && (g_music_info.eg_status.play_status != PauseSta))
    {
        //正在播放，_play_check_status会处理，这里不处理
        return RESULT_NULL;
    }*/

    ret = play_err_deal(EG_ERR_DISK_OUT);
    if (ret == RESULT_NEXT_FUNCTION)
    {
        //已经救不回来了

        //引擎停止播放。不然，如果此时再插入，
        //引擎又开始播，然后退出music,然后又进入music，这样不好。
        music_stop();
        //下面的过程结束时，会退出music
        create_view_hint(EG_ERR_DISK_OUT);
    }
    else if (ret == RESULT_NULL)
    {
        music_create_song_num_view(VIEW_TYPE_MSG, FALSE, PLAY_RESUME);
    }
    else
    {
        ; //QAC
    }

    return RESULT_NULL;
}


MUS_STATIC __section__(".text.BANK20") void sound_mode_handle(void)
{
    g_neednot_song_tts_play = TRUE;
    play_key_play();
    g_neednot_song_tts_play = FALSE; 

    kill_app_timer(music_sound_mode_timer_id);
}



MUS_STATIC __section__(".text.BANK20") void update_sound_mode(void)
{
    sys_comval->dae_cfg.audiopp_type_update_flag = 0;
    sound_mode_handle();
    music_sound_mode_timer_id = set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL,1000, sound_mode_handle);
}

void __section__(".text.BANK20")  update_waves_bin_para(void)
{
    g_app_info_state_all.waves_dae_para_update_flag = 0;
    sound_mode_handle();

    music_sound_mode_timer_id = set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL,1000, sound_mode_handle);
}

