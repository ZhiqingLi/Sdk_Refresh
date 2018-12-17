#include "music.h"

//初始化g_cur_id3_info
void init_id3_info(void)
{
    libc_memset(&g_cur_id3_info.p_id3_info, 0, sizeof(id3_info_t));

    libc_memset(g_id3_filename_buf, 0, sizeof(g_id3_filename_buf));
    g_cur_id3_info.pfile_buf = g_id3_filename_buf;
    g_cur_id3_info.name_len = sizeof(g_id3_filename_buf);

    libc_memset(g_id3_title_buf, 0, sizeof(g_id3_title_buf));
    g_cur_id3_info.p_id3_info.tit2_buffer = g_id3_title_buf;
    g_cur_id3_info.p_id3_info.tit2_length = sizeof(g_id3_title_buf);

    libc_memset(g_id3_artist_buf, 0, sizeof(g_id3_artist_buf));
    g_cur_id3_info.p_id3_info.tpe1_buffer = g_id3_artist_buf;
    g_cur_id3_info.p_id3_info.tpe1_length = sizeof(g_id3_artist_buf);

    libc_memset(g_id3_album_buf, 0, sizeof(g_id3_album_buf));
    g_cur_id3_info.p_id3_info.talb_buffer = g_id3_album_buf;
    g_cur_id3_info.p_id3_info.talb_length = sizeof(g_id3_album_buf);
}

MUS_STATIC bool _get_lyric_name(music_id3_info_t *rcp_id3_info)
{
    //save current path
    //vfs_file_dir_offset(eh_vfs_mount, &g_temp_pdir_layer, &g_temp_pfile_offset, 0);

    //获取文件名
    vfs_get_name(eh_vfs_mount, rcp_id3_info->pfile_buf, rcp_id3_info->name_len / 2 - 2);

    //restore saveed path
    //vfs_file_dir_offset(eh_vfs_mount, &g_temp_pdir_layer, &g_temp_pfile_offset, 1);
    return TRUE;
}

HANDLE music_lyric_check(uint32* len_val)
{
    //当前歌曲的lrc
    HANDLE lrc_file_handle;
    //uint32 lyric_len;
    //bool ret = FALSE;
    bool get_ret;
    init_id3_info();
    //save current path
    vfs_file_dir_offset(eh_vfs_mount, &g_temp_pdir_layer, &g_temp_pfile_offset, 0);
    //set to cur file path
    get_ret = vfs_file_dir_offset(eh_vfs_mount, &g_file_path_info.file_path.dirlocation.dir_layer_info,
            &g_file_path_info.file_path.dirlocation.file_info.file_extend_info.cluster_no, 1);
    if (get_ret == TRUE)
    {
        _get_lyric_name(&g_cur_id3_info);
    }

    lrc_file_handle = lyric_open(g_cur_id3_info.pfile_buf, len_val);
    //restore saveed path
    vfs_file_dir_offset(eh_vfs_mount, &g_temp_pdir_layer, &g_temp_pfile_offset, 1);

    return lrc_file_handle;
}

bool music_is_has_lyric(void)
{
    //当前歌曲的lrc
    HANDLE lrc_file_handle;
    uint32 lyric_len;
    bool ret = FALSE;
    //check lyric exist
    lrc_file_handle = music_lyric_check(&lyric_len);

    if (lrc_file_handle != 0)
    {
        //关闭文件
        vfs_file_close(eh_vfs_mount, lrc_file_handle);
        ret = TRUE;
    }

    return ret;
}

