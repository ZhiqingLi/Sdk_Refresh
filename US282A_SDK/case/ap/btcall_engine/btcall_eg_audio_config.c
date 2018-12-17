
/*******************************************************************************
*                              US282A
*                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
*                            All Rights Reserved.
*        brief   配置打电话场景下的模块dac,dma等
*      Cailizhen          2015-1          creat
*       Wekan        2015-3-30       review
*******************************************************************************/

#include "btcall_engine.h"


/******************************************************************************
* \par  Description:   启动dac,dma等
* \param[in]     null
* \param[out]   null
* \return           none
* \note     
* \  <author>       <time>        <opt>
* \  Cailizhen          2015-1          creat
* \   Wekan       2015-3-30     reviwe
*******************************************************************************/

void __section__(".text.dma_dac_start_transfer") dma_dac_start_transfer(void)
{
    while ((act_readl(DMAIP) & 0x00000808) != 0)
    {
        //clear pending，注意不能使用读-或-写这种方式，避免清掉其他pending位
        act_writel(0x00000808, DMAIP);
    }
    if(0 == g_app_info_state_all.fix_sample_rate_flag)
    {
        //reload enable
        act_writel(act_readl(DMA3CTL) | (1 << DMA3CTL_reload), DMA3CTL);
    }
    //enable transfer
    act_writel(act_readl(DMA3CTL) | (1 << DMA3CTL_DMA3START), DMA3CTL);
}


/******************************************************************************
* \par  Description:   启动相关模块，dac,dma等
* \param[in]     null
* \param[out]   null
* \return           none
* \note     
*   <author>       <time>        <opt>
*    Wekan       2015-3-30     reviwe
*******************************************************************************/

void callring_dac_dma_config(void)
{
    //enalbe dac fifo
    //    act_writel(act_readl(DAC_FIFOCTL) | (~(1<<DAC_FIFOCTL_DAF0RT)), DAC_FIFOCTL);
    
    act_writel(0, DMA3CTL);
    //enable IRQ
    sys_request_irq(IRQ_DMA3, ring_dma_isr);
    //data width 16bit
    act_writel(act_readl(DMA3CTL) | (1 << DMA3CTL_DATAWIDTH_SHIFT), DMA3CTL);
    //source addr type:memory
    act_writel(act_readl(DMA3CTL) | (0 << DMA3CTL_SRCTYPE_SHIFT), DMA3CTL);
    //dest addr type:dac fifo0
    if(1 == g_app_info_state_all.fix_sample_rate_flag)
    {
        act_writel(act_readl(DMA3CTL) | (13 << DMA3CTL_DSTTYPE_SHIFT), DMA3CTL);
    }
    else
    {
        act_writel(act_readl(DMA3CTL) | (11 << DMA3CTL_DSTTYPE_SHIFT), DMA3CTL);
    }
    //reload enable
    //act_writel(act_readl(DMA3CTL) | (1 << DMA3CTL_reload), DMA3CTL);
    //source address
    act_writel((uint32)g_btcall_callring_dac_buf, DMA3SADDR0);
    //dma length
    act_writel(CALLRING_DAC_BUFFER_LENGTH / 2, DMA3FrameLen);
    //enable dma0 half & full trans interupt
    act_writel(act_readl(DMAIP)|0x00000808, DMAIP);
    act_writel(act_readl(DMAIE)|0x00000808, DMAIE);
    
    //enalbe DAC FIFO Empty DRQ
    act_writel(act_readl(DAC_FIFOCTL) | (1 << DAC_FIFOCTL_DAF0EDE), DAC_FIFOCTL);
    //enable transfer
    //act_writel(act_readl(DMA0CTL)|(1 << DMA0CTL_DMA0START), DMA0CTL);
}


