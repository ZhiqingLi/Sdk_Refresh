//依赖目录列表，节目单的文件操作
//执行时会占用PLIST_INFO_ADDR开始的0x1000空间(因为目录列表读到这个内存里)
//执行时会占用PLIST_INDEX_ADDR开始的0x1000空间(因为节目单读到这个内存里)
#include "eh_fsel_entry.h"

static const uint8 card_drv_name[] = "card.drv";
static const uint8 uhost_drv_name[] = "uhost.drv";

//初始化公共参数
void fsel_init_sysdata(void)
{
    //当前目录的层次，0表示根目录，1表示根目录的子目录，以此类推
    eh_dir_layer = 0;
    //根目录的子目录指针，1,2，。。。
    //eh_root_dir = 0;
    //当前文件在目录或列表中的序号
    eh_file_no = 0;
    //当前目录或列表的总文件个数
    eh_file_total = 0;

    //当前磁盘文件序号
    eh_file_no_all = 0;
    //当前磁盘文件总数
    eh_file_total_all = 0;

    //表头指针
    plist_head = (list_head_info_t *) plist_start_addr;
    //列表项指针
    plist_info = (list_dir_info_t *) (plist_start_addr + sizeof(list_head_info_t));

    eh_dir_no = 1;

    //默认进入根目录
    vfs_cd(eh_vfs_mount, CD_ROOT, 0);
}

//获取出错信息
fsel_error_e fsel_get_error(void)
{
    return vfs_get_err_info(eh_vfs_mount);
}



void fsel_set_fileno(uint16 set_num)
{
    eh_file_no_all = set_num;
}

//初始化
int32 fsel_init(fsel_param_t *init_param, uint8 drv_mode)
{
    //介质驱动名称
    uint8 *drv_name;
    //驱动类型
    uint8 drv_type;
    //出错信息
    fsel_error = FSEL_NO_ERR;
    //参数为空则直接返回
    if (init_param == NULL)
    {
        return -1;
    }
    //保存初始化参数,文件选择器退出时使用
    libc_memcpy(&fsel_init_val, init_param, sizeof(fsel_param_t));

    //循环模式
    fsel_init_val.fsel_mode &= 0x0f;
    //shuffle是否打开
    if ((init_param->fsel_mode & 0xf0) != 0)
    {
        eh_shuffle_sw = TRUE;
    }
    else
    {
        eh_shuffle_sw = FALSE;
    }

    if (init_param->disk == DISK_H)
    {
        drv_type = DRV_GROUP_STG_CARD;
        drv_name = (uint8 *) &card_drv_name;
    }
    else if (init_param->disk == DISK_U)
    {
        drv_type = DRV_GROUP_STG_UHOST;
        drv_name = (uint8 *) uhost_drv_name;
    }
    else
    {
        fsel_error = FSEL_ERR_DISK;
        return -1;
    }

    //安装存储驱动
    if (sys_drv_install(drv_type, (uint32)drv_mode, drv_name) != 0)
    {
        fsel_error = FSEL_ERR_STG;
        return -1;
    }

    //挂载FS
    eh_vfs_mount = sys_mount_fs(drv_type, fsel_init_val.disk, 0);

    if (eh_vfs_mount == -1)
    {
        sys_drv_uninstall(drv_type);
        fsel_error = FSEL_ERR_FS;
        return -1;
    }

    /*if (vfs_dir(eh_vfs_mount, DIR_NEXT, NULL, EXT_NAME_ALL) == 0)
    {
        eh_vfs_mount = -1;
    }
    else*/
    //{
    //初始化参数
    fsel_init_sysdata();
    //}

    //文件选择模式为目录模式
    eh_fsel_type = 0;

    return eh_vfs_mount;
}

//退出
bool fsel_exit(void)
{
    int32 ret_val;
    //驱动类型
    uint8 drv_type;

    fsel_error = FSEL_NO_ERR;
    //文件系统挂载成功
    if (eh_vfs_mount != -1)
    {
        ret_val = sys_unmount_fs(eh_vfs_mount);
        eh_vfs_mount = -1;
    }
    else
    {
        return TRUE;
    }
    //文件系统卸载成功
    if (ret_val < 0)
    {
        fsel_error = FSEL_ERR_FS;
        return FALSE;
    }

    if (fsel_init_val.disk == DISK_H)
    {
        drv_type = DRV_GROUP_STG_CARD;
    }
    else if (fsel_init_val.disk == DISK_U)
    {
        drv_type = DRV_GROUP_STG_UHOST;
    }
    else
    {
        fsel_error = FSEL_ERR_DISK;
        return -1;
    }
    //卸载介质驱动
    ret_val = sys_drv_uninstall(drv_type);

    if (ret_val < 0)
    {
        fsel_error = FSEL_ERR_STG;
        return FALSE;
    }

    return TRUE;
}

