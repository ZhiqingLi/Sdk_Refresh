#include "app_radio.h"

app_result_e radio_play_key_deal_next_freq(void);
app_result_e radio_play_key_deal_prev_freq(void);
app_result_e radio_play_key_deal_seek_next(void);
app_result_e radio_play_key_deal_seek_prev(void);
app_result_e radio_play_key_deal_switch_next_preset(void);
app_result_e radio_play_key_deal_switch_prev_preset(void);
app_result_e radio_play_key_deal_auto_search(void);
app_result_e radio_play_key_deal_switch_band(void);
app_result_e radio_play_key_deal_play_mute(void);
app_result_e radio_play_key_deal_play_pause(void);
app_result_e radio_play_key_deal_number_input(void);

const key_event_map_t __section__(".rodata.ke_maplist") radio_play_ke_maplist[] =
{
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
    /*! 切换到下一个频点 */
    { { KEY_NEXT_VOLADD, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, radio_play_key_deal_switch_next_preset },
    { { KEY_NEXT, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, radio_play_key_deal_switch_next_preset },
    /*! 切换到上一个频点 */
    { { KEY_PREV_VOLSUB, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, radio_play_key_deal_switch_prev_preset },
    { { KEY_PREV, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, radio_play_key_deal_switch_prev_preset },
    /*! 向下手动搜台 */
    { { KEY_NEXT, 0, KEY_TYPE_LONG | KEY_TYPE_HOLD, KEY_DEAL_FILTER }, radio_play_key_deal_seek_next },
    /*! 向上手动搜台 */
    { { KEY_PREV, 0, KEY_TYPE_LONG | KEY_TYPE_HOLD, KEY_DEAL_FILTER }, radio_play_key_deal_seek_prev },
#else
    /*! 切换到下一个频点 */
    { { KEY_NEXT_VOLADD, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, radio_play_key_deal_next_freq },
    { { KEY_NEXT, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, radio_play_key_deal_next_freq },
    /*! 切换到上一个频点 */
    { { KEY_PREV_VOLSUB, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, radio_play_key_deal_prev_freq },
    { { KEY_PREV, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, radio_play_key_deal_prev_freq },
    /*! 向下手动搜台 */
    { { KEY_NEXT, 0, KEY_TYPE_LONG | KEY_TYPE_HOLD, KEY_DEAL_FILTER }, radio_play_key_deal_seek_next },
    /*! 向上手动搜台 */
    { { KEY_PREV, 0, KEY_TYPE_LONG | KEY_TYPE_HOLD, KEY_DEAL_FILTER }, radio_play_key_deal_seek_prev },
    /*! 切换到下一电台 */
    { { KEY_PLAY, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, radio_play_key_deal_switch_next_preset },
#endif
    /*! 切换到下一电台 */
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
    { { KEY_PLAY, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, radio_play_key_deal_play_pause },
    { { KEY_PHONEPLAY, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, radio_play_key_deal_play_pause },
#else
    { { KEY_PLAY, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, radio_play_key_deal_switch_next_preset },
#endif
    /*! 自动搜台 */
    //#if(_DEBUG == 1)
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
    { { KEY_PLAY, 0, KEY_TYPE_LONG | KEY_TYPE_HOLD, KEY_DEAL_FILTER}, radio_play_key_deal_auto_search},
    { { KEY_PHONEPLAY, 0, KEY_TYPE_LONG | KEY_TYPE_HOLD, KEY_DEAL_FILTER}, radio_play_key_deal_auto_search},
#endif
    /*! 循环切换波段 */
    { { KEY_REPEAT, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, radio_play_key_deal_switch_band },
    /*! 静音 */
    { { KEY_MUTE, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, radio_play_key_deal_play_mute },
    /*! 数字频点 */
    { { KEY_NUMBER, 0, KEY_TYPE_SHORT_UP | KEY_TYPE_LONG_UP, KEY_DEAL_NULL }, radio_play_key_deal_number_input },

    /*! 结束标志 */
    { { KEY_NULL, 0, KEY_TYPE_NULL, KEY_DEAL_NULL }, NULL },
};

app_result_e radio_play_key_deal_next_freq(void)
{
    app_result_e result;

    if (g_radio_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    result = change_freq(DIR_UP);

    return result;
}

app_result_e radio_play_key_deal_prev_freq(void)
{
    app_result_e result;

    if (g_radio_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    result = change_freq(DIR_DOWN);

    return result;
}

app_result_e radio_play_key_deal_seek_next(void)
{
    app_result_e result;

    if (g_radio_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    result = RESULT_MANUAL_SEARCH_UP;

    return result;
}

app_result_e radio_play_key_deal_seek_prev(void)
{
    app_result_e result;

    if (g_radio_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    result = RESULT_MANUAL_SEARCH_DOWN;

    return result;
}

app_result_e radio_play_key_deal_switch_next_preset(void)
{
    app_result_e result;

    if (g_radio_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    result = select_station(SELECT_NEXT_DIRECTION);

    return result;
}

app_result_e radio_play_key_deal_switch_prev_preset(void)
{
    app_result_e result;

    if (g_radio_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    result = select_station(SELECT_PREV_DIRECTION);

    return result;
}

app_result_e radio_play_key_deal_auto_search(void)
{
    if (g_radio_init_flag == TRUE)
    {
        return RESULT_NULL;
    }
    if(g_searching_flag == FALSE)
    {
        return RESULT_AUTO_SEARCH;
    }
    return RESULT_NULL;
}

__section__(".text.bank2") app_result_e radio_play_key_deal_switch_band(void)
{
    app_result_e result;
    radio_band_e old_band;

    if (g_radio_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    old_band = g_radio_config.band_mode;
    //循环切换波段
    if (g_radio_config.band_mode < Band_Europe)
    {
        g_radio_config.band_mode++;
    }
    else
    {
        g_radio_config.band_mode = Band_China_US;
    }

    radio_do_switch_band(old_band, g_radio_config.band_mode);

    result = RESULT_NULL;

    return result;
}

app_result_e radio_play_key_deal_play_pause(void)
{
    if (g_radio_init_flag == TRUE)
    {
        return RESULT_NULL;
    }
    if (g_playing_flag == TRUE)
    {
        g_playing_flag = FALSE;
        radio_send_msg(NULL, NULL, MSG_FMENGINE_PAUSE);
        radio_set_mute(SetMUTE);
        com_tts_state_play(TTS_MODE_ONLYONE, (void*)TTS_PAUSE_SONG);
        
    }
    else
    {
        com_tts_state_play(TTS_MODE_ONLYONE, (void*)TTS_PLAY_SONG);
        radio_set_mute(releaseMUTE);
        radio_send_msg(NULL, NULL, MSG_FMENGINE_PLAY);
        g_playing_flag = TRUE;
    }

    return RESULT_NULL;
}

app_result_e radio_play_key_deal_play_mute(void)
{
    if (g_radio_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    com_set_mute(!get_mute_enable());
    if (get_mute_enable() == FALSE)
    {
        radio_set_mute(SetMUTE);
        g_playing_flag = FALSE;
    }
    else
    {
        radio_set_mute(releaseMUTE);
        g_playing_flag = TRUE;
    }

    return RESULT_NULL;
}

__section__(".text.bank2") app_result_e radio_play_key_deal_number_input(void)
{
    app_result_e result;
    uint8 number = (uint8) (g_p_view_ke->val - KEY_NUM0);

    if (g_radio_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    result = radio_play_key_digital_store(number);

    return result;
}

//切换波段
//old_band: 当前频段
//new_band: 新频段
__section__(".text.bank2") void radio_do_switch_band(radio_band_e old_band, radio_band_e new_band)
{
    uint8 *pstatus_tmp;

    //play freq
    uint32 enter_preq;

    uint8 num;
    //save old band infomation
    if (old_band == Band_Japan)
    {
        pstatus_tmp = (uint8 *) &g_radio_config.FMStatus_jp;
    }
    else if (old_band == Band_Europe)
    {
        pstatus_tmp = (uint8 *) &g_radio_config.FMStatus_eu;
    }
    else
    {
        pstatus_tmp = (uint8 *) &g_radio_config.FMStatus_us;
    }
    //save info
    libc_memcpy(pstatus_tmp, &g_radio_config.FMStatus, sizeof(fm_play_status_t));

    //改变波段
    radio_set_band(new_band);

    //初始化频率指针
    init_point_freq();
    /* 当前播放频段*/
    if (new_band == Band_Japan)
    {
        pstatus_tmp = (uint8 *) &g_radio_config.FMStatus_jp;
    }
    else if (new_band == Band_Europe)
    {
        pstatus_tmp = (uint8 *) &g_radio_config.FMStatus_eu;
    }
    else
    {
        pstatus_tmp = (uint8 *) &g_radio_config.FMStatus_us;
    }
    //get new band play status
    libc_memcpy(&g_radio_config.FMStatus, pstatus_tmp, sizeof(fm_play_status_t));

    enter_preq = g_radio_config.FMStatus.freq;

    if ((enter_preq <= max_freq[g_radio_config.band_mode]) && (enter_preq >= min_freq[g_radio_config.band_mode]))
    {
        num = whether_in_list(enter_preq, pfreq);
        //切换后的频点是未保存频点
        if (num == 0xff)
        {
            g_radio_config.FMStatus.station = 0;
            com_view_update(RADIO_MAIN_VIEW_PLAY);
        }
        else
        {
            //切换后的频点是已保存频点
            g_radio_config.FMStatus.station = num;
            //电台号界面
            radio_station_create_msg_view(2000);
            //更新频点播放
            radio_set_freq(g_radio_config.FMStatus.freq);
            return;
        }
    }
    else
    {
        //如果预设电台表中不存在,则播放该波段最小频率
        g_radio_config.FMStatus.freq = min_freq[new_band];
    }

    //更新频点播放
    radio_set_freq(g_radio_config.FMStatus.freq);
}

//切换到上一个或下一个频率
//dir: 切换方向
app_result_e change_freq(FM_SeekDir_e dir)
{
    app_result_e ret = RESULT_NULL;
    uint8 space;
    uint8 num;
    //步长
    space = 100;

    if (g_radio_config.band_mode == Band_Europe)
    {
        //欧洲频段,  以50k为步进
        space = 50;
    }

    if (dir == DIR_UP)
    {
        if (g_radio_config.FMStatus.freq < max_freq[g_radio_config.band_mode])
        {
            //向上切换频率
            g_radio_config.FMStatus.freq += space;
        }
        else
        {
            //最大频点切换到最小
            g_radio_config.FMStatus.freq = min_freq[g_radio_config.band_mode];
        }
    }
    else
    {
        if (g_radio_config.FMStatus.freq > min_freq[g_radio_config.band_mode])
        {
            //向下切换
            g_radio_config.FMStatus.freq -= space;
        }
        else
        {
            //最小频点切换到最大
            g_radio_config.FMStatus.freq = max_freq[g_radio_config.band_mode];
        }
    }

    //判断切换后的频点是否在当前所播放的预设电台表中
    num = whether_in_list(g_radio_config.FMStatus.freq, pfreq);

    //切换后的频点是未保存频点
    if (num == 0xff)
    {
        //没有在预设电台表中
        g_radio_config.FMStatus.station = 0;
        com_view_update(RADIO_MAIN_VIEW_PLAY);
        //更新频点播放
        radio_set_freq(g_radio_config.FMStatus.freq);
    }
    else
    {
        //切换后的频点是已保存频点
        g_radio_config.FMStatus.station = num;
        //电台号界面
        radio_station_create_msg_view(2000);

        //更新频点播放
        radio_set_freq(g_radio_config.FMStatus.freq);
    }

    return ret;
}

//处理数字输入按键消息
//number: 0~9
/*******************************************************************************/
__section__(".text.bank2") app_result_e radio_play_key_digital_store(uint8 number)
{
    //buffer满则清空重新保存,并且离开数字点播界面也要清空
    if ((g_digital_count >= MAX_DIGITAL_COUNT) || (g_digit_input_flag == FALSE))
    {
        g_digital_count = 0;
        libc_memset(g_digital_buf, 0, MAX_DIGITAL_COUNT);
    }

    g_digital_buf[g_digital_count] = number;

    g_digital_count++;

    g_digit_input_flag = TRUE;

    //显示数字
    radio_digit_create_sub_view(2000);

    return RESULT_NULL;
}

//切换到下一个已保存电台
//direction: 切换电台方向，0表示切换到上一个电台，1表示切换到下一个电台
app_result_e select_station(uint8 direction)
{
    app_result_e ret = RESULT_NULL;

    //循环查找
    uint8 i, cur_num;
    //电台频率值
    uint32 station_freq;

    radio_set_mute(SetMUTE);

    //预设列表中有已保存电台
    if (g_radio_config.FMStatus.station_count > 0)
    {
        if (g_radio_config.FMStatus.station == 0)
        {
            //当前不在预设列表中的频点上，切换到第一个频点
            g_radio_config.FMStatus.station = 1;
            for (i = 0; i < g_radio_config.FMStatus.station_count; i++)
            {
                if (pfreq[i] >= g_radio_config.FMStatus.freq)
                {
                    g_radio_config.FMStatus.station = i + 1;
                    break;
                }
            }
        }
        else
        {
            if (direction == SELECT_NEXT_DIRECTION)
            {
                if (g_radio_config.FMStatus.station < g_radio_config.FMStatus.station_count)
                {
                    //切换电台号
                    g_radio_config.FMStatus.station++;
                }
                else
                {
                    //最后回第一项
                    g_radio_config.FMStatus.station = 1;
                }
            }
            else
            {
                if (g_radio_config.FMStatus.station > 1)
                {
                    //切换电台号
                    g_radio_config.FMStatus.station--;
                }
                else
                {
                    //最后回第一项
                    g_radio_config.FMStatus.station = g_radio_config.FMStatus.station_count;
                }
            }
        }
        cur_num = g_radio_config.FMStatus.station;
        for (i = 0; i < g_radio_config.FMStatus.station_count; i++)
        {
            station_freq = *(pfreq + cur_num - 1);
            if ((station_freq >= min_freq[g_radio_config.band_mode]) && (station_freq
                    <= max_freq[g_radio_config.band_mode]))
            {
                //找到有效台，退出
                break;
            }
            else
            {
                //向下查找
                cur_num++;
                if (cur_num > g_radio_config.FMStatus.station_count)
                {
                    cur_num = 1;
                }
            }
        }
        //找到有效电台
        g_radio_config.FMStatus.station = cur_num;
        //通过电台号获取频率值
        g_radio_config.FMStatus.freq = *(pfreq + g_radio_config.FMStatus.station - 1);

        //刷新电台号
        radio_station_create_msg_view(2000);
    }
    else
    {
        //没有电台也要播报频点
        radio_freq_tts_play(g_radio_config.FMStatus.freq);
    }

    //更新频点播放(没有保存电台也需要重新设置频点)
    radio_set_freq(g_radio_config.FMStatus.freq);
    radio_set_mute(releaseMUTE);
    return ret;
}

//digital如果小于g_radio_config.FMStatus.station_count
//  (预设电台总数(不超过MAX_STATION_COUNT))
//  那么，digital就表示预设电台的数组的下标。
//  否则digital表示的是频率(单位50kHz)
//  (此时digital的最小值是76M/50k=1520,
//  所以,g_digital_count肯定被设置为4了)
app_result_e radio_digital_to_chanel(uint16 digital)
{
    //电台频率
    uint32 chanel_freq;
    uint8 chanel_num;

    radio_set_mute(SetMUTE);

    if (digital <= g_radio_config.FMStatus.station_count)
    {
        //选择电台号
        chanel_freq = *(pfreq + digital - 1);
        if ((chanel_freq >= min_freq[g_radio_config.band_mode]) && (chanel_freq <= max_freq[g_radio_config.band_mode]))
        {
            //是有效电台
            g_radio_config.FMStatus.station = (uint8) digital;
            //通过电台号获取频率值
            g_radio_config.FMStatus.freq = chanel_freq;
            //电台号界面
            radio_station_create_msg_view(2000);
            //更新频点播放
            radio_set_freq(g_radio_config.FMStatus.freq);
        }
    }
    else if (g_digital_count >= 3)
    {
        //频率数字
        chanel_freq = digital * 50;
        if ((chanel_freq >= min_freq[g_radio_config.band_mode]) && (chanel_freq <= max_freq[g_radio_config.band_mode]))
        {
            g_radio_config.FMStatus.freq = chanel_freq;
            //输入频点是未保存频点
            chanel_num = whether_in_list(chanel_freq, pfreq);
            if (chanel_num == 0xff)
            {
                //没有在预设电台表中
                g_radio_config.FMStatus.station = 0;
                com_view_update(RADIO_MAIN_VIEW_PLAY);
            }
            else
            {
                //切换后的频点是已保存频点
                g_radio_config.FMStatus.station = chanel_num;
                //电台号界面
                radio_station_create_msg_view(2000);
            }
            //更新频点播放
            radio_set_freq(g_radio_config.FMStatus.freq);
        }
    }
    else
    {
        ; //QAC
    }

    radio_set_mute(releaseMUTE);
    return RESULT_NULL;
}

//数字选台
app_result_e radio_play_key_digital_sure(void)
{
    app_result_e deal_ret;
    //选中电台
    uint16 digital_num = 0;
    uint16 curbit_multi = 1;
    uint8 i;
    for (i = 1; i < g_digital_count; i++)
    {
        curbit_multi = curbit_multi * 10;
    }
    for (i = g_digital_count; i > 0; i--)
    {
        digital_num = digital_num + g_digital_buf[g_digital_count - i] * curbit_multi;
        curbit_multi = curbit_multi / 10;
    }
    //digital_num的单位是:100kHz

    deal_ret = radio_digital_to_chanel(digital_num * 2); //以50KHz为单位
    //清空buffer
    g_digital_count = 0;
    return deal_ret;
}

