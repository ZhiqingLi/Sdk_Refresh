#ifndef __BASIC_OP__
#define __BASIC_OP__

#if 0
#define CHAR_BIT      8         /* number of bits in a char */
#define SCHAR_MIN   (-128)      /* minimum signed char value */
#define SCHAR_MAX     127       /* maximum signed char value */
#define UCHAR_MAX     0xff      /* maximum unsigned char value */
#define MB_LEN_MAX    5             /* max. # bytes in multibyte char */
#define SHRT_MIN    (-32768)        /* minimum (signed) short value */
#define SHRT_MAX      32767         /* maximum (signed) short value */
#define USHRT_MAX     0xffff        /* maximum unsigned short value */
#define INT_MIN     (-2147483647 - 1) /* minimum (signed) int value */
#define INT_MAX       2147483647    /* maximum (signed) int value */
#define UINT_MAX      0xffffffff    /* maximum unsigned int value */
#define LONG_MIN    (-2147483647L - 1) /* minimum (signed) long value */
#define LONG_MAX      2147483647L   /* maximum (signed) long value */
#define ULONG_MAX     0xffffffffUL  /* maximum unsigned long value */
#endif

typedef signed char Word8;
#define minWord8  SCHAR_MIN
#define maxWord8  SCHAR_MAX

typedef unsigned char UWord8;
#define minUWord8 0
#define maxUWord8 UCHAR_MAX

typedef short Word16;
#define minWord16     SHRT_MIN
#define maxWord16     SHRT_MAX
typedef unsigned short UWord16;
#define minUWord16    0
#define maxUWord16    USHRT_MAX

typedef int Word32;
#define minWord32     INT_MIN
#define maxWord32     INT_MAX
typedef unsigned int UWord32;
#define minUWord32    0
#define maxUWord32    UINT_MAX

#define MAX_32 (Word32)0x7fffffffL
#define MIN_32 (Word32)0x80000000L

#define MAX_16 (Word16)0x7fff
#define MIN_16 (Word16)0x8000

extern Word32 Overflow;
extern Word32 Carry;
/*___________________________________________________________________________
 |                                                                           |
 |   Prototypes for basic arithmetic operators                               |
 |___________________________________________________________________________|
*/

Word16 add (Word16 var1, Word16 var2);    /* Short add,           1   */
Word16 sub (Word16 var1, Word16 var2);    /* Short sub,           1   */
Word16 abs_s (Word16 var1);               /* Short abs,           1   */
Word16 shl (Word16 var1, Word16 var2);    /* Short shift left,    1   */
Word16 shr (Word16 var1, Word16 var2);    /* Short shift right,   1   */
Word16 mult (Word16 var1, Word16 var2);   /* Short mult,          1   */
Word32 L_mult (Word16 var1, Word16 var2); /* Long mult,           1   */
Word16 negate (Word16 var1);              /* Short negate,        1   */
Word16 extract_h (Word32 L_var1);         /* Extract high,        1   */
Word16 extract_l (Word32 L_var1);         /* Extract low,         1   */
Word16 L_round (Word32 L_var1);             /* Round,               1   */
Word32 L_mac (Word32 L_var3, Word16 var1, Word16 var2);   /* Mac,  1  */
Word32 L_msu (Word32 L_var3, Word16 var1, Word16 var2);   /* Msu,  1  */
Word32 L_macNs (Word32 L_var3, Word16 var1, Word16 var2); /* Mac without
                                                             sat, 1   */
Word32 L_msuNs (Word32 L_var3, Word16 var1, Word16 var2); /* Msu without
                                                             sat, 1   */
Word32 L_add (Word32 L_var1, Word32 L_var2);    /* Long add,        2 */
Word32 L_sub (Word32 L_var1, Word32 L_var2);    /* Long sub,        2 */
Word32 L_add_c (Word32 L_var1, Word32 L_var2);  /* Long add with c, 2 */
Word32 L_sub_c (Word32 L_var1, Word32 L_var2);  /* Long sub with c, 2 */
Word32 L_negate (Word32 L_var1);                /* Long negate,     2 */
Word16 mult_r (Word16 var1, Word16 var2);       /* Mult with round, 2 */
Word32 L_shl (Word32 L_var1, Word16 var2);      /* Long shift left, 2 */
Word32 L_shr (Word32 L_var1, Word16 var2);      /* Long shift right, 2*/
Word16 shr_r (Word16 var1, Word16 var2);        /* Shift right with
                                                   round, 2           */
Word16 mac_r (Word32 L_var3, Word16 var1, Word16 var2); /* Mac with
                                                           rounding,2 */
Word16 msu_r (Word32 L_var3, Word16 var1, Word16 var2); /* Msu with
                                                           rounding,2 */
Word32 L_deposit_h (Word16 var1);        /* 16 bit var1 -> MSB,     2 */
Word32 L_deposit_l (Word16 var1);        /* 16 bit var1 -> LSB,     2 */

Word32 L_shr_r (Word32 L_var1, Word16 var2); /* Long shift right with
                                                round,  3             */
Word32 L_abs (Word32 L_var1);            /* Long abs,              3  */
Word32 L_sat (Word32 L_var1);            /* Long saturation,       4  */
Word16 norm_s (Word16 var1);             /* Short norm,           15  */
Word16 div_s (Word16 var1, Word16 var2); /* Short division,       18  */
Word16 norm_l (Word32 L_var1);           /* Long norm,            30  */   

void L_Extract (Word32 L_32, Word16 *hi, Word16 *lo);
Word32 L_Comp (Word16 hi, Word16 lo);
Word32 Mpy_32 (Word16 hi1, Word16 lo1, Word16 hi2, Word16 lo2);
Word32 Mpy_32_16 (Word16 hi, Word16 lo, Word16 n);
Word32 Div_32 (Word32 L_num, Word16 denom_hi, Word16 denom_lo);
#endif