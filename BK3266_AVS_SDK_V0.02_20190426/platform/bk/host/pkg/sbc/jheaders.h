/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _JHEADERS_H_
#define _JHEADERS_H_

/* uw/port/$CONFIG_PORT/includes.h */
#include <includes.h>
#include <os_port.h>

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef int ssize_t;
#endif

/* Non-GNU environment */
#define  __inline inline
typedef unsigned long long uint64_t;
typedef long long int64_t;
typedef unsigned long uintptr_t;
#define EIO     5
#define ENOMEM 12
#define ENOSPC 28

/* Don't use ARM assembler in sbc_math.h */
#ifdef __arm__
#undef __arm__
#endif

#ifdef CHAR_BIT
#undef CHAR_BIT
#define CHAR_BIT                8
#endif

#ifndef memset
#define memset                  j_memset
#endif

#ifndef memcpy
#define memcpy                  j_memcpy
#endif

#ifndef __BYTE_ORDER
/*
 * Our CPU_LITTLE_ENDIAN and CPU_BIG_ENDIAN are assumed to be identical to
 * Linux __LITTLE_ENDIAN and __BIG_ENDIAN
 */
#define __BYTE_ORDER            CONFIG_BYTE_ORDER
#endif

#endif
