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
 *   deal some data
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_msc.c
 * \par
 * \note
 *******************************************************************************/
void reset_data_pos(void)
{
    g_rd_play_ptr = 0;

    g_play_data_count = 0;

    g_wr_play_ptr = 0;

    g_manul_stop = 0;

    g_reset_flag = 1;
}

/******************************************************************************/
/*!
 * \par  Description:
 *   deal USB disconnect.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_msc.c
 * \par
 * \note
 *******************************************************************************/
void usb_disconnect_isr(void)
{
    if(USB_CONNECT_STATUS != 0x00) {
        g_usb_state = 0x01;
    }

}

/******************************************************************************/
/*!
 * \par  Description:
 *  deal iso outtoken intrrupt.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_irq.c
 * \par
 * \note
 *******************************************************************************/
void iso_out_token_isr(void)
{
    uint32 data_lenth, get_lenth, buf_addr, temp_data = 0;

    uint32 temp_low, temp_high;

    //act_writel(act_readl(GPIOAOUTEN) |(1<<23),GPIOAOUTEN);

    //act_writel(act_readl(GPIOADAT) |(1<<23),GPIOADAT);

    //sys_us_timer_start(0);

    //for multi fifo clear last frame data
    g_datain_flag = 1;

    if((g_start_play == 0) && (g_reset_flag == 0)) {
        reset_data_pos();
    }
    if(low_energy != 0) {
        tmp_play_status = 1;
    } else {
        tmp_play_status = 0;
    }
    while(temp_data < 300) {
        if(0x00 == (act_readb(Out1cs_hcin1cs) & 0x02)) {
            break;
        }

        usb_disconnect_isr();
        if(g_usb_state == 0x01) {
            break;
        }
        temp_data++;
    }

    //get data length
    if(temp_data == 300) {
        data_lenth = 0;
    } else {
        //data_lenth = (uint32) act_readb(Out1bcl_hcin1bcl);

        temp_low = act_readb(Out1bcl_hcin1bcl);

        temp_high = act_readb(Out1bch_hcin1bch);

        data_lenth = (uint32)((temp_high << 8) | temp_low);
    }

    if(data_lenth == 0) {

        goto exit_iso_out;
    }

    if((g_tts_start_flag == 0) && (g_mute_flag == 0)) {
        g_play_data_count += data_lenth;
    }

    if((g_wr_play_ptr + data_lenth) > PLAY_BUFF_SIZE) {
        buf_addr = (uint32) g_play_buf + g_wr_play_ptr;
        get_lenth = PLAY_BUFF_SIZE - g_wr_play_ptr;
        if(0 != get_lenth) {
            access_usbfifo_by_cpu(EP1OUT, (uint32 *) buf_addr, get_lenth);
        }
        buf_addr = (uint32) g_play_buf;//address
        get_lenth = data_lenth - get_lenth; //length
        access_usbfifo_by_cpu(EP1OUT, (uint32 *) buf_addr, get_lenth);
        g_wr_play_ptr = get_lenth;
    } else {
        buf_addr = (uint32) g_play_buf + g_wr_play_ptr;
        get_lenth = data_lenth;
        access_usbfifo_by_cpu(EP1OUT, (uint32 *) buf_addr, get_lenth);
        g_wr_play_ptr += data_lenth;
    }

    if(g_mute_delay != 0) {
        reset_data_pos();
    }

    if(g_play_data_count > PLAY_BUFF_SIZE) {
        //libc_print("full",g_play_data_count,0);

        g_play_data_count = g_wr_play_ptr;
    }

    if(g_start_play == 0) {
        g_start_play = 1;
        g_reset_flag = 0;
    }

exit_iso_out:
    //set busy

    act_writeb(act_readb(Out1cs_hcin1cs) | 0x02, Out1cs_hcin1cs);
    //send data to asrc in
    /*while (g_play_data_count > 0)
     {
     if ((act_readl(ASRC_IN_IP) & (1 << ASRC_IN_IP_WFIFOEmpty)) != 0)
     {
     //g_wr_record_ptr += 4;
     temp_data = *(uint32 *) (g_play_buf + g_rd_play_ptr);

     //set to high word
     *(volatile uint32 *) ASRC_IN_WFIFO = (temp_data & 0xffff) << 16;

     *(volatile uint32 *) ASRC_IN_WFIFO = (temp_data & 0xffff0000);

     g_rd_play_ptr += 4;

     g_play_data_count -= 4;

     if (g_rd_play_ptr == PLAY_BUFF_SIZE)
     {
     g_rd_play_ptr = 0;
     }
     }

     }*/
    if(g_manul_stop != 0) {
        g_start_play = 0;

        reset_data_pos();
    }

    //act_writel(act_readl(GPIOADAT) &(~(1<<23)),GPIOADAT);
    return;
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

    if((g_hid_idle_time == 0) || (g_hid_change != 0)) {
        act_writeb(g_hid_report, Fifo2dat);
        g_hid_report = 0;
        g_hid_change = 0;
        act_writeb(act_readb(In2cs_hcout2cs) | 0x02, In2cs_hcout2cs);

        while(0 != (act_readb(In2cs_hcout2cs) & 0x02)) {
            if(USB_CONNECT_STATUS != 0x00) {
                g_usb_state = 0x01;
                break;
            }
        }
    }

}

/******************************************************************************/
/*!
 * \par  Description:
 *   hid intrrupt check and deal.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_irq.c
 * \par
 * \note
 *******************************************************************************/
void hid_token_check(void)
{
    if(0 != ((act_readb(IN03TOKIRQ)) & 0x04)) { //hid
        act_writeb(0x04 | act_readb(IN03TOKIRQ), IN03TOKIRQ);
        hid_in_token_isr();
    }

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
void usound_isr(void)
{
    if((0 != ((act_readb(OUT03TOKIRQ)) & 0x02))) { //ep1 iso out
        act_writeb((uint8)((0x80 | act_readb(USBEIRQ)) & 0x8f), USBEIRQ);
        act_writeb(0x02, OUT03TOKIRQ);
        iso_out_token_isr();
    } else if(0 != ((act_readb(Usbirq_hcusbirq)) & 0x01)) { //setup
        act_writeb((uint8)((0x80 | act_readb(USBEIRQ)) & 0x8f), USBEIRQ);
        act_writeb(0x01 | act_readb(Usbirq_hcusbirq), Usbirq_hcusbirq);
        deal_setup();
    } else {
    }

    return;
}

//系统pagemiss回调该函数
void usound_isr_cbk(void)
{
    uint32 pending = act_readl(INTC_PD);//INTC_PD 由device的pending位控制
    pending &= act_readl(INTC_MSK);

    if((pending & (1 << IRQ_USB)) != 0) {
        if((0 != ((act_readb(OUT03TOKIRQ)) & 0x02))) { //ep1 iso out

            act_writeb((uint8)((0x80 | act_readb(USBEIRQ)) & 0x8f), USBEIRQ);
            act_writeb(0x02, OUT03TOKIRQ);
            iso_out_token_isr();
        }
    }

}

