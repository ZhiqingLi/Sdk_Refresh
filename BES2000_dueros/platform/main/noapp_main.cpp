#include "hal_cmu.h"
#include "hal_timer.h"
#include "hal_dma.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "hal_bootmode.h"
#include "hal_sysfreq.h"
#include "cmsis.h"
#include "hwtimer_list.h"
#include "pmu.h"
#ifdef RTOS
#include "cmsis_os.h"
#include "rt_Time.h"
#endif
#if defined(USB_SERIAL_TEST) || defined(USB_AUDIO_TEST) || defined(AF_TEST) || defined(MEM_TEST)
#include "hwtest.h"
#endif

#ifdef __cplusplus
#define EXTERN_C                        extern "C"
#else
#define EXTERN_C                        extern
#endif

#ifndef FLASH_FILL
#define FLASH_FILL                      1
#endif

#ifdef RTOS
#define OS_TIME_STR                     "[%2u / %u]"
#define OS_CUR_TIME                     , SysTick->VAL, os_time
#else
#define OS_TIME_STR
#define OS_CUR_TIME
#endif

#if defined(MS_TIME)
#define TIME_STR                        "[%u]" OS_TIME_STR
#define CUR_TIME                        TICKS_TO_MS(hal_sys_timer_get())  OS_CUR_TIME
#elif defined(RAW_TIME)
#define TIME_STR                        "[0x%X]" OS_TIME_STR
#define CUR_TIME                        hal_sys_timer_get()  OS_CUR_TIME
#else
#define TIME_STR                        "[%u / 0x%X]" OS_TIME_STR
#define CUR_TIME                        TICKS_TO_MS(hal_sys_timer_get()), hal_sys_timer_get() OS_CUR_TIME
#endif

static void timer_handler(void *param)
{
    TRACE(TIME_STR " Timer handler: %u", CUR_TIME, (uint32_t)param);
}

const static unsigned char bytes[FLASH_FILL] = { 0x1, };

// GDB can set a breakpoint on the main function only if it is
// declared as below, when linking with STD libraries.
int main(int argc, char *argv[])
{
    HWTIMER_ID timer;
    int ret;

    hwtimer_init();
    hal_audma_open();
    hal_gpdma_open();
#if (DEBUG_PORT == 2)
    hal_iomux_set_analog_i2c();
    hal_iomux_set_uart1();
    hal_trace_open(HAL_TRACE_TRANSPORT_UART1);
#else
    hal_iomux_set_uart0();
    hal_trace_open(HAL_TRACE_TRANSPORT_UART0);
#endif

    TRACE(TIME_STR " main started: filled@0x%08x", CUR_TIME, (uint32_t)bytes);

    ret = pmu_open();
    ASSERT(ret == 0, "Failed to open pmu");

    hal_cmu_simu_pass();

#ifdef SIMU
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_FLASH_BOOT);
    hal_cmu_sys_reboot();
#else
    //hal_sysfreq_req(HAL_SYSFREQ_USER_APP_0, HAL_CMU_FREQ_26M);
    TRACE("CPU freq: %u", hal_sys_timer_calc_cpu_freq(0, 0));
#endif

#if defined(USB_SERIAL_TEST) || defined(USB_AUDIO_TEST) || defined(AF_TEST) || defined(MEM_TEST)
#ifdef USB_SERIAL_TEST
    pmu_usb_config(PMU_USB_CONFIG_TYPE_DEVICE);
    usb_serial_test();
#endif
#ifdef USB_AUDIO_TEST
    pmu_usb_config(PMU_USB_CONFIG_TYPE_DEVICE);
    usb_audio_test();
#endif
#ifdef AF_TEST
    af_test();
#endif
#ifdef MEM_TEST
    osDelay(10);
    mem_test();
#endif
    SAFE_PROGRAM_STOP();
#endif

    timer = hwtimer_alloc(timer_handler, 0);
    hwtimer_start(timer, MS_TO_TICKS(1000));
    TRACE(TIME_STR " Start timer", CUR_TIME);

    while (1) {
        osDelay(4000);
        TRACE(TIME_STR " osDelay done", CUR_TIME);
        hwtimer_start(timer, MS_TO_TICKS(1000));
        TRACE(TIME_STR " Start timer", CUR_TIME);
    }

    SAFE_PROGRAM_STOP();
    return 0;
}

