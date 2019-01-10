/****************************************************************************
 *
 * File:
 *     $Id: sbc_math.h 1329 2009-04-14 15:12:06Z dliechty $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 1329 $
 *
 * Description: This file contains header definitions for the REAL (fixed 
 *     or floating-point) math functions.  
 *
 * Created:     September 14, 2004
 *
 * Copyright 2004-2005 Extended Systems, Inc.
 * Portions copyright BES.
 * All rights reserved.  All unpublished rights reserved.
 *
 * Unpublished Confidential Information of BES.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of 
 * BES. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
 
#ifndef _SBC_MATH_H_
#define _SBC_MATH_H_

#include "xatypes.h"

/*---------------------------------------------------------------------------
 * SBC Real Math layer
 *
 *     SBC requires operations on real numbers when performing the synthesis
 *     or analysis phases.  The BES SBC CODEC is designed to use either 
 *     floating-point or fixed-point integer based math.  Since many processors
 *     do not provide floating-point operations the default is to use 
 *     fixed-point integer math.  This library provides the math functions 
 *     required by the BES SBC CODEC.  
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * INLINE constant
 *
 *     Most compilers allow the ability to compile functions in-line.  This
 *     constant should be defined appropriately in order to enable this feature.
 *     Using in-line functions increases performance significantly.
 *
 *     The default is to define INLINE to __inline
 */
#ifndef INLINE
#define INLINE __inline
#endif

/*---------------------------------------------------------------------------
 * SBC_MATH_USE_FIXED_HI_RES constant
 *
 *     Fixed-point, High resolution math.
 */
#define SBC_MATH_USE_FIXED_HI_RES  1

/*---------------------------------------------------------------------------
 * SBC_MATH_USE_FIXED_LO_RES constant
 *
 *     Fixed-point, Low resolution math.
 */
#define SBC_MATH_USE_FIXED_LO_RES  2

/*---------------------------------------------------------------------------
 * SBC_MATH_USE_FIXED_ASM constant
 *
 *     Hand-coded assembly routines must be provided.
 */
#define SBC_MATH_USE_FIXED_ASM     3

/*---------------------------------------------------------------------------
 * SBC_MATH_USE_FLOAT constant
 *
 *     Floating-point math.
 */
#define SBC_MATH_USE_FLOAT         4

/*---------------------------------------------------------------------------
 * SBC_MATH_FUNCTIONS
 *
 *     Determines the type of math operations used by the SBC Codec.  The 
 *     choice of operation type should be based both on the capabilies of 
 *     the target processor as well as the desired accuracy/performance of
 *     the encode/decode process.  
 *
 *     If a processor has a floating-point co-processor, then the best choice
 *     may be to enable floating-point operations.
 *
 *     If there is no floating-point co-processor, then the best choice will
 *     undoubtedly be to enable fixed-point operations.  Fixed-point math for
 *     the BES Codec uses a 32-bit 17:15 format, where the high order
 *     17 bits represent the whole part of the number and the low order 15 bits
 *     represent the fractional part of the number (see Mul and Div).  In some
 *     calculations, a 2:30 format is used (see MulP).
 *
 *     If low processor usage is more important than accuracy, it is possible
 *     to use low resolution math functions.  It is important to note, however, 
 *     that in order to qualify an SBC Codec it is necessary to declare the 
 *     quality of the calcuations as described in section 9.4.1 of the A2DP test 
 *     specification v1.0.  The quality of the low resolution functions used by 
 *     the BES SBC Codec are defined as K=13 for the decoder, and K=7 for
 *     the encoder (see section 9.4.1).
 *
 *     It is also possible to use hand-coded assembly language routines for
 *     the target processor.  These routines must be provided by the developer.
 *     Sample routines are provided only for the INTEL x386 processor family.
 *     The best performance is made when routines are assembled in-line.
 *
 *     The default is to use fixed-point, high resolution math.
 */
#ifndef SBC_MATH_FUNCTIONS
#define SBC_MATH_FUNCTIONS  SBC_MATH_USE_FIXED_HI_RES
#endif

#if SBC_MATH_FUNCTIONS == SBC_MATH_USE_FIXED_LO_RES
/* Fixed Point enabled, Fixed Macros enabled, Asm disabled, HiRes disabled */
#define SBC_USE_HIRES_MACROS XA_DISABLED
#elif SBC_MATH_FUNCTIONS == SBC_MATH_USE_FIXED_ASM
/* Fixed Point enabled, Fixed Macros disabled, Asm enabled, HiRes - N/A */
#define SBC_USE_FIXED_MACROS XA_DISABLED
#elif SBC_MATH_FUNCTIONS == SBC_MATH_USE_FLOAT
/* Fixed Point disabled, Fixed Macros - N/A, Asm - N/A, HiRes - N/A */
#define SBC_USE_FIXED_POINT  XA_DISABLED
#endif

/* Fixed-point used */
#ifndef SBC_USE_FIXED_POINT
#define SBC_USE_FIXED_POINT XA_ENABLED
#endif

/* Fixed-point macros used */
#ifndef SBC_USE_FIXED_MACROS
#define SBC_USE_FIXED_MACROS XA_ENABLED
#endif

/* High resolution macros used */
#ifndef SBC_USE_HIRES_MACROS
#define SBC_USE_HIRES_MACROS XA_ENABLED
#endif

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/
 
/*---------------------------------------------------------------------------
 * REAL type
 *
 * Fixed-point math (17:15) or floating-point type.  If SBC_USE_FIXED_POINT 
 * is set to XA_ENABLED, then REAL is defined as a 32-bit signed value, 
 * otherwise it is defined as a floating-point value.  Lookup table values 
 * are stored in 2:32 format for increased precision, therefore the MulP() 
 * function must take this into account.  The Mul() function always  operates
 * on 17:15 fixed numbers.
 */
#if SBC_USE_FIXED_POINT == XA_ENABLED
typedef S32 REAL;
#else
typedef float REAL;
#endif
/* End of REAL */

/*---------------------------------------------------------------------------
 * INT8 type
 *
 * 8 bit signed integer type.  This type is used at several points for casting
 * to an 8 bit signed integer.  This fixes some compatibility issues with the
 * ADS toolset.
 */
typedef signed char INT8;
/* End of INT8 */

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * S16toReal()
 *
 *     Converts a signed 16-bit integer to a real number (17:15 fixed or float).
 *
 * Parameters:
 *
 *     Integer - a 16 bit signed integer.
 *
 * Returns:
 *
 *     A real number (fixed or float).
 */
REAL S16toReal(S16 Integer);

/*---------------------------------------------------------------------------
 * RealtoS16()
 *
 *     Converts a real number (17:15 fixed or float) to a signed 16 bit integer.
 *
 * Parameters:
 *
 *     Real - a real number (fixed or float).
 *
 * Returns:
 *
 *     A 16 bit signed integer.
 */
S16 RealtoS16(REAL Real);

/*---------------------------------------------------------------------------
 * RealtoU16()
 *
 *     Converts a real number (17:15 fixed or float) to an unsigned 16 bit integer.
 *
 * Parameters:
 *
 *     Real - a real number (fixed or float).
 *
 * Returns:
 *
 *     A 16 bit unsigned integer.
 */
U16 RealtoU16(REAL Integer);

/*---------------------------------------------------------------------------
 * RealtoS32()
 *
 *     Converts a real number (17:15 fixed or float) to a signed 32-bit integer.
 *
 * Parameters:
 *
 *     Real - a real number (fixed or float).
 *
 * Returns:
 *
 *     A 32-bit signed integer.
 */
S32 RealtoS32(REAL Real);

/*---------------------------------------------------------------------------
 * Mul()
 *
 *     Multiplies two real numbers (fixed or float).  For fixed, both of the
 *     numbers are in 17:15 format.
 *
 *     For fixed-point math, the following formula is used.
 *
 *     (x * y) >> 15
 *
 * Parameters:
 *
 *     x - a real number (fixed or float).
 *
 *     y - a real number (fixed or float).
 *
 * Returns:
 *
 *     Results of the multiplication (fixed or float).
 */
REAL Mul(REAL x, REAL y);

/*---------------------------------------------------------------------------
 * MulP()
 *
 *     Multiplies two real numbers (fixed or float) with high precision.  For 
 *     fixed, one of the numbers is in 17:15 format and the other is in 2:30
 *     format.  This requires a 30-bit shift right after multiplying the two 
 *     numbers.
 *
 *     For fixed-point math, the following formula is used.
 *
 *     (x * y) >> 30
 *
 * Parameters:
 *
 *     x - a real number (fixed or float).
 *
 *     y - a real number (fixed or float).
 *
 * Returns:
 *
 *     Results of the multiplication (fixed or float).
 */
REAL MulP(REAL x, REAL y);

/*---------------------------------------------------------------------------
 * MulPI()
 *
 *     Multiplies a signed integer and a real number (fixed or float) with 
 *     high precision.  For fixed, the first number is in 32:0 format and the 
 *     other is in 2:30 format.  This requires a 30-bit shift right after 
 *     multiplying the two numbers.
 *
 *     For fixed-point math, the following formula is used.
 *
 *     ((x << 15) * y) >> 30 or (x * y) >> 15
 *
 * Parameters:
 *
 *     x - a real number (fixed or float).
 *
 *     y - a real number (fixed or float).
 *
 * Returns:
 *
 *     Results of the multiplication (fixed or float).
 */
REAL MulPI(REAL x, REAL y);

/*---------------------------------------------------------------------------
 * dMulP()
 *
 *     Multiplies two real numbers (fixed or float) with high precision.  This
 *     function is used only by the decoder.  For fixed, one of the numbers is
 *     in 17:15 format and the other is in 2:30 format.  This requires a 30-bit
 *     shift right after multiplying the two numbers.
 *
 *     For fixed-point math, the following macro is used.
 *
 *     (x >> 13) * (y)
 * 
 *     For assembly language implementations, the following formula should be
 *     used, because a different set of tables are used.
 * 
 *     (x * y) >> 30
 *
 * Parameters:
 *
 *     x - a real number (fixed or float).
 *
 *     y - a real number (fixed or float).
 *
 * Returns:
 *
 *     Results of the multiplication (fixed or float).
 */
REAL dMulP(REAL x, REAL y);

#if SBC_USE_FIXED_POINT == XA_ENABLED

/****************************************************************************
 *
 * Fixed point macros
 *
 ****************************************************************************/

/* Some useful contants */
#define ONE_F       (0x00008000)
#define ONE_F_P     (ONE_F << 15) /* For higher precision calculation */

/* Signed 16 to REAL */
#define S16toReal(x) ((REAL)(((S32)(x)) << 15))

/* Unsigned 16 to REAL */
#define U16toReal(x) ((REAL)(((U32)(x)) << 15))

/* Clip a positive signed number */
#define ClipPos(x) (((x) > 0x3FFF8000) ? 0x3FFF8000 : (x))

/* Clip a negative signed number */
#define ClipNeg(x) (((x) < 0xC0000000) ? 0xC0000000 : (x))

/* Clip a value to largest or smallest 16 bit signed value */
#define Clip(x) (((x) < 0) ? ClipNeg(x) : ClipPos(x))

/* Clip a positive signed number (decoder only) */
#define ClipPosD(x) (((x) > 0x1FFFC000) ? 0x1FFFC000: (x))
 
/* Clip a negative signed number (decoder only) */
#define ClipNegD(x) (((x) < 0xE0000000) ? 0xE0000000 : (x))
 
/* Clip a value to largest or smallest 16 bit signed value (decoder only) */
#define ClipD(x) (((x) < 0) ? ClipNegD(x) : ClipPosD(x))
 
/* REAL to signed 16 bit value */
//#define RealtoS16(x) ((S16)((REAL)(ClipD(x)) >> 14))


#define RealtoS16(x) SATURATE32((x>>14),16)

static inline S32 SATURATE32(S32 ARG1, S32 ARG2)
{
  S32 __RES, __ARG1 = (ARG1); 
  ARG2=16;
  asm volatile ("ssat %0, %1, %2" : "=r" (__RES) :  "I" (ARG2), "r" (__ARG1) ); 
  return(__RES);
}


/* REAL to unsigned 16 bit value */
#define RealtoU16(x) ((U16)((REAL)(Clip(x)) >> 15))

/* REAL to signed 32-bit value */
#define RealtoS32(x) ((S32)((REAL)(x) >> 15))

#if SBC_USE_FIXED_MACROS == XA_ENABLED

#ifndef Mul
/* Macros for multiplying 17:15 by 17:15 (high word of b == 0) */
#define MulHiLo(a,b) ((((a) >> 16) * ((b) & 0x0000FFFF)) << 1)
#define MulLoLo(a,b) ((((a) & 0x0000FFFF) * ((b) & 0x0000FFFF)) >> 15)
/* Multiply a REAL by a REAL */
#define Mul(x,y) (REAL)(MulHiLo(x,y) + MulLoLo(x,y))
#endif

#ifndef MulP
/* Macros for multiplying 17:15 by 2:30 */
#if SBC_USE_HIRES_MACROS == XA_ENABLED
#define MulPHiHi(a,b) ((((a) >> 15) * ((b) >> 15)))
#define MulPHiLo(a,b) ((((a) >> 15) * ((b) & 0x00007FFF)) >> 15)
#define MulPLoHi(a,b) ((((a) & 0x00007FFF) * ((b) >> 15)) >> 15)
/* Multiply a REAL by a REAL with high precision */
#define MulP(x,y) (REAL)(MulPHiHi(x,y) + MulPHiLo(x,y) + MulPLoHi(x,y))
#else
/* Multiply a REAL by a REAL with low precision */
#define MulP(x,y) (REAL)((((x) >> 13) * ((y) >> 17)))
#endif
#endif

#ifndef MulPI
/* Macros that assume the first parameter is an integer */
#if SBC_USE_HIRES_MACROS == XA_ENABLED
#define MulPIHiHi(a,b) (((a) * ((b) >> 15)))
#define MulPIHiLo(a,b) (((a) * ((b) & 0x00007FFF)) >> 15)
/* Multiply an S16 by a REAL with high precision */
#define MulPI(x,y) (REAL)(MulPIHiHi(x,y) + MulPIHiLo(x,y))
#else
/* Multiply an S16 by a REAL with low precision */
#define MulPI(x,y) (REAL)(((x) * ((y) >> 12)) >> 3)
#endif
#endif

#ifndef dMulP
/* Decoder only macros for multiplying 17:15 by 2:30 */
#if SBC_USE_HIRES_MACROS == XA_ENABLED
/* Multiply a REAL by a REAL with high precision */
#define dMulP(x,y) (REAL)((((x) >> 13) * (y)))
#else
/* Multiply a REAL by a REAL with low precision */
#define dMulP(x,y) (REAL)((((x) >> 13) * (y)))
#endif
#endif

#else /* SBC_USE_FIXED_MACROS == XA_ENABLED */

#include "..\profiles\a2dp\sbc_math.c"

#endif /* SBC_USE_FIXED_MACROS == XA_ENABLED */

#else /* SBC_USE_FIXED_POINT == XA_ENABLED */

/****************************************************************************
 *
 * Floating point macros
 *
 ****************************************************************************/

/* Some useful contants */
#define ONE_F      (REAL)(1.0)
#define ONE_F_P    (REAL)(1.0)

/* Signed 16 to REAL */
#define S16toReal(x) ((REAL)((S32)(x)))

/* Clip a positive signed number (decoder only) */
#define ClipPos(x) (((x) > 32767) ? 32767: (x))
 
/* Clip a negative signed number (decoder only) */
#define ClipNeg(x) (((x) < -32767) ? -32767 : (x))
 
/* Clip a value to largest or smallest 16 bit signed value (decoder only) */
#define Clip(x) (((x) < 0) ? ClipNeg(x) : ClipPos(x))
 
/* REAL to signed 16 bit value */
#define RealtoS16(x) ((S16)((REAL)(Clip(x*2))))

/* REAL to unsigned 16 bit value */
#define RealtoU16(x) ((U16)(Clip(x)))

/* REAL to signed 32-bit value */
#define RealtoS32(x) ((S32)(x))

/* Multiply a REAL by a REAL  */
#define Mul(x,y) ((REAL)(x)*(REAL)(y))

/* Multiply a REAL by a REAL with high precision */
#define MulP(x,y) ((REAL)(x)*(REAL)(y))

/* Multiply a signed integer by a REAL with high precision */
#define MulPI(x,y) ((REAL)(x)*(REAL)(y))

/* Multiply a REAL by a REAL with high precision (decoder only) */
#define dMulP(x,y) ((REAL)(x)*(REAL)(y))

#endif /* SBC_USE_FIXED_POINT == XA_ENABLED */

#endif /* _SBC_MATH_H_ */

