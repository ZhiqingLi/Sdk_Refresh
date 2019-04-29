#ifndef _PARTHUS_HW_MACRO_DEFS_
#define _PARTHUS_HW_MACRO_DEFS_

/*
 * MODULE NAME:    hw_macro_defs.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Hardware Macro Definitions
 * MAINTAINER:     John Nelson
 * DATE:           1 Jun 1999
 *
 * SOURCE CONTROL: $Id: hw_macro_defs.h,v 1.14 2014/04/14 13:42:40 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    1 June 1999 -   jn       - Initial Version V0.9
 *
 */

/*
 * HW_DATA16 or HW_DATA32  Selected by Build Process -- Makefile #
 * HW matchable to processor endianess.  No current need for SW SWAP_ENDIAN.
 */
#if !(__BYTE_ORDER == __BIG_ENDIAN) && !(__BYTE_ORDER == __LITTLE_ENDIAN)
    #error PROCESSOR ENDIANESS (DATA BYTE ORDERING) NOT SPECIFIED
#endif

#if (!defined(HW_DATA16) && !defined(HW_DATA32))
    #error HARDWARE DATA TYPE NOT SPECIFIED: i.e. #DEFINE HW_DATA16/HW_DATA32
#endif


#define SWAP_ENDIAN16(Value) ((((Value) & 0x00FF) << 8) | \
                              (((Value) & 0xFF00) >> 8))

#ifndef SWAP_ENDIAN32
#define SWAP_ENDIAN32(Value) ((((Value) & 0x000000FF) << 24) | \
                              (((Value) & 0x0000FF00) << 8) | \
                              (((Value) & 0x00FF0000) >> 8) | \
                              (((Value) & 0xFF000000) >> 24))
#endif

#if defined(HW_DATA16)

    /*
     * All addresses and shifts are presented based on a 32 bit memory map
     * All 16 bit or less registers are within a 16 bit word.
     * The basis of the 16 bit adjustments are that if the shift is > 16 then
     * value is in upper word of 32 bit register. Hence the address will be
     * offset by +2, the shift reduced by 16 and the mask left shifted by 16!
     */

    #define u_intHW                     u_int16
    #define SWAP_ENDIAN(Value)          SWAP_ENDIAN16(Value)

    #ifndef HW_OPPOSITE_ENDIANESS

        #define ADDR(Field)      ( (u_intHW volatile *) \
           ((Field##_ADDR) + ((Field##_SHFT) < 16 ? (0) : (2)) ) )

    #else
        #define ADDR(Field)      ( (u_intHW volatile *) \
           ((Field##_ADDR) + ((Field##_SHFT) < 16 ? (2) : (0)) ) )

    #endif

    #define MASK(Field)        \
       ((Field##_SHFT) < 16 ? (Field##_MASK) : ((Field##_MASK) >> 16) )
    #define SHFT(Field)        \
       ((Field##_SHFT) < 16 ? (Field##_SHFT) : ((Field##_SHFT) - 16 ) )

    #define ADDR16(Field)               ADDR(Field)
    #define SHFT16(Field)               SHFT(Field)
    #define MASK16(Field)               MASK(Field)


#elif defined(HW_DATA32)

    #define u_intHW                     u_int32
    #define SWAP_ENDIAN(Value)          SWAP_ENDIAN32(Value)

    #define ADDR(Field)     ((u_intHW volatile *) (Field##_ADDR  ))
    #define MASK(Field)     (Field##_MASK)
    #define SHFT(Field)     (Field##_SHFT)

    #define ADDR16(Field)      ( (u_intHW volatile *) \
       (Field##_ADDR + device_addr + ((Field##_SHFT) < 16 ? (0) : (2)) ) )
    #define MASK16(Field)        \
       ((Field##_SHFT) < 16 ? (Field##_MASK) : ((Field##_MASK) >> 16) )
    #define SHFT16(Field)     \
       ((Field##_SHFT) < 16 ? (Field##_SHFT) : ((Field##_SHFT) - 16 ) )
#endif


#ifdef TEAKLITE4_IO_BB_PORT

#include "hw_hal_macro_defs.h" // hal defined macros

#else

/*
 *  Common Macros for 16 and 32 bit data bus
 */
#ifndef HW_OPPOSITE_ENDIANESS

    #define mGetHWEntry(Field) \
      ( ( *ADDR(Field) & (MASK(Field)) ) >> SHFT(Field) )

    /*
     * Note the single SHFT before the MASK (HW code difference)
     */
    #define mSetHWEntry(Field, Value) \
       { u_intHW volatile *addr = ADDR(Field); \
         *addr = ((((Value) << SHFT(Field)) \
                    & MASK(Field)) | (*addr & ~MASK(Field))) ; }

    #define mTestHWBit(Field) ( ( *ADDR(Field) & MASK(Field)) != 0 )

    #define mSetHWBit(Field) \
       { u_intHW volatile *addr = ADDR(Field); \
         *addr |= MASK(Field); }

    #define mClearHWBit(Field) \
       { u_intHW volatile *addr = ADDR(Field); \
         *addr &= (~MASK(Field) ); }

    #define mToggleHWBit(Field) \
       { u_intHW volatile *addr = ADDR(Field); \
         *addr ^= MASK(Field); }

#else /* Not tested */

    #define mGetHWEntry(Field) \
      ( ( SWAP_ENDIAN(*((u_intHW volatile*)ADDR(Field))) & (MASK(Field)) ) >> SHFT(Field) )

    /*
     * Note the single SHFT before the MASK (HW code difference)
     */
    #define mSetHWEntry(Field, Value) \
       { u_intHW volatile *addr = (u_intHW volatile*)ADDR(Field); \
         *addr = SWAP_ENDIAN(((((u_intHW)Value) << SHFT(Field)) \
                    & MASK(Field)) | (SWAP_ENDIAN(*addr) & ~MASK(Field))) ; }

    #define mTestHWBit(Field)  \
       ( ( SWAP_ENDIAN(*(u_intHW volatile *)ADDR(Field)) & MASK(Field)) != 0 )

    #define mSetHWBit(Field) \
       { u_intHW volatile *addr = (u_intHW volatile *)ADDR(Field); \
         *addr |= SWAP_ENDIAN(MASK(Field)); }

    #define mClearHWBit(Field) \
       { u_intHW volatile *addr = (u_intHW volatile *)ADDR(Field); \
         *addr &= SWAP_ENDIAN((~MASK(Field)) ); }

    #define mToggleHWBit(Field) \
       { u_intHW volatile *addr = (u_intHW volatile *)ADDR(Field); \
         *addr ^= SWAP_ENDIAN(MASK(Field)); }

#endif

/*
 * 32 bit specific macros
 */
#if defined(HW_DATA32)

    #ifndef HW_OPPOSITE_ENDIANESS

        #define mGetHWEntry32(Field) ( *ADDR(Field) )
        #define mSetHWEntry32(Field, Source) ( *ADDR(Field) = Source )

        #define mGetHWEntry64_Ex(Field, pDst) \
           { u_intHW volatile *pSrc = (u_intHW volatile*)ADDR(Field); \
            *(pDst) =  *pSrc; \
            *((pDst) + 1) =  *(++pSrc); }

        #define mSetHWEntry64(Field, pSrc) \
           { u_intHW volatile *pDst = (u_intHW volatile*)ADDR(Field); \
             *pDst = *((pSrc)); \
             *(++pDst) = *((pSrc) + 1); }

        #define mGetHWEntry128_Ex(Field, pDst) \
           { u_intHW volatile *pSrc = (u_intHW volatile*)ADDR(Field); \
            *(pDst) =  *pSrc; \
            *((pDst) + 1) =  *(++pSrc); \
            *((pDst) + 2) =  *(++pSrc); \
            *((pDst) + 3) =  *(++pSrc); }

        #define mSetHWEntry128(Field, pSrc) \
           { u_intHW volatile *pDst = (u_intHW volatile*)ADDR(Field); \
             *pDst = *((pSrc)); \
             *(++pDst) = *((pSrc) + 1); \
             *(++pDst) = *((pSrc) + 2); \
             *(++pDst) = *((pSrc) + 3); }

    #else  /* Not tested */

        #define mGetHWEntry32(Field) ( SWAP_ENDIAN32(*ADDR(Field)) )
        #define mSetHWEntry32(Field, Source) ( *ADDR(Field) = SWAP_ENDIAN32(Source) )

        #define mGetHWEntry64_Ex(Field, pDst) \
           { u_intHW volatile *pSrc = ADDR(Field); \
             *(pDst) =  SWAP_ENDIAN32(*(pSrc));         ++pSrc; \
             *((pDst + 1)) =  SWAP_ENDIAN32(*(pSrc)); }

        #define mSetHWEntry64(Field, pSrc) \
           { u_intHW volatile *pDst = (u_intHW volatile*)ADDR(Field); \
             *pDst = SWAP_ENDIAN32(*((pSrc)));          ++pDst; \
             *(pDst) = SWAP_ENDIAN32(*((pSrc) + 1)); }

        #define mGetHWEntry128_Ex(Field, pDest) \
           { u_intHW volatile *pSrc = (u_intHW volatile*)ADDR(Field); \
             u_intHW volatile *pDst = (u_intHW*) pDest; \

            *(pDst) =  SWAP_ENDIAN32(*(pSrc));          ++pSrc; \
            *((pDst) + 1) =  SWAP_ENDIAN32(*(pSrc));    ++pSrc; \
            *((pDst) + 2) =  SWAP_ENDIAN32(*(pSrc));    ++pSrc; \
            *((pDst) + 3) =  SWAP_ENDIAN32(*(pSrc)); }

        #define mSetHWEntry128(Field, pSource) \
           { u_intHW volatile *pDst = (u_intHW volatile*)ADDR(Field); \
             u_intHW volatile *pSrc = (u_intHW*) pSource; \

             *pDst = SWAP_ENDIAN32(*(pSrc)); \
             *(++pDst) = SWAP_ENDIAN32(*((pSrc) + 1)); \
             *(++pDst) = SWAP_ENDIAN32(*((pSrc) + 2)); \
             *(++pDst) = SWAP_ENDIAN32(*((pSrc) + 3)); }
    #endif

#elif defined(HW_DATA16)

    /*
     * 16 bit specific
     * Note hardware is matchable to endianess selected, so
     * endian-ness swap not necessary
     */

    #ifndef HW_OPPOSITE_ENDIANESS

        #define mGetHWEntry32(Field)                                        \
            (( (u_int32) ((*((u_intHW volatile *)(Field##_ADDR)))           \
                    /* & 0xFFFF */) ) |                                     \
            (( (u_int32)((*((u_intHW volatile *)(Field##_ADDR + 2)))        \
                    /* & 0xFFFF */) ) << 16))

        #define mSetHWEntry32(Field, Source)                                \
            { u_int32 Value = (u_int32) Source; /* May be an expression */  \
              *((u_intHW volatile *) (Field##_ADDR))    =                   \
                          (u_intHW) (Value /* & 0xFFFF */);                 \
              *((u_intHW volatile *) (Field##_ADDR + 2))=                   \
                          (u_intHW) (Value>>16); }

        #define mGetHWEntry64_Ex(Field, pDest)                              \
           { u_intHW volatile *pSrc = (u_intHW volatile*) (Field##_ADDR);   \
             u_intHW volatile *pDst = (u_intHW*) pDest;                     \
             *pDst = *pSrc;                                                 \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc); }

        #define mSetHWEntry64(Field, Source)                                \
           { u_intHW volatile *pDst = (u_intHW volatile*) (Field##_ADDR);   \
             u_intHW volatile *pSrc = (u_intHW*) Source;                    \
             *pDst = *pSrc;                                                 \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc); }

        #define mGetHWEntry128_Ex(Field, pDest)                             \
           { u_intHW volatile *pSrc = (u_intHW volatile*)(Field##_ADDR);    \
             u_intHW volatile *pDst = (u_intHW *) pDest;                    \
             * pDst  =  *pSrc;                                              \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc);                                         \
           }

        #define mSetHWEntry128(Field, pSource)                              \
           { u_intHW volatile *pDst = (u_intHW volatile*) (Field##_ADDR);   \
             u_intHW volatile *pSrc = (u_intHW*) pSource;                   \
             *pDst = *pSrc;                                                 \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc);                                         \
             *(++pDst) = *(++pSrc);                                         \
             }

    #else
        /*
         * Untested!
         */
        #define mGetHWEntry32(Field) \
            (((u_int32) ((SWAP_ENDIAN(*((u_intHW volatile *)(Field##_ADDR + device_addr)))) & 0xFFFF)) | \
            ((u_int32) ((SWAP_ENDIAN(*((u_intHW volatile *)(Field##_ADDR + device_addr + 2)))) << 16)))

        #define mSetHWEntry32(Field, Source) \
            { *((u_intHW volatile *) (Field##_ADDR + device_addr)) = (u_intHW) SWAP_ENDIAN(Source & 0xFFFF); \
              *((u_intHW volatile *) (Field##_ADDR + device_addr + 2)) = (u_intHW) SWAP_ENDIAN(Source>>16); }

        #define mGetHWEntry64_Ex(Field, pDest) \
           { u_intHW volatile *pSrc = (u_intHW*) (Field##_ADDR + device_addr); \
             u_intHW volatile *pDst = (u_intHW*) pDest; \
             \
             *(pDst +1) = SWAP_ENDIAN(*(pSrc)); \
             ++pDst; ++pSrc; \
             *(pDst -1) = SWAP_ENDIAN(*(pSrc)); \
             ++pDst; ++pSrc; \
             *(pDst +1) = SWAP_ENDIAN(*(pSrc)); \
             ++pDst; ++pSrc; \
             *(pDst -1) = SWAP_ENDIAN(*(pSrc)); }

        #define mSetHWEntry64(Field, Source) \
           { u_intHW volatile *pDst = (u_intHW*) (Field##_ADDR + device_addr); \
             u_intHW volatile *pSrc = (u_intHW*) Source; \
             \
             *(pDst +1) = SWAP_ENDIAN(*(pSrc)); \
             ++pDst; ++pSrc; \
             *(pDst -1) = SWAP_ENDIAN(*(pSrc)); \
             ++pDst; ++pSrc; \
             *(pDst +1) = SWAP_ENDIAN(*(pSrc)); \
             ++pDst; ++pSrc; \
             *(pDst -1) = SWAP_ENDIAN(*(pSrc)); }
    #endif

#endif

#endif

#endif   /* #ifndef _PARTHUS_HW_MACRO_DEFS_ */

