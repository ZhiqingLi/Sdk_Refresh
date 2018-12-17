#include "music.h"

int32 eh_vfs_mount = -1;

uint16 g_vmfile_total;
uint16 g_vmdir_total;
//vram record disk
uint16 g_vmdisk_flag;
uint16 g_plist_file_count;
//当前目录
list_dir_info_t g_cur_dir;
//当前目录的序号(1开始,0表示g_cur_dir里面的内容无效)
uint16 g_cur_dir_no;
//将vram的扇区读出来后的buffer
uint8 g_sector_buf[DIR_INFO_SECTOR_SIZE];
//读到了第几个扇区(0表示没有读过),即上面的buffer里面装的是第几个扇区的内容
uint8 g_sector_no = 0;
//上次定位到了哪个目录(的序号(从1开始))
uint16 g_last_dir_no = 0;
//上次定位的文件的序号(1开始)
uint16 g_last_get_index = 0;
//for id3 get(因为放在bss段,所以默认值都是0)
music_id3_info_t g_cur_id3_info;

rcp_file_status_t g_rcp_file_status;

MUS_STATIC const uint8 card_drv_name[] = "card.drv";
MUS_STATIC const uint8 uhost_drv_name[] = "uhost.drv";


//过滤根目录下的RECORD目录
MUS_STATIC bool _is_filter(void)
{
#ifdef FILTER_RECORD_DIR
    uint8 dir_name_buffer[32];
    uint16 name_len;
    uint8 add_len, i, j;
    uint8 *buf;
    uint8 rec_dir_name[8 + 1];

    libc_memset(rec_dir_name, 0, sizeof(rec_dir_name));
    com_load_share_data(COM_SHARE_DATA_REC_FOLDER, rec_dir_name, sizeof(rec_dir_name) - 1, 0);

    name_len = vfs_get_name(eh_vfs_mount, dir_name_buffer, sizeof(dir_name_buffer) / 2 - 2);

    if ((name_len == 11) && (dir_name_buffer[0] != 0xff))
    {
        //8+3
        buf = dir_name_buffer;
        name_len = 8;
        add_len = 1;
    }
    else
    {
        if ((name_len == 8) && (dir_name_buffer[0] == 0xff))
        {
            //unciode
            buf = dir_name_buffer + 2;
            name_len = 6;
            add_len = 2;
        }
        else
        {
            return FALSE;
        }
    }

    i = 0;
    for (j = 0; j < name_len; j++)
    {
        if (rec_dir_name[j] != buf[i])
        {
            return FALSE;
        }
        i = i + add_len;
    }

    return TRUE;

#else
    return FALSE;
#endif
}

//获取文件总数和目录总数
//返回文件总数
uint16 music_rcp_first_get(void)
{
    //获取文件总数和目录总数
    sys_vm_read(g_sector_buf,
            VM_PLIST_INFO_START0, //即：vram_addr[0]
            sizeof(g_sector_buf));
    g_sector_no = 1;
    g_vmfile_total = ((list_head_info_t *) g_sector_buf)->file_total;
    libc_print("get file num",g_vmfile_total,2);
    g_vmdir_total = ((list_head_info_t *) g_sector_buf)->dir_total;
    
    g_vmdisk_flag = ((list_head_info_t *) g_sector_buf)->cur_disk;

    //其实，music_rcp_dir_first已经跳到第一个目录了。
    //这里仍然赋值0，将来会给g_cur_dir赋值
    g_last_dir_no = 0;
    g_last_get_index = 0;
    g_sector_no = 0;
    return g_vmfile_total;
}

bool music_rcp_dir_next(void)
{
    //uint8 dir_type = DIR_HEAD;
    //uint16 temp_num;

    //调用文件系统接口
    if (0 != vfs_dir(eh_vfs_mount, DIR_HEAD, NULL, MUSIC_BITMAP))
    {
        //eh_file_no++;
        //只有一个目录,解决录音文件播放的问题
        return TRUE;
    }
    else
    {
        //fsel_error = FSEL_ERR_FAIL;
        return FALSE;
    }
    //return FALSE;
}

bool music_rcp_dir_first(void)
{
    uint8 cur_dir_layer;
    //需要进入父目录
    bool back;

    //前面的代码已经确保，当前在根目录下，所以，这里赋值为0
    cur_dir_layer = 0;
    for (;;)
    {
        if (music_rcp_dir_next() == TRUE)
        {
            //找文件成功了
            return TRUE;
        }

        //找文件不成功
        //下面干的活都是为了定位到一个目录上

        back = TRUE;
        for (;;)
        {
            if (cur_dir_layer >= (DIRLAYER_MAX - 1))
            {
                //本目录已经够深了，不再往下找，退回到父目录去找
                break;
            }

            if (0 == vfs_dir(eh_vfs_mount, DIR_HEAD, NULL, EXT_NAME_ALL_DIR))
            {
                //本目录没有子目录

                //退回到父目录
                break;
            }

            //本目录有子目录

            if((cur_dir_layer == 0) && (_is_filter() == TRUE)
                && (vfs_dir(eh_vfs_mount, DIR_NEXT, NULL,
                EXT_NAME_ALL_DIR) == FALSE))
            {
                //退回到父目录
                break;
            }

            //进入子目录

            if (FALSE == vfs_cd(eh_vfs_mount, CD_SUB, NULL))
            {
                //进入子目录不成功，肯定是出错了！
                return FALSE;
            }
            //进入子目录
            cur_dir_layer++;

            back = FALSE;
            break;
        }

        if (back == FALSE)
        {
            continue;
        }

        //准备进入父目录

        for (;;)
        {
            //当前是根目录
            if (0 == cur_dir_layer)
            {
                return FALSE;
            }

            if (vfs_cd(eh_vfs_mount, CD_BACK, NULL) == FALSE)
            {
                //回不到父目录，肯定是出错了！
                return FALSE;
            }
            //回到父目录了
            cur_dir_layer--;

            //找下一个目录
            if (vfs_dir(eh_vfs_mount, DIR_NEXT, NULL, EXT_NAME_ALL_DIR) == 0)
            {
                //没有下一个目录了
                continue;
            }

            if((cur_dir_layer == 0) && (_is_filter() == TRUE)
                && (vfs_dir(eh_vfs_mount, DIR_NEXT, NULL,
                EXT_NAME_ALL_DIR) == FALSE))
            {
                //没有下一个目录了
                continue;
            }

            //进入下一个目录
            vfs_cd(eh_vfs_mount, CD_SUB, NULL);
            cur_dir_layer++;
            break;
        }
    }

    //不应该执行到这里
}

bool music_rcp_fs_init(uint8 disk_type, uint8 drv_mode)
{
    uint8 drv_type;
    uint8 *drv_name;
    //int32 vfs_mount;
    
    if (eh_vfs_mount != -1)
    {
        return TRUE;
    }
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
        return FALSE;
    }

    //安装存储驱动
    if (sys_drv_install(drv_type, (uint32)drv_mode, drv_name) != 0)
    {
        return FALSE;
    }

    //挂载FS
    eh_vfs_mount = sys_mount_fs(drv_type, disk_type, 0);

    if (eh_vfs_mount == -1)
    {
        sys_drv_uninstall(drv_type);
        return FALSE;
    }
    //默认进入根目录
    vfs_cd(eh_vfs_mount, CD_ROOT, 0);

    music_rcp_dir_first();
    music_rcp_first_get();

    return TRUE;
}

bool music_rcp_fs_exit(uint8 disk_type, int32 file_sys_id)
{
    int32 ret_val = 0;

    uint8 drv_type;

    if (file_sys_id != -1)
    {
        ret_val = sys_unmount_fs(file_sys_id);
        file_sys_id = -1;
        if (ret_val == 0)
        {
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
        }
    }
    if (0 > ret_val)
    {
        return FALSE;
    }

    return TRUE;
}

