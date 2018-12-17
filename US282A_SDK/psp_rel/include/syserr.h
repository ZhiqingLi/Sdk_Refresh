/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：错误信息类型定义
 * 作者：liminxian
 ********************************************************************************/

#ifndef _SYSERR_H
#define _SYSERR_H
#include <typeext.h>
//**********************************************
//系统内核使用的错误组号 (0x00-0x3f)
//**********************************************
#define ERR_DRV         0x01
#define ERR_DSP         0x02
#define ERR_CFG         0x03
#define ERR_SDFS        0x04
#define ERR_USB         0x05
//**********************************************
//系统内核使用的错误组号 (0x40-0x7f)
//**********************************************
//ui 驱动出错组号
#define ERR_UI          0x40
//vram 驱动出错组号
#define ERR_VRAM        0x41
//文件系统出错组号
#define ERR_FS          0x42
//ud 驱动出错组号
#define ERR_UD          0x43
//键盘驱动出错组号
#define ERR_KY          0x44
//**********************************************
//系统内核使用的错误组号 (0x80-0xff)
//**********************************************
#define ERR_OTHR        0x80

//---------------------------------------------------------------------------------------
#define ERR_DRV_FILE_NOT_FOUND                                  0x00+ERR_DRV*0x100
#define ERR_DRV_FORMAT_ERROR                                    0x01+ERR_DRV*0x100
#define ERR_DRV_DRIVER_NOT_INSTALLED                            0x02+ERR_DRV*0x100
#define ERR_DRV_DRIVE_NOT_SUPPORT                               0x03+ERR_DRV*0x100
#define ERR_DRV_UD_NOT_ACTIVED                                  0x04+ERR_DRV*0x100
#define ERR_DRV_DRIVE_MISSING                                   0x05+ERR_DRV*0x100
#define ERR_DRV_DRIVE_NOT_FORMAT                                0x06+ERR_DRV*0x100
#define ERR_DRV_FS_NOT_SUPPORT                                  0x07+ERR_DRV*0x100
//---------------------------------------------------------------------------------------
#define ERR_DSP_INIT_CODE_FILE_NOT_FOUND                        0x00+ERR_DSP*0x100
//---------------------------------------------------------------------------------------
#define ERR_CFG_MAINAP_NOT_FOUND                                0x00+ERR_CFG*0x100
#define ERR_CFG_MAINAP_FORMAT_ERR                               0x01+ERR_CFG*0x100
//---------------------------------------------------------------------------------------
#define ERR_SDFS_FP_ERROR                                       0x00+ERR_SDFS*0x100
#define ERR_SDFS_FILE_FORMAT_ERROR                              0x01+ERR_SDFS*0x100
#define ERR_SDFS_OPEN_3TH_FILE                                  0x02+ERR_SDFS*0x100
#define ERR_SDFS_FILE_NAME_ILLEGAL                              0x03+ERR_SDFS*0x100
#define ERR_SDFS_MODE_NOT_SUPPORT                               0x04+ERR_SDFS*0x100
#define ERR_SDFS_FILE_NOT_FOUND                                 0x05+ERR_SDFS*0x100
//---------------------------------------------------------------------------------------
#define ERR_USB_SYSINFO_SYS_FILE_NOT_FOUND                      0x00+ERR_USB*0x100
//---------------------------------------------------------------------------------------
#define ERR_OTHER_BANKSW_REENT  				0x05+ERR_OTHR*0x100
//---------------------------------------------------------------------------------------

#endif
