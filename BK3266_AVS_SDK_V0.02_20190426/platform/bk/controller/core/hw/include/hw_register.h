#ifndef _PARTHUS_HW_REGISTER_
#define _PARTHUS_HW_REGISTER_

/******************************************************************************
 * MODULE NAME:    hw_register.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Register and Bit definitions.
 * MAINTAINER:     John Nelson
 * CREATION DATE:  01 May 1999
 *
 * SOURCE CONTROL: $Id: hw_register.h,v 1.20 2014/03/13 12:39:03 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    01.May.1999    -  jn        - initial version
 ******************************************************************************/

#include <limits.h>

/* Common Constants                                                        */
#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000

#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000L
#define BIT24 0x01000000L
#define BIT25 0x02000000L
#define BIT26 0x04000000L
#define BIT27 0x08000000L
#define BIT28 0x10000000L
#define BIT29 0x20000000L
#define BIT30 0x40000000L
#define BIT31 0x80000000UL

/*
 * Cache Register Operations: Create, Load, Store, Clear, Get
 *
 * A Cache Register can only be created for a
 * corresponding addressed register.  Note that operations on
 * Cache Register fields are checked against the definitions
 * to ensure that the fields are appropriate for this register.
 * This provides a compile time sanity check that we don't try
 * to work with fields in invalid registers.
 *
 * e.g. to define a register and an associated field.
 * #define COM_CTRL_REG_20     (0x2000000+0x20)
 * #define DWH_INIT_MASK    0x001f0000
 * #define DWH_INIT_MASK_COM_CTRL_REG DWH_INIT_MASK
 */

/*
 * Endian-ness impartial.
 */
#define mHWreg_Get_Cache_Register(Register) \
        Cache_##Register

#define mHWreg_Assert_Cache_Bit(Register, BitField) \
       { Cache_##Register |= (Register##_##BitField##_MASK); }

#define mHWreg_Clear_Cache_Bit(Register, BitField) \
       { Cache_##Register &= ~(Register##_##BitField##_MASK); }

#define mHWreg_Assert_Cache_Bit_P(Register, BitField) \
       { (*Cache_##Register) |= (Register##_##BitField##_MASK); }

#define mHWreg_Clear_Cache_Bit_P(Register, BitField) \
       { (*Cache_##Register) &= ~(Register##_##BitField##_MASK); }

#define mHWreg_Toggle_Cache_Bit(Register, BitField) \
       { Cache_##Register ^= (Register##_##BitField##_MASK); }

#define mHWreg_Assign_Cache_Bit(Register, BitField, Value) \
{   /* Bit is set if Value is 1, cleared if 0 */           \
    if (Value)                                             \
        Cache_##Register |= (Register##_##BitField##_MASK); \
    else                                                 \
        Cache_##Register &= ~(Register##_##BitField##_MASK);\
}

#define mHWreg_If_Cache_Bit_is_1(Register, BitField) \
        ( Cache_##Register & (Register##_##BitField##_MASK) )

#define mHWreg_If_Cache_Bit_is_0(Register, BitField) \
       ( !(Cache_##Register & (Register##_##BitField##_MASK)) )

#define mHWreg_Assign_Cache_Field(Register, Field, Value) \
       { Cache_##Register = ((((Value) << (Field##_SHFT)) \
          & (Register##_##Field##_MASK)) \
          | (Cache_##Register & ~(Register##_##Field##_MASK))) ; }

#define mHWreg_Get_Cache_Field(Register, Field) \
      ( (Cache_##Register & (Register##_##Field##_MASK) ) >> (Field##_SHFT) )

#define mHWreg_Get_Cache_Bit(Register, BitField) \
      ( (Cache_##Register & (Register##_##BitField##_MASK) ) >> (BitField##_SHFT) )

/*
 * Endian-ness specific
 */
#if !(defined(HW_DATA32) && ((__BYTE_ORDER == __LITTLE_ENDIAN) || defined(CHIMERA)))
/*#error >>>> hw_register.h only supports matched Tabasco register endian-ness
*/
#endif

#define u_intREG u_int32

#if 0

#define mHWreg_Get_Register(Register) \
       ( *((u_intREG volatile*) (Register##_ADDR)) )

#define mHWreg_Assign_Register(Register, Value) \
       ( *((u_intREG volatile*) (Register##_ADDR)) = Value)


#define mHWreg_Load_Cache_Register(Register) \
       { \
         Cache_##Register = *((u_intREG volatile*) (Register##_ADDR)); }

#define mHWreg_Store_Cache_Register(Register) \
       {  \
         *((u_intREG volatile*) (Register##_ADDR)) = Cache_##Register ; }

#define mHWreg_Move_Cache_Register(Register, Destination_Register) \
       {  Destination_Register = Cache_##Register ; }

#define mHWreg_Logical_OR_With_Register(Register, Value) \
       ( *((u_intREG volatile*) (Register##_ADDR)) |= (u_intREG)Value)

#define mHWreg_Logical_AND_With_Register(Register, Value) \
       ( *((u_intREG volatile*) (Register##_ADDR)) &= (u_intREG)Value)

#else
#define mHWreg_Get_Register(Register) \
        mGetHWEntry32(Register)

#define mHWreg_Assign_Register(Register, Value_) \
        mSetHWEntry32(Register, (Value_))

#define mHWreg_Load_Cache_Register(Register) \
       { \
         Cache_##Register = mGetHWEntry32(Register); }

#define mHWreg_Store_Cache_Register(Register) \
       {  \
         mSetHWEntry32(Register, Cache_##Register); }

#define mHWreg_Move_Cache_Register(Register, Destination_Register) \
       {  Destination_Register = Cache_##Register ; }
/*
 * OR/AND operations (non optimised).
 */
#define mHWreg_Logical_OR_With_Register(Register, Value) \
       mSetHWEntry32(Register, \
                         ( mGetHWEntry32(Register) | ((u_intREG)(Value)) ) )

#define mHWreg_Logical_AND_With_Register(Register, Value) \
       mSetHWEntry32(Register, \
                         ( mGetHWEntry32(Register) & ((u_intREG)(Value)) ) )

#endif


#define mHWreg_Create_Cache_Register(Register) \
        u_intREG Cache_##Register=0

#define mHWreg_Assign_Cache_Register(Register, Value) \
       ( Cache_##Register = Value )

#define mHWreg_Clear_Cache_Register(Register) \
       { Cache_##Register = 0; }

#define mHWreg_Move_Cache_Register(Register, Destination_Register) \
       {  Destination_Register = Cache_##Register ; }

/*
 * Assign Register by reference requires that _ADDR postfix be removed
 * since mSetHWEntry32 always postfix 1st argument with _ADDR
 */
#define mHWreg_Assign_Register_Ref(Register, Value_)        \
        { u_intREG volatile *Register##_ADDR = (u_intREG volatile *)Register; \
          mSetHWEntry32(Register, (Value_)); }

/*
 * Should you be wondering why all the 'BitField##_MASK_##Register'
 * The approach below won't work as the Register field will
 * be expanded before the REG_MASK call!

    #define REG_MASK(Register, Field) (Field##_MASK_##Register)
    #define mHWreg_Assert_Cache_Bit_Alt(Register, BitField) \
       { Cache_##Register |= REG_MASK(Register, BitField); }
 */

/*
 * Reverse Bits
 * Note: implicit cast message if n is narrower than mask
 */
//<limits.h>: CHAR_BIT – size of the char type in bits (at least 8 bits)

#define mHWreg_Reverse_Bits(n)                   \
{ u_int32 mask; int i;                    \
    for(i=0; i < sizeof(n)*CHAR_BIT/2; i++)                \
    {                                               \
       mask = (u_int32) ((1UL << i) | (1UL << (sizeof(n)*CHAR_BIT-1-i))); \
       if ( !((n&mask)==0 || (n&mask)==mask))       \
           n ^= mask;                               \
    }                                               \
}

#endif
