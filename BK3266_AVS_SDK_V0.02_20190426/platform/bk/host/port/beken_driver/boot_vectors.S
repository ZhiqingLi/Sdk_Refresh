/*
 * This module contains basic exception handlers for all BA22
 * exceptions.
 * (By default) these are located at 0x008, 0x010, ..., 0x070.
 * Each handler has 8 bytes available. If your handler is longer,
 * you need to use subroutines. You can use HANDLE_EXCEPTION macro
 * to call C subroutine that does the useful part of exception handling.
 *
 * Linker script used with this example reserves 0x1000 bytes for
 * vectors section, so there is some free space after trap handler
 * (0xe00+) and before reset handler (0x0..0xff).
 */

#include "excutil.h"
#include "spr_defs.h"
        .global _check_update_finished
//        .global _handler_entry
        .global  watchdog_reset
#define UNHANDLED_EXCEPTION_RESET                    \
        b.jal  watchdog_reset;                                       \
        b.rfe

/**
 * This macro can be used for exceptions, you don't handle.
 * It will terminate simulation. On HW it just returns from exception.
 * It would probably be nice to have some diagnostic printed or sth.
 * This should never be used in production code.
 */

        .global _check_update_finished
        .global _handler_entry

        .section .boot_vectors, "ax"

/* ---[ 0x008: Reset exception ]----------------------------------------- */
        .org  0x008 /* 000 */

        /* As this code goes directly into vectors section,
         * it needs to be smaller than 0x100 bytes.
         * You can of course have larger initialization in text
         * section and just jump there. */
        /* set up stack pointer
         * and make sure it's 8-byte aligned */

        b.j _reset_f

#define DEBUG_BA22_EXCEPTION
#ifdef DEBUG_BA22_EXCEPTION
/* ---[ 0x010: Instruction Bus Error exception ]------------------------- */
        .org 0x010
        UNHANDLED_EXCEPTION_RESET

/* ---[ 0x018: Aligment exception ]-------------------------------------- */
        .org 0x018
        UNHANDLED_EXCEPTION_RESET

/* ---[ 0x020: Illegal insn exception ]---------------------------------- */
        .org 0x020
        UNHANDLED_EXCEPTION_RESET

/* ---[ 0x028: Trap exception ]------------------------------------------ */
        .org 0x028
        UNHANDLED_EXCEPTION_RESET

/* ---[ 0x030: Timer exception ]----------------------------------------- */
        .org 0x030
       	UNHANDLED_EXCEPTION_RESET

/* ---[ 0x038: External interrupt exception ]---------------------------- */
        .org 0x038
		UNHANDLED_EXCEPTION_RESET

/* ---[ 0x040: Syscall exception ]--------------------------------------- */
        .org 0x040
        UNHANDLED_EXCEPTION_RESET

/* ---[ 0x048: Floating point exception ]-------------------------------- */
        .org 0x048
        UNHANDLED_EXCEPTION_RESET

/* ---[ 0x060: Instruction Page Fault exception ]------------------------ */
        .org 0x060
        UNHANDLED_EXCEPTION_RESET

/* ---[ 0x068: Data Page Fault exception ]-------------------------------- */
        .org 0x068
        UNHANDLED_EXCEPTION_RESET

/* ---[ 0x070: Intruction TLB miss Vector exception ]----------------- */
        .org 0x070
        UNHANDLED_EXCEPTION_RESET

/* ---[ 0x078: Data TLB miss Vector exception ]---------------------- */
        .org 0x078
        UNHANDLED_EXCEPTION_RESET
#else
/* ---[ 0x010: Instruction Bus Error exception ]------------------------- */
        .org 0x010
        UNHANDLED_EXCEPTION

/* ---[ 0x018: Aligment exception ]-------------------------------------- */
        .org 0x018
        UNHANDLED_EXCEPTION

/* ---[ 0x020: Illegal insn exception ]---------------------------------- */
        .org 0x020
        UNHANDLED_EXCEPTION

/* ---[ 0x028: Trap exception ]------------------------------------------ */
        .org 0x028
        UNHANDLED_EXCEPTION

/* ---[ 0x030: Timer exception ]----------------------------------------- */
        .org 0x030
        UNHANDLED_EXCEPTION

/* ---[ 0x038: External interrupt exception ]---------------------------- */
        .org 0x038
        UNHANDLED_EXCEPTION

/* ---[ 0x040: Syscall exception ]--------------------------------------- */
        .org 0x040
        UNHANDLED_EXCEPTION

/* ---[ 0x048: Floating point exception ]-------------------------------- */
        .org 0x048
        UNHANDLED_EXCEPTION

/* ---[ 0x060: Instruction Page Fault exception ]------------------------ */
        .org 0x060
        UNHANDLED_EXCEPTION

/* ---[ 0x068: Data Page Fault exception ]-------------------------------- */
        .org 0x068
        UNHANDLED_EXCEPTION

/* ---[ 0x070: Intruction TLB miss Vector exception ]----------------- */
        .org 0x070
        UNHANDLED_EXCEPTION

/* ---[ 0x078: Data TLB miss Vector exception ]---------------------- */
        .org 0x078
        UNHANDLED_EXCEPTION
#endif
/* ---[ 0x080: TLV Table Base Address ]---------------------- */
        .org 0x080   
/* Only uart download successfully,write password 'BK3266',don't care flash crc check :) */   
        .org 0x100

        .word 0x32334B42
    //    .word 0x01203636 // 4M
        .word 0x00203636   // 8M
        .word 0xffffffff
        .word 0xffffffff  // software version X.2.1.1,X means which is half of flash,'00' is upper half,and '01' is lower half
_reset_f:
        b.ori r1,r0,_stack-4
        b.andi r1,r1,~7
        b.jal _check_update_finished



        
