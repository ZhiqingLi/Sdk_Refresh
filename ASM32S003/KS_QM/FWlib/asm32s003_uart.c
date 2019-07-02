/*
  ******************************************************************************
  * @file    asm32s003_uart.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"
#include "asm32s003_uart.h"
#include "asm32s003_gpio.h"

/* define --------------------------------------------------------------------*/
volatile U8_T RxDataFlag=0;
volatile U8_T TxDataFlag=0;
volatile U8_T UART0_INTFlag[4];
volatile U8_T UART1_INTFlag[4];        //UART interrupt flag
/* externs--------------------------------------------------------------------*/


/*************************************************************/
//UART RESET,CLEAR ALL REGISTER
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void UART_DeInit(void)
{
    UART0->DATA = UART_RESET_VALUE;
    UART0->SR   = UART_RESET_VALUE;
    UART0->CTRL = UART_RESET_VALUE;
    UART0->ISR  = UART_RESET_VALUE;
    UART0->BRDIV =UART_RESET_VALUE;
    UART1->DATA = UART_RESET_VALUE;
    UART1->SR   = UART_RESET_VALUE;
    UART1->CTRL = UART_RESET_VALUE;
    UART1->ISR  = UART_RESET_VALUE;
    UART1->BRDIV =UART_RESET_VALUE;
}
/*************************************************************/
//UART0 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void UART0_Int_Enable(void)
{
    UART0->ISR=0x0F;									//clear UART0 INT status
	INTC_ISER_WRITE(UART0_INT);							//INT Vector Enable UART0/1 Interrupt in CK802
}
/*************************************************************/
//UART0 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void UART0_Int_Disable(void)
{
	INTC_ICER_WRITE(UART0_INT);							//INT Vector Enable UART0/1 Interrupt in CK802
}
/*************************************************************/
//UART1 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void UART1_Int_Enable(void)
{
    UART1->ISR=0x0F;									//clear UART1 INT status
	INTC_ISER_WRITE(UART1_INT);							//INT Vector Enable UART0/1 Interrupt in CK802
}
/*************************************************************/
//UART1 Interrupt Disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void UART1_Int_Disable(void)
{
    INTC_ICER_WRITE(UART1_INT);							//INT Vector Enable UART0/1 Interrupt in CK802
}
/*************************************************************/
//UART0 Wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void UART0_WakeUp_Enable(void)
{
    INTC_IWER_WRITE(UART0_INT);    
}

/*************************************************************/
//UART0 Wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void UART0_WakeUp_Disable(void)
{
    INTC_IWDR_WRITE(UART0_INT);    
}
/*************************************************************/
//UART0 Wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void UART1_WakeUp_Enable(void)
{
    INTC_IWER_WRITE(UART1_INT);    
}

/*************************************************************/
//UART0 Wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void UART1_WakeUp_Disable(void)
{
    INTC_IWDR_WRITE(UART1_INT);    
}
/*************************************************************/
//UART IO Init
//EntryParameter:IO_UARTX,UART_IO_G
//IO_UARTX:IO_UART0,IO_UART1
//UART_IO_G:0 1
//ReturnValue:NONE
/*************************************************************/
void UART_IO_Init(UART_NUM_TypeDef IO_UART_NUM , U8_T UART_IO_G)
{
    if (IO_UART_NUM==IO_UART0)
    {
		if(UART_IO_G==0)
		{
			GPIOA0->CONHR = (GPIOA0->CONHR&0XFF00FFFF)  | 0x00440000;       //PA0.12->RXD0, PA0.13->TXD0
		}
		else if(UART_IO_G==1)
		{
			GPIOA0->CONLR = (GPIOA0->CONLR&0XFF00FFFF)  | 0x00770000;      //PA0.5->RXD0, PA0.4->TXD0
		}
    }
     if (IO_UART_NUM==IO_UART1)
    {
        if(UART_IO_G==0)
		{
			GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFFF00)  | 0x00000055;       //PB0.1->RXD1, PB0.0->TXD1
		}
		else if(UART_IO_G==1)
		{
			GPIOC0->CONLR = (GPIOC0->CONLR&0XFFFF00FF) | 0X00004400;        //PC0.2->RXD1, PC0.3->TXD1
		}
    }
}
/*************************************************************/
//UART Init
//EntryParameter:UART0,UART1 ,baudrate_u16 
//e.g:
//sys_clk@24MHz, 24/4(div)=6MHz, 6000000/115200bps=52,baudrate_u16=52
//sys_clk@24MHz, 24/2(div)=12MHz, 12000000/115200bps=104,baudrate_u16=104
//ReturnValue:NONE
/*************************************************************/
void UARTInit(CSP_UART_T *uart,U16_T baudrate_u16)
{
   // Set Transmitter Enable
   CSP_UART_SET_CTRL(uart, UART_TX | UART_RX);
   // Set Baudrate
   CSP_UART_SET_BRDIV(uart, baudrate_u16);

}
/*************************************************************/
//UART init and enable RX,TX interrupt
//EntryParameter:UART0,UART1 ,baudrate_u16 
//ReturnValue:NONE
/*************************************************************/
void UARTInitRxTxIntEn(CSP_UART_T *uart,U16_T baudrate_u16)
{
   // Set Transmitter Enable
   CSP_UART_SET_CTRL(uart, UART_TX | UART_RX | UART_RX_INT | UART_TX_INT);
   // Set Baudrate
   CSP_UART_SET_BRDIV(uart, baudrate_u16);

}
/*************************************************************/
//UART init and enable RX interrupt
//EntryParameter:UART0,UART1 ,baudrate_u16 
//ReturnValue:NONE
/*************************************************************/
void UARTInitRxIntEn(CSP_UART_T *uart,U16_T baudrate_u16)
{
   // Set Transmitter Enable
   CSP_UART_SET_CTRL(uart, UART_TX | UART_RX | UART_RX_INT);
   // Set Baudrate
   CSP_UART_SET_BRDIV(uart, baudrate_u16);
}
/*************************************************************/
//UART Close
//EntryParameter:UART0,UART1
//ReturnValue:NONE
/*************************************************************/
void UARTClose(CSP_UART_T *uart)
{
	// Set Transmitter Disable
	CSP_UART_SET_CTRL(uart, 0x00);
}

/*************************************************************/
//UART TX Byte 
//EntryParameter:UART0,UART1,txdata_u8
//ReturnValue:NONE
/*************************************************************/
void UARTTxByte(CSP_UART_T *uart,U8_T txdata_u8)
{

	// Write the transmit buffer
	CSP_UART_SET_DATA(uart,txdata_u8);

	// Wait for the transmit done
	while ( (CSP_UART_GET_ISR(uart) & UART_TX_INT_S) != UART_TX_INT_S ) {}

	// Clear status bit for the next transmit
	CSP_UART_SET_ISR(uart, UART_TX_INT_S);

}
/*************************************************************/
//UART Transmit 
//EntryParameter:UART0,UART1,sourceAddress_u16,length_u16
//ReturnValue:NONE
/*************************************************************/
void UARTTransmit(CSP_UART_T *uart,U16_T *sourceAddress_u16,U16_T length_u16)
{
	unsigned int  DataI,DataJ;
	for(DataJ = 0;DataJ < length_u16 ;DataJ ++)
	{
		CSP_UART_SET_DATA(uart,*sourceAddress_u16++);
		do{
			DataI = CSP_UART_GET_SR(uart);
			DataI = DataI & UART_TX_FULL;
		}while(DataI == UART_TX_FULL);    //Loop  when tx is full
	}
}

/*************************************************************/
//UART RX Byte 
//EntryParameter:UART0,UART1,Rxdata_u16
//ReturnValue:NONE
/*************************************************************/
U16_T UARTRxByte(CSP_UART_T *uart,U16_T *Rxdata_u16)
{
	unsigned int  DataI;

	DataI = CSP_UART_GET_SR(uart);
	DataI = DataI & UART_RX_FULL;
	if(DataI != UART_RX_FULL)    //Loop  when rx is not full
		return FALSE;
	else
	{
		*Rxdata_u16 = CSP_UART_GET_DATA(uart);
	    return TRUE;
	}
}

/*************************************************************/
//UART RX Return Byte 
//EntryParameter:UART0,UART1
//ReturnValue:(uart)->DATA
/*************************************************************/
U8_T UART_ReturnRxByte(CSP_UART_T *uart)
{
	RxDataFlag = FALSE;
	while(RxDataFlag != TRUE);
	return CSP_UART_GET_DATA(uart);
}

/*************************************************************/
//UART Receive
//EntryParameter:UART0,UART1，destAddress_u16，length_u16
//ReturnValue:FALSE/TRUE
/*************************************************************/
U16_T UARTReceive(CSP_UART_T *uart,U16_T *destAddress_u16,U16_T length_u16)
{
	unsigned int  DataI,DataJ,LoopTime;

	DataJ = 0;
	LoopTime = 0;
	do{
		DataI = CSP_UART_GET_SR(uart);
		DataI = DataI & UART_RX_FULL;
		if(DataI == UART_RX_FULL)    //Loop  when rx is full
		{
			*destAddress_u16++ = CSP_UART_GET_DATA(uart);
			DataJ++;
			LoopTime = 0;
		}
		if(LoopTime ++ >= 0xfff0)
			return FALSE;
	}while(DataJ < length_u16);
	return TRUE;
}


/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/