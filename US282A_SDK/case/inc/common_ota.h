/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：COMMON OTA服务相关的宏定义，枚举类型，结构体定义，外部接口声明等。
 * 作者：lizijian
 ********************************************************************************/
#ifndef _COMMON_OTA_H
#define _COMMON_OTA_H

#include "psp_includes.h"
#include "enhanced.h"
#include "common_func.h"
#include "vm_def.h"
#include "sys_info.h"



#define RECEIVE_DARA_FAIL  (0)
#define RECEIVE_DARA_SUC   (1)
#define RECEIVE_DARA_COMLETE  (2)

#define BYTE_PER_SECTION   (512)

#define RCP_BUFF_ADDR      (g_rcp_long_buffer)


//查询命令回复的结构体
typedef struct
{
    Fw_Ver_t cru_updata;    //当前下载的版本号
    uint16 cru_pack_count;// 当前收到的是第几个包
    uint8  part_id;       //分区ID
    uint8  rec_data_state;   //数据是否正常接受 0异常；1正常； 2已经收到发送完成的命令(可以重启升级)
                               //    3 分区校验不通过，请求重启。
    uint8 tws_connect_sta;//用于提示用户当前的TWS连接状态，
    uint8 reserved;
} enquiry_message_t;

typedef struct
{
    uint8 connect_state;
    uint8 reserved[15];
}connect_reply_t;


typedef struct
{
    uint8  connect_state;
    uint8  reservd;
    uint16 cru_pack_count;// 当前收到的是第几个包
    uint32 random_upg_key; //升级记录的随机值 
    uint8  part_id;       //分区ID
    uint8  rec_data_state;   //数据是否正常接受 0异常；1正常；2已经收到发送完成的命令(可以重启升级)
    uint8  reservd2[6];
}ota_bkinfo_t;


typedef struct
{
    Fw_Ver_t cru_updata;    //当前下载的版本号 8bytes
    uint32 random_upg_val; //保留
    uint16 complete_send_num; //已经发了多少个分区
    uint8  part_id;       //需要升级的分区ID
    uint8  reserved[1];      //保留字节
    ota_write_info_t wirte_info;
} connect_message_t;//总共516byte

typedef struct
{
    uint32 part_id;            //分区ID
    uint32 part_size;        //分区大小，字节为单位
    uint32 part_offset;        //分区偏移，字节为单位
    uint16 check_sum;        //分区校验和，16位数据的累加和
    uint16 part_max_size;   //分区最大大小
}part_item_t;

typedef struct
{
    uint8 magic[4];         //'OTA'
    uint8 sdk_version[4];    //SDK版本号
    uint8 case_version[4];  //case版本号
    uint8 date[4];            //固件生成日期
    uint8 part_total;        //分区总大小，最多16个分区
    uint8 reserved0[7];
    uint8 module_num[8];
    part_item_t part_item[16]; //最多16个系统分区
    uint8 reserved[224];
}part_head_t;
   
typedef struct
{   
   connect_reply_t g_connect_repy; 
   ota_bkinfo_t g_reply_bkinfo;
   enquiry_message_t g_cur_rec_state;//小机收数据的状态
   uint16 g_backup_part_addr; //用来记录当前分区的NOR的起始位置
   uint16 g_cru_pack_count;//记录当前包数也即写nor的地址偏移
   uint8 g_send_erro_count;//用来计数误发次数
}OTA_malloc_param_t;


extern restore_breakpoint_t g_ota_restore_data;//OTA记录在VRAM的数据
extern OTA_malloc_param_t *g_OTA_var;





extern app_result_e ota_receive_connect_data(uint32 data1, uint32 data2, void *data, uint16 data_len) __FAR__;
extern app_result_e ota_receive_data(uint32 data1, uint32 data2, void *data, uint16 data_len) __FAR__;
extern app_result_e ota_receive_vram_data(uint32 data1, uint32 data2, void *data, uint16 data_len) __FAR__;
extern app_result_e ota_restart_to_clean_data(uint32 data1, uint32 data2, void *data, uint16 data_len) __FAR__;
extern app_result_e ota_restart_to_upgrade(uint32 data1, uint32 data2,  void *data, uint16 data_len) __FAR__;
extern app_result_e ota_send_complete(uint32 data1, uint32 data2,  void *data, uint16 data_len) __FAR__;
extern app_result_e ota_get_ver_num(uint32 data1, uint32 data2) __FAR__;
extern app_result_e ota_enquiry_message(uint32 data1, uint32 data2) __FAR__;
extern app_result_e ota_enquiry_connect(uint32 data1, uint32 data2) __FAR__;
extern uint32 fw_download_new(void);
extern uint32 get_ver_message(Fw_Ver_Module_t *ver_buf);
extern uint32 message_copy(void *data,uint8 message_type);
extern void reboot_system(void);
extern UINT16 CheckSum16(UINT16 *buf, UINT32 len);

extern UINT32 CheckSum32(UINT32 *buf, UINT32 len);




#endif

