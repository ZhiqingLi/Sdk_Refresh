#include "plat_addr_map.h"
#include "hal_cmu.h"
#include CHIP_SPECIFIC_HDR(reg_cmu)
#ifdef AON_CMU_BASE
#include CHIP_SPECIFIC_HDR(reg_aoncmu)
#endif
#include "cmsis.h"
#include "hal_analogif.h"
#include "hal_bootmode.h"
#include "hal_cache.h"
#include "hal_chipid.h"
#include "hal_iomux.h"
#include "hal_location.h"
#include "hal_sleep.h"
#include "hal_sysfreq.h"
#include "hal_timer.h"
#include "hal_trace.h"

// SIMU_RES
#define CMU_SIMU_RES_PASSED         (0x9A55)
#define CMU_SIMU_RES_FAILED         (0xFA11)

typedef void (*HAL_POWER_DOWN_WAKEUP_HANDLER)(void);

extern uint8_t hal_norflash_init(void);

static struct CMU_T * const cmu = (struct CMU_T *)CMU_BASE;
#ifdef AON_CMU_BASE
static struct AONCMU_T * const POSSIBLY_UNUSED aoncmu = (struct AONCMU_T *)AON_CMU_BASE;
#endif

static uint32_t BOOT_DATA_LOC crystal_freq = 26000000;

void BOOT_TEXT_FLASH_LOC hal_cmu_set_crystal_freq(uint32_t freq)
{
    crystal_freq = freq;
}

uint32_t BOOT_TEXT_SRAM_LOC hal_cmu_get_crystal_freq(void)
{
    return crystal_freq;
}

void hal_cmu_write_lock(void)
{
    cmu->WRITE_UNLOCK = 0xCAFE0000;
}

void hal_cmu_write_unlock(void)
{
    cmu->WRITE_UNLOCK = 0xCAFE0001;
}

void hal_cmu_sys_reboot(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_GLOBAL);
}

void hal_cmu_simu_init(void)
{
    cmu->SIMU_RES = 0;
}

void hal_cmu_simu_pass(void)
{
    cmu->SIMU_RES = CMU_SIMU_RES_PASSED;
}

void hal_cmu_simu_fail(void)
{
    cmu->SIMU_RES = CMU_SIMU_RES_FAILED;
}

void hal_cmu_simu_tag(uint8_t shift)
{
    cmu->SIMU_RES |= (1 << shift);
}

void hal_cmu_simu_setval(uint32_t val)
{
    cmu->SIMU_RES = val;
}

void hal_cmu_set_wakeup_pc(uint32_t pc)
{
#ifdef RAMRET_BASE
    uint32_t *wake_pc =
#ifdef CHIP_BEST2000
        (uint32_t *)RAMRET_BASE;
#else
        (uint32_t *)&aoncmu->WAKEUP_PC;

    STATIC_ASSERT(sizeof(HAL_POWER_DOWN_WAKEUP_HANDLER) <= sizeof(uint32_t), "Invalid func ptr size");
#endif

    *wake_pc = pc;
#endif
}

void hal_cmu_rom_wakeup_check(void)
{
#ifdef RAMRET_BASE
    union HAL_HW_BOOTMODE_T hw;
    uint32_t sw;
    HAL_POWER_DOWN_WAKEUP_HANDLER *wake_fn =
#ifdef CHIP_BEST2000
        (HAL_POWER_DOWN_WAKEUP_HANDLER *)RAMRET_BASE;
#else
        (HAL_POWER_DOWN_WAKEUP_HANDLER *)&aoncmu->WAKEUP_PC;
#endif

    hw = hal_rom_hw_bootmode_get();
    if (hw.watchdog == 0 && hw.global == 0) {
        sw = hal_sw_bootmode_get();
        if ((sw & HAL_SW_BOOTMODE_POWER_DOWN_WAKEUP) && *wake_fn) {
            (*wake_fn)();
        }
    }

    *wake_fn = NULL;
#endif
}

void hal_cmu_rom_presetup(void)
{
    hal_cmu_lpu_wait_26m_ready();
    hal_cmu_simu_init();
    hal_cmu_rom_clock_init();
    hal_cmu_timer_select_slow();
#ifdef TIMER1_BASE
    hal_cmu_timer1_select_fast();
#endif
    hal_sys_timer_open();
    hal_cmu_sys_set_freq(HAL_CMU_FREQ_26M);
    // Init peripheral clocks
    hal_cmu_apb_init_div();
    hal_cmu_ispi_set_freq(HAL_CMU_PERIPH_FREQ_26M);
    // Now ISPI can be configured
}

void hal_cmu_rom_setup(enum HAL_CMU_FREQ_T freq)
{
    if (freq >= HAL_CMU_FREQ_78M) {
#ifdef CHIP_HAS_USB
        hal_cmu_pll_enable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_SYS);
        hal_cmu_sys_select_pll(HAL_CMU_PLL_USB);
        hal_cmu_flash_select_pll(HAL_CMU_PLL_USB);
#else
        hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_SYS);
        hal_cmu_sys_select_pll(HAL_CMU_PLL_AUD);
        hal_cmu_flash_select_pll(HAL_CMU_PLL_AUD);
#endif
    } else if (freq == HAL_CMU_FREQ_52M) {
        hal_cmu_osc_x2_enable();
    }

    hal_sysfreq_req(HAL_SYSFREQ_USER_APP_0, freq);

#ifdef CHIP_HAS_USB
    hal_cmu_usb_init_div();
#endif
    hal_cmu_flash_set_freq(HAL_CMU_FREQ_26M);
    hal_cmu_uart0_set_freq(HAL_CMU_PERIPH_FREQ_26M);
#if (CHIP_HAS_UART > 1)
    hal_cmu_uart1_set_freq(HAL_CMU_PERIPH_FREQ_26M);
#endif
#if (CHIP_HAS_UART > 2)
    hal_cmu_uart2_set_freq(HAL_CMU_PERIPH_FREQ_26M);
#endif

    // Reset critical modules (for JTAG reset and run)
    hal_cache_enable(HAL_CACHE_ID_I_CACHE, HAL_CACHE_NO);
    hal_cache_enable(HAL_CACHE_ID_D_CACHE, HAL_CACHE_NO);

    hal_cmu_reset_set(HAL_CMU_MOD_O_FLASH);
    hal_cmu_reset_set(HAL_CMU_MOD_H_FLASH);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_FLASH);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_FLASH);
}

#ifdef FPGA

void BOOT_TEXT_FLASH_LOC hal_cmu_fpga_setup(void)
{
    hal_cmu_timer_select_slow();
    hal_sys_timer_open();

    hal_sysfreq_req(HAL_SYSFREQ_USER_APP_0, HAL_CMU_FREQ_52M);

    hal_cmu_apb_init_div();
#ifdef CHIP_HAS_USB
    hal_cmu_usb_init_div();
#endif
    hal_cmu_ispi_set_freq(HAL_CMU_PERIPH_FREQ_26M);

    hal_cmu_flash_set_freq(HAL_CMU_FREQ_26M);
    hal_norflash_init();
#ifdef CHIP_HAS_PSRAM
    hal_cmu_mem_set_freq(HAL_CMU_FREQ_52M);
    // TODO: Init psram
#endif

    hal_cmu_uart0_set_freq(HAL_CMU_PERIPH_FREQ_26M);
#if (CHIP_HAS_UART > 1)
    hal_cmu_uart1_set_freq(HAL_CMU_PERIPH_FREQ_26M);
#endif
#ifdef CHIP_HAS_SPI
    hal_cmu_spi_set_freq(HAL_CMU_PERIPH_FREQ_26M);
#endif
#ifdef CHIP_HAS_SPILCD
    hal_cmu_slcd_set_freq(HAL_CMU_PERIPH_FREQ_26M);
#endif
#ifdef CHIP_HAS_SDIO
    hal_cmu_sdio_set_freq(HAL_CMU_PERIPH_FREQ_26M);
#endif
#ifdef CHIP_HAS_SDMMC
    hal_cmu_sdmmc_set_freq(HAL_CMU_PERIPH_FREQ_26M);
#endif
    hal_cmu_i2c_set_freq(HAL_CMU_PERIPH_FREQ_26M);
}

#else // !FPGA

void BOOT_TEXT_FLASH_LOC hal_cmu_setup(void)
{
    int ret;
    enum HAL_CMU_FREQ_T freq;

    hal_iomux_set_default_voltage();
    hal_cmu_module_init_state();
    hal_cmu_ema_init();
    hal_cmu_timer_select_slow();
#ifdef TIMER1_BASE
    hal_cmu_timer1_select_fast();
#endif
    hal_sys_timer_open();
    hal_hw_bootmode_init();

    // Set ISPI module freq
    hal_cmu_ispi_set_freq(HAL_CMU_PERIPH_FREQ_26M);
    // Open analogif (ISPI)
    ret = hal_analogif_open();
    if (ret) {
        hal_cmu_simu_tag(31);
        do { volatile int i = 0; i++; } while (1);
    }
    // Init chip id
    // 1) Read id from ana/rf/pmu
    // 2) Init clock settings in ana/rf/pmu if the default h/w register values are bad
    hal_chipid_init();

    // Enable OSC X2/X4 in cmu after enabling their source in hal_chipid_init()
    hal_cmu_osc_x2_enable();
    hal_cmu_osc_x4_enable();

#ifdef CHIP_HAS_USB
    // Enable USB PLL before switching (clock mux requirement)
    // -- USB PLL might not be started in ROM
    hal_cmu_pll_enable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_ALL);
#endif
    hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_SYS);
    // Init system/flash/memory clocks before switching PLL
    hal_cmu_sys_set_freq(HAL_CMU_FREQ_26M);
    hal_cmu_flash_set_freq(HAL_CMU_FREQ_26M);
    hal_cmu_mem_set_freq(HAL_CMU_FREQ_26M);

#ifdef ULTRA_LOW_POWER
    hal_cmu_low_freq_mode_init();
#else
    // Switch system/memory clocks to audio PLL
    hal_cmu_sys_select_pll(HAL_CMU_PLL_AUD);
    hal_cmu_mem_select_pll(HAL_CMU_PLL_AUD);
#endif

#ifdef FLASH_LOW_SPEED
#ifdef CHIP_HAS_USB
    // Switch flash clocks to USB PLL, and then shutdown USB PLL,
    // to save power consumed in clock dividers
    hal_cmu_flash_select_pll(HAL_CMU_PLL_USB);
#endif
#else
    // Switch flash clocks to audio PLL
    hal_cmu_flash_select_pll(HAL_CMU_PLL_AUD);
#endif

    // Init peripheral clocks (which follow SYS PLL selection)

//#ifdef AUDIO_DEBUG
#if 1
    hal_cmu_uart0_set_freq(HAL_CMU_PERIPH_FREQ_52M);
#else
    hal_cmu_uart0_set_freq(HAL_CMU_PERIPH_FREQ_26M);
#endif
    
#if (CHIP_HAS_UART > 1)
    //hal_cmu_uart1_set_freq(HAL_CMU_PERIPH_FREQ_26M);
#endif
	hal_cmu_uart1_set_freq(HAL_CMU_PERIPH_FREQ_52M);//add by tj

#ifdef CHIP_HAS_SPI
    hal_cmu_spi_set_freq(HAL_CMU_PERIPH_FREQ_26M);
#endif
#ifdef CHIP_HAS_SPILCD
    hal_cmu_slcd_set_freq(HAL_CMU_PERIPH_FREQ_26M);
#endif
#ifdef CHIP_HAS_SDIO
    hal_cmu_sdio_set_freq(HAL_CMU_PERIPH_FREQ_26M);
#endif
#ifdef CHIP_HAS_SDMMC
    hal_cmu_sdmmc_set_freq(HAL_CMU_PERIPH_FREQ_52M);
#endif
    hal_cmu_i2c_set_freq(HAL_CMU_PERIPH_FREQ_26M);

#ifdef CHIP_HAS_USB
    // Disable USB PLL after switching (clock mux requirement)
    hal_cmu_pll_disable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_ALL);

#if defined(MCU_HIGH_PERFORMANCE_MODE)
    hal_sys_timer_delay(10);
    hal_cmu_pll_enable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_SYS);
    hal_sys_timer_delay(10);
    hal_cmu_sys_select_pll(HAL_CMU_PLL_USB);
#endif
#endif
#ifdef ULTRA_LOW_POWER
    hal_cmu_pll_disable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_SYS);
#endif

    // Sleep setting
#ifdef NO_LPU_26M
    while (hal_cmu_lpu_init(HAL_CMU_LPU_CLK_NONE, LPU_TIMER_US(3000), 0) == -1);
#else
    while (hal_cmu_lpu_init(HAL_CMU_LPU_CLK_26M, LPU_TIMER_US(3000), 0) == -1);
#endif
    // Init sys freq after applying the sleep setting (which might change sys freq)
#ifdef NO_LPU_26M
   hal_sys_timer_delay(MS_TO_TICKS(20));
#endif

    // Init system clock
#ifdef ULTRA_LOW_POWER
    freq = HAL_CMU_FREQ_52M;
#else
    freq = HAL_CMU_FREQ_104M;
#endif
    hal_sysfreq_req(HAL_SYSFREQ_USER_APP_0, freq);

    // Init flash
    hal_norflash_init();

    // Init psram

    // TODO: Call psram init function and move mem freq setting into the function
#ifdef CHIP_HAS_PSRAM
#ifdef ULTRA_LOW_POWER
    hal_cmu_mem_set_freq(HAL_CMU_FREQ_26M);
#else
    hal_cmu_mem_set_freq(HAL_CMU_FREQ_78M);
#endif
#endif
}

#endif // !FPGA

