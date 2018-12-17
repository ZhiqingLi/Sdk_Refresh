/*******************************************************************************
 *                              US212A
 *                            Module: music ui
 *                 Copyright(c) 2003-2012 Actions Semiconductor
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *     fiona.yang   2011-09-14          1.0              create this file
 *******************************************************************************/

#include "ap_usound.h"

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e _key_play_deal(void)
 * \²¥·Å/ÔÝÍ£
 * \param[in]    status  para1
 * \param[in]
 * \param[out]   none
 * \return       void the result
 * \retval       none
 * \retval       none
 * \ingroup      usound_keymsg.c
 * \note
 */
/*******************************************************************************/
app_result_e _key_play_deal(void)
{
    uint32 gap;

    if (g_usound_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    if (g_play_key_timer < 4)
    {
        g_play_key_timer = 0;

        g_last_time_count = 0;

        g_play_key_flag = g_play_status.play_status;
    }

    //tts
    if (g_play_status.play_status == 1)
    {
        uint32 need_stop_manual = 0;

        if (g_comval.tts_enable == FALSE)
        {
            //²»Ö§³Ö×´Ì¬ÓïÒô²¥±¨¹¦ÄÜ
            need_stop_manual = 1;
        }

        if ((g_tts_play_info.option & IGNORE_PLAY_TTS) != 0)
        {
            //³¡¾°²»±ã½øÐÐTTS£¬ÆÁ±ÎTTS£»
            need_stop_manual = 1;
        }

        if (get_mute_enable() == TRUE)
        {

            //¾²Òô×´Ì¬ÏÂ£¬Ö±½Ó·µ»Ø
            need_stop_manual = 1;
        }

        if ((com_get_sound_volume() == 0) && (g_comval.kt_tts_min_volume == 0))
        {
            //ÔÊÐíÒôÁ¿Îª0Ê±Ã»ÓÐ°´¼üÒôºÍTTS
            need_stop_manual = 1;
        }
        com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_PAUSE_SONG);
        if (need_stop_manual == 1)
        {
            usound_play_pause();
        }
        #if 0
        gap = (g_play_key_timer - g_last_time_count);

        if ((g_play_key_flag == 0) && (gap >= 4))
        {
            com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_PLAY_SONG);
        }
        else
        {
            com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_PAUSE_SONG);
        }
        #endif
        
        g_last_time_count = (uint32)g_play_key_timer;

    }
    else
    {
        com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_PLAY_SONG);
        usound_play_pause();
    }
    //for count
    if (g_play_key_flag == 1)
    {
        //g_play_key_flag = 0;
        g_play_key_flag = !g_play_status.play_status;
    }
    else
    {
        g_play_key_flag = 1;
    }
    libc_print("cur_key_flag", g_play_key_flag, 2);

    //switch app
    return RESULT_NULL;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e _key_next_deal(void)
 * \ÇÐ»»ÏÂÒ»Çú
 * \param[in]    status  para1
 * \param[in]
 * \param[out]   none
 * \return       void the result
 * \retval       none
 * \retval       none
 * \ingroup      usound_keymsg.c
 * \note
 */
/*******************************************************************************/
app_result_e _key_next_deal(void)
{
    if (g_usound_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    //ÇÐ»»¸èÇú
    //usound_tts_start();
    //tts
    com_tts_state_play(TTS_MODE_ONLYONE, (void*) TTS_NEXT_SONG);

    //ÇÐ»»¸èÇú
    //usound_tts_stop();

    usound_play_next();

    g_play_key_timer = -1;
    //switch app
    return RESULT_NULL;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e _key_prev_deal(void)
 * \ÇÐ»»ÉÏÒ»Çú
 * \param[in]    status  para1
 * \param[in]
 * \param[out]   none
 * \return       void the result
 * \retval       none
 * \retval       none
 * \ingroup      usound_keymsg.c
 * \note
 */
/*******************************************************************************/
app_result_e _key_prev_deal(void)
{
    if (g_usound_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    //ÇÐ»»¸èÇú
    //usound_tts_start();
    //tts
    com_tts_state_play(TTS_MODE_ONLYONE, (void*) TTS_LAST_SONG);

    //ÇÐ»»¸èÇú
    //usound_tts_stop();

    usound_play_prev();

    g_play_key_timer = -1;

    //switch app
    return RESULT_NULL;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e _key_vol_add_deal(void)
 * \ÒôÁ¿¼õ
 * \param[in]    status  para1
 * \param[in]
 * \param[out]   none
 * \return       void the result
 * \retval       none
 * \retval       none
 * \ingroup      usound_keymsg.c
 * \note
 */
/*******************************************************************************/
app_result_e _key_vol_add_deal(void)
{

    g_manual_set = 1;
    
    usound_volume_add();

    //for flag
    return RESULT_KEY_EVENT_REDEAL_BY_COMMON;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e _key_vol_sub_deal(void)
 * \ÒôÁ¿¼õ
 * \param[in]    status  para1
 * \param[in]
 * \param[out]   none
 * \return       void the result
 * \retval       none
 * \retval       none
 * \ingroup      usound_keymsg.c
 * \note
 */
/*******************************************************************************/
app_result_e _key_vol_sub_deal(void)
{

    g_manual_set = 1;
    
    usound_volume_sub();

    //for flag
    return RESULT_KEY_EVENT_REDEAL_BY_COMMON;
}
