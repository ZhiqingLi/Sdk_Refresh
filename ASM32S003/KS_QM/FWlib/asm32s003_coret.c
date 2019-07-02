/*
  ******************************************************************************
  * @file    asm32s003_CORET.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "asm32s003_coret.h"
#include "asm32s003_syscon.h"


/* defines -------------------------------------------------------------------*/
volatile unsigned int CORET_INTFlag ;			//CORET interrupt flag
/* externs--------------------------------------------------------------------*/


/*************************************************************/
//Deinitializes the syscon registers to their default reset
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/    
void CORET_DeInit(void)
{
	CK802->CORET_CSR=CORET_CSR_RST;
	CK802->CORET_RVR=CORET_RVR_RST;
	CK802->CORET_CVR=CORET_CVR_RST;
	CK802->CORET_CALIB=CORET_CALIB_RST;
}

/*************************************************************/
//CORET Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void CORET_Int_Enable(void)
{
    CK802->CORET_CVR = 0x0;							// Clear counter and flag
	INTC_ISER_WRITE(EPT_INT);    
}

/*************************************************************/
//CORET Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void CORET_Int_Disable(void)
{
    INTC_ICER_WRITE(EPT_INT);    
}

/*************************************************************/
// CORET Wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void  CORET_WakeUp_Enable(void)
{
    INTC_IWER_WRITE(EPT_INT);    
}

/*************************************************************/
// CORET Wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void  CORET_WakeUp_Disable(void)
{
    INTC_IWDR_WRITE(EPT_INT);    
}

/*************************************************************/
// CORET START
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void CORET_start(void)
{
	CK802->CORET_CSR|=0x01;
}
/*************************************************************/
// CORET stop
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void CORET_stop(void)
{
	CK802->CORET_CSR&=0Xfffffffe;
}
/*************************************************************/
// CORET CLKSOURC EX
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void CORET_CLKSOURCE_EX(void)
{
	CK802->CORET_CSR&=0Xfffffffb;
}
/*************************************************************/
// CORET CLKSOURC IN
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void CORET_CLKSOURCE_IN(void)
{
	CK802->CORET_CSR|=0x04;
}
/*************************************************************/
//CORET TICKINT enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void CORET_TICKINT_Enable(void)
{
    CK802->CORET_CSR|=0x02;
}

/*************************************************************/
//CORET TICKINT enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void CORET_TICKINT_Disable(void)
{
   CK802->CORET_CSR&=0Xfffffffd;
}

/*************************************************************/
// CORET reload
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void CORET_reload(void)
{
	CK802->CORET_CVR = 0x0;							// Clear counter and flag
}

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/