#include "plat_addr_map.h"
#include "reg_cmu_best1000.h"
#include "reg_btcmu_best1000.h"
#include "hal_cmu.h"
#include "hal_location.h"
#include "hal_timer.h"
#include "hal_chipid.h"
#include "cmsis.h"
#include "pmu.h"

#define HAL_CMU_USB_PLL_CLOCK           (240 * 1000 * 1000)

#define HAL_CMU_USB_CLOCK_60M           (60 * 1000 * 1000)
#define HAL_CMU_USB_CLOCK_48M           (48 * 1000 * 1000)

#define HAL_CMU_PWM_FAST_CLOCK          (26 * 1000 * 1000)
#define HAL_CMU_PWM_SLOW_CLOCK          (32 * 1000)

#define HAL_CMU_PLL_LOCKED_TIMEOUT      MS_TO_TICKS(1)
#define HAL_CMU_26M_READY_TIMEOUT       MS_TO_TICKS(3)

static struct CMU_T * const cmu = (struct CMU_T *)CMU_BASE;

static struct BTCMU_T * const btcmu = (struct BTCMU_T *)BT_CMU_BASE;

static uint8_t BOOT_BSS_LOC pll_user_map[HAL_CMU_PLL_QTY];
STATIC_ASSERT(HAL_CMU_PLL_USER_QTY <= sizeof(pll_user_map[0]) * 8, "Too many PLL users");

static uint8_t anc_clk_user_map;

static uint8_t fir_hs_user_map;

#ifdef __AUDIO_RESAMPLE__
static uint8_t aud_resample_en = 0;
#endif

void hal_cmu_audio_resample_enable(void)
{
#ifdef __AUDIO_RESAMPLE__
    aud_resample_en = 1;
#endif
}

void hal_cmu_audio_resample_disable(void)
{
#ifdef __AUDIO_RESAMPLE__
    aud_resample_en = 0;
#endif
}

int hal_cmu_get_audio_resample_status(void)
{
#ifdef __AUDIO_RESAMPLE__
    return aud_resample_en;
#else
    return 0;
#endif
}

int hal_cmu_clock_enable(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HCLK_ENABLE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PCLK_ENABLE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        cmu->OCLK_ENABLE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    return 0;
}

int hal_cmu_clock_disable(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HCLK_DISABLE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PCLK_DISABLE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        cmu->OCLK_DISABLE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    return 0;
}

enum HAL_CMU_CLK_STATUS_T hal_cmu_clock_get_status(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t status;

    if (id >= HAL_CMU_MOD_QTY) {
        return HAL_CMU_CLK_DISABLED;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        status = cmu->HCLK_ENABLE & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        status = cmu->PCLK_ENABLE & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        status = cmu->OCLK_ENABLE & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    return status ? HAL_CMU_CLK_ENABLED : HAL_CMU_CLK_DISABLED;
}

int hal_cmu_clock_set_mode(enum HAL_CMU_MOD_ID_T id, enum HAL_CMU_CLK_MODE_T mode)
{
    __IO uint32_t *reg;
    uint32_t val;
    uint32_t lock;

    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        reg = &cmu->HCLK_MODE;
        val = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        reg = &cmu->PCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        reg = &cmu->OCLK_MODE;
        val = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    lock = int_lock();
    if (mode == HAL_CMU_CLK_MANUAL) {
        *reg |= val;
    } else {
        *reg &= ~val;
    }
    int_unlock(lock);

    return 0;
}

enum HAL_CMU_CLK_MODE_T hal_cmu_clock_get_mode(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t mode;

    if (id >= HAL_CMU_MOD_QTY) {
        return HAL_CMU_CLK_DISABLED;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        mode = cmu->HCLK_MODE & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        mode = cmu->PCLK_MODE & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        mode = cmu->OCLK_MODE & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    return mode ? HAL_CMU_CLK_MANUAL : HAL_CMU_CLK_AUTO;
}

int hal_cmu_reset_set(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HRESET_SET = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PRESET_SET = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        cmu->ORESET_SET = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    return 0;
}

int hal_cmu_reset_clear(enum HAL_CMU_MOD_ID_T id)
{
    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HRESET_CLR = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PRESET_CLR = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        cmu->ORESET_CLR = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    return 0;
}

enum HAL_CMU_RST_STATUS_T hal_cmu_reset_get_status(enum HAL_CMU_MOD_ID_T id)
{
    uint32_t status;

    if (id >= HAL_CMU_MOD_QTY) {
        return HAL_CMU_RST_SET;
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        status = cmu->HRESET_SET & (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        status = cmu->PRESET_SET & (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        status = cmu->ORESET_SET & (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }

    return status ? HAL_CMU_RST_CLR : HAL_CMU_RST_SET;
}

int hal_cmu_reset_pulse(enum HAL_CMU_MOD_ID_T id)
{
    volatile int i;

    if (id >= HAL_CMU_MOD_QTY) {
        return 1;
    }

    if (hal_cmu_reset_get_status(id) == HAL_CMU_RST_SET) {
        return hal_cmu_reset_clear(id);
    }

    if (id < HAL_CMU_MOD_P_CMU) {
        cmu->HRESET_PULSE = (1 << id);
    } else if (id < HAL_CMU_MOD_O_SLEEP) {
        cmu->PRESET_PULSE = (1 << (id - HAL_CMU_MOD_P_CMU));
    } else {
        cmu->ORESET_PULSE = (1 << (id - HAL_CMU_MOD_O_SLEEP));
    }
    // Delay 5+ PCLK cycles (10+ HCLK cycles)
    for (i = 0; i < 3; i++);

    return 0;
}

int hal_cmu_timer_set_div(enum HAL_CMU_TIMER_ID_T id, uint32_t div)
{
    uint32_t lock;

    if (div < 1) {
        return 1;
    }

    div -= 1;
    if ((div & (CMU_TIMER0_DIV_MASK >> CMU_TIMER0_DIV_SHIFT)) != div) {
        return 1;
    }

    lock = int_lock();
    if (id == HAL_CMU_TIMER_ID_00) {
        cmu->TIMER_CLK = SET_BITFIELD(cmu->TIMER_CLK, CMU_TIMER0_DIV, div);
    } else {
        cmu->TIMER_CLK = SET_BITFIELD(cmu->TIMER_CLK, CMU_TIMER1_DIV, div);
    }
    int_unlock(lock);

    return 0;
}

void hal_cmu_timer_select_fast(void)
{
    uint32_t lock;

    lock = int_lock();
    // 6.5M
    cmu->CLK_OUT |= CMU_SEL_TIMER_FAST;
    int_unlock(lock);
}

void hal_cmu_timer_select_slow(void)
{
    uint32_t lock;

    lock = int_lock();
    // 16K
    cmu->CLK_OUT &= ~CMU_SEL_TIMER_FAST;
    int_unlock(lock);
}

#define PERPH_SET_PLL_DIV_FUNC(f, F) \
int hal_cmu_ ##f## _set_pll_div(uint32_t div) \
{ \
    uint32_t lock; \
    if (div < 2) { \
        return 1; \
    } \
    div -= 2; \
    if ((div & (CMU_CFG_DIV_ ##F## _MASK >> CMU_CFG_DIV_ ##F## _SHIFT)) != div) { \
        return 1; \
    } \
    lock = int_lock(); \
    cmu->PERIPH_CLK = (cmu->PERIPH_CLK & ~(CMU_CFG_DIV_ ##F## _MASK | CMU_SEL_ ##F## _PLL)) | \
        CMU_CFG_DIV_ ##F (div) | CMU_SEL_ ##F## _PLL; \
    int_unlock(lock); \
    return 0; \
}

PERPH_SET_PLL_DIV_FUNC(uart0, UART0);
PERPH_SET_PLL_DIV_FUNC(uart1, UART1);
PERPH_SET_PLL_DIV_FUNC(spi, SPI);
PERPH_SET_PLL_DIV_FUNC(slcd, SLCD);
PERPH_SET_PLL_DIV_FUNC(sdio, SDIO);
PERPH_SET_PLL_DIV_FUNC(sdmmc, SDMMC);

#define SYS_SET_FREQ_FUNC(f, F) \
int hal_cmu_ ##f## _set_freq(enum HAL_CMU_FREQ_T freq) \
{ \
    uint32_t lock; \
    uint32_t mask; \
    uint32_t value; \
    if (freq >= HAL_CMU_FREQ_QTY) { \
        return 1; \
    } \
    mask = CMU_SEL_32K_ ##F | CMU_SEL_13M_ ##F | CMU_SEL_26M_ ##F | CMU_SEL_52M_ ##F | \
        CMU_BYPASS_DIV_ ##F | CMU_CFG_DIV_ ##F## _MASK; \
    if (freq == HAL_CMU_FREQ_32K) { \
        value = CMU_SEL_32K_ ##F | CMU_SEL_13M_ ##F | CMU_SEL_26M_ ##F | CMU_SEL_52M_ ##F; \
    } else if (freq == HAL_CMU_FREQ_26M) { \
        value = CMU_SEL_26M_ ##F | CMU_SEL_52M_ ##F; \
    } else if (freq == HAL_CMU_FREQ_52M) { \
        value = CMU_SEL_52M_ ##F; \
    } else if (freq == HAL_CMU_FREQ_78M) { \
        value = CMU_CFG_DIV_ ##F(1); \
    } else if (freq == HAL_CMU_FREQ_104M) { \
        value = CMU_CFG_DIV_ ##F(0); \
    } else { \
        value = CMU_BYPASS_DIV_ ##F; \
    } \
    lock = int_lock(); \
    cmu->SYS_CLK = (cmu->SYS_CLK & ~mask) | value; \
    int_unlock(lock); \
    return 0; \
}

BOOT_TEXT_SRAM_LOC SYS_SET_FREQ_FUNC(flash, FLASH);
SYS_SET_FREQ_FUNC(mem, MEM);
SYS_SET_FREQ_FUNC(sys, SYS);

enum HAL_CMU_FREQ_T BOOT_TEXT_SRAM_LOC hal_cmu_sys_get_freq(void)
{
    uint32_t sys_clk;
    uint32_t div;

    sys_clk = cmu->SYS_CLK;

    if (sys_clk & CMU_SEL_32K_SYS) {
        return HAL_CMU_FREQ_32K;
    } else if (sys_clk & CMU_SEL_13M_SYS) {
        // 13M in fact
        return HAL_CMU_FREQ_26M;
    } else if (sys_clk & CMU_SEL_26M_SYS) {
        return HAL_CMU_FREQ_26M;
    } else if (sys_clk & CMU_SEL_52M_SYS) {
        return HAL_CMU_FREQ_52M;
    } else {
        if (sys_clk & CMU_BYPASS_DIV_SYS) {
            return HAL_CMU_FREQ_208M;
        } else {
            div = GET_BITFIELD(sys_clk, CMU_CFG_DIV_SYS);
            if (div == 0) {
                return HAL_CMU_FREQ_104M;
            } else if (div == 1) {
                // (div == 1): 69M
                return HAL_CMU_FREQ_78M;
            } else {
                // (div == 2): 52M
                // (div == 3): 42M
                return HAL_CMU_FREQ_52M;
            }
        }
    }
}

#define SYS_SEL_PLL_FUNC(f, F) \
int hal_cmu_ ##f## _select_pll(enum HAL_CMU_PLL_T pll) \
{ \
    uint32_t lock; \
    if (pll >= HAL_CMU_PLL_QTY) { \
        return 1; \
    } \
    lock = int_lock(); \
    if (pll == HAL_CMU_PLL_AUD) { \
        cmu->SYS_CLK &= ~ CMU_SEL_PLLUSB_ ##F; \
    } else { \
        cmu->SYS_CLK |= CMU_SEL_PLLUSB_ ##F; \
    } \
    int_unlock(lock); \
    return 0; \
}

BOOT_TEXT_SRAM_LOC SYS_SEL_PLL_FUNC(flash, FLASH);
SYS_SEL_PLL_FUNC(mem, MEM);
SYS_SEL_PLL_FUNC(sys, SYS);

int hal_cmu_get_pll_status(enum HAL_CMU_PLL_T pll)
{
    return !!(cmu->SYS_CLK & ((pll == HAL_CMU_PLL_AUD) ? CMU_PU_PLL_AUD : CMU_PU_PLL_USB));
}

int hal_cmu_pll_enable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user)
{
    uint32_t pu_val;
    uint32_t en_val;
    uint32_t check;
    uint32_t lock;
    uint32_t start;

    if (pll >= HAL_CMU_PLL_QTY) {
        return 1;
    }
    if (user >= HAL_CMU_PLL_USER_QTY && user != HAL_CMU_PLL_USER_ALL) {
        return 2;
    }

    if (pll == HAL_CMU_PLL_AUD) {
        pu_val = CMU_PU_PLL_AUD;
        en_val = CMU_EN_PLL_AUD;
        check = CMU_LOCKED_PLL_AUD;
    } else {
        pu_val = CMU_PU_PLL_USB;
        en_val = CMU_EN_PLL_USB;
        check = CMU_LOCKED_PLL_USB;
    }

    lock = int_lock();
    if (pll_user_map[pll] == 0 || user == HAL_CMU_PLL_USER_ALL) {
        cmu->SYS_CLK |= pu_val;
    } else {
        check = 0;
    }
    if (user != HAL_CMU_PLL_USER_ALL) {
        pll_user_map[pll] |= (1 << user);
    }
    int_unlock(lock);

    start = hal_sys_timer_get();
    do {
        if (check) {
            if (cmu->CLK_OUT & check) {
                break;
            }
        } else {
            if (cmu->SYS_CLK & en_val) {
                break;
            }
        }
    } while ((hal_sys_timer_get() - start) < HAL_CMU_PLL_LOCKED_TIMEOUT);

    lock = int_lock();
    cmu->SYS_CLK |= en_val;
    int_unlock(lock);

    return (cmu->CLK_OUT & check) ? 0 : 2;
}

int hal_cmu_pll_disable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user)
{
    uint32_t lock;

    if (pll >= HAL_CMU_PLL_QTY) {
        return 1;
    }
    if (user >= HAL_CMU_PLL_USER_QTY && user != HAL_CMU_PLL_USER_ALL) {
        return 2;
    }

    lock = int_lock();
    if (user != HAL_CMU_PLL_USER_ALL) {
        pll_user_map[pll] &= ~(1 << user);
    }
    if (pll_user_map[pll] == 0 || user == HAL_CMU_PLL_USER_ALL) {
        if (pll == HAL_CMU_PLL_AUD) {
            cmu->SYS_CLK &= ~CMU_EN_PLL_AUD;
            cmu->SYS_CLK &= ~CMU_PU_PLL_AUD;
        } else {
            cmu->SYS_CLK &= ~CMU_EN_PLL_USB;
            cmu->SYS_CLK &= ~CMU_PU_PLL_USB;
        }
    }
    int_unlock(lock);

    return 0;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_low_freq_mode_init(void)
{
#ifdef CHIP_HAS_USB
    // Switch system/memory clocks to USB PLL, and then shutdown USB PLL,
    // to save power consumed in clock dividers
    hal_cmu_sys_select_pll(HAL_CMU_PLL_USB);
    hal_cmu_mem_select_pll(HAL_CMU_PLL_USB);
#endif
}

void hal_cmu_low_freq_mode_enable(void)
{
    hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_SYS);
    hal_cmu_pll_enable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_SYS);

    // Clock switch requires that both clocks are available before changing switch
    hal_cmu_sys_select_pll(HAL_CMU_PLL_USB);
    // SYS will NOT use AUD PLL from now on

    hal_cmu_pll_disable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_SYS);
    hal_cmu_pll_disable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_SYS);
}

void hal_cmu_low_freq_mode_disable(void)
{
    hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_SYS);
    hal_cmu_pll_enable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_SYS);

    // Clock switch requires that both clocks are available before changing switch
    hal_cmu_sys_select_pll(HAL_CMU_PLL_AUD);
    // SYS will use AUD PLL from now on

    hal_cmu_pll_disable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_SYS);
}

int hal_cmu_codec_adc_set_div(uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    lock = int_lock();
    cmu->AUD_CLK = SET_BITFIELD(cmu->AUD_CLK, CMU_CFG_DIV_CODEC_ADC, div) | CMU_POL_CLK_CODEC_ADC_IN;
    int_unlock(lock);
    return 0;
}

uint32_t hal_cmu_codec_adc_get_div(void)
{
    return GET_BITFIELD(cmu->AUD_CLK, CMU_CFG_DIV_CODEC_ADC) + 2;
}

int hal_cmu_codec_dac_set_div(uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    lock = int_lock();
#ifdef __AUDIO_RESAMPLE__
    if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2) {
        if (hal_cmu_get_audio_resample_status()) {
            cmu->SYS_CLK |= CMU_EN_PLL_AUD;
            cmu->AUD_CLK = SET_BITFIELD(cmu->AUD_CLK, CMU_CFG_DIV_CODEC_DAC, 0);
            cmu->MISC |= CMU_MISC_ALL_CLOCK_26M;
        } else {
            cmu->AUD_CLK = SET_BITFIELD(cmu->AUD_CLK, CMU_CFG_DIV_CODEC_DAC, div);
            cmu->MISC &= ~CMU_MISC_ALL_CLOCK_26M;
        }
    }
    else
#endif
    {
        cmu->AUD_CLK = SET_BITFIELD(cmu->AUD_CLK, CMU_CFG_DIV_CODEC_DAC, div);
    }
    int_unlock(lock);
    return 0;
}

uint32_t hal_cmu_codec_dac_get_div(void)
{
    return GET_BITFIELD(cmu->AUD_CLK, CMU_CFG_DIV_CODEC_DAC) + 2;
}

void hal_cmu_anc_enable(enum HAL_CMU_ANC_CLK_USER_T user)
{
    uint32_t lock;

    lock = int_lock();
    if (anc_clk_user_map == 0) {
        cmu->AUD_CLK |= CMU_EN_CLK_CODEC_ANC;
    }
    anc_clk_user_map |= (1 << user);
    int_unlock(lock);
}

void hal_cmu_anc_disable(enum HAL_CMU_ANC_CLK_USER_T user)
{
    uint32_t lock;

    lock = int_lock();
    anc_clk_user_map &= ~(1 << user);
    if (anc_clk_user_map == 0) {
        cmu->AUD_CLK &= ~CMU_EN_CLK_CODEC_ANC;
    }
    int_unlock(lock);
}

int hal_cmu_anc_get_status(enum HAL_CMU_ANC_CLK_USER_T user)
{
    if (user < HAL_CMU_ANC_CLK_USER_QTY) {
        return !!(anc_clk_user_map & (1 << user));
    } else {
        return !!anc_clk_user_map;
    }
}

void hal_cmu_fir_high_speed_enable(enum HAL_CMU_FIR_USER_T user)
{
    uint32_t lock;

    lock = int_lock();
    if (fir_hs_user_map == 0) {
        cmu->MISC |= CMU_MISC_FIR_CLOCK_DIV_1;
        pmu_fir_high_speed_config(1);
    }
    fir_hs_user_map |= (1 << user);
    int_unlock(lock);
}

void hal_cmu_fir_high_speed_disable(enum HAL_CMU_FIR_USER_T user)
{
    uint32_t lock;

    lock = int_lock();
    fir_hs_user_map &= ~(1 << user);
    if (fir_hs_user_map == 0) {
        cmu->MISC &= ~CMU_MISC_FIR_CLOCK_DIV_1;
        pmu_fir_high_speed_config(0);
    }
    int_unlock(lock);
}

void hal_cmu_codec_clock_enable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->AUD_CLK |= CMU_EN_CODEC_CLK_SRC;
    int_unlock(lock);
}

void hal_cmu_codec_clock_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->AUD_CLK &= ~CMU_EN_CODEC_CLK_SRC;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_out_enable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->AUD_CLK |= CMU_EN_CLK_I2S_OUT;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_out_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->AUD_CLK &= ~CMU_EN_CLK_I2S_OUT;
    int_unlock(lock);
}

void hal_cmu_i2s_set_slave_mode(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->AUD_CLK |= CMU_SEL_I2S_CLKIN;
    int_unlock(lock);
}

void hal_cmu_i2s_set_master_mode(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->AUD_CLK &= ~CMU_SEL_I2S_CLKIN;
    int_unlock(lock);
}

void hal_cmu_i2s_clock_enable(void)
{
}

void hal_cmu_i2s_clock_disable(void)
{
    hal_cmu_i2s_set_div(0x1FFF + 2);
}

int hal_cmu_i2s_set_div(uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    if ((div & (CMU_CFG_DIV_I2S_MASK >> CMU_CFG_DIV_I2S_SHIFT)) != div) {
        return 1;
    }

    lock = int_lock();
    cmu->AUD_CLK = SET_BITFIELD(cmu->AUD_CLK, CMU_CFG_DIV_I2S, div);
    int_unlock(lock);
    return 0;
}

void hal_cmu_pcm_clock_out_enable(void)
{
}

void hal_cmu_pcm_clock_out_disable(void)
{
}

void hal_cmu_pcm_set_slave_mode(void)
{
}

void hal_cmu_pcm_set_master_mode(void)
{
}

void hal_cmu_pcm_clock_enable(void)
{
}

void hal_cmu_pcm_clock_disable(void)
{
}

int hal_cmu_pcm_set_div(uint32_t div)
{
    uint32_t lock;

    if (div < 2) {
        return 1;
    }

    div -= 2;
    if ((div & (CMU_CFG_DIV_PCM_MASK >> CMU_CFG_DIV_PCM_SHIFT)) != div) {
        return 1;
    }

    lock = int_lock();
    cmu->USB_CLK = SET_BITFIELD(cmu->USB_CLK, CMU_CFG_DIV_PCM, div);
    int_unlock(lock);
    return 0;
}

int hal_cmu_spdif_clock_enable(enum HAL_SPDIF_ID_T id)
{
    return 0;
}

int hal_cmu_spdif_clock_disable(enum HAL_SPDIF_ID_T id)
{
    return 0;
}

int hal_cmu_spdif_set_div(enum HAL_SPDIF_ID_T id, uint32_t div)
{
    return hal_cmu_pcm_set_div(div);
}

#ifdef CHIP_HAS_USB
void hal_cmu_usb_set_device_mode(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->USB_CLK |= CMU_USB_ID;
    int_unlock(lock);
}

void hal_cmu_usb_set_host_mode(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->USB_CLK &= ~CMU_USB_ID;
    int_unlock(lock);
}

#ifdef ROM_BUILD
void hal_cmu_rom_select_usb_clock(enum HAL_CMU_USB_CLOCK_SEL_T sel)
{
}
#endif

void hal_cmu_usb_clock_enable(void)
{
    hal_cmu_pll_enable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_USB);
    hal_cmu_clock_enable(HAL_CMU_MOD_H_USBC);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_USB);
    hal_cmu_reset_set(HAL_CMU_MOD_H_USBC);
    hal_cmu_reset_set(HAL_CMU_MOD_O_USB);
    hal_sys_timer_delay(US_TO_TICKS(60));
    hal_cmu_reset_clear(HAL_CMU_MOD_H_USBC);
    hal_cmu_reset_clear(HAL_CMU_MOD_O_USB);
}

void hal_cmu_usb_clock_disable(void)
{
    hal_cmu_reset_set(HAL_CMU_MOD_O_USB);
    hal_cmu_reset_set(HAL_CMU_MOD_H_USBC);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_USB);
    hal_cmu_clock_disable(HAL_CMU_MOD_H_USBC);
    hal_cmu_pll_disable(HAL_CMU_PLL_USB, HAL_CMU_PLL_USER_USB);
}

void BOOT_TEXT_FLASH_LOC hal_cmu_usb_init_div(void)
{
    cmu->USB_CLK = (cmu->USB_CLK & ~(CMU_CFG_DIV_USB60M_MASK | CMU_CFG_DIV_USB48M_MASK)) |
        CMU_CFG_DIV_USB60M(HAL_CMU_USB_PLL_CLOCK / HAL_CMU_USB_CLOCK_60M - 2) |
        CMU_CFG_DIV_USB48M(HAL_CMU_USB_PLL_CLOCK / HAL_CMU_USB_CLOCK_48M - 2);
}
#endif

void BOOT_TEXT_FLASH_LOC hal_cmu_apb_init_div(void)
{
    // Divider defaults to 2 (reg_val = div - 2)
    cmu->USB_CLK = SET_BITFIELD(cmu->USB_CLK, CMU_CFG_DIV_PSYS, 0);
}

#define PERPH_SET_FREQ_FUNC(f, F) \
int hal_cmu_ ##f## _set_freq(enum HAL_CMU_PERIPH_FREQ_T freq) \
{ \
    uint32_t lock; \
    int ret = 0; \
    lock = int_lock(); \
    if (freq == HAL_CMU_PERIPH_FREQ_26M) { \
        cmu->CLK_OUT |= CMU_SEL_ ##F## _26M; \
    } else if (freq == HAL_CMU_PERIPH_FREQ_52M) { \
        cmu->CLK_OUT &= ~CMU_SEL_ ##F## _26M; \
        cmu->PERIPH_CLK &= ~CMU_SEL_ ##F## _PLL; \
    } else { \
        ret = 1; \
    } \
    int_unlock(lock); \
    return ret; \
}

PERPH_SET_FREQ_FUNC(uart0, UART0);
PERPH_SET_FREQ_FUNC(uart1, UART1);
PERPH_SET_FREQ_FUNC(spi, SPI);
PERPH_SET_FREQ_FUNC(slcd, SLCD);
PERPH_SET_FREQ_FUNC(sdio, SDIO);
PERPH_SET_FREQ_FUNC(sdmmc, SDMMC);

int hal_cmu_i2c_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq)
{
    uint32_t lock;
    int ret = 0;

    lock = int_lock();
    if (freq == HAL_CMU_PERIPH_FREQ_26M) {
        cmu->CLK_OUT |= CMU_SEL_I2C_26M;
    } else if (freq == HAL_CMU_PERIPH_FREQ_52M) {
        cmu->CLK_OUT &= ~CMU_SEL_I2C_26M;
        cmu->USB_CLK &= ~CMU_SEL_I2C_PLL;
    } else {
        ret = 1;
    }
    int_unlock(lock);

    return ret;
}

int hal_cmu_ispi_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq)
{
    uint32_t lock;
    int ret = 0;

    lock = int_lock();
    if (freq == HAL_CMU_PERIPH_FREQ_26M) {
        cmu->CLK_OUT |= CMU_SEL_SPI_ITN_26M;
    } else if (freq == HAL_CMU_PERIPH_FREQ_52M) {
        cmu->CLK_OUT &= ~CMU_SEL_SPI_ITN_26M;
    } else {
        ret = 1;
    }
    int_unlock(lock);

    return ret;
}

int hal_cmu_clock_out_enable(enum HAL_CMU_CLOCK_OUT_ID_T id)
{
    uint32_t lock;

    if (id >= HAL_CMU_CLOCK_OUT_QTY) {
        return 1;
    }

    lock = int_lock();
    cmu->CLK_OUT = SET_BITFIELD(cmu->CLK_OUT, CMU_CFG_CLK_OUT, id) | CMU_CFG_CLK_OUT_EN;
    int_unlock(lock);

    return 0;
}

void hal_cmu_clock_out_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->CLK_OUT &= ~CMU_CFG_CLK_OUT_EN;
    int_unlock(lock);
}

int hal_cmu_pwm_set_freq(enum HAL_PWM_ID_T id, uint32_t freq)
{
    uint32_t lock;
    int clk_32k;
    uint32_t div;

    if (id >= HAL_PWM_ID_QTY) {
        return 1;
    }

    if (freq == HAL_CMU_PWM_SLOW_CLOCK) {
        clk_32k = 1;
        div = 0;
    } else {
        clk_32k = 0;
        div = HAL_CMU_PWM_FAST_CLOCK / freq;
        if (div < 2) {
            return 1;
        }

        div -= 2;
        if ((div & (CMU_CFG_DIV_PWM0_MASK >> CMU_CFG_DIV_PWM0_SHIFT)) != div) {
            return 1;
        }
    }

    lock = int_lock();
    if (id == HAL_PWM_ID_0) {
        cmu->PWM01_CLK = (cmu->PWM01_CLK & ~(CMU_CFG_DIV_PWM0_MASK | CMU_SEL_DIV_PWM0_32K)) |
            CMU_CFG_DIV_PWM0(div) | (clk_32k ? CMU_SEL_DIV_PWM0_32K : 0);
    } else if (id == HAL_PWM_ID_1) {
        cmu->PWM01_CLK = (cmu->PWM01_CLK & ~(CMU_CFG_DIV_PWM1_MASK | CMU_SEL_DIV_PWM1_32K)) |
            CMU_CFG_DIV_PWM1(div) | (clk_32k ? CMU_SEL_DIV_PWM1_32K : 0);
    } else if (id == HAL_PWM_ID_2) {
        cmu->PWM23_CLK = (cmu->PWM23_CLK & ~(CMU_CFG_DIV_PWM2_MASK | CMU_SEL_DIV_PWM2_32K)) |
            CMU_CFG_DIV_PWM2(div) | (clk_32k ? CMU_SEL_DIV_PWM2_32K : 0);
    } else {
        cmu->PWM23_CLK = (cmu->PWM23_CLK & ~(CMU_CFG_DIV_PWM3_MASK | CMU_SEL_DIV_PWM3_32K)) |
            CMU_CFG_DIV_PWM3(div) | (clk_32k ? CMU_SEL_DIV_PWM3_32K : 0);
    }
    int_unlock(lock);
    return 0;
}

void hal_cmu_jtag_enable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->CLK_OUT &= ~CMU_DISABLE_JTAG_I2C;
    int_unlock(lock);
}

void hal_cmu_jtag_disable(void)
{
    uint32_t lock;

    lock = int_lock();
    cmu->CLK_OUT |= CMU_DISABLE_JTAG_I2C;
    int_unlock(lock);
}

void hal_cmu_jtag_clock_enable(void)
{
}

void hal_cmu_jtag_clock_disable(void)
{
}

void hal_cmu_rom_clock_init(void)
{
    cmu->SYS_CLK |= CMU_BYPASS_LOCK_FLASH | CMU_BYPASS_LOCK_MEM | CMU_BYPASS_LOCK_SYS;
}

void BOOT_TEXT_FLASH_LOC hal_cmu_osc_x2_enable(void)
{
}

void BOOT_TEXT_FLASH_LOC hal_cmu_osc_x4_enable(void)
{
}

void BOOT_TEXT_FLASH_LOC hal_cmu_module_init_state(void)
{
    cmu->SYS_CLK |= CMU_BYPASS_LOCK_FLASH | CMU_BYPASS_LOCK_MEM | CMU_BYPASS_LOCK_SYS;

#ifndef SIMU
    cmu->ORESET_SET = CMU_ORST_PSRAM | CMU_ORST_USB | CMU_ORST_SDMMC | CMU_ORST_SDIO | CMU_ORST_SPI | CMU_ORST_SLCD |
        CMU_ORST_UART0 | CMU_ORST_UART1 | CMU_ORST_CODEC_AD | CMU_ORST_CODEC_DA | CMU_ORST_I2S | CMU_ORST_SPDIF | CMU_ORST_PCM |
        CMU_ORST_I2C | CMU_ORST_PWM0 | CMU_ORST_PWM1 | CMU_ORST_PWM2 | CMU_ORST_PWM3;
    cmu->PRESET_SET = CMU_PRST_PWM | CMU_PRST_I2C | CMU_PRST_SPI | CMU_PRST_SLCD | CMU_PRST_UART0 | CMU_PRST_UART1 |
        CMU_PRST_CODEC | CMU_PRST_PCM | CMU_PRST_I2S | CMU_PRST_SPDIF;
    cmu->HRESET_SET = CMU_HRST_PSRAM | CMU_HRST_SDIO | CMU_HRST_SDMMC | CMU_HRST_USBC | CMU_HRST_DPDRX | CMU_HRST_DPDTX;

    cmu->OCLK_DISABLE = CMU_OCLK_PSRAM | CMU_OCLK_USB | CMU_OCLK_SDMMC | CMU_OCLK_SDIO | CMU_OCLK_SPI | CMU_OCLK_SLCD |
        CMU_OCLK_UART0 | CMU_OCLK_UART1 | CMU_OCLK_I2S | CMU_OCLK_SPDIF | CMU_OCLK_PCM |
        CMU_OCLK_I2C | CMU_OCLK_PWM0 | CMU_OCLK_PWM1 | CMU_OCLK_PWM2 | CMU_OCLK_PWM3;
    cmu->PCLK_DISABLE = CMU_PCLK_PWM | CMU_PCLK_I2C | CMU_PCLK_SPI | CMU_PCLK_SLCD | CMU_PCLK_UART0 | CMU_PCLK_UART1 |
        CMU_PCLK_CODEC | CMU_PCLK_PCM | CMU_PCLK_I2S | CMU_PCLK_SPDIF;
    cmu->HCLK_DISABLE = CMU_HCLK_PSRAM | CMU_HCLK_SDIO | CMU_HCLK_SDMMC | CMU_HCLK_USBC | CMU_HCLK_DPDRX | CMU_HCLK_DPDTX;

    // Disable codec clock, and set I2S/SPDIF divider to max
    cmu->AUD_CLK = CMU_CFG_DIV_CODEC_ADC(0xF) | CMU_CFG_DIV_CODEC_DAC(0xF) | CMU_CFG_DIV_I2S(0x1FFF);
    cmu->USB_CLK = (cmu->USB_CLK & ~(CMU_CFG_DIV_I2C_MASK | CMU_CFG_DIV_PCM_MASK)) |
        CMU_CFG_DIV_I2C(0xFF) | CMU_CFG_DIV_PCM(0x1FFF);
#endif
}

void BOOT_TEXT_FLASH_LOC hal_cmu_ema_init(void)
{
    uint32_t ema;

    ema = cmu->BOOT_DVS;
    ema = (ema & ~(CMU_ROM_EMA_MASK | CMU_RAM_EMA_MASK)) |
        CMU_ROM_EMA(0) | CMU_RAM_EMA(0);
    cmu->BOOT_DVS = ema;
}

void hal_cmu_lpu_wait_26m_ready(void)
{
    while ((cmu->WAKEUP_CLK_CFG & CMU_LPU_26M_READY) == 0);
}

int hal_cmu_lpu_busy(void)
{
    if ((cmu->WAKEUP_CLK_CFG & CMU_LPU_AUTO_SWITCH26M) &&
        (cmu->WAKEUP_CLK_CFG & CMU_LPU_26M_READY) == 0) {
        return 1;
    }
    if ((cmu->WAKEUP_CLK_CFG & CMU_LPU_AUTO_SWITCHPLL) &&
        (cmu->WAKEUP_CLK_CFG & CMU_LPU_PLL_READY) == 0) {
        return 1;
    }
    return 0;
}

int BOOT_TEXT_FLASH_LOC hal_cmu_lpu_init(enum HAL_CMU_LPU_CLK_CFG_T cfg, uint32_t timer_26m, uint32_t timer_pll)
{
    uint32_t lpu_clk;

    if (cfg >= HAL_CMU_LPU_CLK_QTY) {
        return 1;
    }
    if ((timer_26m & (CMU_TIMER_WT26M_MASK >> CMU_TIMER_WT26M_SHIFT)) != timer_26m) {
        return 2;
    }
    if ((timer_pll & (CMU_TIMER_WTPLL_MASK >> CMU_TIMER_WTPLL_SHIFT)) != timer_pll) {
        return 3;
    }
    if (hal_cmu_lpu_busy()) {
        return -1;
    }

    if (cfg == HAL_CMU_LPU_CLK_26M) {
        lpu_clk = CMU_LPU_AUTO_SWITCH26M;
    } else if (cfg == HAL_CMU_LPU_CLK_PLL) {
        lpu_clk = CMU_LPU_AUTO_SWITCHPLL | CMU_LPU_AUTO_SWITCH26M;
    } else {
        lpu_clk = 0;
    }

    if (lpu_clk & CMU_LPU_AUTO_SWITCH26M) {
#ifdef TRADITIONAL_LPU_26M
        // Enable RAM wakeup early
        cmu->MCU_TIMER |= CMU_RAM_RETN_UP_EARLY;
#else
        // Disable RAM wakeup early
        cmu->MCU_TIMER &= ~CMU_RAM_RETN_UP_EARLY;
        // MCU auto clock gating (which depends on RAM gating signal)
        cmu->HCLK_MODE &= ~CMU_HCLK_MCU;

        // NOTE: On BEST1000 there might be bug for ROM/RAM auto clock gating
        //       -- system will hang up when playing sound for a little while
        // ROM/RAM auto clock gating
        //cmu->HCLK_MODE &= ~(CMU_HCLK_ROM | CMU_HCLK_RAM0 | CMU_HCLK_RAM1);
#endif
    }

    cmu->WAKEUP_CLK_CFG = CMU_TIMER_WT26M(timer_26m) | CMU_TIMER_WTPLL(0) | lpu_clk;
    if (timer_pll) {
        hal_sys_timer_delay(US_TO_TICKS(60));
        cmu->WAKEUP_CLK_CFG = CMU_TIMER_WT26M(timer_26m) | CMU_TIMER_WTPLL(timer_pll) | lpu_clk;
    }
    return 0;
}

int SRAM_TEXT_LOC hal_cmu_lpu_sleep(void)
{
    uint32_t start;
    uint32_t saved_clk_cfg;
    uint32_t wakeup_cfg;
    uint32_t pll_locked;

    saved_clk_cfg = cmu->SYS_CLK;
    wakeup_cfg = cmu->WAKEUP_CLK_CFG;

    // Setup wakeup mask
    cmu->WAKEUP_MASK0 = NVIC->ISER[0];
    cmu->WAKEUP_MASK1 = NVIC->ISER[1];

    if (wakeup_cfg & CMU_LPU_AUTO_SWITCHPLL) {
        // Do nothing
        // Hardware will switch system/memory/flash freq to 32K and shutdown PLLs automatically
    } else {
        // Switch memory/flash freq to 26M or 32K
        if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26M) {
            cmu->SYS_CLK |= CMU_SEL_26M_FLASH;
            cmu->SYS_CLK |= CMU_SEL_26M_MEM;
        } else {
            cmu->SYS_CLK |= CMU_SEL_32K_FLASH;
            cmu->SYS_CLK |= CMU_SEL_32K_MEM;
        }
        // Switch system freq to 26M
        cmu->SYS_CLK |= CMU_SEL_26M_SYS;
        // Shutdown PLLs
        if (saved_clk_cfg & CMU_PU_PLL_AUD) {
            cmu->SYS_CLK &= ~CMU_EN_PLL_AUD;
            cmu->SYS_CLK &= ~CMU_PU_PLL_AUD;
        }
        if (saved_clk_cfg & CMU_PU_PLL_USB) {
            cmu->SYS_CLK &= ~CMU_EN_PLL_USB;
            cmu->SYS_CLK &= ~CMU_PU_PLL_USB;
        }
        if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26M) {
            // Do nothing
            // Hardware will switch system/memory/flash freq to 32K automatically
        } else {
            // Switch system freq to 32K
            cmu->SYS_CLK |= CMU_SEL_32K_SYS;
        }
    }

    if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26M) {
        // Enable auto memory retention
        cmu->SLEEP = (cmu->SLEEP & ~CMU_MANUAL_RAM_RETN) |
            CMU_DEEPSLEEP_EN | CMU_DEEPSLEEP_ROMRAM_EN | CMU_DEEPSLEEP_START;
    } else {
        // Disable auto memory retention
        cmu->SLEEP = (cmu->SLEEP & ~CMU_DEEPSLEEP_ROMRAM_EN) |
            CMU_DEEPSLEEP_EN | CMU_MANUAL_RAM_RETN | CMU_DEEPSLEEP_START;
    }

    SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
    __WFI();

    if (wakeup_cfg & CMU_LPU_AUTO_SWITCHPLL) {
        start = hal_sys_timer_get();
        while ((cmu->WAKEUP_CLK_CFG & CMU_LPU_PLL_READY) == 0 &&
            (hal_sys_timer_get() - start) < HAL_CMU_26M_READY_TIMEOUT + HAL_CMU_PLL_LOCKED_TIMEOUT);
        // !!! CAUTION !!!
        // Hardware will switch system/memory/flash freq to PLL divider and enable PLLs automatically
    } else {
        // Wait for 26M ready
        if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26M) {
            start = hal_sys_timer_get();
            while ((cmu->WAKEUP_CLK_CFG & CMU_LPU_26M_READY) == 0 &&
                (hal_sys_timer_get() - start) < HAL_CMU_26M_READY_TIMEOUT);
            // Hardware will switch system/memory/flash freq to 26M automatically
        } else {
            hal_sys_timer_delay(MS_TO_TICKS(2));
            // Switch system freq to 26M
            cmu->SYS_CLK &= ~CMU_SEL_32K_SYS;
        }
        // System freq is 26M now and will be restored later
        // Restore PLLs
        if (saved_clk_cfg & (CMU_PU_PLL_AUD | CMU_PU_PLL_USB)) {
            pll_locked = 0;
            if (saved_clk_cfg & CMU_PU_PLL_AUD) {
                cmu->SYS_CLK |= CMU_PU_PLL_AUD;
                pll_locked |= CMU_LOCKED_PLL_AUD;
            }
            if (saved_clk_cfg & CMU_PU_PLL_USB) {
                cmu->SYS_CLK |= CMU_PU_PLL_USB;
                pll_locked |= CMU_LOCKED_PLL_USB;
            }
            start = hal_sys_timer_get();
            while ((cmu->CLK_OUT & pll_locked) != pll_locked &&
                    (hal_sys_timer_get() - start) < HAL_CMU_PLL_LOCKED_TIMEOUT);
            if (saved_clk_cfg & CMU_EN_PLL_AUD) {
                cmu->SYS_CLK |= CMU_EN_PLL_AUD;
            }
            if (saved_clk_cfg & CMU_EN_PLL_USB) {
                cmu->SYS_CLK |= CMU_EN_PLL_USB;
            }
        }
#if 0
        // Restore memory/flash freq
        if (wakeup_cfg & CMU_LPU_AUTO_SWITCH26M) {
            cmu->SYS_CLK &= ~CMU_SEL_26M_MEM;
            cmu->SYS_CLK &= ~CMU_SEL_26M_FLASH;
        } else {
            cmu->SYS_CLK &= ~CMU_SEL_32K_MEM;
            cmu->SYS_CLK &= ~CMU_SEL_32K_FLASH;
        }
#endif
    }

    // Restore system/memory/flash freq
    cmu->SYS_CLK = saved_clk_cfg;

    return 0;
}

volatile uint32_t *hal_cmu_get_bootmode_addr(void)
{
    return &cmu->BOOTMODE;
}

volatile uint32_t *hal_cmu_get_memsc_addr(void)
{
    return &cmu->MEMSC[0];
}

void hal_cmu_force_bt_sleep(void)
{
#if 0
    hal_cmu_reset_clear(HAL_CMU_MOD_BTCPU);
    hal_sys_timer_delay(MS_TO_TICKS(10));
#else
    hal_cmu_reset_set(HAL_CMU_MOD_BTCPU);
    btcmu->MISC &= ~BTCMU_DISABLE_FORCE_PU_OSC_DOWN;

    // NOTE: It will prevent analog I2C from accessing digital registers
    //       if disabling BT clock or resetting BT module

    //hal_sys_timer_delay(3);
    //hal_cmu_clock_disable(HAL_CMU_MOD_BT);
#endif
}

