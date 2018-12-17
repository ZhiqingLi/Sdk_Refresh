/*******************************************************************************
 *                              US212A
 *                            Module: MainMenu
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     mainmenu_main.c
 * \brief    MainMenu的main函数模块
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include "ap_upgrade.h"

static const uint8 card_drv_name[] = "card.drv";
static const uint8 uhost_drv_name[] = "uhost.drv";
const uint8 fw_file_name[] =
{
    0xff,0xfe,
    'U', 0x00,
    'P', 0x00,
    'G', 0x00,
    'R', 0x00,
    'A', 0x00,
    'D', 0x00,
    'E', 0x00,
    '.', 0x00,
    'H', 0x00,
    'E', 0x00,
    'X', 0x00,
    0x00,0x00
};

const uint8 fwu_file_name[] =
{
    0xff,0xfe,
    'U', 0x00,
    'P', 0x00,
    'G', 0x00,
    'R', 0x00,
    'A', 0x00,
    'D', 0x00,
    'E', 0x00,
    '.', 0x00,
    'F', 0x00,
    'W', 0x00,
    'U', 0x00,
    0x00,0x00
};

const uint8 fw_ascfile_name[] = {"UPGRADE HEX"};
const uint8 fwu_ascfile_name[] = {"UPGRADE FWU"};

const uint8 fw_file_newname[] =
{
    0xff,0xfe,
    'U', 0x00,
    'P', 0x00,
    'G', 0x00,
    'R', 0x00,
    'A', 0x00,
    'D', 0x00,
    'E', 0x00,
    '~', 0x00,
    '.', 0x00,
    'H', 0x00,
    'E', 0x00,
    'X', 0x00,
    0x00,0x00
};



#if 0

//ap文件魔数
const uint8 build_package_label[] = "yqhx";

void load_decrypt_codec(void)
{
    uint32 file_len;
    uint8 *temp_buffer = (uint8 *) USE_TEMP_BUF_ADDR;
    drv_head_t *drv_head = (drv_head_t *) temp_buffer;

    //获取文件总大小
    vfs_file_get_size(_para.g_file_sys_id, &file_len, _para.file_handle, 0);

    file_len = (file_len - 10240);

    while (1)
    {
        vfs_file_seek(_para.g_file_sys_id, file_len, SEEK_SET, _para.file_handle);
        vfs_file_read(_para.g_file_sys_id, temp_buffer, 512, _para.file_handle);

        if ((libc_memcmp(drv_head->magic, build_package_label, 4) != 0))
        {
            file_len -= 512;
        }
        else
        {
            break;
        }
    }

    if (drv_head->text_length != 0)
    {
        vfs_file_seek(_para.g_file_sys_id, file_len + drv_head->text_offset, SEEK_SET, _para.file_handle);
        vfs_file_read(_para.g_file_sys_id, drv_head->text_addr, drv_head->text_length, _para.file_handle);
    }

    if (drv_head->data_length != 0)
    {
        vfs_file_seek(_para.g_file_sys_id, file_len + drv_head->data_offset, SEEK_SET, _para.file_handle);
        vfs_file_read(_para.g_file_sys_id, drv_head->data_addr, drv_head->data_length, _para.file_handle);
    }

    g_decrypt_ops.fw_decrypt_init = (*(uint32 *) (drv_head->drv_op_entry));

    g_decrypt_ops.fw_decrypt_run = (*((uint32 *) (drv_head->drv_op_entry) + 1));

    libc_memset(drv_head->bss_addr, 0, drv_head->bss_length);

    ((drv_entry_i)(drv_head->drv_init_entry + 1))(0, 0, 0); //init

    vfs_file_seek(_para.g_file_sys_id, 0, SEEK_SET, _para.file_handle);

    return;
}
#endif

bool is_fw_exist(uint8 drv_type, char *drv_name, uint8 disk_type)
{
    if (sys_drv_install(drv_type, 0, drv_name) == 0)
    {
        //装载文件系统
        _para.g_file_sys_id = sys_mount_fs(drv_type, disk_type, 0);
        if (_para.g_file_sys_id == -1)
        {
            sys_drv_uninstall(drv_type);
            return FALSE;
        }
        vfs_cd(_para.g_file_sys_id, CD_ROOT, 0);
        if ((vfs_file_dir_exist(_para.g_file_sys_id, fw_file_name, 1) > 0) || (vfs_file_dir_exist(_para.g_file_sys_id,
                fwu_file_name, 1) > 0) || (vfs_file_dir_exist(_para.g_file_sys_id, fw_ascfile_name, 1) > 0)
                || (vfs_file_dir_exist(_para.g_file_sys_id, fwu_ascfile_name, 1) > 0))
        {
            return TRUE;
        }
        else
        {
            sys_unmount_fs(_para.g_file_sys_id);
            sys_drv_uninstall(drv_type);
        }
    }
    return FALSE;
}

bool upgrade_fw_check(void)
{
    int ret_val;

    //装载物理介质,卡优先
    if ((!is_fw_exist(DRV_GROUP_STG_CARD, card_drv_name, DISK_H)) && (!is_fw_exist(DRV_GROUP_STG_UHOST, uhost_drv_name,
            DISK_U)))
    {
        return FALSE;
    }

    //解除nor写保护，保证可以正常进行固件升级
    //base_write_protect(0);

    _para.progress = INSTALL_DRV;

    _para.file_handle = vfs_file_open(_para.g_file_sys_id, fwu_file_name, R_NORMAL_SEEK); //工厂升级

    if (_para.file_handle == 0)
    {
        _para.file_handle = vfs_file_open(_para.g_file_sys_id, fwu_ascfile_name, R_NORMAL_SEEK);
        if (_para.file_handle == 0)
        {
            _para.upgrade_type = UPGRADE_TYPE_ENDUSER;
            goto next_try;
        }
    }
    _para.upgrade_type = UPGRADE_TYPE_FACTORY;
    goto upgrade_;

    next_try: _para.file_handle = vfs_file_open(_para.g_file_sys_id, fw_file_name, R_NORMAL_SEEK); //终端用户升级

    if (_para.file_handle == 0)
    {
        _para.file_handle = vfs_file_open(_para.g_file_sys_id, fw_ascfile_name, R_NORMAL_SEEK);
        if (_para.file_handle == 0)
        {
            return FALSE;
        }
    }

    vfs_file_dir_remove(_para.g_file_sys_id, fw_file_newname, FS_FILE);

    vfs_file_rename(_para.g_file_sys_id, fw_file_newname, _para.file_handle, 0);

    upgrade_:

    //开启按键音
    keytone_play_init();

    upgrade_start_key_tone();

    ret_val = sys_drv_uninstall(DRV_GROUP_AUDIO_DEVICE);
    if (ret_val == -1)
    {
        return FALSE;
    }

    return TRUE;
}


