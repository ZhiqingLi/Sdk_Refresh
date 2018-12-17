/*******************************************************************************
 *                              us211A
 *                            Module: case
 *                 Copyright(c) 2003-2008 Actions Semiconductor,
 *                            All Rights Reserved. 
 *
 * History:         
 *      <author>    <time>           <version >             <desc>
 *       cailizhen  2014-12-19 16:18     1.0             build this file 
 *******************************************************************************/
/*!
 * \file     vm_def.h
 * \brief    包含各个应用所占VM空间宏定义
 * \author   zhangxs
 * \version  1.0
 * \date     2014-12-19
 *******************************************************************************/
#ifndef _VM_DEF_H
#define _VM_DEF_H

/* AP VRAM 数据，0x00000000 - 0x001f0000，支持32个VARM ID，每个VRAM空间大小没有规定 */
#define  VM_BTSTACK             0x00000000//0 蓝牙设备信息 /*该地址固定下来了，不能修改*/
#define  VM_BTSTACK_PAIREDLIST  0x00010000//1 配对列表信息
#define  VM_AP_SETTING          0x00020000//2 系统设置信息
#define  VM_AP_TIME_ALARM       0x00030000//3 时间闹钟记录
#define  VM_AP_CONFIG           0x00040000//4 开关机配置信息，暂时没用
#define  VM_AP_MUSIC            0x00050000//5 MUSIC配置信息
#define  VM_AP_LINEIN           0x00060000//6 LINEIN配置信息
#define  VM_AP_RADIO            0x00070000//7 收音机配置信息
#define  VM_AP_RECORD           0x00080000//8 录音配置信息
#define  VM_UHOST_BKINFO        0x00090000//9 host 断点保存
#define  VM_CARD_BKINFO         0x000a0000//a card 断点保存
#define  VM_AP_FMENGINE         0x000b0000//b FM引擎配置信息
#define  VM_AP_RECORD_WAV       0x000c0000//c 录音断电恢复的数据记录地址
#define  VM_AP_USOUND           0x000d0000//d usound 保存上次播放的采样率信息
#define  VM_ASET_PARM_BASE      0x000e0000//e ASET调试参数保存
#define  VM_ASQT_PARM_BASE      0x000f0000//f ASQT调试参数保存

//BTMANAGER_ENABLE_HFP_PHONEBOOK 只有该配置项打开才会使用该VRAM区
#define  VM_BTPHONEBOOK         0x00100000//0x10 蓝牙电话本信息，该功能VRAM保存有问题，下次更新 2015-9-18

//安排这部分VRAM数据在这个index，保证数据不会丢失
//!!!注意这部分index不能和其他模块复用,电话本功能暂时关闭
#define VM_S3BT_APP_GLOBAL      0x00100000   //28bytes
#define VM_S3BT_BT_MANAGER      0x00110000   //78bytes
#define VM_S3BT_APP_STATUS      0x00120000   //3bytes
//安排这部分VRAM数据，当VRAM发生merge的时候丢弃这部分数据
#define VM_S3BT_BT_STACK        0x00400000   //0x6000

//SUPPORT_RING_FOLDER 只有该宏配置为1才会使用该VRAM区
#define  VM_RING_FOLDER_BASE    0x001c0000//0x1c ~ 0x1f 每个文件名字32字节，可以保存64个文件，共2KB，分4个INDEX存放
#define  VM_DEW_PARAM_BASE      0x00130000//13 ~ 14 音效增强和减弱参数区
#define  VM_DEW_PARAM_BASE_REV  0x00140000//13 ~ 14 音效增强和减弱参数区

//OTA传输断点保存
#define  VM_OTA  				0x00150000
//OTA分区信息保存
#define  VM_OTA_PART_INFO  	    0x00160000
//OTA握手信息保存
#define  VM_OTA_CONNECT  	    0x00170000

#define  VM_OTA_UPGRADE  	    0x00180000

//FT mode / bqb mode标志
#define  VM_API_FLAG           0x00190000

#define VM_DIR_LIST             0x001a0000

#define  ATT_MAGIC              0xABCD


#define VM_S3BT_BT_STACK_LEN   0x6000


/* AP VRAM 数据区标志 = 0x55AA + ID (0x00 - 0x1f) */
#define  VRAM_MAGIC(vram_addr)  (0x55AA + (short)((vram_addr) >> 16))

//S3BT备份和恢复NOR地址分配
#define  NOR_S3BT_ERASE_WRITE   (0) //size = 0.5KB，仅用来擦写：写第一个扇区擦写
#define  NOR_S3BT_APP_GLOBAL    (1) //size = 1KB
#define  NOR_S3BT_BT_MANAGER    (3) //size = 1KB
#define  NOR_S3BT_APP_STATUS    (5) //size = 1.5KB
#define  NOR_S3BT_BT_STACK      (8) //size = 24KB

//S3BT备份和恢复SRAM地址
#define  SRAM_S3BT_BUFFER       (0x9fc35800) //0x8000 = 28KB

#define  SRAM_S3BT_SIZE         (0x6000)

//RTC BAK0 分配
#define MY_RTC_S3BT         0  //bit0表示进入S3BT
#define MY_RTC_BT_FLAG      1  //bit1表示是否在蓝牙在线时进入S3BT
#define MY_RTC_ESD_FLAG     2  //bit2表示ESD RESTART
#define MY_RTC_FUNC_INDEX   8  //bit[8-15]表示ESD要回去的FUNC INDEX
#define MY_RTC_BTPLAY_FLAG  16 //bit[16-18]表示蓝牙是否处于播放状态
#endif
