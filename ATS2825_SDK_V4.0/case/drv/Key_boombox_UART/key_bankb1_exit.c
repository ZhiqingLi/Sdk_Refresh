/*******************************************************************************
 *                              US280A
 *                            Module: Key driver
 *                 Copyright(c) 2003-2013 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       johnsen     2013-5-18 11:09    1.0             build this file
 *******************************************************************************/
/*!
 * \file     key_bankb_exit.c
 * \brief    key driver exit functions
 * \author   johnsen
 * \version 1.0
 * \date  2013-5-18 11:09
 *******************************************************************************/

#include "key_inner.h"

/******************************************************************************/
/*!
 * \par  Description:
 *    key exit
 * \param[in]    none
 * \param[in]    none
 * \param[in]    none
 * \param[out]   none
 * \ingroup      key drvier
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 * 负责中断向量卸载
 *******************************************************************************/
void key_exit(void)
{

    //clr 中断
    sys_free_irq(IRQ_UART1);

    sys_del_irq_timer1(g_time_irq);

}
module_exit(key_exit)
