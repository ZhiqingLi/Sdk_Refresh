/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：异常处理栈桢相关宏定义和数据结构
 * 作者：liminxian
 ********************************************************************************/
 
#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__
/*
 * important register numbers
 */

#define REG_EPC			37
#define REG_FP			72
#define REG_SP			29

/*
 * Stack layout for the INT exception handler
 * Derived from the stack layout described in asm-mips/stackframe.h
 *
 * The first PTRSIZE*6 bytes are argument save space for C subroutines.
 */

#define PT_R0		(PTRSIZE*6)			/* 0 */
#define PT_R1		((PT_R0) + LONGSIZE)	/* 1 */
#define PT_R2		((PT_R1) + LONGSIZE)	/* 2 */
#define PT_R3		((PT_R2) + LONGSIZE)	/* 3 */
#define PT_R4		((PT_R3) + LONGSIZE)	/* 4 */
#define PT_R5		((PT_R4) + LONGSIZE)	/* 5 */
#define PT_R6		((PT_R5) + LONGSIZE)	/* 6 */
#define PT_R7		((PT_R6) + LONGSIZE)	/* 7 */
#define PT_R8		((PT_R7) + LONGSIZE)	/* 8 */
#define PT_R9		((PT_R8) + LONGSIZE)	/* 9 */
#define PT_R10		((PT_R9) + LONGSIZE)	/* 10 */
#define PT_R11		((PT_R10) + LONGSIZE)	/* 11 */
#define PT_R12		((PT_R11) + LONGSIZE)	/* 12 */
#define PT_R13		((PT_R12) + LONGSIZE)	/* 13 */
#define PT_R14		((PT_R13) + LONGSIZE)	/* 14 */
#define PT_R15		((PT_R14) + LONGSIZE)	/* 15 */
#define PT_R16		((PT_R15) + LONGSIZE)	/* 16 */
#define PT_R17		((PT_R16) + LONGSIZE)	/* 17 */
#define PT_R18		((PT_R17) + LONGSIZE)	/* 18 */
#define PT_R19		((PT_R18) + LONGSIZE)	/* 19 */
#define PT_R20		((PT_R19) + LONGSIZE)	/* 20 */
#define PT_R21		((PT_R20) + LONGSIZE)	/* 21 */
#define PT_R22		((PT_R21) + LONGSIZE)	/* 22 */
#define PT_R23		((PT_R22) + LONGSIZE)	/* 23 */
#define PT_R24		((PT_R23) + LONGSIZE)	/* 24 */
#define PT_R25		((PT_R24) + LONGSIZE)	/* 25 */
#define PT_R26		((PT_R25) + LONGSIZE)	/* 26 */
#define PT_R27		((PT_R26) + LONGSIZE)	/* 27 */
#define PT_R28		((PT_R27) + LONGSIZE)	/* 28 */
#define PT_R29		((PT_R28) + LONGSIZE)	/* 29 */
#define PT_R30		((PT_R29) + LONGSIZE)	/* 30 */
#define PT_R31		((PT_R30) + LONGSIZE)	/* 31 */

/*
 * Saved special registers
 */
#define PT_STATUS	((PT_R31) + LONGSIZE)	/* 32 */
#define PT_HI		((PT_STATUS) + LONGSIZE)	/* 33 */
#define PT_LO		((PT_HI) + LONGSIZE)	/* 34 */
#define PT_BADVADDR	((PT_LO) + LONGSIZE)	/* 35 */
#define PT_CAUSE	((PT_BADVADDR) + LONGSIZE)	/* 36 */
#define PT_EPC		((PT_CAUSE) + LONGSIZE)	/* 37 */

#define PT_SIZE		((((PT_EPC) + LONGSIZE) + (PTRSIZE-1)) & ~(PTRSIZE-1))

#ifndef _ASSEMBLER_

typedef struct pt_regs {
	/* Pad bytes for argument save space on the stack. */
	unsigned long pad0[6];
	/* Saved main processor registers. */
	unsigned long regs[32];
	/* Saved special registers. */
	unsigned long cp0_status;
	unsigned long hi;	
	unsigned long lo;
	unsigned long cp0_badvaddr;
	unsigned long cp0_cause;
	unsigned long cp0_epc;
}pt_regs_t;

//错误类型定义

//堆栈溢出
#define EXCEPTION_STK_OVERFLOW                (1)

//SYSCALL调用异常
#define EXCEPTION_SYSCALL_ERROR               (2)

//中断处理异常
#define EXCEPTION_IRQ_HANDLE_ERROR            (3)

//缺页处理异常
#define EXCEPTION_PAGEMISS_ERROR              (4)

//数据存储异常
#define EXCEPTION_DATA_STORE_ERROR            (5)

//数据读取异常
#define EXCEPTION_DATA_FETCH_ERROR            (6)

//动态内存申请异常
#define EXCEPTION_MALLOC_ERROR                (7)

//内存保护异常
#define EXCEPTION_MPU_ERROR                   (8)

//cache error
#define EXCEPTION_CACHE_ERROR                 (9)

//用户自定义的异常
#define EXCEPTION_USER_DEFINE_ERROR           (32)


extern void default_exception_server(void);
extern int set_except_vector(int n, void *addr);
extern void handle_exception(void);
extern void  handle_exception_fix(uint32 err_code);

#endif /* !_ASSEMBLER_ */
#endif /* __EXCEPTION_H__ */
