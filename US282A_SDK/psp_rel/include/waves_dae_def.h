#ifndef __WAVES_DAE_DEF__
#define __WAVES_DAE_DEF__
#include <psp_includes.h>

//0x9fc3afe0-9fc3afff(32字节)地址存放PC工具发来的系数表 Infobuffer
#define WAVES_ASET_COEFF_ADDR                     (0x9fc3afe0)//(dsp地址:0x2d7f0)
//M4K和DSP协议交互结构体
typedef struct
{
    int DAE_change_flag;            //1表示更新了音效系数;!!!注意M4K置位，DSP取走参数后复位
    int parameter_address;          //更新音效系数存放在内存中的起始地址
    int parameter_length;           //更新的音效系数长度
    int reserve[5];                 //扩展用
}coeff_t;


/*****从0x9fc37000起始地址后的1616字节的数据存放的是WAVES音效参数*********************///DAEbuffer
#define WAVES_DAE_PARA_START_ADDR                 (0x9fc37000)//(dsp地址:0x2b800)
#define WAVES_DAE_PARA_END_ADDR                   (0x9fc37650)//(dsp地址:0x2bb28)
#define WAVES_DAE_PARA_LENGTH                     (WAVES_DAE_PARA_END_ADDR - WAVES_DAE_PARA_START_ADDR)//(1616字节) 

//waves音效input参数起始地址
#define WAVES_INPUT_PARA_START_ADDR               (0x9fc37008)//(mips地址:0x9fc37008) 

#define VM_DATA_LENGTH                            (512) 
#define VM_WAVES_AL_DATA1                         (0x00310000)
#define VM_WAVES_AL_DATA2                         (0x00320000) 
#define VM_WAVES_AL_DATA3                         (0x00330000) 
#define VM_WAVES_AL_DATA4                         (0x00340000) 
#define VM_WAVES_AL_DATA5                         (0x00350000) 
#define VM_WAVES_AL_DATA6                         (0x00360000) 
#define VM_WAVES_AL_DATA7                         (0x00370000) 
#define VM_WAVES_AL_DATA8                         (0x00380000) 
#define VM_WAVES_AL_DATA9                         (0x00390000) 
#define VM_WAVES_AL_DATA10                        (0x003a0000) 
#define VM_WAVES_AL_DATA11                        (0x003b0000) 
#define VM_WAVES_AL_DATA12                        (0x003c0000) 
#define VM_WAVES_AL_DATA13                        (0x003d0000) 
#define VM_WAVES_AL_DATA14                        (0x003e0000) 

typedef struct
{
    uint8  data[VM_DATA_LENGTH];
}waves_dae_para_t;


//PC工具传递的音效系数表结构体
typedef struct
{
    uint32  update_flag;            //1表示PC工具更新了音效系数; 0表示没有更新
    uint32  length;                 //更新的音效系数长度
    uint32  memory_addr;            //更新音效系数存放在内存中的起始地址      
}coeff_property_t;

typedef enum
{
    /*!设置系数表*/
    SET_WAVES_COEFF_PROPERTY,
    /*!设置音效配置参数*/
    SET_WAVES_EFFECT_PARAM,
    /*!音效参数从内存写到VRAM*/
    WAVES_DAE_WRITE_VRAM,
    /*!音效参数从内存写到VRAM*/
    WAVES_DAE_READ_VRAM,
    /*!从bin文件中加载音效参数*/
    SET_WAVES_PARAM_FROM_SDFS,
    /*!设置input音效参数*/
    SET_WAVES_INPUT_PARAM 
} set_waves_effect_type_e;

//0x9fc3b000-0x9fc3b800(2k空间用来作为工具和小机数据交互的缓冲buffer，现在只用了256个字节，剩余的空间可以用其它用途)Tmpbuffer
//#define WAVES_ASET_SHARE_MEM_START_ADDR           (0x9fc3b000)//(dsp地址:0x2d800)
//#define WAVES_ASET_SHARE_MEM_END_ADDR             (0x9fc3b800)//(dsp地址:0x2dc00)
/******************************************************************/

//waves音效最多支持几个bin文件进行动态切换
#define WAVES_DAE_PARA_BIN_MAX_NUM                  3
    
#endif
