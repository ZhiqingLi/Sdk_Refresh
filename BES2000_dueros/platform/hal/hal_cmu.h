#ifndef __HAL_CMU_H__
#define __HAL_CMU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(hal_cmu)

#define LPU_TIMER_US(us)                    (((us) * 32 + 1000 - 1) / 1000)

enum HAL_CMU_CLK_STATUS_T {
    HAL_CMU_CLK_DISABLED,
    HAL_CMU_CLK_ENABLED,
};

enum HAL_CMU_CLK_MODE_T {
    HAL_CMU_CLK_AUTO,
    HAL_CMU_CLK_MANUAL,
};

enum HAL_CMU_RST_STATUS_T {
    HAL_CMU_RST_SET,
    HAL_CMU_RST_CLR,
};

enum HAL_CMU_TIMER_ID_T {
    HAL_CMU_TIMER_ID_00,
    HAL_CMU_TIMER_ID_01,
    HAL_CMU_TIMER_ID_10,
    HAL_CMU_TIMER_ID_11,
    HAL_CMU_TIMER_ID_20,
    HAL_CMU_TIMER_ID_21,
};

enum HAL_CMU_FREQ_T {
    HAL_CMU_FREQ_32K,
    HAL_CMU_FREQ_26M,
    HAL_CMU_FREQ_52M,
    HAL_CMU_FREQ_78M,
    HAL_CMU_FREQ_104M,
    HAL_CMU_FREQ_208M,

    HAL_CMU_FREQ_QTY
};

enum HAL_CMU_PLL_T {
    HAL_CMU_PLL_AUD,
    HAL_CMU_PLL_USB,

    HAL_CMU_PLL_QTY
};

enum HAL_CMU_PLL_USER_T {
    HAL_CMU_PLL_USER_SYS,
    HAL_CMU_PLL_USER_AUD,
    HAL_CMU_PLL_USER_USB,

    HAL_CMU_PLL_USER_QTY,
    HAL_CMU_PLL_USER_ALL = HAL_CMU_PLL_USER_QTY,
};

enum HAL_CMU_PERIPH_FREQ_T {
    HAL_CMU_PERIPH_FREQ_26M,
    HAL_CMU_PERIPH_FREQ_52M,

    HAL_CMU_PERIPH_FREQ_QTY
};

enum HAL_CMU_LPU_CLK_CFG_T {
    HAL_CMU_LPU_CLK_NONE,
    HAL_CMU_LPU_CLK_26M,
    HAL_CMU_LPU_CLK_PLL,

    HAL_CMU_LPU_CLK_QTY
};

enum HAL_PWM_ID_T {
    HAL_PWM_ID_0,
    HAL_PWM_ID_1,
    HAL_PWM_ID_2,
    HAL_PWM_ID_3,
#ifdef CHIP_BEST2000
    HAL_PWM2_ID_0,
    HAL_PWM2_ID_1,
    HAL_PWM2_ID_2,
    HAL_PWM2_ID_3,
#endif

    HAL_PWM_ID_QTY
};

enum HAL_SPDIF_ID_T {
#ifdef CHIP_HAS_SPDIF
	HAL_SPDIF_ID_0 = 0,
#if (CHIP_HAS_SPDIF > 1)
    HAL_SPDIF_ID_1,
#endif
#endif

	HAL_SPDIF_ID_QTY
};

enum HAL_CMU_USB_CLOCK_SEL_T {
    HAL_CMU_USB_CLOCK_SEL_PLL,
    HAL_CMU_USB_CLOCK_SEL_24M_X2,
    HAL_CMU_USB_CLOCK_SEL_48M,
    HAL_CMU_USB_CLOCK_SEL_26M_X2,
    HAL_CMU_USB_CLOCK_SEL_26M_X4,
};

void hal_cmu_set_crystal_freq(uint32_t freq);

uint32_t hal_cmu_get_crystal_freq(void);

int hal_cmu_clock_enable(enum HAL_CMU_MOD_ID_T id);

int hal_cmu_clock_disable(enum HAL_CMU_MOD_ID_T id);

enum HAL_CMU_CLK_STATUS_T hal_cmu_clock_get_status(enum HAL_CMU_MOD_ID_T id);

int hal_cmu_clock_set_mode(enum HAL_CMU_MOD_ID_T id, enum HAL_CMU_CLK_MODE_T mode);

enum HAL_CMU_CLK_MODE_T hal_cmu_clock_get_mode(enum HAL_CMU_MOD_ID_T id);

int hal_cmu_reset_set(enum HAL_CMU_MOD_ID_T id);

int hal_cmu_reset_clear(enum HAL_CMU_MOD_ID_T id);

enum HAL_CMU_RST_STATUS_T hal_cmu_reset_get_status(enum HAL_CMU_MOD_ID_T id);

int hal_cmu_reset_pulse(enum HAL_CMU_MOD_ID_T id);

int hal_cmu_timer_set_div(enum HAL_CMU_TIMER_ID_T id, uint32_t div);

void hal_cmu_timer_select_fast(void);

void hal_cmu_timer_select_slow(void);

void hal_cmu_timer1_select_fast(void);

void hal_cmu_timer1_select_slow(void);

void hal_cmu_timer2_select_fast(void);

void hal_cmu_timer2_select_slow(void);

int hal_cmu_uart0_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq);

int hal_cmu_uart1_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq);

int hal_cmu_uart2_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq);

int hal_cmu_spi_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq);

int hal_cmu_slcd_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq);

int hal_cmu_sdio_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq);

int hal_cmu_sdmmc_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq);

int hal_cmu_i2c_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq);

int hal_cmu_ispi_set_freq(enum HAL_CMU_PERIPH_FREQ_T freq);

int hal_cmu_pwm_set_freq(enum HAL_PWM_ID_T id, uint32_t freq);

int hal_cmu_flash_set_freq(enum HAL_CMU_FREQ_T freq);

int hal_cmu_mem_set_freq(enum HAL_CMU_FREQ_T freq);

int hal_cmu_sys_set_freq(enum HAL_CMU_FREQ_T freq);

enum HAL_CMU_FREQ_T hal_cmu_sys_get_freq(void);

int hal_cmu_flash_select_pll(enum HAL_CMU_PLL_T pll);

int hal_cmu_mem_select_pll(enum HAL_CMU_PLL_T pll);

int hal_cmu_sys_select_pll(enum HAL_CMU_PLL_T pll);

int hal_cmu_get_pll_status(enum HAL_CMU_PLL_T pll);

int hal_cmu_pll_enable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user);

int hal_cmu_pll_disable(enum HAL_CMU_PLL_T pll, enum HAL_CMU_PLL_USER_T user);

void hal_cmu_audio_resample_enable(void);

void hal_cmu_audio_resample_disable(void);

int hal_cmu_get_audio_resample_status(void);

int hal_cmu_codec_adc_set_div(uint32_t div);

uint32_t hal_cmu_codec_adc_get_div(void);

int hal_cmu_codec_dac_set_div(uint32_t div);

uint32_t hal_cmu_codec_dac_get_div(void);

void hal_cmu_codec_clock_enable(void);

void hal_cmu_codec_clock_disable(void);

void hal_cmu_i2s_clock_out_enable(void);

void hal_cmu_i2s_clock_out_disable(void);

void hal_cmu_i2s_set_slave_mode(void);

void hal_cmu_i2s_set_master_mode(void);

void hal_cmu_i2s_clock_enable(void);

void hal_cmu_i2s_clock_disable(void);

int hal_cmu_i2s_set_div(uint32_t div);

void hal_cmu_pcm_clock_out_enable(void);

void hal_cmu_pcm_clock_out_disable(void);

void hal_cmu_pcm_set_slave_mode(void);

void hal_cmu_pcm_set_master_mode(void);

void hal_cmu_pcm_clock_enable(void);

void hal_cmu_pcm_clock_disable(void);

int hal_cmu_pcm_set_div(uint32_t div);

int hal_cmu_spdif_clock_enable(enum HAL_SPDIF_ID_T id);

int hal_cmu_spdif_clock_disable(enum HAL_SPDIF_ID_T id);

int hal_cmu_spdif_set_div(enum HAL_SPDIF_ID_T id, uint32_t div);

void hal_cmu_usb_set_device_mode(void);

void hal_cmu_usb_set_host_mode(void);

void hal_cmu_rom_select_usb_clock(enum HAL_CMU_USB_CLOCK_SEL_T sel);

void hal_cmu_usb_clock_enable(void);

void hal_cmu_usb_clock_disable(void);

int hal_cmu_clock_out_enable(enum HAL_CMU_CLOCK_OUT_ID_T id);

void hal_cmu_clock_out_disable(void);

void hal_cmu_write_lock(void);

void hal_cmu_write_unlock(void);

void hal_cmu_sys_reboot(void);

void hal_cmu_jtag_enable(void);

void hal_cmu_jtag_disable(void);

void hal_cmu_jtag_clock_enable(void);

void hal_cmu_jtag_clock_disable(void);

void hal_cmu_simu_init(void);

void hal_cmu_simu_pass(void);

void hal_cmu_simu_fail(void);

void hal_cmu_simu_tag(uint8_t shift);

void hal_cmu_simu_setval(uint32_t val);

void hal_cmu_low_freq_mode_init(void);

void hal_cmu_low_freq_mode_enable(void);

void hal_cmu_low_freq_mode_disable(void);

void hal_cmu_rom_presetup(void);

void hal_cmu_rom_setup(enum HAL_CMU_FREQ_T freq);

void hal_cmu_fpga_setup(void);

void hal_cmu_setup(void);

// Some internal functions

void hal_cmu_usb_init_div(void);

void hal_cmu_apb_init_div(void);

void hal_cmu_rom_clock_init(void);

void hal_cmu_osc_x2_enable(void);

void hal_cmu_osc_x4_enable(void);

void hal_cmu_module_init_state(void);

void hal_cmu_ema_init(void);

void hal_cmu_lpu_wait_26m_ready(void);

int hal_cmu_lpu_busy(void);

int hal_cmu_lpu_init(enum HAL_CMU_LPU_CLK_CFG_T cfg, uint32_t timer_26m, uint32_t timer_pll);

int hal_cmu_lpu_sleep(void);

void hal_cmu_set_wakeup_pc(uint32_t pc);

volatile uint32_t *hal_cmu_get_bootmode_addr(void);

volatile uint32_t *hal_cmu_get_memsc_addr(void);

#ifdef __cplusplus
}
#endif

#endif

