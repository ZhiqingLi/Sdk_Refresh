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

/* COMMON set/get/control类命令回调函数注册表 */
const rcp_cmd_cb_t __section__(".rodata.usound_rcp") usound_rcp_cmd_cb_tbl[] =
{
    {RCP_CMD_QUERY,   QUERY_ID_GLOBAL_RCP_INFO, (void *)usound_get_global_rcp_info},
    {RCP_CMD_CONTROL, CONTROL_KEY_PLAY,         (void *)usound_apk_key_play},
    {RCP_CMD_CONTROL, CONTROL_KEY_PAUSE,        (void *)usound_apk_key_play},
    {RCP_CMD_CONTROL, CONTROL_KEY_NEXT,         (void *)usound_apk_key_next},
    {RCP_CMD_CONTROL, CONTROL_KEY_PREV,         (void *)usound_apk_key_prev},
    /* 结束项 */
    {0, 0, NULL},
};

void usound_rcp_var_init(void)
{
    com_rcp_set_callback(usound_rcp_cmd_cb_tbl, usound_get_global_rcp_info);
}

void usound_rcp_var_exit(void)
{
    com_rcp_set_callback(NULL, NULL);
}

app_result_e usound_get_global_rcp_info(uint32 data1, uint32 data2)
{
    uint8 tmp_usound_global_rcp_info[sizeof(usound_global_rcp_info_t) + RCP_COMMAND_HEAD_SIZE];
    usound_global_rcp_info_t *p_usound_global_rcp_info = (usound_global_rcp_info_t*) (tmp_usound_global_rcp_info
            + RCP_COMMAND_HEAD_SIZE);

    libc_memset(p_usound_global_rcp_info, 0, sizeof(usound_global_rcp_info_t));

    //获取公共信息
    com_get_com_info(&(p_usound_global_rcp_info->com_info));
    #if 0
    if ((g_play_key_flag != g_play_status.play_status) && (g_play_key_timer != -1) && ((g_play_key_timer
            - g_last_time_count) >= 4))
    {
        //获取APP信息
        p_usound_global_rcp_info->app_info.usound_status = (uint8) g_play_key_flag;
    }
    else
    {
        //获取APP信息
        p_usound_global_rcp_info->app_info.usound_status = (uint8) g_play_status.play_status;
    }
    #endif
    //获取APP信息
    p_usound_global_rcp_info->app_info.usound_status = (uint8) g_play_status.play_status;
    //获取APP信息
    //p_usound_global_rcp_info->app_info.usound_status = (uint8) g_play_status.play_status;

    com_rcp_send_pkg(0, 0, tmp_usound_global_rcp_info, sizeof(usound_global_rcp_info_t),
            sizeof(usound_global_rcp_info_t));

    return RESULT_NULL;
}

app_result_e usound_apk_key_play(uint32 data1, uint32 data2)
{
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

    g_last_time_count = (uint32)g_play_key_timer;

    usound_play_pause();

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
    return RESULT_NULL;
}

app_result_e usound_apk_key_pause(uint32 data1, uint32 data2)
{
    if (g_usound_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    usound_play_pause();
    return RESULT_NULL;
}

app_result_e usound_apk_key_next(uint32 data1, uint32 data2)
{
    if (g_usound_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    usound_play_next();

    g_play_key_timer = -1;

    return RESULT_NULL;
}

app_result_e usound_apk_key_prev(uint32 data1, uint32 data2)
{
    if (g_usound_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    usound_play_prev();

    g_play_key_timer = -1;

    return RESULT_NULL;
}
