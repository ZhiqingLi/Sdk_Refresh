/*
 * Copyright (c) 1996-2006 MIPS Technologies, Inc.
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
 * asm.h: various macros to help assembly language writers
 */

/* ABI specific stack frame layout and manipulation. */
#if defined(_ABIO32) && ! __mips64
/* Standard O32 */
#define SZREG		4	/* saved register size */
#define	REG_S		sw	/* store saved register */
#define	REG_L		lw	/* load saved register */
#define SZARG		4	/* argument register size */
#define	NARGSAVE	4	/* arg register space on caller stack */
#define ALSZ		7	/* stack alignment - 1 */
#define ALMASK		(~7)	/* stack alignment mask */
#define LOG2_STACK_ALGN	3	/* log2(8) */
#define SZPTR		4	/* pointer size */
#define LOG2_SZPTR	2	/* log2(4) */
#define PTR_S		sw	/* store pointer */
#define PTR_L		lw	/* load pointer */
#define PTR_SUBU	subu	/* decrement pointer */
#define PTR_ADDU	addu	/* increment pointer */
#define PTR		.word	/* pointer type pseudo */                  
#elif defined(_ABIO32) && __mips64
/* Algorithmics O32+64-bit */
#define SZREG		8	/* saved register size */		   
#define	REG_S		sd	/* store saved register */		   
#define	REG_L		ld	/* load saved register */		   
#define SZARG		4	/* argument register size */		   
#define	NARGSAVE	4	/* arg register space on caller stack */   
#define ALSZ		7	/* stack alignment - 1 */		   
#define ALMASK		(~7)	/* stack alignment mask */		   
#define LOG2_STACK_ALGN	3	/* log2(8) */
#define SZPTR		4	/* pointer size */			   
#define LOG2_SZPTR	2	/* log2(4) */
#define PTR_S		sw	/* store pointer */			   
#define PTR_L		lw	/* load pointer */			   
#define PTR_SUBU	subu	/* decrement pointer */			   
#define PTR_ADDU	addu	/* increment pointer */			   
#define PTR		.word	/* pointer type pseudo */                  
#elif defined(_ABIO64)
/* Cygnus O64 */
#define SZREG		8	/* saved register size */		   
#define	REG_S		sd	/* store saved register */		   
#define	REG_L		ld	/* load saved register */		   
#define SZARG		8	/* argument register size */		   
#define	NARGSAVE	4	/* arg register space on caller stack */   
#define ALSZ		7	/* stack alignment - 1 */		   
#define ALMASK		(~7)	/* stack alignment mask */		   
#define LOG2_STACK_ALGN	3	/* log2(8) */
#define SZPTR		4	/* pointer size */			   
#define LOG2_SZPTR	2	/* log2(4) */
#define PTR_S		sw	/* store pointer */			   
#define PTR_L		lw	/* load pointer */			   
#define PTR_SUBU	subu	/* decrement pointer */			   
#define PTR_ADDU	addu	/* increment pointer */			   
#define PTR		.word	/* pointer type pseudo */                  
#elif defined (_ABIN32)
/* Standard N32 */
#define SZREG		8	/* saved register size */		   
#define	REG_S		sd	/* store saved register */		   
#define	REG_L		ld	/* load saved register */		   
#define SZARG		8	/* argument register size */		   
#define	NARGSAVE	0	/* arg register space on caller stack */   
#define ALSZ		15	/* stack alignment - 1 */		   
#define ALMASK		(~15)	/* stack alignment mask */		   
#define LOG2_STACK_ALGN	4	/* log2(16) */
#define SZPTR		4	/* pointer size */			   
#define LOG2_SZPTR	2	/* log2(4) */
#define PTR_S		sw	/* store pointer */			   
#define PTR_L		lw	/* load pointer */			   
#define PTR_SUBU	subu	/* decrement pointer (SGI uses sub) */
#define PTR_ADDU	addu	/* increment pointer (SGI uses add) */
#define PTR		.word	/* pointer type pseudo */                  
#elif defined(_ABI64)
/* Standard N64 */
#define SZREG		8	/* saved register size */		   
#define	REG_S		sd	/* store saved register */		   
#define	REG_L		ld	/* load saved register */		   
#define SZARG		8	/* argument register size */		   
#define	NARGSAVE	0	/* arg register space on caller stack */   
#define ALSZ		15	/* stack alignment - 1 */		   
#define ALMASK		(~15)	/* stack alignment mask */		   
#define LOG2_STACK_ALGN	4	/* log2(16) */
#define SZPTR		8	/* pointer size */			   
#define LOG2_SZPTR	3	/* log2(8) */
#define PTR_S		sd	/* store pointer */			   
#define PTR_L		ld	/* load pointer */			   
#define PTR_SUBU	dsubu	/* decrement pointer */			   
#define PTR_ADDU	daddu	/* increment pointer */			   
#define PTR		.dword	/* pointer type pseudo */                  
#else
#error Unknown ABI
#endif

/* Concatenate two names. */
#ifdef __STDC__
# define _ASMCONCAT(A, B) A ## B
#else
# define _ASMCONCAT(A, B) A/**/B
#endif

/* Name of default code section. */
#ifndef _TEXT_SECTION
# define _TEXT_SECTION .section .text, "ax", @progbits
#endif

#ifndef _TEXT_SECTION_NAMED
# ifdef _FUNCTION_SECTIONS_
#  define _TEXT_SECTION_NAMED(name) .pushsection .text ##.name, "ax", @progbits
# else
#  define _TEXT_SECTION_NAMED(name) .pushsection .text, "ax", @progbits
# endif
#endif

/*
 * Leaf functions declarations.
 */

/* Global leaf function. */
#define LEAF(name) 			\
  	_TEXT_SECTION_NAMED(name);	\
  	.globl	name; 			\
  	.ent	name; 			\
name:

/* Static/Local leaf function. */
#define SLEAF(name) 			\
  	_TEXT_SECTION_NAMED(name);	\
  	.ent	name; 			\
name:

/* Weak leaf function. */
#define WLEAF(name) 			\
  	_TEXT_SECTION_NAMED(name);	\
  	.weakext name; 			\
  	.ent	name; 			\
name:

/* Weak alias leaf function. */
#define ALEAF(name,alias) 		\
  	_TEXT_SECTION_NAMED(name);	\
  	.weakext alias,name; 		\
  	.ent	name; 			\
name:

/*
 * Alternative function entrypoints.
 */

/* Global alternative entrypoint. */
#define AENT(name) 			\
  	.globl	name; 			\
  	.aent	name; 			\
name:
#define XLEAF(name)	AENT(name)

/* Local/static alternative entrypoint. */
#define SAENT(name) 			\
  	.aent	name; 			\
name:
#define SXLEAF(name)	SAENT(name)


/*
 * Leaf functions declarations.
 */

/* Global nested function. */
#define NESTED(name, framesz, rareg)	\
  	_TEXT_SECTION_NAMED(name);	\
  	.globl	name; 			\
  	.ent	name; 			\
	.frame	sp, framesz, rareg;	\
name:

/* Static/Local nested function. */
#define SNESTED(name, framesz, rareg)	\
  	_TEXT_SECTION_NAMED(name);	\
  	.ent	name; 			\
	.frame	sp, framesz, rareg;	\
name:

/* Weak nested function. */
#define WNESTED(name, framesz, rareg)	\
  	_TEXT_SECTION_NAMED(name);	\
  	.weakext name; 			\
  	.ent	name; 			\
	.frame	sp, framesz, rareg;	\
name:

/* Weak alias nested function. */
#define ANESTED(name, alias, framesz, rareg) \
  	_TEXT_SECTION_NAMED(name);	\
    	.weakext alias, name;		\
  	.ent	name; 			\
	.frame	sp, framesz, rareg;	\
name:

/*
 * Function termination
 */
#define END(name) 			\
  	.size name,.-name; 		\
  	.end	name;			\
	.popsection

#define SEND(name)	END(name)
#define WEND(name)	END(name)
#define AEND(name,alias) END(name)

/*
 * Global data declaration.
 */
#define EXPORT(name) \
  	.globl name; \
  	.type name,@object; \
name:

/*
 * Global data declaration with size.
 */
#define EXPORTS(name,sz) 		\
  	.globl name; 			\
  	.type name,@object; 		\
  	.size name,sz; 			\
name:

/*
 * Weak data declaration with size.
 */
#define WEXPORT(name,sz) 		\
  	.weakext name; 			\
  	.type name,@object; 		\
  	.size name,sz; 			\
name:

/*
 * Global data reference with size.
 */
#define	IMPORT(name, size) 		\
	.extern	name,size

/*
 * Global zeroed data.
 */
#define BSS(name,size) 			\
  	.type name,@object; 		\
	.comm	name,size

/*
 * Local zeroed data.
 */
#define LBSS(name,size) 		\
  	.lcomm	name,size

/*
 * Insert call to _mcount if profiling.
 */
#ifdef __PROFILING__
#define _MCOUNT 			\
	.set push; 			\
	.set noat; 			\
	move	$1,$31; 		\
	jal	_mcount; 		\
  	.set pop
#else
#define _MCOUNT
#endif
