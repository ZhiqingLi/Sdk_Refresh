#include "music.h"

static int8 s_timer_id_rcp_getplist = -1;

//这里的定义要跟xn里面的一样
#define MUSIC_RCP_DEAL_GET_XDATA_LEN    (512 + 16)

#define PAY_LOAD_LEN    (MUSIC_RCP_DEAL_GET_XDATA_LEN - RCP_COMMAND_HEAD_SIZE)
MUS_STATIC uint8 music_rcp_buffer[PAY_LOAD_LEN + RCP_COMMAND_HEAD_SIZE] _BANK_DATA_ATTR_;

MUS_STATIC const uint8 unknown_genre[] = "unknown genre";
#define MAX_MUSIC_EXT_NAME  7 //支持7种音乐格式，与 case_type.h MUSIC_BITMAP 对应
//跟music_type_e对应
MUS_STATIC const uint8 music_ext_name[][4] =
{ "MP3", "WMA", "WAV", "FLAC", "APE", "MP1", "MP2" };

MUS_STATIC uint8 _music_info_type(uint8 *info)
{
    uint8 code_type = 0; //ANSI

    if ((info[0] == 0xff) && (info[1] == 0xfe))
    {
        //UTF16 LE
        code_type = 1;
    }
    else if ((info[0] == 0xfe) && (info[1] == 0xff))
    {
        //UTF16 BE
        code_type = 2;
    }
    else if ((info[0] == 0xef) && (info[1] == 0xbb) && (info[1] == 0xbf))
    {
        //UTF-8
        code_type = 3;
    }
    else
    {
        ; //do nothing
    }

    return code_type;
}

MUS_STATIC uint8 _music_ext_type(uint8 *ext_name)
{
    uint8 i;

    for (i = 0; i < MAX_MUSIC_EXT_NAME; i++)
    {
        if (libc_memcmp(ext_name, music_ext_name[i], 3) == 0)
        {
            return i;
        }
    }

    return 0;
}

app_result_e music_get_music_info(uint32 data1, uint32 data2)
{
    music_rcp_id3_info_t *p_rcp_id3_info = (music_rcp_id3_info_t*) (music_rcp_buffer + RCP_COMMAND_HEAD_SIZE);
    uint32 cur_file_seq = 0;
    uint16 send_len;

    if(g_scan_handle != NULL)
    {
        //mus_scan在占用内存，id3没有内存执行
        //按照约定，app/apk不会在没有scan完时，进入这里的
        return RESULT_NULL;
    }

    if (eh_vfs_mount == -1)
    {
        return RESULT_NULL;
    }

    if(s_timer_id_rcp_getplist == -1)
    {
        adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL11, FREQ_NULL);
        change_ui_status(UI_STATUS_BUSY);
    }

    //这里会切BANK, 冲掉XDATA, 但没关系
    music_get_filepath(&g_file_path_info); //更新当前文件路径信息，确保文件序号正确
    //获取ID3信息
    init_id3_info();
    libc_memset(music_rcp_buffer, 0, sizeof(music_rcp_buffer));

    cur_file_seq = g_file_path_info.file_path.dirlocation.file_num;
    music_rcp_get_cur_info(&g_cur_id3_info);

    //title
    libc_memcpy(p_rcp_id3_info->tit2_buffer,
            g_cur_id3_info.p_id3_info.tit2_buffer,
            MIN(sizeof(p_rcp_id3_info->tit2_buffer),
            g_cur_id3_info.p_id3_info.tit2_length));

    //artist
    libc_memcpy(p_rcp_id3_info->tpe1_buffer,
            g_cur_id3_info.p_id3_info.tpe1_buffer,
            MIN(sizeof(p_rcp_id3_info->tpe1_buffer),
            g_cur_id3_info.p_id3_info.tpe1_length));

    //album
    libc_memcpy(p_rcp_id3_info->talb_buffer,
            g_cur_id3_info.p_id3_info.talb_buffer,
            MIN(sizeof(p_rcp_id3_info->talb_buffer),
            g_cur_id3_info.p_id3_info.talb_length));

    //file name
    libc_memcpy(p_rcp_id3_info->file_buffer,
            g_cur_id3_info.pfile_buf,
            MIN(sizeof(p_rcp_id3_info->file_buffer),
            g_cur_id3_info.name_len));

    p_rcp_id3_info->music_type = _music_ext_type(g_file_path_info.file_path.dirlocation.file_info.file_extend_info.file_ext);

    p_rcp_id3_info->file_type = _music_info_type(p_rcp_id3_info->file_buffer);

    p_rcp_id3_info->tit2_type = _music_info_type(p_rcp_id3_info->tit2_buffer);

    p_rcp_id3_info->tpe1_type = _music_info_type(p_rcp_id3_info->tpe1_buffer);

    p_rcp_id3_info->talb_type = _music_info_type(p_rcp_id3_info->talb_buffer);

    libc_memcpy(p_rcp_id3_info->genre_buffer, unknown_genre,
            MIN(sizeof(unknown_genre), sizeof(p_rcp_id3_info->genre_buffer)));

    p_rcp_id3_info->genre_type = 0;

    //发送应答命令
    send_len = sizeof(music_rcp_id3_info_t);
    com_rcp_send_pkg(cur_file_seq, 0, music_rcp_buffer, send_len, send_len);

    //本函数执行时间：60毫秒
    if(s_timer_id_rcp_getplist == -1)
    {
        adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL7, FREQ_NULL);
        change_ui_status(UI_STATUS_IDLE);
    }

    return RESULT_NULL;
}

app_result_e music_get_music_lyric(uint32 data1, uint32 data2)
{
    uint8 *p_music_lyric_buf = (uint8 *) (music_rcp_buffer + RCP_COMMAND_HEAD_SIZE);
    HANDLE lrc_file_handle = (HANDLE) NULL;
    uint32 lyric_block;
    uint32 lyric_len;
    uint32 cur_file_seq = 0;
    bool lyric_first = TRUE;

    if(g_scan_handle != NULL)
    {
        //mus_scan在占用内存，id3没有内存执行
        //按照约定，app/apk不会在没有scan完时，进入这里的
        return RESULT_NULL;
    }

    //file sys have already amont
    if (eh_vfs_mount != -1)
    {
        //music_get_filepath(&g_file_path_info);//更新当前文件路径信息，确保文件序号正确
        cur_file_seq = g_file_path_info.file_path.dirlocation.file_num;
        //lrc_file_handle = lyric_open(g_cur_id3_info.pfile_buf, &lyric_len);
        lrc_file_handle = music_lyric_check(&lyric_len);
    }

    if (lrc_file_handle != (HANDLE) NULL)
    {
        while (lyric_len > 0)
        {
            if (lyric_len >= PAY_LOAD_LEN)
            {
                lyric_block = PAY_LOAD_LEN;
            }
            else
            {
                lyric_block = lyric_len;
            }
            //read file use file system
            vfs_file_read(eh_vfs_mount, p_music_lyric_buf, lyric_block, lrc_file_handle);

            if (lyric_first == TRUE)
            {
                //发送应答命令
                com_rcp_send_pkg(cur_file_seq, 0, music_rcp_buffer, (uint16) lyric_block, (uint16) lyric_len);

                lyric_first = FALSE;
            }
            else
            {
                com_rcp_send_pkg_cont(p_music_lyric_buf, (uint16) lyric_block);
            }

            //指针下移
            lyric_len -= lyric_block;
        }
        //关闭文件
        vfs_file_close(eh_vfs_mount, lrc_file_handle);
        //close
        lrc_file_handle = 0;
    }
    else
    {
        //没有歌词文件
        com_rcp_send_pkg(cur_file_seq, 0, music_rcp_buffer, 0, 0);
    }

    return RESULT_NULL;
}

MUS_STATIC __section__(".text.bank2") void _get_plist_over(void)
{
    adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL7, FREQ_NULL);
    change_ui_status(UI_STATUS_IDLE);
    s_timer_id_rcp_getplist = -1;
}

__section__(".text.bank2") app_result_e music_get_music_plist(uint32 data1, uint32 data2)
{
    music_rcp_plist_info_t *p_rcp_plist_info;
    uint8 *p_data_base = music_rcp_buffer + RCP_COMMAND_HEAD_SIZE;

    uint32 tmp_file_seq = data1; //文件序号(从1开始)
    uint32 tmp_file_count = data2;
    uint32 plist_info_len, count_one_time, i;

#if ((SUPPORT_PLIST_TITLE == 1) || (SUPPORT_PLIST_ARTIST == 1)\
    || (SUPPORT_PLIST_ALBUM == 1) || (SUPPORT_PLIST_GENRE == 1))

    if(g_scan_handle != NULL)
    {
        //mus_scan在占用内存，id3没有内存执行
        //按照约定，app/apk不会在没有scan完时，进入这里的
        return RESULT_NULL;
    }

#endif

    if (eh_vfs_mount == -1)
    {
        return RESULT_NULL;
    }

    if(s_timer_id_rcp_getplist == -1)
    {
        s_timer_id_rcp_getplist = set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL | (APP_TIMER_TAG_SYS << 8),
                500, (timer_proc) _get_plist_over);
        if(s_timer_id_rcp_getplist == -1)
        {
            PRINT_ERR("s_timer_id_rcp_getplist err1!");
        }
        adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL11, FREQ_NULL);
        change_ui_status(UI_STATUS_BUSY);
    }
    else
    {
        restart_app_timer(s_timer_id_rcp_getplist);
    }

    //APK已经处理好更新播放列表标志，可以清除更新播放列表标志了
    if(tmp_file_seq == 1)//从1开始
    {
        //重新刷新播放列表，默认进入根目录
        vfs_cd(eh_vfs_mount, CD_ROOT, 0);       
        music_rcp_dir_first();
        music_rcp_first_get();
        if (g_file_path_info.file_path.dirlocation.disk == DISK_U)
        {
            g_app_info_state.need_update_playlist &= (uint8)(~UHOST_NEED_UPLOAD);
        }
        else
        {
            g_app_info_state.need_update_playlist &= (uint8)(~CARD_NEED_UPLOAD);
        }
    }

    plist_info_len = 4 + 1;
#if (SUPPORT_PLIST_FILE == 1)
    plist_info_len += 64 + 1;
#endif
#if (SUPPORT_PLIST_TITLE == 1)
    plist_info_len += Id3BufSIZE + 1;
#endif
#if (SUPPORT_PLIST_ARTIST == 1)
    plist_info_len += ArtBufSIZE + 1;
#endif
#if (SUPPORT_PLIST_ALBUM == 1)
    plist_info_len += ArtBufSIZE + 1;
#endif
#if (SUPPORT_PLIST_GENRE == 1)
    plist_info_len += ArtBufSIZE + 1;
#endif

    count_one_time = (sizeof(music_rcp_buffer) - RCP_COMMAND_HEAD_SIZE) / plist_info_len;
    for (i = 0; i < tmp_file_count; i++)
    {
        p_rcp_plist_info = (music_rcp_plist_info_t*) (p_data_base + plist_info_len * (i % count_one_time));

        init_id3_info();
        //get id3 && music type
        music_rcp_get_list_id3(&g_cur_id3_info, (uint16) tmp_file_seq);

#if (SUPPORT_PLIST_TITLE == 1)
        //title
        libc_memcpy(p_rcp_plist_info->tit2_buffer, g_cur_id3_info.p_id3_info.tit2_buffer,
                MIN(g_cur_id3_info.p_id3_info.tit2_length, sizeof(p_rcp_plist_info->tit2_buffer)));
        p_rcp_plist_info->tit2_type = _music_info_type(p_rcp_plist_info->tit2_buffer);
#endif

#if (SUPPORT_PLIST_ARTIST == 1)
        //artist
        libc_memcpy(p_rcp_plist_info->tpe1_buffer, g_cur_id3_info.p_id3_info.tpe1_buffer,
                MIN(g_cur_id3_info.p_id3_info.tpe1_length, sizeof(p_rcp_plist_info->tpe1_buffer)));
        p_rcp_plist_info->tpe1_type = _music_info_type(p_rcp_plist_info->tpe1_buffer);
#endif

#if (SUPPORT_PLIST_ALBUM == 1)
        //album
        libc_memcpy(p_rcp_plist_info->talb_buffer, g_cur_id3_info.p_id3_info.talb_buffer,
                MIN(g_cur_id3_info.p_id3_info.talb_length, sizeof(p_rcp_plist_info->talb_buffer)));
        p_rcp_plist_info->talb_type = _music_info_type(p_rcp_plist_info->talb_buffer);
#endif

#if (SUPPORT_PLIST_FILE == 1)
        //file name
        libc_memcpy(p_rcp_plist_info->file_buffer, g_cur_id3_info.pfile_buf,
                MIN(g_cur_id3_info.name_len, sizeof(p_rcp_plist_info->file_buffer)));
        p_rcp_plist_info->file_type = _music_info_type(p_rcp_plist_info->file_buffer);
#endif

#if (SUPPORT_PLIST_GENRE == 1)
        //genre no load
#endif

        //获取指定序号文件的文件名和ID3信息，使用libc_memcpy避免数据不对齐访问异常
        libc_memcpy(&(p_rcp_plist_info->file_seq), &tmp_file_seq, sizeof(uint32));
        p_rcp_plist_info->music_type = _music_ext_type(g_file_path_info.file_path.dirlocation.file_info.file_extend_info.file_ext);
        //发送应答命令
        if (((i + 1) % count_one_time) == 0) //空间已满，必须发送
        {
            if (((i + 1) / count_one_time) == 1)
            {
                com_rcp_send_pkg(tmp_file_count, 0,
                        music_rcp_buffer, (uint16) (plist_info_len * count_one_time),
                        (uint16) (plist_info_len * tmp_file_count));
            }
            else
            {
                com_rcp_send_pkg_cont(p_data_base, (uint16) (plist_info_len * count_one_time));
            }
        }

        tmp_file_seq++;
    }

    if ((tmp_file_count % count_one_time) != 0)
    {
        if (tmp_file_count < count_one_time)
        {
            com_rcp_send_pkg(tmp_file_count, 0, music_rcp_buffer, (uint16) (plist_info_len * tmp_file_count),
                    (uint16) (plist_info_len * tmp_file_count));
        }
        else
        {
            com_rcp_send_pkg_cont(p_data_base, (uint16) (plist_info_len * (tmp_file_count % count_one_time)));
        }
    }

    //本函数执行时间：随意选择40首歌, 81+78+120+120+92+112+80+81=764毫秒

    return RESULT_NULL;
}

__section__(".text.bank2") void music_upload_music_plist(void)
{
    if(g_file_path_info.file_path.dirlocation.disk == DISK_U)
    {
        g_app_info_state.need_update_playlist |= UHOST_NEED_UPLOAD;
    }
    else
    {
        g_app_info_state.need_update_playlist |= CARD_NEED_UPLOAD;
    }
    PRINT_INFO("vfs changed.");
}


