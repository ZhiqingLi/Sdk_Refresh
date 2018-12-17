#include "fm_drv.h"
#include "TWI.h"
#include "case_type.h"
#include "fm_qn8035.h"


//fm驱动装载时调用：设置fm的时钟
int fmdrv_init(void* null1, void* null2, FM_init_parm_t *p_arg)
{
    libc_memcpy((uint8*)&fm_init_parm, (uint8*)p_arg, sizeof(FM_init_parm_t));

    if(fm_init_parm.freq_mode == 1)
    {   
        //将GPIOA7设置为FMCLKOUT
        act_writel((act_readl(MFP_CTL1) & (~MFP_CTL1_GPIOA7_MASK)) | (5 << MFP_CTL1_GPIOA7_SHIFT), MFP_CTL1);
        fm_init_parm.gpio_level &= 0x07;
        act_writel((act_readl(PADDRV0) & (~PADDRV0_GPIOA7_MASK)) | (fm_init_parm.gpio_level << PADDRV0_GPIOA7_SHIFT), PADDRV0);
        act_writel(1, CMU_FMCLK);//13M
        //使能FM CLOCK
        act_writel(act_readl(CMU_DEVCLKEN) | (1 << CMU_DEVCLKEN_FMCLKEN), CMU_DEVCLKEN);
    }
    return 0;
}

//fm驱动卸载时调用
int fmdrv_exit(void* null1, void* null2, void* null3)
{
    act_writel(act_readl(CMU_DEVCLKEN) & (~(1 << CMU_DEVCLKEN_FMCLKEN)), CMU_DEVCLKEN);
    return 0;
}

module_init(fmdrv_init)
module_exit(fmdrv_exit)

