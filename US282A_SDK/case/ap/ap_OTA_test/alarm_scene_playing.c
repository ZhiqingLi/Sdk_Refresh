/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：闹钟闹铃响播放主场景，实现用户交互。
 * 作者：cailizhen
 ********************************************************************************/

#include "app_alarm.h"

app_result_e alarm_alarm_snooze(uint32 data1, uint32 data2);
app_result_e alarm_alarm_close(uint32 data1, uint32 data2);

app_result_e alarm_key_deal_pause_alarm(void);
app_result_e alarm_key_deal_stop_alarm(void);
void alarm_ring_handle(void);
app_result_e alarm_key_deal_non_alarm(void);

app_result_e alarm_key_deal_set_sdd_io_id16(void);
app_result_e alarm_key_deal_set_sdd_io_id17(void);
app_result_e alarm_key_deal_set_sdd_io_id18(void);
app_result_e alarm_key_deal_set_sdd_io_id19(void);
app_result_e alarm_key_deal_set_sdd_io_id20(void);
app_result_e alarm_key_deal_set_sdd_io_id21(void);
app_result_e alarm_key_deal_set_sdd_io_id22(void);
app_result_e alarm_key_deal_set_sdd_io_id23(void);
app_result_e alarm_key_deal_set_sdd_io_id24(void);
app_result_e alarm_key_deal_set_sdd_io_id25(void);







const key_event_map_t __section__(".rodata.ke_maplist") alarm_playing_ke_maplist[] =
{
    /*! 关掉闹钟 */
    //{ { KEY_MODE, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER }, alarm_key_deal_stop_alarm },

    /*! 暂停闹钟 */
    { { KEY_PLAY, 0, KEY_TYPE_SHORT_UP, 0 }, alarm_key_deal_non_alarm }, //可以映射到快捷键长按PLAY关机
    { { KEY_PHONEPLAY, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER }, alarm_key_deal_non_alarm },
  //  { { KEY_NEXT, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER }, alarm_key_deal_non_alarm },
  //  { { KEY_PREV, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER }, alarm_key_deal_non_alarm },
    //{ { KEY_PREV_VOLSUB, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER }, alarm_key_deal_non_alarm },
    { { KEY_NEXT_VOLADD, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER }, alarm_key_deal_non_alarm },
    { { KEY_ABREC, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER }, alarm_key_deal_non_alarm },
    { { KEY_MUTE, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER }, alarm_key_deal_non_alarm },
    //单击prev next16 17
    { { KEY_NEXT, 0 , KEY_TYPE_SHORT_UP, KEY_DEAL_FILTER_UP }, alarm_key_deal_set_sdd_io_id16 },
    { { KEY_PREV, 0 , KEY_TYPE_SHORT_UP, KEY_DEAL_FILTER_UP }, alarm_key_deal_set_sdd_io_id17 },
    //双击prev next18 19    
    { { KEY_NEXT, 0 , KEY_TYPE_DBL_CLICK, KEY_DEAL_FILTER_UP }, alarm_key_deal_set_sdd_io_id18 },
    { { KEY_PREV, 0 , KEY_TYPE_DBL_CLICK, KEY_DEAL_FILTER_UP }, alarm_key_deal_set_sdd_io_id19 }, 
    //长按prev next20 21
    { { KEY_NEXT, 0 , KEY_TYPE_LONG_UP, KEY_DEAL_FILTER_UP }, alarm_key_deal_set_sdd_io_id20 },
    { { KEY_PREV, 0 , KEY_TYPE_LONG_UP, KEY_DEAL_FILTER_UP }, alarm_key_deal_set_sdd_io_id21 }, 
    //单击音量加减22 23
    { { KEY_VADD, 0, KEY_TYPE_DOWN, KEY_DEAL_FILTER_UP }, alarm_key_deal_set_sdd_io_id22 },
    { { KEY_VSUB, 0, KEY_TYPE_DOWN, KEY_DEAL_FILTER_UP }, alarm_key_deal_set_sdd_io_id23 },  
    //双击音量加减24 25
    { { KEY_VADD, 0, KEY_TYPE_DBL_CLICK, KEY_DEAL_FILTER_UP }, alarm_key_deal_set_sdd_io_id24 },
    { { KEY_VSUB, 0, KEY_TYPE_DBL_CLICK, KEY_DEAL_FILTER_UP }, alarm_key_deal_set_sdd_io_id25 },  
    /*! 结束标志 */
    { { KEY_NULL, 0, KEY_TYPE_NULL, 0 }, NULL },
};

app_result_e alarm_key_deal_pause_alarm(void)
{
    return RESULT_ALARM_SNOOZE;
}

app_result_e alarm_key_deal_stop_alarm(void)
{
    return RESULT_ALARM_CLOSE;
}

app_result_e alarm_key_deal_non_alarm(void)
{
    return RESULT_NULL;
}

app_result_e alarm_key_deal_set_sdd_io_id16(void)
{
    return RESULT_PLAY_SDD_FID16;
}

app_result_e alarm_key_deal_set_sdd_io_id17(void)
{
    return RESULT_PLAY_SDD_FID17;
}
app_result_e alarm_key_deal_set_sdd_io_id18(void)
{
    return RESULT_PLAY_SDD_FID18;
}
app_result_e alarm_key_deal_set_sdd_io_id19(void)
{
    return RESULT_PLAY_SDD_FID19;
}
app_result_e alarm_key_deal_set_sdd_io_id20(void)
{
    return RESULT_PLAY_SDD_FID20;
}app_result_e alarm_key_deal_set_sdd_io_id21(void)
{
    return RESULT_PLAY_SDD_FID21;
}
app_result_e alarm_key_deal_set_sdd_io_id22(void)
{
    return RESULT_PLAY_SDD_FID22;
}
app_result_e alarm_key_deal_set_sdd_io_id23(void)
{
    return RESULT_PLAY_SDD_FID23;
}
app_result_e alarm_key_deal_set_sdd_io_id24(void)
{
    return RESULT_PLAY_SDD_FID24;
}

app_result_e alarm_key_deal_set_sdd_io_id25(void)
{
    return RESULT_PLAY_SDD_FID25;
}







const sys_event_map_t __section__(".rodata.se_maplist") alarm_playing_se_maplist[] =
{
    { { MSG_SD_IN, 0 }, NULL },
    { { MSG_UH_IN, 0 }, NULL },
    { { MSG_LINEIN_IN, 0 }, NULL },
    { { MSG_USB_STICK, 0 }, NULL },

    /*! 结束标志 */
    { { MSG_NULL, 0 }, NULL },
};

/* COMMON set/get/control类命令回调函数注册表 */
const rcp_cmd_cb_t __section__(".rodata.alarm_rcp") alarm_rcp_cmd_cb_tbl[] =
{
    //{ RCP_CMD_CONTROL, CONTROL_ALARM_SNOOZE, (void *) alarm_alarm_snooze }, //闹钟SNOOZE
    //{ RCP_CMD_CONTROL, CONTROL_ALARM_SHUTOFF, (void *) alarm_alarm_close }, //闹钟关闭

    /* 结束项 */
    { 0, 0, NULL },
};

/*app_result_e alarm_alarm_snooze(uint32 data1, uint32 data2)
{
    g_rcp_ctrl_quit_flag = TRUE;
    return RESULT_ALARM_SNOOZE;
}

app_result_e alarm_alarm_close(uint32 data1, uint32 data2)
{
    g_rcp_ctrl_quit_flag = TRUE;
    return RESULT_ALARM_CLOSE;
}*/

void alarm_rcp_var_init(void)
{
    com_rcp_set_callback(alarm_rcp_cmd_cb_tbl, NULL);
}

void alarm_rcp_var_exit(void)
{
    com_rcp_set_callback(NULL, NULL);
}


uint32 g_loop_play_ms = 0;


//SD扩展区读数据,回调函数，不能放在bank



#if 1

int sdd_open(sdd_open_arg_t* arg)
{
    g_loop_play_ms = arg->loop_interval_ms;
    return ((int)sys_sd_fopen_ext(arg->file_name, arg->id));
}



//SD扩展区读数据,回调函数，不能放在bank

int sdd_read(uint8*buff, uint32 size, uint32 len, storage_io_t *io)
{
    uint32 read_total = 0;
    int ret = 0;
    sd_file_t *fp = io->handel;
    uint32 f_size = (fp->endaddr - fp->startaddr) + 1;
    uint8 seek_cnt = 0;
    uint32 mp3_id3_size = *io->loop_play.mp3_id3_size;

    read_again:
    
    ret = (int32) sys_sd_fread(fp, buff, len);
    if(g_loop_play_ms != 0)
    {
        goto jump_end;
    }
    read_total = sys_sd_ftell(fp) - fp->startaddr;//tell这种方式更可靠 

    //seek后，可能不是512对齐，最后 的数据补0
    if((ret < len) && (ret > 0))
    {
        libc_memset(buff + ret, 0x00, len - ret);
        ret = len;
    }
    
    if((read_total <= (mp3_id3_size + 2048))||(f_size <= (read_total + 2048)))
    {
        *io->loop_play.clr_Energy = LOOP_PLAY_ENERGY;//无缝播放能量值
    }
    else
    {
        *io->loop_play.clr_Energy = 0;
    }
    if(f_size <= read_total)//提前seek
    {
        sys_sd_fseek(fp, SEEK_SET, mp3_id3_size);
    }
    
    if((ret < 1) && (seek_cnt < 3))
    {
        seek_cnt++;
        goto read_again;
    }
    jump_end: 
    return ret;
}




//SD扩展区操作数据seek,回调函数，不能放在bank
int sdd_seek(storage_io_t *io, int32 ofs, int32 whence)
{
    return ((int32)sys_sd_fseek(io->handel, whence, ofs));
}



//SD扩展区操作数据tell，告诉当前的位置
//回调函数，不能放在bank
int sdd_tell(storage_io_t *io)
{
    return ((int32) sys_sd_ftell(io->handel));
}

#endif


//闹钟响显示
static void __alarm_ring_view_redraw(void)
{
    uint16 hour, minute;

    //hour = g_p_alarm_ring->alarm_time.hour;
    //minute = g_p_alarm_ring->alarm_time.minute;

    //hour = (g_display_sec / 120) % 100;
    //minute = (g_display_sec/2) % 60;


    led_display(LCD_COL, 0xff, 0); //清除:号
    led_display(LCD_FM, 0xff, 0); //清楚.号及FM标记
    led_display(LCD_NO1, 0xff, 0); //清楚.号及FM标记
    
    led_display(0,NUM_0, 1);
    led_display(1,NUM_0, 1);

    led_display(2, (g_sdd_open_parm.id / 10) + NUM_0, 1);
    led_display(3, (g_sdd_open_parm.id % 10) + NUM_0, 1);
    //led_display(LCD_COL, 0xff, 0);

   

    //g_p_alarm_ring->twinkling_flag = !g_p_alarm_ring->twinkling_flag;
}

void alarm_playing_main_view(view_update_e mode)
{
    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        {
            g_p_alarm_ring->twinkling_flag = TRUE;
            g_p_alarm_ring->twinkling_timer_id = set_app_timer(APP_TIMER_ATTRB_UI, 500, alarm_ring_handle);
            g_p_alarm_ring->overtime_flag = FALSE;
#if (SUPPORT_LED_DRIVER == 1)
            if (g_comval.support_led_display == 1)
            {
                __alarm_ring_view_redraw();
            }
#endif
        }
        break;
        
        case VIEW_UPDATE_DESTROY:
        {
            if (g_p_alarm_ring->twinkling_timer_id != -1)
            {
                kill_app_timer(g_p_alarm_ring->twinkling_timer_id);
            }
        }
        break;

        case VIEW_UPDATE_REDRAW:
        case VIEW_UPDATE_STATUS:
        {
#if (SUPPORT_LED_DRIVER == 1)
            if (g_comval.support_led_display == 1)
            {
                __alarm_ring_view_redraw();
            }
#endif
        }
        break;

        default:
        break;
    }
}

//闹钟响2Hz定时处理
void check_alarm_overtime(void)
{
    /*
    time_t *p_start_alarm = &(g_p_alarm_ring->cur_alarm_record.last_time);
    uint32 start_second, cur_second;
    if (g_has_next_alarm == TRUE)
    {
        if ((g_next_alarm_time.hour == g_p_alarm_ring->alarm_time.hour)
                && (g_next_alarm_time.minute == g_p_alarm_ring->alarm_time.minute)
                && (g_next_alarm_time.second == g_p_alarm_ring->alarm_time.second))
        {
            g_next_alarm_already_ring = TRUE;
            return;
        }
    }

    start_second = p_start_alarm->minute * 60;
    start_second += p_start_alarm->second;

    sys_get_time(&(g_p_alarm_ring->alarm_time));

    cur_second = g_p_alarm_ring->alarm_time.minute * 60;
    cur_second += g_p_alarm_ring->alarm_time.second;

    if (cur_second < start_second)
    {
        cur_second += 3600; //越过小时
    }

    if ((cur_second - start_second) >= (g_comval.alarm_overtime * 60))
    {
        g_p_alarm_ring->overtime_flag = TRUE;
    }
    */
    g_display_sec++;
}


/******************************************************************************/
/*!
 * \par  Description:
 *    TOOLS 应用闹钟播放场景主函数
 * \param[in]    none
 * \param[out]   none
 * \return       app_result_e 返回 RESULT_APP_QUIT 或 RESULT_NULL 等
 * \ingroup
 * \note
 *******************************************************************************/
uint8 g_temp_buffer[512];
app_result_e alarm_ring_entry(void)
{
    app_result_e result = RESULT_NULL;
    bool alarm_play_flag = TRUE;
    uint32 temp_flag;
    int ret;
    bool first_alarm_play_init_flag = TRUE;
    uint8 alarm_idx = 0;

    //g_next_alarm_already_ring = FALSE;
    //创建主视图
    {
        create_view_param_t create_view_param;

        create_view_param.type = VIEW_TYPE_MAIN;
        create_view_param.unknown_key_deal = UNKNOWN_KEY_IGNORE;
        create_view_param.overtime = 0;
        create_view_param.ke_maplist = alarm_playing_ke_maplist;
        create_view_param.se_maplist = alarm_playing_se_maplist;
        create_view_param.view_cb = alarm_playing_main_view;
        com_view_add(APP_VIEW_ID_MAIN, &create_view_param);
    }

    //alarm_rcp_var_init();
    com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NOBLOCK, (void *) TTS_PLAY_MUSIC);
    //等待TTS结束
    if (com_tts_state_play_wait() == TTS_PLAY_RET_BY_KEY)
    {
        sys_os_time_dly(10); //等待100ms，以便短按抬起能够完成
    }
    alarm_next_again:

    alarm_idx = 0;

    //初始化闹钟播放
    //g_app_info_state.ring_alarm = TRUE;

    alarm_play_init_again:

    alarm_play_flag = alarm_play_init(g_p_alarm_ring);
    if (alarm_play_flag == FALSE)
    {
        result = RESULT_ALARM_CLOSE;
        goto init_fail;
    }

    while (1)
    {
        if (alarm_check_status() == FALSE) //出错之后退出闹钟
        {
            result = RESULT_ALARM_CLOSE;
            break;
        }
        result = com_view_loop();
        if(result == RESULT_PLAY_SDD_FID16)
        {
            set_SDD_file_IO(16, TRUE);
           
            //alarm_play();
        }
        else if(result == RESULT_PLAY_SDD_FID17)
        {
            set_SDD_file_IO(17, TRUE);
            //alarm_play();
        }
        else if(result == RESULT_PLAY_SDD_FID18)
        {
            set_SDD_file_IO(18, TRUE);
            //alarm_play();
        }
        else if(result == RESULT_PLAY_SDD_FID19)
        {
            set_SDD_file_IO(19, TRUE);
            //alarm_play();
        }
        else if(result == RESULT_PLAY_SDD_FID20)
        {
            set_SDD_file_IO(20, TRUE);
            //alarm_play();
        }
        else if(result == RESULT_PLAY_SDD_FID21)
        {
            set_SDD_file_IO(21, TRUE);
            //alarm_play();
        }
        else if(result == RESULT_PLAY_SDD_FID22)
        {
            set_SDD_file_IO(22, TRUE);
            //alarm_play();
        }
        else if(result == RESULT_PLAY_SDD_FID23)
        {
            set_SDD_file_IO(23, TRUE);
            //alarm_play();
        }
        else if(result == RESULT_PLAY_SDD_FID24)
        {
            set_SDD_file_IO(24, TRUE);
            //alarm_play();
        }
        else if(result == RESULT_PLAY_SDD_FID25)
        {
            set_SDD_file_IO(25, TRUE);
            //alarm_play();
        }
        else if (result > RESULT_COMMON_RESERVE)
        {
            break;
        }
        else
        {
            ;//nothing
        }
        //挂起10ms，多任务调度
        sys_os_time_dly(1);
    }

    //退出闹钟播放
    alarm_play_deinit();
    init_fail:

    //alarm_rcp_var_exit();
    return result;
}

//闹钟响2Hz定时处理
void alarm_ring_handle(void)
{
    //if (g_comval.alarm_overtime != 0)
    {
        check_alarm_overtime();
    }

#if (SUPPORT_LED_DRIVER == 1)
    if (g_comval.support_led_display == 1)
    {
        alarm_playing_main_view(VIEW_UPDATE_REDRAW);
    }
#endif
}


