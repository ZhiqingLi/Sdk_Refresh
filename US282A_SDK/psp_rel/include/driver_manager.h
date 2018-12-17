/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：设备驱动信息定义
 * 作者：liminxian
 ********************************************************************************/

#ifndef _DRIVER_H
#define _DRIVER_H

#include <typeext.h>
#include <attr.h>

#ifndef _ASSEMBLER_

/*5110的快慢速驱动都是一样，但两种驱动的位置可能会不一样*/
/*因为数据拷贝的需求，每份文件系统也有可能出现在不同的空间*/
/*所以三种存储驱动和两份文件系统都可能有两份drv文件*/
#define MODE_NORMAL 0
#define MODE_SPECIAL 1

typedef struct {
	unsigned char file_type;
	unsigned char drv_type;
	unsigned char Major_version;
	unsigned char minor_version;
	unsigned char magic[4];
	unsigned int text_offset;
	unsigned int text_length;
	unsigned int text_addr;
    unsigned int text1_offset;
    unsigned int text1_length;
    unsigned int text1_addr;
    unsigned int text2_offset;
    unsigned int text2_length;
    unsigned int text2_addr;
	unsigned int data_offset;
	unsigned int data_length;
	unsigned int data_addr;
	unsigned int bss_length;
	unsigned int bss_addr;
	unsigned int drv_init_entry;
	unsigned int drv_exit_entry;
	unsigned int drv_banka_file_offset;
	unsigned int drv_bankb_file_offset;
	unsigned int drv_bankc_file_offset;
	unsigned int drv_op_entry;
} drv_head_t;

typedef struct {
	uint32 file_startaddr;
    uint16 bank_a_offset;
	uint16 bank_b_offset;
	uint16 bank_c_offset;
    uint16 resever;
	uint32 exit_entry;
} drv_bank_info_t;

typedef struct {
    uint32 file_startaddr;
	uint16 bank_a_offset;
	uint16 bank_b_offset;
} sys_bank_info_t;

#define MAX_DRV_GROUP_TOTAL 15
#define SUPPORT_DRV_GROUP_TOTAL 14  //暂时只有14组而已,减少可以省drv_bank_info_t对应的数据空间
#define API_TOTAL_ENTRY (MAX_DRV_GROUP_TOTAL+1+1)//sys+libc   vfs单独成一个接口
typedef enum {
	DRV_GROUP_STG_BASE = 0,
	DRV_GROUP_STG_CARD,
	DRV_GROUP_STG_UHOST,
	DRV_GROUP_FAT,
	DRV_GROUP_EXFAT,
	DRV_GROUP_UD,
	DRV_GROUP_LCD,
	DRV_GROUP_FM,
	DRV_GROUP_KEY,//TOUCH&GSENSOR
	DRV_GROUP_CCD,//DRV_GROUP_I2C,
	DRV_GROUP_AUDIO_DEVICE,
	DRV_GROUP_TTS,
	DRV_GROUP_BT,
	DRV_RESERVER2,
	DRV_GROUP_SYS = 15,
} drv_type_t;

/*重新定义init 和 exit 的接口，有三个参数*/
/*不能通过CMD来调用*/
/*void*  (*vfs_init)(vfs_mount_t* p_vfs_mount, void* null, void* null, void* null);*/
/*void* (*vfs_exit)(vfs_mount_t* p_vfs_mount, void* null1, void* null2, void* null3);*/

typedef int (*drv_entry_i)(void *param1, void *param2, void *param3);



#endif

#endif /*_DRIVER_H */

