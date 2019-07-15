#include "include.h"
#include "bsp_spiflash1.h"

#if EX_SPIFLASH_SUPPORT

#if 0
#define TRACE(...)              printf(__VA_ARGS__)
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
#define SF_ERASE                0x20                //SPIFlash擦除 //sector erase(4K擦除)

#define SPIFLASH1_BAUD          (200000)            //200K

#define SPIFLASH_ID             0x40170000  //通过读ID判断FLASH是否在线, 需要改成SPIFLASH对应的ID
#define SPIFALSH_BAUD           (500000)    //SPI波特率500K

AT(.text.spiflash1_drv)
void spi1_cs_init(void)
{
    GPIOAFEN &= ~BIT(5);
    GPIOADE |= BIT(5);
    GPIOADIR &= ~BIT(5);
    GPIOASET = BIT(5);
}

AT(.text.spiflash1_drv)
void spi1_cs_en(void)
{
    GPIOACLR = BIT(5);
}

AT(.text.spiflash1_drv)
void spi1_cs_dis(void)
{
    GPIOASET = BIT(5);
}

///SPIFlash Delay
AT(.text.spiflash1_drv)
static void spi1_delay(void)
{
    uint cnt = 20;
    while (cnt--) {
        asm("nop");
    }
}

///SPI接口获取1Byte数据
AT(.text.spiflash1_drv)
static u8 spi1_getbyte(void)
{
    SPI1CON |= BIT(4);                              //RX
    SPI1BUF = 0xff;
    while (!(SPI1CON & BIT(16)));                   //Wait pending
    return SPI1BUF;
}

///SPI接口发送1Byte数据
AT(.text.spiflash1_drv)
static void spi1_sendbyte(u8 data)
{
    SPI1CON &= ~BIT(4);                             //TX
    SPI1BUF = data;
    while (!(SPI1CON & BIT(16)));                   //Wait pending
}

AT(.text.spiflash1_drv)
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

AT(.text.spiflash1_drv)
void spi1_init(u32 baud)
{
    //SPI1_G1: CLK_PA3, DIO_PA4,   //CS_PA5
    GPIOAFEN |= BIT(3)|BIT(4);
    GPIOADE |=  BIT(3)|BIT(4);
    GPIOADIR &= ~(BIT(3) | BIT(4));  //CLK,DO output
    GPIOADIR |= BIT(4);  //DI input
    GPIOAPU |= BIT(4);   //DI pull up 10K
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = (0x01<<12);  //G1

//    //SPI1_G2:  DIO_PA7,  CLK_PA6, //CS_PA5
//    GPIOAFEN |= BIT(6)|BIT(7);
//    GPIOADE |= BIT(6)|BIT(7);
//    GPIOADIR &= ~ BIT(6) ;  //CLK output
//    GPIOADIR |= BIT(7);  //DI input
//    GPIOAPU |= BIT(7);   //DI pull up 10K
//    FUNCMCON1 = (0x0F<<12);
//    FUNCMCON1 = (0x02<<12); //G2

    SPI1_CS_INIT();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SPI1BAUD = 24000000/baud - 1;   //sys 24M.
    SPI1CON =  0x01 | (1<<2);
}

///SPIFlash写使能
AT(.text.spiflash1_drv)
void spiflash1_init(void)
{
    spi1_init(SPIFALSH_BAUD);
    //while(1) {WDT_CLR();printf("Flash ID = 0x%X\n", spiflash1_id_read());delay_ms(300);}

}

///SPIFlash写使能
AT(.text.spiflash1_drv)
static void spiflash1_write_enable(void)
{
    SPI1_CS_EN();
    spi1_sendbyte(SF_WRITE_EN);
    SPI1_CS_DIS();
    spi1_delay();
}

///SPIFlash读状态寄存器
AT(.text.spiflash1_drv)
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
AT(.text.spiflash1_drv)
static void spiflash1_waitbusy(void)
{
    do {
        spi1_delay();
    } while (spiflash1_readssr() & 0x01);
}

///发送SPIFlash的地址
AT(.text.spiflash1_drv)
static void spiflash1_sendaddr(u32 addr)
{
    spi1_sendbyte(addr >> 16);
    spi1_sendbyte(addr >> 8);
    spi1_sendbyte(addr);
}

///SPIFlash读取
AT(.text.spiflash1_drv)
void spiflash1_read(void *buf, u32 addr, u32 len)
{
    TRACE("[r:0x%X,%d]",addr, len);
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
AT(.text.spiflash1_drv)
void spiflash1_write(void *buf, u32 addr, u32 len)
{
    TRACE("[w:0x%X,%d]",addr, len);

    u8 *write_buf = (u8*)buf;
    spiflash1_write_enable();
    SPI1_CS_EN();
    spi1_sendbyte(SF_PROGRAM);
    spiflash1_sendaddr(addr);

    SPI1CON &= ~BIT(4);                             //TX
    SPI1DMAADR = DMA_ADR(write_buf);
    SPI1DMACNT = len;
    while (!(SPI1CON & BIT(16)));                   //Wait pending

//    u32 i;
//    for (i = 0; i < len; i++) {
//        spi1_sendbyte(write_buf[i]);
//    }
    SPI1_CS_DIS();
    spiflash1_waitbusy();
}

///SPIFlash擦除
AT(.text.spiflash1_drv)
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

////#define SF_ERASE_BLOCK          0xD8              //SPIFlash擦除  //block erase   //64K
//AT(.text.spiflash1_drv)
//void spiflash1_erase_block(u32 addr)
//{
//    TRACE("spi erase: %x\n", addr);
//    spiflash1_write_enable();
//
//    SPI1_CS_EN();
//    spi1_sendbyte(SF_ERASE_BLOCK);
//    spiflash1_sendaddr(addr);
//    SPI1_CS_DIS();
//
//    spiflash1_waitbusy();
//}

AT(.text.spiflash1_drv)
bool is_exspiflash_online(void)
{
    u32 flash_id;
    u32 i = 5;
    while(i--) {
        WDT_CLR();
        flash_id = spiflash1_id_read();
        printf("flash id = 0x%X\n",flash_id);
        if (SPIFLASH_ID == flash_id) {
            return true;
        } else {
            printf("--->!!!Read Flash ID Err\n",flash_id);
            delay_ms(1);
        }
    }
    return false;
}

#if SPIFALSH_MUSIC_BIN_WRITE_TEST
extern u8 const exspiflash_music_bin[36864];
void write_music_bin_to_spiflash(void)
{
    spiflash1_init();
    u8 rx_buf[256];
    int block_total,block_ok, block_fail, i;
    block_total = sizeof(exspiflash_music_bin)/256;
    for (i = 0; i < sizeof(exspiflash_music_bin); ) {
        spiflash1_erase(i);
        printf("erase addr = 0x%X\n",i);
        i += 4096;
    }
    printf("write exspiflash_music_bin,len = %d, block_total = %d\n", sizeof(exspiflash_music_bin) ,block_total);
    //write
    for (i = 0; i < block_total; i++) {
        spiflash1_write((void*)&exspiflash_music_bin[i*256], i*256, 256);
        printf("[0x%X]",i*256);
    }
    //read_back and compare
    printf("\nread_back and compare\n");
    block_ok = 0;
    block_fail = 0;
    for (i = 0; i < block_total; i++) {
        spiflash1_read(rx_buf,i*256,256);
        if (memcmp(rx_buf,&exspiflash_music_bin[i*256],256)) {
            printf("[fail:0x%X]",i*256);
            block_fail++;
        } else {
            printf("[ok:0x%X]",i*256);
            block_ok++;
        }
    }
    printf_end("block_ok = %d  block_fail = %d\n", block_ok, block_fail);
}
#endif  //SPIFALSH_MUSIC_BIN_WRITE_TEST

#endif //(EX_SPIFLASH_SUPPORT & EXSPI_REC)
