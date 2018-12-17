/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：SD区信息定义
 * 作者：liminxian
 ********************************************************************************/

#ifndef _KERNEL_SD_H
#define _KERNEL_SD_H

#include <typeext.h>
#include <ucos/init.h>

//typedef unsigned int uint32;

typedef struct 
{
    uint32 file_offset;
    uint32 sram_addr;
    uint32 len;
}sd_byte_param_t;




typedef struct 
{
    uint32 file_offset;
    uint32 sram_addr;
    uint32 sec_num;
}sd_sec_param_t;

#define NAND_BASE_PLATFORM 0
#define CARD_BASE_PLATFORM 1
#define NOR_BASE_PLATFORM 2

typedef struct
{  
    int (*sd_sec_read)(uint32 file_offset, uint32 sram_addr, uint32 sec_num);
    int (*sd_sec_write)(uint32 file_offset, uint32 sram_addr, uint32 sec_num);
    int (*vm_sec_read)(uint32 file_offset, uint32 sram_addr, uint32 sec_num);
    int (*vm_sec_write)(uint32 file_offset, uint32 sram_addr, uint32 sec_num);
    int8 base_type;
    int8 reserse[3];
}base_op_t;




typedef struct{
unsigned int SD_cap;
unsigned int VM_cap;
unsigned int HID_cap;
unsigned int AUTORUN_cap;
unsigned int ENCRYPT_cap;
unsigned int UNKNOWN_cap;
}sysdef_info_t;



 #endif

