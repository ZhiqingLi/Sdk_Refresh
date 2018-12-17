/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：数据类型定义
 * 作者：liminxian
 ********************************************************************************/
#ifndef _TYPEEXT_H
#define _TYPEEXT_H

#include <ucos_types.h>

#ifndef _ASSEMBLER_

//#define  bool unsigned char
#ifndef  uchar
#define  uchar unsigned char
#endif

#ifndef  uint8
typedef unsigned char uint8;
#endif

#ifndef  int8
typedef signed char int8;
#endif

#ifndef  uint16
typedef unsigned short uint16;
#endif

#ifndef  int16
typedef signed short int16;
#endif

#ifndef  uint32
typedef unsigned int uint32;
#endif

#ifndef  int32
typedef signed int int32;
#endif

#ifndef  ulong32
typedef unsigned long ulong32;
#endif

#ifndef  long32
typedef signed long long32;
#endif

#ifndef  HWORD
#define  HWORD unsigned short
#endif

#ifndef  hword
#define  hword unsigned short
#endif

//#define  WORD unsigned int
#ifndef  word
#define  word unsigned int
#endif


#ifndef PC

//#ifndef  DWORD
//#define  DWORD unsigned long long
//#endif

#ifndef  dword
#define  dword unsigned long long
#endif

#endif

#ifndef  HANDLE
#define  HANDLE unsigned int
#endif

//#ifndef  handle
//#define  handle unsigned int
//#endif


#ifndef  ulong
#define  ulong unsigned long
#endif

#ifndef  INT8
typedef char INT8;
#endif

#ifndef  UINT8
typedef unsigned char UINT8;
#endif

#ifndef  INT16
typedef short INT16;
#endif

#ifndef  UINT16
typedef unsigned short UINT16;
#endif

#ifndef  INT32
typedef int INT32;
#endif

#ifndef  UINT32
typedef unsigned int UINT32;
#endif

#ifndef PC
#ifndef  INT64
typedef long long INT64;
#endif

#ifndef  UINT64
typedef unsigned long long UINT64;
#endif

#else
typedef _int64 INT64;
typedef _int64 UINT64;
#endif

#ifndef  NULL
#define  NULL ((void *)0)
#endif

#endif/* _ASSEMBLER_ */

#ifndef  TRUE
#define  TRUE   1
#endif

#ifndef  FALSE
#define  FALSE  0
#endif

#ifndef  FOREVER
#define  FOREVER 1
#endif

#ifndef  NEVER
#define  NEVER 0
#endif


#ifndef NULL
#define  NULL 0
#endif

#define VOID_SIGNED(value) ((void*)((int32)(value)))
#define VOID_UNSIGNED(value) ((void*)((uint32)(value)))

#ifndef PC
#define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
#else
#define __ATTRIBUTE_PACKED__
#endif

//for register operate 
#ifndef REG32
#define REG32 volatile unsigned int*  
#endif

#endif  /* _TYPEEXT_H*/
