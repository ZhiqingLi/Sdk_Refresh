/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：ap文件结构定义
 * 作者：liminxian
 ********************************************************************************/
#ifndef __AP_BANK_DEFINE_H__
#define __AP_BANK_DEFINE_H__

#include <typeext.h>
#include <attr.h>

#define FPGA_BUG


typedef struct
{
    unsigned int    bank_offset; 
    unsigned int    bank_addr;     
    unsigned int    bank_length;    
}ap_bank_head_t;

typedef struct
{
    unsigned int    bank_offset;          
    unsigned int    bank_length;  
    unsigned int    bank_addr;  
}dsp_bank_head_t;

typedef struct
{
    unsigned int    ap_bank1_offset;
    unsigned int    ap_bank2_offset;
    unsigned int    ap_enhanced1_offset;
    unsigned int    ap_enhanced2_offset;
}ap_bank_offset_t;

typedef struct{    
    unsigned int    text_offset;     
    unsigned int    text_length;         
    unsigned int    text_addr;    
}text_code_index_t;

typedef struct
{
    unsigned short  prio;
    unsigned short  stk_len;
    unsigned int    stk_pos;    
}task_attr_t;
typedef struct
{
    unsigned char    file_type;          //'P'
    unsigned char    ap_type;            //0:ap_system, 1:ap_user    
    unsigned char    major_version;      //3, for ACTOS 3.0
    unsigned char    minor_version;      //0
    unsigned char    magic[4];            //'W', 'G', 0x19, 0x97
    unsigned int    text_offset;		
    unsigned int    text_length;
    unsigned int    text_addr;          //
    unsigned int    text1_offset;
    unsigned int    text1_length;
    unsigned int    text1_addr;
    unsigned int    text2_offset;
    unsigned int    text2_length;
    unsigned int    text2_addr;
    unsigned int    data_offset;		
    unsigned int    data_length;
    unsigned int    data_addr;          //有初始化的全局变量
    unsigned int    bss_length;
    unsigned int    bss_addr;
    unsigned int    entry;
    task_attr_t     task_attr;
    unsigned int    reserved;
    unsigned short  bank_index_offset[8];    
//    ap_bank_offset_t   ap_bank_offset;
}ap_head_t;


typedef struct
{
    unsigned int    basal_offset;
    unsigned int    codec_offset;
}codec_bank_offset_t;

typedef struct
{
unsigned char file_type;
unsigned char media_type;
unsigned char major_version;
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
unsigned int module_api_entry;
}codec_head_t;

//dspcodec driver head info
typedef struct
{
    UINT8   magic[4];
    UINT8   file_type;
    UINT8   media_type;
    UINT8   major_version;
    UINT8   minor_version;

    UINT32  text0_offset;
    UINT32  text0_length;
    UINT32  text0_addr;
    UINT32  data0_offset;
    UINT32  data0_length;
    UINT32  data0_addr;

    UINT32  text1_offset;
    UINT32  text1_length;
    UINT32  text1_addr;
    UINT32  data1_offset;
    UINT32  data1_length;
    UINT32  data1_addr;

    UINT32  text2_offset;
    UINT32  text2_length;
    UINT32  text2_addr;

    UINT32  bss0_length;
    UINT32  bss0_addr;

    //UINT32  module_api_entry;

    //codec_bank_head_t bank_group_a[MAX_BANK_NUM_IN_CODEC_GROUP];
    //codec_bank_head_t bank_group_b[MAX_BANK_NUM_IN_CODEC_GROUP];
    //codec_bank_head_t bank_group_c[MAX_BANK_NUM_IN_CODEC_GROUP];
    //codec_bank_head_t bank_group_d[MAX_BANK_NUM_IN_CODEC_GROUP];
}dspcodec_head_t;      //96byte



typedef struct
{
    uint32 basal_entry;
    uint32 codec_entry;
}mid_entry_t;

/*
typedef struct
{
    uint32 ap_file_start;
    uint32 basal_file_start;
    uint32 codec_file_start;
}app_file_start_t;
*/
typedef uint32 app_file_start_t;
typedef uint32 dsp_file_start_t;

#define AP_FRONT_AP_FILE    0
#define AP_FRONT_BASAL_FILE 1
#define AP_FRONT_CODEC_FILE 2
#define AP_BACK_AP_FILE     3
#define AP_BACK_BASAL_FILE  4
#define AP_BACK_CODEC_FILE  5
#define AP_MANAGER_FILE     6
#define AP_BTENGINE_FILE    7  //PSP层使用，固化代码已使用，所以保留
#define AP_BTSTACK_FILE     7  //CASE层使用
#define AP_RESERVER_FILE    8 


#endif


