/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：mmu定义
 * 作者：liminxian
 ********************************************************************************/
 
/* m32tlb.h : MIPS32 MMU definitions*/
 
#ifndef __M32TLB_H__
#define __M32TLB_H__

#ifdef __cplusplus
extern "C" {
#endif

/* XXX For the moment we just share the R4000 definitions, but
   we need to add support for BAT based processor variants. */
#assert mmu(r4k)

/*#include <mips/r4ktlb.h> ########## by jason 2005-9-21*/
#include "r4ktlb.h"

/* MIPS32r2 EntryLo extended page frame address bits */
#if __mips == 64 || __mips64
#define TLB_PFNXMASK	0x007fffffc0000000LL
#else
#define TLB_PFNXMASK	0xc0000000
#endif
#define TLB_PFNXSHIFT	29

#ifdef __cplusplus
}
#endif
#endif /* __M32TLB_H__ */

