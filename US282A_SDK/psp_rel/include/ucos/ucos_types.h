/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：ucos可以移植类型
 * 作者：liminxian
 ********************************************************************************/
 
#ifndef   __USDK130_TYPES_H__
#define   __USDK130_TYPES_H__

#ifndef _ASSEMBLER_

typedef unsigned char BOOLEAN;
typedef unsigned char INT8U; /* Unsigned  8 bit quantity                           */
typedef signed char INT8S; /* Signed    8 bit quantity                           */

typedef unsigned short INT16U; /* Unsigned 16 bit quantity                           */
typedef signed short INT16S; /* Signed   16 bit quantity                           */

typedef unsigned long INT32U; /* Unsigned 32 bit quantity                           */
typedef signed long INT32S; /* Signed   32 bit quantity                           */
typedef float FP32; /* Single precision floating point                    */
typedef double FP64; /* Double precision floating point                    */

#ifndef PC
typedef unsigned long long int INT64U;
typedef long long int INT64S;
#else
typedef _int64 INT64U;
typedef _int64 INT64S;
#endif

typedef unsigned int OS_STK; /* Each stack entry is 16-bit wide                    */
typedef unsigned int OS_CPU_SR; /* Define size of CPU status register (PSR = 32 bits) */

#ifndef  BYTE
#define BYTE           INT8S                     /* Define data types for backward compatibility ...   */
#endif

#ifndef  UBYTE
#define UBYTE          INT8U                     /* ... to uC/OS V1.xx.  Not actually needed for ...   */
#endif

#ifndef  WORD
#define WORD           INT16S                    /* ... uC/OS-II.                                      */
#endif

#ifndef  UWORD
#define UWORD          INT16U
#endif

#ifndef  LONG
#define LONG           INT32S
#endif

#ifndef  ULONG
#define ULONG          INT32U
#endif

#ifndef  bool
#define bool INT8U
#endif
typedef INT16U OS_FLAGS; /* Date type for event flag bits (8, 16 or 32 bits)             */
#endif /*_ASSEMBLER_*/

#endif /* __USDK130_TYPES_H__ */

