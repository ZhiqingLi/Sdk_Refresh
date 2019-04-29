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
.section RAMCODE,"ax",%progbits
.globl TIME_InterruptHandler
.globl SDIO_InterruptHandler
.globl SADC_InterruptHandler
.globl AUD_InterruptHandler
.globl PWM0_InterruptHandler
.globl PWM1_InterruptHandler
.globl PWM2_InterruptHandler
.globl PWM3_InterruptHandler
.globl PWM4_InterruptHandler
.globl PWM5_InterruptHandler
.globl GPIO_InterruptHandler
.globl SPI_InterruptHandler
.globl SPI2_InterruptHandler
.globl I2C1_InterruptHandler
.globl I2C2_InterruptHandler
.globl UART_InterruptHandler
.globl FFT_InterruptHandler
.globl DMA_InterruptHandler
.globl CEVA_InterruptHandler
.globl USB_InterruptHandler
.globl IRDA_InterruptHandler

.globl BK3000_wdt_power_on
.globl BK3000_ICU_Initial

/* copy memory region
 *
 *   from:     src_beg
 *   to start:     dst_beg
 *   to end:     dst_end
 *
 * with registers t1,t2,t3,t4 as temporary
 */

#define COPY_MEMORY(src_beg,dst_beg,dst_end,t1,t2,t3,t4)    \
        b.ori   t1,r0,src_beg    ;\
        b.ori   t2,r0,dst_beg    ;\
        b.ori   t3,r0,dst_end    ;\
1:      b.bgeu  t2,t3,2f         ;\
        b.lwz   t4,0(t1)         ;\
        b.sw    0(t2),t4         ;\
        b.addi  t1,t1,4          ;\
        b.addi  t2,t2,4          ;\
        b.j     1b               ;\
2:


        .section .boot_magic_word, "ax"
        
        .word 0x01234567
        .word 0x89ABCDEF
        .word 0xAABBCCDD
        .word _software_mark_
        .word 0xFFFFFFFF
        .word 0xFFFFFFFF
        .word 0xFFFFFFFF
        .word 0xFFFFFFFF
        .word 0xFFFFFFFF
        .word 0xFFFFFFFF
        .word 0xFFFFFFFF
        .word 0xFFFFFFFF

        .section .boot_handlers, "ax"

_handler_entry:
        b.jal BK3000_wdt_power_on
#ifndef BA22_DE
        /* Enable caches (if present).
         * This can be removed if your CPU does not have
         * caches implemented. */
        b.jal _enable_dcache
        b.jal _enable_icache
#endif
        b.jal BK3000_ICU_Initial

_copy_data_to_ram:
    COPY_MEMORY(_data_flash_begin,_data_ram_begin,_data_ram_end,r3,r4,r5,r8)
    COPY_MEMORY(_sdata_flash_begin,_sdata_ram_begin,_sdata_ram_end,r3,r4,r5,r8)
    COPY_MEMORY(_iqmem_start_lma,_iqmem_start_vma,_iqmem_end_vma,r3,r4,r5,r8)
        /* clear BSS section */
        b.ori    r3,r0,_bss_start
        b.ori    r4,r0,_bss_end
        b.jal    _clear_memory

        /* clear SBSS section */
        b.ori    r3,r0,_sbss_start
        b.ori    r4,r0,_sbss_end
        b.jal    _clear_memory

        /* clear STACK section */
        b.ori    r3,r0,_heap_limit
        b.ori    r4,r0,_stack
        b.jal    _clear_memory

        /* clear heap and stack section */
        b.ori	r3,r0,_sbss_end
        b.ori	r4,r0,_stack-64
        b.jal	_fill_memory

        /* for check DRAM_CODE */

        b.ori	r3,r0,_stack
        b.ori	r4,r0,_stack+8
        b.jal	_fill_memory

        /* set up global pointer */
        b.ori    r2,r0,__gp

        /* execute main */
        b.jal    main


_main_done:
        /* Main returns here.
         * In embedded programs this really should not happen,
         * but if it does... */

        /* nop (special one, that terminates simulations ) */
        b.nop   1
        /* If not run on simulator, just loop that nop,
         * rather than go and execute some random code */
        b.j _main_done
	
/**
 * This function fills (sets to 5a) memory from address stored
 * in r3 to (excluding) address stored in r4.
 */
_fill_memory:
        b.bgeu r3,r4,4f
1:      //first write word by word
        b.addi r3,r3,4
        b.addi r6,r0,0XDEADBEEF
        b.bgtu r3,r4,2f
        b.sw   -4(r3),r6
        b.j 1b
2:      //then the remainder by bytes
        b.addi r3,r3,-4
3:      b.bgeu r3,r4,4f
        b.sb 0(r3),r6
        b.addi r3,r3,1
        b.j 3b
4:      b.return

/**
 * This function clears (sets to 0) memory from address stored
 * in r3 to (excluding) address stored in r4.
 */
_clear_memory:
        b.bgeu r3,r4,4f
1:      //first write word by word
        b.addi r3,r3,4
        b.bgtu r3,r4,2f
        b.sw   -4(r3),r0
        b.j 1b
2:      //then the remainder by bytes
        b.addi r3,r3,-4
3:      b.bgeu r3,r4,4f
        b.sb 0(r3),r0
        b.addi r3,r3,1
        b.j 3b
4:      b.return



#ifndef BA22_DE

/**
 * This function enables data caches (if present in implementation)
 * Whole procedure is:
 *   -make sure they are disabled first
 *   -invalidate any data present in cache
 *      For this "range invalidation" (newer, faster) can be used,
 *      or block invalidation (supported in oldest BA22 implementations).
 *   -reenable cache
 * If your CPU does not have data cache implemented, you can remove this.
 */
_enable_dcache:
        b.mfspr r3,r0,SPR_DCCFGR
        b.beq r3,r0,a_enable_dcache     //cache not implemented=>nothing to do
        //disable data cache
        b.mfspr r4,r0,SPR_SR
        b.andi r4,r4,~SPR_SR_DCE
        b.mtspr r0,r4,SPR_SR
        //use best method supported to invalidate it
        b.andi r3,r3,SPR_DCCFGR_RIRI    //check for range invalidation
        b.beq r3,r0,o_enable_dcache     //present in all but very old implementations
        //use range invalidation (invalidate all addresses)
        b.mtspr r0,r0,SPR_RIR_MIN
        b.ori r3,r0,0xfffffff0+SPR_RIR_MAX_DC
        b.mtspr r0,r3,SPR_RIR_MAX       //this does it
        b.j x_enable_dcache
o_enable_dcache:
        //use block invalidate (if you know your implementations
        //have range invalidate you can remove this part down to x_enable)
        b.mfspr r5,r0,SPR_DCCFGR
        b.andi r6,r5,SPR_DCCFGR_CBS
        b.srli r6,r6,8
        b.addi r6,r6,2
        b.ori r7,r0,1
        b.sll r7,r7,r6 // r7 = cache line size
        b.andi r5,r5,SPR_DCCFGR_NCS
        b.srli r5,r5,4
        b.add r6,r5,r6
        b.ori r5,r0,1
        b.sll r5,r5,r6 // r5 = cache size
        b.ori r6,r0,0
1:      b.mtspr r0,r6,SPR_DCBIR
        b.add r6,r6,r7
        b.bleu r6,r5,1b
x_enable_dcache:
        //enable data cache
        b.ori r4,r4,SPR_SR_DCE
        b.mtspr r0,r4,SPR_SR
a_enable_dcache:
        b.return

/**
 * Enables instruction cache.
 * Practically the same as enable_dcache, but registers are different.
 * If your CPU does not have instruction cache implemented, you can remove this.
 */
_enable_icache:
        b.mfspr r3,r0,SPR_ICCFGR
        b.beq r3,r0,a_enable_icache     //cache not implemented=>nothing to do
        //disable data cache
        b.mfspr r4,r0,SPR_SR
        b.andi r4,r4,~SPR_SR_ICE
        b.mtspr r0,r4,SPR_SR
        //use best method supported to invalidate it
        b.andi r3,r3,SPR_ICCFGR_RIRI    //check for range invalidation
        b.beq r3,r0,o_enable_icache     //present in all but very old implementations
        //use range invalidation (invalidate all addresses)
        b.mtspr r0,r0,SPR_RIR_MIN
        b.ori r3,r0,0xfffffff0+SPR_RIR_MAX_IC
        b.mtspr r0,r3,SPR_RIR_MAX       //do it
        b.j x_enable_icache
o_enable_icache:
        //use block invalidate (if you know your implementations
        //have range invalidate you can remove this part down to x_enable)
        b.mfspr r5,r0,SPR_ICCFGR
        b.andi r6,r5,SPR_ICCFGR_CBS
        b.srli r6,r6,8
        b.addi r6,r6,2
        b.ori r7,r0,1
        b.sll r7,r7,r6 // r7 = cache line size
        b.andi r5,r5,SPR_ICCFGR_NCS
        b.srli r5,r5,4
        b.add r6,r5,r6
        b.ori r5,r0,1
        b.sll r5,r5,r6 // r5 = cache size
        b.ori r6,r0,0
1:      b.mtspr r0,r6,SPR_ICBIR
        b.add r6,r6,r7
        b.bleu r6,r5,1b
x_enable_icache:
        //enable instruction  cache
        b.ori r4,r4,SPR_SR_ICE
        b.mtspr r0,r4,SPR_SR
a_enable_icache:
        b.return

#endif
.globl _sdio_isr
.globl _sadc_isr
.globl _pwm0_isr
.globl _pwm1_isr
.globl _pwm2_isr
.globl _pwm3_isr
.globl _pwm4_isr
.globl _gpio_isr
.globl _spi_isr
.globl _spi2_isr
.globl _irda_isr
.globl _i2c2_isr
.globl _uart_isr
.globl _usb_isr
.globl _i2c1_isr
.globl _dma_isr

_tt_f     : HANDLE_EXCEPTION(TIME_InterruptHandler)
_sdio_isr : HANDLE_EXCEPTION(SDIO_InterruptHandler)
_sadc_isr : HANDLE_EXCEPTION(SADC_InterruptHandler)
_pwm0_isr : HANDLE_EXCEPTION(PWM0_InterruptHandler)
_pwm1_isr : HANDLE_EXCEPTION(PWM1_InterruptHandler)
_pwm2_isr : HANDLE_EXCEPTION(PWM2_InterruptHandler)
_pwm3_isr : HANDLE_EXCEPTION(PWM3_InterruptHandler)
_pwm4_isr : HANDLE_EXCEPTION(PWM4_InterruptHandler)
_gpio_isr : HANDLE_EXCEPTION(GPIO_InterruptHandler)
_spi_isr  : HANDLE_EXCEPTION( SPI_InterruptHandler)
_spi2_isr  : HANDLE_EXCEPTION( SPI2_InterruptHandler)
_i2c1_isr : HANDLE_EXCEPTION(I2C1_InterruptHandler)
_i2c2_isr : HANDLE_EXCEPTION(I2C2_InterruptHandler)
_uart_isr : HANDLE_EXCEPTION(UART_InterruptHandler)
_irda_isr : HANDLE_EXCEPTION(IRDA_InterruptHandler)
_usb_isr  : HANDLE_EXCEPTION( USB_InterruptHandler)
_dma_isr  : HANDLE_EXCEPTION( DMA_InterruptHandler)

.section dram_code,"ax",%progbits
.globl _ceva_isr
.globl _fft_isr
.globl _aud_isr
.globl _pwm5_isr

_ceva_isr : HANDLE_EXCEPTION(CEVA_InterruptHandler)
_aud_isr  : HANDLE_EXCEPTION( AUD_InterruptHandler)
_fft_isr  : HANDLE_EXCEPTION( FFT_InterruptHandler)
_pwm5_isr : HANDLE_EXCEPTION(PWM5_InterruptHandler)

