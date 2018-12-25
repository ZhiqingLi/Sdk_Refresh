/*
  ******************************************************************************
  * @file    asm32s003_gtc.h
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _asm32s003_gtc_H
#define _asm32s003_gtc_H

/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"

//--------------------------------------------------------------------------------
//-----------------------------GTC  value enum define----------------------------
//--------------------------------------------------------------------------------
/**
  * @brief  GTC clock
  */
typedef enum
{
	GTC_FIN_PCK		=		((CSP_REGISTER_T)0X00),
	GTC_FIN_TCLK0	=		((CSP_REGISTER_T)0X01),
	GTC_FIN_TCLK2	=		((CSP_REGISTER_T)0X03),
	GTC_FIN_CountA	=		((CSP_REGISTER_T)0X05),
	GTC_FIN_IMOSC	=		((CSP_REGISTER_T)0X07)
}GTC_FIN_TypeDef;

/**
  * @brief  GTC bit selection
  */
typedef enum
{
	Counter_Size_8BIT	=	((CSP_REGISTER_T)0X07),
	Counter_Size_10BIT	=	((CSP_REGISTER_T)0X09),
	Counter_Size_16BIT	=	((CSP_REGISTER_T)0X0F),
	Counter_Size_32BIT	=	((CSP_REGISTER_T)0X1F)
}Counter_Size_TypeDef;

/**
  * @brief  GTC Control Set
  */
typedef enum
{
	GTC_ControlSet_start_stop		=	((CSP_REGISTER_T)0X01ul<<0),					//Start The Counter
	GTC_ControlSet_UPDATE			=	((CSP_REGISTER_T)0X01ul<<1),					//Update Register Request
	GTC_ControlSet_STOPHOLD			=	((CSP_REGISTER_T)0X01ul<<2),					//Stop Count Hold is Enabled
	GTC_ControlSet_STOPCLEAR		=	((CSP_REGISTER_T)0X01ul<<3),					//Stop Count Hold is Enabled
	GTC_ControlSet_IDLEST			=	((CSP_REGISTER_T)0X01ul<<8),					//IDLE Status Level will be HIGH
	GTC_ControlSet_OUTST			=	((CSP_REGISTER_T)0X01ul<<9),					//Output Status Level will be HIGH
	GTC_ControlSet_KEEP				=	((CSP_REGISTER_T)0X01ul<<10),					//Keep Stop Level is Enabled
	GTC_ControlSet_PWMIM			=	((CSP_REGISTER_T)0X01ul<<11),					//Toggle Operation is Enabled/PWM Operation is Disabled
	GTC_ControlSet_PWMEN			=	((CSP_REGISTER_T)0X01ul<<12),					//TPWM Output is Enabled
	GTC_ControlSet_REPEAT			=	((CSP_REGISTER_T)0X01ul<<13),					//Repeat Mode is Enabled
	GTC_ControlSet_CNTM				=	((CSP_REGISTER_T)0X01ul<<14),					//Continuous Counting Mode is Enabled
	GTC_ControlSet_HWTRIG			=	((CSP_REGISTER_T)0X01ul<<15),					//Hardware Auto-trigger is Enabled
	GTC_ControlSet_CA1semi_F			=	((CSP_REGISTER_T)0X01ul<<17),					//When Falling Edge of External Input Signal is Detected,Current Counter Value is Stored into the Capture Down Register.
	GTC_ControlSet_CA1semi_R			=	((CSP_REGISTER_T)0X01ul<<18),					//When Rising Edge of External Input Signal is Detected,Current Counter Value is Stored into the Capture Up Register.
	GTC_ControlSet_CA1semi_TCAP0		=	((CSP_REGISTER_T)0X00ul<<19),					//Capture input source is TCAP0
	GTC_ControlSet_CA1semi_TCAP1		=	((CSP_REGISTER_T)0X01ul<<19),					//Capture input source is TCAP1
	GTC_ControlSet_CA1semi_TCAP2		=	((CSP_REGISTER_T)0X02ul<<19),					//Capture input source is TCAP2
	GTC_ControlSet_CA1semi_TCAP3		=	((CSP_REGISTER_T)0X03ul<<19),					//Capture input source is TCAP3
	GTC_ControlSet_PWME0			=	((CSP_REGISTER_T)0X01ul<<24),			
	GTC_ControlSet_PWME1			=	((CSP_REGISTER_T)0X02ul<<24),
	GTC_ControlSet_PWME2			=	((CSP_REGISTER_T)0X04ul<<24),
	GTC_ControlSet_PWME3			=	((CSP_REGISTER_T)0X08ul<<24),	
	GTC_ControlSet_PWME4			=	((CSP_REGISTER_T)0X10ul<<24),	
	GTC_ControlSet_PWME5			=	((CSP_REGISTER_T)0X20ul<<24)
//	PWM Extension Bits			
//DUTY(%)=(PULSE/PERIOD+E/(PERIOD*64))*100%
//PWMEX	â€œStretchedâ€?Cycle Number
//PWMEX0	32		(E=1)		
//PWMEX1	16, 48
//PWMEX2	8, 24, 40, 56
//PWMEX3	4, 12, 20, 28, 36 , 44, 52, 60
//PWMEX4	2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62
//PWMEX5	1, 3, 5, 7, 9,11,13,15,17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63
}GTC_ControlSet_TypeDef;
  
/**
  * @brief  GTC INT MASK SET/CLR  Set
  */
typedef enum
{
	GTC_STARTI     	=	((CSP_REGISTER_T)(0x01ul << 0)),      /**< Overflow Interrupt     */
	GTC_STOPI    	=	((CSP_REGISTER_T)(0x01ul << 1)),      /**< Match Interrupt        */
	GTC_PSTARTI    	=	((CSP_REGISTER_T)(0x01ul << 2)),      /**< Overflow Interrupt     */
	GTC_PENDI    	=	((CSP_REGISTER_T)(0x01ul << 3)),      /**< Match Interrupt        */
	GTC_MATI   		=	((CSP_REGISTER_T)(0x01ul << 4)),      /**< Overflow Interrupt     */
	GTC_OVFI 	   	=	((CSP_REGISTER_T)(0x01ul << 5)),      /**< Match Interrupt        */
	GTC_CA1semiI    	=	((CSP_REGISTER_T)(0x01ul << 6))       /**< Overflow Interrupt     */
}GTC_IMSCR_TypeDef;  
  
/**
  * @brief  GTC IO INIT MODE
  */  
typedef enum
{  
	GTC_IO_TXOUT	=		1,
	GTC_IO_TCLK0	=		2,
	GTC_IO_TCLK2	=		3,
	GTC_IO_TCAPX	=		4
}GTC_IO_MODE_TypeDef;  
  
/**
  * @brief  GTC IO INIT MODE
  */  
typedef enum
{  
	GTC0_IO	=		1,
	GTC1_IO	=		2,
	GTC2_IO	=		3,
	GTC3_IO	=		4
}GTC_IO_NUM_TypeDef;    
/******************************************************************************
************************** GTC Registers Definition ****************************
******************************************************************************/
/** @addtogroup GTC Registers mask Value
  * @{
  */
#define GTC_IDR_MASK          (0x03FFFFFFul)    /**< IDR mask        */
#define GTC_CSSR_MASK	      (0x00000001ul)    /**< CSSR mask       */
#define GTC_CEDR_MASK	      (0x80000001ul)    /**< CEDR mask       */
#define GTC_SRR_MASK          (0x00000001ul)    /**< SRR mask        */
#define GTC_CSR_MASK          (0x00077F0Dul)    /**< CSR mask        */
#define GTC_CCR_MASK          (0x00077F0Dul)    /**< CCR mask        */
#define GTC_IMSCR_MASK        (0x0000007Ful)    /**< IMSCR mask      */
#define GTC_RISR_MASK         (0x0000007Ful)    /**< RISR mask       */
#define GTC_MISR_MASK         (0x0000007Ful)    /**< MIS mask       */
#define GTC_ICR_MASK          (0x0000007Ful)    /**< ICR mask        */
#define GTC_SR_MASK           (0xFFFF7F0Dul)    /**< SR mask         */
#define GTC_CDR_MASK          (0xFFFFFFFFul)    /**< SR mask         */
#define GTC_CSMR_MASK         (0x0000001Ful)    /**< SR mask         */
#define GTC_CVR_MASK          (0xFFFFFFFFul)    /**< CVR mask        */
#define GTC_PRDR_MASK         (0xFFFFFFFFul)    /**< DR mask         */
#define GTC_PULR_MASK         (0xFFFFFFFFul)    /**< SR mask         */
#define GTC_UCDR_MASK         (0xFFFFFFFFul)    /**< SR mask         */
#define GTC_UCSMR_MASK        (0x0000001Ful)    /**< SR mask         */
#define GTC_UPRDR_MASK        (0xFFFFFFFFul)    /**< DR mask         */
#define GTC_UPULR_MASK        (0xFFFFFFFFul)    /**< SR mask         */
#define GTC_CUCR_MASK         (0xFFFFFFFFul)    /**< SR mask         */
#define GTC_CDCR_MASK         (0xFFFFFFFFul)    /**< SR mask         */

/** @addtogroup GTC Registers RST  Value
  * @{
  */
#define GTC_IDR_RST          (0x0011000Aul)    /**< IDR reset value         */
#define GTC_CSSR_RST	     (0x00000000ul)    /**< CSSR reset value        */
#define GTC_CEDR_RST	     (0x00000000ul)    /**< CEDR reset value        */
#define GTC_SRR_RST          (0x00000000ul)    /**< SRR reset value         */
#define GTC_CSR_RST          (0x00002000ul)    /**< CSR reset value         */
#define GTC_CCR_RST          (0x00000000ul)    /**< CCR reset value         */
#define GTC_IMSCR_RST        (0x00000000ul)    /**< IMSCR reset value       */
#define GTC_RISR_RST         (0x00000000ul)    /**< RISR reset value        */
#define GTC_MISR_RST         (0x00000000ul)    /**< MIS Rreset value        */
#define GTC_ICR_RST          (0x00000000ul)    /**< ICR reset value         */
#define GTC_SR_RST           (0x00F82000ul)    /**< SR reset value          */
#define GTC_CDR_RST          (0x00000000ul)    /**< SR reset value          */
#define GTC_CSMR_RST         (0x0000001Ful)    /**< SR reset value          */
#define GTC_CVR_RST          (0x00000000ul)    /**< CVR reset value         */
#define GTC_PRDR_RST         (0x00000000ul)    /**< DR reset value          */
#define GTC_PULR_RST         (0x00000000ul)    /**< SR reset value          */
#define GTC_UCDR_RST         (0x00000000ul)    /**< SR reset value          */
#define GTC_UCSMR_RST        (0x0000001Ful)    /**< SR reset value          */
#define GTC_UPRDR_RST        (0x00000000ul)    /**< DR reset value          */
#define GTC_UPULR_RST        (0x00000000ul)    /**< SR reset value          */
#define GTC_CUCR_RST         (0x00000000ul)    /**< SR reset value          */
#define GTC_CUCR_RST         (0x00000000ul)    /**< SR reset value          */
#define GTC_CDCR_RST         (0x00000000ul)    /**< SR reset value          */

//IDR : ID Register
#define GTC_IPIDCODE_MASK  	(0x03FFFFFFul << 0)    		/**<  IPIDCODE mask     */
#define GTC_IPIDCODE  	   	(0x0011000Aul << 0)    		/**<  IPIDCODE value    */

// CEDR : Clock Enable/Disable Register
#define GTC_CLKEN          	(0x01ul << 0)         		/**< IP Clock           */
#define GTC_DBGEN          	(0x01ul << 31)        		/**< Debug Mode Enable  */

//SRR : Software Reset Register
#define GTC_SWRST          	(0x01ul << 0)        	 	/**< Software Reset     */

// SR : Status Register
#define GTC_CLKENS     		(0x01ul << 30)	 			/**< tc16 Clock Enable   */
#define GTC_CLKSRCS			(0x01ul << 31)	 			/**< tc16 DBGEN          */


/**
  * @brief  GTC FIN selected 
  */

/******************************************************************************
********************** GTC External Functions Declaration **********************
******************************************************************************/
extern void GTC0_Init(void);
extern void GTC1_Init(void);
extern void GTC2_Init(void);
extern void GTC3_Init(void);
extern void GTC_RESET_VALUE(CSP_GTC_T *GTC);
extern void GTC_MASK_VALUE(CSP_GTC_T *GTC);
extern void GTC_Configure(CSP_GTC_T *GTC , GTC_FIN_TypeDef  GTC_FIN_X, int  GTC_DIVN , int  GTC_DINM , Counter_Size_TypeDef  Counter_Size_X  , U32_T loadCounter_PRDR , U32_T loadCounter_PULR);
extern void GTC_ControlSet_Configure (CSP_GTC_T *GTC , GTC_ControlSet_TypeDef GTC_ControlSet_x , FunctionalStatus NewState);
extern void GTC_ConfigInterrupt_CMD(CSP_GTC_T *GTC , GTC_IMSCR_TypeDef GTC_IMSCR_X , FunctionalStatus newState);
extern void GTC_CMD(CSP_GTC_T *GTC , FunctionalStatus newState);
extern void GTC_SoftwareReset(CSP_GTC_T *GTC );
extern void GTC_start_stop(CSP_GTC_T *GTC );
extern U32_T GTC_Counter_Value(CSP_GTC_T *GTC);
extern U32_T GTC_Counter_PRDR_Value(CSP_GTC_T *GTC);
extern U32_T GTC_CaptureUp_Count_Value(CSP_GTC_T *GTC);
extern U32_T GTC_CaptureDown_Count_Value(CSP_GTC_T *GTC);
extern void GTC0_Int_Enable(void);
extern void GTC0_Int_Disable(void);
extern void GTC1_Int_Enable(void);
extern void GTC1_Int_Disable(void);
extern void GTC2_Int_Enable(void);
extern void GTC2_Int_Disable(void);
extern void GTC3_Int_Enable(void);
extern void GTC3_Int_Disable(void);
extern void GTC0_WakeUp_Enable(void);
extern void GTC0_WakeUp_Disable(void);
extern void GTC1_WakeUp_Enable(void);
extern void GTC1_WakeUp_Disable(void);
extern void GTC2_WakeUp_Enable(void);
extern void GTC2_WakeUp_Disable(void);
extern void GTC3_WakeUp_Enable(void);
extern void GTC3_WakeUp_Disable(void);
extern void GTC0_IO_Init(GTC_IO_MODE_TypeDef  GTC_IO_MODE_X , U8_T GTC_IO_G );
extern void GTC1_IO_Init(GTC_IO_MODE_TypeDef  GTC_IO_MODE_X , U8_T GTC_IO_G );
extern void GTC2_IO_Init(GTC_IO_MODE_TypeDef  GTC_IO_MODE_X , U8_T GTC_IO_G );
extern void GTC3_IO_Init(GTC_IO_MODE_TypeDef  GTC_IO_MODE_X , U8_T GTC_IO_G );
extern void GTC_Start(CSP_GTC_T *GTC);
extern void GTC_Stop(CSP_GTC_T *GTC);
extern void GTC_Set_Period(CSP_GTC_T *GTC , U32_T loadCounter_PRDR , U32_T loadCounter_PULR);
#endif   /* asm32s003_gtc_H */

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/