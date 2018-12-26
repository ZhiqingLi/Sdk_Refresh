#include "common_func.h"

void i2s_mfp_cfg(void)
{
    //i2s mclk- B9
    *((REG32)(GPIOBOUTEN)) &= (~(1 << 9));
    *((REG32)(GPIOBINEN))  &= (~(1 << 9));
    *((REG32)(MFP_CTL3)) &= (~MFP_CTL3_GPIOB9_MASK);
    *((REG32)(MFP_CTL3)) |= (0x02 << MFP_CTL3_GPIOB9_SHIFT);

    //i2s bclk- B8
    *((REG32)(GPIOBOUTEN)) &= (~(1 << 8));
    *((REG32)(GPIOBINEN))  &= (~(1 << 8));
    *((REG32)(MFP_CTL3)) &= (~MFP_CTL3_GPIOB8_MASK);
    *((REG32)(MFP_CTL3)) |= (0x02 << MFP_CTL3_GPIOB8_SHIFT);

    //i2s lrclk- A6
    *((REG32)(GPIOAOUTEN)) &= (~(1 << 6));
    *((REG32)(GPIOAINEN))  &= (~(1 << 6));
    *((REG32)(MFP_CTL1)) &= (~MFP_CTL1_GPIOA6_MASK);
    *((REG32)(MFP_CTL1)) |= (0x03 << MFP_CTL1_GPIOA6_SHIFT);

#if defined(BOARD_JL_36410_FAR)
    //i2s din - B7
    *((REG32)(GPIOBOUTEN)) &= (~(1 << 7));
    *((REG32)(GPIOBINEN))  &= (~(1 << 7));
    *((REG32)(MFP_CTL3)) &= (~MFP_CTL3_GPIOB7_MASK);
    *((REG32)(MFP_CTL3)) |= (0x03 << MFP_CTL3_GPIOB7_SHIFT);
#else
    // SIO9 (VROS)  Input Enable, Output Disable
    *((REG32)(DEBUGSIOIE)) |= (1 << 9);
    *((REG32)(DEBUGSIOOE)) &= (~(1 << 9));

    //i2s din- VROS
    *((REG32)(MFP_CTL3)) &= (~MFP_CTL3_VROS_MASK);
    *((REG32)(MFP_CTL3)) |= (1 << MFP_CTL3_VROS_SHIFT);
#endif
}
