////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2011, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:debug.h
//
//		Description	:
//					Define debug ordinary print & debug routine
//
//		Changelog	:
///////////////////////////////////////////////////////////////////////////////

/**
 * @addtogroup MVUtils
 * @{
 * @defgroup debug debug
 * @{
 */


#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include "type.h"

///extern int kprintf(const char *fmt, ...);

#define	DBG(format, ...)		printf(format, ##__VA_ARGS__)

int DbgUartInit(int Which, unsigned int BaudRate, unsigned char DatumBits, unsigned char Parity, unsigned char StopBits);

#endif //__DBG_H__ 

