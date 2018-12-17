#ifndef __COMMON_H
#define __COMMON_H
#define SBC_MIPS

#include "sbc_encode_interface.h"

#define memset        libc_memset
#define memcpy        libc_memcpy

//#define SBC_32BPCM
#ifndef SBC_MIPS
#define SBC_DEBUG

//#include <stdio.h>
//#include <malloc.h>
//#include <string.h>

typedef unsigned __int64 uint64_t;
typedef __int64 int64_t;
#else
#define CHAR_BIT      8         /* number of bits in a char */
typedef unsigned long long uint64_t;
typedef long long int64_t;
#endif

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef int int32_t;
#define int16_t short
#ifndef _TYPEEXT_H
typedef unsigned int size_t;
#endif
typedef int ssize_t;

typedef unsigned int uintptr_t;

#define SBC_ALWAYS_INLINE  
#define AFMT_S16_BE 0
#define VERSION 0

#ifndef NULL
#define NULL 0
#endif
#endif
