#include "app_radio.h"

//FM初始化
//band_mode: 波段
bool radio_modul_open(uint8 band_mode)
{
    return radio_send_msg(&band_mode, NULL, MSG_FM_MODE_OPEN);
}

//打开主控的通路
bool radio_connect_ain(void)
{
    return radio_send_msg(NULL, NULL, MSG_FM_AIN_OPEN);
}


int8 s_mute_count = 0; //这里不考虑溢出的情况，因为调用方会保证
//freq单位kHz
bool radio_set_freq(uint32 freq)
{
    bool bret = TRUE;
    
    //解除静音(上一次的定时器没有到，需要取消定时器，且releasemute 计数)
    //radio_set_mute(releaseMUTE);
    if (g_mute_set_timer != -1)
    {
        kill_app_timer(g_mute_set_timer);
        g_mute_set_timer = -1;
        //这里做了一定的妥协:如果不调mute直接调release,也使之能出声音
        if (s_mute_count > 0) 
        {
            s_mute_count--;
        }
    }

    //静音, 避免频率切换过程中的杂音
    radio_set_mute(SetMUTE);
    //send message
    bret = radio_send_msg(&freq, NULL, MSG_FMENGINE_SETFREQ_SYNC);

    if (g_mute_set_timer == -1)
    {
        g_mute_set_timer = set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL | (APP_TIMER_TAG_SYS << 8),
                150, release_mute_timer);

        if (g_mute_set_timer == -1)
        {
            sys_os_time_dly(15);
            //解除静音
            radio_set_mute(releaseMUTE);
        }
    }

    //当FM处于暂停时，切换频点后要play下，否则不能播放
    if (g_playing_flag == FALSE)
    {
        radio_set_mute(releaseMUTE);
        radio_send_msg(NULL, NULL, MSG_FMENGINE_PLAY);
        g_playing_flag = TRUE;
    }

    //for RCP, 同步电台频点
    radio_get_freq();

    return bret;
}

//静音或解除静音
//  (这里用的是计数器)
bool radio_set_mute(FM_MUTE_e mode)
{
    //调用mute的次数
    //static int8 s_mute_count = 0; //这里不考虑溢出的情况，因为调用方会保证

    if (mode == SetMUTE)
    {
        if (s_mute_count == 0)
        {
            s_mute_count++;
        }
        else
        {
            s_mute_count++;
            return TRUE;
        }
    }
    else
    {
        if (s_mute_count > 0) //这里做了一定的妥协:如果不调mute直接调release,也使之能出声音
        {
            s_mute_count--;
        }
        if (s_mute_count == 0)
        {
        }
        else
        {
            return TRUE;
        }
    }

    return radio_send_msg(&mode, NULL, MSG_FMENGINE_SETMUTE);
}

//设置电台波段
bool radio_set_band(radio_band_e mode)
{
    return radio_send_msg(&mode, NULL, MSG_FMENGINE_SETBAND);
}

//设置搜台门限
//val: 级别(对应到驱动中的门限数组)
bool radio_set_throd(uint8 val)
{
    return radio_send_msg(&val, NULL, MSG_FMENGINE_SETTHRED);
}

/*! \endcond */

