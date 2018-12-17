#include "app_radio.h"

//全自动搜台场景参数初始化
bool radio_auto_search_init(FM_SeekDir_e dir)
{
    uint8* freq_tab;

    if (g_radio_config.band_mode == Band_China_US)
    {
        //普通波段
        //libc_memset(g_radio_config.fmstation_us, 0, MAX_STATION_COUNT * 4);
        freq_tab = (uint8*) g_radio_config.fmstation_us;
        g_radio_config.FMStatus.freq = MIN_US;
    }
    else if (g_radio_config.band_mode == Band_Japan)
    {
        //日本波段
        //libc_memset(g_radio_config.fmstation_jp, 0, MAX_STATION_COUNT * 4);
        freq_tab = (uint8*) g_radio_config.fmstation_jp;
        g_radio_config.FMStatus.freq = MIN_JP;
    }
    else
    {
        //欧洲波段
        //libc_memset(g_radio_config.fmstation_eu, 0, MAX_STATION_COUNT * 4);
        freq_tab = (uint8*) g_radio_config.fmstation_eu;
        g_radio_config.FMStatus.freq = MIN_EU;
    }
    //清除电台个数、当前电台号
    g_radio_config.FMStatus.station = 0;
    g_radio_config.FMStatus.station_count = 0;

    libc_memset(freq_tab, 0, MAX_STATION_COUNT * 4);
    //清空缓存电台列表
    libc_memset(&Auto_tab[0], 0, MAX_STATION_COUNT * 4);

    return TRUE;
}

//全自动搜台场景退出
bool radio_auto_search_exit(void)
{
    uint32 play_freq;

    uint8* ret_freq;

    uint8* ret_status;

    if (g_radio_config.FMStatus.station_count != 0)
    {
        //有搜索到的电台，选择第一个播放
        g_radio_config.FMStatus.station = 1;
        g_radio_config.FMStatus.freq = Auto_tab[0];
    }
    else
    {
        //一个电台也未搜索到，选择默认频点播放
        g_radio_config.FMStatus.station = 0;
        if (g_radio_config.band_mode == Band_Japan)
        {
            play_freq = MIN_JP;
        }
        else
        {
            play_freq = MIN_US;
        }
        g_radio_config.FMStatus.freq = play_freq;
    }

    if (g_radio_config.band_mode == Band_China_US)
    {
        //普通频段
        //libc_memcpy(&g_radio_config.fmstation_us[0], &Auto_tab[0], MAX_STATION_COUNT * 4);
        //libc_memcpy(&g_radio_config.FMStatus_us, &g_radio_config.FMStatus, sizeof(fm_play_status_t));
        ret_freq = (uint8*) &g_radio_config.fmstation_us[0];
        ret_status = (uint8*) &g_radio_config.FMStatus_us;

    }
    else if (g_radio_config.band_mode == Band_Japan)
    {
        //日本频段
        //libc_memcpy(&g_radio_config.fmstation_jp[0], &Auto_tab[0], MAX_STATION_COUNT * 4);
        //libc_memcpy(&g_radio_config.FMStatus_jp, &g_radio_config.FMStatus, sizeof(fm_play_status_t));
        ret_freq = (uint8*) &g_radio_config.fmstation_jp[0];
        ret_status = (uint8*) &g_radio_config.FMStatus_jp;
    }
    else
    {
        //欧洲频段
        //libc_memcpy(&g_radio_config.fmstation_eu[0], &Auto_tab[0], MAX_STATION_COUNT * 4);
        //libc_memcpy(&g_radio_config.FMStatus_eu, &g_radio_config.FMStatus, sizeof(fm_play_status_t));
        ret_freq = (uint8*) &g_radio_config.fmstation_eu[0];
        ret_status = (uint8*) &g_radio_config.FMStatus_eu;
    }
    libc_memcpy(ret_freq, &Auto_tab[0], MAX_STATION_COUNT * 4);
    libc_memcpy(ret_status, &g_radio_config.FMStatus, sizeof(fm_play_status_t));
    //设置频点播放
    radio_set_freq(g_radio_config.FMStatus.freq);

    return TRUE;
}

//全自动搜台场景
//dir: 搜台方向( 向上或向下)
app_result_e radio_scene_auto_search(FM_SeekDir_e dir)
{
    app_result_e result;
    bool bret;
    FM_SeekMode_e mode;

    mode = g_radio_config.seek_mode;
    g_searching_flag = TRUE;
    /* 初始化*/
    bret = radio_auto_search_init(dir);
    if (!bret)
    {
        g_searching_flag = FALSE;
        //初始化失败返回
        return RESULT_ERROR;
    }


    //开始全自动搜台前，先mute
    radio_set_mute(SetMUTE);

    //创建搜台主视图
    g_search_current_freq = get_start_freq(dir);
    if (mode == HARDSEEK)
    {
        radio_hard_search_create_main_view();
    }
    else
    {
        radio_soft_search_create_main_view();
    }

    //语音播报
    com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_SEARCHING);

    //获取门限值
    g_search_throd = (uint8) com_get_config_default(FMENGINE_AP_ID_SEEKTH);
    //设置搜台门限
    radio_set_throd(g_search_throd);
    PRINT_INFO_INT("start FM seek.throd&mode=", (g_search_throd << 16) + mode);
    if (mode == HARDSEEK)
    {
        
        //处理硬件全自动搜台
        result = deal_hard_auto_seek(dir);
    }
    else
    {
        //处理软件全自动搜台
        result = deal_soft_auto_seek(dir);
    }
    PRINT_INFO_INT("FM seek end.total-ch=",g_radio_config.FMStatus.station_count);
    radio_auto_search_exit();
    g_searching_flag = FALSE;
    //等到播报频率的时候才解除静音
    //radio_set_mute(releaseMUTE);

    return result;
}

