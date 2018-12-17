/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：解决流水线冒险相关头文件
 * 作者：liminxian
 ********************************************************************************/
 
#ifndef __ASM_HAZARDS_H__
#define __ASM_HAZARDS_H__

#include <asm-mips/config.h>

#ifndef PC
#ifdef _ASSEMBLER_

	.macro	_ssnop
	sll	$0, $0, 1
	.endm

	.macro	_ehb
	sll	$0, $0, 3
	.endm


#ifdef CONFIG_CPU_MIPSR2
/*
 * Use a macro for ehb unless explicit support for MIPSR2 is enabled
 */

#define irq_enable_hazard
	_ehb

#define irq_disable_hazard
	_ehb

#else

/*
 * Classic MIPS needs 1 - 3 nops or ssnops
 */
#define irq_enable_hazard
#define irq_disable_hazard						\
	_ssnop; _ssnop; _ssnop

#endif

#else /* _ASSEMBLER_ */
/*c embadded assembler*/
__asm__(
	"	.macro	_ssnop					\n\t"
	"	sll	$0, $2, 1				\n\t"
	"	.endm						\n\t"
	"							\n\t"
	"	.macro	_ehb					\n\t"
	"	sll	$0, $0, 3				\n\t"
	"	.endm						\n\t");


#ifdef CONFIG_CPU_MIPSR2

__asm__(
	"	.macro\tirq_enable_hazard			\n\t"
	"	_ehb						\n\t"
	"	.endm						\n\t"
	"							\n\t"
	"	.macro\tirq_disable_hazard			\n\t"
	"	_ehb						\n\t"
	"	.endm						\n\t"
	"							\n\t"
	"	.macro\tback_to_back_c0_hazard			\n\t"
	"	_ehb						\n\t"
	"	.endm");

#define irq_enable_hazard()						\
	__asm__ __volatile__(						\
	"irq_enable_hazard")

#define irq_disable_hazard()						\
	__asm__ __volatile__(						\
	"irq_disable_hazard")

#define back_to_back_c0_hazard()					\
	__asm__ __volatile__(						\
	"back_to_back_c0_hazard")

#else

/*
 * Default for classic MIPS processors.  Assume worst case hazards but don't
 * care about the irq_enable_hazard - sooner or later the hardware will
 * enable it and we don't care when exactly.
 */

__asm__(
	"	#						\n\t"
	"	# There is a hazard but we do not care		\n\t"
	"	#						\n\t"
	"	.macro\tirq_enable_hazard			\n\t"
	"	.endm						\n\t"
	"							\n\t"
	"	.macro\tirq_disable_hazard			\n\t"
	"	_ssnop; _ssnop; _ssnop				\n\t"
	"	.endm");

#define irq_enable_hazard()	do { } while (0)
#define irq_disable_hazard()						\
	__asm__ __volatile__(						\
	"irq_disable_hazard")

#define back_to_back_c0_hazard()					\
	__asm__ __volatile__(						\
	"	.set noreorder				\n"		\
	"	nop; nop; nop				\n"		\
	"	.set reorder				\n")

#endif

#ifdef CONFIG_CPU_MIPSR2
#define instruction_hazard()						\
do {									\
__label__ __next;							\
	__asm__ __volatile__(						\
	"	jr.hb	%0					\n"	\
	:								\
	: "r" (&&__next));						\
__next:									\
	;								\
} while (0)

#else
#define instruction_hazard() do { } while (0)
#endif

#endif /* _ASSEMBLER_ */

#endif /* __ASM_HAZARDS_H__ */
#endif /* __ASM_HAZARDS_H__ */
