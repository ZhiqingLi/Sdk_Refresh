/*
  ******************************************************************************
  * @file    asm32s003_adc.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/

  
/* Includes ------------------------------------------------------------------*/
#include "asm32s003_adc.h"
/* defines -------------------------------------------------------------------*/
volatile U8_T ADC12_EOCFlag;
volatile U8_T ADC12_READFlag;
volatile U8_T ADC12_OVRFlag;
volatile U8_T ADC12_CMP0HFlag;
volatile U8_T ADC12_CMP0LFlag;
volatile U8_T ADC12_CMP1HFlag;
volatile U8_T ADC12_CMP1LFlag;
U32_T   adc_dr;
U8_T AD_mode;
/* externs--------------------------------------------------------------------*/


/*************************************************************/
//ADC12 ESET VALUE 
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/  
void ADC12_RESET_VALUE(void)
{
	 ADC->ECR	=	ADC_ECR_RST;     				/**< ECR  reset value  */
	 ADC->DCR	=	ADC_DCR_RST;                  	/**< DCR  reset value  */
	 ADC->PMSR	= 	ADC_PMSR_RST;       		  	/**< PMSR reset value  */
	 ADC->CR	=	ADC_CR_RST;             		/**< CR   reset value  */
	 ADC->MR	=   ADC_MR_RST;                  	/**< MR   reset value  */
	 ADC->SR 	=	ADC_SR_RST;                   	/**< SR   reset value  */
	 ADC->IER 	=	ADC_IER_RST;                	/**< IER  reset value  */
	 ADC->IDR 	=	ADC_IDR_RST;              		/**< IDR  reset value  */
	 ADC->IMR  	=	ADC_IMR_RST;               		/**< IMR  reset value  */
	 ADC->CMR[0] 	=	ADC_CMR0_RST;             	/**< CMR0  reset value */
	 ADC->CMR[1] 	=	ADC_CMR1_RST;             	/**< CMR1  reset value */
	 ADC->CMR[2] 	=	ADC_CMR2_RST;             	/**< CMR2  reset value */
	 ADC->DR 	=	ADC_DR_RST;                		/**< DR   reset value  */
	 ADC->CMP0 	=	ADC_CMP1_RST;               	/**< CMP1 reset value  */
	 ADC->CMP1 	=	ADC_CMP2_RST;                	/**< CMP2 reset value  */
}  
  
/*************************************************************/
//ADC12 MASK VALUE
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/  
void ADC12_MASK_VALUE(void)
{
	 ADC->ECR	=	ADC_ECR_MASK;     				/**< ECR  MASK value  */
	 ADC->DCR	=	ADC_DCR_MASK;                  	/**< DCR  MASK value  */
	 ADC->PMSR	= 	ADC_PMSR_MASK;       		  	/**< PMSR MASK value  */
	 ADC->CR	=	ADC_CR_MASK;              		/**< CR   MASK value  */
	 ADC->MR	=   ADC_MR_MASK;                  	/**< MR   MASK value  */
	 ADC->SR 	=	ADC_SR_MASK;                    /**< SR   MASK value  */
	 ADC->IER 	=	ADC_IER_MASK;                	/**< IER  MASK value  */
	 ADC->IDR 	=	ADC_IDR_MASK;              		/**< IDR  MASK value  */
	 ADC->IMR  	=	ADC_IMR_MASK;               	/**< IMR  MASK value  */
	 ADC->CMR[0] 	=	ADC_CMR0_MASK;             	/**< CMR0  MASK value */
	 ADC->CMR[1] 	=	ADC_CMR1_MASK;             	/**< CMR1  MASK value */
	 ADC->CMR[2] 	=	ADC_CMR2_MASK;             	/**< CMR2  MASK value */
	 ADC->DR 	=	ADC_DR_MASK;               		/**< DR   MASK value  */
	 ADC->CMP0 	=	ADC_CMP1_MASK;               	/**< CMP1 MASK value  */
	 ADC->CMP1 	=	ADC_CMP2_MASK;                	/**< CMP2 MASK value  */
}  

/*************************************************************/
//ADC12 read IPIDCODE
//EntryParameter:NONE
//ReturnValue:ADC IPIDCODE
/*************************************************************/  
U32_T ADC12_read_IPIDCODE(void)
{
	return (ADC->PMSR&ADC12_IPIDCODE_MASK);
}  

/*************************************************************/
//ADC12 Control
//EntryParameter:ADC12_Control_x 
//ADC12_Control_x:ADC12_SWRST , ADC12_ADCEN , ADC12_ADCDIS, ADC12_START, ADC12_STOP
//ReturnValue:NONE
/*************************************************************/  
  //control:ADC enable/disable ,start/stop,swrst
void ADC12_Control(ADC12_Control_TypeDef ADC12_Control_x )
{
	ADC->CR |= ADC12_Control_x;							// 
}

/*************************************************************/
//DC12 Interrupt ENABLE AND DISABLE
//EntryParameter:ADC_IMR_X,NewState
//ADC_IMR_X:ADC12_EOC,ADC12_READY,ADC12_OVR,ADC12_CMP1H,ADC12_CMP1L,ADC12_CMP2H,ADC12_CMP2L
//NewState:ENABLE , DISABLE
//ReturnValue:NONE
/*************************************************************/    
  //ADC12_EOC:End of conversion interrupt
  //ADC12_READY:ADC ready for conversion interrupt
  //ADC12_OVR:Overrun interrupt
  //ADC12_CMP1H:Higher than ADC_CMP1 interrupt
  //ADC12_CMP1L:Lower than ADC_CMP1 interrupt
  //ADC12_CMP2H:Higher than ADC_CMP2 interrupt
  //ADC12_CMP2L:Lower than ADC_CMP2 interrupt
void ADC12_ConfigInterrupt_CMD( ADC12_IMR_TypeDef ADC_IMR_X , FunctionalStatus NewState)
{
	if (NewState != DISABLE)
	{
		ADC->IER  |= ADC_IMR_X;						//SET
	}
	else
	{
		ADC->IDR  |= ADC_IMR_X;						//CLR
	}
} 
 
/*************************************************************/
//ADC12 CLK ENABLE AND DISABLE
//EntryParameter:ADC_CLK_CMD,NewState
//ADC_CLK_CMD:ADC_CLK_CR,ADC_DEBUG_MODE
//NewState:ENABLE , DISABLE
//ReturnValue:NONE
/*************************************************************/      
void ADC12_CLK_CMD(ADC12_CLK_TypeDef ADC_CLK_CMD , FunctionalStatus NewState)
{
	if (NewState != DISABLE)
	{
		ADC->ECR  |= ADC_CLK_CMD;						//ENABLE
		while(!(ADC->PMSR&ADC_CLK_CMD));
	}
	else
	{
		ADC->DCR  |= ADC_CLK_CMD;						//DISABLE
		while(ADC->PMSR&ADC_CLK_CMD);
	}
}

/*************************************************************/
//ADC12 software reset
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/    
void ADC12_Software_Reset(void)
{
	ADC12_Control(ADC12_SWRST);
}
 
/*************************************************************/
//ADC12 ENABLE
//EntryParameter:NewState
//NewState:ENABLE , DISABLE
//ReturnValue:NONE
/*************************************************************/ 
void ADC12_CMD(FunctionalStatus NewState)
{
	if (NewState != DISABLE)
	{
		ADC12_Control(ADC12_ADCEN);						//ADC12 ENABLE
		while(!(ADC->SR &ADC12_ADCENS));
	}
	else
	{
		ADC12_Control(ADC12_ADCDIS);					//ADC12 DISABLE
		while(ADC->SR&ADC12_ADCENS);
	}
}

/*************************************************************/
//ADC12 READY wait
//EntryParameter:NONE
//ReturnValue:ADC12 READ FLAG
/*************************************************************/ 
void ADC12_ready_wait(void)  
{
	while(!(ADC->SR&ADC12_READY));   					// Waiting for ADC Ready
}

/*************************************************************/
//ADC12 End of conversion wait
//EntryParameter:NONE
//ReturnValue:ADC12 EOC
/*************************************************************/ 
void ADC12_EOC_wait(void)
{
	while(!(ADC->SR & ADC12_EOC));			// EOC wait
} 

/*************************************************************/
//ADC12 Data Register output
//EntryParameter:NONE
//ReturnValue:ADC12 DR
/*************************************************************/ 
U32_T ADC12_DATA_OUPUT(void)
{
	return(ADC->DR);
} 

/*************************************************************/
//ADC12 Configure
//EntryParameter:ADC12_BIT_SELECTED,ADC12_Sample_SELECTED,ADC12_ConverMode,ADC12_DIV,NumConver
//ADC12_BIT_SELECTED:ADC12_12BIT,ADC12_10BIT
//ADC12_Sample_SELECTED:ADC12_3CYCLES,ADC12_4CYCLES,ADC12_6CYCLES,ADC12_8CYCLES
//ADC12_ConverMode:One_shot_mode,Continuous_mode
//ADC12_DIV:ADC_CLK=PCLK/2*DIV . If (PRLVAL == 0 or PRLVAL == 1) FADC = PCLK / 2  , Else FADC = PCLK / (2*PRLVAL);
//NumConver:Number of Conversions value=(1~12);
//ReturnValue:NONE
/*************************************************************/ 
  //10BIT or 12BIT adc ;
  //ADC12_BIT_SELECTED:ADC12_12BIT/ADC12_10BIT;
  //ADC12_Sample_SELECTED:ADC12_3CYCLES/ADC12_4CYCLES/ADC12_6CYCLES/ADC12_8CYCLES;
  //ADC12_ConverMode:One_shot_mode/Continuous_mode;
  //adc date output=last number of Conversions;
void  ADC12_Configure_Mode(ADC12_10bitor12bit_TypeDef ADC12_BIT_SELECTED  , ADC12_Sampleing_TypeDef ADC12_Sample_SELECTED , ADC12_ConverMode_TypeDef  ADC12_ConverMode  , U8_T ADC12_DIV , U8_T NumConver ) 
{
	ADC->MR|=ADC12_DIV|((NumConver-1)<<10);
	if(ADC12_ConverMode==One_shot_mode)
	{
		AD_mode=0;
		ADC->MR&=~CONTCV;								//one short mode
		while(ADC->SR&ADC12_CTCVS);							
	}
	else if(ADC12_ConverMode==Continuous_mode)
	{
		AD_mode=1;
		ADC->MR|=CONTCV;								//Continuous mode
		while(!(ADC->SR&ADC12_CTCVS));							
	}
	ADC12_CMD(ENABLE);									//ADC enable
	if(ADC12_BIT_SELECTED)
	{
		ADC->CR|=ADC12_10BITor12BIT;
	}
	else
	{
		ADC->CR&=~ADC12_10BITor12BIT;
	}
	ADC->CR|=ADC12_Sample_SELECTED;
} 

/*************************************************************/
//ADC12 Compare function set
//EntryParameter:ConverNum_CM0,ConverNum_CM1,CMP0_data,CMP1_data
//ConverNum_CM0:VALUE=(1~12)
//ConverNum_CM1:VALUE=(1~12)
//CMP0_data:VALUE=(1~(0X3FF/0XFFF))
//CMP1_data:VALUE=(1~(0X3FF/0XFFF))
//ReturnValue:NONE
/*************************************************************/ 
  //ConverNum_CM0:Number of Conversions for Compare Function
  //ConverNum_CM1:Number of Conversions for Compare Function
  //ADC will generate a CMPxH/CMPxL interrupt when result of this number of conversion is higher/lower than data set in ADC_CMPx register.
  //ConverNum_CM1Number of Conversions for Compare Function
  //ADC will generate a CMP1H/CMP1L interrupt when result of this number of conversion is greater/less than data set in ADC_CMP1 register.
  
void ADC12_CompareFunction_set(U8_T ConverNum_CM0 , U8_T ConverNum_CM1 , U32_T CMP0_data , U32_T CMP1_data ) 
{
	ADC->MR|=((ConverNum_CM0-1)<<16)|((ConverNum_CM1-1)<<22);
	ADC->CMP0=CMP0_data;
	ADC->CMP1=CMP1_data;
}

/*************************************************************/
//ADC12 Compare statue output
//EntryParameter:NBRCMPx_TypeDef,NBRCMPX_L_TypeDef
//NBRCMPx_TypeDef:NBRCMP0_TypeDef,NBRCMP1_TypeDef
//NBRCMPX_L_TypeDef:NBRCMPX_L_TypeDef,NBRCMPX_H_TypeDef
//ReturnValue:ADC12 Compare result flag
/*************************************************************/ 
  //output statue:ADC-SR(ADC12_CMP0H/ADC12_CMP0L/ADC12_CMP1H/ADC12_CMP1L)
U8_T ADC12_Compare_statue(ADC12_NBRCMPx_TypeDef NBRCMPx_TypeDef , ADC12_NBRCMPx_HorL_TypeDef NBRCMPX_L_TypeDef)
{
	if(NBRCMPx_TypeDef==NBRCMP0_TypeDef)
	{
		if(NBRCMPX_L_TypeDef==NBRCMPX_L_TypeDef)
		{
			return((ADC->SR)&ADC12_CMP0L);
		}
		else
		{
			return((ADC->SR)&ADC12_CMP0H);
		}
		
	}
	else
	{
		if(NBRCMPX_L_TypeDef==NBRCMPX_L_TypeDef)
		{
			return((ADC->SR)&ADC12_CMP1L);
		}
		else
		{
			return((ADC->SR)&ADC12_CMP1H);
		}
	}
}

/*************************************************************/
//ADC12 Conversion chanle seting
//EntryParameter:CMRx,CVx,ADC12_ADCINX
//CVx:VALUE=(1~12)
//ADC12_ADCINX:ADC12_ADCIN0~ADC12_ADCIN12
//ReturnValue:NONE
/*************************************************************/ 
void ADC12_ConversionChannel_Config(ADC12_InputSet_TypeDef ADC12_ADCINX,U8_T CVx)
{
	switch(ADC12_ADCINX)
	{
		case 0:	
			GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFFFFFF)  | 0x00000000;							//ADC0
			GPIOA0->CONHR = (GPIOA0->CONHR&0XFF0FFFFF)  | 0x00A00000;	
			GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFFFFF)  | 0x00000000;
			break;
		case 1:
			GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFFFFFF)  | 0x00000000;							//ADC1
			GPIOA0->CONHR = (GPIOA0->CONHR&0XFFF0FFFF)  | 0x000A0000;	
			GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFFFFF)  | 0x00000000;
			break;
		case 2:
			GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFFFFFF)  | 0x00000000;							//ADC2
			GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFF0FFF)  | 0x0000A000;	
			GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFFFFF)  | 0x00000000;	
			break;
		case 3:
			GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFFFFFF)  | 0x00000000;							//ADC3
			GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFF0FF)  | 0x00000A00;	
			GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFFFFF)  | 0x00000000;	
			break;
		case 4:
			GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFFFFFF)  | 0x00000000;							//ADC4
			GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFFF0F)  | 0x000000A0;	
			GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFFFFF)  | 0x00000000;	
			break;
		case 5:
			GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFFFFFF)  | 0x00000000;							//ADC5
			GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFFFF0)  | 0x0000000A;	
			GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFFFFF)  | 0x00000000;	
			break;
		case 6:
			GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFFFFFF)  | 0x00000000;							//ADC6
			GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFFFFF)  | 0x00000000;	
			GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFF0FFF)  | 0x0000A000;	
			break;
		case 7:
			GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFFFFFF)  | 0x00000000;							//ADC7
			GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFFFFF)  | 0x00000000;	
			GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFF0FF)  | 0x00000A00;	
			break;
		case 8:
			GPIOA0->CONLR = (GPIOA0->CONLR&0X0FFFFFFF)  | 0xA0000000;							//ADC8
			GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFFFFF)  | 0x00000000;	
			GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFFFFF)  | 0x00000000;	
			break;
		case 9:
			GPIOA0->CONLR = (GPIOA0->CONLR&0XF0FFFFFF)  | 0x0A000000;							//ADC9
			GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFFFFF)  | 0x00000000;	
			GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFFFFF)  | 0x00000000;	
			break;
		case 10:
			GPIOA0->CONLR = (GPIOA0->CONLR&0XFF0FFFFF)  | 0x00A00000;							//ADC10
			GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFFFFF)  | 0x00000000;	
			GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFFFFF)  | 0x00000000;	
			break;
		case 11:
			GPIOA0->CONLR = (GPIOA0->CONLR&0XFFF0FFFF)  | 0x000A0000;							//ADC11
			GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFFFFF)  | 0x00000000;	
			GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFFFFF)  | 0x00000000;		
			break;
		case 12:
			GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFF0FFF)  | 0x0000A000;							//ADC12
			GPIOA0->CONHR = (GPIOA0->CONHR&0XFFFFFFFF)  | 0x00000000;	
			GPIOB0->CONLR = (GPIOB0->CONLR&0XFFFFFFFF)  | 0x00000000;		
			break;
	}
	if (AD_mode==0)
	{
    ADC->CMR[0]=0;
    ADC->CMR[1]=0;	
    ADC->CMR[2]=0;
	}
    if (CVx<5)
    {
        ADC->CMR[0]=ADC->CMR[0]| (ADC12_ADCINX<<((CVx-1)*8));
    }
    if ((CVx<9)&&(CVx>4))
    {
        ADC->CMR[1]=ADC->CMR[1] | (ADC12_ADCINX<<((CVx-5)*8));
    }
    if ((CVx<13)&&(CVx>8))
    {
        ADC->CMR[2]=ADC->CMR[2] | (ADC12_ADCINX<<((CVx-9)*8));
    }
}
/*************************************************************/
//ADC Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void ADC_Int_Enable(void)
{
    INTC_ISER_WRITE(ADC_INT);    
}
/*************************************************************/
//ADC Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void ADC_Int_Disable(void)
{
    INTC_ICER_WRITE(ADC_INT);    
}

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/