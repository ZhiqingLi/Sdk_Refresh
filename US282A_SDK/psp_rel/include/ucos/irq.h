/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：中断处理相关宏定义、数据结构和函数声明
 * 作者：liminxian
 ********************************************************************************/
 
#ifndef __IRQ_H__
#define __IRQ_H__

#include <typeext.h>
#include <asm-mips/mach-gl5116/irq.h>
#include <asm-mips/exception.h>
#include <os_cpu.h>
#include <attr.h>

#define SA_SHIRQ        0x04000000
#define IRQ_DISABLED    2   /* IRQ disabled - do not enter! */

#define IRQ_HANDLED     (1)
#define IRQ_NONE            (0)

#define OFFSET_OS_SYSCALL_API       (0x268)
#define OFFSET_OS_SYSCALL_STATUS    (0x26c)

#define OS_START 0xbfc00000
#define ABS_OS_SYSCALL_API          (OS_START|OFFSET_OS_SYSCALL_API)
#define ABS_OS_SYSCALL_STATUS       (OS_START|OFFSET_OS_SYSCALL_STATUS)

#ifndef _ASSEMBLER_

#include <asm-mips/interrupt.h>

typedef int irqreturn_t;

typedef void (*interrupt_handle_t)(void);

extern interrupt_handle_t interrupt_handle[NR_IRQS];

#endif /*_ASSEMBLER_*/

#endif /*__IRQ_H__*/
