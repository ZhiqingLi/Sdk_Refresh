#include "app_radio.h"

//半自动搜台初始化
//dir: 搜台方向
//返回值: 步进
static __section__(".text.bank2") uint8 init_start_freq(FM_SeekDir_e dir)
{
    uint8 space;
    //搜台步进
    space = 100;
    //欧洲频段,步长为50
    if (g_radio_config.band_mode == Band_Europe)
    {
        //欧洲波段或者在用户模式，步进调整为50K
        space = 50;
    }

    //设置初始判断频率
    if (dir == DIR_UP)
    {
        if (g_radio_config.FMStatus.freq < max_freq[g_radio_config.band_mode])
        {
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
        if (g_radio_config.FMStatus.freq> min_freq[g_radio_config.band_mode])
        {
            g_radio_config.FMStatus.freq -= space;
        }
        else
        {
            //最小频点切换到最大
            g_radio_config.FMStatus.freq = max_freq[g_radio_config.band_mode];
        }
    }
    return space;
}

//看一个频率是否在Auto_tab[0..cur_max - 1]中存在
//返回: Auto_tab的下标
static uint8 fliter_exist_freq(uint32 input_freq, uint8 cur_max)
{
    uint8 number;
    for (number = 0; number < cur_max; number++)
    {
        if (input_freq == Auto_tab[(number)])
        {
            break;
        }
    }
    return number;
}

//插入Auto_tab
//前提：Auto_tab是按照从小到大的顺序， 初始化时，它都是0
//返回值：如果插入成功，返回插入位置的Auto_tab的下标
//          如果已经在Auto_tab存在，返回-1
//          如果已满，返回-2
//          如果出错，返回-3
static int32 _insert_tab(uint32 freq)
{
    uint8 n;
    uint8 nLast;
    uint8 n2;

    for (n = 0; n < ARR_COUNT(Auto_tab); n++)
    {
        if (Auto_tab[n] == 0)
        {
            Auto_tab[n] = freq;
            return (int32) n;
        }

        if (Auto_tab[n] == freq)
        {
            //这种情况应该不存在，因为在调用本函数之前，已经进行了检查
            return -1;
        }

        if (Auto_tab[n] < freq)
        {
            continue;
        }

        //Auto_tab[n] > freq
        //往后移动

        //最后一个非0数的下标
        for (nLast = ARR_COUNT(Auto_tab) - 1; nLast >= n; nLast--)
        {
            if (Auto_tab[nLast] != 0)
            {
                break;
            }
        }
        if (nLast == (ARR_COUNT(Auto_tab) - 1))
        {
            //已经插不进去了, 满了
            return -2;
        }

        for (n2 = nLast; n2 >= n; n2--)
        {
            Auto_tab[n2 + 1] = Auto_tab[n2];
        }
        Auto_tab[n] = freq;
        return (uint32) n;
    }

    //不会执行到这里
    return -3;
}

static bool _is_search_over(uint32 freq, uint32 end, FM_SeekDir_e dir)
{
    if (((freq >= end) && (dir == DIR_UP)) || ((freq <= end) && (dir == DIR_DOWN)))
    {
        return TRUE;
    }

    return FALSE;
}

//硬件全自动搜台处理
app_result_e deal_hard_auto_seek(FM_SeekDir_e dir)
{
    app_result_e ret = RESULT_NULL;
    uint8 tab_num, i;
    bool result, need_restart = TRUE;
    uint32 current_freq, end_freq;
    uint8 space;
    uint32 last_timestamp, cur_timestamp;

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
    if (g_radio_config.hint_mode_cfg == 1)
    {
        led_duty_t fm_link_duty;
        fm_link_duty.cycle_count = -1;
        fm_link_duty.duty_on_time = 250;
        fm_link_duty.duty_off_time = 250;
        discrete_led_set(LED_ID_FM_PLAY, LED_TWINKLE, &fm_link_duty);
    }

#endif

    //设置当前波段, 主要是确定space
    radio_set_band(g_radio_config.band_mode);
    //起始频率
    current_freq = get_start_freq(dir);
    //结束频率
    end_freq = get_end_freq(dir);
    //电台个数
    tab_num = 0;

    //搜台步进
    if (g_radio_config.band_mode == Band_Europe)
    {
        //欧洲波段, 步进调整为50K
        space = 50;
    }
    else
    {
        space = 100;
    }

    PRINT_DBG("自动搜台(硬)");

    //显示当前频点
    g_search_current_freq = current_freq;
    com_view_update(RADIO_MAIN_VIEW_HARD_SEARCH);

    last_timestamp = sys_get_ab_timer();

    while (1)
    {
        if (need_restart == TRUE)
        {
            result = radio_set_hardseek(current_freq, dir);
            need_restart = FALSE;
            if (!result)
            {
                //硬件搜台启动失败
                break;
            }
        }

        //硬件搜台启动成功,  取是否完成标志
        result = radio_get_seekflag();

        if (result == TRUE)
        {
            //获取当前频率
            radio_get_freq();
            current_freq = g_engine_status.FM_CurrentFreq;

            if ((g_engine_status.STC_flag == HARDSEEK_NOT_START) || (_is_search_over(current_freq, end_freq, dir)
                    == TRUE))
            {
                //此轮搜台完成
                radio_break_hardseek();
                break;
            }

            if ((g_engine_status.STC_flag == HARDSEEK_COMPLETE) || (g_engine_status.STC_flag
                    == HARDSEEK_COMPLETE_NOT_VALID))
            {
                if ((g_engine_status.STC_flag == HARDSEEK_COMPLETE) && (!should_be_passed(current_freq)))
                {
                    //保存有效电台
                    i = fliter_exist_freq(current_freq, tab_num);
                    if ((i == tab_num) && (_insert_tab(current_freq) >= 0))
                    {
                        tab_num++;
                    }
                }

                //显示当前频点
                g_search_current_freq = current_freq;
                com_view_update(RADIO_MAIN_VIEW_HARD_SEARCH);

                if (dir == DIR_UP)
                {
                    current_freq += space;
                }
                else
                {
                    current_freq -= space;
                }

                if ((_is_search_over(current_freq, end_freq, dir) == TRUE))
                {
                    //此轮搜台完成
                    radio_break_hardseek();
                    break;
                }
                else if (g_engine_status.STC_flag == HARDSEEK_COMPLETE)
                {
                    uint32 j = 0;
                    radio_break_hardseek();
                    //for play
                    radio_set_mute(releaseMUTE);
                    while (1)
                    {
                        ret = com_view_loop();
                        if (ret > RESULT_COMMON_RESERVE)
                        {
                            goto hard_serach_over;
                        }
                        sys_os_time_dly(1);
                        j++;
                        if (j >= 100)
                        {
                            break;
                        }
                    }

                    radio_set_mute(SetMUTE);
                    need_restart = TRUE;
                    last_timestamp = sys_get_ab_timer();
                    continue;
                }
                else
                {
                    ;//
                }
            }

            //未到截至频点, 需启动新一轮搜台
            need_restart = TRUE;
        }

        //电台列表已满
        if (tab_num >= MAX_STATION_COUNT)
        {
            radio_break_hardseek();
            break;
        }

        ret = com_view_loop();
        if (ret > RESULT_COMMON_RESERVE)
        {
            radio_break_hardseek();
            break;
        }
        if (g_radio_config.hint_mode_cfg == 0)
        {
            cur_timestamp = sys_get_ab_timer();
            if ((cur_timestamp - last_timestamp) >= 1500)
            {
                //暂时中断搜台，以保证播放提示音时切换BANK不会影响到搜台效果
                radio_break_hardseek();
                radio_get_freq();
                current_freq = g_engine_status.FM_CurrentFreq;
                need_restart = TRUE;
                keytone_play(KEYTONE_WARNING, KEYTONE_BLOCK);

                last_timestamp = cur_timestamp;
            }
        }
        //挂起10ms 多任务交互
        sys_os_time_dly(1);
    }

    hard_serach_over:

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
    if (g_radio_config.hint_mode_cfg == 1)
    {
        discrete_led_set(LED_ID_FM_PLAY, LED_ALWAYS_ON, NULL);
    }
#endif
    //保存电台数
    g_radio_config.FMStatus.station_count = tab_num;
    return ret;
}

//硬件半自动搜台处理
//dir: 搜台方向
app_result_e deal_hard_manual_search(FM_SeekDir_e dir)
{
    app_result_e ret = RESULT_NULL;
    bool result, need_restart = TRUE;
    uint32 current_freq, end_freq;
    uint8 num;
    uint8 space;
    uint32 last_timestamp, cur_timestamp;

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
    if (g_radio_config.hint_mode_cfg == 1)
    {
        led_duty_t fm_link_duty;
        fm_link_duty.cycle_count = -1;
        fm_link_duty.duty_on_time = 250;
        fm_link_duty.duty_off_time = 250;
        discrete_led_set(LED_ID_FM_PLAY, LED_TWINKLE, &fm_link_duty);
    }
#endif

    //设置当前波段, 主要是确定space
    radio_set_band(g_radio_config.band_mode);

    init_start_freq(dir);
    //起始频率
    current_freq = g_radio_config.FMStatus.freq;
    //结束频率
    end_freq = get_end_freq(dir);

    //搜台步进
    if (g_radio_config.band_mode == Band_Europe)
    {
        //欧洲波段, 步进调整为50K
        space = 50;
    }
    else
    {
        space = 100;
    }

    last_timestamp = sys_get_ab_timer();

    while (1)
    {
        if (need_restart == TRUE)
        {
            //判断当前频点是否是已保存频点
            //并更新电台号
            num = whether_in_list(g_radio_config.FMStatus.freq, pfreq);
            if (num == 0xff)
            {
                g_radio_config.FMStatus.station = 0;
            }
            else
            {
                g_radio_config.FMStatus.station = num;
            }
            //不显示开始搜索的频点
            //g_search_current_freq = g_radio_config.FMStatus.freq;
            //com_view_update(RADIO_MAIN_VIEW_HARD_SEARCH);

            //启动新一轮硬件seek
            result = radio_set_hardseek(current_freq, dir);
            need_restart = FALSE;
            if (!result)
            {
                //硬件搜台启动失败,返回到播放场景
                break;
            }
        }

        //硬件搜台启动成功,  取是否完成标志
        result = radio_get_seekflag();

        if (result == TRUE)
        {
            radio_get_freq();
            current_freq = g_engine_status.FM_CurrentFreq;

            if ((g_engine_status.STC_flag == HARDSEEK_NOT_START) || (_is_search_over(current_freq, end_freq, dir)
                    == TRUE))
            {
                //此轮搜台完成或seek fail, 退出
                radio_break_hardseek();
                break;
            }

            if ((g_engine_status.STC_flag == HARDSEEK_COMPLETE) || (g_engine_status.STC_flag
                    == HARDSEEK_COMPLETE_NOT_VALID))
            {
                //搜到台了
                if ((g_engine_status.STC_flag == HARDSEEK_COMPLETE) && (!should_be_passed(current_freq)))
                {
                    //真正的台
                    radio_break_hardseek();
                    break;
                }

                //是本身的干扰
                if (dir == DIR_UP)
                {
                    current_freq += space;
                }
                else
                {
                    current_freq -= space;
                }
                if (_is_search_over(current_freq, end_freq, dir) == TRUE)
                {
                    //此轮搜台完成
                    radio_break_hardseek();
                    break;
                }
            }

            //未到截至频点, 需启动新一轮搜台
            need_restart = TRUE;
        }
        else
        {
            //显示现在搜索到哪个频点了
            //但这样，会有I2C通讯、也可能有SPI通讯。
            //在有些方案上，这些通讯会干扰搜台，导致锁不住。
            //所以，请慎重选择。
#if 0
            radio_get_freq();
            g_search_current_freq = g_engine_status.FM_CurrentFreq;
            com_view_update(RADIO_MAIN_VIEW_HARD_SEARCH);
#endif
        }

        ret = com_view_loop();
        if (ret > RESULT_COMMON_RESERVE)
        {
            break;
        }
        if (g_radio_config.hint_mode_cfg == 0)
        {
            cur_timestamp = sys_get_ab_timer();
            if ((cur_timestamp - last_timestamp) >= 1500)
            {
                //暂时中断搜台，以保证播放提示音时切换BANK不会影响到搜台效果
                radio_break_hardseek();
                radio_get_freq();
                current_freq = g_engine_status.FM_CurrentFreq;
                need_restart = TRUE;
                keytone_play(KEYTONE_WARNING, KEYTONE_BLOCK);

                last_timestamp = cur_timestamp;
            }
        }
        //挂起10ms 多任务交互
        sys_os_time_dly(1);
    }

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
    if (g_radio_config.hint_mode_cfg == 1)
    {
        discrete_led_set(LED_ID_FM_PLAY, LED_ALWAYS_ON, NULL);
    }
#endif

    return ret;
}

//硬件搜台
//freq: 硬件搜台起始频率
//dir: 搜台方向
bool radio_set_hardseek(uint32 freq, uint8 dir)
{
    //seek paramter
    seek_param_t search_parm;
    //frequncy
    search_parm.freq = freq;
    //seek dir
    search_parm.dir = dir;

    return radio_send_msg(&search_parm, NULL, MSG_FMENGINE_SET_HARDSEEK);

}

//获取硬件搜台是否完成标志
bool radio_get_seekflag(void)
{
    bool bret = TRUE;

    bret = radio_send_msg(NULL, &g_engine_status.STC_flag, MSG_FMENGINE_AUTO_SEEK_OVER);

    if (g_engine_status.STC_flag == HARDSEEK_DOING)
    {
        bret = FALSE;
    }
    return bret;
}

//中断硬件搜台过程
bool radio_break_hardseek(void)
{
    return radio_send_msg(NULL, NULL, MSG_FMENGINE_SET_SEEKBREAK);
}
