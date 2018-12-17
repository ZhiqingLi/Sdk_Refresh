

/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                             All Rights Reserved.
 *      Description: 退出按键驱动
 *      <author>       <time>           
 *       Wekan       2015-3-26pm            
 *******************************************************************************/


#include "key_inner.h"

/******************************************************************************
 * \par  Description:   退出按键驱动
 * \param[in]     none
 * \param[out]   none
 * \return           none
 * \note     
 *   <author>    <time>    
 *    Wekan   2015-3-26pm  
 *******************************************************************************/
void key_exit(void)
{
    //release scan timer irq
    sys_del_irq_timer1(g_time_irq);
    
    //release touch key irq
    #if (SUPPORT_TK_DRIVER == TK_DRIVER_INNER)
    sys_free_irq(IRQ_TOUCHKEY);
    #endif
    
    //release ir irq
    #if (SUPPORT_IR_DRIVER == 1)
    sys_free_irq(IRQ_UART1);
    #endif
}
module_exit(key_exit)
