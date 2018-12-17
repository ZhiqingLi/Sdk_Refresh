/*
  ******************************************************************************
  * @file    asm32s003_gpio.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "asm32s003_gpio.h"

/* define --------------------------------------------------------------------*/
volatile U8_T EXI_INTFlag[14] ;					//GPIO EXI interrupt flag
/* externs--------------------------------------------------------------------*/

/*************************************************************/
//IO RESET CLEAR ALL REGISTER
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/  
void GPIO_DeInit(void)
{
    GPIOA0->CONLR = GPIO_RESET_VALUE;                     
    GPIOA0->CONHR = GPIO_RESET_VALUE;
    GPIOB0->CONLR = GPIO_RESET_VALUE;                     
    GPIOB0->CONHR = GPIO_RESET_VALUE;
    GPIOA0->WODR  = GPIO_RESET_VALUE; 
    GPIOB0->WODR  = GPIO_RESET_VALUE;
    GPIOA0->SODR  = GPIO_RESET_VALUE; 
    GPIOB0->SODR  = GPIO_RESET_VALUE;
    GPIOA0->CODR  = GPIO_RESET_VALUE; 
    GPIOB0->CODR  = GPIO_RESET_VALUE;
    GPIOA0->PUDR  = GPIO_RESET_VALUE; 
    GPIOB0->PUDR  = GPIO_RESET_VALUE;
    GPIOA0->DSCR  = GPIO_RESET_VALUE;
    GPIOB0->DSCR  = GPIO_RESET_VALUE;
    GPIOA0->IECR  = GPIO_RESET_VALUE;
    GPIOB0->IECR  = GPIO_RESET_VALUE;
    GPIOA0->ODSR  = GPIO_RESET_VALUE;
    GPIOB0->ODSR  = GPIO_RESET_VALUE;
    GPIOA0->OMCR  = GPIO_RESET_VALUE;
    GPIOB0->OMCR  = GPIO_RESET_VALUE;
    GPIOGRP->IGRP  = GPIO_RESET_VALUE;
}  
void GPIO_Debug_IO_12_13(void)
{
    GPIOA0->CONHR |= 0x00550000;
}
void GPIO_Debug_IO_01_02(void)
{
    GPIOA0->CONLR |= 0x00000770;
}
/*************************************************************/
//IO OUTPUT INPUT SET 2
//EntryParameter:GPIOx,byte,val
//GPIOx:GPIOA0,GPIOB0,GPIOC0
//byte:Lowbyte(PIN_0~7),Highbyte(PIN_8~15)
//val:0x0000000~0xFFFFFFFF
//val=0x11111111 all IO as input
//val=0x22222222 all IO as output
//ReturnValue:NONE
/*************************************************************/  
void GPIO_Init2(CSP_GPIO_T *GPIOx,GPIO_byte_TypeDef byte,uint32_t val)
{
    if (byte==0)
    {
        (GPIOx)->CONLR=val;
    }
    else if(byte==1)
    {
        (GPIOx)->CONHR=val;
    }
}
/*************************************************************/
//IO OUTPUT INPUT SET 1
//EntryParameter:GPIOx,GPIO_Pin(0~15),byte,Dir
//GPIOx:GPIOA0,GPIOB0,GPIOC0
//GPIO_Pin:PIN_0~15
//byte:Lowbyte(PIN_0~7),Highbyte(PIN_8~15)
//Dir:0:output 1:input
//ReturnValue:NONE
/*************************************************************/  
void GPIO_Init(CSP_GPIO_T *GPIOx,uint8_t PinNum,GPIO_Dir_TypeDef Dir)
{
    uint32_t data_temp;
    uint8_t GPIO_Pin;
    if(PinNum<8)
    {
	    switch (PinNum)
	    {
	        case 0:data_temp=0xfffffff0;GPIO_Pin=0;break;
	        case 1:data_temp=0xffffff0f;GPIO_Pin=4;break;
	        case 2:data_temp=0xfffff0ff;GPIO_Pin=8;break;
	        case 3:data_temp=0xffff0fff;GPIO_Pin=12;break;
	        case 4:data_temp=0xfff0ffff;GPIO_Pin=16;break;
	        case 5:data_temp=0xff0fffff;GPIO_Pin=20;break;
	        case 6:data_temp=0xf0ffffff;GPIO_Pin=24;break;
	        case 7:data_temp=0x0fffffff;GPIO_Pin=28;break;
	    }
        if (Dir)
        {
        	(GPIOx)->CONLR =((GPIOx)->CONLR & data_temp) | 1<<GPIO_Pin;
        }
        else
        {
        	(GPIOx)->CONLR = ((GPIOx)->CONLR & data_temp) | 2<<GPIO_Pin; 
        }
    }
    else if (PinNum<16)
    {
        switch (PinNum)
	    {
	        case 8:data_temp=0xfffffff0;GPIO_Pin=0;break;
	        case 9:data_temp=0xffffff0f;GPIO_Pin=4;break;
	        case 10:data_temp=0xfffff0ff;GPIO_Pin=8;break;
	        case 11:data_temp=0xffff0fff;GPIO_Pin=12;break;
	        case 12:data_temp=0xfff0ffff;GPIO_Pin=16;break;
	        case 13:data_temp=0xff0fffff;GPIO_Pin=20;break;
	        case 14:data_temp=0xf0ffffff;GPIO_Pin=24;break;
	        case 15:data_temp=0x0fffffff;GPIO_Pin=28;break;
	    } 
      	if (Dir)
        {
        	(GPIOx)->CONHR = ((GPIOx)->CONHR & data_temp) | 1<<GPIO_Pin;  
        }
        else
        {
        	(GPIOx)->CONHR = ((GPIOx)->CONHR & data_temp) | 2<<GPIO_Pin;    
        }
    }
}
/*************************************************************/
//IO OUTPUT INPUT Disable
//EntryParameter:GPIOx,GPIO_Pin(0~15)
//GPIOx:GPIOA0,GPIOB0,GPIOC0
//GPIO_Pin:PIN_0~15
//byte:Lowbyte(PIN_0~7),Highbyte(PIN_8~15)
//ReturnValue:NONE
/*************************************************************/  
void GPIO_InPutOutPut_Disable(CSP_GPIO_T *GPIOx,uint8_t PinNum)
{
    uint32_t data_temp;
    uint8_t GPIO_Pin;
    if(PinNum<8)
    {
    switch (PinNum)
    {
        case 0:data_temp=0xfffffff0;GPIO_Pin=0;break;
        case 1:data_temp=0xffffff0f;GPIO_Pin=4;break;
        case 2:data_temp=0xfffff0ff;GPIO_Pin=8;break;
        case 3:data_temp=0xffff0fff;GPIO_Pin=12;break;
        case 4:data_temp=0xfff0ffff;GPIO_Pin=16;break;
        case 5:data_temp=0xff0fffff;GPIO_Pin=20;break;
        case 6:data_temp=0xf0ffffff;GPIO_Pin=24;break;
        case 7:data_temp=0x0fffffff;GPIO_Pin=28;break;
    }
         (GPIOx)->CONLR = (GPIOx)->CONLR & data_temp;
    }
    else if (PinNum<16)
    {
        switch (PinNum)
    {
        case 8:data_temp=0xfffffff0;GPIO_Pin=0;break;
        case 9:data_temp=0xffffff0f;GPIO_Pin=4;break;
        case 10:data_temp=0xfffff0ff;GPIO_Pin=8;break;
        case 11:data_temp=0xffff0fff;GPIO_Pin=12;break;
        case 12:data_temp=0xfff0ffff;GPIO_Pin=16;break;
        case 13:data_temp=0xff0fffff;GPIO_Pin=20;break;
        case 14:data_temp=0xf0ffffff;GPIO_Pin=24;break;
        case 15:data_temp=0x0fffffff;GPIO_Pin=28;break;
    } 
         (GPIOx)->CONHR = (GPIOx)->CONHR & data_temp;    
    }
}
/*************************************************************/
//IO OUTPUT INPUT SET
//EntryParameter:IO_MODE,GPIOx,val
//GPIOx:GPIOA0,GPIOB0,GPIOC0
//IO_MODE:PUDR(IO PULL HIGH/LOW)
//IO_MODE:DSCR(IO DRIVE STRENGHT)
//IO_MODE:OMCR(IO OPEN DRAIN)
//IO_MODE:IECR(IO INT ENABLE)
//IO_MODE:IGRP(IO INT GROUP)
//ReturnValue:NONE
/*************************************************************/  
void GPIO_MODE_Init(CSP_GPIO_T *GPIOx,GPIO_Mode_TypeDef IO_MODE,uint32_t val)
{
        switch (IO_MODE)
        {
            case 0:(GPIOx)->PUDR  = val;break;               
            case 1:(GPIOx)->DSCR  = val;break;
            case 2:(GPIOx)->OMCR  = val;break;
            case 3:(GPIOx)->IECR  = val;break;
            case 4:GPIOGRP->IGRP  = val;break;
        }
}
/*************************************************************/
//Write GPIO pull high/low
//EntryParameter:GPIOx,uint8_t bit
//GPIOx:GPIOA0,GPIOB0,GPIOC0
//bit:0~15
//ReturnValue:VALUE
/*************************************************************/
void GPIO_PullHigh_Init(CSP_GPIO_T *GPIOx,uint8_t bit)
{
	(GPIOx)->PUDR  = ((GPIOx)->PUDR) | (0x01<<(bit*2));
}
void GPIO_PullLow_Init(CSP_GPIO_T *GPIOx,uint8_t bit)
{
	(GPIOx)->PUDR  = ((GPIOx)->PUDR) | (0x02<<(bit*2));
}
void GPIO_PullHighLow_DIS(CSP_GPIO_T *GPIOx,uint8_t bit)
{
	(GPIOx)->PUDR  = ((GPIOx)->PUDR) & ~(0x03<<(bit*2));
}
/*************************************************************/
//Write GPIO open drain init
//EntryParameter:GPIOx,uint8_t bit
//GPIOx:GPIOA0,GPIOB0,GPIOC0
//bit:0~15
//ReturnValue:VALUE
/*************************************************************/
void GPIO_OpenDrain_EN(CSP_GPIO_T *GPIOx,uint8_t bit)
{
	(GPIOx)->OMCR  = ((GPIOx)->OMCR) | (0x01<<bit);
}
void GPIO_OpenDrain_DIS(CSP_GPIO_T *GPIOx,uint8_t bit)
{
	(GPIOx)->OMCR  = ((GPIOx)->OMCR) & ~(0x01<<bit);
}
/*************************************************************/
//Write GPIO Drive Strength init
//EntryParameter:GPIOx,uint8_t bit
//GPIOx:GPIOA0,GPIOB0,GPIOC0
//bit:0~15
//ReturnValue:VALUE
/*************************************************************/
void GPIO_DriveStrength_EN(CSP_GPIO_T *GPIOx,uint8_t bit)
{
	(GPIOx)->DSCR  = ((GPIOx)->DSCR) | (0x03<<(bit*2));
}
void GPIO_DriveStrength_DIS(CSP_GPIO_T *GPIOx,uint8_t bit)
{
	(GPIOx)->DSCR  = ((GPIOx)->DSCR) & ~(0x03<<(bit*2));
}
/*************************************************************/
//IO OUTPUT INPUT SET
//EntryParameter:
//IO_MODE:IGRP(IO INT GROUP)
//ReturnValue:NONE
/*************************************************************/  
void GPIO_IntGroup_Set(GPIO_Group_TypeDef IO_MODE)
{
        switch (IO_MODE)
        {
            case 0:GPIOGRP->IGRP  = 0X00000000;break;               
            case 1:GPIOGRP->IGRP  = 0X05555555;break;
            case 2:GPIOGRP->IGRP  = 0X0AAAAAAA;break;
            case 3:GPIOGRP->IGRP  = 0X0FFFFFFF;break;
        }
}
/*************************************************************/
//IO EXI SET 
//EntryParameter:EXI_IO(EXI0~EXI13)
//ReturnValue:NONE
/*************************************************************/  
void GPIOA0_EXI_Init(GPIO_EXI_TypeDef EXI_IO)
{
    switch (EXI_IO)
    {
        case 0:GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFFFFF0) | 0X00000001;break;
        case 1:GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFFFF0F) | 0X00000010;break;
        case 2:GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFFF0FF) | 0X00000100;break;
        case 3:GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFF0FFF) | 0X00001000;break;
        case 4:GPIOA0->CONLR = (GPIOA0->CONLR&0XFFF0FFFF) | 0X00010000;break;
        case 5:GPIOA0->CONLR = (GPIOA0->CONLR&0XFF0FFFFF) | 0X00100000;break;
        case 6:GPIOA0->CONLR = (GPIOA0->CONLR&0XF0FFFFFF) | 0X01000000;break;
        case 7:GPIOA0->CONLR = (GPIOA0->CONLR&0X0FFFFFFF) | 0X10000000;break;
        case 8:GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFFFF0) | 0X00000001;break;
        case 9:GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFFF0F) | 0X00000010;break;
        case 10:GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFF0FF) | 0X00000100;break;
        case 11:GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFF0FFF) | 0X00001000;break;
        case 12:GPIOA0->CONHR = (GPIOA0->CONHR&0XFFF0FFFF) | 0X00010000;break;
        case 13:GPIOA0->CONHR = (GPIOA0->CONHR&0XFF0FFFFF) | 0X00100000;break;
    }
}
void GPIOB0_EXI_Init(GPIO_EXI_TypeDef EXI_IO)
{
    switch (EXI_IO)
    {
        case 0:GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFFFF0) | 0X00000001;break;
        case 1:GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFFF0F) | 0X00000010;break;
        case 2:GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFF0FF) | 0X00000100;break;
        case 3:GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFF0FFF) | 0X00001000;break;
        default:break;
    }
}
void GPIOC0_EXI_Init(GPIO_EXI_TypeDef EXI_IO)
{
    switch (EXI_IO)
    {
        case 0:GPIOC0->CONLR = (GPIOC0->CONLR&0XFFFFFFF0) | 0X00000001;break;
        case 1:GPIOC0->CONLR = (GPIOC0->CONLR&0XFFFFFF0F) | 0X00000010;break;
        case 2:GPIOC0->CONLR = (GPIOC0->CONLR&0XFFFFF0FF) | 0X00000100;break;
        case 3:GPIOC0->CONLR = (GPIOC0->CONLR&0XFFFF0FFF) | 0X00001000;break;
        default:break;
    }
}
void GPIO_EXI_EN(CSP_GPIO_T *GPIOx,GPIO_EXI_TypeDef EXI_IO)
{
    (GPIOx)->IECR  |= 1<<EXI_IO;
}
/*************************************************************/
//Write GPIO 
//EntryParameter:GPIOx,uint8_t bit
//GPIOx:GPIOA0,GPIOB0,GPIOC0
//bit:0~15
//ReturnValue:VALUE
/*************************************************************/
void GPIO_Write_High(CSP_GPIO_T *GPIOx,uint8_t bit)
{
        (GPIOx)->SODR = (1ul<<bit);
}
void GPIO_Write_Low(CSP_GPIO_T *GPIOx,uint8_t bit)
{
        (GPIOx)->CODR = (1ul<<bit);
}
/*************************************************************/
//Write GPIO 
//EntryParameter:GPIOx,uint8_t bitposi bitval
//GPIOx:GPIOA0,GPIOB0,GPIOC0
//bitposi:0~15 bitval:0~1 0=low 1=high
//ReturnValue:VALUE
/*************************************************************/
void GPIO_Set_Value(CSP_GPIO_T *GPIOx,uint8_t bitposi,uint8_t bitval)
{
    if (bitval==1)
    {
        (GPIOx)->SODR = (1ul<<bitposi);
    }
    else if ((bitval==0))
    {
        (GPIOx)->CODR = (1ul<<bitposi);
    }
}
/*************************************************************/
//Write GPIO reverse
//EntryParameter:GPIOx,uint8_t bit
//GPIOx:GPIOA0,GPIOB0,GPIOC0
//bit:0~15
//ReturnValue:VALUE
/*************************************************************/
void GPIO_Reverse(CSP_GPIO_T *GPIOx,uint8_t bit)
{
     uint32_t dat = 0;
     dat=((GPIOx)->ODSR>>bit)&1ul;
     {
       if (dat==1)  
       {
           (GPIOx)->CODR = (1ul<<bit);
           return;
       }
       if (dat==0)
       {
           (GPIOx)->SODR = (1ul<<bit);
           return;
       }
     }
}
/*************************************************************/
//READ PA IO STATUS
//EntryParameter:GPIOx,uint8_t bit
//GPIOx:GPIOA0,GPIOB0,GPIOC0
//bit:0~15
//ReturnValue:VALUE
/*************************************************************/
uint8_t GPIO_Read_Status(CSP_GPIO_T *GPIOx,uint8_t bit)
{
    uint8_t value = 0;
    uint32_t dat = 0;
    dat=((GPIOx)->PSDR)&(1<<bit);
    if (dat == (1<<bit))								
	{
	    value = 1;
	} 
    return value;
}
/*************************************************************/
//READ PA OUTPUT STATUS
//EntryParameter:GPIOx,uint8_t bit
//GPIOx:GPIOA0,GPIOB0,GPIOC0
//bit:0~15
//ReturnValue:VALUE
/*************************************************************/
uint8_t GPIO_Read_Output(CSP_GPIO_T *GPIOx,uint8_t bit)
{
    uint8_t value = 0;
    uint32_t dat = 0;
    dat=((GPIOx)->ODSR)&(1<<bit);
    if (dat == (1<<bit))								
	{
	    value = 1;
	} 
    return value;
}
/*************************************************************/
//set PA0.0 as reset pin
//EntryParameter:NONE
//ReturnValue:VALUE
/*************************************************************/
void GPIOA00_Set_ResetPin()
{
    GPIOA0->CONLR=(GPIOA0->CONLR & 0XFFFFFFF0)|0x00000006; 
}
/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/