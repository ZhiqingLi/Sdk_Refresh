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
 * \file     vfs_interface.h
 * \brief    虚拟文件系统接口
 * \author   wuyueqian
 * \version  1.0
 * \date  2011/9/10
 *******************************************************************************/

#ifndef __SYS_OTA_INTERFACE_H__
#define __SYS_OTA_INTERFACE_H__

#define FORCE_READ_PART_MODE    (0x80)

#define OTA_FW_ZONE_CONTINUE        (0)

#define OTA_FW_ZONE_NOT_CONTINUE    (1)  

typedef enum
{
    //连接状态，开机默认值
    OTA_CONNECT_INIT = 0,
    //握手数据错误，不能通过检验
    OTA_CONNECT_DATA_ERROR,
    //连接成功
    OTA_CONNECT_SUCCESS,
    //固件太大不能升级
    OTA_FW_DATA_TOO_BIG,
    //2次升级固件不一致
    OTA_FW_DIFFERENT,
    //断电续传
    OTA_BK_CONTINUE,
}ota_connect_msg_e;


typedef enum
{
    //该分区为固有分区，也就是正常数据分区
    OTA_PART_AGE_NORMAL = 0,
    //该分区为固有分区，但分区数据可能在备份的时候被别的分区破坏
    OTA_PART_AGE_NORMAL_ERROR,
    //该分区为某个固件分区的备份数据区
    OTA_PART_AGE_BACKUP,
    //该分区为新增加的数据区
    OTA_PART_AGE_ADD
}ota_part_age_e;

typedef enum
{
    OTA_DOWNLOAD_STATE_NULL = 0,
    OTA_DOWNLOAD_STATE_START,
    OTA_DOWNLOAD_STATE_END,
    OTA_DOWNLOAD_STATE_CLEAR
}ota_download_state_e;

typedef enum
{
    OTA_UPDATE_STATE_NULL = 0,
    OTA_UPDATE_STATE_UNCLEAR_BKINFO = 1,
    OTA_UPDATE_STATE_END
}ota_update_state_e;


typedef struct
{
	uint8 part_id;  
	uint8 part_age;   //分区新旧标志，数值越大，分区数据越新
	uint16 part_offset; //分区位置，扇区为单位
	uint16 part_size; //分区大小，扇区为单位
	uint16 part_max_size; //分区大小，扇区为单位
	uint16 part_checksum; //分区校验和	
}fw_part_item_t;

typedef struct
{
    uint8 part_total; //总分区个数
    uint8 part_type_total; //分区类型个数
    fw_part_item_t part_item[0];
}fw_part_table_t;

typedef struct
{
    uint8 write_mode; //0:备份写 1:覆盖写
    uint8 part_id;    //分区ID
    uint16 part_size; //块实际大小
    uint16 part_max_size; //块最大大小
    uint16 part_offset;
    uint16 part_checksum;
}ota_write_item_t;

typedef struct
{
    uint8 part_total;
    //uint8 reserved;
    uint8 fw_mode;    //0 分区之间无预留 1 分区之间有预留
    ota_write_item_t part_info[0]; //变长数组
}ota_write_info_t;


typedef struct
{
    uint8 part_total;
    uint8 part_id[0]; //变长数组
}ota_part_info_t;

typedef struct
{
	unsigned short PID; //设备pid
	unsigned short VID; //设备vid
	char Version[4]; //版本号xx.x, no use now
}Fw_Ver_t;

typedef struct
{
    Fw_Ver_t fw_ver;
    char module[8]; 
}Fw_Ver_Module_t;


//写VM结构体;
typedef struct
{
	uint16 magic;
	uint8  dowmload_statue; //下载的状态    0  :  无下载1:下载中   2:下载完成
	uint8  updata_statue; //更新的状态       0  : 未更新	1:已完成复制，但没清除 
	uint16 pack_count;	//每N个block记录一次下载的包数
	uint16 complete_send_num; //已经发了多少个分区
    uint8  part_id ;//当前下载的分区
    uint8  ud_att;//本次升级的属性 0:不含系统分区1:含系统分区  如果只更新数据区，重启后不擦VRAM
    uint8  reboot_flg;//重启升级确认0:brec不需要OTA 出来，1:需要OTA处理
    uint8  reserved[1];
	Fw_Ver_t cru_updata;	//当前下载的版本信息

} restore_breakpoint_t;


typedef struct
{
    uint32 part_id;                 //分区ID
    uint8 *data_buffer;            //数据buffer
    uint32 data_len;                //数据长度
    uint16 part_base_offset;        //分区偏移位置 
    uint16 part_write_offset;       //分区写位置偏移
}ota_write_param_t;




#endif
