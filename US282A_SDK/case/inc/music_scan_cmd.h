/*! \cond MUSIC_SCAN_API*/
/********************************************************************************
 *                              5110
 *                            Module: image_decode
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>       <time>           <version >             <desc>
 *      cailizhen    2011-08-27           1.0             build this file
 ********************************************************************************/
/*!
 * \file     music_scan_cmd.h
 * \brief    定义MUSIC目录扫描命令ID
 * \author   cailizhen
 * \version  1.0
 * \date  2011/09/19
 *******************************************************************************/
/*! \endcond*/
#ifndef __MUSIC_SCAN_CMD_H__
#define __MUSIC_SCAN_CMD_H__

/* MIPS */
#include "psp_includes.h"

/*! \cond MMM_ID_API*/
/*!
 *  \brief
 *     提供给应用层的图片解码命令
 */
typedef enum {
	/*! 打开MUSIC SCAN */
	MUSIC_SCAN_OPEN = 0,
	/*! 关闭MUSIC SCAN */
	MUSIC_SCAN_CLOSE,
	/*! 获取状态 */
	MUSIC_SCAN_GET_STATUS,
} music_scan_cmd_t;


/*!
 *  \brief
 *     接口函数声明
 *     接口函数定义在kernel_api.S，加载驱动以后使用，驱动API号在api.h，后续会统一整理
 */
extern int music_scan_cmd(void *ms_handle, music_scan_cmd_t cmd, unsigned int param)__FAR__;

#endif // __MUSIC_SCAN_CMD_H__
