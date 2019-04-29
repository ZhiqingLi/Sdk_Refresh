/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2004              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

#ifndef __MUSB_NONE_PLATFORM_MEMORY_H__
#define __MUSB_NONE_PLATFORM_MEMORY_H__

/*
 * non-OS platform-specific memory abstraction definitions
 * $Revision: 1.3 $
 */

#include "brd_mem.h"

#ifdef MUSB_STDLIB_MEM

#include <stdlib.h>
#include <string.h>

#define MUSB_MemAlloc malloc
#define MUSB_MemRealloc realloc
#define MUSB_MemFree free
#define MUSB_MemCopy memcpy
#define MUSB_MemSet memset

#endif

#endif	/* multiple inclusion protection */
