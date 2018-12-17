#include "ccd_inner.h"

//驱动装载时初始化
//pa_param : byte0 -- postgain
//           byte1 -- drc_enable
int ccd_Init(void *null0, void *null1, uint32 pa_param)
{
    #if (SPEAKER_D_PA_RESET_LEVEL != ENABLE_FIX_ACTIVE)
    act_writel(act_readl(SPEAKER_D_PA_RESET_GPIOOUT) | SPEAKER_D_PA_RESET_PIN, SPEAKER_D_PA_RESET_GPIOOUT);
    #endif
    act_writel(act_readl(SPEAKER_D_PA_EN_GPIOOUT) | SPEAKER_D_PA_EN_PIN, SPEAKER_D_PA_EN_GPIOOUT);

    #if (SPEAKER_D_PA_EN_LEVEL == ENABLE_LEVEL_HIGH)
    act_writel((act_readl(SPEAKER_D_PA_EN_GPIODATA) | SPEAKER_D_PA_EN_PIN), SPEAKER_D_PA_EN_GPIODATA);
    #else
    act_writel((act_readl(SPEAKER_D_PA_EN_GPIODATA) & (~SPEAKER_D_PA_EN_PIN)), SPEAKER_D_PA_EN_GPIODATA);
    #endif

    #if (SPEAKER_D_PA_RESET_LEVEL == ENABLE_LEVEL_HIGH)
    act_writel((act_readl(SPEAKER_D_PA_RESET_GPIODATA) & (~SPEAKER_D_PA_RESET_PIN)), SPEAKER_D_PA_RESET_GPIODATA);
    sys_udelay(200);
    act_writel((act_readl(SPEAKER_D_PA_RESET_GPIODATA) | SPEAKER_D_PA_RESET_PIN), SPEAKER_D_PA_RESET_GPIODATA);
    sys_udelay(200);
    act_writel((act_readl(SPEAKER_D_PA_RESET_GPIODATA) & (~SPEAKER_D_PA_RESET_PIN)), SPEAKER_D_PA_RESET_GPIODATA);
    #elif (SPEAKER_D_PA_RESET_LEVEL == ENABLE_LEVEL_LOW)
    act_writel((act_readl(SPEAKER_D_PA_RESET_GPIODATA) | SPEAKER_D_PA_RESET_PIN), SPEAKER_D_PA_RESET_GPIODATA);
    sys_udelay(200);
    act_writel((act_readl(SPEAKER_D_PA_RESET_GPIODATA) & (~SPEAKER_D_PA_RESET_PIN)), SPEAKER_D_PA_RESET_GPIODATA);
    sys_udelay(200);
    act_writel((act_readl(SPEAKER_D_PA_RESET_GPIODATA) | SPEAKER_D_PA_RESET_PIN), SPEAKER_D_PA_RESET_GPIODATA);
    #else
    ; //nothing
    #endif

    g_i2s_pa_status = 0;
    g_i2s_pa_delay_twi = 20;
    g_i2s_pa_drc_enable = (uint8) pa_param;

    return 0;
}

//卸载驱动时调用
int ccd_Exit(void *null0, void *null1, void *null2)
{
    if (g_i2s_pa_status == 1)
    {
        pa_op_powerdown();
    }
    #if (SPEAKER_D_PA_RESET_LEVEL == ENABLE_LEVEL_HIGH)
    //act_writel((act_readl(SPEAKER_D_PA_RESET_GPIODATA) & (~SPEAKER_D_PA_RESET_PIN)), SPEAKER_D_PA_RESET_GPIODATA);
    //sys_udelay(200);
    act_writel((act_readl(SPEAKER_D_PA_RESET_GPIODATA) | SPEAKER_D_PA_RESET_PIN), SPEAKER_D_PA_RESET_GPIODATA);
    //sys_udelay(200);
    //act_writel((act_readl(SPEAKER_D_PA_RESET_GPIODATA) & (~SPEAKER_D_PA_RESET_PIN)), SPEAKER_D_PA_RESET_GPIODATA);
    #elif (SPEAKER_D_PA_RESET_LEVEL == ENABLE_LEVEL_LOW)
    //act_writel((act_readl(SPEAKER_D_PA_RESET_GPIODATA) | SPEAKER_D_PA_RESET_PIN), SPEAKER_D_PA_RESET_GPIODATA);
    //sys_udelay(200);
    act_writel((act_readl(SPEAKER_D_PA_RESET_GPIODATA) & (~SPEAKER_D_PA_RESET_PIN)), SPEAKER_D_PA_RESET_GPIODATA);
    //sys_udelay(200);
    //act_writel((act_readl(SPEAKER_D_PA_RESET_GPIODATA) | SPEAKER_D_PA_RESET_PIN), SPEAKER_D_PA_RESET_GPIODATA);
    #else
    ; //nothing
    #endif
    
    //diable 
    #if (SPEAKER_D_PA_EN_LEVEL == ENABLE_LEVEL_HIGH)
    act_writel((act_readl(SPEAKER_D_PA_EN_GPIODATA) & (~SPEAKER_D_PA_EN_PIN)), SPEAKER_D_PA_EN_GPIODATA);
    #else
    act_writel((act_readl(SPEAKER_D_PA_EN_GPIODATA) | SPEAKER_D_PA_EN_PIN), SPEAKER_D_PA_EN_GPIODATA);
    #endif
    

    return 0;
}

module_init(ccd_Init)
module_exit(ccd_Exit)
