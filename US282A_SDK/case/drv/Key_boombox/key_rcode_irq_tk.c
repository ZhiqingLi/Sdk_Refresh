/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *       info:处理tk按键中断相关功能
 *      <author>       <time>           
 *       Wekan       2015-3-23pm            
 *******************************************************************************/


#include "key_inner.h"

#if (SUPPORT_TK_DRIVER == TK_DRIVER_INNER)

#define    MAX_TK_NUM    8

//pending bit map table
uint8 tk_irq_pending_tab[MAX_TK_NUM] =
{
    //tk0 irq pending bit
    TK_IRQ_STATUS_TK0_IRQ_PD,
    //tk1 irq pending bit
    TK_IRQ_STATUS_TK1_IRQ_PD,
    //tk2 irq pending bit
    TK_IRQ_STATUS_TK2_IRQ_PD,
    //tk3 irq pending bit
    TK_IRQ_STATUS_TK3_IRQ_PD,
    //tk4 irq pending bit
    TK_IRQ_STATUS_TK4_IRQ_PD,
    //tk5 irq pending bit
    TK_IRQ_STATUS_TK5_IRQ_PD,
    //tk6 irq pending bit
    TK_IRQ_STATUS_TK6_IRQ_PD,
    //tk7 irq pending bit
    TK_IRQ_STATUS_TK7_IRQ_PD
};
//press mapping
uint8 tk_press_pending_tab[MAX_TK_NUM] =
{
    //press tk0 pending bit
    TK_IRQ_STATUS_TK0_PRESS,
    //press tk1 pending bit
    TK_IRQ_STATUS_TK1_PRESS,
    //press tk2 pending bit
    TK_IRQ_STATUS_TK2_PRESS,
    //press tk3 pending bit
    TK_IRQ_STATUS_TK3_PRESS,
    //press tk4 pending bit
    TK_IRQ_STATUS_TK4_PRESS,
    //press tk5 pending bit
    TK_IRQ_STATUS_TK5_PRESS,
    //press tk6 pending bit
    TK_IRQ_STATUS_TK6_PRESS,
    //press tk7 pending bit
    TK_IRQ_STATUS_TK7_PRESS
};


/******************************************************************************
 * \par  Description:   tk中断服务处理
 * \param[in]     none
 * \param[out]   none
 * \return           none
 * \note     
 *   <author>    <time>           <version >             <desc>
 *    Wekan   2015-3-23pm         1.0             review this file
 *******************************************************************************/

void key_tk_int_process(void)
{
    uint8 i;
    //check tk irq
    for (i = 0; i < MAX_TK_NUM; i++)
    {
        if ((act_readl(TK_IRQ_STATUS) & (1 << tk_irq_pending_tab[i])) != 0)
        {
            //clear pending bit
            act_writel((act_readl(TK_IRQ_STATUS) | (1 << tk_irq_pending_tab[i])), TK_IRQ_STATUS);
            //check press status
            if ((act_readl(TK_IRQ_STATUS) & (1 << tk_press_pending_tab[i])) != 0)
            {
                //send tk
                g_tk_key = 1;
                //send press message
                g_tk_oldkey_val = i;
                //counter
                g_tk_tmp_count = 0;
                
                post_key_msg(KEY_TYPE_TK, KEY_PHY_DOWN, g_tk_oldkey_val);
                //just check one key every interrupt
                break;
            }
        }
    }
}

/******************************************************************************
 * \par  Description:   检测tk是否抬起
 * \param[in]     none
 * \param[out]   none
 * \return           none
 * \note     
 *   <author>    <time>           <version >             <desc>
 *    Wekan   2015-3-23pm         1.0             review this file
 *******************************************************************************/

bool key_tk_up_status_check(void)
{
    bool result = FALSE;

    if (g_tk_oldkey_val < MAX_TK_NUM)
    {
        //check tk press pending clear
        if ((act_readl(TK_IRQ_STATUS) & (1 << tk_press_pending_tab[g_tk_oldkey_val])) == 0)
        {
            //press up
            result = TRUE;
        }
    }
    
    return result;
}

#endif
