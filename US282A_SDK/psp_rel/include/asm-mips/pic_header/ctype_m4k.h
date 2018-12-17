/*
 * Copyright (c) 1996-2003 MIPS Technologies, Inc.
 * All rights reserved.
 * 
 * Unpublished rights (if any) reserved under the copyright laws of the
 * United States of America and other countries.
 * 
 * This code is proprietary to MIPS Technologies, Inc. ("MIPS
 * Technologies").  Any copying, reproducing, modifying or use of
 * this code (in whole or in part) that is not expressly permitted
 * in writing by MIPS Technologies or an authorized third party is
 * strictly prohibited.  At a minimum, this code is protected under
 * unfair competition and copyright laws.  Violations thereof may result
 * in criminal penalties and fines.
 * 
 * MIPS Technologies reserves the right to change this code to improve
 * function, design or otherwise.  MIPS Technologies does not assume any
 * liability arising out of the application or use of this code, or of
 * any error or omission in such code.  Any warranties, whether express,
 * statutory, implied or otherwise, including but not limited to the
 * implied warranties of merchantability or fitness for a particular
 * purpose, are excluded.  Except as expressly provided in any written
 * license agreement from MIPS Technologies or an authorized third party,
 * the furnishing of this code does not give recipient any license to
 * any intellectual property rights, including any patent rights, that
 * cover this code.
 * 
 * This code shall not be exported, reexported, transferred, or released,
 * directly or indirectly, in violation of the law of any country or
 * international law, regulation, treaty, Executive Order, statute,
 * amendments or supplements thereto.  Should a conflict arise regarding
 * the export, reexport, transfer, or release of this code, the laws of
 * the United States of America shall be the governing law.
 * 
 * This code constitutes one or more of the following: commercial
 * computer software, commercial computer software documentation or
 * other commercial items.  If the user of this code, or any related
 * documentation of any kind, including related technical data or
 * manuals, is an agency, department, or other entity of the United
 * States government ("Government"), the use, duplication, reproduction,
 * release, modification, disclosure, or transfer of this code, or
 * any related documentation of any kind, is restricted in accordance
 * with Federal Acquisition Regulation 12.212 for civilian agencies
 * and Defense Federal Acquisition Regulation Supplement 227.7202 for
 * military agencies.  The use of this code by the Government is further
 * restricted in accordance with the terms of the license agreement(s)
 * and/or applicable contract terms and conditions covering this code
 * from MIPS Technologies or an authorized third party.
 * 
 * 
 */

/*
 * ctype.h : ctype definitions
 */


#ifndef __CTYPE_H
#ifdef __cplusplus
extern "C" {
#endif
#define __CTYPE_H

/* --- Constants --- */
#define	_CT_U	0x01
#define	_CT_L	0x02
#define	_CT_N	0x04
#define	_CT_S	0x08
#define _CT_P	0x10
#define _CT_C	0x20
#define _CT_X	0x40
#define _CT_SP	0x80

/* --- Prototypes --- */
int isalnum	(int c);
int isalpha	(int c);
int isascii	(int c);
int iscntrl	(int c);
int isdigit	(int c);
int isgraph	(int c);
int islower	(int c);
int isprint	(int c);
int ispunct	(int c);
int isspace	(int c);
int isupper	(int c);
int isxdigit	(int c);
int toascii	(int c);
int _tolower	(int c);
int _toupper	(int c);
int tolower	(int c);
int toupper	(int c);

/* --- Storage --- */
extern const char _ctype[];

/* --- Macros --- */
#define isalnum(c)	((_ctype+1)[(unsigned)c] & (_CT_U|_CT_L|_CT_N))
#define	isalpha(c)	((_ctype+1)[(unsigned)c] & (_CT_U|_CT_L))
#define iscntrl(c)	((_ctype+1)[(unsigned)c] & (_CT_C))
#define	isdigit(c)	((_ctype+1)[(unsigned)c] & (_CT_N))
#define isgraph(c)	((_ctype+1)[(unsigned)c] & (_CT_U|_CT_L|_CT_N|_CT_P))
#define	islower(c)	((_ctype+1)[(unsigned)c] & (_CT_L))
#define isprint(c)	((_ctype+1)[(unsigned)c] & (_CT_U|_CT_L|_CT_N|_CT_P|_CT_SP))
#define ispunct(c)	((_ctype+1)[(unsigned)c] & (_CT_P))
#define	isspace(c)	((_ctype+1)[(unsigned)c] & (_CT_S|_CT_SP))
#define	isupper(c)	((_ctype+1)[(unsigned)c] & (_CT_U))
#define	isxdigit(c)	((_ctype+1)[(unsigned)c] & (_CT_N|_CT_X))

#define isascii(c)	((unsigned)(c) <= 0x7f)
#define toascii(c)	((c) & 0x7f)

#define _tolower(c)	((c) - 'A' + 'a')
#define _toupper(c)	((c) - 'a' + 'A')

#ifdef __cplusplus
}
#endif
#endif /* !defined __CTYPE_H */
