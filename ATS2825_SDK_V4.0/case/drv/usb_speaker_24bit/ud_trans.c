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
void data_convert2(uint8 *src_addr, uint8 *dst_addr, uint32 data_lenth)
{
    uint32 ptr, cnt = 0;
    if(g_ud_stop_by_tts == 1) {
        return;
    }

    while(data_lenth >= 3) {
        data_lenth -= 3;
        *dst_addr++ = 0;
        *dst_addr++ = *src_addr++;
        *dst_addr++ = *src_addr++;
        *dst_addr++ = *src_addr++;
        cnt++;
    }
    ptr = cnt * 4;

}
void data_convert(uint32 *src_addr, uint32 *dst_addr, uint32 data_lenth)
{
    uint32 i;
    uint32 temp[3] = {0, 0, 0};

    data_lenth >>= 2; //Fifodat是32位
    for(i = 0; i < (data_lenth / 3); i++) {
        temp[0] = *src_addr;
        src_addr++;
        temp[1] = *src_addr;
        src_addr++;
        temp[2] = *src_addr;
        src_addr++;
        *dst_addr = ((temp[0] << 8) & 0xffffff00);
        dst_addr++;
        *dst_addr = (((temp[0] >> 16) | (temp[1] << 16)) & 0xffffff00);
        dst_addr++;
        *dst_addr = (((temp[1] >> 8) | (temp[2] << 24)) & 0xffffff00);
        dst_addr++;
        *dst_addr = (temp[2] & 0xffffff00);
        dst_addr++;
    }
}
void access_usbfifo_by_cpu(uint8 epsel, uint32 *address, uint32 data_lenth)
{
    uint32 i;
    uint32 temp;

    if(g_mute_delay != 0) {
        if(((data_lenth == 0x108) && (g_play_sample == 44100)) \
                || ((data_lenth == 0x120) && (g_play_sample == 48000)) \
                || ((data_lenth == 0x240) && (g_play_sample == 96000))) \
        {
            g_mute_delay --;
        } else {
            g_mute_delay = 3;
            libc_print("clear", 0, 0);
        }
    }
    //g_mute_delay = 0;
    //uint32 temp_data;
    while((data_lenth % 4) != 0) {
        data_lenth += 2;        //24bit的数据需要补齐
    }

    data_lenth >>= 2; //Fifodat是32位

    for(i = 0; i < data_lenth; i++) {
        if(epsel == EP1OUT) {
            if((g_ud_stop_by_tts == 0) && (g_mute_flag == 0) && (g_mute_delay == 0)) {
                *address = act_readl(Fifo1dat);
            } else {
                temp = act_readl(Fifo1dat);         //丢弃
            }
#if 0
            if((*address == 0) || (*address == 0xffffffff)) {
                usb_energy_low_count++;
            } else {
                usb_energy_low_count = 0;
                g_usb_data_count = 500;
            }
            if(usb_energy_low_count >= 20) {
                g_usb_data_count = 0;
                usb_energy_low_count = 0;
            }
#endif
        } else if(epsel == EP1IN) {
            act_writel(*address, Fifo1dat);
        } else if(epsel == EP2OUT) {
            *address = act_readl(Fifo2dat);
        } else {
            ;//for qac
        }
        if((i % 10) == 0) {
            if((*address == 0) || (*address == 0xffffffff) || (address == 0x01)) {
                g_low_energy_count ++;
                //g_energy_count = 0;
            } else {
                //g_energy_count ++;
                g_low_energy_count = 0;
            }
        }
        address++;
    }
}


