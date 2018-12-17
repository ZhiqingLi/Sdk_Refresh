#include "music.h"

/* COMMON set/get/control类命令回调函数注册表 */
const rcp_cmd_cb_t music_rcp_cmd_cb_tbl[] =
{
    { RCP_CMD_SETTING, SETTING_ID_MUSIC_SETTING, (void *) music_set_music_setting },//设置循环模式
    { RCP_CMD_SETTING, SETTING_ID_SONG_SEQ, (void *) music_set_song_seq },//数字点歌
    { RCP_CMD_SETTING, SETTING_ID_MUSIC_PLIST, (void *) music_set_music_plist },//设置文件列表
    { RCP_CMD_SETTING, SETTING_ID_MUSIC_DISK, (void *) music_set_music_disk },

    { RCP_CMD_QUERY, QUERY_ID_GLOBAL_RCP_INFO, (void *) music_get_global_rcp_info },//music全局信息
    { RCP_CMD_QUERY, QUERY_ID_MUSIC_INFO, (void *) music_get_music_info },//获取歌曲id3等信息
    { RCP_CMD_QUERY, QUERY_ID_MUSIC_LYRIC, (void *) music_get_music_lyric },//获取歌词信息
    { RCP_CMD_QUERY, QUERY_ID_MUSIC_PLIST, (void *) music_get_music_plist },//获取文件列表

    { RCP_CMD_CONTROL, CONTROL_KEY_PLAY, (void *) music_apk_key_play },//播放
    { RCP_CMD_CONTROL, CONTROL_KEY_PAUSE, (void *) music_apk_key_pause },//暂停
    { RCP_CMD_CONTROL, CONTROL_KEY_PREV, (void *) music_apk_key_prev },//上一曲
    { RCP_CMD_CONTROL, CONTROL_KEY_NEXT, (void *) music_apk_key_next },//下一曲

    { RCP_CMD_QUERY, QUERY_ID_MUSIC_STATUS, (void *) music_get_dir_list_status },//获取目录状态
    { RCP_CMD_QUERY, QUERY_ID_MUSIC_DLIST, (void *) music_get_music_dir_list },//获取目录列表

    /* 结束项 */
    { 0, 0, NULL },
};

void music_rcp_var_init(void)
{
    init_id3_info();
    com_rcp_set_callback(music_rcp_cmd_cb_tbl, music_get_global_rcp_info);
}

void music_rcp_var_exit(void)
{
    com_rcp_set_callback(NULL, NULL);
}

app_result_e music_set_music_setting(uint32 data1, uint32 data2,
        void *data, uint16 data_len)
{
    uint8 setting_id = (uint8) data1;

    switch (setting_id)
    {
        case SETTING_ID_MS_LOOP:/*设置循环模式*/
        {
            g_music_config.repeat_mode = (fsel_mode_e) data2;
            music_set_loop_mode(g_music_config.repeat_mode);
        }
        break;

        default:
        break;
    }

    return RESULT_NULL;
}

app_result_e music_set_song_seq(uint32 data1, uint32 data2, void *data,
        uint16 data_len)
{
    uint16 digital_num = (uint16) data1;
    play_mode_e play_mode;
    
    if((g_music_info.eg_status.play_status != StopSta)
        && (g_music_info.eg_status.play_status != PauseSta))
    {
        play_mode = PLAY_NORMAL;
    }
    else
    {
        play_mode = PLAY_NO_PLAY;
    }

    //数字点歌
    music_get_filepath(&g_file_path_info);
    if (digital_num != g_file_path_info.file_path.dirlocation.file_num)
    {
        if (music_digital_song(digital_num) == TRUE)
        {
            music_switch_song(TTS_INVALID, play_mode);
            music_file_switch_save();
        }
    }

    return RESULT_NULL;
}

app_result_e music_set_music_plist(uint32 data1, uint32 data2,
        void *data, uint16 data_len)
{
    PRINT_DBG("set_plist");
#if 0
    MUS_STATIC uint8 music_rcp_plist_buffer[512] _BANK_DATA_ATTR_;
    uint16 *p_file_index = (uint16 *) data;
    uint16 file_start, file_count;
    uint16 page_cur, page_offset, tmp_count;
    uint16 file_one_page = (512 / sizeof(uint16));
    uint16 i, j;

    if (((g_rcp_cmd_flags[0] & RCP_SEGMENT_MASK) == RCP_SEGMENT_FIRST) || ((g_rcp_cmd_flags[0] & RCP_SEGMENT_MASK)
                    == RCP_SEGMENT_NONE))
    {
        g_plist_file_count = 0;
    }

    file_start = g_plist_file_count;
    file_count = data_len / sizeof(uint16);
    //get real file number
    music_get_disknumber(&j);

    //check cur file sorted index
    for (i = 0; i < file_count; i++)
    {
        if (p_file_index[i] == j)
        {
            break;
        }
    }

    //get cur file dest index
    if (i < file_count)
    {
        i++;
        //send message to music engine
        music_set_sortnumber((file_start + i));

        //set file switch
        g_rcp_file_status.file_switch = 1;

        g_rcp_file_status.file_seq = (file_start + i);

        music_get_filepath(&g_file_path_info);

    }

    //update file count
    g_plist_file_count += file_count;

    //读取file index array VRAM，更新file index array
    page_cur = file_start / file_one_page;
    page_offset = file_start % file_one_page;

    //更新head
    if (page_offset != 0)
    {
        sys_vm_read(music_rcp_plist_buffer, VM_PLIST_INDEX_START0 + 512 * page_cur, 512);
        if ((page_offset + file_count) > file_one_page)
        {
            tmp_count = file_one_page - page_offset;
        }
        else
        {
            tmp_count = file_count;
        }

        libc_memcpy(music_rcp_plist_buffer + sizeof(uint16) * page_offset, p_file_index, sizeof(uint16) * tmp_count);
        sys_vm_write(music_rcp_plist_buffer, VM_PLIST_INDEX_START0 + 512 * page_cur, 512);

        file_start += tmp_count;
        p_file_index += tmp_count;
        file_count -= tmp_count;
    }

    //更新中间满page
    while (file_count >= file_one_page)
    {
        page_cur = file_start / file_one_page;
        sys_vm_write(p_file_index, VM_PLIST_INDEX_START0 + 512 * page_cur, 512);

        file_start += file_one_page;
        p_file_index += file_one_page;
        file_count -= file_one_page;
    }

    //更新tail
    if (file_count != 0)
    {
        page_cur = file_start / file_one_page;
        sys_vm_read(music_rcp_plist_buffer, VM_PLIST_INDEX_START0 + 512 * page_cur, 512);
        libc_memcpy(music_rcp_plist_buffer, p_file_index, sizeof(uint16) * file_count);
        sys_vm_write(music_rcp_plist_buffer, VM_PLIST_INDEX_START0 + 512 * page_cur, 512);

        p_file_index += file_count;
    }
#endif
    return RESULT_NULL;

}

app_result_e music_set_music_disk(uint32 data1, uint32 data2,
        void *data, uint16 data_len)
{
    app_result_e result;

    if ((data1 & 0xff) == 1) //1 for card/2 for udisk
    {
        result = RESULT_MUSIC_CPLAY;
    }
    else
    {
        result = RESULT_MUSIC_UPLAY;
    }

    return result;
}

