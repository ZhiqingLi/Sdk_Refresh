
#ifndef _APP_EXCEPTION_H_
#define _APP_EXCEPTION_H_

#ifdef DEBUG_BA22_EXCEPTION
extern void instruction_bus_error(void);
extern void aligment_exception(void);
extern void illegal_insn_exception(void);
extern void trap_exception(void);
extern void timer_exception(void);
extern void external_interrupt_exception(void);
extern void syscall_exception(void);
extern void floating_point_exception(void);
extern void instruction_page_fault(void);
extern void data_page_fault(void);
extern void instruction_tlb_miss_vector(void);
extern void data_tlb_miss_vector(void);
#endif

#endif // _APP_EXCEPTION_H_
