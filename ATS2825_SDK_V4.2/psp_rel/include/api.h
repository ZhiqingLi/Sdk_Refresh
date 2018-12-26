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
 * \file     api.h
 * \brief    各个驱动总入口的索引号,API syscall需要调用
 * \author   wuyueqian
 * \version  1.0
 * \date  2011/9/10
 *******************************************************************************/
#ifndef _API_H
#define _API_H

#define API_BASE_ENTRY            0x0
#define API_CARD_ENTRY            0x1
#define API_UHOST_ENTRY           0x2
#define API_FAT32_ENTRY           0x3
//#define API_FAT16_ENTRY 0x4
#define API_EXFAT_ENTRY           0x4
#define API_UD_ENTRY              0x5
#define API_LCD_ENTRY             0x6
#define API_FM_ENTRY              0x7
#define API_KEY_ENTRY             0x8
//#define API_I2C_ENTRY             0x9
#define API_CCD_ENTRY             0x9
#define API_AUDIO_DEVICE_ENTRY    0xa
#define API_TTS_ENTRY             0xB
#define API_BT_ENTRY              0xC
#define API_RESERVE1_ENTRY        0xD

#define API_SYS_ENTRY             0xF
#define API_LIBC_ENTRY            0x10
#define API_VFS_ENTRY             0x11

#define API_FRONT_BASAL_ENTRY        0x00000040
#define API_FRONT_CODEC_ENTRY        0x00000041
#define API_BACK_BASAL_ENTRY         0x00000042
#define API_BACK_CODEC_ENTRY         0x00000043

#endif
