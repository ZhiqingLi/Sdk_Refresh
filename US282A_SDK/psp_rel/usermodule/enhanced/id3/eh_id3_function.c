#include "eh_id3.h"

//music playlist
static const char no_artist[] = "Unknown Artist";
static const char no_album[] = "Unknown Album";
static const char no_genre[] = "Unknown Genre";
static const char no_drm[] = "Unknown Drm";
static const char no_Track[] = "No Track";
//audible playlist
static const char no_book[] = "Unknown Book";
static const char no_author[] = "Unknown Author";

static const char album_jpg_name[] = "Folder";
static const char album_jpg_name2[] =
{ 0xff, 0xfe, 'F', 0x00, 'o', 0x00, 'l', 0x00, 'd', 0x00, 'e', 0x00, 'r', 0x00, 0x00, 0x00, };

static uint8 bank_temp_buf[96] _BANK_DATA_ATTR_;

void check_id3_is_empty(id3_info_t *id3_info, id3_type_e file_type);
void filtrate_char_sub(uint8 *buf_pt);
static void find_album_jpg(id3_info_t *id3_info);

/******************************************************************************/
/*
 * \par  Description:过滤id3信息中的字符
 *
 * \param[in]    id3_info，id3的buf

 * \param[out]

 * \return

 * \note
 *******************************************************************************/
void filtrate_id3_char(id3_info_t *id3_info)
{
    uint32 *buf_pt;
    uint8 *len_pt;
    uint8 count = 6;

    buf_pt = (uint32*) id3_info;
    len_pt = (uint8*) id3_info + (4 * count);

    while (count > 0)
    {
        if ((*len_pt > 0) && (*buf_pt != 0x00))
        {
            filtrate_char_sub((uint8*) (*buf_pt));
        }

        len_pt++;
        buf_pt++;
        count--;

    }

}

/******************************************************************************/
/*
 * \par  Description:过滤buf中的空格字符
 *
 * \param[in]

 * \param[out]

 * \return

 * \note
 *******************************************************************************/
void filtrate_char_sub(uint8 *buf_pt)
{
    uint8 flag, last_flag, uni_flag;
    uint16 *pt16;
    uint8 *pt8;
    uint8 *start, *end;
    //uint8 code_flag=0;

    flag = 0;
    last_flag = 0;
    end = start = buf_pt;

    if ((*buf_pt == 0xff) && (*(buf_pt + 1) == 0xfe))
    {
        //code_flag=1;
        pt16 = (uint16*) (buf_pt + 2);
        while (*pt16 != 0x0000)
        {
            if (*pt16 == 0x0020)
            {
                last_flag = 0;
                if (flag == 0)
                {
                    start = (uint8*) (pt16 + 1);
                }
                else
                {
                    end = (uint8*) pt16;
                }
            }
            else
            {
                flag = 1;
                last_flag = 1;
                end = (uint8*) pt16;
            }
            pt16++;
        }

        uni_flag = 2;
    }
    else
    {
        pt8 = (uint8*) buf_pt;
        while (*pt8 != 0x00)
        {
            if (*pt8 == 0x20)
            {
                last_flag = 0;
                if (flag == 0)
                {
                    start = pt8 + 1;
                }
                else
                {
                    end = pt8;
                }
            }
            else
            {
                flag = 1;
                last_flag = 1;
                end = pt8;
            }
            pt8++;
        }
        uni_flag = 1;
    }

    if ((flag | last_flag) == 0)
    {
        //全是空格
        *buf_pt = 0x00;
        return;
    }
    else if (last_flag == 0)
    {
        //后面有空格
        *end = 0x00;
        *(end + 1) = 0x00;
    }
    else
    {
        ; //不处理
    }

    if ((start != buf_pt) && (end > start))
    {
        uint32 cpy_count;
        cpy_count = (uint32) (end - start) + uni_flag + 1;
        libc_memcpy(buf_pt, start, cpy_count);
    }
    return;

}

//过滤空格,转换为 xx.yyy格式
void deal_if_83name(uint8 * p_file_name)
{
    uint8 i, j;

    if ((p_file_name[0] == 0xff) && (p_file_name[1] == 0xfe))
    {
        return;
    }

    //扩展名往后移1个字节
    for (i = 11; i > 8; i--)
    {
        p_file_name[i] = p_file_name[i - 1];
    }

    //过滤文件名空格
    for (i = 7; i > 0; i--)
    {
        if (p_file_name[i] != 0x20)
        {
            break;
        }
    }

    //加点
    i++;
    p_file_name[i] = 0x2e; //加"."

    //过滤扩展名空格
    for (j = 9; j < 12; j++)
    {
        if (p_file_name[j] != 0x20)
        {
            i++;
            p_file_name[i] = p_file_name[j];
        }
    }

    if (p_file_name[i] != 0x2e)
    {
        i++;
    }

    p_file_name[i] = 0x00;

    return;
}

/******************************************************************************/
/*
 * \par  Description:检查id3是否为空，空就填入指定字符串
 *
 * \param[in]    id3_info，id3的buf
 file_type,id3类型
 * \param[out]

 * \return

 * \note
 *******************************************************************************/
void check_id3_is_empty(id3_info_t *id3_info, id3_type_e file_type)
{
    //若title为空，填入文件名
    if ((id3_info->tit2_length > 4) && (id3_info->tit2_buffer[0] == 0x00))
    {
        vfs_get_name(eh_vfs_mount, id3_info->tit2_buffer, id3_info->tit2_length / 2 - 2);
        deal_if_83name(id3_info->tit2_buffer);
    }

    if ((id3_info->talb_length > sizeof(no_album)) && (id3_info->talb_buffer[0] == 0x00))
    {

        switch (file_type)
        {
            case ID3_TYPE_AA:
            case ID3_TYPE_AAC:
            case ID3_TYPE_AAX:
            libc_memcpy(id3_info->talb_buffer, no_album, sizeof(no_book));
            break;
            default:
            libc_memcpy(id3_info->talb_buffer, no_album, sizeof(no_album));
            break;
        }

    }

    if ((id3_info->genre_length > sizeof(no_genre)) && (id3_info->genre_buffer[0] == 0x00))
    {
        libc_memcpy(id3_info->genre_buffer, no_genre, sizeof(no_genre));
    }

    if ((id3_info->drm_length > sizeof(no_drm)) && (id3_info->drm_buffer[0] == 0x00))
    {
        libc_memcpy(id3_info->drm_buffer, no_drm, sizeof(no_drm));
    }

    if ((id3_info->track_length > sizeof(no_Track)) && (id3_info->track_buffer[0] == 0x00))
    {
        libc_memcpy(id3_info->track_buffer, no_Track, sizeof(no_Track));
    }

    if ((id3_info->tpe1_length > sizeof(no_artist)) && (id3_info->tpe1_buffer[0] == 0x00))
    {
        switch (file_type)
        {
            case ID3_TYPE_AA:
            case ID3_TYPE_AAC:
            case ID3_TYPE_AAX:
            libc_memcpy(id3_info->tpe1_buffer, no_author, sizeof(no_author));
            break;
            default:
            libc_memcpy(id3_info->tpe1_buffer, no_artist, sizeof(no_artist));
            break;
        }
    }

    if ((id3_info->apic_flag == 1) && (id3_info->apic_type == 0))
    {
        //没有内嵌图片，找默认专辑图片
        find_album_jpg(id3_info);
    }

    return;
}

//检查buffer
void check_id3_buffer(id3_info_t *id3_info, id3_type_e file_type)
{

    //过滤字符串前后的空格键
#if(FILTRATE_SPACE_SW==1)
    filtrate_id3_char(id3_info);
#endif
    check_id3_is_empty(id3_info, file_type);
    return;
}

// find album jpg
static void find_album_jpg(id3_info_t *id3_info)
{
    uint8 result = 1;
    //save current path

    sys_os_sched_lock();
    
    vfs_file_dir_offset(eh_vfs_mount, bank_temp_buf, bank_temp_buf + 76, 0);

    if (0 == vfs_dir(eh_vfs_mount, DIR_HEAD, album_jpg_name, EXT_NAME_JPG))
    {
        //try uncoide name
        if (0 == vfs_dir(eh_vfs_mount, DIR_HEAD, album_jpg_name2, EXT_NAME_JPG))
        {
            result = 0;
        }
    }

    if (result == 1)
    {
        //内嵌图片，apic_offset为指针buffer 输入，存jpg 的cluster_no和dir_entry
        vfs_file_dir_offset(eh_vfs_mount, NULL, (uint8* ) id3_info->apic_offset, 0);
        id3_info->apic_type = 2; //set album jpg flag
    }

    // recover path
    vfs_file_dir_offset(eh_vfs_mount, bank_temp_buf, bank_temp_buf + 76, 1);

    sys_os_sched_unlock();
}

