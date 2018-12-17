#include "music.h"

uint8 g_id3_title_buf[64];
uint8 g_id3_artist_buf[32];
uint8 g_id3_album_buf[32];
uint8 g_id3_filename_buf[64];

//MUS_STATIC id3_info_t cur_id3_info _BANK_DATA_ATTR_;
//MUS_STATIC id3_info_t cur_id3_info;
pfile_offset_t g_temp_pfile_offset;
pdir_layer_t g_temp_pdir_layer;

//第一个扇区的头12个字节装的是list_head_info_t
//接着开始存list_dir_info_t
//list_dir_info_t是20个字节的
//
//第一个元素：第一个512字节存的第一个list_dir_info_t结构的序号(1开始)：1
//第二个元素：第二个512字节存的第一个list_dir_info_t结构的序号(1开始)：26
//第三个元素：第三个512字节存的第一个list_dir_info_t接口的序号(1开始)：52
//              这个扇区的头上entry_off[2](即8)字节装的是51号(0开始)的一部分
//...
//最后一个元素：第八个512字节的最后一个list_dir_info_t结构(它是完整的)的序号
MUS_STATIC const uint8 dir_bound[] =
{
    1, 26, 52, 78, 103, 129, 154, 180, 204
};

MUS_STATIC const uint8 entry_off[] =
{
    12, 0, 8, 16, 4, 12, 0, 8
};

//for vram addr
MUS_STATIC const uint32 vram_addr[] =
{
    VM_PLIST_INFO_START0,
    VM_PLIST_INFO_START1,
    VM_PLIST_INFO_START2,
    VM_PLIST_INFO_START3,
    VM_PLIST_INFO_START4,
    VM_PLIST_INFO_START5,
    VM_PLIST_INFO_START6,
    VM_PLIST_INFO_START7
};

//定义const data 所有后缀数据
MUS_STATIC const uint8 support_ext[MUSIC_EXT_NUM][4] =
{
    "MP1", //mp3
    "MP2", //mp3
    "MP3", //mp3
    "WMA", //wma
    "APE", //ape
    "FLA", //flac
    "OGG", //ogg
};

MUS_STATIC const id3_type_e support_id3_type[] =
{
    ID3_TYPE_MP3, //mp1
    ID3_TYPE_MP3, //mp2
    ID3_TYPE_MP3, //mp3
    ID3_TYPE_WMA, //wma
    ID3_TYPE_APE, //ape
    ID3_TYPE_FLAC, //fla
    ID3_TYPE_OGG, //ogg
};

 id3_type_e _check_id3_type(uint8 *extname)
{
    uint8 cnt;
    id3_type_e ret;
    //转换当前后缀名(转成大写)
    extname[0] = ((extname[0] >= 'a') && (extname[0] <= 'z')) ? (extname[0] - 'a' + 'A') : extname[0];
    extname[1] = ((extname[1] >= 'a') && (extname[1] <= 'z')) ? (extname[1] - 'a' + 'A') : extname[1];
    extname[2] = ((extname[2] >= 'a') && (extname[2] <= 'z')) ? (extname[2] - 'a' + 'A') : extname[2];
    //比较后缀
    for (cnt = 0; cnt < MUSIC_EXT_NUM; cnt++)
    {
        if (libc_strncmp(extname, support_ext[cnt], sizeof(support_ext[cnt])) == 0)
        {
            ret = support_id3_type[cnt];
            break;
        }
    }
    if (cnt == MUSIC_EXT_NUM)
    {
        ret = ID3_TYPE_END;
    }
    return ret;
}

 bool _rcp_get_id3_info(id3_info_t *id3_info_ptr, uint32 file_ext_name)
{
    //file id3 check
    id3_type_e cur_id3_type;
    cur_id3_type = _check_id3_type((uint8 *) &file_ext_name);
    if (cur_id3_type != ID3_TYPE_END)
    {
        //存在id3_info_t
        fsel_get_id3_info(id3_info_ptr, NULL, cur_id3_type);
    }
    return TRUE;
}

 bool music_get_name_info(music_id3_info_t *rcp_id3_info)
{
    //save current path
    //vfs_file_dir_offset(eh_vfs_mount, &g_temp_pdir_layer, &g_temp_pfile_offset, 0);

    //获取文件名
    vfs_get_name(eh_vfs_mount, rcp_id3_info->pfile_buf, rcp_id3_info->name_len / 2 - 2);

    if(rcp_id3_info->p_id3_info.tit2_buffer[0] == 0)
    {
        libc_memcpy(rcp_id3_info->p_id3_info.tit2_buffer,
                rcp_id3_info->pfile_buf,
                (uint32)MIN(rcp_id3_info->p_id3_info.tit2_length,
                rcp_id3_info->name_len));
    }
    //restore saveed path
    //vfs_file_dir_offset(eh_vfs_mount, &g_temp_pdir_layer, &g_temp_pfile_offset, 1);
    return TRUE;
}

//根据目录的序号找到目录项
//dir_no从1开始编号
//如果没有找到(到底了)，返回FALSE
 bool _get_dir_by_dirno(uint16 dir_no, list_dir_info_t *p_dir)
{
    uint8 sector_index;
    uint32 pos;
    uint32 offset;
    uint32 byte_count;

    if (dir_no > g_vmdir_total)
    {
        return FALSE;
    }

    //算出它在哪个扇区

    //第一个扇区的头sizeof(list_head_info_t)字节不是存放的list_dir_info_t
    pos = sizeof(list_head_info_t) + (dir_no - 1) * sizeof(list_dir_info_t);
    sector_index = (uint8) (pos / DIR_INFO_SECTOR_SIZE + 1); //从1开始编号
    if (sector_index > DIR_INFO_SECTOR_COUNT)
    {
        return FALSE;
    }

    if (sector_index != g_sector_no)
    {
        g_sector_no = sector_index;
        sys_vm_read(g_sector_buf, vram_addr[g_sector_no - 1], sizeof(g_sector_buf));
    }

    offset = pos % DIR_INFO_SECTOR_SIZE;
    if ((offset + sizeof(list_dir_info_t)) < DIR_INFO_SECTOR_SIZE)
    {
        libc_memcpy(p_dir, &g_sector_buf[offset], sizeof(*p_dir));
        return TRUE;
    }

    //还有一部分在g_sector_buf中
    byte_count = DIR_INFO_SECTOR_SIZE - offset;
    libc_memcpy(p_dir, &g_sector_buf[offset], byte_count);

    //再读剩下的部分
    if (g_sector_no >= (sizeof(vram_addr)/sizeof(uint32)))
    {
        g_sector_no = 0;
    }
    sys_vm_read(g_sector_buf, vram_addr[g_sector_no], sizeof(g_sector_buf));
    g_sector_no++;
    
    libc_memcpy((uint8 * )p_dir + byte_count, g_sector_buf,
            sizeof(list_dir_info_t) - byte_count);

    return TRUE;
}

//定位文件所在的目录项
//file_no: 文件序号(从1开始编号)
//找到了，结果装到g_cur_dir，返回TRUE
//如果没找到，返回FALSE
bool _get_dir_by_fileno(uint16 file_no)
{
    list_dir_info_t next_dir;

    if (file_no < g_cur_dir.begin_index)
    {
        //从第一个DIR开始找
        _get_dir_by_dirno(1, &g_cur_dir);
        g_cur_dir_no = 1;
    }

    for (;;)
    {
        if (!_get_dir_by_dirno(g_cur_dir_no + 1, &next_dir))
        {
            //已经到底了
            if (file_no >= g_cur_dir.begin_index)
            {
                //找到了
                return TRUE;
            }
            //不可能进入这里
            return FALSE;
        }

        if (file_no < next_dir.begin_index)
        {
            //找到了
            return TRUE;
        }

        libc_memcpy(&g_cur_dir, &next_dir, sizeof(g_cur_dir));
        g_cur_dir_no++;
    }
}

//file_index: 文件序号(从1开始编号)
bool music_rcp_get_list_id3(music_id3_info_t *rcp_id3_info, uint16 file_index)
{
    uint32 cur_ext_name = 0;
    uint16 gap_num = 0;
    list_dir_info_t entry;
    bool prev_flag = FALSE;

    if(g_last_dir_no == 0)
    {
        //music scan刚刚结束
        g_cur_dir_no = 1;
        _get_dir_by_dirno(g_cur_dir_no, &g_cur_dir);
        g_last_dir_no = g_cur_dir_no;
        vfs_get_name(eh_vfs_mount, &cur_ext_name, 0);
    }
    else
    {
        libc_memcpy(&entry, &g_cur_dir, sizeof(entry));

        if (file_index < g_cur_dir.begin_index)
        {
            prev_flag = TRUE;
        }
        if (!_get_dir_by_fileno(file_index))
        {
            //这个文件超出了记录(vram里面没有记录它)
            return FALSE;
        }

        if (g_cur_dir_no != g_last_dir_no)
        {
            //跳到目录
            if(!vfs_jump_to_direntry(eh_vfs_mount,
                ((prev_flag == TRUE) ? PREV_FLAG : 0), &g_cur_dir, &entry))
            {
                PRINT_ERR_STR("E1");
                return FALSE;
            }
            g_last_dir_no = g_cur_dir_no;
            gap_num = (file_index - (g_cur_dir.begin_index - 1));
            prev_flag = FALSE;
        }
        else if(file_index > g_last_get_index)
        {
            prev_flag = FALSE;
            gap_num = file_index - g_last_get_index;
        }
        else
        {
            prev_flag = TRUE;
            gap_num = g_last_get_index - file_index;
        }
        cur_ext_name = vfs_dir_current_entry_file(eh_vfs_mount,
            (prev_flag ? PREV_FLAG : 0) | gap_num,
            &g_cur_dir, MUSIC_BITMAP);
    }
    if (cur_ext_name == 0)
    {
        PRINT_ERR_INT("E2:", gap_num);
        return FALSE;
    }

#if ((SUPPORT_PLIST_TITLE == 1) || (SUPPORT_PLIST_ARTIST == 1) || (SUPPORT_PLIST_ALBUM == 1))
    //获取ID3
    _rcp_get_id3_info(&rcp_id3_info->p_id3_info, cur_ext_name);
#endif
    //获取文件名
    music_get_name_info(rcp_id3_info);
    g_last_get_index = file_index;

    return TRUE;
}

bool __section__(".text.bank2") music_rcp_get_cur_info(music_id3_info_t *rcp_id3_info)
{
    bool get_ret;
    uint32 now_ext_name;

    //save current path
    vfs_file_dir_offset(eh_vfs_mount, &g_temp_pdir_layer, &g_temp_pfile_offset, 0);
    //set to cur file path
    get_ret = vfs_file_dir_offset(eh_vfs_mount, &g_file_path_info.file_path.dirlocation.dir_layer_info,
            &g_file_path_info.file_path.dirlocation.file_info.file_extend_info.cluster_no, 1);
    if (get_ret == TRUE)
    {
        vfs_get_name(eh_vfs_mount, &now_ext_name, 0);
        _rcp_get_id3_info(&rcp_id3_info->p_id3_info, now_ext_name);
        music_get_name_info(rcp_id3_info);
    }

    //restore saveed path
    vfs_file_dir_offset(eh_vfs_mount, &g_temp_pdir_layer, &g_temp_pfile_offset, 1);
    return get_ret;
}


bool get_dir_by_dirno(uint16 dir_no, list_dir_info_t *p_dir)
{
    return _get_dir_by_dirno(dir_no,p_dir);
}
