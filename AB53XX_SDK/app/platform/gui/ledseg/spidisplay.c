#include "include.h"
#include "spidisplay.h"

#if (GUI_SELECT == GUI_SPISEG_16XX)

#if 0
#define TRACE(...)              TRACE(__VA_ARGS__)
#else
#define TRACE(...)
#endif


static void spi1_cs_en(void);
static void spi1_cs_dis(void);
static void spi1_cs_init(void);

#define SPI1_CS_INIT()           spi1_cs_init()
#define SPI1_CS_EN()             spi1_cs_en()
#define SPI1_CS_DIS()            spi1_cs_dis()

#define SPIDISPLY_BAUD          (100000)            //200K

AT(.text.spidisplay)
static void spi1_cs_init(void)
{
    GPIOAFEN &= ~BIT(5);
    GPIOADE |= BIT(5);
    GPIOADIR &= ~BIT(5);
    GPIOASET = BIT(5);
}

AT(.text.spidisplay)
static void spi1_cs_en(void)
{
    GPIOACLR = BIT(5);
}

AT(.text.spidisplay)
static void spi1_cs_dis(void)
{
    GPIOASET = BIT(5);
}

///SPIFlash Delay
AT(.text.spidisplay)
static void spi1_delay(void)
{
    uint cnt = 20;
    while (cnt--) {
        asm("nop");
    }
}

/*//SPI接口获取1Byte数据
AT(.text.spidisplay)
static u8 spi1_getbyte(void)
{
    SPI1CON |= BIT(4);                              //RX
    SPI1BUF = 0xff;
    while (!(SPI1CON & BIT(16)));                   //Wait pending
    return SPI1BUF;
}*/

///SPI接口发送1Byte数据
AT(.text.spidisplay)
static void spi1_sendbyte(u8 data)
{
    SPI1CON &= ~BIT(4);                             //TX
    //数据高低位交换，SPI数据高位先发，16xx要求低位在前
    data = (data<<4)|(data>>4);
    data = ((data<<2)&0xCC)|((data>>2)&0x33);
    data = ((data<<1)&0xAA)|((data>>1)&0x55);
 
    SPI1BUF = data;
    while (!(SPI1CON & BIT(16)));                   //Wait pending
}

AT(.text.spidisplay)
void DisplaySpi1Init(u32 baud)
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
    SPI1CON = (0x01|BIT(2));
}

AT(.text.spidisplay)
void SpiDisplayClr(void)
{
	SPI1_CS_EN();
    spi1_sendbyte(DATA_ADDR_AUTO);
    SPI1_CS_DIS();
    spi1_delay();
	SPI1_CS_EN();
    spi1_sendbyte(ADDR_STA_OFFSET);
    
    uint8_t DataCnt = 0;
    for ( DataCnt = 0 ; DataCnt < MAX_SEND_DATA_LEN ; DataCnt++ )
    {
        spi1_sendbyte(0x00);
    }
    SPI1_CS_DIS();
}

AT(.text.spidisplay)
void SpiDisplayInit(void)
{
	DisplaySpi1Init(SPIDISPLY_BAUD);
	SpiDisplayClr();
	spi1_delay();
	SPI1_CS_EN();
    spi1_sendbyte(LED_DISP_PWM_14);
    SPI1_CS_DIS();
}

AT(.text.spidisplay)
void RefreshSpiDisplayData(void)
{
	uint8_t DisplayBuf[16];
	
	memset(&DisplayBuf, 0x00, sizeof(DisplayBuf));
	//根据硬件逻辑修改以下代码
	DisplayBuf[0] = ledseg_buf[0]&(~CHAR_ACTER_MASK);
	DisplayBuf[2] = ledseg_buf[0]&(~CHAR_ACTER_MASK);

	DisplayBuf[4] = ledseg_buf[1]&(~CHAR_ACTER_MASK);
	DisplayBuf[6] = ledseg_buf[1]&(~CHAR_ACTER_MASK);

	DisplayBuf[8] = ledseg_buf[2]&(~CHAR_ACTER_MASK);
	DisplayBuf[10] = ledseg_buf[2]&(~CHAR_ACTER_MASK);

	DisplayBuf[12] = ledseg_buf[3]&(~CHAR_ACTER_MASK);
	DisplayBuf[14] = ledseg_buf[3]&(~CHAR_ACTER_MASK);
	
	SPI1_CS_EN();
    spi1_sendbyte(DATA_ADDR_AUTO);
    SPI1_CS_DIS();
    spi1_delay();
	SPI1_CS_EN();
	spi1_sendbyte(ADDR_STA_OFFSET);
	uint8_t Adder,DataCnt;
	for (Adder = 0,DataCnt = 0; Adder < MAX_SEND_DATA_LEN; Adder++)
	{
	    spi1_sendbyte(DisplayBuf[DataCnt]);
	    DataCnt++;
	}
	SPI1_CS_DIS();
    spi1_delay();
	SPI1_CS_EN();
    spi1_sendbyte(LED_DISP_PWM_14);
    SPI1_CS_DIS();
}


#endif //(EX_SPIFLASH_SUPPORT & EXSPI_REC)
