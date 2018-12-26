#include "key_inner.h"

#if defined(SUPPORT_SPI_FUNCTION)
//select work mode 0,1:write/read 2:write only 3:read only
#define SPI_MODE_READ_WRITE             0
#define SPI_MODE_READ_ONLY              3
#define SPI_MODE_WRITE_ONLY             2

#define SPI_SET_RW_MODE(mode)           (act_writel((act_readl(SPI1_CTL) & (~(SPI1_CTL_SPI1_WR_MASK))) \
        | ((mode) << SPI1_CTL_SPI1_WR_SHIFT), SPI1_CTL))
#define SPI_RESET_FIFO()                (act_writel((1 << SPI1_STA_TXER | 1 << SPI1_STA_RXER), SPI1_STA))

void __section__(".bank0") spi_init(void)
{
    act_writel(act_readl(CMU_DEVCLKEN) | 0x80, CMU_DEVCLKEN);
    act_writel(act_readl(CMU_SPICLK) | (1 << CMU_SPICLK_SEL1_SHIFT), CMU_SPICLK);
    act_writel(act_readl(CMU_DEVCLKEN) | (1 << CMU_DEVCLKEN_SPI1CLKEN), CMU_DEVCLKEN);

    //Tink:设置四线管脚的模式，禁掉输入输出启用复用模式
    act_writel(act_readl(GPIOAOUTEN) & ~(0x0F << 8), GPIOAOUTEN);
    act_writel(act_readl(GPIOAINEN) & ~(0x0F << 8), GPIOAINEN);
    act_writel(act_readl(MFP_CTL0) & ~(MFP_CTL0_GPIOA11_MASK), MFP_CTL0);
    act_writel(act_readl(MFP_CTL0) | (5 << MFP_CTL0_GPIOA11_SHIFT), MFP_CTL0);    // spi1 mosi

    act_writel(act_readl(MFP_CTL1) & ~(MFP_CTL1_GPIOA10_MASK | MFP_CTL1_GPIOA9_MASK | MFP_CTL1_GPIOA8_MASK), MFP_CTL1);
    act_writel(act_readl(MFP_CTL1) | (5 << MFP_CTL1_GPIOA10_SHIFT) | (4 << MFP_CTL1_GPIOA9_SHIFT) |
               (4 << MFP_CTL1_GPIOA8_SHIFT), MFP_CTL1);    // spi1 miso

    //spi1 reset
    act_writel(act_readl(MRCR) & (~(1 << MRCR_SPI1Reset)), MRCR);
    sys_usleep(100000);
    act_writel(act_readl(MRCR) | (1 << MRCR_SPI1Reset), MRCR);

    act_writel((1 << SPI1_CTL_SPI1_EN)
               | (10 << SPI1_CTL_RLRS1_SHIFT)
               | (1 << SPI1_CTL_SPI1_SS)
               | (0 << SPI1_CTL_SPI1_MS)
               | (2 << SPI1_CTL_SPI1_WR_SHIFT), SPI1_CTL);

    //select SPI FIFO AHB bus access, clear IRQ pending
    act_writel((1 << SPI1_DRQ_SPI1_TIRQ_PD)
               | (1 << SPI1_DRQ_SPI1_RIRQ_PD), SPI1_DRQ);
    //sclk divider is 1, 8bit data width, TX & TX endian convert mode
    act_writel(0x0F, SPI1_CLKDIV);

    //disable spi delay chain, and set delay chain to 8ns, 1x IO mode
    act_writel(0x70, SPI1_BCH);

    libc_print("spi init finish", 0, 0);
}

int key_inner_spi_write(uint8 *buf, uint16 len)
{
    int nWait = 10000;

    //set cs low
    SPI_SET_RW_MODE(SPI_MODE_WRITE_ONLY);
    SPI_RESET_FIFO();

    act_writel((act_readl(SPI1_CTL) & ~(1 << SPI1_CTL_SPI1_SS)), SPI1_CTL);

    while(len > 0) {
        if(act_readl(SPI1_STA) & (1 << SPI1_STA_TXFU))
            continue;

        act_writel(*buf, SPI1_TXDAT);
        buf++;
        len--;
    }

    while((act_readl(SPI1_STA) & (1 << SPI1_STA_TXEM)) == 0 && (--nWait) > 0) {
        //loop until tx FIFO empty
    }

    act_writel((act_readl(SPI1_CTL) | (1 << SPI1_CTL_SPI1_SS)), SPI1_CTL);

    return nWait;
}

int key_inner_spi_read(uint8 *buf, uint16 len)
{
    SPI_SET_RW_MODE(SPI_MODE_READ_ONLY);
    SPI_RESET_FIFO();

    act_writel((act_readl(SPI1_CTL) & ~(1 << SPI1_CTL_SPI1_SS)), SPI1_CTL);
    act_writel(len, SPI1_BCL);
    //start clock
    act_writel((act_readl(SPI1_BCH) | (1 << SPI1_BCH_SPI1_RS)), SPI1_BCH);
    while(len > 0) {
        if(act_readl(SPI1_STA) & (1 << SPI1_STA_RXEM))
            continue;

        *buf = (uint8)act_readl(SPI1_RXDAT);
        buf++;
        len--;
    }

    //set cs high
    act_writel((act_readl(SPI1_CTL) | (1 << SPI1_CTL_SPI1_SS)), SPI1_CTL);

    return 1;
}

int32 key_inner_spi_read_write(uint8 *wbuf, uint8 *rbuf, uint32 size)
{
    uint16 wlen = size & 0xFFFF;
    uint16 rlen = (size >> 16) & 0xFFFF;
    int nWait = 10000;

    SPI_SET_RW_MODE(SPI_MODE_WRITE_ONLY);
    SPI_RESET_FIFO();

    act_writel((act_readl(SPI1_CTL) & ~(1 << SPI1_CTL_SPI1_SS)), SPI1_CTL);

    while(wlen > 0) {
        if(act_readl(SPI1_STA) & (1 << SPI1_STA_TXFU))
            continue;

        act_writel(*wbuf, SPI1_TXDAT);
        wbuf++;
        wlen--;
    }

    do {
        while((act_readl(SPI1_STA) & (1 << SPI1_STA_TXEM)) == 0 && (--nWait) > 0) {
            //loop until tx FIFO empty
        }

        if(nWait <= 0)
            break;

        SPI_SET_RW_MODE(SPI_MODE_READ_ONLY);
        SPI_RESET_FIFO();
        act_writel(rlen, SPI1_BCL);
        //start clock
        act_writel((act_readl(SPI1_BCH) | (1 << SPI1_BCH_SPI1_RS)), SPI1_BCH);
        while(rlen > 0) {
            if(act_readl(SPI1_STA) & (1 << SPI1_STA_RXEM))
                continue;

            *rbuf = (uint8)act_readl(SPI1_RXDAT);
            rbuf++;
            rlen--;
        }

        act_writel((act_readl(SPI1_CTL) | (1 << SPI1_CTL_SPI1_SS)), SPI1_CTL);
    } while(0);

    return nWait;
}
#else
void __section__(".bank0") spi_init(void)
{
}

int key_inner_spi_write(uint8 *buf, uint16 len)
{
    return 0;
}

int key_inner_spi_read(uint8 *buf, uint16 len)
{
    return 0;
}

int32 key_inner_spi_read_write(uint8 *wbuf, uint8 *rbuf, uint32 size)
{
    return 0;
}
#endif
