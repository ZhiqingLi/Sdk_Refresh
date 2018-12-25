/*
  ******************************************************************************
  * @file    asm32s003_led.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "asm32s003_led.h"

/* define --------------------------------------------------------------------*/
volatile U8_T LED0_ICENDFlag;
volatile U8_T LED0_IPENDFlag;
volatile U8_T LED0_IKEYDETFlag;						//LED interrupt flag
U8_T LED_KEYST;
unsigned char _CHAR_[10] = {
			0x5F,/*0*/
			0x06,/*1*/
			0x3B,/*2*/
			0x2F,/*3*/
			0x66,/*4*/
			0x6D,/*5*/
			0x7D,/*6*/
			0x07,/*7*/
			0x7F,/*8*/
			0x6F,/*9*/
	};
/* externs--------------------------------------------------------------------*/

/*************************************************************/
//LED RESET VALUE
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/  
void LED_RESET_VALUE(void)
{
	 LED0->CR		=	LED0_CR_RST;     					/**< CR  reset value  	*/
	 LED0->BRIGHT	=	LED0_BRIGHT_RST;                  	/**< BRIGHT  reset value */
	 LED0->RISR		= 	LED0_RISR_RST;       		  		/**< RISR reset value  	*/
	 LED0->IMCR		=	LED0_IMCR_RST;             			/**< IMCR   reset value  */
	 LED0->MISR		=   LED0_MISR_RST;                  	/**< MISR   reset value  */
	 LED0->ICR 		=	LED0_ICR_RST;                   	/**< ICR   reset value  */
	 LED0->SEGDATA0 	=	LED0_SEGDAT0_RST;                	/**< SEGDAT0  reset value  */
	 LED0->SEGDATA1 	=	LED0_SEGDAT1_RST;              		/**< SEGDAT1  reset value  */
	 LED0->SEGDATA2  	=	LED0_SEGDAT2_RST;               	/**< SEGDAT2  reset value  */
	 LED0->SEGDATA3 	=	LED0_SEGDAT3_RST;             		/**< SEGDAT3  reset value */
	 LED0->KEYST 	=	LED0_KEYST_RST;             		/**< KEYST  reset value */
	 LED0->TIMCR 	=	LED0_TIMCR_RST;             		/**< TIMCR  reset value */
}  

/*************************************************************/
//LED scan enable
//EntryParameter:NewState
//NewState:ENABLE,DISABLE
//ReturnValue:NONE
/*************************************************************/  
void LED_SCAN_ENABLE(FunctionalStatus NewState)
{
	if (NewState != DISABLE)
	{
		LED0->CR|=LIGHTON;
	}
	else
	{
		LED0->CR&=~LIGHTON;
	}
}

/*************************************************************/
//LED Control config
//EntryParameter:LED_LEDCLK,LED_ComMode,xKEY_SCEN,xTkeyShare,LED_Bright_X,DCOMCNT,NOVCNT,KCOMCNT
//LED_LEDCLK:LEDCLK_DIV4,LEDCLK_DIV8,LEDCLK_DIV16,LEDCLK_DIV32,LEDCLK_DIV64,LEDCLK_DIV128,LEDCLK_DIV256,LEDCLK_DIV512
//LED_ComMode:LED_ComMode_2COM,LED_ComMode_3COM,LED_ComMode_4COM
//xKEY_SCEN:ENKEY_SCEN,DISKEY_SCEN
//xTkeyShare:ENTkeyShare,DISTkeyShare
//TKEYSCCNT:0~7
//LED_Bright_X:LED_Bright_100,LED_Bright_87,LED_Bright_75,LED_Bright_62,LED_Bright_50,LED_Bright_37,LED_Bright_25,LED_Bright_12
//DCOMCNT:VALUE=1~0XFF(clk=LEDCLK/8)
//NOVCNT:VALUE=1~0XFF
//KCOMCNT:VALUE=1~0XFFclk=LEDCLK)
//ReturnValue:NONE
/*************************************************************/  
  //LEDCLK=PLCK/LEDCLK_DIVx
  //LED_Bright_X:LED_Bright_100/LED_Bright_87/LED_Bright_75/LED_Bright_62/LED_Bright_50/LED_Bright_37LED_Bright_25/LED_Bright_12
  //DCOMCNT:The Counter Data to Decide the COM enable time under display scanThe Counter clock is based on LEDCLK/8
  //NOVCNT:The Counter Data to Decide the None-overlap time under display scan
  //KCOMCNT:The Counter Data to Decide the COM enable time under key scanThe Counter clock is based on LEDCLKa
void LED_Control_Config(LED_LEDCLK_TypeDef LED_LEDCLK , LED_ComMode_TypeDef LED_ComMode , LED_KEY_SCEN_TypeDef xKEY_SCEN , LED_TkeyShare_TypeDef xTkeyShare , U8_T TKEYSCCNT , LED_Bright_TypeDef LED_Bright_X , U8_T DCOMCNT , U8_T NOVCNT , U8_T KCOMCNT )
{
	if(LED_ComMode==LED_ComMode_2COM)
	{
		GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFFF00)  | 0x00000055;
		if(xTkeyShare==DISTkeyShare) GPIOA0->CONLR = (GPIOA0->CONLR&0X00000000)  | 0x55555555;
	}
	else if(LED_ComMode==LED_ComMode_3COM)
	{
		GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFF000)  | 0x00000555;
		if(xTkeyShare==DISTkeyShare) GPIOA0->CONLR = (GPIOA0->CONLR&0X00000000)  | 0x55555555;
	}
	else if(LED_ComMode==LED_ComMode_4COM)
	{
		GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFF0000)  | 0x00005555;
		if(xTkeyShare==DISTkeyShare) GPIOA0->CONLR = (GPIOA0->CONLR&0X00000000)  | 0x55555555;
	}
	LED0->CR|=LED_LEDCLK|LED_ComMode|xKEY_SCEN|xTkeyShare|(TKEYSCCNT<<8);
	LED0->BRIGHT=LED_Bright_X;
	LED0->TIMCR=DCOMCNT|(NOVCNT<<8)|(KCOMCNT<<16);
}

/*************************************************************/
//LED Interruput enable and disable
//EntryParameter:LED_INT_x,NewState 
//LED_INT_x:ICEND,IPEND,IKEYDET
//NewState:ENABLE,DISABLE
//ReturnValue:NONE
/*************************************************************/  
  //ICEND:Interrupt Masking Enable/Disable for One COM scan cycle completing
  //IPEND:Interrupt Masking Enable/Disable for All COM scan cycle is completing
  //IKEYDET:Interrupt Masking Enable/Disable for Key Status Change
void LED_INT_CMD(LED_INT_TypeDef LED_INT_x , FunctionalStatus NewState)
{
	if (NewState != DISABLE)
	{
		LED0->IMCR  |= LED_INT_x;						//ENABLE
	}
	else
	{
		LED0->IMCR  &= ~LED_INT_x;						//DIABLE
	}
} 

/*************************************************************/
//LED SEGDATX data write
//EntryParameter:SEGDATX,SEGDATX_data
//SEGDATX:SEGDAT_NUM0,SEGDAT_NUM1,SEGDAT_NUM2,SEGDAT_NUM3
//SEGDATX_data:VALUE=1~0X7F
//ReturnValue:NONE
/*************************************************************/  
void LED_SEGDATX_data_write(LED_SEGDATX_TypeDef SEGDATX , U8_T SEGDATX_data)
{
	switch(SEGDATX)
	{
		case SEGDAT_NUM0:
			LED0->SEGDATA0=SEGDATX_data;
			break;
		case SEGDAT_NUM1:
			LED0->SEGDATA1=SEGDATX_data;
			break;
		case SEGDAT_NUM2:
			LED0->SEGDATA2=SEGDATX_data;
			break;
		case SEGDAT_NUM3:
			LED0->SEGDATA3=SEGDATX_data;
			break;
	}		
}
  
/*************************************************************/
//Led key statue
//EntryParameter:SEGDATX
//SEGDATX:SEGDAT_NUM0,SEGDAT_NUM1,SEGDAT_NUM2,SEGDAT_NUM3
//ReturnValue:key statue value
/*************************************************************/  
  //SEGDATX:SEGDAT_NUM0/SEGDAT_NUM1/SEGDAT_NUM2/SEGDAT_NUM3
U8_T LED_KEYST_Read(LED_SEGDATX_TypeDef SEGDATX)
{
	switch(SEGDATX)
	{
		case SEGDAT_NUM0:
			LED_KEYST=(LED0->KEYST)>>0;
			break;
		case SEGDAT_NUM1:
			LED_KEYST=(LED0->KEYST)>>8;
			break;
		case SEGDAT_NUM2:
			LED_KEYST=(LED0->KEYST)>>16;
			break;
		case SEGDAT_NUM3:
			LED_KEYST=(LED0->KEYST)>>24;
			break;
	}		
	return(LED_KEYST&0x7f);
}
    
/*************************************************************/
//LED Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void LED_Int_Enable(void)
{
    LED0->ICR=0x07;						//CLear LED INT status
	INTC_ISER_WRITE(LED_INT);    
}

/*************************************************************/
//led Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void LED_Int_Disable(void)
{
    INTC_ICER_WRITE(LED_INT);    
}
/*************************************************************/
//LED Interrupt wakeup enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void LED_Wakeup_Enable(void)
{
    INTC_IWER_WRITE(LED_INT);    
}

/*************************************************************/
//led Interrupt wakeup enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void LED_Wakeup_Disable(void)
{
    INTC_IWDR_WRITE(LED_INT);    
}
/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/