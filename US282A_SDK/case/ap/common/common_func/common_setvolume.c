/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：CASE 设置音量接口，可以通过UI和TTS、KEYTONE进行反馈。
 * 作者：cailizhen
 ********************************************************************************/

#include "psp_includes.h"
#include "music_common.h"
#include "common_func.h"

void com_set_volume_view(view_update_e mode);
void com_create_volume_view(void)
{
    create_view_param_t create_view_param;

#if (SUPPORT_LED_DRIVER == 1)
    if (sys_comval->support_led_display == 1)
    {
        create_view_param.type = VIEW_TYPE_MSG;
        create_view_param.unknown_key_deal = UNKNOWN_KEY_IGNORE;
        create_view_param.overtime = 3000;
        create_view_param.ke_maplist = NULL;
        create_view_param.se_maplist = NULL;
        create_view_param.view_cb = com_set_volume_view;
        com_view_add(COM_VIEW_ID_SET_VOLUME, &create_view_param);
    }
#endif
}

static void __com_set_volume_redraw(void)
{
    led_display(LCD_COL, 0xff, 0); //清除:号
    led_display(0, NUM_U, 1);
    led_display(1, NUM_0, 0);
    led_display(2, (sys_comval->volume_current / 10) + NUM_0, 1);
    led_display(3, (sys_comval->volume_current % 10) + NUM_0, 1);
}

void com_set_volume_view(view_update_e mode)
{
    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        {
            __com_set_volume_redraw();
        }
        break;

        case VIEW_UPDATE_DESTROY:
        {
        }
        break;

        case VIEW_UPDATE_REDRAW:
        {
            __com_set_volume_redraw();
        }
        break;

        case VIEW_UPDATE_STATUS:
        {
            __com_set_volume_redraw();
        }
        break;

        default:
        break;
    }
}

void com_volume_add(bool tts_flag)
{
    uint8 cur_volume;

    if (sys_comval->volume_current < VOLUME_VALUE_MAX)
    {   
        cur_volume = sys_comval->volume_current + 1;  
        com_set_sound_volume(cur_volume, SET_VOLUME_SYNC_TO_REMOTE);
    }
    
    if (tts_flag == FALSE)
    {
        com_create_volume_view();
    }

    if (sys_comval->volume_current >= VOLUME_VALUE_MAX)
    {
        //com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_USEFIFO, (void *) TTS_MAX_VOLUME);
#if 1
        keytone_play(KEYTONE_WARNING, KEYTONE_NOBLOCK);
#endif
#if 0
        if (g_p_view_ke->type == KEY_TYPE_DOWN)
        {
            com_start_key_tone(KEYTONE_BLOCK);
        }
        else
        {
            com_start_key_tone(KEYTONE_BLOCK);
            com_start_key_tone(KEYTONE_BLOCK);
        }

        sys_os_time_dly(10);
        com_filter_keymsg_in_queue(g_p_view_ke->val, KEY_TYPE_SHORT_UP);
        com_filter_keymsg_in_queue(g_p_view_ke->val, KEY_TYPE_HOLD);
#endif
    }
}

void com_volume_sub(bool tts_flag)
{
    uint8 cur_volume;

    if (sys_comval->volume_current > 0)
    {
        cur_volume = sys_comval->volume_current - 1; 
        com_set_sound_volume(cur_volume, SET_VOLUME_SYNC_TO_REMOTE);   
    }

    if (tts_flag == FALSE)
    {
        com_create_volume_view();
    }

    if (sys_comval->volume_current == 0)
    {
        //com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_USEFIFO, (void *) TTS_MIN_VOLUME);
    }
}

void com_volume_set(uint8 set_vol, uint8 set_mode)
{
    if ((set_vol != sys_comval->volume_current) && (set_vol <= VOLUME_VALUE_MAX))
    {
        com_set_sound_volume(set_vol, 0);

        if ((set_mode & SET_VOLUME_VIEW) != 0)
        {
            com_create_volume_view();
        }

        if ((set_mode & SET_VOLUME_TTS) != 0)
        {
            if (set_vol >= VOLUME_VALUE_MAX)
            {
                //com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_USEFIFO, (void *) TTS_MAX_VOLUME);
#if 1
                keytone_play(KEYTONE_WARNING, KEYTONE_NOBLOCK);
#endif
#if 0
                com_start_key_tone(KEYTONE_BLOCK);
                com_start_key_tone(KEYTONE_BLOCK);
#endif
            }

            if (set_vol == 0)
            {
                //com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_USEFIFO, (void *) TTS_MIN_VOLUME);
            }
        }
    }
}

