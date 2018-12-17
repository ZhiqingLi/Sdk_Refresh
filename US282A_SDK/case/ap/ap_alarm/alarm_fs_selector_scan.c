/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：闹铃目录扫描功能接口实现。
 * 作者：cailizhen
 ********************************************************************************/

#include "app_alarm.h"

uint8 g_dir_file_name[512] _BANK_DATA_ATTR_;

/******************************************************************************/
/*
 * \par  Description: 扫描文件
 *
 * \param[in]

 * \param[out]

 * \return        当前目录的文件总数

 * \note
 *******************************************************************************/
uint8 alarm_scan_ring_folder_do(uint8* dir_name_long, uint8* dir_name_short)
{
    uint8 *p_file_name_buf;
    uint8 filenum;
    uint8 dir_type;

    //查找特殊目录
    //long name
    if (vfs_cd(g_fs_mount_id, CD_SUB, dir_name_long) == FALSE)
    {
        //short name
        if (vfs_cd(g_fs_mount_id, CD_SUB, dir_name_short) == FALSE)
        {
            return 0xff;
        }
    }

    filenum = 0;
    dir_type = DIR_HEAD;
    while (vfs_dir(g_fs_mount_id, dir_type, NULL, MUSIC_BITMAP) != 0)
    {
        //cur file name 
        p_file_name_buf = &g_dir_file_name[(g_dir_file_total % RING_FOLDER_FILENUM_PER_SEC) * RING_FOLDER_FILENAME_LEN];
        vfs_get_name(g_fs_mount_id, p_file_name_buf, RING_FOLDER_FILENAME_LEN/2-2);

        //one dir max file number
        g_dir_file_total++;
        filenum++;
        if ((g_dir_file_total % RING_FOLDER_FILENUM_PER_SEC) == 0)
        {
            uint32 write_page;

            write_page = (g_dir_file_total / RING_FOLDER_FILENUM_PER_SEC) - 1;
            sys_vm_write(g_dir_file_name, VM_RING_FOLDER_BASE + (write_page*0x10000), 512);
            libc_memset(g_dir_file_name, 0, sizeof(g_dir_file_name));
        }
        if (g_dir_file_total >= RING_FOLDER_FILENUM_MAX)
        {
            break;
        }

        dir_type = DIR_NEXT;
    }

    //返回到上一级目录
    if (FALSE == vfs_cd(g_fs_mount_id, CD_BACK, NULL))
    {
        return 0xff;
    }

    return filenum;
}

/******************************************************************************/
/*
 * \par  Description: 扫描文件
 *
 * \param[in]

 * \param[out]

 * \return        文件总数

 * \note
 *******************************************************************************/

int alarm_scan_ring_folder(void)
{
    uint8 alarm_folder_filenum[RCP_RING_FOLDER_MAX];
    uint8 filenum, i;
    uint8 tmp_folder_name[12];
    uint8 tmp_sub_folder_long[12];
    uint8 tmp_sub_folder_short[12];
    int ret_val = 0;
    PRINT_INFO("enter alarm ring scan");

    libc_memset(alarm_folder_filenum, 0x00, sizeof(alarm_folder_filenum));

    g_fs_mount_id = alarm_fs_init(DISK_H);
    if (g_fs_mount_id == -1)
    {
        //return;    
        ret_val = -1;
        goto ret_exit;
    }
    //进入根目录
    vfs_cd(g_fs_mount_id, CD_ROOT, 0);

    //查找"闹钟"目录
    //long name
    com_load_share_data(COM_SHARE_DATA_RING_FLODER_ROOT, tmp_folder_name, 12, 1);
    if (vfs_cd(g_fs_mount_id, CD_SUB, tmp_folder_name) == FALSE)
    {
        //short name
        com_load_share_data(COM_SHARE_DATA_RING_FLODER_ROOT, tmp_folder_name, 12, 0);
        if (vfs_cd(g_fs_mount_id, CD_SUB, tmp_folder_name) == FALSE)
        {
            ret_val = -2;
            goto ret_exit;
        }
    }

    //scan dir 
    g_dir_file_total = 0;
    //clear as zero
    libc_memset(g_dir_file_name, 0x00, sizeof(g_dir_file_name));
    for (i = 0; (i < RCP_RING_FOLDER_MAX) && (g_dir_file_total < RING_FOLDER_FILENUM_MAX); i++)
    {
        com_load_share_data(COM_SHARE_DATA_RING_FOLDER_SUB, tmp_sub_folder_long, 12, (1 << 8) | i);
        com_load_share_data(COM_SHARE_DATA_RING_FOLDER_SUB, tmp_sub_folder_short, 12, (0 << 8) | i);

        filenum = alarm_scan_ring_folder_do(tmp_sub_folder_long, tmp_sub_folder_short);
        PRINT_INFO_INT("ring floder:", filenum);
        if (filenum != 0xff)
        {
            alarm_folder_filenum[i] = filenum;
        }
    }

    if ((g_dir_file_total % RING_FOLDER_FILENUM_PER_SEC) != 0)
    {
        uint32 write_page;

        write_page = g_dir_file_total / RING_FOLDER_FILENUM_PER_SEC;
        sys_vm_write(g_dir_file_name, VM_RING_FOLDER_BASE + (write_page*0x10000), 512);
    }

    ret_exit:
    //保存闹铃目录下各个子目录下的文件数目
    time_alarm_record_list_operate(RECORD_LIST_WRITE_FOLDERNUM, alarm_folder_filenum, 0);

    //退出文件系统
    alarm_fs_deinit(DISK_H, g_fs_mount_id);
    return ret_val;
}

