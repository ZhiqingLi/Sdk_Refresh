#include "uart_recordkey.h"

#ifdef SUPPORT_UART_SIM_KEY

void uart1_rx_init(uint32 bitrate)
{
	/* 选择 UART1 时钟源: HOSC */
    act_writel((act_readl(CMU_UART1CLK) & ~(1 << CMU_UART1CLK_SEL)),CMU_UART1CLK);

    /* 使能 UART1 时钟 */
    act_writel((act_readl(CMU_DEVCLKEN) | (1 << CMU_DEVCLKEN_UART1CLKEN)),CMU_DEVCLKEN);
	/* 复位 UART1 */
    act_writel(act_readl(MRCR)& ~(1 << MRCR_UART1Reset),MRCR);
    act_writel(act_readl(MRCR)| (1 << MRCR_UART1Reset),MRCR);

   
    /* 设置 UART_TX1: GPIOA23, UART_RX1: GPIOA21 */
    #if (CASE_BOARD_TYPE == CASE_BOARD_EVB)
    act_writel(act_readl(GPIOAOUTEN) & ~(1 << 21) & ~(1 << 23), GPIOAOUTEN);
    act_writel(act_readl(GPIOAINEN) &~(1 << 21) & ~(1 << 23), GPIOAINEN);
    act_writel((act_readl(MFP_CTL0) & ~(MFP_CTL0_GPIOA23_MASK | MFP_CTL0_GPIOA21_MASK)) \
                | (6 << MFP_CTL0_GPIOA23_SHIFT) | (4 << MFP_CTL0_GPIOA21_SHIFT),MFP_CTL0);
    #else if (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825)
    act_writel(act_readl(GPIOAOUTEN) & ~(1 << 2) & ~(1 << 23), GPIOAOUTEN);
    act_writel(act_readl(GPIOAINEN) & ~(1 << 2) & ~(1 << 23), GPIOAINEN);
    act_writel((act_readl(MFP_CTL0) & ~(7 << MFP_CTL0_GPIOA23_SHIFT)) | (6 << MFP_CTL0_GPIOA23_SHIFT),MFP_CTL0);
    act_writel((act_readl(MFP_CTL1) & ~(7 << MFP_CTL1_GPIOA2_SHIFT)) | (5 << MFP_CTL1_GPIOA2_SHIFT),MFP_CTL1);
    #endif
	/* 配置 UART1 波特率分频 */
    act_writel( 
        ((HOSC_CLOCK / bitrate) << UART1_BR_TXBRDIV1_SHIFT) |
        ((HOSC_CLOCK / bitrate) << UART1_BR_RXBRDIV1_SHIFT),UART1_BR);

    /* 配置 UART1 控制器 */
    act_writel( 
        (1 << UART1_CTL_EN1)            // 使能 UART1
        |(0 << UART1_CTL_PRS1_SHIFT)    // 奇偶位: 无
        |(0 << UART1_CTL_STPS1)         // 停止位: 1 位
        |(3 << UART1_CTL_DWLS1_SHIFT)   // 数据位: 8 位
        |(1 << UART1_CTL_RXIE1),UART1_CTL); //设置IRQ中断 
       
    /* 复位 FIFO */
    act_writel( act_readl(UART1_STA) | 0x1f,UART1_STA);
  
}

#endif
