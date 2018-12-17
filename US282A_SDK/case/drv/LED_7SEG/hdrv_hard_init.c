
/*******************************************************************************
*                              US282A
*                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
*                            All Rights Reserved.
*
*      <author>       <time>           
*       Wekan       2015-3-26pm            
*******************************************************************************/


#include "led_inner.h"


bool Led_sleep_falg = TRUE;


/******************************************************************************
* \par  Description: 设置显示相关寄存器
* \param[in]   none
* \param[out]  none
* \return      none
*   <author>    <time>     
*    Wekan   2015-3-26pm  
*******************************************************************************/
void hdrv_set_mfp_to_emif(void)
{
    //set sgement
    reg_writel((act_readl(MFP_CTL0) & (uint32)0xfffff800), MFP_CTL0); //A15/A14/A13/A12/A11
    reg_writel((act_readl(MFP_CTL1) & (uint32)0x007fffff), MFP_CTL1); //A10/A9/A8

    //driver cabability
    reg_writel(((act_readl(PADDRV0) &(uint32)0x0000ffff)), PADDRV0); //A15/A14/A12/A11/A10/A9/A8 drv

    //selcet high osc   
    reg_writel((act_readl(CMU_SEGLCDCLK)|(uint32)0x04), CMU_SEGLCDCLK); //LCD clock div 
    
    //set current
    reg_writel((act_readl(LED_SEG_BIAS_EN)|(uint32)0x03), LED_SEG_BIAS_EN);  //4mA
    
    //selcet hosc
    reg_writel((act_readl(CMU_SEGLCDCLK)&(uint32)0xfffffff0), CMU_SEGLCDCLK); //LCD clock div
    //4分频--注掉表示1分频
    // reg_writel((act_readl(CMU_SEGLCDCLK)|(uint32)0x03), CMU_SEGLCDCLK); //LCD clock div

}

/******************************************************************************
* \par  Description: 开户led显示功能
* \param[in]   none
* \param[out]  none
* \return      none
*   <author>    <time>     
*    Wekan   2015-3-26pm  
*******************************************************************************/
void hdrv_set_led(void)
{
    //开启控制器时钟 bit18
    reg_writel((act_readl(CMU_DEVCLKEN) | (uint32)0x040000), CMU_DEVCLKEN);
    //设置控制器为正常工作模式 bit 14
    reg_writel((act_readl(MRCR) | (uint32)0x4000), MRCR);

    //设置为7matrix模式并开启显示功能
    reg_writel((act_readl(SEG_SREEN_CTL) | (uint32)0xac), SEG_SREEN_CTL);
    Led_sleep_falg = FALSE;//转为非休眠状态
}


/******************************************************************************
* \par  Description: 初始化LED的硬件设备
* \param[in]   none
* \param[out]  none
* \return      none
*   <author>    <time>     
*    Wekan   2015-3-26pm  
*******************************************************************************/

void hdrv_hard_init(void)
{
    //led controller init
    hdrv_set_led();
    //select led segment and com
    hdrv_set_mfp_to_emif();
}


/******************************************************************************
* \par  Description: 返回唤醒前的状态
* \param[in]   参数没有意义
* \param[out]  none
* \return      FALSE休眠(被唤醒)，TRUE显示(无需唤醒)
*   <author>    <time>     
*    Wekan   2015-3-26pm  
*******************************************************************************/

uint8 sdrv_wake_up_LED_display(uint8 a,uint8 b,uint8 c)
{
    if(Led_sleep_falg != 0)
    {
        hdrv_set_led();
        return 0;
    }
    return 1;
}

/******************************************************************************
* \par  Description: 使led显示进入休眠状态
* \param[in]   参数没有意义
* \param[out]  none
* \return    
*   <author>    <time>     
*    Wekan   2015-3-26pm  
*******************************************************************************/

void sdrv_sleep_LED_display(uint8 a,uint8 b,uint8 c)
{
    
    Led_sleep_falg = TRUE;
    //设置为关模式
    reg_writel((act_readl(SEG_SREEN_CTL) & (uint32)(~0xA0)), SEG_SREEN_CTL);
    //关控制器时钟 bit18
    reg_writel((act_readl(CMU_DEVCLKEN) & (uint32)(~0x040000)), CMU_DEVCLKEN);
}


