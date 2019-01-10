#include "hal_sleep.h"
#include "hal_cmu.h"
#include "hal_location.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_sysfreq.h"
#include "hal_dma.h"
#include "hal_norflash.h"
#include "hal_gpadc.h"
#include "analog.h"
#include "pmu.h"
#include "cmsis.h"

//static uint8_t SRAM_STACK_LOC sleep_stack[128];

static HAL_SLEEP_HOOK_HANDLER sleep_hook_handler[HAL_SLEEP_HOOK_USER_QTY] = {0};

static uint32_t cpu_wake_lock_map;

static uint32_t sys_wake_lock_map;

#ifdef SLEEP_STATS_TRACE
static uint32_t stats_trace_interval;
static uint32_t stats_trace_time;
#endif
static uint32_t stats_interval;
static uint32_t stats_start_time;
static uint32_t light_sleep_time;
static uint32_t deep_sleep_time;
static bool stats_started;
static bool stats_valid;
static uint8_t light_sleep_ratio;
static uint8_t deep_sleep_ratio;

void hal_sleep_start_stats(uint32_t stats_interval_ms, uint32_t trace_interval_ms)
{
    uint32_t lock;

    lock = int_lock();
    if (stats_interval_ms) {
        stats_interval = MS_TO_TICKS(stats_interval_ms);
        stats_start_time = hal_sys_timer_get();
        light_sleep_time = 0;
        deep_sleep_time = 0;
        stats_valid = false;
        stats_started = true;
    } else {
        stats_started = false;
    }
    int_unlock(lock);

#ifdef SLEEP_STATS_TRACE
    if (stats_interval_ms && trace_interval_ms) {
        stats_trace_interval = MS_TO_TICKS(trace_interval_ms);
    } else {
        stats_trace_interval = 0;
    }
#endif
}

int hal_sleep_get_stats(struct CPU_USAGE_T *usage)
{
    int ret;
    uint32_t lock;

    lock = int_lock();
    if (stats_valid) {
        usage->light_sleep = light_sleep_ratio;
        usage->deep_sleep = deep_sleep_ratio;
        usage->busy = 100 - (light_sleep_ratio + deep_sleep_ratio);
        ret = 0;
    } else {
        ret = 1;
    }
    int_unlock(lock);

    return ret;
}

static int hal_sleep_cpu_busy(void)
{
    if (cpu_wake_lock_map || hal_cmu_lpu_busy()) {
        return 1;
    } else {
        return 0;
    }
}

static int hal_sleep_sys_busy(void)
{
    if (sys_wake_lock_map || hal_sysfreq_busy() || hal_dma_busy()) {
        return 1;
    } else {
        return 0;
    }
}

int hal_sleep_set_lowpower_hook(enum HAL_SLEEP_HOOK_USER_T user, HAL_SLEEP_HOOK_HANDLER handler)
{
    sleep_hook_handler[user] = handler;
    return 0;
}

static void SRAM_TEXT_LOC hal_sleep_lowpower_hook(void)
{
    int i;

    for (i = 0; i < HAL_SLEEP_HOOK_USER_QTY; i++) {
        if (sleep_hook_handler[i])
            sleep_hook_handler[i]();
    }
}

static bool SRAM_TEXT_LOC hal_sleep_irq_pending(void)
{
#if 0
    int i;

    for (i = 0; i < (NVIC_NUM_VECTORS - NVIC_USER_IRQ_OFFSET + 31) / 32; i++) {
        if (NVIC->ICPR[i] & NVIC->ISER[i]) {
            return true;
        }
    }
#else
    // If there is any pending and enabled exception (including sysTick)
    if (SCB->ICSR & SCB_ICSR_VECTPENDING_Msk) {
        return true;
    }
#endif

    return false;
}

static enum HAL_SLEEP_STATUS_T SRAM_TEXT_LOC hal_sleep_lowpower_mode(void)
{
    enum HAL_SLEEP_STATUS_T ret;
    uint32_t time = 0;

    ret = HAL_SLEEP_STATUS_SHALLOW;

    if (stats_started) {
        time = hal_sys_timer_get();
    }

    hal_sleep_lowpower_hook();

    // Modules (except for psram and flash) sleep
    analog_sleep();
    pmu_sleep();
    hal_gpadc_sleep();

    // End of sleep

    //psram_sleep();
    hal_norflash_sleep(HAL_NORFLASH_ID_0);

    // Now neither psram nor flash are usable

    if (!hal_sleep_irq_pending()) {
        hal_cmu_lpu_sleep();
        ret = HAL_SLEEP_STATUS_DEEP;
    }

    hal_norflash_wakeup(HAL_NORFLASH_ID_0);
    //psram_wakeup();

    // Now both psram and flash are usable

    pmu_wakeup();
    hal_gpadc_wakeup();
    analog_wakeup();
    // Modules (except for psram and flash) wakeup

    // End of wakeup

    if (stats_started) {
        deep_sleep_time += hal_sys_timer_get() - time;
    }

    return ret;
}

// GCC has trouble in detecting static function usage in embedded ASM statements.
// The following function might be optimized away if there is no explicted call in C codes.
// Specifying "used" (or "noclone") attribute on the function can avoid the mistaken optimization.
static enum HAL_SLEEP_STATUS_T SRAM_TEXT_LOC NOINLINE USED hal_sleep_proc(int shallow_sleep)
{
    enum HAL_SLEEP_STATUS_T ret;
    uint32_t lock;
    uint32_t time = 0;
    uint32_t interval;

    ret = HAL_SLEEP_STATUS_SHALLOW;

    lock = int_lock();

    // Check the sleep conditions in interrupt-locked context
    if (hal_sleep_cpu_busy()) {
        // Cannot sleep
    } else if (hal_sleep_sys_busy()) {
        // Light sleep

        if (stats_started) {
            time = hal_sys_timer_get();
        }

#ifdef NO_LIGHT_SLEEP
        // WFI during USB ISO transfer might generate very weak (0.1 mV) 1K tone interference ???
        while (!hal_sleep_irq_pending());
#else
        SCB->SCR = 0;
        __WFI();
#endif

        if (stats_started) {
            light_sleep_time += hal_sys_timer_get() - time;
        }
#ifdef DEBUG
    } else if (hal_trace_busy()) {
        // Light sleep with trace busy only

        if (stats_started) {
            time = hal_sys_timer_get();
        }

        // No irq will be generated when trace becomes idle, so the trace status should
        // be kept polling actively instead of entering WFI
        while (!hal_sleep_irq_pending() && hal_trace_busy());

        if (stats_started) {
            light_sleep_time += hal_sys_timer_get() - time;
        }

        if (!hal_sleep_irq_pending()) {
            goto _deep_sleep;
        }
#endif
    } else {
        // Deep sleep

_deep_sleep: POSSIBLY_UNUSED;

        if (shallow_sleep) {
            ret = HAL_SLEEP_STATUS_DEEP;
        } else {
            ret = hal_sleep_lowpower_mode();
        }
    }

    if (stats_started) {
        time = hal_sys_timer_get();
        interval = time - stats_start_time;
        if (interval >= stats_interval) {
            if (light_sleep_time > UINT32_MAX / 100) {
                light_sleep_ratio = (uint64_t)light_sleep_time * 100 / interval;
            } else {
                light_sleep_ratio = light_sleep_time * 100 / interval;
            }
            if (deep_sleep_time > UINT32_MAX / 100) {
                deep_sleep_ratio = (uint64_t)deep_sleep_time * 100 / interval;
            } else {
                deep_sleep_ratio = deep_sleep_time * 100 / interval;
            }
            stats_valid = true;
            light_sleep_time = 0;
            deep_sleep_time = 0;
            stats_start_time = time;
        }
    }

    int_unlock(lock);

#ifdef SLEEP_STATS_TRACE
    if (stats_trace_interval && stats_started && stats_valid) {
        time = hal_sys_timer_get();
        if (time - stats_trace_time >= stats_trace_interval) {
            TRACE("CPU USAGE: busy=%d light_sleep=%d deep_sleep=%d", 100 - (light_sleep_ratio + deep_sleep_ratio), light_sleep_ratio, deep_sleep_ratio);
            stats_trace_time = time;
#ifdef DEBUG_SLEEP_USER
            TRACE("SLEEP_USER: cpulock=0x%X syslock=0x%X", cpu_wake_lock_map, sys_wake_lock_map);
            hal_sysfreq_print();
#endif
        }
    }
#endif

    return ret;
}

#if 0
static int hal_sleep_lowpower_wrapper(void)
{
    // Assuming MSP is always in internal SRAM
    return hal_sleep_lowpower_mode();
}
#endif

#define RET_int32_t    __r0

#define SVC_ArgN(n) \
  register int __r##n __asm("r"#n);

#define SVC_ArgR(n,t,a) \
  register t   __r##n __asm("r"#n) = a;

#define SVC_Arg1(t1)                                                           \
  SVC_ArgR(0,t1,a1)                                                            \
  SVC_ArgN(1)                                                                  \
  SVC_ArgN(2)                                                                  \
  SVC_ArgN(3)

#if (defined (__CORTEX_M0)) || defined (__CORTEX_M0PLUS)
#define SVC_Call(f)                                                            \
  __asm volatile                                                               \
  (                                                                            \
    "ldr r7,="#f"\n\t"                                                         \
    "mov r12,r7\n\t"                                                           \
    "svc 0"                                                                    \
    :               "=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)         \
    :                "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)         \
    : "r7", "r12", "lr", "cc"                                                  \
  );
#else
#define SVC_Call(f)                                                            \
  __asm volatile                                                               \
  (                                                                            \
    "ldr r12,="#f"\n\t"                                                        \
    "svc 0"                                                                    \
    :               "=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)         \
    :                "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)         \
    : "r12", "lr", "cc"                                                        \
  );
#endif

#define SVC_1_1(f,t,t1,rv)                                                     \
__attribute__((always_inline))                                                 \
static inline  t __##f (t1 a1) {                                               \
  SVC_Arg1(t1);                                                                \
  SVC_Call(f);                                                                 \
  return (t) rv;                                                               \
}

SVC_1_1(hal_sleep_proc, enum HAL_SLEEP_STATUS_T, int, RET_int32_t);

enum HAL_SLEEP_STATUS_T SRAM_TEXT_LOC hal_sleep_enter_sleep(void)
{
    enum HAL_SLEEP_STATUS_T ret;

    ret = HAL_SLEEP_STATUS_SHALLOW;

#ifdef NO_SLEEP
    return ret;
#endif

#ifdef DEBUG
    if (__get_IPSR() != 0) {
        ASSERT(false, "Cannot enter sleep in ISR");
        return ret;
    }
#endif

    switch (__get_CONTROL() & 0x03) {
        case 0x00:                                  // Privileged Thread mode & MSP
            ret = hal_sleep_proc(false);
            break;
        default:
        //case 0x01:                                // Unprivileged Thread mode & MSP
        //case 0x02:                                // Privileged Thread mode & PSP
        //case 0x03:                                // Unprivileged Thread mode & PSP
            ret =__hal_sleep_proc(false);
            break;
    }

    return ret;
}

enum HAL_SLEEP_STATUS_T SRAM_TEXT_LOC hal_sleep_shallow_sleep(void)
{
#ifdef NO_SLEEP
    return HAL_SLEEP_STATUS_SHALLOW;
#endif

    return hal_sleep_proc(true);
}

int hal_cpu_wake_lock(enum HAL_CPU_WAKE_LOCK_USER_T user)
{
    uint32_t lock;

    if (user >= HAL_CPU_WAKE_LOCK_USER_QTY) {
        return 1;
    }

    lock = int_lock();
    cpu_wake_lock_map |= (1 << user);
    int_unlock(lock);

    return 0;
}

int hal_cpu_wake_unlock(enum HAL_CPU_WAKE_LOCK_USER_T user)
{
    uint32_t lock;

    if (user >= HAL_CPU_WAKE_LOCK_USER_QTY) {
        return 1;
    }

    lock = int_lock();
    cpu_wake_lock_map &= ~(1 << user);
    int_unlock(lock);

    return 0;
}

int hal_sys_wake_lock(enum HAL_SYS_WAKE_LOCK_USER_T user)
{
    uint32_t lock;

    if (user >= HAL_SYS_WAKE_LOCK_USER_QTY) {
        return 1;
    }

    lock = int_lock();
    sys_wake_lock_map |= (1 << user);
    int_unlock(lock);

    return 0;
}

int hal_sys_wake_unlock(enum HAL_SYS_WAKE_LOCK_USER_T user)
{
    uint32_t lock;

    if (user >= HAL_SYS_WAKE_LOCK_USER_QTY) {
        return 1;
    }

    lock = int_lock();
    sys_wake_lock_map &= ~(1 << user);
    int_unlock(lock);

    return 0;
}

