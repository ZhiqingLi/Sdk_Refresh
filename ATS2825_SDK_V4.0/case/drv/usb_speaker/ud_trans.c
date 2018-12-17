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
    uint32 temp_data = 0;
    //libc_print("",data_lenth,2);
    //必须在数据包为采样点的整数倍时才能开始收数

    if((g_mute_delay != 0) && (epsel == EP1OUT)) {
        if(((data_lenth == 0xb0) && (g_dac_sample == 44100)) \
                || ((data_lenth == 0xc0) && (g_dac_sample == 48000))\
                || ((data_lenth == 0x180) && (g_dac_sample == 96000))) {
            g_mute_delay --;
        } else {
            g_mute_delay = 3;
            libc_print("clear", 0, 0);
        }
    }


    data_lenth >>= 2; //Fifodat是32位

    for(i = 0; i < data_lenth; i++) {
        if(epsel == EP1OUT) {
            if((g_tts_start_flag == 1) || (g_mute_flag == 1) || (g_mute_delay != 0)) {
                temp_data = act_readl(Fifo1dat);

                //libc_print("",temp_data,2);

                //*address = 0;
            } else {
                *address = act_readl(Fifo1dat);
                ////检查USBfifo空的次数，用于判断ESD后，USB获取数据是否异常
                if((*address != 0xffff)
                        && (*address != 0)
                        && (*address != 1)) {
                    if(g_no_empty_count < 50) {
                        g_no_empty_count++;
                    }
                } else {
                    g_no_empty_count = 0;
                }
            }
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
