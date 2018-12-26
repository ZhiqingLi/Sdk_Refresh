/********************************************************************************
 *                              US280A
 *                            Module: USB device
 *                 Copyright(c) 2001-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>            <version >             <desc>
 *      fiona 2015-01-18 16:40     1.0                build this file
 ********************************************************************************/
#include "ud_intern.h"

/******************************************************************************/
/*!
 * \par  Description:
 *   deal hid intoken intrrupt.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_irq.c
 * \par
 * \note
 *******************************************************************************/
void hid_out_token_isr(void)
{

    uint32 data_lenth, get_len, temp_low, temp_high;

    while(0 != (act_readb(Out1cs_hcin1cs) & 0x02)) {
        if(USB_CONNECT_STATUS == 0x00) {
            g_usb_state = 0x01;

            break;
        }
    }

    temp_low = act_readb(Out1bcl_hcin1bcl);

    temp_high = act_readb(Out1bch_hcin1bch);

    data_lenth = (uint32)((temp_high << 8) | temp_low);

    while(data_lenth > 0) {
        if(data_lenth > HID_REPORT_SIZE) {
            get_len = HID_REPORT_SIZE;
        } else {
            get_len = data_lenth;
        }

        data_lenth -= get_len;

        access_usbfifo_by_cpu(EP1OUT, (uint32 *) g_hid_out_buf, get_len);
    }

    act_writeb(act_readb(Out1cs_hcin1cs) | 0x02, Out1cs_hcin1cs);
    //return;
}

/******************************************************************************/
/*!
 * \par  Description:
 *   deal hid intoken intrrupt.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_irq.c
 * \par
 * \note
 *******************************************************************************/
void hid_in_token_isr(void)
{

    if(g_hid_change == 1) {

        access_usbfifo_by_cpu(EP1IN, (uint32 *) g_hid_in_buf, HID_REPORT_SIZE);

        act_writeb(act_readb(In1cs_hcout1cs) | 0x02, In1cs_hcout1cs);

        while(0 != (act_readb(In1cs_hcout1cs) & 0x02)) {
            if(USB_CONNECT_STATUS == 0x00) {
                g_usb_state = 0x01;
                break;
            }
        }

        g_hid_change = 0;
    }

    //return;
}

/******************************************************************************/
/*!
 * \par  Description:
 *   USB intrrupt entry.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_irq.c
 * \par
 * \note
 *******************************************************************************/
void uhid_isr(void)
{
    if((0 != ((act_readb(OUT03TOKIRQ)) & 0x02))) { //ep1 int out
        act_writeb((uint8)((0x80 | act_readb(USBEIRQ)) & 0x8f), USBEIRQ);
        act_writeb(0x02, OUT03TOKIRQ);
        hid_out_token_isr();
    } else if((0 != ((act_readb(IN03TOKIRQ)) & 0x02))) { //ep1 int in
        act_writeb((uint8)((0x80 | act_readb(USBEIRQ)) & 0x8f), USBEIRQ);
        act_writeb(0x02, IN03TOKIRQ);
        hid_in_token_isr();
    } else if(0 != ((act_readb(Usbirq_hcusbirq)) & 0x01)) { //setup
        act_writeb((uint8)((0x80 | act_readb(USBEIRQ)) & 0x8f), USBEIRQ);
        act_writeb(0x01 | act_readb(Usbirq_hcusbirq), Usbirq_hcusbirq);
        deal_setup();
    } else {
        ;//do nothing
    }

    return;
}

