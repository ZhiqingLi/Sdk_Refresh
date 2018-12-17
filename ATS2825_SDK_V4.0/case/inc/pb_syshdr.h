/* This is an example of a header file for platforms/compilers that do
 * not come with stdint.h/stddef.h/stdbool.h/string.h. To use it, define
 * PB_SYSTEM_HEADER as "pb_syshdr.h", including the quotes, and add the
 * extra folder to your include path.
 *
 * It is very likely that you will need to customize this file to suit
 * your platform. For any compiler that supports C99, this file should
 * not be necessary.
 */

#ifndef _PB_SYSHDR_H_
#define _PB_SYSHDR_H_

/* stdint.h subset */
#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
/* You will need to modify these to match the word size of your platform. */
#ifndef int8_t
#define int8_t signed char
#endif

typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;

#ifndef uint32_t
#define uint32_t unsigned int
#endif

typedef signed long long int64_t;
typedef unsigned long long uint64_t;

/* These are ok for most platforms, unless uint8_t is actually not available,
 * in which case you should give the smallest available type. */
typedef int8_t int_least8_t;
typedef uint8_t uint_least8_t;
typedef uint8_t uint_fast8_t;
typedef int16_t int_least16_t;
typedef uint16_t uint_least16_t;
#endif

/* stddef.h subset */
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#else

#ifndef size_t
#define size_t uint32_t
#endif
#define offsetof(st, m) ((size_t)(&((st *)0)->m))

#ifndef NULL
#define NULL 0
#endif

#endif

/* stdbool.h subset */
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#else

#ifndef __cplusplus
#ifndef bool
#define bool int
#endif
#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif
#endif

#endif

/* stdlib.h subset */
#ifdef PB_ENABLE_MALLOC
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
void *realloc(void *ptr, size_t size);
void free(void *ptr);
#endif
#endif

/* string.h subset */
#ifdef HAVE_STRING_H
#include <string.h>
#else
#endif

#endif
