/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：文件选择器装载，卸载，定位接口实现。
 * 作者：cailizhen
 ********************************************************************************/
 
#include "app_alarm.h"

static const uint8 card_drv_name[] = "card.drv";
static const uint8 uhost_drv_name[] = "uhost.drv";

//装载物理介质和文件系统
int32 alarm_fs_init(uint8 disk_type)
{
    uint8 drv_type;
    uint8 *drv_name;
    int32 vfs_mount;

    if (disk_type == DISK_H)
    {
        drv_type = DRV_GROUP_STG_CARD;
        drv_name = (uint8*) card_drv_name;
    }
    else if (disk_type == DISK_U)
    {
        drv_type = DRV_GROUP_STG_UHOST;
        drv_name = (uint8*) uhost_drv_name;
    }
    else
    {
        return -1;
    }

    //安装存储驱动
    if (sys_drv_install(drv_type, 0, drv_name) != 0)
    {
        return -1;
    }

    //挂载FS
    vfs_mount = sys_mount_fs(drv_type, disk_type, 0);
    if (vfs_mount == -1)
    {
        sys_drv_uninstall(drv_type);
    }

    return vfs_mount;
}

//卸载物理介质和文件系统
bool alarm_fs_deinit(uint8 disk_type, int32 file_sys_id)
{
    int32 ret_val = 0;
    uint8 drv_type;

    if (file_sys_id != -1)
    {
        ret_val = sys_unmount_fs(file_sys_id);
        file_sys_id = -1;
    }
    else
    {
        return TRUE;
    }

    if (0 > ret_val)
    {
        return FALSE;
    }

    if (disk_type == DISK_H)
    {
        drv_type = DRV_GROUP_STG_CARD;
    }
    else if (disk_type == DISK_U)
    {
        drv_type = DRV_GROUP_STG_UHOST;
    }
    else
    {
        return FALSE;
    }

    ret_val = sys_drv_uninstall(drv_type);
    if (0 > ret_val)
    {
        return FALSE;
    }

    return TRUE;
}

//初始化文件选择器
bool alarm_fsel_init(uint8 disk_type, uint8 ring_type, uint16 ring_file_no)
{
    file_location_t *plocation;
    uint32 ext_name = 0;
    bool result = FALSE;
    uint16 total = 0;

    do
    {
        g_fs_mount_id = alarm_fs_init(disk_type);
        if (g_fs_mount_id == -1)
        {
            break;
        }

        //进入根目录，进入alarm目录，并fsel第一个音乐文件
        if (vfs_cd(g_fs_mount_id, CD_ROOT, 0) == FALSE)
        {
            break;
        }

        //闹铃目录：“闹钟” -> 闹铃目录
        if ((ring_type >= RING_TYPE_FOLDER1) && (ring_type <= RING_TYPE_FOLDER4))
        {
            uint8 index;
            uint8 dir_type = DIR_HEAD;
            uint16 i;
            uint8 tmp_folder_name[12];

            //long name
            com_load_share_data(COM_SHARE_DATA_RING_FLODER_ROOT, tmp_folder_name, 12, 1);
            if (vfs_cd(g_fs_mount_id, CD_SUB, tmp_folder_name) == FALSE)
            {
                //short name
                com_load_share_data(COM_SHARE_DATA_RING_FLODER_ROOT, tmp_folder_name, 12, 0);
                if (vfs_cd(g_fs_mount_id, CD_SUB, tmp_folder_name) == FALSE)
                {
                    break;
                }
            }

            //cd to target dir
            index = (ring_type - RING_TYPE_FOLDER1);
            //long name
            com_load_share_data(COM_SHARE_DATA_RING_FOLDER_SUB, tmp_folder_name, 12, (1 << 8) | index);
            if (vfs_cd(g_fs_mount_id, CD_SUB, tmp_folder_name) == FALSE)
            {
                //short name
                com_load_share_data(COM_SHARE_DATA_RING_FOLDER_SUB, tmp_folder_name, 12, (0 << 8) | index);
                if (vfs_cd(g_fs_mount_id, CD_SUB, tmp_folder_name) == FALSE)
                {
                    break;
                }
            }

            while (1)
            {
                if (vfs_dir(g_fs_mount_id, dir_type, NULL, MUSIC_BITMAP) == 0)
                {
                    break;
                }
                total++;
                dir_type = DIR_NEXT;
            }

            if (total < ring_file_no)
            {
                break;
            }

            //dir to point file
            dir_type = DIR_HEAD;

            for (i = 0; i < ring_file_no; i++)
            {
                ext_name = vfs_dir(g_fs_mount_id, dir_type, NULL, MUSIC_BITMAP);
                dir_type = DIR_NEXT;
            }
            //the last one is our target
            if (ext_name != 0)
            {
                result = TRUE;
            }
        }
    } while (0);

    if (result == TRUE)
    {
        //构建文件路径
        plocation = &(g_alarm_path.file_path.dirlocation);
        plocation->disk = disk_type;
        libc_memcpy(plocation->file_info.file_extend_info.file_ext, &ext_name, 4);
        plocation->dir_layer = 0;
        //get dir location

        if (vfs_file_dir_offset(g_fs_mount_id, &plocation->dir_layer_info,\
            &plocation->file_info.file_extend_info.cluster_no, 0) == TRUE)
        {
            plocation->file_total = total;
            plocation->file_num = ring_file_no;
        }
        else
        {
            result = FALSE;
        }
    }

    alarm_fs_deinit(disk_type, g_fs_mount_id);
    return result;
}
