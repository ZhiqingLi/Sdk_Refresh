
#ifndef _V_TYPDEF_H
#define _V_TYPDEF_H


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include <stdint.h>

#ifdef TARGET_BEST1000
#include "plat_types.h"
#else
#define __PSRAMBSS
#endif

#ifndef FALSE
#define FALSE                          0
#endif

#ifndef TRUE
#define TRUE                           1
#endif

#define VOS_TRUE                        TRUE
#define VOS_FALSE                       FALSE


#define VOS_NULL               (0)


#if defined(VOS_ZSP)

typedef unsigned short          VOS_BOOL;

typedef unsigned short          VOS_UINT8;

typedef signed   short          VOS_INT8;

typedef signed   short          VOS_CHAR;

typedef unsigned short          VOS_UCHAR;

typedef unsigned short          VOS_CPU_SR;

typedef unsigned short          VOS_STK;

#endif

#if defined(VOS_ARM) || (VOS_X86)

typedef unsigned long          VOS_BOOL;

typedef unsigned char          VOS_UINT8;

typedef signed char            VOS_INT8;

typedef char                   VOS_CHAR;

typedef unsigned char          VOS_UCHAR;

typedef unsigned long          VOS_CPU_SR;

typedef unsigned long          VOS_STK;

#ifdef TARGET_BEST1000
#define VOS_PSRAM_STATIC       __PSRAMBSS static
#else
#define VOS_PSRAM_STATIC
#endif

#endif

typedef void             VOS_VOID;

typedef uint16_t         VOS_UINT16;

typedef int16_t          VOS_INT16;

typedef uint32_t         VOS_UINT32;

typedef int32_t          VOS_INT32;

typedef int32_t          VOS_INT;

typedef float            VOS_FLOAT;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _V_TYPDEF_H */

