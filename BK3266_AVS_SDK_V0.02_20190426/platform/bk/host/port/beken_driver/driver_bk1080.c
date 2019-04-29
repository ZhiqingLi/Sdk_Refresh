/*--------------------------------------------------------------------------*/
/**@file     bk1080.c
   @brief    BK1080收音底层驱动
   @details
   @author
   @date   2011-3-30
   @note
*/
/*----------------------------------------------------------------------------*/
#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "driver_bk1080.h"

#ifdef FM_BK1080H

const __attribute__((section(".rodata")))  uint8 TABLE_BK_VOULE[] =
{
	0,  2,	4,	4,	5,	6,	7,	8,	8,	9,
	9,	10,	10,	10,	10,	11,	11,	11,	12,	12,	
	12,	12,	13,	13,	13,	14,	14,	14,	14,	15,
	15,	15,
};

const __attribute__((section(".rodata")))  uint16 HW_Reg[]=
{
    0x0008,
    0x1080,
    //#ifdef XTAL_CLOCK //reg2
    0x1201, //for crystal clock --32.768K
    //#else
    //0x0201,   //for extern clock
    //#endif
    0x0000,
    0x40C0,
    0x0A1F,
    0x002E,
    0x02FF,
    0x5B11,
    0x0000,
    0x411E,
    0x0000,
    0xCE00,
    0x0000,
    0x0000,
    0x1000,
    0x0010,//reg16
    0x0000,
    0x13FF,
    0x9852,
    0x0000,
    0x0000,
    0x0008,
    0x0000,
    0x51E1,
    0x28DC, //reg25
    0x2645,
    0x00E4,
    0x1CD8,
    0x3A50,
    0xEAE0,
    0x3000,//31
    0x0200,
    0x0000,
};

#define BK_DATA_HIGH()    gpio_output(DATA_PIN,1)
#define BK_DATA_LOW()     gpio_output(DATA_PIN,0)
#define BK_DATA_READ()    gpio_input(DATA_PIN)

#define BK_CLK_HIGH()     gpio_output(CLK_PIN,1)
#define BK_CLK_LOW()      gpio_output(CLK_PIN,0)

#define SDADIROUT()        gpio_config(DATA_PIN,1)
#define SDADIRIN()         gpio_config(DATA_PIN,3)
#define SCLDIROUT()        gpio_config(CLK_PIN,1)

void Delay1us()//please revise this function according to your system
{
	volatile uint16 i;
	for(i=0;i<10;i++);
}

/*************************************************
  Function:       BEKEN_I2C_init
  Description:    BEKEN I2C initialize
  Input:          None
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_init(void)
{
    SDADIROUT();                  //SDA output
    SCLDIROUT();                  //SCL output
    BK_CLK_HIGH();
    BK_DATA_HIGH();
}

/*************************************************
  Function:       BEKEN_I2C_Start
  Description:    BEKEN I2C transaction start
  Input:          None
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_Start(void)
{
    BEKEN_I2C_init();
    Delay1us();
    BK_DATA_LOW();
    Delay1us();
    BK_CLK_LOW();
    Delay1us();
    BK_DATA_HIGH();
}

/*************************************************
  Function:       BEKEN_I2C_Stop
  Description:    BEKEN I2C transaction end
  Input:          None
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_Stop(void)
{
    SDADIROUT();
    BK_DATA_LOW();
    Delay1us();
    BK_CLK_HIGH();
    Delay1us();
    BK_DATA_HIGH();
    Delay1us();
}

/*************************************************
  Function:       BEKEN_I2C_ack
  Description:    send ACK,accomplish a operation
  Input:          None
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_ack(void)
{
    SDADIROUT();

    BK_CLK_LOW();
    BK_DATA_LOW();
    Delay1us();

    BK_CLK_HIGH();
    Delay1us();
    BK_CLK_LOW();
}      

/*************************************************
  Function:       BEKEN_I2C_nack
  Description:    send NACK,accomplish a operation
  Input:          None
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_nack(void)
{
    SDADIROUT();

    BK_CLK_LOW();
    BK_DATA_HIGH();
    Delay1us();

    BK_CLK_HIGH();
    Delay1us();
    BK_CLK_LOW();
}

/*************************************************
  Function:       BEKEN_I2C_ReceiveACK
  Description:    wait ACK,accomplish a operation
  Input:          None
                          
  Output:         None
  Return:         ack flag
*************************************************/
uint8 BEKEN_I2C_ReceiveACK(void)
{
    uint32 ackflag;
    SDADIRIN();
    Delay1us();
    BK_CLK_HIGH();
    Delay1us();
    ackflag = (uint8)BK_DATA_READ();
    BK_CLK_LOW();
    Delay1us();
    return ackflag;
}

/*************************************************
  Function:       BEKEN_I2C_sendbyte
  Description:    write a byte
  Input:          a byte written
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_sendbyte(uint8 I2CSendData)
{
   
    uint8  i;
    SDADIROUT();
    Delay1us();;
    for(i = 0;i < 8;i++)
    {
        if(I2CSendData & 0x80)
        {
            BK_DATA_HIGH();     //if high bit is 1,SDA= 1
        }
        else
            BK_DATA_LOW();                               //else SDA=0

       	Delay1us();
        BK_CLK_HIGH();
       	Delay1us();
        I2CSendData <<= 1;                          //shift left 1 bit
        BK_CLK_LOW();
    }                       
}

/*************************************************
  Function:       BEKEN_I2c_Readbyte
  Description:    read a byte
  Input:          None
                          
  Output:         None
  Return:         a byte read
*************************************************/
uint8 BEKEN_I2c_Readbyte(void)
{
    uint8 i;
    uint8 ucRDData = 0;                     //return value
    
    SDADIRIN();
    Delay1us();;
    for(i = 0;i < 8;i++)
    {
        BK_CLK_HIGH();
        ucRDData <<= 1;
        Delay1us();
        if(BK_DATA_READ())
            ucRDData|=0x01;
        BK_CLK_LOW();
        Delay1us();
    }
    return(ucRDData);
}

/*************************************************
  Function:       BEKEN_I2c_Read
  Description:    read a array
  Input:          reg: register number
                  len:the length of read data   
                          
  Output:         pBuff: a buffer contain read data
  Return:         None
*************************************************/
void BEKEN_I2c_Read(uint8 reg,uint8 *pBuf,uint8 len)
{
    uint8 i;
    uint8 Addr;
        
   uint32 interrupts_info, mask;
   VICMR_disable_interrupts(&interrupts_info, &mask);
    
    Addr=CHIP_DEV_ID;//device address.

    BEKEN_I2C_Start();

    BEKEN_I2C_sendbyte(Addr);   
    BEKEN_I2C_ReceiveACK();

    reg=reg<<1;
    reg|=0x01;
    BEKEN_I2C_sendbyte(reg);
    BEKEN_I2C_ReceiveACK();

    for(i=0;i<len-1;i++)
    {
        pBuf[i] = BEKEN_I2c_Readbyte();
        BEKEN_I2C_ack();
    }
    pBuf[i] = BEKEN_I2c_Readbyte();
    BEKEN_I2C_nack();
    
    BEKEN_I2C_Stop();
    
    VICMR_restore_interrupts(interrupts_info, mask);
 
}

/*************************************************
  Function:       BEKEN_I2c_Write
  Description:    write a array
  Input:          reg: register number
                  pBuff: a buffer contain written data
                  len:the length of written data      
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2c_Write(uint8 reg,uint8 *pBuf,uint8 len)
{
    uint8 i;
    uint8 Addr;

   uint32 interrupts_info, mask;
   VICMR_disable_interrupts(&interrupts_info, &mask);
    
    Addr=CHIP_DEV_ID;//device address.

    BEKEN_I2C_Start();

    BEKEN_I2C_sendbyte(Addr);
    BEKEN_I2C_ReceiveACK();

    reg=reg<<1;
    BEKEN_I2C_sendbyte(reg);   
    BEKEN_I2C_ReceiveACK();
	 
    for(i=0;i<len;i++)
    {
        BEKEN_I2C_sendbyte(pBuf[i]);
        BEKEN_I2C_ReceiveACK(); 
    }
    BEKEN_I2C_Stop();
    
     VICMR_restore_interrupts(interrupts_info, mask);
}


/*----------------------------------------------------------------------------*/
/**@brief    BK1080 初始化
   @param    无
   @return   无
   @note     void init_BK1080(void)
*/
/*----------------------------------------------------------------------------*/
void init_BK1080(void)
{

}


/*----------------------------------------------------------------------------*/
/**@brief    设置一个频点 BK1080
   @param    curFreq：设置频点
   @return   无
   @note     void BK1080_SetFreq(u16 curFreq)
*/
/*----------------------------------------------------------------------------*/
void BK1080_SetFreq(uint16 curFreq)
{

}


/*----------------------------------------------------------------------------*/
/**@brief    设置一个频点BK1080
   @param    fre 频点  875~1080
   @return   1：当前频点有台，0：当前频点无台
   @note     bool set_fre_BK1080(u16 freq)
*/
/*----------------------------------------------------------------------------*/
uint8 set_fre_BK1080(uint16 freq)
{

    return 1;
}



/*----------------------------------------------------------------------------*/
/**@brief    关闭 BK1080的电源
   @param    无
   @return   无
   @note     void BK1080_PowerDown(void)
*/
/*----------------------------------------------------------------------------*/
void BK1080_PowerDown(void)
{

}



/*----------------------------------------------------------------------------*/
/**@brief    FM 模块静音控制
   @param    dir：1-mute 0-unmute
   @return   无
   @note     void BK1080_mute(dir)
*/
/*----------------------------------------------------------------------------*/
void BK1080_mute(uint8 flag)
{

}



/*----------------------------------------------------------------------------*/
/**@brief   
   @param   
   @return   
   @note    
*/
/*----------------------------------------------------------------------------*/
void BK1080_set_vol(uint8 vol)
{

}



/*----------------------------------------------------------------------------*/
/**@brief   FM模块检测，获取BK1080 模块ID
   @param   无
   @return  检测到BK1080模块返回1，否则返回0
   @note    bool BK1080_Read_ID(void)   
*/
/*----------------------------------------------------------------------------*/
uint8  BK1080_Read_ID(void)
{

    return FALSE;//
}

#endif


