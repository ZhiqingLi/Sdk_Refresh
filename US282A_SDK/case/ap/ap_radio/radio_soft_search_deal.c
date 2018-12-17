#include "app_radio.h"


uint32 Dec2Hex(uint32 h)
{
    uint32 d = 0;
    uint8 i = 0;
    while(h > 0)
    {
        d *= 10;
        d += h % 10;
        h /= 10;
        i++;
    }
    h = d;
    d = 0;
    while(i > 0)
    {
        d <<= 4;
        d += (h % 10);
        h /= 10;
        i--;
    }
    return d;
}


//半自动搜台初始化
//dir: 搜台方向
//返回值: 步进
static uint8 init_start_freq(FM_SeekDir_e dir)
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
        if (g_radio_config.FMStatus.freq > min_freq[g_radio_config.band_mode])
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
    int8 n;
    int8 nLast;
    int8 n2;

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

//软件全自动搜台处理
app_result_e deal_soft_auto_seek(FM_SeekDir_e dir)
{
    app_result_e ret = RESULT_NULL;
    uint8 space, tab_num;
    //起始、结束频点
    uint32 seek_freq, end_freq;
    bool new_freq = FALSE;
    uint8 seek_times = 0;
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

    //搜台步进
    space = 100;
    if (g_radio_config.band_mode == Band_Europe)
    {
        //欧洲波段, 步进调整为50K
        space = 50;
    }
    tab_num = 0;


    Seek_again:
    //获取起始频点
    seek_freq = get_start_freq(dir);
    //获取结束频点
    end_freq = get_end_freq(dir);
    PRINT_DBG("自动搜台(软)");

    last_timestamp = sys_get_ab_timer();

    while (1)
    {
        new_freq = FALSE;
        //判断当前频率是否有效台
        if (radio_set_softseek(seek_freq, dir) == TRUE)
        {
            if (!should_be_passed(seek_freq))
            {
                //保存有效电台
                if (fliter_exist_freq(seek_freq, tab_num) == tab_num)
                {
                    if (_insert_tab(seek_freq) >= 0)
                    {
                        tab_num++;
                        new_freq = TRUE;
                    }
                }
            }
            if(new_freq == TRUE)
            {
                uint32 j = 0;

                libc_print("FM search ok=",Dec2Hex(seek_freq),2);
                //need_test_play
                radio_set_mute(releaseMUTE);
                while (1)
                {
                    ret = com_view_loop();
                    if (ret > RESULT_COMMON_RESERVE)
                    {
                        break;
                    }
                    sys_os_time_dly(1);
                    j++;
                    if (j >= 100)
                    {
                        break;
                    }
                }
                radio_set_mute(SetMUTE);
                sys_os_time_dly(1);
                last_timestamp = sys_get_ab_timer();
                if (ret > RESULT_COMMON_RESERVE)
                {
                    break;
                }
            }
        }

        //电台列表已满
        if (tab_num >= MAX_STATION_COUNT)
        {
            break;
        }

        if (dir == DIR_UP)
        {
            if ((seek_freq + space) > end_freq)
            {
                //结束扫描,回到播放场景
                break;
            }
            else
            {
                //更新频点
                seek_freq += space;
            }
        }
        else
        {
            if ((seek_freq - space) < end_freq)
            {
                //结束扫描,回到播放场景
                break;
            }
            else
            {
                //更新频点
                seek_freq -= space;
            }
        }

        //刷新频点
        g_search_current_freq = seek_freq;
        com_view_update(RADIO_MAIN_VIEW_SOFT_SEARCH);

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
                keytone_play(KEYTONE_WARNING, KEYTONE_BLOCK);

                last_timestamp = cur_timestamp;
            }
        }
        //挂起10ms 多任务交互
        sys_os_time_dly(1);
    }
    seek_times++;
    if((seek_times < 2)&&(ret <= RESULT_COMMON_RESERVE))
    {
       goto Seek_again;
    }
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

//软件半自动搜台处理
//  一次设置一个频点进行tune，并判断是否真台
//  找到一个真台就停止，边界回绕，搜索一轮。
//dir: 搜台方向
app_result_e deal_soft_manual_search(FM_SeekDir_e dir)
{
    app_result_e ret = RESULT_NULL;
    uint8 space;
    uint8 num;
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
    space = init_start_freq(dir);

    last_timestamp = sys_get_ab_timer();

    while (1)
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

        //界面刷新,每个循环都刷
        g_search_current_freq = g_radio_config.FMStatus.freq;
        com_view_update(RADIO_MAIN_VIEW_SOFT_SEARCH);

        //判断当前频率是否有效台

        if (radio_set_softseek(g_radio_config.FMStatus.freq, dir) == TRUE)
        {
            if (!should_be_passed(g_radio_config.FMStatus.freq))
            {
                //是有效电台，完成退出
                break;
            }
        }

        //退出条件
        if (dir == DIR_UP)
        {
            //边界回绕
            if (g_radio_config.FMStatus.freq >= max_freq[g_radio_config.band_mode])
            {
                g_radio_config.FMStatus.freq = min_freq[g_radio_config.band_mode];
            }
            //搜完一轮，未找到有效电台，退出
            if (g_radio_config.FMStatus.freq == startfreq)
            {
                break;
            }
            else
            {
                //更新频点
                g_radio_config.FMStatus.freq += space;
            }
        }
        else //DIR_DOWN
        {
            //边界回绕
            if (g_radio_config.FMStatus.freq <= min_freq[g_radio_config.band_mode])
            {
                g_radio_config.FMStatus.freq = max_freq[g_radio_config.band_mode];
            }
            //搜完一轮，未找到有效电台，退出
            if (g_radio_config.FMStatus.freq == startfreq)
            {
                break;
            }
            else
            {
                //更新频点
                g_radio_config.FMStatus.freq -= space;
            }
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

//软件搜台(即设置到一个频率，看它有没有信号
//freq: 频率
//dir: 废弃
bool radio_set_softseek(uint32 freq, uint8 dir)
{
    bool bret = TRUE;
    //seek paramter
    seek_param_t search_parm;
    //frequncy
    search_parm.freq = freq;
    //seek dir
    search_parm.dir = dir;

    bret = radio_send_msg(&search_parm, NULL, MSG_FMENGINE_SET_SOFTSEEK);

    //for RCP, 同步电台频点
    //radio_get_freq();
    g_engine_status.FM_CurrentFreq = freq;
    return bret;
}

