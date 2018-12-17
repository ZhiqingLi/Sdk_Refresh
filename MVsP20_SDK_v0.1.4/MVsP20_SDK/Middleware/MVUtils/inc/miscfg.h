/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2012, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			msic.h
* Description:		define global MACRO in here
*
******************************************************************************
*/

/**
 * @addtogroup MVUtils
 * @{
 * @defgroup msicfg msicfg
 * @{
 */


#ifndef __MISCFG_H__
#define __MISCFG_H__

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Flash low-1KB  configuration>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define DWNLDR_IMGSIZE_ADDR			(0xE8)
#define DWNLDR_PID2CRC_ADDR			(0xEC)	
#define DWNLDR_CONSTDAT_ADDR		(0xF0)
#define DWNLDR_CONSTDAT_OFFSET	(0x080000)	//1MB offset from flash start address 0
#define DWNLDR_USRDAT_ADDR			(0xF4)
#define DWNLDR_USRDAT_OFFSET	(0x100000)	//2MB offset from flash start address 0
#define DWNLDR_SDKVER_ADDR			(0xF8)
#define DWNLDR_CODECRC_ADDR			(0x100)
#define DWNLDR_CODEMAGIC_ADDR		(0x104)

#endif //_MISCFG_H_

