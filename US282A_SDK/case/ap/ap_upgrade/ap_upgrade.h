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
 * \file     ap_udevice.h
 * \brief    MainMenu的main函数模块
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#ifndef _AP_UPGRADE_H
#define _AP_UPGRADE_H

#include  "psp_includes.h"
#include  "case_include.h"
#include "ud_extern.h"
#include "led_driver.h"
#include "sys_info.h"
#include <uhost.h>

//play界面显示元素标识

#define up_usbflag           0x01   //usb
#define up_volflag           0x02   //vol
//-----------------------------清除刷新标志

#define clr_usbflag        0xfe   //清除磁盘拔出
#define clr_volflag        0xfd  //清除出错提示
#define BUFFER_ADDR    0x30000
#define BREC_START_ADDR   0x04

#define MIN_ONCE_READ_LEN  (18 * 1024)

#define DECRYPT_USE_BUF_ADDR 0x2b000 //(DECRYPT_INIT_BUF_ADDR + DECRYPT_INIT_BUF_SIZE) //18k
#define DECRYPT_USE_BUF_SIZE 0x4800 //解密用buffer 18k
#define DECRYPT_GL_BUF_ADDR  (DECRYPT_USE_BUF_ADDR + DECRYPT_USE_BUF_SIZE) //300
#define DECRYPT_GL_BUF_SIZE  300

#define DECRYPT_INIT_BUF_ADDR 0x30000 //(SRAM_BASE_ADDR + 0x0000) //20k
#define DECRYPT_INIT_BUF_SIZE 0x5000 //初始化解密buffer 20k
#define BT_VRAM_BUFF             (DECRYPT_INIT_BUF_ADDR+DECRYPT_INIT_BUF_SIZE)

#define BREC_BUF_ADDR         (BT_VRAM_BUFF + 0x200)

#define UPGRADE_KEYTONE_BUFFER (0x37000)
#define UPGRADE_KEYTONE_LEN    (0x1000)

#define DECRYPT_INIT_BUF_LEN  19888
#define DECRYPT_GL_BUF_LEN  300
#define DECRYPT_USR_BUF_LEN 18432

typedef uint32 (*func_def0)(void);
typedef void (*func_def1)(uint32 a);
//#undef sys_local_irq_save
//#undef sys_local_irq_restore
//#undef sys_vm_write
//#define sys_local_irq_save()               ((func_def0)0xbfc05f98)()
//#define sys_local_irq_restore(a)             ((func_def1)0xbfc05fa4)(a)
//#define sys_vm_write(a,b)           		(int)sys_op_entry((void*)(a), (void*)(b), (void*)0, VM_WRITE)


typedef struct
{
    uint32 file_offset;
    uint32 sec_num;
    uint32 sram_addr;
} ud_sector_param_t;

//目录项结构
typedef struct
{
    char filename[11];
    char reserve1[5];
    unsigned long offset;
    unsigned long length;
    char reserve2[4];
    unsigned long checksum;
}__attribute__ ((packed)) DIR_t; //32字节

typedef struct
{
    LFIHead_t lfi_head;
    DIR_t diritem[240];
}__attribute__ ((packed)) LFIHead_dir_t; //8k bytes

typedef struct
{
    HANDLE file_handle;
    int32 g_file_sys_id;
    int32 mbrec_length;
    int32 mbrec_offset;    
    int32 brec_length;
    int32 brec_offset;
    int32 nor_id_length;
    int32 nor_id_offset;
    int32 lfi_length;
    int32 lfi_offset;
    int32 part_tbl_length;
    int32 part_tbl_offset;
    int32 part_length;
    int32 part_offset;    
    int32 g_write_sector;
    int32 progress;
    int32 lfi_start_addr;
    uint32 upgrade_type;
    uint8 udisk_setting;
    uint8 fw_mode;
    uint8 reserved[2];
    uint8 UsbSetupInfo[48];
    uint32 CapInfo[4];
}__attribute__ ((packed)) _para_t;

typedef enum
{
    ENTER_UPGREADE = 0,
    INSTALL_DRV,
    SEARCH_FMU,
    READ_BREC,
    WRITE_BREC,
    CHECK_BREC,
    READ_LFI,
    WRITE_LFI,
    WRITE_MBREC,    
    CHECK_LFI,
    WRITE_PART,
    WRITE_PART_TABLE,
    EARSE_VRAM,
    RENAME_FMU,
    UPGREADE_OVER
} upgrade_prg;
typedef struct
{
    unsigned char *pInOutBuffer;
    long InOutLen;
    long FileLength;
    unsigned char *pGLBuffer;
    unsigned char *initusebuffer;
    long initusebufferlen;
} PCFWDecrypt_struct;

struct fw_decrypt_operations
{
    audio_device_op_i fw_decrypt_init;
    audio_device_op_i fw_decrypt_run;
};


#define UPGRADE_TYPE_FACTORY  0
#define UPGRADE_TYPE_ENDUSER  1

extern PCFWDecrypt_struct g_decrypt;
extern _para_t _para;
extern struct fw_decrypt_operations g_decrypt_ops;
extern uint32 g_firmware_offset;
extern uint8 g_led_state;

extern void deal_upgrade(void) __FAR__;
extern void decrypt_init(void) __FAR__;
extern uint32 _read_file_translate(uint8 *read_buffer, uint32 read_len) __FAR__;
extern uint32 _read_file(uint32 read_len) __FAR__;
extern void PCFWDecrypt(unsigned char *pInOutBuffer, long BufferLength,unsigned char *pGLBuffer)__FAR__;
extern int PCFWDecryptInit(PCFWDecrypt_struct *PCFWDecrypt_s)__FAR__;

#endif
