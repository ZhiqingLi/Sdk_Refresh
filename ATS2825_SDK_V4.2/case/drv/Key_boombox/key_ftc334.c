#include "key_inner.h"

#if defined(TK_DRIVER_FTC334F)

void __section__(".bank0") key_ftc334_init(void)
{
    //init these pin in welcome.c
    //act_writel(act_readl(FTC_GPIO_SCAN_PIN_IN_EN) & (~FTC_GPIO_SCAN_PIN_NUMBER), FTC_GPIO_SCAN_PIN_IN_EN);
    act_writel(act_readl(FTC_GPIO_SCAN_PIN_OUT_EN) | FTC_GPIO_SCAN_PIN_NUMBER, FTC_GPIO_SCAN_PIN_OUT_EN);
    //act_writel(act_readl(FTC_GPIO_SCAN_PIN_PU_EN) | FTC_GPIO_SCAN_PIN_NUMBER, FTC_GPIO_SCAN_PIN_PU_EN);
    //act_writel(act_readl(GPIOAPDEN) | FTC_GPIO_SCAN_PIN_NUMBER, GPIOAPDEN);

    //act_writel(act_readl(FTC_GPIO_KEY_DATA_OUT0_OUT_EN) & (~FTC_GPIO_KEY_DATA_OUT0_PIN), FTC_GPIO_KEY_DATA_OUT0_OUT_EN);
    act_writel(act_readl(FTC_GPIO_KEY_DATA_OUT0_IN_EN) | FTC_GPIO_KEY_DATA_OUT0_PIN, FTC_GPIO_KEY_DATA_OUT0_IN_EN);
    //act_writel(act_readl(FTC_GPIO_KEY_DATA_OUT0_PD_EN) | FTC_GPIO_KEY_DATA_OUT0_PIN, FTC_GPIO_KEY_DATA_OUT0_PD_EN);

    //act_writel(act_readl(FTC_GPIO_KEY_DATA_OUT1_OUT_EN) & (~FTC_GPIO_KEY_DATA_OUT1_PIN), FTC_GPIO_KEY_DATA_OUT1_OUT_EN);
    act_writel(act_readl(FTC_GPIO_KEY_DATA_OUT1_IN_EN) | FTC_GPIO_KEY_DATA_OUT1_PIN, FTC_GPIO_KEY_DATA_OUT1_IN_EN);
    //act_writel(act_readl(FTC_GPIO_KEY_DATA_OUT1_PD_EN) | FTC_GPIO_KEY_DATA_OUT1_PIN, FTC_GPIO_KEY_DATA_OUT1_PD_EN);
}

uint8 read_ftc334_data(void)
{
    uint16 val = 0;
    static uint16 last_val = 0;

    act_writel(act_readl(FTC_GPIO_SCAN_PIN_DATA) | FTC_GPIO_SCAN_PIN_NUMBER, FTC_GPIO_SCAN_PIN_DATA);
    //sys_usleep(1000);
    //key_tk_delay(10);
    sys_udelay(10);
    if(act_readl(FTC_GPIO_KEY_DATA_OUT0_DATA) & (FTC_GPIO_KEY_DATA_OUT0_PIN)) {
        val = 1;
    }
    if(act_readl(FTC_GPIO_KEY_DATA_OUT1_DATA) & (FTC_GPIO_KEY_DATA_OUT1_PIN)) {
        val |= 0x02;
    }

    act_writel(act_readl(FTC_GPIO_SCAN_PIN_DATA) & (~FTC_GPIO_SCAN_PIN_NUMBER), FTC_GPIO_SCAN_PIN_DATA);
    //sys_usleep(1000);
    //key_tk_delay(10);
    sys_udelay(10);
    if(act_readl(FTC_GPIO_KEY_DATA_OUT0_DATA) & (FTC_GPIO_KEY_DATA_OUT0_PIN)) {
        val |= 0x04;
    }
    if(act_readl(FTC_GPIO_KEY_DATA_OUT1_DATA) & (FTC_GPIO_KEY_DATA_OUT1_PIN)) {
        val |= 0x08;
    }
    /*
        if(val != last_val) {
            last_val = val;
            libc_print("key: ", val, 2);
        }
    */
    val &= 0x0F;
#if defined(BOARD_JL_36410_FAR)
    return (val >= 4 && val != 0x0F) ? (val - 4) : 0xFF;
#else
    return (val != 0x0F) ? val : 0xFF;
#endif
}

uint8 key_ftc334_check(void)
{
    uint16 val;
    if(read_ftc334_data() == 0xFF)
        return NO_KEY;

    return (uint8)val;
}
#endif
