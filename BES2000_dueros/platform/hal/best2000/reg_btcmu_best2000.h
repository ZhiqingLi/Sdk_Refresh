#ifndef __REG_BTCMU_BEST2000_H__
#define __REG_BTCMU_BEST2000_H__

#include "plat_types.h"

struct BTCMU_T {
    __IO uint32_t CLK_ENABLE;           // 0x00
    __IO uint32_t CLK_DISABLE;          // 0x04
    __IO uint32_t CLK_MODE;             // 0x08
    __IO uint32_t DIV_TIMER;            // 0x0C
    __IO uint32_t RESET_SET;            // 0x10
    __IO uint32_t RESET_CLR;            // 0x14
    __IO uint32_t DIV_WDT;              // 0x18
    __IO uint32_t RESET_PULSE;          // 0x1C
         uint32_t RESERVED[0x30 / 4];   // 0x20
    __IO uint32_t ISIRQ_SET;            // 0x50
    __IO uint32_t ISIRQ_CLR;            // 0x54
};

#define BT_HCLK_A2A_CODEC               (1 << 17)
#define BT_HRST_A2A_CODEC               (1 << 17)

#endif

