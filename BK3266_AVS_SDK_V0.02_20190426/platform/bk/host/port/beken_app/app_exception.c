#include "driver_beken_includes.h"
#include "app_beken_includes.h"

#ifdef DEBUG_BA22_EXCEPTION
#define PRINT_BA22_EXCEPTION        0
/* ---[ 0x010: Instruction Bus Error exception ]------------------------- */
void instruction_bus_error(void) {
#if (PRINT_BA22_EXCEPTION == 1)
    os_printf("instruction_bus_error\r\n");
	//BK3000_wdt_reset();
#else
    BK3000_WDT_CONFIG = 0x5A0001;
    BK3000_WDT_CONFIG = 0xA50001;
#endif
}

/* ---[ 0x018: Aligment exception ]-------------------------------------- */
void aligment_exception(void) {
#if (PRINT_BA22_EXCEPTION == 1)
    os_printf("aligment_exception:%p\r\n", get_spr(SPR_EPCR_BASE));
    os_printf("aligment_exception:%p\r\n", get_spr(SPR_JPC));
	//BK3000_wdt_reset();
#else
    BK3000_WDT_CONFIG = 0x5A0001;
    BK3000_WDT_CONFIG = 0xA50001;
#endif

}
/* ---[ 0x020: Illegal insn exception ]---------------------------------- */
void illegal_insn_exception(void) {
#if (PRINT_BA22_EXCEPTION == 1)
    os_printf("illegal_insn_exception:%p\r\n", get_spr(SPR_EPCR_BASE));
	//BK3000_wdt_reset();
#else
    BK3000_WDT_CONFIG = 0x5A0001;
    BK3000_WDT_CONFIG = 0xA50001;
#endif

}
/* ---[ 0x028: Trap exception ]------------------------------------------ */
void trap_exception(void) {
#if (PRINT_BA22_EXCEPTION == 1)
    os_printf("trap_exception:%p\r\n", get_spr(SPR_EPCR_BASE));
	//BK3000_wdt_reset();
#else
    BK3000_WDT_CONFIG = 0x5A0001;
    BK3000_WDT_CONFIG = 0xA50001;
#endif

}
/* ---[ 0x030: Timer exception ]----------------------------------------- */
void timer_exception(void) {
#if (PRINT_BA22_EXCEPTION == 1)
    os_printf("timer_exception\r\n");
	//BK3000_wdt_reset();
#else
    BK3000_WDT_CONFIG = 0x5A0001;
    BK3000_WDT_CONFIG = 0xA50001;
#endif

}
/* ---[ 0x038: External interrupt exception ]---------------------------- */
void external_interrupt_exception(void) {
#if (PRINT_BA22_EXCEPTION == 1)
    os_printf("external_interrupt_exception\r\n");
	//BK3000_wdt_reset();
#else
    BK3000_WDT_CONFIG = 0x5A0001;
    BK3000_WDT_CONFIG = 0xA50001;
#endif

}
/* ---[ 0x040: Syscall exception ]--------------------------------------- */
void syscall_exception(void) {
#if (PRINT_BA22_EXCEPTION == 1)
    os_printf("syscall_exception\r\n");
	//BK3000_wdt_reset();
#else
    BK3000_WDT_CONFIG = 0x5A0001;
    BK3000_WDT_CONFIG = 0xA50001;
#endif

}
/* ---[ 0x048: Floating point exception ]-------------------------------- */
void floating_point_exception(void) {
#if (PRINT_BA22_EXCEPTION == 1)
    os_printf("floating_point_exception\r\n");
	//BK3000_wdt_reset();
#else
    BK3000_WDT_CONFIG = 0x5A0001;
    BK3000_WDT_CONFIG = 0xA50001;
#endif

}
/* ---[ 0x060: Instruction Page Fault exception ]------------------------ */
void instruction_page_fault(void) {
#if (PRINT_BA22_EXCEPTION == 1)
    os_printf("instruction_page_fault\r\n");
	//BK3000_wdt_reset();
#else
    BK3000_WDT_CONFIG = 0x5A0001;
    BK3000_WDT_CONFIG = 0xA50001;
#endif

}
/* ---[ 0x068: Data Page Fault exception ]-------------------------------- */
void data_page_fault(void) {
#if (PRINT_BA22_EXCEPTION == 1)
    os_printf("data_page_fault\r\n");
	//BK3000_wdt_reset();
#else
    BK3000_WDT_CONFIG = 0x5A0001;
    BK3000_WDT_CONFIG = 0xA50001;
#endif

}
/* ---[ 0x070: Intruction TLB miss Vector exception ]----------------- */
void instruction_tlb_miss_vector(void) {
#if (PRINT_BA22_EXCEPTION == 1)
    os_printf("instruction_tlb_miss_vector\r\n");
	//BK3000_wdt_reset();
#else
    BK3000_WDT_CONFIG = 0x5A0001;
    BK3000_WDT_CONFIG = 0xA50001;
#endif

}
/* ---[ 0x078: Data TLB miss Vector exception ]---------------------- */
void data_tlb_miss_vector(void) {
#if (PRINT_BA22_EXCEPTION == 1)
    os_printf("data_tlb_miss_vector\r\n");
	//BK3000_wdt_reset();
#else
    BK3000_WDT_CONFIG = 0x5A0001;
    BK3000_WDT_CONFIG = 0xA50001;
#endif

}
#endif

// EOF
