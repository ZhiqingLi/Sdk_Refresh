#include "include.h"
#include "bsp_spiflash1.h"

#if REC_TO_SPIFALSH

#if 0
#define TRACE(...)              TRACE(__VA_ARGS__)
#else
#define TRACE(...)
#endif


void spi1_cs_en(void);
void spi1_cs_dis(void);
void spi1_cs_init(void);

#define SPI1_CS_INIT()           spi1_cs_init()
#define SPI1_CS_EN()             spi1_cs_en()
#define SPI1_CS_DIS()            spi1_cs_dis()

#define SF_PROGRAM              0x02                //SPIFlash编程
#define SF_READ                 0x03                //SPIFlash读取
#define SF_READSSR              0x05                //SPIFlash读状态寄存器
#define SF_WRITE_EN             0x06                //SPIFlash写使能
#define SF_ERASE                0x20                //SPIFlash擦除  //sector erase
#define SF_ERASE_BLOCK          0xD8                //SPIFlash擦除  //block erase   //64K

#define SPIFLASH1_BAUD          (200000)            //200K

void printf_spi(u32 addr, u32 len);

AT(.com_text.spiflash)
void spi1_cs_init(void)
{
    GPIOAFEN &= ~BIT(5);
    GPIOADE |= BIT(5);
    GPIOADIR &= ~BIT(5);
    GPIOASET = BIT(5);
}

AT(.com_text.spiflash)
void spi1_cs_en(void)
{
    GPIOACLR = BIT(5);
}

AT(.com_text.spiflash)
void spi1_cs_dis(void)
{
    GPIOASET = BIT(5);
}

///SPIFlash Delay
AT(.com_text.spiflash)
static void spi1_delay(void)
{
    uint cnt = 20;
    while (cnt--) {
        asm("nop");
    }
}

///SPI接口获取1Byte数据
AT(.com_text.spiflash)
static u8 spi1_getbyte(void)
{
    SPI1CON |= BIT(4);                              //RX
    SPI1BUF = 0xff;
    while (!(SPI1CON & BIT(16)));                   //Wait pending
    return SPI1BUF;
}

///SPI接口发送1Byte数据
AT(.com_text.spiflash)
static void spi1_sendbyte(u8 data)
{
    SPI1CON &= ~BIT(4);                             //TX
    SPI1BUF = data;
    while (!(SPI1CON & BIT(16)));                   //Wait pending
}

AT(.com_text.spiflash)
u32 spiflash1_id_read(void)
{
    u32 id = 0,i;
    SPI1_CS_EN();
    spi1_delay();
    spi1_sendbyte(0x9F);
    for (i = 0; i < 4; i++) {
        id |= spi1_getbyte();
        id <<= 8;
    }
    SPI1_CS_DIS();
    return id;
}

AT(.com_text.spiflash)
void spi1_init(u32 baud)
{
    //SPI1_G1: DI_PA2 , CLK_PA3, DO_PA4,   //CS_PA5
//    GPIOAFEN |= BIT(2)|BIT(3)|BIT(4);
//    GPIOADE |= BIT(2)|BIT(3)|BIT(4);
//    GPIOADIR &= ~(BIT(3) | BIT(4));  //CLK,DO output
//    GPIOADIR |= BIT(2);  //DI input
//    GPIOAPU |= BIT(2);   //DI pull up 10K
//    FUNCMCON1 = (0x0F<<12);
//    FUNCMCON1 = (0x01<<12);  //G1

    //SPI1_G2:  DI_PA7,  CLK_PA6, //CS_PA5
    GPIOAFEN |= BIT(6)|BIT(7);
    GPIOADE |= BIT(6)|BIT(7);
    GPIOADIR &= ~ BIT(6) ;  //CLK output
    GPIOADIR |= BIT(7);  //DI input
    GPIOAPU |= BIT(7);   //DI pull up 10K
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = (0x02<<12); //G2

    SPI1_CS_INIT();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SPI1BAUD = 24000000/baud - 1;   //sys 24M.
    SPI1CON =  0x01 | (1<<2);
}

///SPIFlash写使能
AT(.com_text.spiflash)
void spiflash1_init(void)
{
    spi1_init(SPIFLASH1_BAUD);
    //while(1) {WDT_CLR();printf("Flash ID = 0x%X\n", spiflash1_id_read());delay_ms(300);}

}

///SPIFlash写使能
AT(.com_text.spiflash)
static void spiflash1_write_enable(void)
{
    SPI1_CS_EN();
    spi1_sendbyte(SF_WRITE_EN);
    SPI1_CS_DIS();
    spi1_delay();
}

///SPIFlash读状态寄存器
AT(.com_text.spiflash)
static uint spiflash1_readssr(void)
{
    uint ssr;
    SPI1_CS_EN();
    spi1_sendbyte(SF_READSSR);
    ssr = spi1_getbyte();
    SPI1_CS_DIS();
    spi1_delay();
    return ssr;
}

///SPIFlash等待BUSY
AT(.com_text.spiflash)
static void spiflash1_waitbusy(void)
{
    do {
        spi1_delay();
    } while (spiflash1_readssr() & 0x01);
}

///发送SPIFlash的地址
AT(.com_text.spiflash)
static void spiflash1_sendaddr(u32 addr)
{
    spi1_sendbyte(addr >> 16);
    spi1_sendbyte(addr >> 8);
    spi1_sendbyte(addr);
}

///SPIFlash读取
AT(.com_text.spiflash)
void spiflash1_read(void *buf, u32 addr, u32 len)
{
    TRACE("spi read %x: %x %x\n", buf, addr, len);
    u32 i;
    u8 *read_buf = (u8*)buf;
    SPI1_CS_EN();
    spi1_sendbyte(SF_READ);
    spiflash1_sendaddr(addr);
    for (i = 0; i < len; i++) {
       read_buf[i] = spi1_getbyte();
    }
    SPI1_CS_DIS();
}

///SPIFlash编程
AT(.com_text.spiflash)
void spiflash1_write(void *buf, u32 addr, u32 len)
{
    //TRACE("spi write %x: %x %x\n", buf, addr, len);
    u32 i;
    u8 *write_buf = (u8*)buf;
    spiflash1_write_enable();
    SPI1_CS_EN();
    spi1_sendbyte(SF_PROGRAM);
    spiflash1_sendaddr(addr);

//    SPI1CON &= ~BIT(4);                             //TX
//    SPI1DMAADR = DMA_ADR(buf);
//    SPI1DMACNT = len;
//    while (!(SPI1CON & BIT(16)));                   //Wait pending

    for (i = 0; i < len; i++) {
        spi1_sendbyte(write_buf[i]);
    }
    SPI1_CS_DIS();
    spiflash1_waitbusy();
}

///SPIFlash擦除
AT(.com_text.spiflash)
void spiflash1_erase(u32 addr)
{
    TRACE("spi erase: %x\n", addr);
    spiflash1_write_enable();

    SPI1_CS_EN();
    spi1_sendbyte(SF_ERASE);
    spiflash1_sendaddr(addr);
    SPI1_CS_DIS();

    spiflash1_waitbusy();
}

AT(.com_text.spiflash)
void spiflash1_erase_block(u32 addr)
{
    TRACE("spi erase: %x\n", addr);
    spiflash1_write_enable();

    SPI1_CS_EN();
    spi1_sendbyte(SF_ERASE_BLOCK);
    spiflash1_sendaddr(addr);
    SPI1_CS_DIS();

    spiflash1_waitbusy();
}

#endif //REC_TO_SPIFALSH
