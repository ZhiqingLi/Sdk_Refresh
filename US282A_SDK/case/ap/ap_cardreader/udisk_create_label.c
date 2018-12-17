/*******************************************************************************
 *                              US212A
 *                            Module: Config
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     config_main_sub.c
 * \brief    开关机的处理
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include  "ap_udisk.h"

const uint8 card_drv_name[] = "card.drv";

/******************************************************************************/
/*!
 * \par  Description:
 *  装载物理介质和文件系统
 * \param[in]    disk_type 磁盘类型
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
int32 _label_fs_init(uint8 disk_type)
{
    uint8 drv_type;
    uint8 *drv_name;
    int32 file_sys_id;

    if (disk_type == DISK_H)
    {
        drv_type = DRV_GROUP_STG_CARD;
        drv_name = (uint8 *) card_drv_name;
    }
    else
    {
        return -1;
    }

    //装载物理介质
    if (sys_drv_install(drv_type, 0, drv_name) != 0)
    {
        return -2;
    }

    //装载文件系统
    file_sys_id = sys_mount_fs(drv_type, disk_type, 0);

    if (file_sys_id == -1)
    {
        //卸载驱动程序
        sys_drv_uninstall(drv_type);
        return -3;
    }

    return file_sys_id;
}
/******************************************************************************/
/*!
 * \par  Description:
 *  卸载物理介质和文件系统
 * \param[in]    disk_type 磁盘类型
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
bool _label_fs_deinit(uint8 disk_type, int32 file_sys_id)
{
    int32 ret_val;
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

/******************************************************************************/
/*!
 * \par  Description:
 *    转换文件名，将长文件名转化为unicode字符
 * \param[in]    src_name 源文件名缓冲区
 * \param[in]   dest_name 目标文件名缓冲区
 * \return       none
 * \ingroup      common
 * \note
 *******************************************************************************/
void _convert_string(const uint8 * src_name, uint8 * dest_name)
{
    *dest_name = 0xff;
    *(dest_name + 1) = 0xfe;
    dest_name += 2;

    while (*src_name != '\0')
    {
        *dest_name = *src_name;
        *(dest_name + 1) = 0x00;
        dest_name += 2;
        src_name++;
    }

    *dest_name = 0x00;
    *(dest_name + 1) = 0x00;

    return;
}

/******************************************************************************/
/*!
 * \par  Description:_config_disk_label
 * \volume set
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
bool _config_disk_label(void)
{
    bool ret_val = FALSE;

    bool label_flag = 0;
    
    int32 file_sys_id;

    uint8 volume_label_src[12] = {0};

    uint8 volume_label_dst[28];

    uint8 i;

    if (sys_detect_disk(DRV_GROUP_STG_CARD) == -1)
    {
        return FALSE;
    }

    libc_memset(volume_label_src, 0, sizeof(volume_label_src));

    libc_memset(volume_label_dst, 0, sizeof(volume_label_dst));

    //装载主盘
    file_sys_id = _label_fs_init(DISK_H);
    if (file_sys_id < 0)
    {
        return FALSE;
    }

    //判断卷标是否存在
    //if(0 == vfs_create_volume(file_sys_id, 0))
    //{
    //量产升级后，卷标不存在,写入配置项卷标
    //从配置项读取卷标名字,最多支持11个英文字符
    com_get_config_struct(INF_UDISK_LABEL, volume_label_src, 11);
    for(i = 0;i<11;i++)
    {
        //不为全0；全F；全f；空
        if(((volume_label_src[i] != '0')&&(volume_label_src[i] != 'F')&&(volume_label_src[i] != 'f')\
            &&(volume_label_src[i] != 0))||(volume_label_src[i] != volume_label_src[0]))
        {
            label_flag = 1;
            break;
        }
    }
    if(label_flag == 1)
    {
        //短名转换成长名形式
        //com_ansi_to_unicode(volume_label);
        _convert_string(volume_label_src, volume_label_dst);

        //统一由长名创建卷标名
        ret_val = vfs_create_volume(file_sys_id, volume_label_dst);
        //}
    }
    _label_fs_deinit(DISK_H, file_sys_id);

    return ret_val;
}

