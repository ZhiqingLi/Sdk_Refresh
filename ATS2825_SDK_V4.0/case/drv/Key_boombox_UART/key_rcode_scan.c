/*******************************************************************************
 *                              US280A
 *                            Module: Key driver
 *                 Copyright(c) 2003-2013 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       johnsen     2013-5-18 10:28    1.0             build this file
 *******************************************************************************/
/*!
 * \file     key_rcode_scan.c
 * \brief    key scan and message deal
 * \author   johnsen
 * \version 1.0
 * \date  2013-5-18 10:28
 *******************************************************************************/

#include "key_inner.h"

int g_time_irq;

/******************************************************************************/
/*!
 * \par  Description:
 *    key scan of times
 * \param[in]    none
 * \param[in]    none
 * \param[in]    none
 * \param[out]   none
 * \return       the result
 * \retval           0
 * \retval           1
 * \ingroup      key drvier
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 * 按键扫描以及充电轮询，每隔20ms扫描一次，按键已经含有去抖处理
 *******************************************************************************/
void status_scan(void)
{
    //need reply
    if((get_communicate_status() == COMUNICATE_REPLY) && (get_send_status() != SEND_STATUS_END)) {
        //send data to physical bus
        load_answer_data();
    }

}
