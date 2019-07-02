/*
  ******************************************************************************
  * @file    asm32s003_types_local.h
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/

 
/* Define to prevent recursive inclusion -------------------------------------*/ 
#ifndef _asm32s003_types_local_H
#define _asm32s003_types_local_H

/**************************************************************************
STANDARD DEFINES
**************************************************************************/
#define NIL             '\000'

#define TRUE            1
#define FALSE           0
#define YES             1
#define NO              0
#define ON              1
#define OFF             0
#define GOOD            1
#define BAD             0

#define BELL            7        /*   Ring the bell          */
#define MAX_SINT16      32767
#define CPNULL          ((char *)0)
#define NONENTRY        -1.0E12
#define DPIE            (DOUBLE)3.141592653589793

/* other stuff... */
#define STRNCPY(a,b,c)          strncpy (a,b,c); (*((a)+(c)) = 0x00);


/**************************************************************************
STANDARD TYPEDEFS

The ANSI C std defines:
        short <= int <= long
        char >= 8 bits
        short >= 16 bits
        long >= 32 bits
        (from Harbison & Steele, "C, A Ref. Manual" 3rd ed. p. 99)

        so all ANSI C compliant compilers will accept the following.
**************************************************************************/
#ifndef CSP_TYPES_H
#define CSP_TYPES_H


/* Signed Types                                                              */
typedef signed char     S8_T;
typedef short           S16_T;
typedef long            S32_T;

/* Unsigned Types                                                            */
typedef unsigned char   	U8_T;
typedef unsigned short  	U16_T;
typedef unsigned long   	U32_T;
typedef unsigned long long 	U64_T;

/* Float Types                                                               */
typedef float           F32_T;
typedef double          F64_T;

/* Boolean types declared as U8_T, as enums are generated as 16 bit          */
typedef U8_T            B_T;

/* Definitions for the two members of the Boolean type                       */
#ifndef FALSE
#define FALSE ((B_T) 0)
#endif

#ifndef TRUE
#define TRUE  ((B_T) 1)
#endif

/* UNUSED Definition for unused Interrupt numbers * and unused PDC channels  */
/* in the CHIP structure. (cf. CSP.C file)                                   */
#ifndef UNUSED
#define UNUSED ((U8_T) 0xFF)
#endif

/* NULL definition */
#ifndef NULL
#define NULL  0
#endif

typedef enum {ENABLE = 1, DISABLE = !ENABLE} ClockStatus, FunctionalStatus;
typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;
typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

/******************************************************************************
* Peripherals Type
******************************************************************************/
typedef volatile U32_T CSP_REGISTER_T; 
typedef volatile U16_T CSP_REGISTER16_T;
typedef volatile U8_T  CSP_REGISTER8_T;

#endif   /* CSP_TYPE_H */

/* define 8 bit types */
typedef unsigned char           UINT8;
typedef signed char             SINT8;

/* define 16 bit types */
typedef unsigned short          UINT16;
typedef signed short            SINT16;

/* define 32 bit types */
typedef unsigned long           UINT32;
typedef signed long             SINT32;

typedef void                    VOID;
typedef signed char             CHAR;   /* be careful of EOF!!! (EOF = -1) */
typedef unsigned char           BOOL;
typedef signed long             TIME_T;

typedef float                   SINGLE;
#ifdef  DOUBLE
#undef  DOUBLE
#endif
typedef double                  DOUBLE;

typedef struct
{
	  unsigned bit0 : 1;
	  unsigned bit1 : 1;
	  unsigned bit2 : 1;
	  unsigned bit3 : 1;
	  unsigned bit4 : 1;
	  unsigned bit5 : 1;
	  unsigned bit6 : 1;
	  unsigned bit7 : 1;
} REG8;

typedef struct
{
	  unsigned bit0 : 1;
	  unsigned bit1 : 1;
	  unsigned bit2 : 1;
	  unsigned bit3 : 1;
	  unsigned bit4 : 1;
	  unsigned bit5 : 1;
	  unsigned bit6 : 1;
	  unsigned bit7 : 1;
	  unsigned bit8 : 1;
	  unsigned bit9 : 1;
	  unsigned bit10: 1;
	  unsigned bit11: 1;
	  unsigned bit12: 1;
	  unsigned bit13: 1;
	  unsigned bit14: 1;
	  unsigned bit15: 1;
} REG16;



/**************************************************************************
STANDARD STRING TYPEDEFS
**************************************************************************/
typedef char    STRING_3[4];
typedef char    STRING_5[6];
typedef char    STRING_8[9];
typedef char    STRING_10[11];
typedef char    STRING_12[13];
typedef char    STRING_16[17];
typedef char    STRING_24[25];
typedef char    STRING_30[31];
typedef char    STRING_32[33];
typedef char    STRING_48[49];
typedef char    STRING_50[51];
typedef char    STRING_60[61];
typedef char    STRING_80[81];
typedef char    STRING_132[133];
typedef char    STRING_256[257];
typedef char    STRING_512[513];


/********************************************/
/* STANDARD SYSTEM SIZES                                        */
/********************************************/
#define SIZE_UINT8              (size_t)(sizeof (UINT8 ))
#define SIZE_SINT8              (size_t)(sizeof (SINT8 ))

#define SIZE_UINT16             (size_t)(sizeof (UINT16))
#define SIZE_SINT16             (size_t)(sizeof (SINT16))

#define SIZE_UINT32             (size_t)(sizeof (UINT32))
#define SIZE_SINT32             (size_t)(sizeof (SINT32))

#define SIZE_VOID               (size_t)(sizeof (VOID  ))
#define SIZE_CHAR               (size_t)(sizeof (CHAR  ))
#define SIZE_BOOL               (size_t)(sizeof (BOOL  ))
#define SIZE_TIME_T             (size_t)(sizeof (TIME_T))

#define SIZE_SINGLE             (size_t)(sizeof (SINGLE))
#define SIZE_DOUBLE             (size_t)(sizeof (DOUBLE))

#define SIZE_STRING_3           (size_t)(sizeof (STRING_3  ))
#define SIZE_STRING_5           (size_t)(sizeof (STRING_5  ))
#define SIZE_STRING_8           (size_t)(sizeof (STRING_8  ))
#define SIZE_STRING_10          (size_t)(sizeof (STRING_10 ))
#define SIZE_STRING_12          (size_t)(sizeof (STRING_12 ))
#define SIZE_STRING_16          (size_t)(sizeof (STRING_16 ))
#define SIZE_STRING_24          (size_t)(sizeof (STRING_24 ))
#define SIZE_STRING_30          (size_t)(sizeof (STRING_30 ))
#define SIZE_STRING_32          (size_t)(sizeof (STRING_32 ))
#define SIZE_STRING_48          (size_t)(sizeof (STRING_48 ))
#define SIZE_STRING_50          (size_t)(sizeof (STRING_50 ))
#define SIZE_STRING_60          (size_t)(sizeof (STRING_60 ))
#define SIZE_STRING_80          (size_t)(sizeof (STRING_80 ))
#define SIZE_STRING_132         (size_t)(sizeof (STRING_132))
#define SIZE_STRING_256         (size_t)(sizeof (STRING_256))
#define SIZE_STRING_512         (size_t)(sizeof (STRING_512))


/**************************************************************************
STANDARD BIT MANIPULATIONS
**************************************************************************/
#define SETBIT( target, bit )   ((target) |= (1u << (bit)))
#define CLRBIT( target, bit )   ((target) &= ~(1u << (bit)))
#define TOGBIT( target, bit )   ((target) ^= (1u << (bit)))

#define ISBITSET( target, bit ) (!!((target) & (1u << (bit))))
#define ISBITCLR( target, bit ) ( !((target) & (1u << (bit))))


#endif		/**< asm32s003_types_local_H */

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/