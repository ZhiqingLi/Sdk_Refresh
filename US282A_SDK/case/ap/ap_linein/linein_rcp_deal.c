/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：LineIn的rcp的处理函数
 * 作者：Gary Wang
 ********************************************************************************/
#include "app_linein.h"

/* COMMON set/get/control类命令回调函数注册表 */
static const rcp_cmd_cb_t linein_rcp_cmd_cb_tbl[] =
{
    { RCP_CMD_SETTING, SETTING_ID_AUX_SETTING, (void *) linein_set_aux_setting },
    { RCP_CMD_QUERY, QUERY_ID_GLOBAL_RCP_INFO, (void *) linein_get_global_rcp_info },
    { RCP_CMD_CONTROL, CONTROL_KEY_PLAY, (void *) linein_apk_key_play },
    /* 结束项 */
    { RCP_CMD_NONE, 0, NULL },
};

void linein_rcp_var_init(void)
{
    com_rcp_set_callback(linein_rcp_cmd_cb_tbl, linein_get_global_rcp_info);
}

void linein_rcp_var_exit(void)
{
    com_rcp_set_callback(NULL, NULL);
}

app_result_e linein_get_global_rcp_info(uint32 data1, uint32 data2)
{
    uint8 tmp_linein_global_rcp_info[sizeof(linein_global_rcp_info_t) + RCP_COMMAND_HEAD_SIZE];
    linein_global_rcp_info_t *p_linein_global_rcp_info = (linein_global_rcp_info_t*) (tmp_linein_global_rcp_info
            + RCP_COMMAND_HEAD_SIZE);

    libc_memset(p_linein_global_rcp_info, 0, sizeof(linein_global_rcp_info_t));

    //获取公共信息
    com_get_com_info(&(p_linein_global_rcp_info->com_info));

    //获取APP信息
    p_linein_global_rcp_info->app_info.play_status = g_linein_eg_status.play_status;

    com_rcp_send_pkg(0, 0, tmp_linein_global_rcp_info, sizeof(linein_global_rcp_info_t), \
            sizeof(linein_global_rcp_info_t));

    return RESULT_NULL;
}

app_result_e linein_set_aux_setting(uint32 data1, uint32 data2)
{
    return RESULT_NULL;
}

app_result_e linein_apk_key_play(uint32 data1, uint32 data2)
{
    app_result_e result = RESULT_NULL;

    if (g_linein_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    g_neednot_tts_play = TRUE;
    result = key_play_deal();
    g_neednot_tts_play = FALSE;

    return result;
}

