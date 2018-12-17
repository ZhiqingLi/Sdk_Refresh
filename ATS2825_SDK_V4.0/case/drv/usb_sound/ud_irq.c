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

#ifdef _USB_RECORD_

/******************************************************************************/
/*!
 * \par  Description:void cap_adc_dma_stop(void)
 * \config record use dma and stop
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_msc.c
 * \par
 * \note
 *******************************************************************************/

void cap_adc_dma_stop(void)
{
    //stop
    act_writel(act_readl(DMA0CTL) & (~(1 << DMA0CTL_DMA0START)), DMA0CTL); // stop dma

    //disable interrupt
    act_writel((act_readl(DMAIE) & (~(1 << DMAIE_DMA0TCIE)) & (~(1 << DMAIE_DMA0HFIE))), DMAIE);
    //release irq
    //sys_free_irq(IRQ_DMA0);

}

/******************************************************************************/
/*!
 * \par  Description:
 *     interrupt service
 * \param[in]    none.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void cap_adc_dma_isr(void)
{
    //clr dma interrupt pending
    act_writel((act_readl(DMAIP) | (1 << DMAIP_DMA0TCIP) | (1 << DMAIP_DMA0HFIP)), DMAIP);

    //no data has been sent to pc anymore
    if(g_record_data_count >= RECORD_BUFF_SIZE) {
        cap_adc_dma_stop();

        g_start_record = 0;

        return;
    }

    g_record_data_count += RECORD_BUFF_SIZE / 2;
    if(g_record_data_count > RECORD_HIGH_THRESHOLD) {
        //act_writel(act_readl(AUDIO_PLL_CTL) & (~(3 << 0)), AUDIO_PLL_CTL);
        //*((REG32) (AUDIOPLL_CTL)) &= ~(3 << 9);//adjust more slower
        // config decimation ratio
        //libc_print("slow",0,0);
        if(g_asrc_adj_flag == 1) {
            *((REG32)(ASRC_IN_DEC0)) = (uint32)(((double)(K_ASRC_IN_DEC0 + 5000)) / 44100) * 48000;
        } else {
            *((REG32)(ASRC_IN_DEC0)) = (K_ASRC_IN_DEC0 + 5000);
        }

    } else if(g_record_data_count < RECORD_LOW_THRESHOLD) {
        //*((REG32) (AUDIOPLL_CTL)) |= (7 << 9);//adjust more faster
        //act_writel(act_readl(AUDIO_PLL_CTL) | (3 << 0), AUDIO_PLL_CTL);
        // config decimation ratio
        //libc_print("fast",0,0);
        if(g_asrc_adj_flag == 1) {
            *((REG32)(ASRC_IN_DEC0)) = (uint32)(((double)(K_ASRC_IN_DEC0 - 5000)) / 44100) * 48000;
        } else {
            *((REG32)(ASRC_IN_DEC0)) = (K_ASRC_IN_DEC0 - 5000);
        }

    }

    else {
        ;//for QAC
    }
}

/******************************************************************************/
/*!
 * \par  Description:void cap_adc_dma_cfg(void)
 * \config record use dma and start
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_msc.c
 * \par
 * \note
 *******************************************************************************/
void cap_adc_dma_cfg(void)
{
    act_writel(act_readl(DMA0CTL) & (~(1 << DMA0CTL_DMA0START)), DMA0CTL); // stop dma

    //clr dma interrupt pending
    act_writel((act_readl(DMAIP) | (1 << DMAIP_DMA0TCIP) | (1 << DMAIP_DMA0HFIP)), DMAIP);

    //register dma
    //sys_request_irq(IRQ_DMA0, cap_adc_dma_isr);

    //enable interrupt
    act_writel((act_readl(DMAIE) | (1 << DMAIE_DMA0TCIE) | (1 << DMAIE_DMA0HFIE)), DMAIE);
    //dest addr
    act_writel((uint32)g_record_buf, DMA0DADDR0);
    //dest addr another
    act_writel((uint32)g_rec_no_up, DMA0DADDR1);
    //data len
    act_writel(RECORD_BUFF_SIZE, DMA0FrameLen);

    //adc to memory and 16bit & seperate mode for audio data
    act_writel(0x140d2, DMA0CTL);

    //start
    act_writel((act_readl(DMA0CTL) | (1 << DMA0CTL_DMA0START)), DMA0CTL);
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
void iso_in_token_isr(void)
{
    uint32 data_lenth, send_lenth, send_addr; //,temp_data;

    //start record flag set
    if(g_start_record == 0) {
        cap_adc_dma_cfg();

        g_start_record = 1;

        g_record_data_count = 0;
    }

    //report data length
    if(g_record_data_count < g_record_packet_size) {
        data_lenth = (uint32)g_record_data_count;
    } else {
        data_lenth = g_record_packet_size;
    }

    //libc_print("cddf",data_lenth,2);

    if(g_record_data_count > 0) {
        //sub buffer length
        g_record_data_count -= data_lenth;
    }

    //send data to usb fifo
    if((g_rd_record_ptr + data_lenth) > RECORD_BUFF_SIZE) {
        //g_rd_record_ptr record cur read position
        send_addr = (uint32)g_record_buf + g_rd_record_ptr;
        //send length caculate
        send_lenth = (RECORD_BUFF_SIZE - g_rd_record_ptr);
        //left ptr do not meet one frame
        if(0 != send_lenth) {
            //send to pc
            access_usbfifo_by_cpu(EP1IN, (uint32 *)send_addr, send_lenth);
        }
        //reset to buffer header
        send_addr = (uint32)g_record_buf;
        //frame length
        send_lenth = data_lenth - send_lenth;
        //send to fifo again
        access_usbfifo_by_cpu(EP1IN, (uint32 *)send_addr, send_lenth);
        //read ptr record position
        g_rd_record_ptr = send_lenth;
    } else {
        //write ptr
        send_addr = (uint32)g_record_buf + g_rd_record_ptr;

        //length
        send_lenth = data_lenth;

        access_usbfifo_by_cpu(EP1IN, (uint32 *)send_addr, send_lenth);
        //update position
        g_rd_record_ptr += data_lenth;
    }
    //set busy status
    act_writeb(act_readb(In1cs_hcout1cs) | 0x02, In1cs_hcout1cs);

    while(0 != (act_readb(In1cs_hcout1cs) & 0x02)) {
        if(USB_CONNECT_STATUS != 0x00) {
            g_usb_state = 0x01;
            break;
        }
    }

}
#endif

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
    //sys_us_timer_start();

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

    //data_lenth = (uint32) act_readb(Out1bcl_hcin1bcl);
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
        buf_addr = (uint32) g_play_buf;
        get_lenth = data_lenth - get_lenth;
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

    //temp_data = sys_us_timer_break();

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
    }
#ifdef _USB_RECORD_
    else if((0 != ((act_readb(IN03TOKIRQ)) & 0x02)))  //ep1 iso in

    {
        act_writeb((uint8)((0x80 | act_readb(USBEIRQ)) & 0x8f), USBEIRQ);
        act_writeb(0x02, IN03TOKIRQ);
        iso_in_token_isr();
    }
#endif
    else if(0 != ((act_readb(Usbirq_hcusbirq)) & 0x01)) { //setup
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

#ifdef _USB_RECORD_
        else if((0 != ((act_readb(IN03TOKIRQ)) & 0x02))) { //ep1 iso in
            act_writeb((uint8)((0x80 | act_readb(USBEIRQ)) & 0x8f), USBEIRQ);
            act_writeb(0x02, IN03TOKIRQ);
            iso_in_token_isr();
        }
#endif
        else {
            ;//nothing
        }
    }

}
