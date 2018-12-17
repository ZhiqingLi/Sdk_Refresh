#include "enhanced.h"
#include "eh_globle_data.h"

#define  VM_AP_LYRIC_FILE       0x00002000//歌词文件，占用4KB
#define  VM_AP_LYRIC_LENGTH     0x00002e00//歌词文件长度，在扇区最后4字节结构体如下
                                          //{uint16 magic = 0x5a5a;uint16 lyric_len;}
#define LRC_EXT_BITMAP EXT_NAME_LRC
uint8 lrc_temp_buf[EH_SECTOR_SIZE] _BANK_DATA_ATTR_; //读lrc缓存
HANDLE lrc_handle _BANK_DATA_ATTR_;
uint32 lrc_pos_file _BANK_DATA_ATTR_;
uint32 lrc_file_len _BANK_DATA_ATTR_;
static const uint8 lrc_asc[] = "LRC";

uint16 len_sting(uint8 *str, uint8 str_type);
bool open_lrc_file(uint8 *lrc_name);

//后续考虑增加ID3歌词标签
//1 SYLT Synchronized lyric/text，http://id3.org/id3v2.3.0#sec4.10
//2 USLT Unsychronized lyric/text，http://id3.org/id3v2.3.0#sec4.9
//3 Lyrics3v2 千千静听内嵌歌词格式，放在APEv2之后，ID3v1之前，http://id3.org/Lyrics3v2

/******************************************************************************/
/*
 * \par  Description: 装入lrc歌词
 *
 * \param[in]    music_filename，歌曲名

 * \param[out]

 * \return        TRUE OR FALSE

 * \note
 *******************************************************************************/
HANDLE lyric_open(char * music_filename,uint32* file_len)
{
    //uint8 *lrc_vram_p;
    //uint8 result = TRUE;

    /*if (lrc_handle != 0)
    {
        vfs_file_close(eh_vfs_mount, lrc_handle);
        lrc_handle = 0;
    }

    if (FALSE != open_lrc_file(music_filename))
    {
        //将歌词文件加载到VRAM中
        if (lrc_file_len >= EH_SECTOR_SIZE*8 - 4)
        {//歌词内容最长为4KB，但尾部要保留4字节写入歌词magic及其长度
            lrc_file_len = EH_SECTOR_SIZE*8 - 4;
        }
        lrc_vram_p = VM_AP_LYRIC_FILE;
        lrc_pos_file = 0;
        while ((lrc_file_len - lrc_pos_file) >= EH_SECTOR_SIZE)
        {
            if (vfs_file_read(eh_vfs_mount, lrc_temp_buf, EH_SECTOR_SIZE, lrc_handle) != 0)
            {
                sys_vm_write(lrc_temp_buf, lrc_vram_p);
            }
            else
            {
                result = FALSE;
                break;
            }
            
            lrc_pos_file += EH_SECTOR_SIZE;
            lrc_vram_p += EH_SECTOR_SIZE;
        }
        
        libc_memset(lrc_temp_buf, 0x00, EH_SECTOR_SIZE);
        if ((lrc_file_len - lrc_pos_file) > 0)
        {
            if (vfs_file_read(eh_vfs_mount, lrc_temp_buf, (lrc_file_len - lrc_pos_file), lrc_handle) != 0)
            {
                if (lrc_vram_p != VM_AP_LYRIC_LENGTH)
                {
                    sys_vm_write(lrc_temp_buf, lrc_vram_p);
                    libc_memset(lrc_temp_buf, 0x00, EH_SECTOR_SIZE);
                }
            }
        }
        
        //写入歌词magic及其长度
        *(uint16*)(lrc_temp_buf + 508) = 0x5a5a;
        *(uint16*)(lrc_temp_buf + 510) = lrc_file_len;
        sys_vm_write(lrc_temp_buf, VM_AP_LYRIC_LENGTH);

        vfs_file_close(eh_vfs_mount, lrc_handle);
        lrc_handle = 0;   
    }
    else
    {
        result = FALSE;
    }
    
    if (result == FALSE)
    {//清除有效标志
        libc_memset(lrc_temp_buf, 0x00, EH_SECTOR_SIZE);
        sys_vm_write(lrc_temp_buf, VM_AP_LYRIC_LENGTH);
    }*/
//    sys_os_sched_lock();
    
    open_lrc_file(music_filename);
    
    *file_len = lrc_file_len;

//    sys_os_sched_unlock();

    return lrc_handle;
}


/******************************************************************************/
/*
 * \par  Description: 计算字符串的长度
 *
 * \param[in]    str，字符串内容
 str_type,字符的类型，1-unicode，0-内码

 * \param[out]

 * \return       字符的长度

 * \note
 *******************************************************************************/
uint16 len_sting(uint8 *str, uint8 str_type)
{
    uint16 len = 0;

    if (str_type == 1)
    {
        while (*(uint16*) str != 0x00)
        {
            len++;
            str = str + 2;
        }
    }
    else
    {
        while (*str != 0x00)
        {
            len++;
            str++;
        }
    }

    return len;

}

void copy_lrc_name(void *src_name, void *dst_name)
{
    uint32 i;
    uint8 *src_lrc_name_ansi;
    uint8 *dst_lrc_name_ansi;
    uint16 *src_lrc_name_uni;
    uint16 *dst_lrc_name_uni;
    if (*(uint16*) src_name == 0xfeff)
    {
        src_lrc_name_uni = (uint16 *)src_name;
        dst_lrc_name_uni = (uint16 *)dst_name;
        //unicode
        for(i = 0; i<64; i++)
        {
            //先确定结束符再确定后缀的.号位置
            if(src_lrc_name_uni[i] == 0x0000)
            {
                if(src_lrc_name_uni[i-4] == 0x002e)
                {
                    i=i-4;
                }
                else if(src_lrc_name_uni[i-5] == 0x002e)
                {
                    i=i-5;
                }
                else
                {
                    //qac;
                }
                libc_print("lrc i = ",i,2);
                break;
            } 
            dst_lrc_name_uni[i] = src_lrc_name_uni[i];
        }
        //添加后缀名
        dst_lrc_name_uni[i++] = 0x002e;
        dst_lrc_name_uni[i++] = 'l';
        dst_lrc_name_uni[i++] = 'r';
        dst_lrc_name_uni[i++] = 'c';
        dst_lrc_name_uni[i++] = 0;
    }
    else//ASCII
    {
        src_lrc_name_ansi = (uint8 *)src_name;
        dst_lrc_name_ansi = (uint8 *)dst_name;
        libc_memcpy(dst_lrc_name_ansi, src_lrc_name_ansi, (uint32)64);
        src_lrc_name_ansi[8] = 'l';
        src_lrc_name_ansi[9] = 'r';
        src_lrc_name_ansi[10] = 'c';
        src_lrc_name_ansi[11] = 0;
    }
    return ;
}
/******************************************************************************/
/*
 * \par  Description:打开歌词文件
 *
 * \param[in]    lrc_name，歌词文件名

 * \param[out]

 * \return      TRUE OR FALSE

 * \note
 *******************************************************************************/
bool open_lrc_file(uint8 *lrc_name)
{
    uint8 i;
    uint16 temp;
    uint32 tmp32;
    pfile_offset_t temp_pfile_offset;
    pdir_layer_t temp_pdir_layer; 
    if (lrc_name == NULL)
    {
        return FALSE;
    }

    if (*(uint16*) lrc_name == 0xfeff)
    {
        //unicode
        i = 1;
    }
    else
    {
        i = 0;
    }

    temp = len_sting(lrc_name, i);
    if ((0 == temp) || ((EH_SECTOR_SIZE / 2) < temp))
    {
        return FALSE;
    }

    lrc_handle = 0;
    copy_lrc_name(lrc_name, lrc_temp_buf);

    //save current path
    vfs_file_dir_offset(eh_vfs_mount, &temp_pdir_layer, &temp_pfile_offset, 0);

    if(vfs_file_dir_exist(eh_vfs_mount, &lrc_temp_buf, 1) == 0)
    {
        libc_print("NO exist lrc",0,0);
        return FALSE;
    }
    
    if (i > 0)
    {
        temp = temp-4;//去掉.XXX
        //unicode
//        tmp32 = vfs_dir(eh_vfs_mount, DIR_HEAD, NULL, LRC_EXT_BITMAP);

//        while(1)
//        {
//            if (0 != tmp32)
//            {
//                vfs_get_name(eh_vfs_mount,lrc_temp_buf,temp+1);//包含结束符
//                if(0 == libc_memcmp(lrc_name,lrc_temp_buf,(uint32)temp<<1))
//                {
//                    break;
//                }
//            }
//            else
//            {
//                break;
//            }
//            tmp32 = vfs_dir(eh_vfs_mount, DIR_NEXT, NULL, LRC_EXT_BITMAP);
//        }
        
//        if(0 != tmp32)
        {
            lrc_handle = vfs_file_open(eh_vfs_mount, lrc_temp_buf, R_NORMAL_SEEK);                
        }
//        else
//        {
//            lrc_handle = 0;
//        }
    }
    else//ASCII
    {
        temp = temp - 3;
        libc_memcpy(lrc_temp_buf, lrc_name, (uint32) temp);
        libc_memcpy(lrc_temp_buf + temp, lrc_asc, 4);
        lrc_handle = vfs_file_open(eh_vfs_mount, lrc_temp_buf, R_NORMAL_SEEK);
    }

    //back up path
    vfs_file_dir_offset(eh_vfs_mount, &temp_pdir_layer, &temp_pfile_offset, 1);

    if (lrc_handle == 0)
    {
        libc_print("!!open lrc error!!",0,0);
        return FALSE;
    }

    lrc_file_len = 0;
    vfs_file_get_size(eh_vfs_mount, &lrc_file_len, lrc_handle, 0);
   
    if (lrc_file_len < 16)
    {
        vfs_file_close(eh_vfs_mount, lrc_handle);
        lrc_handle = 0;
        libc_print("!!lrc len <16!!",0,0);
        return FALSE;
    }

    return TRUE;
}
