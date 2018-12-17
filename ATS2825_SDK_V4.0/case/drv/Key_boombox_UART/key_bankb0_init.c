/*******************************************************************************
 *                              US280A
 *                            Module: Key driver
 *                 Copyright(c) 2003-2013 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       fiona     2014-1-28 10:46    1.0             build this file
 *******************************************************************************/

#include "key_inner.h"

//for error deal
void uart1_init(void)
{
    //init
    //rx1 a17  tx1 a16
    //act_writel((act_readl(MFP_CTL0) & 0xffffafff), MFP_CTL0);

    //act_writel((act_readl(MFP_CTL0) | 0x00002800), MFP_CTL0);

    //rx1 a21  tx1 a23
    act_writel((act_readl(MFP_CTL0) & 0xcf7fffff), MFP_CTL0);

    act_writel((act_readl(MFP_CTL0) | 0x30800000), MFP_CTL0);

    //cts1 a11
    //act_writel((act_readl(MFP_CTL0) & 0xfffffffc), MFP_CTL0);

    //act_writel((act_readl(MFP_CTL0) | 0x00000004), MFP_CTL0);

    //rts1 a10
    //act_writel((act_readl(MFP_CTL1) & 0xe7fffff), MFP_CTL1);

    //act_writel((act_readl(MFP_CTL1) | 0x20000000), MFP_CTL1);

    //select UART1 device clock hosc
    act_writel(0x00000000, CMU_UART1CLK);

    //Enable UART1 device clock
    act_writel(act_readl(CMU_DEVCLKEN) | 0x00000200, CMU_DEVCLKEN);

    //reset
    act_writel(act_readl(MRCR) & 0xfffdffff, MRCR);
    //module reset disable (UART1)
    act_writel(act_readl(MRCR) | 0x00020000, MRCR);

    //UART_RX1 10k Pull-up Enable
    act_writel(act_readl(PADPUPD) | 0x00000001, PADPUPD);

    //Set UART1 baudrate
    act_writel(0x15b, UART1_BR);//115200,40m/347

    act_writel(0x00048a03, UART1_CTL);//tx dma

    //reset fifo
    act_writel(act_readl(UART1_STA) | 0x0000001f, UART1_STA);
    //set idle
    g_bus_status = UART_IDLE;

}

/******************************************************************************/
/*!
 * \par  Description:
 *    key initialize
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
 * 按键初始化，负责中断向量安装等
 *******************************************************************************/
int key_init(void)
{

    uart1_init();
    //for uart module mode
    sys_request_irq(IRQ_UART1, uart1_int_process);

    //申请定时器
    g_time_irq = sys_set_irq_timer1(status_scan, 20);

    //send start packet
    /*uart1_transmit_deal(SEND_CMD_FLAG, 0);

     libc_memset(g_transmit_buf, 0x55, 16);
     //send cmd
     uart1_transmit_deal(SEND_CMD_DATA, 16);
     //send end flag
     uart1_transmit_deal(SEND_CMD_FLAG, 0);*/

    return 0;
}

module_init(key_init)
