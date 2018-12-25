/*
  ******************************************************************************
  * @file    asm32s003_CORET.h
  * @author  A1semi
  * @version V1.00
  * @date    2017/04/11
  ******************************************************************************/

  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _asm32s003_CORET_H
#define _asm32s003_CORET_H

/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"

/******************************************************************************
************************* syscon Registers Definition *************************
******************************************************************************/
#define CORET_CSR_RST     	((CSP_REGISTER_T)0x00000004)
#define CORET_RVR_RST     	((CSP_REGISTER_T)0x00000000)
#define CORET_CVR_RST     	((CSP_REGISTER_T)0x00000000)
#define CORET_CALIB_RST     	((CSP_REGISTER_T)0x00000000)




extern void CORET_DeInit(void);
extern void CORET_Int_Enable(void);
extern void CORET_Int_Disable(void);
extern void CORET_WakeUp_Enable(void);
extern void CORET_WakeUp_Disable(void);
extern void CORET_start(void);
extern void CORET_stop(void);
extern void CORET_CLKSOURCE_EX(void);
extern void CORET_CLKSOURCE_IN(void);
extern void CORET_TICKINT_Enable(void);
extern void CORET_TICKINT_Disable(void);
extern void CORET_reload(void);

#endif   /**< asm32s003_syscon_H */

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/