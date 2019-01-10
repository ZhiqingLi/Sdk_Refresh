/**************************************************************************//**
 * @file     system_ARMCM4.c
 * @brief    CMSIS Device System Source File for
 *           ARMCM4 Device Series
 * @version  V1.09
 * @date     27. August 2014
 *
 * @note
 *
 ******************************************************************************/
/* Copyright (c) 2011 - 2014 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#include "cmsis.h"
#include "hal_cache.h"
#include "hal_cmu.h"
#include "hal_location.h"


#ifdef SYSTEM_USE_PSRAM
#include "pmu.h"
#include "analog.h"
#include "hal_timer.h"
#include "hal_analogif.h"
#include "hal_psram.h"
#endif

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void SystemInit (void)
{
#if (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                   (3UL << 11*2)  );               /* set CP11 Full Access */
#endif

    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;
#ifdef UNALIGNED_ACCESS
    SCB->CCR &= ~SCB_CCR_UNALIGN_TRP_Msk;
#else
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif
#ifdef USAGE_FAULT
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;
    NVIC_SetPriority(UsageFault_IRQn, IRQ_PRIORITY_REALTIME);
#else
    SCB->SHCSR &= ~SCB_SHCSR_USGFAULTENA_Msk;
#endif
#ifdef BUS_FAULT
    SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
    NVIC_SetPriority(BusFault_IRQn, IRQ_PRIORITY_REALTIME);
#else
    SCB->SHCSR &= ~SCB_SHCSR_BUSFAULTENA_Msk;
#endif
#ifdef MEM_FAULT
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
    NVIC_SetPriority(MemoryManagement_IRQn, IRQ_PRIORITY_REALTIME);
#else
    SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;
#endif
}

#ifndef UNALIGNED_ACCESS

bool get_unaligned_access_status(void)
{
    return !(SCB->CCR & SCB_CCR_UNALIGN_TRP_Msk);
}

bool config_unaligned_access(bool enable)
{
    bool en;

    en = !(SCB->CCR & SCB_CCR_UNALIGN_TRP_Msk);

    if (enable) {
        SCB->CCR &= ~SCB_CCR_UNALIGN_TRP_Msk;
    } else {
        SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
    }

    return en;
}

#endif

// -----------------------------------------------------------
// Boot initialization
// CAUTION: This function and all the functions it called must
//          NOT access global data/bss, for the global data/bss
//          is not available at that time.
// -----------------------------------------------------------
extern uint32_t __boot_sram_start_flash__[];
extern uint32_t __boot_sram_end_flash__[];
extern uint32_t __boot_sram_start__[];
extern uint32_t __boot_bss_sram_start__[];
extern uint32_t __boot_bss_sram_end__[];

extern uint32_t __sram_text_data_start_flash__[];
extern uint32_t __sram_text_data_end_flash__[];
extern uint32_t __sram_text_data_start__[];
extern uint32_t __sram_bss_start__[];
extern uint32_t __sram_bss_end__[];
extern uint32_t __fast_sram_text_data_start__[];
extern uint32_t __fast_sram_text_data_end__[];
extern uint32_t __fast_sram_text_data_start_flash__[];
extern uint32_t __fast_sram_text_data_end_flash__[];

#ifdef SYSTEM_USE_PSRAM
extern uint32_t __psram_text_data_start_flash__[];
extern uint32_t __psram_text_data_end_flash__[];
extern uint32_t __psram_text_data_start__[];
extern uint32_t __psram_bss_start__[];
extern uint32_t __psram_bss_end__[];

void hwtimer_init(void);
/*
psram will be init in main function ,so don't
push any static c lib or cpu init code in psram.
*/
void system_psram_init(void)
{
    int ret = 0;
    uint32_t *dst, *src;
#if 0
	hal_sys_timer_open();

    hal_analogif_open();

    pmu_open();

    analog_open();
#endif
    hal_psram_init();

    for (dst = __psram_text_data_start__, src = __psram_text_data_start_flash__;
            src < __psram_text_data_end_flash__;
            dst++, src++) {
        *dst = *src;
    }
    for (dst = __psram_bss_start__; dst < __psram_bss_end__; dst++) {
        *dst = 0;
    } 
}
#endif


void BOOT_TEXT_FLASH_LOC BootInit(void)
{
    uint32_t *dst, *src;

    // Enable icache
    hal_cache_enable(HAL_CACHE_ID_I_CACHE, HAL_CACHE_YES);
    // Enable dcache
    hal_cache_enable(HAL_CACHE_ID_D_CACHE, HAL_CACHE_YES);
    // Enable write buffer
    hal_cache_writebuffer_enable(HAL_CACHE_ID_D_CACHE, HAL_CACHE_YES);

    // Init boot sections
    for (dst = __boot_sram_start__, src = __boot_sram_start_flash__;
            src < __boot_sram_end_flash__;
            dst++, src++) {
        *dst = *src;
    }

    for (dst = __boot_bss_sram_start__; dst < __boot_bss_sram_end__; dst++) {
        *dst = 0;
    }

#ifdef FPGA
    hal_cmu_fpga_setup();
#else
    hal_cmu_setup();
#endif

    for (dst = __sram_text_data_start__, src = __sram_text_data_start_flash__;
            src < __sram_text_data_end_flash__;
            dst++, src++) {
        *dst = *src;
    }

    for (dst = __sram_bss_start__; dst < __sram_bss_end__; dst++) {
        *dst = 0;
    }

    for (dst = __fast_sram_text_data_start__, src = __fast_sram_text_data_start_flash__;
            src < __fast_sram_text_data_end_flash__;
            dst++, src++) {
        *dst = *src;
    }
}

// -----------------------------------------------------------
// Mutex flag
// -----------------------------------------------------------

int set_bool_flag(bool *flag)
{
    bool busy;

    do {
        busy = (bool)__LDREXB((unsigned char *)flag);
        if (busy) {
            __CLREX();
            return -1;
        }
    } while (__STREXB(true, (unsigned char *)flag));
    __DMB();

    return 0;
}

void clear_bool_flag(bool *flag)
{
    *flag = false;
    __DMB();
}

