/*
  ******************************************************************************
  * @file    asm32s003_gtc.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "asm32s003_gtc.h"
#include "app_config.h"

/* define --------------------------------------------------------------------*/
volatile U8_T GTC0_PENDIFlag;
volatile U8_T GTC0_STARTIFlag;
volatile U8_T GTC0_PSTARTIFlag;
volatile U8_T GTC0_MATIFlag;
volatile U8_T GTC0_STOPIFlag;
volatile U8_T GTC0_CA1semiIFlag;
volatile U8_T GTC0_OVFIFlag;

volatile U8_T GTC1_PENDIFlag;
volatile U8_T GTC1_STARTIFlag;
volatile U8_T GTC1_PSTARTIFlag;
volatile U8_T GTC1_MATIFlag;
volatile U8_T GTC1_STOPIFlag;
volatile U8_T GTC1_CA1semiIFlag;
volatile U8_T GTC1_OVFIFlag;

volatile U8_T GTC2_PENDIFlag;
volatile U8_T GTC2_STARTIFlag;
volatile U8_T GTC2_PSTARTIFlag;
volatile U8_T GTC2_MATIFlag;
volatile U8_T GTC2_STOPIFlag;
volatile U8_T GTC2_CA1semiIFlag;
volatile U8_T GTC2_OVFIFlag;

volatile U8_T GTC3_PENDIFlag;
volatile U8_T GTC3_STARTIFlag;
volatile U8_T GTC3_PSTARTIFlag;
volatile U8_T GTC3_MATIFlag;
volatile U8_T GTC3_STOPIFlag;
volatile U8_T GTC3_CA1semiIFlag;
volatile U8_T GTC3_OVFIFlag;
//GTC interrupt flag
/* externs--------------------------------------------------------------------*/



/*************************************************************/
//GTC RESET VALUE
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/  
void GTC_RESET_VALUE(CSP_GTC_T * GTC)									//reset value
{
	GTC->IDR 	= GTC_IDR_RST;											/**< IDR reset value         */    
	GTC->CSSR 	= GTC_CSSR_RST;	     								   	/**< CSSR reset value        */
	GTC->CEDR 	= GTC_CEDR_RST;  	 									/**< CEDR reset value        */
	GTC->SRR 	= GTC_SRR_RST;             								/**< SRR reset value         */
	GTC->CSR 	= GTC_CSR_RST;             	 							/**< CSR reset value         */
	GTC->CCR 	= GTC_CCR_RST;              							/**< CCR reset value         */
	GTC->IMSCR 	= GTC_IMSCR_RST;        								/**< IMSCR reset value       */
	GTC->RISR	= GTC_RISR_RST;          								/**< RISR reset value        */
	GTC->MISR 	= GTC_MISR_RST;         								/**< MIS reset value        */
	GTC->ICR 	= GTC_ICR_RST;          								/**< ICR reset value         */
	GTC->SR 	= GTC_SR_RST;           								/**< SR reset value          */
	GTC->CDR 	= GTC_CDR_RST;        									/**< SR reset value          */
	GTC->CSMR 	= GTC_CSMR_RST;           								/**< SR reset value          */
	GTC->CVR 	= GTC_CVR_RST;              							/**< CVR reset value         */
	GTC->PRDR 	= GTC_PRDR_RST;           								/**< DR reset value          */
	GTC->PULR	= GTC_PULR_RST;          								/**< SR reset value          */
	GTC->UCDR 	= GTC_UCDR_RST;           								/**< SR reset value          */
	GTC->UCSMR 	= GTC_UCSMR_RST;        								/**< SR reset value          */
	GTC->UPRDR 	= GTC_UPRDR_RST;        								/**< DR reset value          */
	GTC->UPULR 	= GTC_UPULR_RST;        				 				/**< SR reset value          */
	GTC->CUCR 	= GTC_CUCR_RST;           								/**< SR reset value          */
	GTC->CUCR 	= GTC_CUCR_RST;         								/**< SR reset value          */
	GTC->CDCR 	= GTC_CDCR_RST;            								/**< SR reset value          */
}

/*************************************************************/
//GTC MASK VALUE
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/  
void GTC_MASK_VALUE(CSP_GTC_T *GTC)
{
	GTC->IDR 	= GTC_IDR_MASK;											/**< IDR MASK value         */    
	GTC->CSSR 	= GTC_CSSR_MASK;	     								/**< CSSR MASK value        */
	GTC->CEDR 	= GTC_CEDR_MASK;  	 									/**< CEDR MASK value        */
	GTC->SRR 	= GTC_SRR_MASK;             							/**< SRR MASK value         */
	GTC->CSR 	= GTC_CSR_MASK;             	 						/**< CSR MASK value         */
	GTC->CCR 	= GTC_CCR_MASK;              							/**< CCR MASK value         */
	GTC->IMSCR 	= GTC_IMSCR_MASK;        								/**< IMSCR MASK value       */
	GTC->RISR	= GTC_RISR_MASK;          								/**< RISR MASK value        */
	GTC->MISR 	= GTC_MISR_MASK;         								/**< MIS MASK value        */
	GTC->ICR 	= GTC_ICR_MASK;          								/**< ICR MASK value         */
	GTC->SR 	= GTC_SR_MASK;           								/**< SR MASK value          */
	GTC->CDR 	= GTC_CDR_MASK;        									/**< SR MASK value          */
	GTC->CSMR 	= GTC_CSMR_MASK;           								/**< SR MASK value          */
	GTC->CVR 	= GTC_CVR_MASK;              							/**< CVR MASK value         */
	GTC->PRDR 	= GTC_PRDR_MASK;           								/**< DR MASK value          */
	GTC->PULR	= GTC_PULR_MASK;          								/**< SR MASK value          */
	GTC->UCDR 	= GTC_UCDR_MASK;           								/**< SR MASK value          */
	GTC->UCSMR 	= GTC_UCSMR_MASK;        								/**< SR MASK value          */
	GTC->UPRDR 	= GTC_UPRDR_MASK;        								/**< DR MASK value          */
	GTC->UPULR 	= GTC_UPULR_MASK;        				 				/**< SR MASK value          */
	GTC->CUCR 	= GTC_CUCR_MASK;           								/**< SR MASK value          */
	GTC->CUCR 	= GTC_CUCR_MASK;         								/**< SR MASK value          */
	GTC->CDCR 	= GTC_CDCR_MASK;            							/**< SR MASK value          */
}

/*************************************************************/
//gtc Configure
//EntryParameter:*GTC,GTC_FIN_X,GTC_DIVN,GTC_DINM,Counter_Size_X,loadCounter_PRDR,loadCounter_PULR
//*GTC:GTC0,GTC1,GTC2,GTC3
//GTC_FIN_X:GTC_FIN_PCK,GTC_FIN_TCLK0,GTC_FIN_TCLK2,GTC_FIN_CountA,GTC_FIN_IMOSC
//GTC_DIVN:VALUE=0~15
//GTC_DINM:0~255
//Counter_Size_X:Counter_Size_8BIT,Counter_Size_10BIT,Counter_Size_16BIT,Counter_Size_32BIT
//loadCounter_PRDR:Timer/Counter Period Data Register
//loadCounter_PULR:loadCounter_PULR<loadCounter_PRDR  (attention)
//ReturnValue:NONE
/*************************************************************/  
  //TCCLK=FIN/2^DIVN/(DINM+1)  GTC_FIN_TypeDef;
  //It is forbidden to set DIVM to zero when DIVN is not zero
void GTC_Configure(CSP_GTC_T *GTC , GTC_FIN_TypeDef  GTC_FIN_X, int  GTC_DIVN , int  GTC_DINM , Counter_Size_TypeDef  Counter_Size_X  , U32_T loadCounter_PRDR , U32_T loadCounter_PULR)
{
	GTC->CSSR = GTC_FIN_X;													//selected GTC clk
	GTC->CEDR = GTC_CLKEN|GTC_DBGEN;										//ENABLE GTC CLK
	GTC->CDR = GTC_DIVN|(GTC_DINM<<4);										//DIVN and DINM set
	GTC->CSMR = Counter_Size_X;												//selected GTC conter size
	GTC->PRDR = loadCounter_PRDR;											//Period of GTC date register
	GTC->PULR = loadCounter_PULR;											//Pulse of GTC date register
}

/*************************************************************/
//gtc Timer/Counter Control Set Register
//EntryParameter:*GTC,GTC_ControlSet_x,NewState
//*GTC:GTC0,GTC1,GTC2,GTC3
//GTC_ControlSet_x:Reference library
//NewState:ENABLE,DISABLE
//ReturnValue:NONE
/*************************************************************/  
void GTC_ControlSet_Configure (CSP_GTC_T *GTC , GTC_ControlSet_TypeDef GTC_ControlSet_x , FunctionalStatus NewState)
{
	if (NewState != DISABLE)
	{
		GTC->CSR |= GTC_ControlSet_x;
		if(GTC_ControlSet_x)
		{
			while(!(GTC->SR&GTC_ControlSet_x));
		}
	}
	else
	{
		GTC->CCR |= GTC_ControlSet_x;
		while(GTC->SR&GTC_ControlSet_x);
	}
}

/*************************************************************/
// gtc inturrpt Configure
//EntryParameter:*GTC,GTC_IMSCR_X,NewState
//*GTC:GTC0,GTC1,GTC2,GTC3
//GTC_IMSCR_X:GTC_STARTI,GTC_STOPI,GTC_PSTARTI,GTC_PENDI,GTC_MATI,GTC_OVFI,GTC_CA1semiI
//NewState:ENABLE,DISABLE
//ReturnValue:NONE
/*************************************************************/ 
void GTC_ConfigInterrupt_CMD(CSP_GTC_T *GTC , GTC_IMSCR_TypeDef GTC_IMSCR_X , FunctionalStatus NewState)
{
	GTC->ICR=0x7F;					//clear GTC2 INT status 
	if (NewState != DISABLE)
	{
		GTC->IMSCR  |= GTC_IMSCR_X;						//SET
	}
	else
	{
		GTC->IMSCR  &= ~GTC_IMSCR_X;					//CLR
	}
}

/*************************************************************/
// gtc software reset
//EntryParameter:*GTC
//*GTC:GTC0,GTC1,GTC2,GTC3
//ReturnValue:NONE
/*************************************************************/ 
void GTC_SoftwareReset(CSP_GTC_T *GTC )
{
	GTC->SRR = GTC_SWRST;							// Software reset
}
  
/*************************************************************/
// gtc start stop
//EntryParameter:*GTC
//*GTC:GTC0,GTC1,GTC2,GTC3
//ReturnValue:NONE
/*************************************************************/ 
void GTC_start_stop(CSP_GTC_T *GTC)
{
	GTC_ControlSet_Configure (GTC , GTC_ControlSet_start_stop , ENABLE); 
	//GTC->CSR = GTC_START;
}
/*************************************************************/
// gtc start
//EntryParameter:*GTC
//*GTC:GTC0,GTC1,GTC2,GTC3
//ReturnValue:NONE
/*************************************************************/ 
void GTC_Start(CSP_GTC_T *GTC)
{
	GTC_ControlSet_Configure (GTC , GTC_ControlSet_start_stop , ENABLE); 
}
/*************************************************************/
// gtc stop
//EntryParameter:*GTC
//*GTC:GTC0,GTC1,GTC2,GTC3
//ReturnValue:NONE
/*************************************************************/ 
void GTC_Stop(CSP_GTC_T *GTC)
{
	GTC_ControlSet_Configure (GTC , GTC_ControlSet_start_stop , DISABLE); 
}
/*************************************************************/
// gtc counter read
//EntryParameter:*GTC
//*GTC:GTC0,GTC1,GTC2,GTC3
//ReturnValue:Counter value
/*************************************************************/ 
U32_T GTC_Counter_Value(CSP_GTC_T *GTC)
{
	return (GTC->CVR);
}
/*************************************************************/
// gtc counter period DATA read 
//EntryParameter:*GTC,loadCounter_PRDR,loadCounter_PULR
//*GTC:GTC0,GTC1,GTC2,GTC3
//ReturnValue:Conter prdr register value
/*************************************************************/ 
void GTC_Set_Period(CSP_GTC_T *GTC , U32_T loadCounter_PRDR , U32_T loadCounter_PULR)
{
	GTC->PRDR = loadCounter_PRDR;											//Period of GTC date register
	GTC->PULR = loadCounter_PULR;											//Pulse of GTC date register
	GTC->CSR = (GTC->CSR & 0xFFFFFFFD) | 0x02;
}
/*************************************************************/
// gtc counter period DATA read 
//EntryParameter:*GTC
//*GTC:GTC0,GTC1,GTC2,GTC3
//ReturnValue:Conter prdr register value
/*************************************************************/ 
U32_T GTC_Counter_PRDR_Value(CSP_GTC_T *GTC)
{
	return (GTC->PRDR);
}

/*************************************************************/
//gtc counter period DATA read 
//EntryParameter:*GTC
//*GTC:GTC0,GTC1,GTC2,GTC3
//ReturnValue:CaptureUp Count Value
/*************************************************************/ 
U32_T GTC_CaptureUp_Count_Value(CSP_GTC_T *GTC)
{
	return (GTC->CUCR);
}

/*************************************************************/
//gtc counter period DATA read 
//EntryParameter:*GTC
//*GTC:GTC0,GTC1,GTC2,GTC3
//ReturnValue:CaptureDown Count Value
/*************************************************************/ 
U32_T GTC_CaptureDown_Count_Value(CSP_GTC_T *GTC)
{
	return (GTC->CDCR);
}

/*************************************************************/
//GTC0 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC0_Int_Enable(void)
{
    GTC0->ICR=0x7F;					//clear GTC0 INT status 
	INTC_ISER_WRITE(GTC0_INT);    
}

/*************************************************************/
//GTC0 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC0_Int_Disable(void)
{
    INTC_ICER_WRITE(GTC0_INT);    
}

/*************************************************************/
//GTC1 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC1_Int_Enable(void)
{
    GTC1->ICR=0x7F;					//clear GTC1 INT status 
	INTC_ISER_WRITE(GTC1_INT);    
}

/*************************************************************/
//GTC1 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC1_Int_Disable(void)
{
    INTC_ICER_WRITE(GTC1_INT);    
}

/*************************************************************/
//GTC2 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC2_Int_Enable(void)
{
    GTC2->ICR=0x7F;					//clear GTC2 INT status 
	INTC_ISER_WRITE(GTC2_INT);    
}

/*************************************************************/
//GTC2 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC2_Int_Disable(void)
{
    INTC_ICER_WRITE(GTC2_INT);    
}

/*************************************************************/
//GTC3 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC3_Int_Enable(void)
{
    GTC3->ICR=0x7F;					//clear GTC3 INT status 
	INTC_ISER_WRITE(GTC3_INT);    
}

/*************************************************************/
//GTC3 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC3_Int_Disable(void)
{
    INTC_ICER_WRITE(GTC3_INT);    
}
/*************************************************************/
//GTC0 Interrupt wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC0_WakeUp_Enable(void)
{
    INTC_IWER_WRITE(GTC0_INT);    
}

/*************************************************************/
//GTC0 Interrupt wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC0_WakeUp_Disable(void)
{
    INTC_IWDR_WRITE(GTC0_INT);    
}
/*************************************************************/
//GTC1 Interrupt wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC1_WakeUp_Enable(void)
{
    INTC_IWER_WRITE(GTC1_INT);    
}

/*************************************************************/
//GTC1 Interrupt wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC1_WakeUp_Disable(void)
{
    INTC_IWDR_WRITE(GTC1_INT);    
}
/*************************************************************/
//GTC2 Interrupt wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC2_WakeUp_Enable(void)
{
    INTC_IWER_WRITE(GTC2_INT);    
}

/*************************************************************/
//GTC0 Interrupt wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC2_WakeUp_Disable(void)
{
    INTC_IWDR_WRITE(GTC2_INT);    
}
/*************************************************************/
//GTC0 Interrupt wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC3_WakeUp_Enable(void)
{
    INTC_IWER_WRITE(GTC3_INT);    
}

/*************************************************************/
//GTC0 Interrupt wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC3_WakeUp_Disable(void)
{
    INTC_IWDR_WRITE(GTC3_INT);    
}
/*************************************************************/
//GTC0 IO Init
//EntryParameter:GTC_IO_MODE_X,GTC_IO_G
//GTC_IO_MODE_X:GTC_IO_TXOUT,GTC_IO_TCLKX,GTC_IO_TCAPX
//GTC_IO_G;GTC_IO_TXOUT(0->PB0.02 ;1->PB0.01;2->PA0.0;3->PA0.11),GTC_IO_TXCLK(0->PA0.06),GTC_IO_TCAPX(0->PA0.12)
//ReturnValue:NONE
/*************************************************************/
void GTC0_IO_Init(GTC_IO_MODE_TypeDef  GTC_IO_MODE_X , U8_T GTC_IO_G )
{
	if(GTC_IO_MODE_X==GTC_IO_TXOUT)
	{
		if(GTC_IO_G==0)
		{
			GPIOB0->CONLR=(GPIOB0->CONLR & 0XFFFFF0FF)|0x00000700;										//T0OUT(PB0.02->AF4)
		}
		else if(GTC_IO_G==1)
		{
			GPIOB0->CONLR=(GPIOB0->CONLR & 0XFFFFFF0F)|0x00000040;										//T0OUT(PB0.01->AF1)
		}
		else if(GTC_IO_G==2)
		{
			GPIOA0->CONLR=(GPIOA0->CONLR & 0XFFFFFFF0)|0x00000004;										//T0OUT(PA0.0->AF1)
		}
		else if(GTC_IO_G==3)
		{
			GPIOA0->CONHR=(GPIOA0->CONHR & 0XFFFF0FFF)|0x00004000;										//T0OUT(PA0.11->AF1)
		}
        else if(GTC_IO_G==4)
		{
			GPIOC0->CONLR=(GPIOC0->CONLR & 0XFFFF0FFF)|0x00005000;										//T0OUT(PC0.3->AF1)
		}
	}
	else if(GTC_IO_MODE_X==GTC_IO_TCLK2)
	{
		if(GTC_IO_G==0)
		{
			GPIOB0->CONLR=(GPIOB0->CONLR & 0XFFFF0FFF)|0x00005000;										//TCLK2(PB0.03->AF2)
		}
		else if(GTC_IO_G==1)
		{
			GPIOA0->CONHR=(GPIOA0->CONHR & 0XFFFFFF0F)|0x00000060;										//TCLK2(PA0.09->AF3)
		}
	}
	else if(GTC_IO_MODE_X==GTC_IO_TCLK0)
	{
		if(GTC_IO_G==0)
		{
			GPIOA0->CONLR=(GPIOA0->CONLR & 0XF0FFFFFF)|0x04000000;										//TCLK0(PA0.06->AF1)
		}
	}
	else if(GTC_IO_MODE_X==GTC_IO_TCAPX)
	{
		if(GTC_IO_G==0)
		{
			GPIOA0->CONHR=(GPIOA0->CONHR & 0XFFF0FFFF)|0x00060000;										//TCAP0(PA0.12->AF3)
		}	
		else if(GTC_IO_G==1)
		{
			GPIOA0->CONLR=(GPIOA0->CONLR & 0XFFFF0FFF)|0x00004000;										//TCAP0(PA0.03->AF1)
		}
	}
}

/*************************************************************/
//GTC1 IO Init
//EntryParameter:GTC_IO_MODE_X,GTC_IO_G
//GTC_IO_MODE_X:GTC_IO_TXOUT,GTC_IO_TCLKX,GTC_IO_TCAPX
//GTC_IO_G;GTC_IO_TXOUT(0->PA0.09 ;1->PA0.08;2->PB0.03;3->PA0.05),GTC_IO_TXCLK(0->PA0.05),GTC_IO_TCAPX(0->PA0.10;1->PA0.6)
//ReturnValue:NONE
/*************************************************************/
void GTC1_IO_Init(GTC_IO_MODE_TypeDef  GTC_IO_MODE_X , U8_T GTC_IO_G )
{
	if(GTC_IO_MODE_X==GTC_IO_TXOUT)
	{
		if(GTC_IO_G==0)
		{
			GPIOA0->CONHR=(GPIOA0->CONHR & 0XFFFFFF0F)|0x00000040;										//T1OUT(PA0.09->AF1)
		}
		else if(GTC_IO_G==1)
		{
			GPIOA0->CONHR=(GPIOA0->CONHR & 0XFFFFFFF0)|0x00000007;										//T1OUT(PA0.08->AF4)
		}
		else if(GTC_IO_G==2)
		{
			GPIOB0->CONLR=(GPIOB0->CONLR & 0XFFFF0FFF)|0x00004000;										//T1OUT(PB0.03->AF1)
		}
		else if(GTC_IO_G==3)
		{
			GPIOA0->CONLR=(GPIOA0->CONLR & 0XFF0FFFFF)|0x00800000;										//T1OUT(PA0.05->AF5)
		}
        else if(GTC_IO_G==4)
		{
			GPIOC0->CONLR=(GPIOC0->CONLR & 0XFFFFF0FF)|0x00000500;										//T1OUT(PC0.2->AF1)
		}
	}
	else if(GTC_IO_MODE_X==GTC_IO_TCLK2)
	{
		if(GTC_IO_G==0)
		{
			GPIOB0->CONLR=(GPIOB0->CONLR & 0XFFFF0FFF)|0x00005000;										//TCLK2(PB0.03->AF2)
		}
		else if(GTC_IO_G==1)
		{
			GPIOA0->CONHR=(GPIOA0->CONHR & 0XFFFFFF0F)|0x00000060;										//TCLK2(PA0.09->AF3)
		}
	}
	else if(GTC_IO_MODE_X==GTC_IO_TCLK0)
	{
		if(GTC_IO_G==0)
		{
			GPIOA0->CONLR=(GPIOA0->CONLR & 0XF0FFFFFF)|0x04000000;										//TCLK0(PA0.04->AF1)
		}
	}
	else if(GTC_IO_MODE_X==GTC_IO_TCAPX)
	{
		if(GTC_IO_G==0)
		{
			GPIOA0->CONHR=(GPIOA0->CONHR & 0XFFFFF0FF)|0x00000400;										//TCAP1(PA0.10->AF1)
		}	
		else if(GTC_IO_G==1)
		{
			GPIOA0->CONLR=(GPIOA0->CONLR & 0XF0FFFFFF)|0x06000000;										//TCAP1(PA0.06->AF3)
		}
	}
}

/*************************************************************/
//GTC2 IO Init
//EntryParameter:GTC_IO_MODE_X,GTC_IO_G
//GTC_IO_MODE_X:GTC_IO_TXOUT,GTC_IO_TCLKX,GTC_IO_TCAPX
//GTC_IO_G;GTC_IO_TXOUT(0->PB0.00 ;1->PA0.13;2->PA0.07;3->PA0.04),GTC_IO_TXCLK(0->PA0.04;1->PB0.2;2->PA0.9),GTC_IO_TCAPX(0->PA0.08;1->PB0.2)
//ReturnValue:NONE
/*************************************************************/
void GTC2_IO_Init(GTC_IO_MODE_TypeDef  GTC_IO_MODE_X , U8_T GTC_IO_G )
{
	if(GTC_IO_MODE_X==GTC_IO_TXOUT)
	{
		if(GTC_IO_G==0)
		{
			GPIOB0->CONLR=(GPIOB0->CONLR & 0XFFFFFFF0)|0x00000004;										//T2OUT(PB0.00->AF1)
		}
		else if(GTC_IO_G==1)
		{
			GPIOA0->CONHR=(GPIOA0->CONHR & 0XFF0FFFFF)|0x00600000;										//T2OUT(PA0.13->AF3)
		}
		else if(GTC_IO_G==2)
		{
			GPIOA0->CONLR=(GPIOA0->CONLR & 0X0FFFFFFF)|0x70000000;										//T2OUT(PA0.07->AF4)
		}
		else if(GTC_IO_G==3)
		{
			GPIOA0->CONLR=(GPIOA0->CONLR & 0XFFF0FFFF)|0x00080000;										//T2OUT(PA0.04->AF5)
		}
        else if(GTC_IO_G==4)
		{
			GPIOC0->CONLR=(GPIOC0->CONLR & 0XFFFFFF0F)|0x00000050;										//T2OUT(PC0.1->AF1)
		}
	}
	else if(GTC_IO_MODE_X==GTC_IO_TCLK2)
	{
		if(GTC_IO_G==0)
		{
			GPIOB0->CONLR=(GPIOB0->CONLR & 0XFFFF0FFF)|0x00005000;										//TCLK2(PB0.03->AF2)
		}
		else if(GTC_IO_G==1)
		{
			GPIOA0->CONHR=(GPIOA0->CONHR & 0XFFFFFF0F)|0x00000060;										//TCLK2(PA0.09->AF3)
		}
	}
	else if(GTC_IO_MODE_X==GTC_IO_TCLK0)
	{
		if(GTC_IO_G==0)
		{
			GPIOA0->CONLR=(GPIOA0->CONLR & 0XF0FFFFFF)|0x04000000;										//TCLK0(PA0.04->AF1)
		}
	}
	else if(GTC_IO_MODE_X==GTC_IO_TCAPX)
	{
		if(GTC_IO_G==0)
		{
			GPIOA0->CONHR=(GPIOA0->CONHR & 0XFFFFFFF0)|0x00000004;										//TCAP2(PA0.08->AF1)
		}	
		else if(GTC_IO_G==1)
		{
			GPIOB0->CONLR=(GPIOB0->CONLR & 0XFFFFF0FF)|0x0B000500;										//TCAP2(PB0.02->AF2)
		}
	}
}

/*************************************************************/
//GTC3 IO Init
//EntryParameter:GTC_IO_MODE_X,GTC_IO_G
//GTC_IO_MODE_X:GTC_IO_TXOUT,GTC_IO_TCLKX,GTC_IO_TCAPX
//GTC_IO_G;GTC_IO_TXOUT(0->PB0.02 ;1->PA0.03;2->PB0.03)GTC_IO_TCAPX(0->PB0.00)
//ReturnValue:NONE
/*************************************************************/
void GTC3_IO_Init(GTC_IO_MODE_TypeDef  GTC_IO_MODE_X , U8_T GTC_IO_G )
{
	if(GTC_IO_MODE_X==GTC_IO_TXOUT)
	{
		if(GTC_IO_G==0)
		{
			GPIOB0->CONLR=(GPIOB0->CONLR & 0XFFFFF0FF)|0x00000400;										//T3OUT(PB0.02->AF1)
		}
		else if(GTC_IO_G==1)
		{
			GPIOA0->CONLR=(GPIOA0->CONLR & 0XFFFF0FFF)|0x00006000;										//T3OUT(PA0.03->AF3)
		}
		else if(GTC_IO_G==2)
		{
			GPIOB0->CONLR=(GPIOB0->CONLR & 0XFFFF0FFF)|0x00007000;										//T3OUT(PB0.03->AF4)
		}
        else if(GTC_IO_G==4)
		{
			GPIOC0->CONLR=(GPIOC0->CONLR & 0XFFFFFFF0)|0x00000005;										//T3OUT(PC0.0->AF1)
		}
	}
	else if(GTC_IO_MODE_X==GTC_IO_TCLK2)
	{
		if(GTC_IO_G==0)
		{
			GPIOB0->CONLR=(GPIOB0->CONLR & 0XFFFF0FFF)|0x00005000;										//TCLK2(PB0.03->AF2)
		}
		else if(GTC_IO_G==1)
		{
			GPIOA0->CONHR=(GPIOA0->CONHR & 0XFFFFFF0F)|0x00000060;										//TCLK2(PA0.09->AF3)
		}
	}
	else if(GTC_IO_MODE_X==GTC_IO_TCLK0)
	{
		if(GTC_IO_G==0)
		{
			GPIOA0->CONLR=(GPIOA0->CONLR & 0XF0FFFFFF)|0x04000000;										//TCLK0(PA0.04->AF1)
		}
	}
	else if(GTC_IO_MODE_X==GTC_IO_TCAPX)
	{
		if(GTC_IO_G==0)
		{
			GPIOB0->CONLR=(GPIOB0->CONLR & 0XFFFFFFF0)|0x00000006;										//TCAP3(PB0.00->AF3)
		}	
	}
}


/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/