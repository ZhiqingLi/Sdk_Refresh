#ifndef __REG_CMU_BEST3001_H__
#define __REG_CMU_BEST3001_H__

#include "plat_types.h"

struct CMU_T {
    __IO uint32_t HCLK_ENABLE;      // 0x00
    __IO uint32_t HCLK_DISABLE;     // 0x04
    __IO uint32_t PCLK_ENABLE;      // 0x08
    __IO uint32_t PCLK_DISABLE;     // 0x0C
    __IO uint32_t OCLK_ENABLE;      // 0x10
    __IO uint32_t OCLK_DISABLE;     // 0x14
    __IO uint32_t HCLK_MODE;        // 0x18
    __IO uint32_t PCLK_MODE;        // 0x1C
    __IO uint32_t OCLK_MODE;        // 0x20
         uint32_t RESERVED_024;     // 0x24
    __IO uint32_t HRESET_PULSE;     // 0x28
    __IO uint32_t PRESET_PULSE;     // 0x2C
    __IO uint32_t ORESET_PULSE;     // 0x30
    __IO uint32_t HRESET_SET;       // 0x34
    __IO uint32_t HRESET_CLR;       // 0x38
    __IO uint32_t PRESET_SET;       // 0x3C
    __IO uint32_t PRESET_CLR;       // 0x40
    __IO uint32_t ORESET_SET;       // 0x44
    __IO uint32_t ORESET_CLR;       // 0x48
    __IO uint32_t TIMER0_CLK;       // 0x4C
    __IO uint32_t BOOTMODE;         // 0x50
    __IO uint32_t MCU_TIMER;        // 0x54
    __IO uint32_t SLEEP;            // 0x58
    __IO uint32_t UART_CLK;         // 0x5C
    __IO uint32_t SYS_CLK_ENABLE;   // 0x60
    __IO uint32_t SYS_CLK_DISABLE;  // 0x64
         uint32_t RESERVED_068;     // 0x68
    __IO uint32_t BOOT_DVS;         // 0x6C
    __IO uint32_t CLK_OUT;          // 0x70
    __IO uint32_t I2C_CLK;          // 0x74
    __IO uint32_t RESERVED_078;     // 0x78
    __IO uint32_t RESERVED_07C;     // 0x7C
    __IO uint32_t WRITE_UNLOCK;     // 0x80
    __IO uint32_t WAKEUP_MASK0;     // 0x84
    __IO uint32_t WAKEUP_MASK1;     // 0x88
    __IO uint32_t WAKEUP_CLK_CFG;   // 0x8C
    __IO uint32_t TIMER1_CLK;       // 0x90
    __IO uint32_t PWM01_CLK;        // 0x94
    __IO uint32_t PWM23_CLK;        // 0x98
    __IO uint32_t I2S_SPDIF_CLK;    // 0x9C
    __IO uint32_t CLK_SELECT;       // 0xA0
    __IO uint32_t CODEC_CLK;        // 0xA4
    __IO uint32_t SYS_DIV;          // 0xA8
         uint32_t DMA_CFG0;         // 0xAC
         uint32_t RESERVED_0B0[4];  // 0xB0
    __IO uint32_t MEMSC[4];         // 0xC0
    __IO uint32_t MEMSC_STATUS;     // 0xD0
    __IO uint32_t DMA_CFG1;         // 0xD4
         uint32_t RESERVED_0D4[2];  // 0xD8
    __IO uint32_t CHIP_FEATURE;     // 0xE0
         uint32_t RESERVED_0E4[3];  // 0xE4
    __IO uint32_t MISC;             // 0xF0
    __IO uint32_t SIMU_RES;         // 0xF4
    __IO uint32_t CMU_RES[2];       // 0xF8
};

// reg_00
#define CMU_MANUAL_HCLK_ENABLE(n)               (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_HCLK_ENABLE_MASK             (0xFFFFFFFF << 0)
#define CMU_MANUAL_HCLK_ENABLE_SHIFT            (0)

// reg_04
#define CMU_MANUAL_HCLK_DISABLE(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_HCLK_DISABLE_MASK            (0xFFFFFFFF << 0)
#define CMU_MANUAL_HCLK_DISABLE_SHIFT           (0)

// reg_08
#define CMU_MANUAL_PCLK_ENABLE(n)               (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_PCLK_ENABLE_MASK             (0xFFFFFFFF << 0)
#define CMU_MANUAL_PCLK_ENABLE_SHIFT            (0)

// reg_0c
#define CMU_MANUAL_PCLK_DISABLE(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_PCLK_DISABLE_MASK            (0xFFFFFFFF << 0)
#define CMU_MANUAL_PCLK_DISABLE_SHIFT           (0)

// reg_10
#define CMU_MANUAL_OCLK_ENABLE(n)               (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_OCLK_ENABLE_MASK             (0xFFFFFFFF << 0)
#define CMU_MANUAL_OCLK_ENABLE_SHIFT            (0)

// reg_14
#define CMU_MANUAL_OCLK_DISABLE(n)              (((n) & 0xFFFFFFFF) << 0)
#define CMU_MANUAL_OCLK_DISABLE_MASK            (0xFFFFFFFF << 0)
#define CMU_MANUAL_OCLK_DISABLE_SHIFT           (0)

// reg_18
#define CMU_MODE_HCLK(n)                        (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_HCLK_MASK                      (0xFFFFFFFF << 0)
#define CMU_MODE_HCLK_SHIFT                     (0)

// reg_1c
#define CMU_MODE_PCLK(n)                        (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_PCLK_MASK                      (0xFFFFFFFF << 0)
#define CMU_MODE_PCLK_SHIFT                     (0)

// reg_20
#define CMU_MODE_OCLK(n)                        (((n) & 0xFFFFFFFF) << 0)
#define CMU_MODE_OCLK_MASK                      (0xFFFFFFFF << 0)
#define CMU_MODE_OCLK_SHIFT                     (0)

// reg_28
#define CMU_HRESETN_PULSE(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CMU_HRESETN_PULSE_MASK                  (0xFFFFFFFF << 0)
#define CMU_HRESETN_PULSE_SHIFT                 (0)

// reg_2c
#define CMU_PRESETN_PULSE(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CMU_PRESETN_PULSE_MASK                  (0xFFFFFFFF << 0)
#define CMU_PRESETN_PULSE_SHIFT                 (0)
#define CMU_GLOBAL_RESETN_PULSE                 (1 << (SYS_PRST_NUM+1-1))

// reg_30
#define CMU_ORESETN_PULSE(n)                    (((n) & 0xFFFFFFFF) << 0)
#define CMU_ORESETN_PULSE_MASK                  (0xFFFFFFFF << 0)
#define CMU_ORESETN_PULSE_SHIFT                 (0)

// reg_34
#define CMU_HRESETN_SET(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_HRESETN_SET_MASK                    (0xFFFFFFFF << 0)
#define CMU_HRESETN_SET_SHIFT                   (0)

// reg_38
#define CMU_HRESETN_CLR(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_HRESETN_CLR_MASK                    (0xFFFFFFFF << 0)
#define CMU_HRESETN_CLR_SHIFT                   (0)

#define SYS_PRST_NUM                            15

// reg_3c
#define CMU_PRESETN_SET(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_PRESETN_SET_MASK                    (0xFFFFFFFF << 0)
#define CMU_PRESETN_SET_SHIFT                   (0)
#define CMU_GLOBAL_RESETN_SET                   (1 << (SYS_PRST_NUM+1-1))

// reg_40
#define CMU_PRESETN_CLR(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_PRESETN_CLR_MASK                    (0xFFFFFFFF << 0)
#define CMU_PRESETN_CLR_SHIFT                   (0)
#define CMU_GLOBAL_RESETN_CLR                   (1 << (SYS_PRST_NUM+1-1))

// reg_44
#define CMU_ORESETN_SET(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_ORESETN_SET_MASK                    (0xFFFFFFFF << 0)
#define CMU_ORESETN_SET_SHIFT                   (0)

// reg_48
#define CMU_ORESETN_CLR(n)                      (((n) & 0xFFFFFFFF) << 0)
#define CMU_ORESETN_CLR_MASK                    (0xFFFFFFFF << 0)
#define CMU_ORESETN_CLR_SHIFT                   (0)

// reg_4c
#define CMU_CFG_DIV_TIMER00(n)                  (((n) & 0xFFFF) << 0)
#define CMU_CFG_DIV_TIMER00_MASK                (0xFFFF << 0)
#define CMU_CFG_DIV_TIMER00_SHIFT               (0)
#define CMU_CFG_DIV_TIMER01(n)                  (((n) & 0xFFFF) << 16)
#define CMU_CFG_DIV_TIMER01_MASK                (0xFFFF << 16)
#define CMU_CFG_DIV_TIMER01_SHIFT               (16)

// reg_50
#define CMU_WATCHDOG_RESET                      (1 << 0)
#define CMU_SOFT_GLOBLE_RESET                   (1 << 1)
#define CMU_RTC_INTR_H                          (1 << 2)
#define CMU_CHG_INTR_H                          (1 << 3)
#define CMU_SOFT_BOOT_MODE(n)                   (((n) & 0xFFFFFFF) << 4)
#define CMU_SOFT_BOOT_MODE_MASK                 (0xFFFFFFF << 4)
#define CMU_SOFT_BOOT_MODE_SHIFT                (4)

// reg_54
#define CMU_CFG_HCLK_MCU_OFF_TIMER(n)           (((n) & 0xFF) << 0)
#define CMU_CFG_HCLK_MCU_OFF_TIMER_MASK         (0xFF << 0)
#define CMU_CFG_HCLK_MCU_OFF_TIMER_SHIFT        (0)
#define CMU_HCLK_MCU_ENABLE                     (1 << 8)
#define CMU_SECURE_BOOT                         (1 << 9)
#define CMU_RAM_RETN_UP_EARLY                   (1 << 10)

// reg_58
#define CMU_SLEEP_TIMER(n)                      (((n) & 0xFFFFFF) << 0)
#define CMU_SLEEP_TIMER_MASK                    (0xFFFFFF << 0)
#define CMU_SLEEP_TIMER_SHIFT                   (0)
#define CMU_SLEEP_TIMER_EN                      (1 << 24)
#define CMU_DEEPSLEEP_EN                        (1 << 25)
#define CMU_DEEPSLEEP_ROMRAM_EN                 (1 << 26)
#define CMU_MANUAL_RAM_RETN                     (1 << 27)
#define CMU_DEEPSLEEP_START                     (1 << 28)
#define CMU_DEEPSLEEP_MODE                      (1 << 29)
#define CMU_PU_OSC                              (1 << 30)
#define CMU_WAKEUP_DEEPSLEEP_L                  (1 << 31)

// reg_5c
#define CMU_CFG_DIV_UART0(n)                    (((n) & 0x1F) << 0)
#define CMU_CFG_DIV_UART0_MASK                  (0x1F << 0)
#define CMU_CFG_DIV_UART0_SHIFT                 (0)
#define CMU_SEL_OSCX2_UART0                     (1 << 5)
#define CMU_SEL_PLL_UART0                       (1 << 6)
#define CMU_EN_PLL_UART0                        (1 << 7)
#define CMU_CFG_DIV_UART1(n)                    (((n) & 0x1F) << 8)
#define CMU_CFG_DIV_UART1_MASK                  (0x1F << 8)
#define CMU_CFG_DIV_UART1_SHIFT                 (8)
#define CMU_SEL_OSCX2_UART1                     (1 << 13)
#define CMU_SEL_PLL_UART1                       (1 << 14)
#define CMU_EN_PLL_UART1                        (1 << 15)
#define CMU_SEL_32K_TIMER(n)                    (((n) & 0x3) << 16)
#define CMU_SEL_32K_TIMER_MASK                  (0x3 << 16)
#define CMU_SEL_32K_TIMER_SHIFT                 (16)
#define CMU_SEL_32K_WDT                         (1 << 18)
#define CMU_SEL_TIMER_FAST(n)                   (((n) & 0x3) << 19)
#define CMU_SEL_TIMER_FAST_MASK                 (0x3 << 19)
#define CMU_SEL_TIMER_FAST_SHIFT                (19)
#define CMU_MASK_OBS(n)                         (((n) & 0x3F) << 21)
#define CMU_MASK_OBS_MASK                       (0x3F << 21)
#define CMU_MASK_OBS_SHIFT                      (21)

// reg_60
#define CMU_RSTN_DIV_FLS_ENABLE                 (1 << 0)
#define CMU_SEL_OSC_FLS_ENABLE                  (1 << 1)
#define CMU_SEL_OSCX2_FLS_ENABLE                (1 << 2)
#define CMU_SEL_PLL_FLS_ENABLE                  (1 << 3)
#define CMU_BYPASS_DIV_FLS_ENABLE               (1 << 4)
#define CMU_RSTN_DIV_SYS_ENABLE                 (1 << 5)
#define CMU_SEL_OSC_SYS_ENABLE                  (1 << 6)
#define CMU_SEL_OSCX2_SYS_ENABLE                (1 << 7)
#define CMU_SEL_PLL_SYS_ENABLE                  (1 << 8)
#define CMU_BYPASS_DIV_SYS_ENABLE               (1 << 9)
#define CMU_EN_CLK_TOP_OSCX4_ENABLE             (1 << 10)
#define CMU_EN_CLK_TOP_OSCX2_ENABLE             (1 << 11)
#define CMU_EN_CLK_TOP_JTAG_ENABLE              (1 << 12)
#define CMU_EN_CLK_USB48M_ENABLE                (1 << 13)
#define CMU_EN_CLK_DCDC0_ENABLE                 (1 << 14)
#define CMU_EN_CLK_DCDC1_ENABLE                 (1 << 15)
#define CMU_EN_X2_DIG_ENABLE                    (1 << 16)
#define CMU_EN_X4_DIG_ENABLE                    (1 << 17)
#define CMU_EN_PLLUSB_ENABLE                    (1 << 18)
#define CMU_PU_PLLUSB_ENABLE                    (1 << 19)

// reg_64
#define CMU_RSTN_DIV_FLS_DISABLE                (1 << 0)
#define CMU_SEL_OSC_FLS_DISABLE                 (1 << 1)
#define CMU_SEL_OSCX2_FLS_DISABLE               (1 << 2)
#define CMU_SEL_PLL_FLS_DISABLE                 (1 << 3)
#define CMU_BYPASS_DIV_FLS_DISABLE              (1 << 4)
#define CMU_RSTN_DIV_SYS_DISABLE                (1 << 5)
#define CMU_SEL_OSC_SYS_DISABLE                 (1 << 6)
#define CMU_SEL_OSCX2_SYS_DISABLE               (1 << 7)
#define CMU_SEL_PLL_SYS_DISABLE                 (1 << 8)
#define CMU_BYPASS_DIV_SYS_DISABLE              (1 << 9)
#define CMU_EN_CLK_TOP_OSCX4_DISABLE            (1 << 10)
#define CMU_EN_CLK_TOP_OSCX2_DISABLE            (1 << 11)
#define CMU_EN_CLK_TOP_JTAG_DISABLE             (1 << 12)
#define CMU_EN_CLK_USB48M_DISABLE               (1 << 13)
#define CMU_EN_CLK_DCDC0_DISABLE                (1 << 14)
#define CMU_EN_CLK_DCDC1_DISABLE                (1 << 15)
#define CMU_EN_X2_DIG_DISABLE                   (1 << 16)
#define CMU_EN_X4_DIG_DISABLE                   (1 << 17)
#define CMU_EN_PLLUSB_DISABLE                   (1 << 18)
#define CMU_PU_PLLUSB_DISABLE                   (1 << 19)

// reg_6c
#define CMU_ROM_EMA(n)                          (((n) & 0x7) << 0)
#define CMU_ROM_EMA_MASK                        (0x7 << 0)
#define CMU_ROM_EMA_SHIFT                       (0)
#define CMU_ROM_KEN                             (1 << 3)
#define CMU_ROM_PGEN(n)                         (((n) & 0x7) << 4)
#define CMU_ROM_PGEN_MASK                       (0x7 << 4)
#define CMU_ROM_PGEN_SHIFT                      (4)
#define CMU_RAM_EMA(n)                          (((n) & 0x7) << 7)
#define CMU_RAM_EMA_MASK                        (0x7 << 7)
#define CMU_RAM_EMA_SHIFT                       (7)
#define CMU_RAM_EMAW(n)                         (((n) & 0x3) << 10)
#define CMU_RAM_EMAW_MASK                       (0x3 << 10)
#define CMU_RAM_EMAW_SHIFT                      (10)
#define CMU_RF_EMA(n)                           (((n) & 0x7) << 12)
#define CMU_RF_EMA_MASK                         (0x7 << 12)
#define CMU_RF_EMA_SHIFT                        (12)
#define CMU_RF_EMAW(n)                          (((n) & 0x3) << 15)
#define CMU_RF_EMAW_MASK                        (0x3 << 15)
#define CMU_RF_EMAW_SHIFT                       (15)
#define CMU_RAM_RET0N0                          (1 << 17)
#define CMU_RAM_RET0N1                          (1 << 18)
#define CMU_RAM_RET1N0                          (1 << 19)
#define CMU_RAM_RET1N1                          (1 << 20)
#define CMU_RF_RETN0                            (1 << 21)
#define CMU_RF_RETN1                            (1 << 22)

// reg_70
#define CMU_EN_CLK_OUT                          (1 << 0)
#define CMU_CFG_CLK_OUT(n)                      (((n) & 0x1F) << 1)
#define CMU_CFG_CLK_OUT_MASK                    (0x1F << 1)
#define CMU_CFG_CLK_OUT_SHIFT                   (1)
#define CMU_CFG_DIV_DCDC(n)                     (((n) & 0xF) << 6)
#define CMU_CFG_DIV_DCDC_MASK                   (0xF << 6)
#define CMU_CFG_DIV_DCDC_SHIFT                  (6)
#define CMU_BYPASS_DIV_DCDC                     (1 << 10)
#define CMU_SEL_DCDC_PLL                        (1 << 11)
#define CMU_SEL_DCDC_OSCX2                      (1 << 12)
#define CMU_CLK_DCDC_DRV(n)                     (((n) & 0x3) << 13)
#define CMU_CLK_DCDC_DRV_MASK                   (0x3 << 13)
#define CMU_CLK_DCDC_DRV_SHIFT                  (13)
#define CMU_SEL_DCDC_PHASE0(n)                  (((n) & 0x1F) << 15)
#define CMU_SEL_DCDC_PHASE0_MASK                (0x1F << 15)
#define CMU_SEL_DCDC_PHASE0_SHIFT               (15)
#define CMU_SEL_DCDC_PHASE1(n)                  (((n) & 0x1F) << 20)
#define CMU_SEL_DCDC_PHASE1_MASK                (0x1F << 20)
#define CMU_SEL_DCDC_PHASE1_SHIFT               (20)

// reg_74
#define CMU_CFG_DIV_I2C(n)                      (((n) & 0xFF) << 0)
#define CMU_CFG_DIV_I2C_MASK                    (0xFF << 0)
#define CMU_CFG_DIV_I2C_SHIFT                   (0)
#define CMU_SEL_OSC_I2C                         (1 << 8)
#define CMU_SEL_OSCX2_I2C                       (1 << 9)
#define CMU_SEL_PLL_I2C                         (1 << 10)
#define CMU_EN_PLL_I2C                          (1 << 11)
#define CMU_POL_CLK_ADC_ANA                     (1 << 12)
#define CMU_POL_CLK_DAC_OUT                     (1 << 13)
#define CMU_EN_CLK_DAC_OUT                      (1 << 14)
#define CMU_POL_CLK_I2S_IN                      (1 << 15)
#define CMU_SEL_I2S_CLKIN                       (1 << 16)
#define CMU_EN_CLK_I2S_OUT                      (1 << 17)
#define CMU_POL_CLK_I2S_OUT                     (1 << 18)
#define CMU_FORCE_PU_OFF                        (1 << 19)
#define CMU_LOCK_CPU_EN                         (1 << 20)
#define CMU_CFG_DIV_MCLK(n)                     (((n) & 0xF) << 21)
#define CMU_CFG_DIV_MCLK_MASK                   (0xF << 21)
#define CMU_CFG_DIV_MCLK_SHIFT                  (21)
#define CMU_BYPASS_LOCK_PLLUSB                  (1 << 25)
#define CMU_LOCK_PLLUSB                         (1 << 26)

// reg_80
#define CMU_WRITE_UNLOCK_H                      (1 << 0)
#define CMU_WRITE_UNLOCK_STATUS                 (1 << 1)

// reg_84
#define CMU_WAKEUP_IRQ_MASK0(n)                 (((n) & 0xFFFFFFFF) << 0)
#define CMU_WAKEUP_IRQ_MASK0_MASK               (0xFFFFFFFF << 0)
#define CMU_WAKEUP_IRQ_MASK0_SHIFT              (0)

// reg_88
#define CMU_WAKEUP_IRQ_MASK1(n)                 (((n) & 0xFFFF) << 0)
#define CMU_WAKEUP_IRQ_MASK1_MASK               (0xFFFF << 0)
#define CMU_WAKEUP_IRQ_MASK1_SHIFT              (0)

// reg_8c
#define CMU_TIMER_WT26(n)                       (((n) & 0xFF) << 0)
#define CMU_TIMER_WT26_MASK                     (0xFF << 0)
#define CMU_TIMER_WT26_SHIFT                    (0)
#define CMU_TIMER_WTPLL(n)                      (((n) & 0xF) << 8)
#define CMU_TIMER_WTPLL_MASK                    (0xF << 8)
#define CMU_TIMER_WTPLL_SHIFT                   (8)
#define CMU_LPU_AUTO_SWITCH26                   (1 << 12)
#define CMU_LPU_AUTO_SWITCHPLL                  (1 << 13)
#define CMU_LPU_STATUS_26M                      (1 << 14)
#define CMU_LPU_STATUS_PLL                      (1 << 15)

// reg_90
#define CMU_CFG_DIV_TIMER10(n)                  (((n) & 0xFFFF) << 0)
#define CMU_CFG_DIV_TIMER10_MASK                (0xFFFF << 0)
#define CMU_CFG_DIV_TIMER10_SHIFT               (0)
#define CMU_CFG_DIV_TIMER11(n)                  (((n) & 0xFFFF) << 16)
#define CMU_CFG_DIV_TIMER11_MASK                (0xFFFF << 16)
#define CMU_CFG_DIV_TIMER11_SHIFT               (16)

// reg_94
#define CMU_CFG_DIV_PWM0(n)                     (((n) & 0xFFF) << 0)
#define CMU_CFG_DIV_PWM0_MASK                   (0xFFF << 0)
#define CMU_CFG_DIV_PWM0_SHIFT                  (0)
#define CMU_SEL_OSC_PWM0                        (1 << 12)
#define CMU_EN_OSC_PWM0                         (1 << 13)
#define CMU_CFG_DIV_PWM1(n)                     (((n) & 0xFFF) << 16)
#define CMU_CFG_DIV_PWM1_MASK                   (0xFFF << 16)
#define CMU_CFG_DIV_PWM1_SHIFT                  (16)
#define CMU_SEL_OSC_PWM1                        (1 << 28)
#define CMU_EN_OSC_PWM1                         (1 << 29)

// reg_98
#define CMU_CFG_DIV_PWM2(n)                     (((n) & 0xFFF) << 0)
#define CMU_CFG_DIV_PWM2_MASK                   (0xFFF << 0)
#define CMU_CFG_DIV_PWM2_SHIFT                  (0)
#define CMU_SEL_OSC_PWM2                        (1 << 12)
#define CMU_EN_OSC_PWM2                         (1 << 13)
#define CMU_CFG_DIV_PWM3(n)                     (((n) & 0xFFF) << 16)
#define CMU_CFG_DIV_PWM3_MASK                   (0xFFF << 16)
#define CMU_CFG_DIV_PWM3_SHIFT                  (16)
#define CMU_SEL_OSC_PWM3                        (1 << 28)
#define CMU_EN_OSC_PWM3                         (1 << 29)

// reg_9c
#define CMU_CFG_DIV_I2S(n)                      (((n) & 0x1FFF) << 0)
#define CMU_CFG_DIV_I2S_MASK                    (0x1FFF << 0)
#define CMU_CFG_DIV_I2S_SHIFT                   (0)
#define CMU_SEL_I2S_MCLK(n)                     (((n) & 0x7) << 13)
#define CMU_SEL_I2S_MCLK_MASK                   (0x7 << 13)
#define CMU_SEL_I2S_MCLK_SHIFT                  (13)
#define CMU_CFG_DIV_SPDIF(n)                    (((n) & 0x1FFF) << 16)
#define CMU_CFG_DIV_SPDIF_MASK                  (0x1FFF << 16)
#define CMU_CFG_DIV_SPDIF_SHIFT                 (16)
#define CMU_EN_CLK_PLL_SPDIF                    (1 << 29)
#define CMU_EN_CLK_PLL_I2S                      (1 << 30)
#define CMU_EN_I2S_MCLK                         (1 << 31)

// reg_a0
#define CMU_SEL_OSCX2_DIG                       (1 << 0)
#define CMU_SEL_OSCX4_DIG                       (1 << 1)
#define CMU_SEL_X2_PHASE(n)                     (((n) & 0x1F) << 2)
#define CMU_SEL_X2_PHASE_MASK                   (0x1F << 2)
#define CMU_SEL_X2_PHASE_SHIFT                  (2)
#define CMU_SEL_X4_PHASE(n)                     (((n) & 0x1F) << 7)
#define CMU_SEL_X4_PHASE_MASK                   (0x1F << 7)
#define CMU_SEL_X4_PHASE_SHIFT                  (7)
#define CMU_SEL_X4_SYS                          (1 << 12)
#define CMU_SEL_X4_AUD                          (1 << 13)
#define CMU_CFG_DIV_PER(n)                      (((n) & 0x3) << 14)
#define CMU_CFG_DIV_PER_MASK                    (0x3 << 14)
#define CMU_CFG_DIV_PER_SHIFT                   (14)
#define CMU_BYPASS_DIV_PER                      (1 << 16)

// reg_a4
#define CMU_CFG_DIV_CODECIIR(n)                 (((n) & 0xF) << 0)
#define CMU_CFG_DIV_CODECIIR_MASK               (0xF << 0)
#define CMU_CFG_DIV_CODECIIR_SHIFT              (0)
#define CMU_SEL_OSC_CODECIIR                    (1 << 4)
#define CMU_SEL_OSCX2_CODECIIR                  (1 << 5)
#define CMU_BYPASS_DIV_CODECIIR                 (1 << 6)
#define CMU_CFG_DIV_CODECRS(n)                  (((n) & 0xF) << 8)
#define CMU_CFG_DIV_CODECRS_MASK                (0xF << 8)
#define CMU_CFG_DIV_CODECRS_SHIFT               (8)
#define CMU_SEL_OSC_CODECRS                     (1 << 12)
#define CMU_SEL_OSCX2_CODECRS                   (1 << 13)
#define CMU_BYPASS_DIV_CODECRS                  (1 << 14)

// reg_a8
#define CMU_CFG_DIV_SYS(n)                      (((n) & 0x3) << 0)
#define CMU_CFG_DIV_SYS_MASK                    (0x3 << 0)
#define CMU_CFG_DIV_SYS_SHIFT                   (0)
#define CMU_CFG_DIV_FLS(n)                      (((n) & 0x3) << 2)
#define CMU_CFG_DIV_FLS_MASK                    (0x3 << 2)
#define CMU_CFG_DIV_FLS_SHIFT                   (2)
#define CMU_CFG_DIV_USB48M(n)                   (((n) & 0x3) << 4)
#define CMU_CFG_DIV_USB48M_MASK                 (0x3 << 4)
#define CMU_CFG_DIV_USB48M_SHIFT                (4)
#define CMU_SEL_USB_SRC(n)                      (((n) & 0x3) << 6)
#define CMU_SEL_USB_SRC_MASK                    (0x3 << 6)
#define CMU_SEL_USB_SRC_SHIFT                   (6)
#define CMU_POL_CLK_USB                         (1 << 8)
#define CMU_USB_ID                              (1 << 9)
#define CMU_CFG_DIV_PCLK(n)                     (((n) & 0x3) << 10)
#define CMU_CFG_DIV_PCLK_MASK                   (0x3 << 10)
#define CMU_CFG_DIV_PCLK_SHIFT                  (10)
#define CMU_CFG_DIV_SPI0(n)                     (((n) & 0xF) << 12)
#define CMU_CFG_DIV_SPI0_MASK                   (0xF << 12)
#define CMU_CFG_DIV_SPI0_SHIFT                  (12)
#define CMU_SEL_OSCX2_SPI0                      (1 << 16)
#define CMU_SEL_PLL_SPI0                        (1 << 17)
#define CMU_EN_PLL_SPI0                         (1 << 18)
#define CMU_SEL_OSCX2_SPI1                      (1 << 19)

// reg_ac
#define CMU_DMA_CH0_REQ(n)                      (((n) & 0x1F) << 0)
#define CMU_DMA_CH0_REQ_MASK                    (0x1F << 0)
#define CMU_DMA_CH0_REQ_SHIFT                   (0)
#define CMU_DMA_CH1_REQ(n)                      (((n) & 0x1F) << 5)
#define CMU_DMA_CH1_REQ_MASK                    (0x1F << 5)
#define CMU_DMA_CH1_REQ_SHIFT                   (5)
#define CMU_DMA_CH2_REQ(n)                      (((n) & 0x1F) << 10)
#define CMU_DMA_CH2_REQ_MASK                    (0x1F << 10)
#define CMU_DMA_CH2_REQ_SHIFT                   (10)
#define CMU_DMA_CH3_REQ(n)                      (((n) & 0x1F) << 15)
#define CMU_DMA_CH3_REQ_MASK                    (0x1F << 15)
#define CMU_DMA_CH3_REQ_SHIFT                   (15)
#define CMU_DMA_CH4_REQ(n)                      (((n) & 0x1F) << 20)
#define CMU_DMA_CH4_REQ_MASK                    (0x1F << 20)
#define CMU_DMA_CH4_REQ_SHIFT                   (20)
#define CMU_DMA_CH5_REQ(n)                      (((n) & 0x1F) << 25)
#define CMU_DMA_CH5_REQ_MASK                    (0x1F << 25)
#define CMU_DMA_CH5_REQ_SHIFT                   (25)

// reg_c0
#define CMU_MEMSC0                              (1 << 0)

// reg_c4
#define CMU_MEMSC1                              (1 << 0)

// reg_c8
#define CMU_MEMSC2                              (1 << 0)

// reg_cc
#define CMU_MEMSC3                              (1 << 0)

// reg_d0
#define CMU_MEMSC_STATUS0                       (1 << 0)
#define CMU_MEMSC_STATUS1                       (1 << 1)
#define CMU_MEMSC_STATUS2                       (1 << 2)
#define CMU_MEMSC_STATUS3                       (1 << 3)

// reg_d4
#define CMU_DMA_CH6_REQ(n)                      (((n) & 0x1F) << 0)
#define CMU_DMA_CH6_REQ_MASK                    (0x1F << 0)
#define CMU_DMA_CH6_REQ_SHIFT                   (0)
#define CMU_DMA_CH7_REQ(n)                      (((n) & 0x1F) << 5)
#define CMU_DMA_CH7_REQ_MASK                    (0x1F << 5)
#define CMU_DMA_CH7_REQ_SHIFT                   (5)
#define CMU_DMA_CH8_REQ(n)                      (((n) & 0x1F) << 10)
#define CMU_DMA_CH8_REQ_MASK                    (0x1F << 10)
#define CMU_DMA_CH8_REQ_SHIFT                   (10)
#define CMU_DMA_CH9_REQ(n)                      (((n) & 0x1F) << 15)
#define CMU_DMA_CH9_REQ_MASK                    (0x1F << 15)
#define CMU_DMA_CH9_REQ_SHIFT                   (15)
#define CMU_DMA_CH10_REQ(n)                     (((n) & 0x1F) << 20)
#define CMU_DMA_CH10_REQ_MASK                   (0x1F << 20)
#define CMU_DMA_CH10_REQ_SHIFT                  (20)
#define CMU_DMA_CH11_REQ(n)                     (((n) & 0x1F) << 25)
#define CMU_DMA_CH11_REQ_MASK                   (0x1F << 25)
#define CMU_DMA_CH11_REQ_SHIFT                  (25)

// reg_e0
#define CMU_EFUSE(n)                            (((n) & 0xFFFF) << 0)
#define CMU_EFUSE_MASK                          (0xFFFF << 0)
#define CMU_EFUSE_SHIFT                         (0)
#define CMU_EFUSE_LOCK                          (1 << 31)

// reg_f0
#define CMU_DEBUG0(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CMU_DEBUG0_MASK                         (0xFFFFFFFF << 0)
#define CMU_DEBUG0_SHIFT                        (0)

// reg_f4
#define CMU_DEBUG1(n)                           (((n) & 0xFFFFFFFF) << 0)
#define CMU_DEBUG1_MASK                         (0xFFFFFFFF << 0)
#define CMU_DEBUG1_SHIFT                        (0)

// reg_f8
#define CMU_RESERVED0(n)                        (((n) & 0xFFFFFFFF) << 0)
#define CMU_RESERVED0_MASK                      (0xFFFFFFFF << 0)
#define CMU_RESERVED0_SHIFT                     (0)

// reg_fc
#define CMU_RESERVED1(n)                        (((n) & 0xFFFFFFFF) << 0)
#define CMU_RESERVED1_MASK                      (0xFFFFFFFF << 0)
#define CMU_RESERVED1_SHIFT                     (0)

// MCU System AHB Clocks:
#define SYS_HCLK_MCU                    (1 << 0)
#define SYS_HRST_MCU                    (1 << 0)
#define SYS_HCLK_ROM                    (1 << 1)
#define SYS_HRST_ROM                    (1 << 1)
#define SYS_HCLK_RAM0                   (1 << 2)
#define SYS_HRST_RAM0                   (1 << 2)
#define SYS_HCLK_RAM1                   (1 << 3)
#define SYS_HRST_RAM1                   (1 << 3)
#define SYS_HCLK_AHB0                   (1 << 4)
#define SYS_HRST_AHB0                   (1 << 4)
#define SYS_HCLK_AHB1                   (1 << 5)
#define SYS_HRST_AHB1                   (1 << 5)
#define SYS_HCLK_GDMA                   (1 << 6)
#define SYS_HRST_GDMA                   (1 << 6)
#define SYS_HCLK_EXTMEM                 (1 << 7)
#define SYS_HRST_EXTMEM                 (1 << 7)
#define SYS_HCLK_FLASH                  (1 << 8)
#define SYS_HRST_FLASH                  (1 << 8)
#define SYS_HCLK_USBC                   (1 << 9)
#define SYS_HRST_USBC                   (1 << 9)
#define SYS_HCLK_CODEC                  (1 << 10)
#define SYS_HRST_CODEC                  (1 << 10)
#define SYS_HCLK_I2C_SLAVE              (1 << 11)
#define SYS_HRST_I2C_SLAVE              (1 << 11)
#define SYS_HCLK_USBH                   (1 << 12)
#define SYS_HRST_USBH                   (1 << 12)
#define SYS_HCLK_FFT                    (1 << 13)
#define SYS_HRST_FFT                    (1 << 13)

// MCU System APB Clocks:
#define SYS_PCLK_CMU                    (1 << 0)
#define SYS_PRST_CMU                    (1 << 0)
#define SYS_PCLK_GPIO                   (1 << 1)
#define SYS_PRST_GPIO                   (1 << 1)
#define SYS_PCLK_GPIO_INT               (1 << 2)
#define SYS_PRST_GPIO_INT               (1 << 2)
#define SYS_PCLK_WDT                    (1 << 3)
#define SYS_PRST_WDT                    (1 << 3)
#define SYS_PCLK_PWM                    (1 << 4)
#define SYS_PRST_PWM                    (1 << 4)
#define SYS_PCLK_TIMER0                 (1 << 5)
#define SYS_PRST_TIMER0                 (1 << 5)
#define SYS_PCLK_TIMER1                 (1 << 6)
#define SYS_PRST_TIMER1                 (1 << 6)
#define SYS_PCLK_I2C                    (1 << 7)
#define SYS_PRST_I2C                    (1 << 7)
#define SYS_PCLK_SPI                    (1 << 8)
#define SYS_PRST_SPI                    (1 << 8)
#define SYS_PCLK_SPI_ITN                (1 << 9)
#define SYS_PRST_SPI_ITN                (1 << 9)
#define SYS_PCLK_UART0                  (1 << 10)
#define SYS_PRST_UART0                  (1 << 10)
#define SYS_PCLK_UART1                  (1 << 11)
#define SYS_PRST_UART1                  (1 << 11)
#define SYS_PCLK_I2S                    (1 << 12)
#define SYS_PRST_I2S                    (1 << 12)
#define SYS_PCLK_SPDIF                  (1 << 13)
#define SYS_PRST_SPDIF                  (1 << 13)
#define SYS_PCLK_IOMUX                  (1 << 14)
#define SYS_PRST_IOMUX                  (1 << 14)

// MCU System Other Clocks:
#define SYS_OCLK_SLEEP                  (1 << 0)
#define SYS_ORST_SLEEP                  (1 << 0)
#define SYS_OCLK_FLASH                  (1 << 1)
#define SYS_ORST_FLASH                  (1 << 1)
#define SYS_OCLK_USB                    (1 << 2)
#define SYS_ORST_USB                    (1 << 2)
#define SYS_OCLK_GPIO                   (1 << 3)
#define SYS_ORST_GPIO                   (1 << 3)
#define SYS_OCLK_WDT                    (1 << 4)
#define SYS_ORST_WDT                    (1 << 4)
#define SYS_OCLK_TIMER0                 (1 << 5)
#define SYS_ORST_TIMER0                 (1 << 5)
#define SYS_OCLK_TIMER1                 (1 << 6)
#define SYS_ORST_TIMER1                 (1 << 6)
#define SYS_OCLK_I2C                    (1 << 7)
#define SYS_ORST_I2C                    (1 << 7)
#define SYS_OCLK_SPI                    (1 << 8)
#define SYS_ORST_SPI                    (1 << 8)
#define SYS_OCLK_SPI_ITN                (1 << 9)
#define SYS_ORST_SPI_ITN                (1 << 9)
#define SYS_OCLK_UART0                  (1 << 10)
#define SYS_ORST_UART0                  (1 << 10)
#define SYS_OCLK_UART1                  (1 << 11)
#define SYS_ORST_UART1                  (1 << 11)
#define SYS_OCLK_I2S                    (1 << 12)
#define SYS_ORST_I2S                    (1 << 12)
#define SYS_OCLK_SPDIF                  (1 << 13)
#define SYS_ORST_SPDIF                  (1 << 13)
#define SYS_OCLK_USB32K                 (1 << 14)
#define SYS_ORST_USB32K                 (1 << 14)
#define SYS_OCLK_IOMUX                  (1 << 15)
#define SYS_ORST_IOMUX                  (1 << 15)
#define SYS_OCLK_PWM0                   (1 << 16)
#define SYS_ORST_PWM0                   (1 << 16)
#define SYS_OCLK_PWM1                   (1 << 17)
#define SYS_ORST_PWM1                   (1 << 17)
#define SYS_OCLK_PWM2                   (1 << 18)
#define SYS_ORST_PWM2                   (1 << 18)
#define SYS_OCLK_PWM3                   (1 << 19)
#define SYS_ORST_PWM3                   (1 << 19)
#define SYS_OCLK_CODEC                  (1 << 20)
#define SYS_ORST_CODEC                  (1 << 20)
#define SYS_OCLK_CODECIIR               (1 << 21)
#define SYS_ORST_CODECIIR               (1 << 21)
#define SYS_OCLK_CODECRS                (1 << 22)
#define SYS_ORST_CODECRS                (1 << 22)
#define SYS_OCLK_CODECADC               (1 << 23)
#define SYS_ORST_CODECADC               (1 << 23)
#define SYS_OCLK_CODECAD0               (1 << 24)
#define SYS_ORST_CODECAD0               (1 << 24)
#define SYS_OCLK_CODECAD1               (1 << 25)
#define SYS_ORST_CODECAD1               (1 << 25)
#define SYS_OCLK_CODECDA0               (1 << 26)
#define SYS_ORST_CODECDA0               (1 << 26)
#define SYS_OCLK_CODECDA1               (1 << 27)
#define SYS_ORST_CODECDA1               (1 << 27)

#endif

