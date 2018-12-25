/*
  ******************************************************************************
  * @file    asm32s003_interrupt.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"
#include "asm32s003_i2c.h"
#include "asm32s003_syscon.h"
#include "asm32s003_gtc.h"
#include "asm32s003_adc.h"
#include "asm32s003_led.h"
#include "asm32s003_gpio.h"
#include "asm32s003_ifc.h"
#include "app_config.h"
#include "asm32s003_uart.h"

/* externs--------------------------------------------------------------------*/
extern volatile U8_T GTC0_PENDIFlag;
extern volatile U8_T GTC0_STARTIFlag;
extern volatile U8_T GTC0_PSTARTIFlag;
extern volatile U8_T GTC0_MATIFlag;
extern volatile U8_T GTC0_STOPIFlag;
extern volatile U8_T GTC0_CA1semiIFlag;
extern volatile U8_T GTC0_OVFIFlag;

extern volatile U8_T GTC1_PENDIFlag;
extern volatile U8_T GTC1_STARTIFlag;
extern volatile U8_T GTC1_PSTARTIFlag;
extern volatile U8_T GTC1_MATIFlag;
extern volatile U8_T GTC1_STOPIFlag;
extern volatile U8_T GTC1_CA1semiIFlag;
extern volatile U8_T GTC1_OVFIFlag;

extern volatile U8_T GTC2_PENDIFlag;
extern volatile U8_T GTC2_STARTIFlag;
extern volatile U8_T GTC2_PSTARTIFlag;
extern volatile U8_T GTC2_MATIFlag;
extern volatile U8_T GTC2_STOPIFlag;
extern volatile U8_T GTC2_CA1semiIFlag;
extern volatile U8_T GTC2_OVFIFlag;

extern volatile U8_T GTC3_PENDIFlag;
extern volatile U8_T GTC3_STARTIFlag;
extern volatile U8_T GTC3_PSTARTIFlag;
extern volatile U8_T GTC3_MATIFlag;
extern volatile U8_T GTC3_STOPIFlag;
extern volatile U8_T GTC3_CA1semiIFlag;
extern volatile U8_T GTC3_OVFIFlag;

extern volatile U8_T SYSCON_ISOSCSTFlag;
extern volatile U8_T SYSCON_IMOSCSTFlag;
extern volatile U8_T SYSCON_EMOSCSTFlag;
extern volatile U8_T SYSCON_SYSCLKSTFlag;
extern volatile U8_T SYSCON_IWDTINTFlag;
extern volatile U8_T SYSCON_LVDINTFlag;
extern volatile U8_T SYSCON_EM_CMFAILFlag;
extern volatile U8_T SYSCON_EM_CMRCVFlag;
extern volatile U8_T SYSCON_CMD_ERRFlag;

extern volatile U8_T IFC_ENDFlag;
extern volatile U8_T IFC_PRT_ERRFlag;
extern volatile U8_T IFC_UND_ERRFlag;
extern volatile U8_T IFC_ADDR_ERRFlag;
extern volatile U8_T IFC_OVW_ERRFlag;

extern volatile U8_T ADC12_EOCFlag;
extern volatile U8_T ADC12_READFlag;
extern volatile U8_T ADC12_OVRFlag;
extern volatile U8_T ADC12_CMP0HFlag;
extern volatile U8_T ADC12_CMP0LFlag;
extern volatile U8_T ADC12_CMP1HFlag;
extern volatile U8_T ADC12_CMP1LFlag;

extern volatile U8_T CORET_INTFlag;
extern volatile U32_T CORET_Systick_Count;	
extern volatile U8_T EXI_INTFlag[14] ;
extern volatile U8_T UART0_INTFlag[4] ;
extern volatile U8_T UART1_INTFlag[4] ;
extern volatile U8_T CNTA_INTFlag ;
extern volatile U8_T restart ;
extern volatile U8_T I2C_INTFlag ;
extern volatile U8_T LED0_ICENDFlag;
extern volatile U8_T LED0_IPENDFlag;
extern volatile U8_T LED0_IKEYDETFlag;
extern volatile U8_T I2C_mode;

#ifdef FUNC_CODING_KEY_EN
extern volatile U8_T ClockWiseCnt;
extern volatile U8_T CounterClockWiseCnt;
#endif
/*************************************************************/
//CORET Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void CORETHandler(void) 
{
    // ISR content ...
	CORET_INTFlag=1;
	CK802->CORET_CVR = 0x0;							// Clear counter and flag
	CORET_Systick_Count++;

	//¨°????¨¦¨º¨º¦Ì¡À¨¬¨ª?¨®¨º¦Ì¨º¡ÀD?¨°a?¨®????¦Ì?¡ä|¨¤¨ª¡ä¨²??
#ifdef FUNC_CODING_KEY_EN
	if(EXI_INTFlag[CODING_KEY_A_PORT_BIT] == TRUE)
	{
		EXI_INTFlag[CODING_KEY_A_PORT_BIT] = FALSE;
		if((ClockWiseCnt == 0) && (CounterClockWiseCnt == 0))
		{			
			if(GPIO_Read_Status(CODING_KEY_B_PORT_OUT, CODING_KEY_B_PORT_BIT))
			{
				//clockwise rotation
				ClockWiseCnt++;
			}
			else
			{
				//counterclockwise rotation
				CounterClockWiseCnt++;
			}
		}
	}
#endif

}
/*************************************************************/
//SYSCON Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void SYSCONIntHandler(void) 
{
    // ISR content ...
	if((SYSCON->ISR&ISOSC_ST)==ISOSC_ST)
	{
		SYSCON->ICR = ISOSC_ST;
		SYSCON_ISOSCSTFlag=1;
	} 
	else if((SYSCON->ISR&IMOSC_ST)==IMOSC_ST)
	{
		SYSCON->ICR = IMOSC_ST;
		SYSCON_IMOSCSTFlag=1;
	} 
	else if((SYSCON->ISR&EMOSC_ST)==EMOSC_ST)
	{
		SYSCON->ICR = EMOSC_ST;
		SYSCON_EMOSCSTFlag=1;
	} 
	else if((SYSCON->ISR&SYSCLK_ST)==SYSCLK_ST)
	{
		SYSCON->ICR = SYSCLK_ST;
		SYSCON_SYSCLKSTFlag=1;
	}
	else if((SYSCON->ISR&IWDT_INT_ST)==IWDT_INT_ST)
	{
		SYSCON->ICR = IWDT_INT_ST;
		SYSCON_IWDTINTFlag=1;
		//SYSCON_IWDCNT_Reload();              //used when WDT wake up
	}
	else if((SYSCON->ISR&LVD_INT_ST)==LVD_INT_ST)
	{
		SYSCON->ICR = LVD_INT_ST;
		SYSCON_LVDINTFlag=1;
	}
	else if((SYSCON->ISR&EM_CMFAIL_ST)==EM_CMFAIL_ST)
	{
		SYSCON->ICR = EM_CMFAIL_ST;
		SYSCON_EM_CMFAILFlag=1;
	}
	else if((SYSCON->ISR&EM_CMRCV_ST)==EM_CMRCV_ST)
	{
		SYSCON->ICR = EM_CMRCV_ST;
		SYSCON_EM_CMRCVFlag=1;
	}
	else if((SYSCON->ISR&CMD_ERR_ST)==CMD_ERR_ST)
	{
		SYSCON->ICR = CMD_ERR_ST;
		SYSCON_CMD_ERRFlag=1;
	}
}

/*************************************************************/
//IFC Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void IFCIntHandler(void) 
{
    // ISR content ...
	if((IFC->MISR&End_INT)==End_INT)
	{
		IFC->ICLR = End_INT;
		IFC_ENDFlag=1;
	} else if((IFC->MISR&Port_INT)==Port_INT)
	{
		IFC->ICLR = Port_INT;
		IFC_PRT_ERRFlag=1;
	} else if((IFC->MISR&Undefine_INT)==Undefine_INT)
	{
		IFC->ICLR = Undefine_INT;
		IFC_UND_ERRFlag=1;
	} else if((IFC->MISR&Add_INT)==Add_INT)
	{
		IFC->ICLR = Add_INT;
		IFC_ADDR_ERRFlag=1;
	} else if((IFC->MISR&OverWrite_INT)==OverWrite_INT)
	{
		IFC->ICLR = OverWrite_INT;
		IFC_OVW_ERRFlag=1;
	}
}
/*************************************************************/
//ADC Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void ADCIntHandler(void) 
{
    // ISR content ...
	if((ADC->SR&ADC12_EOC)==ADC12_EOC)
	{
		ADC->CSR = ADC12_EOC;
		ADC12_EOCFlag=1;
	}
	else if((ADC->SR&ADC12_READY)==ADC12_READY)
	{
		ADC->CSR = ADC12_READY;
		ADC12_READFlag=1;
	}
	else if((ADC->SR&ADC12_OVR)==ADC12_OVR)
	{
		ADC->CSR = ADC12_OVR;
		ADC12_OVRFlag=1;
	}
	else if((ADC->SR&ADC12_CMP0H)==ADC12_CMP0H)
	{
		ADC->CSR = ADC12_CMP0H;
		ADC12_CMP0HFlag=1;
	}
	else if((ADC->SR&ADC12_CMP0L)==ADC12_CMP0L)
	{
		ADC->CSR = ADC12_CMP0L;
		ADC12_CMP0LFlag=1;
	}
	else if((ADC->SR&ADC12_CMP1H)==ADC12_CMP1H)
	{
		ADC->CSR = ADC12_CMP1H;
		ADC12_CMP1HFlag=1;
	}
	else if((ADC->SR&ADC12_CMP1L)==ADC12_CMP1L)
	{
		ADC->CSR = ADC12_CMP1L;
		ADC12_CMP1LFlag=1;
	}
}

/*************************************************************/
//GTC0 Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC0IntHandler(void) 
{
    // ISR content ...
	if((GTC0->MISR&GTC_PENDI)==GTC_PENDI)					//Period End interrupt
	{
		GTC0->ICR = GTC_PENDI;
		GTC0_PENDIFlag=1;
	} 
	else if((GTC0->MISR&GTC_STOPI)==GTC_STOPI)			//Stop interrupt.
	{
		GTC0->ICR = GTC_STOPI;
		GTC0_STOPIFlag=1;
	} 
	else if((GTC0->MISR&GTC_OVFI)==GTC_OVFI)				//Overflow interrupt
	{
		GTC0->ICR = GTC_OVFI;
		GTC0_OVFIFlag=1;
	} 
	else if((GTC0->MISR&GTC_STARTI)==GTC_STARTI)			//Start interrupt
	{
		GTC0->ICR = GTC_STARTI;
		GTC0_STARTIFlag=1;
	} 
	else if((GTC0->MISR&GTC_PSTARTI)==GTC_PSTARTI)		//Period Start interrupt
	{
		GTC0->ICR = GTC_PSTARTI;
		GTC0_PSTARTIFlag=1;
	} 
	else if((GTC0->MISR&GTC_MATI)==GTC_MATI)				//Pulse Match interrupt.
	{
		GTC0->ICR = GTC_MATI;
		GTC0_MATIFlag=1;
	} 
	else if((GTC0->MISR&GTC_CA1semiI)==GTC_CA1semiI)			//Capture interrupt
	{
		GTC0->ICR = GTC_CA1semiI;
		GTC0_CA1semiIFlag=1;
	}
}

/*************************************************************/
//GTC1 Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC1IntHandler(void) 
{
    // ISR content ...
	if((GTC1->MISR&GTC_PENDI)==GTC_PENDI)					//Period End interrupt
	{
		GTC1->ICR = GTC_PENDI;
		GTC1_PENDIFlag=1;
	} 
	else if((GTC1->MISR&GTC_STOPI)==GTC_STOPI)			//Stop interrupt.
	{
		GTC1->ICR = GTC_STOPI;
		GTC1_STOPIFlag=1;
	} 
	else if((GTC1->MISR&GTC_OVFI)==GTC_OVFI)				//Overflow interrupt
	{
		GTC1->ICR = GTC_OVFI;
		GTC1_OVFIFlag=1;
	} 
	else if((GTC1->MISR&GTC_STARTI)==GTC_STARTI)			//Start interrupt
	{
		GTC1->ICR = GTC_STARTI;
		GTC1_STARTIFlag=1;
	} 
	else if((GTC1->MISR&GTC_PSTARTI)==GTC_PSTARTI)		//Period Start interrupt
	{
		GTC1->ICR = GTC_PSTARTI;
		GTC1_PSTARTIFlag=1;
	} 
	else if((GTC1->MISR&GTC_MATI)==GTC_MATI)				//Pulse Match interrupt.
	{
		GTC1->ICR = GTC_MATI;
		GTC1_MATIFlag=1;
	} 
	else if((GTC1->MISR&GTC_CA1semiI)==GTC_CA1semiI)			//Capture interrupt
	{
		GTC1->ICR = GTC_CA1semiI;
		GTC1_CA1semiIFlag=1;
	}
}

/*************************************************************/
//GTC2 Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC2IntHandler(void) 
{
    // ISR content ...
	if((GTC2->MISR&GTC_PENDI)==GTC_PENDI)					//Period End interrupt
	{
		GTC2->ICR = GTC_PENDI;
		GTC2_PENDIFlag=1;
	} 
	else if((GTC2->MISR&GTC_STOPI)==GTC_STOPI)			//Stop interrupt.
	{
		GTC2->ICR = GTC_STOPI;
		GTC2_STOPIFlag=1;
	} 
	else if((GTC2->MISR&GTC_OVFI)==GTC_OVFI)				//Overflow interrupt
	{
		GTC2->ICR = GTC_OVFI;
		GTC2_OVFIFlag=1;
	} 
	else if((GTC2->MISR&GTC_STARTI)==GTC_STARTI)			//Start interrupt
	{
		GTC2->ICR = GTC_STARTI;
		GTC2_STARTIFlag=1;
	}
    else if((GTC2->MISR&GTC_PSTARTI)==GTC_PSTARTI)		//Period Start interrupt
	{
		GTC2->ICR = GTC_PSTARTI;
		GTC2_PSTARTIFlag=1;
	} 
	else if((GTC2->MISR&GTC_MATI)==GTC_MATI)				//Pulse Match interrupt.
	{
		GTC2->ICR = GTC_MATI;
		GTC2_MATIFlag=1;
	} 
	else if((GTC2->MISR&GTC_CA1semiI)==GTC_CA1semiI)			//Capture interrupt
	{
		GTC2->ICR = GTC_CA1semiI;
		GTC2_CA1semiIFlag=1;
	}
}
/*************************************************************/
//GTC3 Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void GTC3IntHandler(void) 
{
	// ISR content ...
	if((GTC3->MISR&GTC_PENDI)==GTC_PENDI)					//Period End interrupt
	{
		GTC3->ICR = GTC_PENDI;
		GTC3_PENDIFlag=1;
	} 
	else if((GTC3->MISR&GTC_STOPI)==GTC_STOPI)			//Stop interrupt.
	{
		GTC3->ICR = GTC_STOPI;
		GTC3_STOPIFlag=1;
	} 
	else if((GTC3->MISR&GTC_OVFI)==GTC_OVFI)				//Overflow interrupt
	{
		GTC3->ICR = GTC_OVFI;
		GTC3_OVFIFlag=1;
	} 
	else if((GTC3->MISR&GTC_STARTI)==GTC_STARTI)			//Start interrupt
	{
		GTC3->ICR = GTC_STARTI;
		GTC3_STARTIFlag=1;
	} 
	else if((GTC3->MISR&GTC_PSTARTI)==GTC_PSTARTI)		//Period Start interrupt
	{
		GTC3->ICR = GTC_PSTARTI;
		GTC3_PSTARTIFlag=1;
	} 
	else if((GTC3->MISR&GTC_MATI)==GTC_MATI)				//Pulse Match interrupt.
	{
		GTC3->ICR = GTC_MATI;
		GTC3_MATIFlag=1;
	} 
	else if((GTC3->MISR&GTC_CA1semiI)==GTC_CA1semiI)			//Capture interrupt
	{
		GTC3->ICR = GTC_CA1semiI;
		GTC3_CA1semiIFlag=1;
	}
}
/*************************************************************/
//EXI0 Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI0IntHandler(void)
 {
    // ISR content ...
	if ((SYSCON->EXIRS&EXI_PIN0)==EXI_PIN0) 
	{
		SYSCON->EXICR = EXI_PIN0;
		EXI_INTFlag[0]=1;
	}
}

/*************************************************************/
//EXI1 Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI1IntHandler(void) 
{
    // ISR content ...
	if ((SYSCON->EXIRS&EXI_PIN1)==EXI_PIN1) 
	{
		SYSCON->EXICR = EXI_PIN1;
		EXI_INTFlag[1]=1;
	}
}

/*************************************************************/
//UART0 Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void UART0IntHandler(void) 
{
    // ISR content ...
	if ((UART0->ISR&UART_RX_INT_S)==UART_RX_INT_S)
	{
		UART0->ISR=UART_RX_INT_S;
	}
	if( (UART0->ISR&UART_TX_INT_S)==UART_TX_INT_S ) 
    {
		UART0->ISR=UART_TX_INT_S;
#ifdef FUNC_APP_DEBUG_EN
		if((APP_DEBUG_UART_PROT_SEL == UART0) && (GetDebugDataLen() != FALSE))
		{
			CSP_UART_SET_DATA(UART0, GetDebugBufferData());
		}
#endif
	}
	if ((UART0->ISR&UART_RX_IOV_S)==UART_RX_IOV_S)
	{
		UART0->ISR=UART_RX_IOV_S;
	}
	if ((UART0->ISR&UART_TX_IOV_S)==UART_TX_IOV_S)
	{
		UART0->ISR=UART_TX_IOV_S;
	}
}

/*************************************************************/
//UART1 Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void UART1IntHandler(void) 
{
    // ISR content ...
	if ((UART1->ISR&UART_RX_INT_S)==UART_RX_INT_S)
	{
		UART1->ISR=UART_RX_INT_S;
	}
	if( (UART1->ISR&UART_TX_INT_S)==UART_TX_INT_S ) 
    {
		UART1->ISR=UART_TX_INT_S;
#ifdef FUNC_APP_DEBUG_EN
		if((APP_DEBUG_UART_PROT_SEL == UART1) && (GetDebugDataLen() != FALSE))
		{
			CSP_UART_SET_DATA(UART0, GetDebugBufferData());
		}
#endif
	}
	if ((UART1->ISR&UART_RX_IOV_S)==UART_RX_IOV_S)
	{
		UART1->ISR=UART_RX_IOV_S;
	}
	if ((UART1->ISR&UART_TX_IOV_S)==UART_TX_IOV_S)
	{
		UART1->ISR=UART_TX_IOV_S;
	}
}

/*************************************************************/
//I2C Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void I2CIntHandler(void) 
{
		//I2C master mode Interrupt
		/*I2C_INTFlag=1;
		if(restart==1) 
		{
			restart=0;
			I2C0->CR = (I2C0->CR & (~I2C_SI)) | I2C_STA ;
		} 
		else 
		{
			I2C0->CR = I2C0->CR & (~I2C_SI);
		}*/
		//I2C slave mode Interrupt
		I2C_Slave_Receive();


}

/*************************************************************/
//EXI2~3 Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI2to3IntHandler(void) 
{
    // ISR content ...
	if ((SYSCON->EXIRS&EXI_PIN2)==EXI_PIN2) 
	{
		SYSCON->EXICR = EXI_PIN2;
		EXI_INTFlag[2]=1;
	} 
	else if ((SYSCON->EXIRS&EXI_PIN3)==EXI_PIN3) 
	{
		SYSCON->EXICR = EXI_PIN3;
		EXI_INTFlag[3]=1;
	}
}
/*************************************************************/
//EXI4~8 Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI4to8IntHandler(void) 
{
    // ISR content ...
	if ((SYSCON->EXIRS&EXI_PIN4)==EXI_PIN4) 
	{
		SYSCON->EXICR = EXI_PIN4;
		EXI_INTFlag[4]=1;
	} 
	else if ((SYSCON->EXIRS&EXI_PIN5)==EXI_PIN5) 
	{
		SYSCON->EXICR = EXI_PIN5;
		EXI_INTFlag[5]=1;
	} 
	else if ((SYSCON->EXIRS&EXI_PIN6)==EXI_PIN6) 
	{
		SYSCON->EXICR = EXI_PIN6;
		EXI_INTFlag[6]=1;
	} 
	else if ((SYSCON->EXIRS&EXI_PIN7)==EXI_PIN7) 
	{
		SYSCON->EXICR = EXI_PIN7;
		EXI_INTFlag[7]=1;
	} 
	else if ((SYSCON->EXIRS&EXI_PIN8)==EXI_PIN8) 
	{
		SYSCON->EXICR = EXI_PIN8;
		EXI_INTFlag[8]=1;
	}
}

/*************************************************************/
//EXI9~13 Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void EXI9to13IntHandler(void) 
{
    // ISR content ...
	if ((SYSCON->EXIRS&EXI_PIN9)==EXI_PIN9) 
	{
		SYSCON->EXICR = EXI_PIN9;
		EXI_INTFlag[9]=1;
	} 
	else if ((SYSCON->EXIRS&EXI_PIN10)==EXI_PIN10) 
	{
		SYSCON->EXICR = EXI_PIN10;
		EXI_INTFlag[10]=1;
	} 
	else if ((SYSCON->EXIRS&EXI_PIN11)==EXI_PIN11) 
	{
		SYSCON->EXICR = EXI_PIN11;
		EXI_INTFlag[11]=1;
	} 
	else if ((SYSCON->EXIRS&EXI_PIN12)==EXI_PIN12) 
	{
		SYSCON->EXICR = EXI_PIN12;
		EXI_INTFlag[12]=1;
	} 
	else if ((SYSCON->EXIRS&EXI_PIN13)==EXI_PIN13) 
	{
		SYSCON->EXICR = EXI_PIN13;
		EXI_INTFlag[13]=1;
	}
}
/*************************************************************/
//COUNTA Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void CNTAIntHandler(void) 
{
    // ISR content ...
	CNTA_INTFlag=1;
}

/*************************************************************/
//LED Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void LEDIntHandler(void) 
{
    // ISR content ...
	if ((LED0->RISR &ICEND)==ICEND) 
	{
		LED0->ICR = ICEND;
		LED0_ICENDFlag=1;
	} 
	else if((LED0->RISR &IPEND)==IPEND) 
	{
		LED0->ICR = IPEND;
		LED0_IPENDFlag=1;
	} 
	else if((LED0->RISR &IKEYDET)==IKEYDET) 
	{
		LED0->ICR = IKEYDET;
		LED0_IKEYDETFlag=1;
	} 
}

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/