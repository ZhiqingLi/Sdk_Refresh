##==============================================================================
##
##  crt0.S
##
##  startup code
##
##==============================================================================

#include <nds32_init.inc>
#include "interrupt.h"

##------------------------------------------------------------------------------
	.macro vector name
	.align 2
	j OS_Trap_\name
	.endm

	.macro OS_Trap_Interrupt_HW num
    OS_Trap_Interrupt_HW\num:
	push $r0
	li  $r0, \num
	b   common_ISR_wrapper
	.endm

	.section .vector, "ax"
!========================================================================
! Vector table
!========================================================================
	.align 2
exception_vector:
_start:
	j ___start                    !  (0) Trap Reset
	vector TLB_Fill             !  (1) Trap TLB fill0000
	vector PTE_Not_Present      !  (2) Trap PTE not present
	vector TLB_Misc             !  (3) Trap TLB misc
	vector TLB_VLPT_Miss        !  (4) Trap TLB VLPT miss
	vector Machine_Error        !  (5) Trap Machine error
	vector Debug_Related        !  (6) Trap Debug related
	vector General_Exception    !  (7) Trap General exception
	vector Syscall              !  (8) Syscall

	/* HW interrupt vector layout */
	hal_hw_vectors
exception_vector_end:

.section .driver.isr, "ax"
	.align  2
exception_handler:
	.long tlb_exception_handler
	.long error_exception_handler
	.long syscall_handler
ISR_TABLE:
	.long Timer1Interrupt  //0
	.long WakeupInterrupt  //1
	.long GpioInterrupt    //2
	.long RtcInterrupt     //3
	.long WatchDogInterrupt//4
	.long OS_Trap_Interrupt_SWI //5
	.long I2C_Interrupt    //6
	.long UART0_Interrupt  //7
	.long Timer2Interrupt  //8
	.long DMA0_Interrupt   //9
	.long DMA1_Interrupt   //10
	.long DMA2_Interrupt   //11
	.long DMA3_Interrupt   //12
	.long DMA4_Interrupt   //13
	.long DMA5_Interrupt   //14
	.long DMA6_Interrupt   //15
	.long DMA7_Interrupt   //16
	.long UART1_Interrupt  //17
	.long UART2_Interrupt  //18
	.long I2sInterrupt     //19
	.long Timer3Interrupt  //20
	.long Timer4Interrupt  //21
	.long Timer5Interrupt  //22
	.long Timer6Interrupt  //23
	.long Timer7Interrupt  //24
	.long Timer8Interrupt  //24
	.long SDIO_Interrupt   //26
	.long UsbInterrupt     //27
	.long SPIM0_Interrupt  //28
	.long SPIM1_Interrupt  //29
	.long SPIS_Interrupt   //30
	.long FDMAInterrupt    //31
/*
 * exception handlers
 */

/*----------------------------------------------------------------------
   Since N903 doesn't have MMU, make them share the common error handler.
----------------------------------------------------------------------*/
OS_Trap_TLB_Fill:
/*
	SAVE_ALL
	li  $r0, #0x1
	b   tlb_exception_handler
*/

OS_Trap_PTE_Not_Present:
/*
	SAVE_ALL
	li  $r0, #0x2
	b   tlb_exception_handler
*/

OS_Trap_TLB_Misc:
/*
	SAVE_ALL
	li  $r0, #0x3
	b   tlb_exception_handler
*/

OS_Trap_TLB_VLPT_Miss:
1:  b   1b
	SAVE_ALL
	li  $r0, #0x4
	b   tlb_exception_handler

OS_Trap_Machine_Error:
1:  b   1b
	SAVE_ALL
	li  $r0, #0x5
	b   error_exception_handler

OS_Trap_Debug_Related:
1:  b   1b
	SAVE_ALL
	li  $r0, #0x6
	b   error_exception_handler

OS_Trap_General_Exception:
//1:  b   1b
	SAVE_ALL
	li  $r0, #0x7
	b   error_exception_handler

OS_Trap_Syscall:
	SYSCALL_SAVE_ALL
	bal syscall_handler
	SYSCALL_RESTORE_ALL
	iret

common_ISR_wrapper:
	SAVE_ALL_HW
	la  $r1, ISR_TABLE
	lw  $r1, [$r1+$r0<<2]
	jral $r1
	RESTORE_ALL_HW
	iret

OS_Trap_Interrupt_UNDEF:
	push $r0
	li  $r0, 0xff
	b   common_ISR_wrapper

OS_Trap_Int_Comm:
	SAVE_ALL_HW
	la  $r1, ISR_TABLE
	lw  $r1, [$r1+$r0<<2]
	jral $r1
	RESTORE_ALL_HW
	iret

! Interrupt wrapper function
OS_Trap_Interrupt_HW0:
	push	$r0
	li	$r0, 0x0
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW1:
	push	$r0
	li	$r0, 0x01
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW2:
	push	$r0
	li	$r0, 0x02
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW3:
	push	$r0
	li	$r0, 0x03
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW4:
	push	$r0
	li	$r0, 0x04
	j	OS_Trap_Int_Comm

/*
OS_Trap_Interrupt_HW5:
	push	$r0
	li	$r0, 0x05
	j	OS_Trap_Int_Comm
	*/


OS_Trap_Interrupt_HW5:
	pushm	$r0, $r1
	j	OS_Trap_Interrupt_SWI


OS_Trap_Interrupt_HW6:
	push	$r0
	li	$r0, 0x06
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW7:
	push	$r0
	li	$r0, 0x07
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW8:
	push	$r0
	li	$r0, 0x08
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW9:
	push	$r0
	li	$r0, 0x09
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW10:
	push	$r0
	li	$r0, 0x0a
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW11:
	push	$r0
	li	$r0, 0x0b
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW12:
	push	$r0
	li	$r0, 0x0c
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW13:
	push	$r0
	li	$r0, 0x0d
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW14:
	push	$r0
	li	$r0, 0x0e
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW15:
	push	$r0
	li	$r0, 0x0f
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW16:
	push	$r0
	li	$r0, 0x10
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW17:
	push	$r0
	li	$r0, 0x11
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW18:
	push	$r0
	li	$r0, 0x12
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW19:
	push	$r0
	li	$r0, 0x13
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW20:
	push	$r0
	li	$r0, 0x14
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW21:
	push	$r0
	li	$r0, 0x15
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW22:
	push	$r0
	li	$r0, 0x16
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW23:
	push	$r0
	li	$r0, 0x17
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW24:
	push	$r0
	li	$r0, 0x18
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW25:
	push	$r0
	li	$r0, 0x19
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW26:
	push	$r0
	li	$r0, 0x1a
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW27:
	push	$r0
	li	$r0, 0x1b
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW28:
	push	$r0
	li	$r0, 0x1c
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW29:
	push	$r0
	li	$r0, 0x1d
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW30:
	push	$r0
	li	$r0, 0x1e
	j	OS_Trap_Int_Comm

OS_Trap_Interrupt_HW31:
	push	$r0
	li	$r0, 0x1f
	j	OS_Trap_Int_Comm

##------------------------------------------------------------------------------
## Startup code

	.section .text
	.global _start
	.weak _call_exit
	.weak _SDA_BASE_
	.func _start
	.type _start, @function

	.align 2
___start:
	!************************** Begin of do-not-modify **************************
	! Please don't modify this code
	! Initialize the registers used by the compiler

	nds32_init		! NDS32 startup initial macro in <nds32_init.inc>
	!*************************** End of do-not-modify ***************************
	#bal _nds32_init_mem
	bal __init

	bal main
1:  b   1b

	.size _start, .-___start

	.end
