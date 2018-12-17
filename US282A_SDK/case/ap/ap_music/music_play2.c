///
//      播放窗口
//
//  (因为一个bank放不下，所以，部分代码放到这里来)
//
//  因为在播放时，在不按按键、不出异常时，要求不切bank，
//      所以，这里的代码的特点是：“一次性”、“异常”、“按键”
#include "music.h"
#include "music_scan_cmd.h"

extern void com_send_msg_to_manager(uint32 msg_type, uint8 param1,
        uint8 param2, bool synch_flag) __FAR__;

void __section__(".text.BANK2") music_check_first_play(uint8 cur_disk)
{

    if(g_music_info.eg_status.first_file_status == FIRST_PLAY_FROM_START)
    {
        if (cur_disk == DISK_H)
        {
            g_app_info_state.need_update_playlist |= CARD_NEED_MASK;
        }
        else
        {
            g_app_info_state.need_update_playlist |= UHOST_NEED_MASK;
        }

    }
    music_send_msg(NULL, NULL, MSG_MENGINE_CLR_BKERR_SYNC);
}

//开关引擎

MUS_STATIC app_result_e _play_open_eg(mengine_enter_mode_e start_mode)
{
    app_result_e result = RESULT_NULL;
    bool bret = FALSE;
    msg_apps_t msg;

    //message type
    msg.type = MSG_CREAT_APP_SYNC;
    //param
    msg.content.data[0] = APP_ID_MENGINE;
    if(g_file_path_info.file_path.dirlocation.disk == DISK_H)
    {
        msg.content.data[1] = (uint8) (start_mode | ENTER_MUSIC_DISK_H);
        if(g_ap_switch_var.card_in_out_flag ==0)
        {
            msg.content.data[1] |= MUSIC_NEED_BK;
        }
    }
    else
    {
        msg.content.data[1] = (uint8) (start_mode | ENTER_MUSIC_DISK_U);
        if(g_ap_switch_var.uhost_in_out_flag ==0)
        {
            msg.content.data[1] |= MUSIC_NEED_BK;
        }
    }
    g_ap_switch_var.card_in_out_flag = 0;
    g_ap_switch_var.uhost_in_out_flag = 0;
    //msg.content.data[1] = (uint8) (start_mode | g_file_path_info.file_path.dirlocation.disk);
    //发送同步消息
    if (send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0) == ERR_NO_ERR)
    {
        bret = TRUE;
    }

    if (bret == TRUE)
    {
        //等待MUSIC状态可用
        while (play_get_status(&g_music_info) == FALSE)
        {
            sys_os_time_dly(1);
        }

        while (1)
        {
            if (play_get_status(&g_music_info) == FALSE)
            {
                result = RESULT_NEXT_FUNCTION;
                break;
            }

            if (g_music_info.eg_status.ctl_status != EG_CTL_INITING)
            {
                //if need update flag
                music_check_first_play(g_file_path_info.file_path.dirlocation.disk);
                break;
            }

            if ((g_standby_exit_flag == FALSE) && (g_esd_restart_flag == FALSE))
            {
                result = com_view_loop();
                if (result> RESULT_COMMON_RESERVE)
                {
                    break;
                }
            }

            sys_os_time_dly(1);
        }
    }
    else
    {
        //初始化失败，切换到下一个应用
        result = RESULT_NEXT_FUNCTION;
    }

    return result;
}

MUS_STATIC bool _play_close_eg(uint8 engine_id)
{
    bool bret = FALSE;
    msg_apps_t msg;
    msg.type = MSG_KILL_APP_SYNC;
    msg.content.data[0] = engine_id;
    //发送同步消息
    if (send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0) == ERR_NO_ERR)
    {
        bret = TRUE;
    }

    return bret;
}

//开关扫描线程

void __section__(".text.BANK2") music_open_scan(uint8 scan_disk)
{
    char* scan_name =
    {   "mus_scan.al"};
    bool exit_ret;

    if (g_app_info_state.autotest_mode == TRUE)
    {
        return;
    }

    if (((scan_disk == DISK_H) && ((g_app_info_state.need_update_playlist & CARD_NEED_SCAN) == 0))
            || ((scan_disk == DISK_U) && ((g_app_info_state.need_update_playlist & UHOST_NEED_SCAN) == 0)))
    {
        //g_rcp_file_status.scan_status = 1;
        music_rcp_fs_init(g_file_path_info.file_path.dirlocation.disk, 0);
        //music_send_msg(NULL, NULL, MSG_MENGINE_LIST_ACCESS_ALLOW_SYNC);
        if(g_vmdisk_flag == scan_disk)
        {
            g_rcp_file_status.scan_status = 1;
            music_send_msg(NULL, NULL, MSG_MENGINE_LIST_ACCESS_ALLOW_SYNC);
            return;
        }
    }

    //退出上一次的id3获取
    exit_ret = music_rcp_fs_exit(scan_disk, eh_vfs_mount);

    if (exit_ret == TRUE)
    {
        eh_vfs_mount = -1;
    }

    //scan end flag
    g_rcp_file_status.scan_status = 0;

    if(g_app_info_state.stub_tools_type == 0)
    {
        //加载扫描线程
        sys_load_mmm(scan_name, FALSE);
        if (g_scan_handle == NULL)
        {
            uint8 scan_param[4];
            scan_param[0] = scan_disk;
            scan_param[1] = g_enter_mode;

            //初始化扫描
            music_scan_cmd(&g_scan_handle, MUSIC_SCAN_OPEN, (unsigned int) scan_param);
        }
    }
}

void music_close_scan(void)
{
    uint8 scan_ret = 0;
    if (g_scan_handle != NULL)
    {
        music_scan_cmd(g_scan_handle, MUSIC_SCAN_GET_STATUS, (unsigned int) &scan_ret);
        music_scan_cmd(g_scan_handle, MUSIC_SCAN_CLOSE, (unsigned int) NULL);
        music_get_filepath(&g_file_path_info);
        if (g_file_path_info.file_path.dirlocation.disk == DISK_H)
        {
            g_app_info_state.need_update_playlist &= (uint8) (~(CARD_NEED_SCAN));
        }
        else if (g_file_path_info.file_path.dirlocation.disk == DISK_U)
        {
            g_app_info_state.need_update_playlist &= ~(UHOST_NEED_SCAN);
        }
        else
        {
            ; //QAC
        }
        g_scan_handle = NULL;

        //卸载扫描线程
        sys_free_mmm(FALSE);
    }

    if ((scan_ret & 0x01) == 0x01)
    {
        //scan结束
        g_rcp_file_status.scan_status = 1;
        music_rcp_fs_init(g_file_path_info.file_path.dirlocation.disk, 0);
    }
}

bool play_next(void)
{
    bool bret;
    //只是切换文件，不会播
    //所以：只要文件存在，就会成功；
    //即使文件格式不支持。
    bret = music_send_msg(NULL, NULL, MSG_MENGINE_PLAY_NEXT_SYNC);
    return bret;
}

bool play_prev(void)
{
    bool bret;
    //只是切换文件，不会播
    //所以：只要文件存在，就会成功；
    //即使文件格式不支持。
    bret = music_send_msg(NULL, NULL, MSG_MENGINE_PLAY_PREV_SYNC);
    return bret;
}

MUS_STATIC bool _play_switch_disk2(uint8 disk_flag)
{
    bool bret;

    //关闭扫描线程
    music_close_scan();

    if (g_file_path_info.file_path.dirlocation.disk == DISK_H)
    {
        //快速插拔卡不更新列表
        //g_app_info_state.need_update_playlist |= CARD_NEED_UPLOAD;

    }
    else
    {
        //快速插拔U盘不更新列表
        //g_app_info_state.need_update_playlist |= UHOST_NEED_UPLOAD;
    }

    bret = music_send_msg(&disk_flag, NULL, MSG_MENGINE_SWITCH_DISK_SYNC);

    //切换盘符打开静音
    com_set_mute(FALSE);

    return bret;
}


app_result_e play_err_deal(eg_err_e error_number)
{
    app_result_e err_ret = RESULT_NULL;
    bool ret_val = FALSE;
    bool disk_out = FALSE;
    uint8 disk;
    int8 card_exist = -1, uhost_exist = -1;
    uint8 err_cnt = 0;
    bool err_rty_agin = FALSE;
    PRINT_ERR_INT("Disk-play-err.mode:", error_number);
    Try_agin:
    //检测是否磁盘可切换
    if (com_get_config_default(SETTING_APP_SUPPORT_CARDPLAY) != 0)
    {
        //卡存在否
        card_exist = (int8) sys_detect_disk(DRV_GROUP_STG_CARD);
    }
    if (com_get_config_default(SETTING_APP_SUPPORT_UHOSTPLAY) != 0)
    {
        //u盘存在否
        uhost_exist = (int8) sys_detect_disk(DRV_GROUP_STG_UHOST);
    }

    //获取文件路径
    music_get_filepath(&g_file_path_info);
    disk = g_file_path_info.file_path.dirlocation.disk;
    //判断是否是拔出磁盘的错误
    if ((card_exist == -1) && (disk == DISK_H))
    {
        //当前在播卡，但卡却不存在了
        disk_out = TRUE;
    }
    if ((uhost_exist == -1) && (disk == DISK_U))
    {
        //当前在播U，但U却不存在了
        disk_out = TRUE;
    }
    if (disk_out == TRUE)
    {
        music_send_msg(NULL, NULL, MSG_MENGINE_SAVE_BKINFO_SYNC);
    }

    if ((error_number == EG_ERR_DISK_OUT) || (error_number == EG_ERR_NO_FILE) || (disk_out == TRUE))
    {
        //拔卡U或者没文件

        //防止系统消息处理完之后,显示出现错乱
        if ((disk == DISK_H) && (disk_out == FALSE))
        {
            //在播卡，卡现在也还是在的，但刚才却提示
            //EG_ERR_DISK_OUT或EG_ERR_NO_FILE
            //(这是ESD打卡时的场景)
            for (err_cnt = 0; err_cnt < 5; err_cnt++)
            {
                #if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
                //闪灯指示错误
                discrete_led_set(LED_ID_MUSIC_PLAY, LED_ALWAYS_OFF, NULL);
                sys_os_time_dly(5);
                discrete_led_set(LED_ID_MUSIC_PLAY, LED_ALWAYS_ON, NULL);
                #endif
                sys_os_time_dly(3);
                PRINT_ERR_INT("Disk-init.try=", err_cnt);
                if (_play_switch_disk2(DISK_H) == TRUE)
                {
                    music_open_scan(DISK_H);
                    break;
                }
                //没开WD时，不考虑ESD的异常情况,直接切应用
                if ((act_readl(WD_CTL) & (1 << WD_CTL_WDEN)) == 0)
                {
                    err_ret = RESULT_NEXT_FUNCTION;
                    break;
                }

                //方案支持ESD打卡
                //#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
                if (err_cnt >= 3) //断电重试2次
                {
                    //拉GPIO口断SD卡电
                    music_send_msg(NULL, NULL, MSG_MENGINE_SD_PW_RESET_SYNC);
                }
                //#endif
            }
            if (err_cnt == 5)
            {
                PRINT_ERR("WD-reset...");
                sys_os_time_dly(1);
                sys_os_sched_lock();
                //WD_CTL_176ms ,  使能WD,  清WD计数
                act_writel((0 << WD_CTL_CLKSEL_SHIFT) | (1 << WD_CTL_WDEN) | (1 << WD_CTL_CLR), WD_CTL);//352ms
                while (1)
                {
                    ;//等待重启
                }
            }
        }
        else
        {
            err_ret = RESULT_NEXT_FUNCTION;
        }
    }
    else
    {
        uint16 file_total_number;
        if(err_rty_agin == FALSE)
        {
            err_rty_agin = TRUE;
            sys_os_time_dly(1);
            goto Try_agin;
        }
        //其他情况是解码出错
        if (g_error_file_num != g_file_path_info.file_path.dirlocation.file_num)
        {
            g_error_file_num = g_file_path_info.file_path.dirlocation.file_num;
            g_error_num++;
        }

        file_total_number = g_file_path_info.file_path.dirlocation.file_total;

        if (((file_total_number != 0) && (g_error_num > file_total_number)) || ((file_total_number == 1)\
                && (g_error_num == file_total_number) && (0 == g_music_info.eg_playinfo.cur_time)))
        {
            //切换到下一个应用
            err_ret = RESULT_NEXT_FUNCTION;
        }
        else
        {
            if (g_switch_prev == 1)
            {
                //切换到上一曲
                ret_val = play_prev();
            }
            else
            {
                //切换到下一曲
                ret_val = play_next();
            }
            if (ret_val == TRUE)
            {
                music_play();
            }
            else
            {        
                if (disk == DISK_U)
                {
                    err_ret = RESULT_NEXT_FUNCTION;
                }
                else
                {
                    err_ret = RESULT_NULL;
                }             
            }
        }
    }

    return err_ret;
}

app_result_e play_key_play(void)
{
    if (g_musicplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    if ((g_music_info.eg_status.play_status == PauseSta) || (g_music_info.eg_status.play_status == StopSta))
    {
        //TTS:"播放"
        if (g_neednot_song_tts_play == FALSE)
        {
            com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_PLAY_SONG);
        }
        music_resume(PLAY_RESUME);
    }
    else if (g_music_info.eg_status.play_status == PlaySta)
    {
        //TTS:"暂停"
        music_pause();
        if (g_neednot_song_tts_play == FALSE)
        {
            com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_PAUSE_SONG);
        }
    }
    else
    {
        ; //QAC
    }
    g_play_refresh_flag |= PLAY_REFRESH_STATUS;
    com_view_update(APP_VIEW_ID_MAIN);
    return RESULT_NULL;
}

static void _clear_err(void)
{
    //如果play_err_deal里面music_play已经成功了，但马上又按“下一曲”键
    //这个按键的处理会在com_view_loop里面
    //如果下一首刚好有错
    //循环中_play_check_status时就会得到一个ERROR
    //于是g_error_num又加1
    //这样会导致g_error_num >= file_total_number
    //结果RESULT_NEXT_FUNCTION
    //而实际不应该这样：因为这一首是可以播的。
    //所以，加如下代码
    if (g_error_num != 0)
    {
        play_get_status(&g_music_info);
        if (g_music_info.eg_status.err_status == EG_ERR_NONE)
        {
            //虽然此时EG_ERR_NONE，不一定真的EG_ERR_NONE
            //但如果此时g_error_num没有清零，可能会漏过一个好的文件
            g_error_num = 0;
            g_error_file_num = 0;
        }
    }
}

app_result_e play_key_prev(void)
{
    play_mode_e play_mode;

    if (g_musicplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    //过滤来不及响应的短按按键
    com_filter_keymsg_in_queue(g_p_view_ke->val, KEY_TYPE_SHORT_UP);

    if ((g_music_info.eg_status.play_status != StopSta) && (g_music_info.eg_status.play_status != PauseSta))
    {
        play_mode = PLAY_NORMAL;
    }
    else
    {
        play_mode = PLAY_NO_PLAY;
    }

    //为出错处理使用
    g_switch_prev = 1;
    _clear_err();
    play_prev();
    music_create_song_num_view(VIEW_TYPE_SUB, TRUE, play_mode);

    return RESULT_NULL;
}

app_result_e play_key_next(void)
{
    play_mode_e play_mode;

    if (g_musicplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    //过滤来不及响应的短按按键
    com_filter_keymsg_in_queue(g_p_view_ke->val, KEY_TYPE_SHORT_UP);

    if ((g_music_info.eg_status.play_status != StopSta) && (g_music_info.eg_status.play_status != PauseSta))
    {
        play_mode = PLAY_NORMAL;
    }
    else
    {
        play_mode = PLAY_NO_PLAY;
    }

    g_switch_prev = 0;
    _clear_err();
    play_next();
    music_create_song_num_view(VIEW_TYPE_SUB, TRUE, play_mode);

    return RESULT_NULL;
}

//打开引擎, 播放
app_result_e __section__(".text.BANK2")play_init(void)
{
    app_result_e result = RESULT_NULL;
    bool need_not_play = g_standby_exit_flag;

    com_set_mute(FALSE); //解除静音

    if (get_engine_type() != ENGINE_NULL)
    {
        _play_close_eg(APP_ID_THEENGINE);
    }

    g_musicplay_init_flag = TRUE;

    if ((g_ap_switch_var.call_background_status != CALL_BACK_STATUS_BACK) && (g_standby_exit_flag == FALSE))
    {
        if(g_esd_restart_flag == FALSE)
        {
            if ((g_enter_mode == PARAM_RECORD_CPLAY) || (g_enter_mode == PARAM_RECORD_UPLAY))
            {
                com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NOBLOCK, (void *) TTS_PLAY_RECORD);
            }
            else if (g_enter_mode == PARAM_MUSIC_SDCARD)
            {
                com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NOBLOCK, (void *) TTS_PLAY_SDCARD);
            }
            else
            {
                com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NOBLOCK, (void *) TTS_PLAY_UHOST);
            }
        }
        if (g_comval.support_bt_inback == 1)
        {
            //创建蓝牙后台
            com_btmanager_init(FALSE);
        }
        else
        {
            //杀死蓝牙后台
            com_btmanager_exit(FALSE, TRUE);
        }

        if (com_tts_state_play_wait() == TTS_PLAY_RET_BY_KEY)
        {
            sys_os_time_dly(10); //等待100ms，以便短按抬起能够完成
        }
    }

    if ((g_standby_exit_flag == FALSE) && (g_esd_restart_flag == FALSE))
    {
        while (1)
        {
            result = com_view_loop();
            if (result == RESULT_NONE_EVENT)
            {
                result = RESULT_NULL;
                break;
            }
            else if (result> RESULT_COMMON_RESERVE)
            {
                g_musicplay_init_flag = FALSE;
                return result;
            }
            else
            {
                ; //nothing
            }
        }
    }

    //打开引擎
    if ((g_enter_mode == PARAM_RECORD_CPLAY) || (g_enter_mode == PARAM_RECORD_UPLAY))
    {
        result = _play_open_eg(ENTER_RECOD_PLAY);
    }
    else
    {
        result = _play_open_eg(ENTER_MUSIC_START);
    }

    g_standby_exit_flag = FALSE;
    g_esd_restart_flag = FALSE;
    //libc_print("openok",result,2);

    g_musicplay_init_flag = FALSE;

    if (result <= RESULT_COMMON_RESERVE)
    {
        com_reset_sound_volume(0);

        music_set_loop_mode(g_music_config.repeat_mode);

        if ((g_music_info.eg_status.err_status == EG_ERR_NONE) && (need_not_play == 0))
        {
            music_resume(PLAY_RESUME);
        }
#ifdef PRINT_BANK_INFO
        print_label = 0x12345678;
#endif
    }

    return result;
}

bool play_exit(void)
{
    //文件系统退出
    if (eh_vfs_mount != -1)
    {
        music_rcp_fs_exit(g_file_path_info.file_path.dirlocation.disk, eh_vfs_mount);
        eh_vfs_mount = -1;
    }
#ifdef PRINT_BANK_INFO
    print_label = 0;
#endif
    return TRUE;
}

