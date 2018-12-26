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
 *DMA control
 *
 *
 *******************************************************************************/

void cap_dac_dma_stop(void)
{
    //stop
    act_writel(act_readl(DMA0CTL) & (~(1 << DMA0CTL_DMA0START)), DMA0CTL); // stop dma

    //disable interrupt
    act_writel((act_readl(DMAIE) & (~(1 << DMAIE_DMA0TCIE)) & (~(1 << DMAIE_DMA0HFIE))), DMAIE);
    //release irq
    //sys_free_irq(IRQ_DMA0);

}

void cap_dac_dma_isr(void)
{
    //clr dma interrupt pending
    act_writel((act_readl(DMAIP) | (1 << DMAIP_DMA0TCIP) | (1 << DMAIP_DMA0HFIP)), DMAIP);

    g_play_data_count -= CONVERT_BUFF_SIZE / 2;

    if(g_play_data_count <= 0x00) { //BUFFER 已空，需要停止搬数
        dac_stop();
        g_start_play = 0;
        g_play_data_count = 0;
        return;
    }

    if(g_play_data_count > (CONVERT_BUFF_SIZE * 11 / 32)) { //搬数过慢，BUFFER快要满了
        *((REG32)(AUDIO_PLL_CTL)) = (*((REG32)(AUDIO_PLL_CTL)) & 0xfffffffc) | (2 << AUDIO_PLL_CTL_APS_SHIFT);
    }
    if(g_play_data_count < (CONVERT_BUFF_SIZE * 5 / 32)) { //搬数过快，BUFFER快要空了
        *((REG32)(AUDIO_PLL_CTL)) = (*((REG32)(AUDIO_PLL_CTL)) & 0xfffffffc) | (1 << AUDIO_PLL_CTL_APS_SHIFT);
    }

}

void cap_dac_dma_cfg(void)
{
    act_writel((act_readl(MRCR) | 0x01), MRCR);
    act_writel((act_readl(CMU_DEVCLKEN) | 0x01), CMU_DEVCLKEN);
    act_writel(act_readl(DMA0CTL) & (~(1 << DMA0CTL_DMA0START)), DMA0CTL); // stop dma

    //clr dma interrupt pending
    act_writel((act_readl(DMAIP) | (1 << DMAIP_DMA0TCIP) | (1 << DMAIP_DMA0HFIP)), DMAIP);

    //register dma
    //sys_request_irq(IRQ_DMA0, cap_dac_dma_isr);

    //enable interrupt
    act_writel((act_readl(DMAIE) | (1 << DMAIE_DMA0TCIE) | (1 << DMAIE_DMA0HFIE)), DMAIE);
    //source addr
    act_writel((uint32)g_convert_buf, DMA0SADDR0);
    //act_writel((uint32)g_play_buf, DMA0SADDR1);
    //data len
    act_writel((CONVERT_BUFF_SIZE / 4), DMA0FrameLen);

    act_writel(0x08c02, DMA0CTL);

    //start
    act_writel((act_readl(DMA0CTL) | (1 << DMA0CTL_DMA0START)), DMA0CTL);
}

void dac_start(void)
{
    cap_dac_dma_cfg();

    // normal DAC&I2STX*ADC&I2SRX set as normal
    *((REG32)(MRCR)) |= (1 << MRCR_AudioIOReset);

    //*((REG32) (DAC_ANACTL)) |= ((1 << DAC_ANACTL_DAENR) | (1 << DAC_ANACTL_DAENL));

    *((REG32)(DAC_DIGCTL)) |= (1 << DAC_DIGCTL_DDEN);

    *((REG32)(DAC_ANACTL)) &= (~(1 << DAC_ANACTL_ZERODT));


    //out chanel 1
    //dac fifo1 set

    *((REG32)(DAC_FIFOCTL)) &= (~DAC_FIFOCTL_DAF1IS_MASK);

    *((REG32)(DAC_FIFOCTL)) |= (1 << DAC_FIFOCTL_DAF1IS_SHIFT);

    *((REG32)(DAC_FIFOCTL)) |= (1 << DAC_FIFOCTL_DAF1EDE);

    *((REG32)(DAC_FIFOCTL)) &= (~(1 << DAC_FIFOCTL_DAF1RT));

    *((REG32)(DAC_FIFOCTL)) |= (1 << DAC_FIFOCTL_DAF1RT);

    *((REG32)(DAC_DIGCTL)) |= (1 << DAC_DIGCTL_DAF1M2DAEN);

    *((REG32)(AUDIO_PLL_CTL)) = (*((REG32)(AUDIO_PLL_CTL)) & 0xfffffffc) | (1 << AUDIO_PLL_CTL_APS_SHIFT);

//    libc_print("dac start",0,0);

}

void dac_stop(void)
{
    uint32 dac_run;

    reset_data_pos();

//    libc_print("dac stop",0,0);

    //dac fifo reset
    *((REG32)(DAC_FIFOCTL)) &= (~(1 << DAC_FIFOCTL_DAF1RT));
    //fifo1 mix disable
    *((REG32)(DAC_DIGCTL)) &= (~(1 << DAC_DIGCTL_DAF1M2DAEN));

    dac_run = *((REG32)(DAC_FIFOCTL));

    if((dac_run & ((1 << DAC_FIFOCTL_DAF0RT) | (1 << DAC_FIFOCTL_DAF1RT))) == 0) {

        //*((REG32) (DAC_ANACTL)) &= (~((1 << DAC_ANACTL_DAENR) | (1 << DAC_ANACTL_DAENL)));

        *((REG32)(DAC_ANACTL)) |= (1 << DAC_ANACTL_ZERODT);

        *((REG32)(DAC_DIGCTL)) &= (~(1 << DAC_DIGCTL_DDEN));
    }
    cap_dac_dma_stop();
}

void set_dac_sample_rate(uint32 sample_rate)
{
//    libc_print("sample",sample_rate,2);

    *((REG32)(AUDIO_PLL_CTL)) = (1 << AUDIO_PLL_CTL_AUDPLLEN);

    switch(sample_rate) {
        case 44:
            *((REG32)(AUDIO_PLL_CTL)) = (*((REG32)(AUDIO_PLL_CTL)) & 0xfffffff0) | (0 << AUDIO_PLL_CTL_APS_SHIFT);

            *((REG32)(CMU_ADDACLK)) = ((*((REG32)(CMU_ADDACLK)) & 0xfffffff8) | 0x01);

            *((REG32)(AUDIO_PLL_CTL)) = (*((REG32)(AUDIO_PLL_CTL)) & 0xfffffffc) | (1 << AUDIO_PLL_CTL_APS_SHIFT);
            break;

        case 48:
            *((REG32)(AUDIO_PLL_CTL)) = (*((REG32)(AUDIO_PLL_CTL)) & 0xfffffff0) | (4 << AUDIO_PLL_CTL_APS_SHIFT);

            *((REG32)(CMU_ADDACLK)) = ((*((REG32)(CMU_ADDACLK)) & 0xfffffff8) | 0x01);

            *((REG32)(AUDIO_PLL_CTL)) = (*((REG32)(AUDIO_PLL_CTL)) & 0xfffffffc) | (1 << AUDIO_PLL_CTL_APS_SHIFT);
            break;

        case 96:
            *((REG32)(AUDIO_PLL_CTL)) &= (~AUDIO_PLL_CTL_APS_MASK);

            *((REG32)(AUDIO_PLL_CTL)) |= (6 << AUDIO_PLL_CTL_APS_SHIFT);

            *((REG32)(CMU_ADDACLK)) &= 0xfffffff8;

            *((REG32)(AUDIO_PLL_CTL)) = (*((REG32)(AUDIO_PLL_CTL)) & 0xfffffffc) | (1 << AUDIO_PLL_CTL_APS_SHIFT);
            break;

        default:
            libc_print("no such rate", sample_rate, 2);
            break;
    }
    //libc_print("AUDIO PLL:",act_readl(AUDIO_PLL_CTL),2);
    //libc_print("CMU_ADDACLK:",act_readl(CMU_ADDACLK),2);
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
    uint32 data_lenth, true_lenth, get_lenth = 0;
    uint32 buf_addr, src_addr, dst_addr, temp_data = 0;

    uint32 temp_low, temp_high;
    uint16 wait_data = 0;

    g_datain_flag = 1;

    //act_writel(act_readl(GPIOAOUTEN) |(1<<23),GPIOAOUTEN);

    //act_writel(act_readl(GPIOADAT) |(1<<23),GPIOADAT);

    //sys_us_timer_start(0);

    //for multi fifo clear last frame data
    /*
    if (g_start_play == 0)
    {
        reset_data_pos();

        //dac_start();
    }
    */
    if(g_play_sample == 96000) {
        wait_data = 600;
    } else {
        wait_data = 300;
    }

    while(temp_data < wait_data) {
        if(0x00 == (act_readb(Out1cs_hcin1cs) & 0x02)) {
            break;
        }

        usb_disconnect_isr();
        if(g_usb_state == 0x01) {
            break;
        }
        temp_data++;
    }

    if(temp_data == 600) {
        data_lenth = 0;
    } else {

        temp_low = act_readb(Out1bcl_hcin1bcl);

        temp_high = act_readb(Out1bch_hcin1bch);

        data_lenth = (uint32)((temp_high << 8) | temp_low);
    }

    if(data_lenth == 0) {

        goto exit_iso_out;
    }

    //read data from fifo
    buf_addr = (uint32) g_play_buf;
    //从usb fifo读取数据到RAM
    access_usbfifo_by_cpu(EP1OUT, (uint32 *) buf_addr, data_lenth);
    if(g_ud_stop_by_tts == 1) {
        return 0;//播TTS时不做4字节对齐
    }

    //data convert
    true_lenth = data_lenth * 4 / 3; //DMA搬运数据量为PC下传数据的4/3(24-32)
    src_addr = (uint32) g_play_buf;
    if((g_wr_play_ptr + true_lenth) > CONVERT_BUFF_SIZE) { //BUFFER满需要重头装
        dst_addr = (uint32) g_convert_buf + g_wr_play_ptr;
        get_lenth = (CONVERT_BUFF_SIZE - g_wr_play_ptr) * 3 / 4;
        if(0 != get_lenth) {
            data_convert2((uint8 *) src_addr, (uint8 *) dst_addr, get_lenth);
        }

        dst_addr = (uint32) g_convert_buf;//address
        src_addr = src_addr + get_lenth;
        get_lenth = data_lenth - get_lenth; //length
        data_convert2((uint8 *) src_addr, (uint8 *) dst_addr, get_lenth);

        g_wr_play_ptr = (get_lenth * 4 / 3);
    } else {
        dst_addr = (uint32) g_convert_buf + g_wr_play_ptr;
        get_lenth = data_lenth;
        data_convert2((uint8 *) src_addr, (uint8 *) dst_addr, get_lenth);
        g_wr_play_ptr += true_lenth;
    }

    g_play_data_count += (int32)true_lenth;

    if(g_play_data_count > CONVERT_BUFF_SIZE) { //从头计数
        g_play_data_count = (int32)g_wr_play_ptr;
    }

    if((g_mute_delay != 0) || (g_mute_flag != 0)) {
        reset_data_pos();
    }

    if((g_play_data_count >= (CONVERT_BUFF_SIZE / 4)) && (g_start_play == 0) && (g_ud_stop_by_tts == 0)) {
        g_start_play = 1;
        dac_start();
    }

    if(g_low_energy_count == 0) {
        tmp_play_status = 0;
    }
    if(g_low_energy_count >= 10) {
        tmp_play_status = 1;
        g_low_energy_count = 0;
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

        dac_stop();
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
//        libc_print("ep1_out",0,0);
        act_writeb((uint8)((0x80 | act_readb(USBEIRQ)) & 0x8f), USBEIRQ);
        act_writeb(0x02, OUT03TOKIRQ);
        iso_out_token_isr();
    } else if(0 != ((act_readb(Usbirq_hcusbirq)) & 0x01)) { //setup
        act_writeb((uint8)((0x80 | act_readb(USBEIRQ)) & 0x8f), USBEIRQ);
        act_writeb(0x01 | act_readb(Usbirq_hcusbirq), Usbirq_hcusbirq);
        deal_setup();
    } else {
        ;//for QAC
    }

    return;
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

