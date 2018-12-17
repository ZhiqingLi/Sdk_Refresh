/*******************************************************************************
*                              US282A
*                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
*                            All Rights Reserved.
*     brief    led显示，供AP调用
*      <author>       <time>           
*       Wekan       2015-3-26pm            
*******************************************************************************/


#include "led_inner.h"
uint32 bak_seg_sreen_data0;
uint32 bak_seg_sreen_data1;

/******************************************************************************
* \par  Description: 闪烁屏幕
* \param[in]   onoff --1表示显示原数据，0表示清除原数据
* \param[out]  none
* \return      none
* \note:闪烁必须首先调用关闭，然后调用显示原数据
*   <author>    <time>     
*    Wekan   2015-3-26pm  
*******************************************************************************/

void sdrv_flash_screen(uint8 onoff, void *null2, void *null3)
{
    if (0 == onoff)
    {
        bak_seg_sreen_data0 = act_readl(SEG_SREEN_DATA0);
        bak_seg_sreen_data1 = (act_readl(SEG_SREEN_DATA1));
        reg_writel(0, SEG_SREEN_DATA0);
        reg_writel(0, SEG_SREEN_DATA1);
    }
    else
    {
        reg_writel((act_readl(SEG_SREEN_DATA0) |(uint32)bak_seg_sreen_data0), SEG_SREEN_DATA0);
        reg_writel((act_readl(SEG_SREEN_DATA1) |(uint32)bak_seg_sreen_data1), SEG_SREEN_DATA1);
    }
    //wait for data trans end
    wait_for_trans();
}

