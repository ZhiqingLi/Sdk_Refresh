

/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                             All Rights Reserved.
 *      Description:处理红外按键，中断和状态查询
 *      <author>       <time>           
 *       Wekan       2015-3-26pm            
 *******************************************************************************/

#include "key_inner.h"

/******************************************************************************
 * \par  Description:   红外中断服务程序
 * \param[in]     none
 * \param[out]   none
 * \return           none
 * \note     
 *   <author>    <time>  
 *    Wekan   2015-3-26pm 
 *******************************************************************************/

void key_ir_int_process(void)
{
    #if (SUPPORT_IR_DRIVER == 1)
    uint8 key_data;
    
    //check whether is ir interrupt
    if((act_readl(IRC_STA) & (1 << IRC_STA_IIP)) == 0)
    {
        return ;
    }
    //clear irq pending
    act_writel((1 << IRC_STA_IIP), IRC_STA);

    //repeat code detcet
    if ((act_readl(IRC_STA) & (1 << IRC_STA_RCD)) != 0)
    {
        //write 1 to this bit to clear  repeat flag
        act_writel((1 << IRC_STA_RCD), IRC_STA);
        
        g_ir_key = 1;
        g_ir_tmp_count = 0;
        post_key_msg(KEY_TYPE_IR, KEY_PHY_CONT, g_ir_oldkey_val);
        
        return;
    }

    key_data = act_readb(IRC_KDC); //read key data value
    g_ir_oldkey_val = key_data;
    g_ir_key = 1;
    g_ir_tmp_count = 0;
    post_key_msg(KEY_TYPE_IR, KEY_PHY_DOWN, g_ir_oldkey_val);
    #endif
}


/******************************************************************************
 * \par  Description:   检测红外寄存器状态,进行用户码的切换
 * \param[in]     none
 * \param[out]   none
 * \return           none
 * \note     
 *   <author>    <time> 
 *    Wekan   2015-3-26pm
 *******************************************************************************/
void key_ir_status_check(void)
{
    #if (SUPPORT_IR_DRIVER == 1)
    
    //检测到错误的遥控按键用户码后自动切换用户码
    if ((act_readl(IRC_STA) & (1 << IRC_STA_UCMP)) != 0)
    {
        uint32 tmp;
        
        tmp = act_readl(IRC_CC)>>16;
        act_writel(tmp, IRC_CC);
        act_writel((act_readl(IRC_STA) | (1 << IRC_STA_UCMP)), IRC_STA);
        //sys_irq_print("w-ir_cc=",tmp,2);
        
        if (tmp == 0x7F80)
        {
            g_current_usercode = IR_CODE_7F80;
        }
        else if (tmp == 0xBF00)
        {
            g_current_usercode = IR_CODE_BF00;
        }
        else if (tmp == 0xFF00)
        {
            g_current_usercode = IR_CODE_FF00;
        }
        else
        {
            g_current_usercode = IR_CODE_C33C;
        }
    }
    #endif
} 
