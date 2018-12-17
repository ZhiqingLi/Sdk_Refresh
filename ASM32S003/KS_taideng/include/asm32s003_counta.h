/*
  ******************************************************************************
  * @file    asm32s003_counta.h
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _asm32s003_counta_H
#define _asm32s003_counta_H

/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"

#define CA_RESET_VALUE  (0x00000000)
//--------------------------------------------------------------------------------
//-----------------------------countA  value enum define--------------------------
//--------------------------------------------------------------------------------
/**
  * @brief  countA Period int 
  */
typedef enum
{
    Period_NA  = 0,                            //Interrupt enable/disable for High/low period elapsed              
    Period_H   = 1,
    Period_L   = 2,
    Period_H_L = 3,                                                  
}CA_INT_TypeDef;
/**
  * @brief  countA DIV 
  */
typedef enum
{
    DIV1    =	((0 & 0x03ul)<<4) ,            //Counter A input clock frequency selection 
    DIV2    =	((1 & 0x03ul)<<4) ,
    DIV4    =	((2 & 0x03ul)<<4) ,
    DIV8    =	((3 & 0x03ul)<<4) ,
}CA_CLKDIV_TypeDef;
/**
  * @brief  countA mode
  */
typedef enum
{
    ONESHOT_MODE    =	(0x00ul << 1),          //Mode Selection:oneshotmode/repeat mode
    REPEAT_MODE     =	(0x01ul << 1),
}CA_Mode_TypeDef;
/**
  * @brief  countA carrier setting 
  */
typedef enum
{
    CARRIER_OFF 	=	(0x00ul << 25),         //Carrier signal
    CARRIER_ON  	=	(0x01ul << 25),
}CA_CARRIER_TypeDef;
/**
  * @brief  Carrier Waveform Output Starting Polarity
  */
typedef enum
{
    OSP_LOW 	=	0,                          //Carrier Waveform Output Starting Polarity
    OSP_HIGH    =	1,
}CA_OSP_TypeDef;
/**
  * @brief  Carrier register load
  */
typedef enum
{
    HW_STROBE_0 =   (0x01ul<<17),               //Counter A data register Hardware/software load enable.  
    HW_STROBE_1 =   (0x01ul<<18),
    SW_STROBE   =   (0x01ul<<16), 
}CA_STROBE_TypeDef;
/**
  * @brief  Carrier rem output signal
  */
typedef enum
{
    ENVELOPE_0    =	(0x00ul << 24),             //REM output signal selection bit
    ENVELOPE_1    =	(0x01ul << 24),
}CA_ENVELOPE_TypeDef;
/**
  * @brief  Carrier PENDREM 
  */
typedef enum
{
    PENDREM_OFF  =	((0 & 0x03ul)<<21),
    PENDREM_1    =	((1 & 0x03ul)<<21),
    PENDREM_2    =	((2 & 0x03ul)<<21),
}CA_PENDREM_TypeDef;
/**
  * @brief  Carrier ATCHREM 
  */
typedef enum
{
    MATCHREM_OFF  =	((0 & 0x03ul)<<19),
    MATCHREM_1    =	((1 & 0x03ul)<<19),
    MATCHREM_2    =	((2 & 0x03ul)<<19),
}CA_MATCHREM_TypeDef;
/**
  * @brief  Carrier REMSTAT
  */
typedef enum
{
   REMSTAT_0   =	((0 & 0x01ul)<<23),
   REMSTAT_1   =	((1 & 0x01ul)<<23),
}CA_REMSTAT_TypeDef;

/******************************************************************************
* CACON0 : Control 0 Register
******************************************************************************/
#define CA_START       		(0x01ul << 2)          /**< Counter A Start       */
#define CA_ONESHOT_MODE    	(0x00ul << 1)
#define CA_REPEAT_MODE     	(0x01ul << 1)
#define CA_INTTIME_LOW  	(0x00ul)
#define CA_INTTIME_HIGH 	(0x01ul)
#define CA_INTTIME_BOTH 	(0x02ul)
#define CA_SDIV_Div1    	(0x00ul)
#define CA_SDIV_Div2    	(0x01ul)
#define CA_SDIV_Div4    	(0x02ul)
#define CA_SDIV_Div8    	(0x03ul)

#define CA_OSP(val)         ((val & 0x01ul)<<0)   /**< Output Start Polarity */
           
#define CA_MODE_SEL(val)    ((val & 0x01ul)<<1)   /**< Counter A Mode Selection */
#define CA_STOP        		(0x01ul<<3)				  /**< Counter A Stop  */
#define CA_CLK_DIV(val)     ((val & 0x03ul)<<4)   /**< Counter A Clock Divider  */

#define CA_SW_STROBE        (0x01ul << 16)        /**< S/W load CADATA        */
#define CA_CARRIER_OFF 		(0x00ul << 25)
#define CA_CARRIER_ON  		(0x01ul << 25)
#define CA_ENVELOPE    		(0x01ul << 24)
#define CA_REM_STAT(val)    ((val & 0x01ul)<<23)   /**< REM port status   */
#define CA_HW_STROBE(val)   ((val & 0x01ul)<<17)   /**< H/W load CADATA enable */

#define CA_TCPEND_REM_CON(val) ((val & 0x03ul)<<21)   /**< CARRIER on/off when TC period end interrupt occurs */
#define CA_TCMATCH_REM_CON(val) ((val & 0x03ul)<<19)   /**< CARRIER on/off when TC pulse match interrupt occurs */

/** Set CACON register                                       */
 #define CSP_CA_SET_CACON (ca, val)    ((ca)->CACON = (val))
/** Get CACON register                                               */
 #define CSP_CA_GET_CACON(ca)        	((ca)->CACON1 )
/******************************************************************************
********************** CA External Functions Declaration **********************
******************************************************************************/
extern void COUNTA_Init(uint32_t Data_H,uint32_t Data_L,CA_INT_TypeDef INT_Mode,
                     CA_CLKDIV_TypeDef DIVx,CA_Mode_TypeDef Mode,CA_CARRIER_TypeDef Carrier,
                     CA_OSP_TypeDef OSP_Mode) ;
extern void COUNTA_Config(CA_STROBE_TypeDef STROBE,CA_PENDREM_TypeDef Pend_CON,
                        CA_MATCHREM_TypeDef Match_CON,CA_REMSTAT_TypeDef Stat_CON,CA_ENVELOPE_TypeDef ENVELOPE );
extern void COUNT_DeInit(void);
extern void COUNTA_Start(void);
extern void COUNTA_Stop(void);
extern void COUNTA_Int_Disable(void);
extern void COUNTA_Int_Enable(void);
extern void COUNTA_Wakeup_Disable(void);
extern void COUNTA_Wakeup_Enable(void);
extern void COUNTA_IO_Init(U8_T COUNTA_IO_G);
extern void COUNTA_Data_Update(uint32_t Data_H,uint32_t Data_L);
#endif   /* asm32s003_counta_H */
/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/