#ifndef __REG_CMU_BEST1000_H__
#define __REG_CMU_BEST1000_H__

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
         uint32_t RESERVED0[1];     // 0x24
    __IO uint32_t HRESET_PULSE;     // 0x28
    __IO uint32_t PRESET_PULSE;     // 0x2C
    __IO uint32_t ORESET_PULSE;     // 0x30
    __IO uint32_t HRESET_SET;       // 0x34
    __IO uint32_t HRESET_CLR;       // 0x38
    __IO uint32_t PRESET_SET;       // 0x3C
    __IO uint32_t PRESET_CLR;       // 0x40
    __IO uint32_t ORESET_SET;       // 0x44
    __IO uint32_t ORESET_CLR;       // 0x48
    __IO uint32_t TIMER_CLK;        // 0x4C
    __IO uint32_t BOOTMODE;         // 0x50
    __IO uint32_t MCU_TIMER;        // 0x54
    __IO uint32_t SLEEP;            // 0x58
    __IO uint32_t PERIPH_CLK;       // 0x5C
    __IO uint32_t SYS_CLK;          // 0x60
    __IO uint32_t AUD_CLK;          // 0x64
    __IO uint32_t ROM_BIST;         // 0x68
    __IO uint32_t BOOT_DVS;         // 0x6C
    __IO uint32_t USB_CLK;          // 0x70
    __IO uint32_t CLK_OUT;          // 0x74
    __IO uint32_t PWM01_CLK;        // 0x78
    __IO uint32_t PWM23_CLK;        // 0x7C
    __IO uint32_t WRITE_UNLOCK;     // 0x80
    __IO uint32_t WAKEUP_MASK0;     // 0x84
    __IO uint32_t WAKEUP_MASK1;     // 0x88
    __IO uint32_t WAKEUP_CLK_CFG;   // 0x8C
    __IO uint32_t PSRAM_DLL;        // 0x90
         uint32_t RESERVED2[3];     // 0x94
    __IO uint32_t ISIRQ_SET;        // 0xA0
    __IO uint32_t ISIRQ_CLR;        // 0xA4
         uint32_t RESERVED3[6];     // 0xA8
    __IO uint32_t MEMSC[4];         // 0xC0
    __I  uint32_t MEMSC_STATUS;     // 0xD0
         uint32_t RESERVED4[7];     // 0xD4
    __IO uint32_t MISC;             // 0xF0
    __IO uint32_t SIMU_RES;         // 0xF4
};


#define CMU_HCLK_MCU                (1 << 0)
#define CMU_HCLK_ROM                (1 << 1)
#define CMU_HCLK_RAM0               (1 << 2)
#define CMU_HCLK_RAM1               (1 << 3)
#define CMU_HCLK_AHB0               (1 << 4)
#define CMU_HCLK_AHB1               (1 << 5)
#define CMU_HCLK_AH2H               (1 << 6)
#define CMU_HCLK_AH2H_BT            (1 << 7)
#define CMU_HCLK_ADMA               (1 << 8)
#define CMU_HCLK_GDMA               (1 << 9)
#define CMU_HCLK_PSRAM              (1 << 10)
#define CMU_HCLK_FLASH              (1 << 11)
#define CMU_HCLK_SDIO               (1 << 12)
#define CMU_HCLK_SDMMC              (1 << 13)
#define CMU_HCLK_USBC               (1 << 14)
#define CMU_HCLK_DPDRX              (1 << 15)
#define CMU_HCLK_DPDTX              (1 << 16)

#define CMU_PCLK_CMU                (1 << 0)
#define CMU_PCLK_GPIO               (1 << 1)
#define CMU_PCLK_GPIO_INT           (1 << 2)
#define CMU_PCLK_WDT                (1 << 3)
#define CMU_PCLK_PWM                (1 << 4)
#define CMU_PCLK_TIMER              (1 << 5)
#define CMU_PCLK_I2C                (1 << 6)
#define CMU_PCLK_SPI                (1 << 7)
#define CMU_PCLK_SLCD               (1 << 8)
#define CMU_PCLK_UART0              (1 << 9)
#define CMU_PCLK_UART1              (1 << 10)
#define CMU_PCLK_CODEC              (1 << 11)
#define CMU_PCLK_PCM                (1 << 12)
#define CMU_PCLK_I2S                (1 << 13)
#define CMU_PCLK_SPI_ITN            (1 << 14)
#define CMU_PCLK_IOMUX              (1 << 15)
#define CMU_PCLK_SPDIF              (1 << 16)

#define CMU_OCLK_SLEEP              (1 << 0)
#define CMU_OCLK_FLASH              (1 << 1)
#define CMU_OCLK_PSRAM              (1 << 2)
#define CMU_OCLK_USB                (1 << 3)
#define CMU_OCLK_SDMMC              (1 << 4)
#define CMU_OCLK_SDIO               (1 << 5)
#define CMU_OCLK_WDT                (1 << 6)
#define CMU_OCLK_BT                 (1 << 7)
#define CMU_OCLK_TIMER              (1 << 8)
#define CMU_OCLK_SPI                (1 << 9)
#define CMU_OCLK_SLCD               (1 << 10)
#define CMU_OCLK_UART0              (1 << 11)
#define CMU_OCLK_UART1              (1 << 12)
#define CMU_OCLK_CODEC_AD           (1 << 13)
#define CMU_OCLK_CODEC_DA           (1 << 14)
#define CMU_OCLK_I2S                (1 << 15)
#define CMU_OCLK_SPDIF              (1 << 16)
#define CMU_OCLK_PCM                (1 << 17)
#define CMU_OCLK_I2C                (1 << 18)
#define CMU_OCLK_SPI_ITN            (1 << 19)
#define CMU_OCLK_GPIO               (1 << 20)
#define CMU_OCLK_PWM0               (1 << 21)
#define CMU_OCLK_PWM1               (1 << 22)
#define CMU_OCLK_PWM2               (1 << 23)
#define CMU_OCLK_PWM3               (1 << 24)

#define CMU_HRST_MCU                (1 << 0)
#define CMU_HRST_ROM                (1 << 1)
#define CMU_HRST_RAM0               (1 << 2)
#define CMU_HRST_RAM1               (1 << 3)
#define CMU_HRST_AHB0               (1 << 4)
#define CMU_HRST_AHB1               (1 << 5)
#define CMU_HRST_AH2H               (1 << 6)
#define CMU_HRST_AH2H_BT            (1 << 7)
#define CMU_HRST_ADMA               (1 << 8)
#define CMU_HRST_GDMA               (1 << 9)
#define CMU_HRST_PSRAM              (1 << 10)
#define CMU_HRST_FLASH              (1 << 11)
#define CMU_HRST_SDIO               (1 << 12)
#define CMU_HRST_SDMMC              (1 << 13)
#define CMU_HRST_USBC               (1 << 14)
#define CMU_HRST_DPDRX              (1 << 15)
#define CMU_HRST_DPDTX              (1 << 16)

#define CMU_PRST_CMU                (1 << 0)
#define CMU_PRST_GPIO               (1 << 1)
#define CMU_PRST_GPIO_INT           (1 << 2)
#define CMU_PRST_WDT                (1 << 3)
#define CMU_PRST_PWM                (1 << 4)
#define CMU_PRST_TIMER              (1 << 5)
#define CMU_PRST_I2C                (1 << 6)
#define CMU_PRST_SPI                (1 << 7)
#define CMU_PRST_SLCD               (1 << 8)
#define CMU_PRST_UART0              (1 << 9)
#define CMU_PRST_UART1              (1 << 10)
#define CMU_PRST_CODEC              (1 << 11)
#define CMU_PRST_PCM                (1 << 12)
#define CMU_PRST_I2S                (1 << 13)
#define CMU_PRST_SPI_ITN            (1 << 14)
#define CMU_PRST_IOMUX              (1 << 15)
#define CMU_PRST_SPDIF              (1 << 16)
#define CMU_PRST_GLOBAL             (1 << 17)

#define CMU_ORST_SLEEP              (1 << 0)
#define CMU_ORST_FLASH              (1 << 1)
#define CMU_ORST_PSRAM              (1 << 2)
#define CMU_ORST_USB                (1 << 3)
#define CMU_ORST_SDMMC              (1 << 4)
#define CMU_ORST_SDIO               (1 << 5)
#define CMU_ORST_WDT                (1 << 6)
#define CMU_ORST_BT                 (1 << 7)
#define CMU_ORST_TIMER              (1 << 8)
#define CMU_ORST_SPI                (1 << 9)
#define CMU_ORST_SLCD               (1 << 10)
#define CMU_ORST_UART0              (1 << 11)
#define CMU_ORST_UART1              (1 << 12)
#define CMU_ORST_CODEC_AD           (1 << 13)
#define CMU_ORST_CODEC_DA           (1 << 14)
#define CMU_ORST_I2S                (1 << 15)
#define CMU_ORST_SPDIF              (1 << 16)
#define CMU_ORST_PCM                (1 << 17)
#define CMU_ORST_I2C                (1 << 18)
#define CMU_ORST_SPI_ITN            (1 << 19)
#define CMU_ORST_GPIO               (1 << 20)
#define CMU_ORST_PWM0               (1 << 21)
#define CMU_ORST_PWM1               (1 << 22)
#define CMU_ORST_PWM2               (1 << 23)
#define CMU_ORST_PWM3               (1 << 24)
#define CMU_ORST_BTCPU              (1 << 25)

// TIMER_CLK
#define CMU_TIMER0_DIV(n)           (((n) & 0xFFFF) << 0)
#define CMU_TIMER0_DIV_MASK         (0xFFFF << 0)
#define CMU_TIMER0_DIV_SHIFT        (0)
#define CMU_TIMER1_DIV(n)           (((n) & 0xFFFF) << 16)
#define CMU_TIMER1_DIV_MASK         (0xFFFF << 16)
#define CMU_TIMER1_DIV_SHIFT        (16)

// MCU_TIMER
#define CMU_HCLK_MCU_OFF_TIMER(n)   (((n) & 0xFF) << 0)
#define CMU_HCLK_MCU_OFF_TIMER_MASK (0xFF << 0)
#define CMU_HCLK_MCU_OFF_TIMER_SHIFT (0)
#define CMU_HCLK_MCU_ENABLE         (1 << 8)
#define CMU_RAM_RETN_UP_EARLY       (1 << 9)

// SLEEP
#define CMU_SLEEP_TIMER(n)          (((n) & 0x00FFFFFF) << 0)
#define CMU_SLEEP_TIMER_MASK        (0x00FFFFFF << 0)
#define CMU_SLEEP_TIMER_SHIFT       (0)
#define CMU_SLEEP_TIMER_EN          (1 << 24)
#define CMU_DEEPSLEEP_EN            (1 << 25)
#define CMU_DEEPSLEEP_ROMRAM_EN     (1 << 26)
#define CMU_MANUAL_RAM_RETN         (1 << 27)
#define CMU_DEEPSLEEP_START         (1 << 28)
#define CMU_DEEPSLEEP_MODE          (1 << 29)
#define CMU_PU_OSC                  (1 << 30)
#define CMU_WAKEUP_DEEPSLEEP_L      (1 << 31)

// PERIPH_CLK
#define CMU_CFG_DIV_UART0(n)        (((n) & 0x1F) << 0)
#define CMU_CFG_DIV_UART0_MASK      (0x1F << 0)
#define CMU_CFG_DIV_UART0_SHIFT     (0)
#define CMU_SEL_UART0_PLL           (1 << 5)
#define CMU_CFG_DIV_UART1(n)        (((n) & 0x1F) << 6)
#define CMU_CFG_DIV_UART1_MASK      (0x1F << 6)
#define CMU_CFG_DIV_UART1_SHIFT     (6)
#define CMU_SEL_UART1_PLL           (1 << 11)
#define CMU_CFG_DIV_SPI(n)          (((n) & 0xF) << 12)
#define CMU_CFG_DIV_SPI_MASK        (0xF << 12)
#define CMU_CFG_DIV_SPI_SHIFT       (12)
#define CMU_SEL_SPI_PLL             (16)
#define CMU_CFG_DIV_SLCD(n)         (((n) & 0xF) << 17)
#define CMU_CFG_DIV_SLCD_MASK       (0xF << 17)
#define CMU_CFG_DIV_SLCD_SHIFT      (17)
#define CMU_SEL_SLCD_PLL            (1 << 21)
#define CMU_CFG_DIV_SDIO(n)         (((n) & 0xF) << 22)
#define CMU_CFG_DIV_SDIO_MASK       (0xF << 22)
#define CMU_CFG_DIV_SDIO_SHIFT      (22)
#define CMU_SEL_SDIO_PLL            (1 << 26)
#define CMU_CFG_DIV_SDMMC(n)        (((n) & 0xF) << 27)
#define CMU_CFG_DIV_SDMMC_MASK      (0xF << 27)
#define CMU_CFG_DIV_SDMMC_SHIFT     (27)
#define CMU_SEL_SDMMC_PLL           (1 << 31)

// SYS_CLK
#define CMU_SEL_32K_FLASH           (1 << 0)
#define CMU_SEL_13M_FLASH           (1 << 1)
#define CMU_SEL_26M_FLASH           (1 << 2)
#define CMU_SEL_52M_FLASH           (1 << 3)
#define CMU_SEL_PLLUSB_FLASH        (1 << 4)
#define CMU_BYPASS_LOCK_FLASH       (1 << 5)
#define CMU_BYPASS_DIV_FLASH        (1 << 6)
#define CMU_CFG_DIV_FLASH(n)        (((n) & 3) << 7)
#define CMU_CFG_DIV_FLASH_MASK      (3 << 7)
#define CMU_CFG_DIV_FLASH_SHIFT     (7)
#define CMU_SEL_32K_MEM             (1 << 9)
#define CMU_SEL_13M_MEM             (1 << 10)
#define CMU_SEL_26M_MEM             (1 << 11)
#define CMU_SEL_52M_MEM             (1 << 12)
#define CMU_SEL_PLLUSB_MEM          (1 << 13)
#define CMU_BYPASS_LOCK_MEM         (1 << 14)
#define CMU_BYPASS_DIV_MEM          (1 << 15)
#define CMU_CFG_DIV_MEM(n)          (((n) & 3) << 16)
#define CMU_CFG_DIV_MEM_MASK        (3 << 16)
#define CMU_CFG_DIV_MEM_SHIFT       (16)
#define CMU_SEL_32K_SYS             (1 << 18)
#define CMU_SEL_13M_SYS             (1 << 19)
#define CMU_SEL_26M_SYS             (1 << 20)
#define CMU_SEL_52M_SYS             (1 << 21)
#define CMU_SEL_PLLUSB_SYS          (1 << 22)
#define CMU_BYPASS_LOCK_SYS         (1 << 23)
#define CMU_BYPASS_DIV_SYS          (1 << 24)
#define CMU_CFG_DIV_SYS(n)          (((n) & 3) << 25)
#define CMU_CFG_DIV_SYS_MASK        (3 << 25)
#define CMU_CFG_DIV_SYS_SHIFT       (25)
#define CMU_EN_PLL_AUD              (1 << 27)
#define CMU_EN_PLL_USB              (1 << 28)
#define CMU_PU_PLL_AUD              (1 << 29)
#define CMU_PU_PLL_USB              (1 << 30)
#define CMU_LOCK_CPU_EN             (1 << 31)

// AUD_CLK
#define CMU_CFG_DIV_CODEC_ADC(n)    (((n) & 0xF) << 0)
#define CMU_CFG_DIV_CODEC_ADC_MASK  (0xF << 0)
#define CMU_CFG_DIV_CODEC_ADC_SHIFT (0)
#define CMU_CFG_DIV_CODEC_DAC(n)    (((n) & 0xF) << 4)
#define CMU_CFG_DIV_CODEC_DAC_MASK  (0xF << 4)
#define CMU_CFG_DIV_CODEC_DAC_SHIFT (4)
//#define CMU_POL_CLK_CODEC_ADC       (1 << 8)
#define CMU_POL_CLK_CODEC_DAC       (1 << 9)
#define CMU_EN_CLK_CODEC_ANC        (1 << 10)
#define CMU_EN_CLK_I2S_OUT          (1 << 11)
#define CMU_POL_CLK_I2S_OUT         (1 << 12)
#define CMU_POL_CLK_I2S_IN          (1 << 13)
#define CMU_POL_CLK_PCM_IN          (1 << 14)
#define CMU_CFG_DIV_I2S(n)          (((n) & 0x1FFF) << 15)
#define CMU_CFG_DIV_I2S_MASK        (0x1FFF << 15)
#define CMU_CFG_DIV_I2S_SHIFT       (15)
#define CMU_SEL_I2S_CLKIN           (1 << 28)
#define CMU_SEL_PCM_CLKIN           (1 << 29)
#define CMU_EN_CODEC_CLK_SRC        (1 << 30)
#define CMU_POL_CLK_CODEC_ADC_IN    (1 << 31)

// BOOT_DVS
#define CMU_ROM_EMA(n)              (((n) & 7) << 0)
#define CMU_ROM_EMA_MASK            (7 << 0)
#define CMU_ROM_EMA_SHIFT           (0)
#define CMU_ROM_EKN                 (1 << 3)
#define CMU_RAM_EMA(n)              (((n) & 7) << 4)
#define CMU_RAM_EMA_MASK            (7 << 4)
#define CMU_RAM_EMA_SHIFT           (4)
#define CMU_RAM_EMAW(n)             (((n) & 3) << 7)
#define CMU_RAM_EMAW_MASK           (3 << 7)
#define CMU_RAM_EMAW_SHIFT          (7)
#define CMU_SMP_CMU_SEL(n)          (((n) & 0xF) << 9)
#define CMU_SMP_CMU_SEL_MASK        (0xF << 9)
#define CMU_SMP_CMU_SEL_SHIFT       (9)

// USB_CLK
#define CMU_CFG_DIV_I2C(n)          (((n) & 0xFF) << 0)
#define CMU_CFG_DIV_I2C_MASK        (0xFF << 0)
#define CMU_CFG_DIV_I2C_SHIFT       (0)
#define CMU_SEL_I2C_PLL             (1 << 8)
#define CMU_CFG_DIV_PCM(n)          (((n) & 0x1FFF) << 9)
#define CMU_CFG_DIV_PCM_MASK        (0x1FFF << 9)
#define CMU_CFG_DIV_PCM_SHIFT       (9)
#define CMU_CFG_DIV_USB60M(n)       (((n) & 3) << 22)
#define CMU_CFG_DIV_USB60M_MASK     (3 << 22)
#define CMU_CFG_DIV_USB60M_SHIFT    (22)
#define CMU_CFG_DIV_USB48M(n)       (((n) & 3) << 24)
#define CMU_CFG_DIV_USB48M_MASK     (3 << 24)
#define CMU_CFG_DIV_USB48M_SHIFT    (24)
#define CMU_CFG_DIV_PSYS(n)         (((n) & 0xF) << 26)
#define CMU_CFG_DIV_PSYS_MASK       (0xF << 26)
#define CMU_CFG_DIV_PSYS_SHIFT      (26)
#define CMU_POL_USB_CLK             (1 << 30)
#define CMU_USB_ID                  (1 << 31)

// CLK_OUT
#define CMU_SEL_SDMMC_26M           (1 << 0)
#define CMU_SEL_SDIO_26M            (1 << 1)
#define CMU_SEL_SLCD_26M            (1 << 2)
#define CMU_SEL_SPI_26M             (1 << 3)
#define CMU_SEL_UART0_26M           (1 << 4)
#define CMU_SEL_UART1_26M           (1 << 5)
#define CMU_SEL_I2C_26M             (1 << 6)
#define CMU_SEL_SPI_ITN_26M         (1 << 7)
#define CMU_SEL_TIMER_FAST          (1 << 8)
#define CMU_SEL_DCDC_PLL            (1 << 9)
#define CMU_EN_CLK_DCDC             (1 << 10)
#define CMU_CFG_DIV_DCDC(n)         (((n) & 0xFF) << 11)
#define CMU_CFG_DIV_DCDC_MASK       (0xFF << 11)
#define CMU_CFG_DIV_DCDC_SHIFT      (11)
#define CMU_CFG_CLK_OUT(n)          (((n) & 0xF) << 19)
#define CMU_CFG_CLK_OUT_MASK        (0xF << 19)
#define CMU_CFG_CLK_OUT_SHIFT       (19)
#define CMU_CFG_CLK_OUT_EN          (1 << 23)
#define CMU_DISABLE_JTAG_I2C        (1 << 24)
#define CMU_SEL_USB_52M             (1 << 25)
#define CMU_LOCKED_PLL_USB          (1 << 26)
#define CMU_LOCKED_PLL_AUD          (1 << 27)

// PWM01_CLK
#define CMU_CFG_DIV_PWM0(n)         (((n) & 0xFFF) << 0)
#define CMU_CFG_DIV_PWM0_MASK       (0xFFF << 0)
#define CMU_CFG_DIV_PWM0_SHIFT      (0)
#define CMU_SEL_DIV_PWM0_32K        (1 << 15)
#define CMU_CFG_DIV_PWM1(n)         (((n) & 0xFFF) << 16)
#define CMU_CFG_DIV_PWM1_MASK       (0xFFF << 16)
#define CMU_CFG_DIV_PWM1_SHIFT      (16)
#define CMU_SEL_DIV_PWM1_32K        (1 << 31)

// PWM23_CLK
#define CMU_CFG_DIV_PWM2(n)         (((n) & 0xFFF) << 0)
#define CMU_CFG_DIV_PWM2_MASK       (0xFFF << 0)
#define CMU_CFG_DIV_PWM2_SHIFT      (0)
#define CMU_SEL_DIV_PWM2_32K        (1 << 15)
#define CMU_CFG_DIV_PWM3(n)         (((n) & 0xFFF) << 16)
#define CMU_CFG_DIV_PWM3_MASK       (0xFFF << 16)
#define CMU_CFG_DIV_PWM3_SHIFT      (16)
#define CMU_SEL_DIV_PWM3_32K        (1 << 31)

// WRITE_UNLOCK
#define CMU_WRITE_UNLOCK_H          (1 << 0)
#define CMU_WRITE_UNLOCK_STATUS     (1 << 1)

// WAKEUP_CLK_CFG
#define CMU_TIMER_WT26M(n)              (((n) & 0xFF) << 0)
#define CMU_TIMER_WT26M_MASK            (0xFF << 0)
#define CMU_TIMER_WT26M_SHIFT           (0)
#define CMU_TIMER_WTPLL(n)              (((n) & 0xF) << 8)
#define CMU_TIMER_WTPLL_MASK            (0xF << 8)
#define CMU_TIMER_WTPLL_SHIFT           (8)
#define CMU_LPU_AUTO_SWITCH26M          (1 << 12)
#define CMU_LPU_AUTO_SWITCHPLL          (1 << 13)
#define CMU_LPU_26M_READY               (1 << 14)
#define CMU_LPU_PLL_READY               (1 << 15)

// PSRAM_DLL
#define CMU_PSRAM_DLL_LOCK_DIV(n)   (((n) & 0xFFFF) << 0)
#define CMU_PSRAM_DLL_LOCK_DIV_MASK (0xFFFF << 0)
#define CMU_PSRAM_DLL_LOCK_DIV_SHIFT (0)
#define CMU_PSRAM_DLL_LOCK_TRIGGER  (1 << 16)
#define CMU_PSRAM_DLL_LOCK          (1 << 21)

// MISC
#define CMU_MISC_ALL_CLOCK_26M      (1 << 5)
#define CMU_MISC_FIR_CLOCK_DIV_1    (1 << 6)

#endif

