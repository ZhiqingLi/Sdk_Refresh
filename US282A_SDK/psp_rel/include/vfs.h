/********************************************************************************
 *                              USDK(GL5110)
 *                            Module: KERNEL
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>      <time>           <version >             <desc>
 *      wuyueqian     2011-9-10 09:00     1.0             build this file
 ********************************************************************************/
/*!
 * \file     vfs.h
 * \brief    虚拟文件系统接口
 * \author   wuyueqian
 * \version  1.0
 * \date  2011/9/10
 *******************************************************************************/

#ifndef __VFS_DEFINE_H__
#define __VFS_DEFINE_H__

#include <storage_device.h>
#include <attr.h>

/*vfs manager*/
#define SUPPORT_STG_TYPE_MAX 3
#define STG_BASE_NAND_NOR 0
#define STG_CARD 1
#define STG_UHOST 2

#define SUPPORT_FS_TYPE_MAX 3
#define FAT32 0
#define FAT16 1
#define EXFAT 2

#define DISK_SD 0 /*系统区*/
#define DISK_SD_SIZE 0xa00000 /*系统区10M*/
#define DISK_SD_BAK 1  /*系统备份区*/
#define DISK_SD_BAK_SIZE 0xa00000  /*系统备份区 10M*/
#define DISK_VM 2  /*VM区*/
#define DISK_VM_SIZE 0x100000  /*VM区 1M*/
#define DISK_HID 3 /*隐藏盘*/
#define DISK_HID_SIZE 0x100000/*隐藏盘 1M*/
#define DISK_AUTORUN 4 /*autorun区*/
#define DISK_AUTORUN_SIZE 0x100000 /*autorun区1M*/
#define DISK_ENCRYPT 5
#define DISK_ENCRYPT_SIZE 0
#define DISK_UNKNOWN 6
#define DISK_UNKNOWN_SIZE 0
#define DISK_VM_EXT 6
#define DISK_VM_EXT_SIZE (64*1024)
#define DISK_C 7 /*base存储用户区*/
/*C盘中有三个分区，U盘应用向PC报A&B，或者A&C盘*/
#define DISK_BASE_MAX 8
//因为在有些情况下，它跟mengine_enter_mode_e类型:(如ENTER_MUSIC_START)组成一个字节
//要注意：他们组成一个字节后，还能否分开的问题
#define DISK_H 0x10  /*card盘*/
#define DISK_U 0x20 /*uhost盘*/
#define DISK_S 0x40 /*stub盘*/

typedef struct {
 unsigned char drv_type;
 unsigned char fs_type;
 unsigned char disk;
 //    unsigned char partition;
 unsigned int start_offset;
 void *fat_private_data;
} vfs_mount_t;

typedef void* (*fs_ops_i)(vfs_mount_t* p_vfs_mount, void *param1, void *param2,
  void *param3);

struct file_system_operations {
 fs_ops_i vfs_cd;
 fs_ops_i vfs_dir;
 fs_ops_i vfs_getname;
 fs_ops_i vfs_filedir_offset;
 fs_ops_i vfs_makedir;
 fs_ops_i vfs_fopen;
 fs_ops_i vfs_fcreate;
 fs_ops_i vfs_fclose;
 fs_ops_i vfs_filedir_remove;
 fs_ops_i vfs_getsize;
 fs_ops_i vfs_gettime;
 fs_ops_i vfs_fseek;
 fs_ops_i vfs_ftell;
 fs_ops_i vfs_fread;
 fs_ops_i vfs_fwrite;
 fs_ops_i vfs_getspace;
 fs_ops_i vfs_geterrinfo;
 fs_ops_i vfs_filedir_exist;
 fs_ops_i vfs_filecut;
 fs_ops_i vfs_fattralter;
 fs_ops_i vfs_frename;
 fs_ops_i vfs_createvolume;
 fs_ops_i vfs_set_time;
 fs_ops_i vfs_set_private_data;
 fs_ops_i vfs_clear_private_data;
 fs_ops_i vfs_file_move;
 fs_ops_i vfs_dir_current_entry_file;
 fs_ops_i vfs_jump_to_direntry;
 fs_ops_i vfs_cut_file_tail;
 fs_ops_i vfs_file_divided;
 fs_ops_i vfs_file_insert;
};

typedef struct {
 /*
  1 每个文件系统都需在内部定义一个struct file_system_operations fat_ops操作集
  2 实现操作集中的所有函数(参数类型不能变，函数内部强制转换)，并把函数地址填入该结构体中
  3 需要定义一个总入口函数，能够根据cmd 容易进行转化处理
  4 驱动加载时会把这个总入口注册给下面的fs_ops.
  */
 /*filesys unique entry*/
 //fs_ops_i fs_ops;
 unsigned int fs_ops_table_addr;
} fs_t;

extern void* blk_layer_ops(void *param1, void *param2, void *param3,
  device_cmd_e cmd_type, vfs_mount_t* p_vfs_mount);

typedef void* (*device_ops_i)(void *param1, void *param2, void *param3);

typedef struct {
 /*
  1 每个存储驱动都需在内部定义一个struct block_device_operations blk_ops操作集
  2 实现操作集中的所有函数(参数类型不能变，函数内部强制转换)，并把函数地址填入该结构体中
  3 需要定义一个总入口函数，能够根据cmd 容易进行转化处理
  4 驱动加载时会把这个总入口注册给下面的device_ops.
  */
 /*device drvier unique entry*/
 //device_ops_i device_ops;
 unsigned int device_ops_table_addr;
} block_device_t;

/*以上是针对vfs的块设备，下面主要是针对vfs的字符设备
 因此这里会包括所有的15种驱动的入口，包括字符设备或者块设备*/
typedef struct {
 /*
  1 每个存储驱动都需在内部定义一个struct block_device_operations blk_ops操作集
  2 实现操作集中的所有函数(参数类型不能变，函数内部强制转换)，并把函数地址填入该结构体中
  3 需要定义一个总入口函数，能够根据cmd 容易进行转化处理
  4 驱动加载时会把这个总入口注册给下面的device_ops.
  */
 /*device drvier unique entry*/
 //device_ops_i device_ops;
 unsigned int device_ops_table_addr;
} device_t;

#endif

