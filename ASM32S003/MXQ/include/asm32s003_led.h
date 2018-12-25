/*
  ******************************************************************************
  * @file    asm32s003_led.h
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _asm32s003_led_H
#define _asm32s003_led_H

/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"

/******************************************************************************
************************** LED0 Registers Definition ****************************
******************************************************************************/
#define		 LED0_CR_RST     			((CSP_REGISTER_T)0x00000000)		/**< CR  reset value  	*/
#define		 LED0_BRIGHT_RST            ((CSP_REGISTER_T)0x00000000)      	/**< BRIGHT  reset value */
#define		 LED0_RISR_RST       		((CSP_REGISTER_T)0x00000000)  		/**< RISR reset value  	*/
#define		 LED0_IMCR_RST             	((CSP_REGISTER_T)0x00000000)		/**< IMCR   reset value  */
#define		 LED0_MISR_RST              ((CSP_REGISTER_T)0x00000000)    	/**< MISR   reset value  */
#define		 LED0_ICR_RST               ((CSP_REGISTER_T)0x00000000)    	/**< ICR   reset value  */
#define		 LED0_SEGDAT0_RST           ((CSP_REGISTER_T)0x00000000)     	/**< SEGDAT0  reset value  */
#define		 LED0_SEGDAT1_RST           ((CSP_REGISTER_T)0x00000000)   		/**< SEGDAT1  reset value  */
#define		 LED0_SEGDAT2_RST           ((CSP_REGISTER_T)0x00000000)    	/**< SEGDAT2  reset value  */
#define		 LED0_SEGDAT3_RST           ((CSP_REGISTER_T)0x00000000)  		/**< SEGDAT3  reset value */
#define		 LED0_KEYST_RST             ((CSP_REGISTER_T)0x00000000)		/**< KEYST  reset value */
#define		 LED0_TIMCR_RST             ((CSP_REGISTER_T)0x00000000)		/**< TIMCR  reset value */
/******************************************************************************
***************************** LED0 Registers mask ***************************
******************************************************************************/

/******************************************************************************
************************* LED0 Registers reset value ************************
******************************************************************************/
 #define LIGHTON          ((CSP_REGISTER_T)(0x00000001ul))          //led  LIGHT ON

/**
  * @brief  LED CLK Control
  */
typedef enum
{
		LEDCLK_DIV4  =  ((CSP_REGISTER_T)(0x00ul << 1)),
		LEDCLK_DIV8  =  ((CSP_REGISTER_T)(0x01ul << 1)),
		LEDCLK_DIV16 =  ((CSP_REGISTER_T)(0x02ul << 1)),
		LEDCLK_DIV32 =  ((CSP_REGISTER_T)(0x03ul << 1)),
		LEDCLK_DIV64 =  ((CSP_REGISTER_T)(0x04ul << 1)),
		LEDCLK_DIV128 = ((CSP_REGISTER_T)(0x05ul << 1)),
		LEDCLK_DIV256 = ((CSP_REGISTER_T)(0x06ul << 1)),
		LEDCLK_DIV512 = ((CSP_REGISTER_T)(0x07ul << 1))
}LED_LEDCLK_TypeDef;

/**
  * @brief  LED COME MODE Control
  */
typedef enum
{
	LED_ComMode_2COM	=		((CSP_REGISTER_T)(0X00<<4)),	
	LED_ComMode_3COM	=		((CSP_REGISTER_T)(0X01<<4)),
	LED_ComMode_4COM	=		((CSP_REGISTER_T)(0X02<<4))
}LED_ComMode_TypeDef;

/**
  * @brief  Mechanical Key Input Scan Enable
  */
typedef enum
{
	ENKEY_SCEN		=			((CSP_REGISTER_T)(0X01<<6)),	
	DISKEY_SCEN		=			((CSP_REGISTER_T)(0X00<<6))	
}LED_KEY_SCEN_TypeDef;

/**
  * @brief  LED Tkey share diable and enable
  */
typedef enum
{
	ENTkeyShare			=			((CSP_REGISTER_T)(0X01<<7)),	
	DISTkeyShare		=			((CSP_REGISTER_T)(0X00<<7))
}LED_TkeyShare_TypeDef;

/**
  * @brief  LED Bright set
  */
typedef enum
{
	LED_Bright_100			=			((CSP_REGISTER_T)(0X00)),	
	LED_Bright_87			=			((CSP_REGISTER_T)(0X01)),
	LED_Bright_75			=			((CSP_REGISTER_T)(0X02)),	
	LED_Bright_62			=			((CSP_REGISTER_T)(0X03)),
	LED_Bright_50			=			((CSP_REGISTER_T)(0X04)),	
	LED_Bright_37			=			((CSP_REGISTER_T)(0X05)),
	LED_Bright_25			=			((CSP_REGISTER_T)(0X06)),	
	LED_Bright_12			=			((CSP_REGISTER_T)(0X07))
}LED_Bright_TypeDef;

/**
  * @brief  LED INT
  */
  //MISR/IMCR/ICR/RISR
typedef enum
{
	ICEND				=			((CSP_REGISTER_T)(0X01<<0)),	
	IPEND				=			((CSP_REGISTER_T)(0X01<<1)),	
	IKEYDET				=			((CSP_REGISTER_T)(0X01<<2))		
}LED_INT_TypeDef;		

/**
  * @brief  LED SEGDATX data selected
  */
typedef enum
{
	SEGDAT_NUM0				=			((CSP_REGISTER_T)(0X00)),	
	SEGDAT_NUM1				=			((CSP_REGISTER_T)(0X01)),	
	SEGDAT_NUM2				=			((CSP_REGISTER_T)(0X02)),
	SEGDAT_NUM3				=			((CSP_REGISTER_T)(0X03))
}LED_SEGDATX_TypeDef;
/******************************************************************************
********************* ADC12 External Functions Declaration ********************
******************************************************************************/
extern void LED_RESET_VALUE(void);
extern void LED_Init(void); 
extern void LED_SCAN_ENABLE(FunctionalStatus NewState);
extern void LED_Control_Config(LED_LEDCLK_TypeDef LED_LEDCLK , LED_ComMode_TypeDef LED_ComMode , LED_KEY_SCEN_TypeDef xKEY_SCEN , LED_TkeyShare_TypeDef xTkeyShare , U8_T TKEYSCCNT , LED_Bright_TypeDef LED_Bright_X , U8_T DCOMCNT , U8_T NOVCNT , U8_T KCOMCNT );
extern void LED_INT_CMD(LED_INT_TypeDef LED_INT_x , FunctionalStatus NewState);
extern void LED_SEGDATX_data_write(LED_SEGDATX_TypeDef SEGDATX , U8_T SEGDATX_data);
extern U8_T LED_KEYST_Read(LED_SEGDATX_TypeDef SEGDATX);
extern unsigned char _CHAR_[10];
extern void LED_Int_Enable(void);
extern void LED_Int_Disable(void);
extern void LED_Wakeup_Enable(void);
extern void LED_Wakeup_Disable(void);
#endif   /* asm32s003_led_H */

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/