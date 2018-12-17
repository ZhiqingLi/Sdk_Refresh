/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：ucos需要移植的文件之一，包括类型定义、栈增长方向和临界区保护宏定义
 * 作者：liminxian
 ********************************************************************************/


#ifndef __OS_CPU_H__
#define __OS_CPU_H__

#include <attr.h>

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT  extern
#endif

#define UVIEW_TYPE_USER_DEF 0x52455355

#ifndef _ASSEMBLER_
typedef enum {
	/*! 应用程序数据类型*/
	PROCESS_CREATE = 0x0,
	MUTEX_CREATE = 0x10,
	MUTEX_DELETE,
	MUTEX_LOCK,
	MUTEX_UNLOCK,
	SEM_CREATE = 0x20,
	SEM_DELETE,
	SEM_WAIT_UVIEW,
	SEM_TIMEDWAIT,
	SEM_POST_UVIEW,
	COND_INIT = 0x30,
	COND_DELETE,
	COND_WAIT,
	COND_TIMEDWAIT,
	COND_BRODCAST,
	COND_SIGNAL,
	MQ_CREATE = 0x40,
	MQ_DELETE,
	MQ_WAIT,
	MQ_TIMEDWAIT,
	MQ_SEND_UVIEW,
	MQ_TIMEDSEND,
	/*! 内核模块数据类型*/
	T_CREATE = 0x80,
	T_DELETE,
	T_PRIO_CHANGE,
	T_RESUME,
	T_SUSPEND,
	T_SCHED,
	S_CREATE = 0x90,
	S_DELETE,
	S_PEND,
	S_POST,
	Q_CREATE = 0xa0,
	Q_DELETE,
	Q_PEND,
	Q_POST,
	F_CREATE = 0xb0,
	F_DELETE,
	F_PEND,
	F_POST,
	/*! 内核模块的其它事件类型*/
	PM_CHANGE = 0x100,
	SCH_INT,
	SCH_TIMEOUT,
	SCH_UNLOCK,
	SCH_PTHEAD_ACTIVE,
	/* user defined */
	USER_DEFINED = UVIEW_TYPE_USER_DEF,
	/* 总的开关 */
	UVIEW_WHOLE_SWITCH = 0x7fffffff
} uview_type_e;

/*
 *********************************************************************************************************
 *                                              DATA TYPES
 *                                         (Compiler Specific)
 *********************************************************************************************************
 */
#include <ucos/types.h>
/*
 *********************************************************************************************************
 *                              MIPS
 *
 * Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts
 *             will be enabled even if they were disabled before entering the critical section.
 *
 * Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In other words, if
 *             interrupts were disabled before entering the critical section, they will be disabled when
 *             leaving the critical section.
 *
 * Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
 *             would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
 *             disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
 *             disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
 *             into the CPU's status register.
 *
 * Note     :  In this MIPS Port: Only Method #3 is implemeted.
 *
 *********************************************************************************************************
 */
/* Don't modify these lines. This port can only support OS_CRITICAL_METHOD 3. */
#define  OS_CRITICAL_METHOD    3

#if      OS_CRITICAL_METHOD == 1
#define  OS_ENTER_CRITICAL()  (cpu_sr = os_cpu_save_sr())    /* Disable interrupts                        */
#define  OS_EXIT_CRITICAL()   (os_cpu_store_sr(cpu_sr))    /* Restore  interrupts                       */
#endif

#if      OS_CRITICAL_METHOD == 3
#define  OS_INT_SAVE_AREA      unsigned int  cpu_sr;
#define  OS_ENTER_CRITICAL()  ( cpu_sr = os_cpu_save_sr() )    /* Disable interrupts                        */
#define  OS_EXIT_CRITICAL()   ( os_cpu_store_sr(cpu_sr) )    /* Restore  interrupts                       */
#endif

/*
 *********************************************************************************************************
 *                           ARM Miscellaneous
 *********************************************************************************************************
 */

#define  OS_STK_GROWTH        1                       /* Stack grows from HIGH to LOW memory on 80x86  */

#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
extern unsigned int os_cpu_save_sr(void) __FAR__;
extern void os_cpu_store_sr(unsigned int cpu_sr) __FAR__;
#endif

#endif

#endif /*__OS_CPU_H__*/
