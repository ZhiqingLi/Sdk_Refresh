#include "hal_cmu.h"
#include "hal_bootmode.h"
#include "hal_location.h"
#include "cmsis.h"

// BOOTMODE
#define CMU_BOOTMODE_WATCHDOG       (1 << 0)
#define CMU_BOOTMODE_GLOBAL         (1 << 1)
#define CMU_BOOTMODE_RTC            (1 << 2)
#define CMU_BOOTMODE_CHARGER        (1 << 3)

static union HAL_HW_BOOTMODE_T BOOT_BSS_LOC hw_bm;

union HAL_HW_BOOTMODE_T hal_hw_bootmode_get(void)
{
    return hw_bm;
}

union HAL_HW_BOOTMODE_T hal_rom_hw_bootmode_get(void)
{
    union HAL_HW_BOOTMODE_T bm;
    volatile uint32_t *addr;

    addr = hal_cmu_get_bootmode_addr();

    bm.reg = *addr & HAL_HW_BOOTMODE_MASK;
    return bm;
}

void hal_hw_bootmode_init(void)
{
    volatile uint32_t *addr;

    addr = hal_cmu_get_bootmode_addr();

    hw_bm.reg = *addr & HAL_HW_BOOTMODE_MASK;
#ifdef CHIP_BEST3001
    *addr = (*addr | HAL_HW_BOOTMODE_MASK) >> 2;
#else
    *addr = *addr | HAL_HW_BOOTMODE_MASK;
#endif
}

uint32_t hal_sw_bootmode_get(void)
{
    volatile uint32_t *addr;

    addr = hal_cmu_get_bootmode_addr();

    return *addr & HAL_SW_BOOTMODE_MASK;
}

void hal_sw_bootmode_set(uint32_t bm)
{
    uint32_t lock;
    volatile uint32_t *addr;

    addr = hal_cmu_get_bootmode_addr();

    lock = int_lock();
#ifdef CHIP_BEST3001
    *addr = ((*addr | bm) & HAL_SW_BOOTMODE_MASK) >> 2;
#else
    *addr = (*addr | bm) & HAL_SW_BOOTMODE_MASK;
#endif
    int_unlock(lock);
}

void hal_sw_bootmode_clear(uint32_t bm)
{
    uint32_t lock;
    volatile uint32_t *addr;

    addr = hal_cmu_get_bootmode_addr();

    lock = int_lock();
#ifdef CHIP_BEST3001
    *addr = ((*addr & ~bm) & HAL_SW_BOOTMODE_MASK) >> 2;
#else
    *addr = (*addr & ~bm) & HAL_SW_BOOTMODE_MASK;
#endif
    int_unlock(lock);
}

