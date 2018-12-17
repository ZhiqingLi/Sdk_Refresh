/*
  ******************************************************************************
  * @file    asm32s003_syscon.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "asm32s003_syscon.h"
#include "asm32s003_gpio.h"
#include "asm32s003_uart.h"

/* define --------------------------------------------------------------------*/
volatile U8_T SYSCON_ISOSCSTFlag=0;
volatile U8_T SYSCON_IMOSCSTFlag=0;
volatile U8_T SYSCON_EMOSCSTFlag=0;
volatile U8_T SYSCON_SYSCLKSTFlag=0;
volatile U8_T SYSCON_IWDTINTFlag=0;
volatile U8_T SYSCON_LVDINTFlag=0;
volatile U8_T SYSCON_EM_CMFAILFlag=0;
volatile U8_T SYSCON_EM_CMRCVFlag=0;
volatile U8_T SYSCON_CMD_ERRFlag=0;						//SYSCON interrupt flag
    
/* externs--------------------------------------------------------------------*/


/*************************************************************/
//Deinitializes the syscon registers to their default reset
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/    
void SYSCON_RST_VALUE(void)									//reset value
{
	SYSCON->IDCCR=SYSCON_IDCCR_RST;
	SYSCON->GCER=SYSCON_GCER_RST;
	SYSCON->GCDR=SYSCON_GCDR_RST;
	SYSCON->SCLKCR=SYSCON_SCLKCR_RST;
	SYSCON->PCLKCR=SYSCON_PCLKCR_RST;
	SYSCON->PCER0=SYSCON_PCER0_RST;
	SYSCON->PCDR0=SYSCON_PCDR0_RST;
	SYSCON->PCSR0=SYSCON_PCSR0_RST;
	SYSCON->PCER1=SYSCON_PCER1_RST;
	SYSCON->PCDR1=SYSCON_PCDR1_RST;
	SYSCON->PCSR1=SYSCON_PCSR1_RST;
	SYSCON->OSTR=SYSCON_OSTR_RST;
	SYSCON->LVDCR=SYSCON_LVDCR_RST;
	//SYSCON->IMFT=SYSCON_IMFT_RST;
	//SYSCON->PWRCR=SYSCON_PWRCR_RST;
	//SYSCON->OPT1=SYSCON_OPT1_RST;
	//SYSCON->OPT2=SYSCON_OPT2_RST;
	SYSCON->IECR=SYSCON_IECR_RST;
	SYSCON->IDCR=SYSCON_IDCR_RST;
	SYSCON->IMSR=SYSCON_IMSR_RST;
	SYSCON->IAR=SYSCON_IAR_RST;
	SYSCON->ICR=SYSCON_ICR_RST;
	SYSCON->RISR=SYSCON_RISR_RST;	
	SYSCON->ISR=SYSCON_ISR_RST;
	SYSCON->RSR=SYSCON_RSR_RST;
	SYSCON->EXIRT=SYSCON_EXIRT_RST;
	SYSCON->EXIFT=SYSCON_EXIFT_RST;
	SYSCON->EXIER=SYSCON_EXIER_RST;
	SYSCON->EXIDR=SYSCON_EXIDR_RST;
	SYSCON->EXIMR=SYSCON_EXIMR_RST;
	SYSCON->EXIAR=SYSCON_EXIAR_RST;
	SYSCON->EXICR=SYSCON_EXICR_RST;
	SYSCON->EXIRS=SYSCON_EXIRS_RST;
	SYSCON->IWDCR=SYSCON_IWDCR_RST;
	SYSCON->IWDCNT=SYSCON_IWDCNT_RST;
	//SYSCON->IWDEDR=SYSCON_IWDEDR_RST;
	//SYSCON->CINF0=SYSCON_CINF0_RST;
	//SYSCON->CINF1=SYSCON_CINF1_RST;
	//SYSCON->FINF0=SYSCON_FINF1_RST;
	SYSCON->ERRINF=SYSCON_ERRINF_RST;
}

/*************************************************************/
//SYSCON General Control
//EntryParameter:NewState:,ENDIS_X
//NewState:ENABLE,DISABLE
//ENDIS_X:ENDIS_ISOSC,ENDIS_IMOSC,ENDIS_EMOSC,ENDIS_IDLE_PCLK,ENDIS_SYSTICK,ENDIS_EM_CM,ENDIS_EM_CMRST
//ReturnValue:NONE
/*************************************************************/  
void SYSCON_General_CMD(FunctionalStatus NewState, SYSCON_General_CMD_TypeDef ENDIS_X )
{
	if (NewState != DISABLE)
	{
		if(ENDIS_X==ENDIS_EMOSC) 
		GPIOA0->CONLR=(GPIOA0->CONLR & 0XFFFFF00f)|0x00000660;					//浣胯兘瀵瑰簲PIN
		SYSCON->GCER|=ENDIS_X;													//enable SYSCON General Control
		while(!(SYSCON->GCSR&ENDIS_X));											//check  Enable?	
		
		switch(ENDIS_X)
		{
			case ENDIS_IMOSC:
				while(!SYSCON_IMOSCSTFlag); SYSCON_IMOSCSTFlag=0;				// Wait IMOSC stable interrupt
				break;
			case ENDIS_EMOSC:
				while(!SYSCON_EMOSCSTFlag); SYSCON_EMOSCSTFlag=0;				// Wait EMOSC stable interrupt	
				break;	
			case ENDIS_ISOSC:
				while(!SYSCON_ISOSCSTFlag); SYSCON_ISOSCSTFlag=0;				// Wait ISOS stable interrupt	
				break;
			case ENDIS_IDLE_PCLK:
				break;
			case ENDIS_SYSTICK:
				break;
			case ENDIS_EM_CM:
				break;
			case ENDIS_EM_CMRST:
				break;
		}
	}
	else
	{
		SYSCON->GCDR|=ENDIS_X;													//disable SYSCON General Control
		while(SYSCON->GCSR&ENDIS_X);											//check  Disable?
		SYSCON->ICR|=ENDIS_X;													// Clear ENDIS_X stable bit
	}
}
/*************************************************************/
//Seleted system clk and seleted clk div
//EntryParameter:SYSCLK_X,HCLK_DIV_X,PCLK_DIV_X
//SYSCLK_X:SYSCLK_IMOSC,SYSCLK_EMOSC,SYSCLK_ISOSC
//HCLK_DIV_X:HCLK_DIV_1/2/3/4/5/6/7/8/12/16/24/32/64/128/256
//PCLK_DIV_X:PCLK_DIV_1,PCLK_DIV_2,PCLK_DIV_4,PCLK_DIV_8,PCLK_DIV_16
//ReturnValue:NONE
/*************************************************************/ 
void SystemCLK_HCLKDIV_PCLKDIV_Config(SystemCLK_TypeDef SYSCLK_X , SystemCLK_Div_TypeDef HCLK_DIV_X , PCLK_Div_TypeDef PCLK_DIV_X)
{
	SYSCON->SCLKCR=SYSCLK_KEY | HCLK_DIV_X| SYSCLK_X;
	while(!SYSCON_SYSCLKSTFlag);												//Wait clock switch stable interrupt
	SYSCON_SYSCLKSTFlag=0;
	SYSCON->PCLKCR=PCLK_KEY|PCLK_DIV_X;											//PCLK DIV 1 2 4 6 8 16		
	while(SYSCON->PCLKCR!=PCLK_DIV_X);											//Wait PCLK DIV
	if(SYSCLK_X==SYSCLK_EMOSC) GPIOA0->CONLR=(GPIOA0->CONLR & 0XFFFFF00f)|0x00000660;					//浣胯兘瀵瑰簲PIN
}
/*************************************************************/
//SYSON Peripheral clock enable and disable 
//EntryParameter:X_PCLK0,NewState
//X_PCLK0:IFC_CLK,ADC_CLK,ICSI_CLK,TKEY_CLK,UART0_CLK,I2C_CLK
//NewState:ENABLE,DISABLE
//ReturnValue:NONE
/*************************************************************/
void SYSCON_PCLK0_CMD( SYSCON_PCLK0_CMD_TypeDef X_PCLK0 , FunctionalStatus NewState )
{
	if (NewState != DISABLE)
	{
		SYSCON->PCER0|=X_PCLK0;													//enable SYSCON Peripheral clock enable
		while(!(SYSCON->PCSR0&X_PCLK0));										//check  Enable?	
	}
	else
	{
		SYSCON->PCDR0|=X_PCLK0;													//disable SYSCON General Control
		while(SYSCON->PCSR0&X_PCLK0);
	}
}

/*************************************************************/
//SYSON Peripheral clock enable and disable 
//EntryParameter:X_PCLK1,NewState
//X_PCLK1:GTC0_CLK,GTC1_CLK,GTC2_CLK,GTC3_CLK,CFG_CLK,LED_CLK,PIOA_CLK,PIOB_CLK
//NewState:ENABLE,DISABLE
//ReturnValue:NONE
/*************************************************************/
void SYSCON_PCLK1_CMD(SYSCON_PCLK1_CMD_TypeDef X_PCLK1 , FunctionalStatus NewState)
{
	if (NewState != DISABLE)
	{
		SYSCON->PCER1=X_PCLK1;													//enable SYSCON Peripheral clock enable
		while(!(SYSCON->PCSR1&X_PCLK1));										//check  Enable?	
	}
	else
	{
		SYSCON->PCDR1=X_PCLK1;													//disable SYSCON General Control
		while(SYSCON->PCSR1&X_PCLK1);
	}
}

/*************************************************************/
//WDT enable and disable 
//EntryParameter:,NewState
//NewState:ENABLE,DISABLE
//ReturnValue:NONE
/*************************************************************/
void SYSCON_WDT_CMD(FunctionalStatus NewState)
{
	if(NewState != DISABLE)
	{
		SYSCON->IWDEDR=IWDTEDR_KEY|Enable_IWDT;
		while(!(SYSCON->IWDCR&Check_IWDT_BUSY));
	}
	else
	{
		SYSCON->IWDEDR=IWDTEDR_KEY|Disable_IWDT;
		while(SYSCON->IWDCR&Check_IWDT_BUSY);
	}
}

/*************************************************************/
//Read WDT_CNT
//EntryParameter:NONE
//ReturnValue:IWDCNT
/*************************************************************/
U32_T SYSCON_GET_IWDCNT(void )
{
	return (SYSCON->IWDCNT&0x3FFFFF);
}

/*************************************************************/
//Read IWDT_CLR statue
//EntryParameter:NONE
//ReturnValue: WDT clr flag
/*************************************************************/
B_T SYSCON_GET_IWDT_CLRBSY(void)					
{
	if(SYSCON->IWDCNT&0x80000000)
	{
		return 1;
	}
	else return 0;
}
  
/*************************************************************/
//reload WDT CN
//EntryParameter:NONE
//ReturnValue: NONE
/*************************************************************/
void SYSCON_IWDCNT_Reload(void)
{
	SYSCON->IWDCNT=CLR_IWDT;
}

/*************************************************************/
//IWDCNT Config
//EntryParameter:NewStateE_IWDT_SHORT,IWDT_TIME_X,IWDT_INTW_DIV_X
//NewStateE_IWDT_SHORT:ENABLE_IWDT_SHORT,DISABLE_IWDT_SHORT
//IWDT_TIME_X:IWDT_TIME_128MS,IWDT_TIME_256MS,IWDT_TIME_500MS,IWDT_TIME_1S,IWDT_TIME_2S,IWDT_TIME_3S,IWDT_TIME_4S,IWDT_TIME_8S
//IWDT_INTW_DIV_X:IWDT_INTW_DIV_7/6/5/4/3/2/1
//ReturnValue: NONE
/*************************************************************/
void SYSCON_IWDCNT_Config(IWDT_SHORT_TypeDef NewStateE_IWDT_SHORT , IWDT_TIME_TypeDef IWDT_TIME_X , IWDT_TIMEDIV_TypeDef IWDT_INTW_DIV_X )
{
	SYSCON->IWDCR=IWDT_KEY|NewStateE_IWDT_SHORT|IWDT_TIME_X|IWDT_INTW_DIV_X;
}

/*************************************************************/
//LVD Config  and set LVD INT
//EntryParameter:X_LVDEN,INTDET_LVL_X,RSTDET_LVL_X,X_LVD_INT
//X_LVDEN:ENABLE_LVDEN,DISABLE_LVDEN
//INTDET_LVL_X:INTDET_LVL_2_5V,INTDET_LVL_3_0V,INTDET_LVL_3_3V,INTDET_LVL_4_1V
//RSTDET_LVL_X:RSTDET_LVL_2_2V,RSTDET_LVL_2_7V
//X_LVD_INT:ENABLE_LVD_INT,DISABLE_LVD_INT
//ReturnValue: NONE
/*************************************************************/
void SYSCON_LVD_Config(X_LVDEN_TypeDef X_LVDEN , INTDET_LVL_X_TypeDef INTDET_LVL_X , RSTDET_LVL_X_TypeDef RSTDET_LVL_X , X_LVD_INT_TypeDef X_LVD_INT)
{
	SYSCON->LVDCR=LVD_KEY|X_LVDEN|INTDET_LVL_X|RSTDET_LVL_X|X_LVD_INT;
}

/*************************************************************/
//LVD detection event status.
//EntryParameter:NONE
//ReturnValue: LVD detection flag
/*************************************************************/
U8_T SYSCON_LVDFLAG(void)
{
	if(SYSCON->LVDCR&LVDFLAG)
	{
		return 1;							//1: VDD is lower than LVD threshold selected with INTDET_LVL bits
	}
	else return 0;							//0: VDD is higher than LVD threshold selected with INTDET_LVL bits.   
}	
/*************************************************************/
//POR Reset event status.
//EntryParameter:NONE
//ReturnValue:POR detection flag
/*************************************************************/
U8_T SYSCON_POR_STATUS(void)
{
	if(SYSCON->RSR&PORST)
	{
		return 1;							//1: POR RESET
	}
	else return 0;							//0: NOT POR RESET   
}
/*************************************************************/
//LVR Reset event status.
//EntryParameter:NONE
//ReturnValue:POR detection flag
/*************************************************************/
U8_T SYSCON_LVR_STATUS(void)
{
	if(SYSCON->RSR&LVRRST)
	{
		return 1;							//1: LVR RESET
	}
	else return 0;							//0: NOT LVR RESET   
}	
/*************************************************************/
//EXT Reset event status.
//EntryParameter:NONE
//ReturnValue:POR detection flag
/*************************************************************/
U8_T SYSCON_EXTRST_STATUS(void)
{
	if(SYSCON->RSR&EXTRST)
	{
		return 1;							//1: EXT RESET
	}
	else return 0;							//0: NOT EXT RESET
}
/*************************************************************/
//IWDRST Reset event status.
//EntryParameter:NONE
//ReturnValue:POR detection flag
/*************************************************************/
U8_T SYSCON_IWDRST_STATUS(void)
{
	if(SYSCON->RSR&IWDRST)
	{
		return 1;							//1: IWDT RESET 
	}
	else return 0;							//0: NOT IWDT RESET   
}	
/*************************************************************/
//EMCMRST Reset event status.
//EntryParameter:NONE
//ReturnValue:POR detection flag
/*************************************************************/
U8_T SYSCON_EMCMRST_STATUS(void)
{
	if(SYSCON->RSR&EMCMRST)
	{
		return 1;							//1: EXT CLOCK RESET
	}
	else return 0;							//0: NOT EXT CLOCK RESET  
}	
/*************************************************************/
//CPURSTREQ Reset event status.
//EntryParameter:NONE
//ReturnValue:POR detection flag
/*************************************************************/
U8_T SYSCON_CPURSTREQ_STATUS(void)
{
	if(SYSCON->RSR&CPURSTREQ)
	{
		return 1;							//1: CPU REQUEST RESET
	}
	else return 0;							//0: NOT CPU REQUEST RESET  
}	
/*************************************************************/
//SWRST_RSR Reset event status.
//EntryParameter:NONE
//ReturnValue:POR detection flag
/*************************************************************/
U8_T SYSCON_SWRST_RSR_STATUS(void)
{
	if(SYSCON->RSR&SWRST_RSR)
	{
		return 1;							//1: SOFTWARE RESET
	}
	else return 0;							//0: NOT SOFTWARE RESET  
}	
/*************************************************************/
//LVD INT ENABLE.
//EntryParameter:NONE
//ReturnValue: NONE
/*************************************************************/
void LVD_Int_Enable(void)
{
	SYSCON->ICR = LVD_INT_ST;				//clear LVD INT status
	SYSCON->IECR  |= LVD_INT_ST;
}
/*************************************************************/
//LVD INT DISABLE.
//EntryParameter:NONE
//ReturnValue: NONE
/*************************************************************/
void LVD_Int_Disable(void)
{
	SYSCON->IDCR  |= LVD_INT_ST;
}
/*************************************************************/
//WDT INT ENABLE.
//EntryParameter:NONE
//ReturnValue: NONE
/*************************************************************/
void IWDT_Int_Enable(void)
{
	SYSCON->ICR = IWDT_INT_ST;				//clear LVD INT status
	SYSCON->IECR  |= IWDT_INT_ST;
}
/*************************************************************/
//WDT INT DISABLE.
//EntryParameter:NONE
//ReturnValue: NONE
/*************************************************************/
void IWDT_Int_Disable(void)
{
	SYSCON->IDCR  |= IWDT_INT_ST;
}
/*************************************************************/
//Reset status.
//EntryParameter:NONE
//ReturnValue: rsr_dat
//rsr_dat=0x01 power on reset
//rsr_dat=0x02 low voltage reset
//rsr_dat=0x04 ex-pin reset
//rsr_dat=0x10 wdt reset
//rsr_dat=0x40 ex clock invalid reset
//rsr_dat=0x80 cpu request reset
//rsr_dat=0x100 software reset
/*************************************************************/
U32_T Read_Reset_Status(void)
{
	return (SYSCON->RSR & 0x1ff);
}
/*************************************************************/
//external trigger  Mode Selection Functions
//EntryParameter:NewState,EXIPIN,EXI_tringer_mode
//NewState:ENABLE,DISABLE
//EXIPIN:EXI_PIN0/1/2/3/4/5/6/7/8/9/10/11/12/13
//EXI_tringer_mode:_EXIRT,_EXIFT
//ReturnValue: LVD detection flag
/*************************************************************/
void EXTI_trigger_CMD(FunctionalStatus NewState , SYSCON_EXIPIN_TypeDef  EXIPIN , EXI_tringer_mode_TypeDef EXI_tringer_mode)
{
	switch(EXI_tringer_mode)
	{
		case _EXIRT:
			if(NewState != DISABLE)
			{
				SYSCON->EXIRT |=EXIPIN;
			}
			else
			{
				SYSCON->EXIRT &=~EXIPIN;	
			}
			break;
		case _EXIFT:
			if(NewState != DISABLE)
			{
				SYSCON->EXIFT |=EXIPIN;
			}
			else
			{
				SYSCON->EXIFT &=~EXIPIN;	
			}
			break;
	}	
}

/*************************************************************/
//external interrupt enable and disable 
//EntryParameter:NewState,EXIPIN,* GPIOX
//* GPIOX:GPIOA,GPIOB
//EXIPIN:EXI_PIN0/1/2/3/4/5/6/7/8/9/10/11/12/13
//NewState:ENABLE,DISABLE
//ReturnValue:NONE
/*************************************************************/
void EXTI_interrupt_CMD(FunctionalStatus NewState , SYSCON_EXIPIN_TypeDef  EXIPIN)
{
	SYSCON->EXICR = 0X3FFF;									//Claer EXI INT status
	if(NewState != DISABLE)
	{
		SYSCON->EXIER|=EXIPIN;								//EXI4 interrupt enable
		while(!(SYSCON->EXIMR&EXIPIN));						//Check EXI is enabled or not
		SYSCON->EXICR |=EXIPIN;								// Clear EXI status bit
	}
	else
	{
		SYSCON->EXIDR|=EXIPIN;
	}
}
/*************************************************************/
//GPIO EXTI interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GPIO_EXTI_interrupt(CSP_GPIO_T * GPIOX,U32_T GPIO_IECR_VALUE)
{
	GPIOX->IECR = GPIOX->IECR|GPIO_IECR_VALUE;
}
/*************************************************************/
//PLCK goto SLEEP mode
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void PCLK_goto_idle_mode(void)
{
	SYSCON->GCER = ENDIS_IDLE_PCLK;
	asm ("doze");											// Enter sleep mode
}  
/*************************************************************/
//PLCK goto SLEEP mode
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void PCLK_goto_deepsleep_mode(void)
{
	SYSCON->GCER = ENDIS_IDLE_PCLK;
	asm ("stop");											// Enter sleep mode
}  
/*************************************************************/
//EXI0 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI0_Int_Enable(void)
{
	INTC_ISER_WRITE(EXI0_INT);    
}

/*************************************************************/
//EXI0 Interrupt disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI0_Int_Disable(void)
{
    INTC_ICER_WRITE(EXI0_INT);    
}

/*************************************************************/
//EXI1 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI1_Int_Enable(void)
{
	INTC_ISER_WRITE(EXI1_INT);    
}

/*************************************************************/
//EXI1 Interrupt disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI1_Int_Disable(void)
{
    INTC_ICER_WRITE(EXI1_INT);    
}

/*************************************************************/
//EXI2 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI2_Int_Enable(void)
{
	INTC_ISER_WRITE(EXI2_INT);    
}

/*************************************************************/
//EXI2 Interrupt disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI2_Int_Disable(void)
{
    INTC_ICER_WRITE(EXI2_INT);    
}

/*************************************************************/
//EXI3 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI3_Int_Enable(void)
{
	INTC_ISER_WRITE(EXI3_INT);    
}

/*************************************************************/
//EXI3 Interrupt disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI3_Int_Disable(void)
{
    INTC_ICER_WRITE(EXI3_INT);    
}

/*************************************************************/
//EXI4 Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI4_Int_Enable(void)
{
	INTC_ISER_WRITE(EXI4_INT);    
}

/*************************************************************/
//EXI4 Interrupt disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI4_Int_Disable(void)
{
    INTC_ICER_WRITE(EXI4_INT);    
}
/*************************************************************/
//EXI0 Wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI0_WakeUp_Enable(void)
{
    INTC_IWER_WRITE(EXI0_INT);    
}

/*************************************************************/
//EXI0 Wake up disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI0_WakeUp_Disable(void)
{
    INTC_IWDR_WRITE(EXI0_INT);    
}

/*************************************************************/
//EXI1 Wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI1_WakeUp_Enable(void)
{
    INTC_IWER_WRITE(EXI1_INT);    
}

/*************************************************************/
//EXI1 Wake up disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI1_WakeUp_Disable(void)
{
    INTC_IWDR_WRITE(EXI1_INT);    
}

/*************************************************************/
//EXI2 Wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI2_WakeUp_Enable(void)
{
    INTC_IWER_WRITE(EXI2_INT);    
}

/*************************************************************/
//EXI2 Wake up disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI2_WakeUp_Disable(void)
{
    INTC_IWDR_WRITE(EXI2_INT);    
}

/*************************************************************/
//EXI3 Wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI3_WakeUp_Enable(void)
{
    INTC_IWER_WRITE(EXI3_INT);    
}

/*************************************************************/
//EXI3 Wake up disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI3_WakeUp_Disable(void)
{
    INTC_IWDR_WRITE(EXI3_INT);    
}

/*************************************************************/
//EXI4 Wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI4_WakeUp_Enable(void)
{
    INTC_IWER_WRITE(EXI4_INT);    
}

/*************************************************************/
//EXI4 Wake up disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI4_WakeUp_Disable(void)
{
    INTC_IWDR_WRITE(EXI4_INT);    
}
/*************************************************************/
//SYSCON Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void SYSCON_Int_Enable(void)
{
    INTC_ISER_WRITE(SYSCON_INT);    
}

/*************************************************************/
//SYSCON Interrupt disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void SYSCON_Int_Disable(void)
{
    INTC_ICER_WRITE(SYSCON_INT);    
}
/*************************************************************/
//SYSCON Wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void SYSCON_WakeUp_Enable(void)
{
    INTC_IWER_WRITE(SYSCON_INT);    
}

/*************************************************************/
//SYSCON Wake up disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void SYSCON_WakeUp_Disable(void)
{
    INTC_IWDR_WRITE(SYSCON_INT);    
}
/*************************************************************/
//SYSCON power control config
//EntryParameter:PWRCR_dat
//PWRCR_dat=0xA66A000x; default=0xA66A0009
//ReturnValue:NONE
/*************************************************************/
void SYSCON_PWRCR_CONFIG(Power_Drive_TypeDef PWRCR_dat)
{
	SYSCON->PWRCR=PWRCR_dat;
}
/*************************************************************/
//set PA0.0/PA0.8 as CLO output 
//EntryParameter:CLO_PA00/CLO_PA08
//ReturnValue:NONE
/*************************************************************/
void SYSCON_CLO_CONFIG(CLO_IO_TypeDef clo_io)
{
	if (clo_io==CLO_PA00)
	{
		GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFFFFF0) | 0X00000007;
	}
	if (clo_io==CLO_PA08)
	{
		GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFFFF0) | 0X00000006;
	}
	
}
/*************************************************************/
//SYSCON CLO output adjust
//EntryParameter:PWRCR_dat
//0x000X0YYY;X=0~C YYY=0X1FF~0X000
//ReturnValue:NONE
/*************************************************************/
void SYSCON_IMFT_CONFIG(U32_T IMFT_dat)
{
	SYSCON->IMFT=IMFT_dat;
}
/*************************************************************/
//READ CIF0 data
//EntryParameter:None
//ReturnValue:VALUE
/*************************************************************/
U32_T SYSCON_Read_CINF0(void)
{
    U32_T value = 0;
    value=SYSCON->CINF0;
    return value;
}
/*************************************************************/
//READ CIF1 data
//EntryParameter:None
//ReturnValue:VALUE
/*************************************************************/
U32_T SYSCON_Read_CINF1(void)
{
    U32_T value = 0;
    value=SYSCON->CINF1;
    return value;
}
/*****************************************************************************
 函 数 名  : SYSCON_Soft_Reset
 功能描述  : 执行软件复位
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年9月13日
    作    者   : 李治清
    修改内容   : 新生成函数

*****************************************************************************/
void SYSCON_Soft_Reset(void)
{
    SYSCON->IDCCR = IDCCR_KEY|SWRST;
}

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/
