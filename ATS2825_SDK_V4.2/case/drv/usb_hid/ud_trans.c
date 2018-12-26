/********************************************************************************
 *                              US280A
 *                            Module: USB device
 *                 Copyright(c) 2001-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>            <version >             <desc>
 *      dengtaiping 2013-01-18 16:40     1.0                build this file
 ********************************************************************************/
#include "ud_intern.h"

/******************************************************************************/
/*!
 * \par  Description:
 *   CPU方式访问USB FIFO .
 * \param[in]
 * \return  none
 * \retval  none
 * \ingroup ud_trans.c
 * \par
 * \note
 *******************************************************************************/
void access_usbfifo_by_cpu(uint8 epsel, uint32 *address, uint32 data_lenth)
{
    uint32 i;
    //uint32 temp_data;

    data_lenth >>= 2; //Fifodat是32位

    for(i = 0; i < data_lenth; i++) {
        if(epsel == EP1OUT) {
            *address = act_readl(Fifo1dat);
        } else if(epsel == EP1IN) {
            act_writel(*address, Fifo1dat);
        } else if(epsel == EP2OUT) {
            *address = act_readl(Fifo2dat);
        } else {
            ;//for qac
        }
        address++;
    }
}
