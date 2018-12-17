#include "app_radio.h"

//半自动搜台场景初始化
bool scene_manual_search_init(void)
{
    bool ret = TRUE;

    //取得当前播放频率
    radio_get_freq();

    //保存搜索起始频率
    startfreq = g_engine_status.FM_CurrentFreq;

    g_radio_config.FMStatus.freq = startfreq;

    //开始半自动搜台前，先mute
    radio_set_mute(SetMUTE);
    return ret;
}

//半自动搜台场景退出
bool scene_manual_search_exit(void)
{
    uint8 num;
    bool result;

    //获取当前频率值
    result = radio_get_freq();
    if (result == TRUE)
    {
        //正常获取，播放当前频点
        g_radio_config.FMStatus.freq = g_engine_status.FM_CurrentFreq;

        if (g_radio_config.FMStatus.freq > max_freq[g_radio_config.band_mode])
        {
            //大于最大频点
            g_radio_config.FMStatus.freq = startfreq;
        }

        if (g_radio_config.FMStatus.freq < min_freq[g_radio_config.band_mode])
        {
            //小于最小频点(87-87.5之间的值)
            g_radio_config.FMStatus.freq = min_freq[g_radio_config.band_mode];
        }
    }
    else
    {
        //获取失败，播放半自动搜台前频点
        g_radio_config.FMStatus.freq = startfreq;
    }

    //判断当前频点是否是已保存频点
    num = whether_in_list(g_radio_config.FMStatus.freq, pfreq);

    //切换后的频点是未保存频点
    if (num == 0xff)
    {
        g_radio_config.FMStatus.station = 0;
    }
    else
    {
        //切换后的频点是已保存频点
        g_radio_config.FMStatus.station = num;
    }

    //设置频点播放
    radio_set_freq(g_radio_config.FMStatus.freq);

    //等到播报频率的时候才解除静音
    //radio_set_mute(releaseMUTE);

    return TRUE;
}

//半自动搜台场景
//dir: 半自动搜台方向
app_result_e radio_scene_manual_search(FM_SeekDir_e dir)
{
    bool result;
    //返回值
    app_result_e ret = RESULT_NULL;
    //for RCP, 进入搜索状态
    g_searching_flag = TRUE;
    result = scene_manual_search_init();
    if (result == RESULT_NULL)
    {
        g_searching_flag = FALSE;
        //返回正常播放场景
        return ret;
    }

    //创建搜台主视图
    g_search_current_freq = g_radio_config.FMStatus.freq;
    if (g_radio_config.seek_mode == HARDSEEK)
    {
        radio_hard_search_create_main_view();
    }
    else
    {
        radio_soft_search_create_main_view();
    }

    com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_SEARCHING);
    //获取门限值
    g_search_throd = (uint8) com_get_config_default(FMENGINE_AP_ID_SEEKTH);
    //设置搜台门限
    radio_set_throd(g_search_throd);

    if (g_radio_config.seek_mode == HARDSEEK)
    {
        //处理硬件半自动搜台
        ret = deal_hard_manual_search(dir);
    }
    else
    {
        //处理软件半自动搜台
        ret = deal_soft_manual_search(dir);
    }

    //for RCP, 退出搜索状态
    g_searching_flag = FALSE;

    scene_manual_search_exit();

    return ret;
}
