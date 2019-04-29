/**
 * @file   types.h
 * @author  <lab@LAB-21>
 * @date   Tue Apr 14 10:57:24 2015
 * 
 * @brief  
 * 
 * 
 */
#ifndef __TYPES_H__
#define __TYPES_H__

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifdef FALSE
#undef FALSE
#endif

#ifdef TRUE
#undef TRUE
#endif

typedef enum { FALSE    = 0, TRUE   = !FALSE } boolean, bool_t;

typedef unsigned long long      uint64;
typedef unsigned int            uint32;
typedef unsigned short          uint16;
typedef unsigned char           uint8;
typedef long long               int64;
typedef int                     int32;
typedef short                   int16;
typedef char                    int8;
typedef float                   fp32;
typedef double                  fp64;

/* 
 * Unsigned types
 */
typedef unsigned char           u_int8;  /* unsigned integer,  8 bits long */
typedef unsigned short          u_int16; /* unsigned integer, 16 bits long */
typedef unsigned int            u_int32; /* unsigned integer, 32 bits long */

/*
 * Signed types 
 */
typedef char                    s_int8;
typedef short                   s_int16;
typedef int                     s_int32;

/*
 * Natural size of integers on processor
 * Uses: 1. for printf  format control and parameter compatability!
 *       2. for defining bit-fields (non-portable!)
 *       3. for efficient use of local variables (avoids masking)
 */
typedef unsigned int            u_int;  /* natural unsigned integer on processor */
typedef unsigned long int       u_intL; /* natural long unsigned integer on processor */

typedef unsigned long long      uint64_t;
typedef unsigned int            uint32_t;
typedef unsigned short          uint16_t;
typedef unsigned char           uint8_t;
typedef unsigned int            uint_t;
typedef long long               int64_t;
typedef int                     int32_t;
typedef short                   int16_t;
typedef char                    int8_t;
typedef int                     int_t;

typedef struct s_u_int64 {
    u_int32 low ;      /* low 32 bits of a 64 bit value */
    u_int32 high;      /* high 32 bits of a 64 bit value */
} t_u_int64;
typedef union _HALF_WORD {
    uint16 i; 
    uint8 c[2]; 
} HF_WORD;

typedef void (*intr_callback)(void);
#define BIT(n)		(1L << (n))
#endif
