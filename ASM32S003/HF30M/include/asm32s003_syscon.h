/*
  ******************************************************************************
  * @file    asm32s003_syscon.h
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _asm32s003_syscon_H
#define _asm32s003_syscon_H

/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"

/******************************************************************************
************************* syscon Registers Definition *************************
******************************************************************************/
/** @addtogroup SYSCON Registers Reset Value
  * @{
  */
  
#define SYSCON_IDCCR_RST     	((CSP_REGISTER_T)0x00000001)
#define SYSCON_GCER_RST     	((CSP_REGISTER_T)0x00000000)
#define SYSCON_GCDR_RST     	((CSP_REGISTER_T)0x00000000)
#define SYSCON_GCSR_RST     	((CSP_REGISTER_T)0x00000003)
#define SYSCON_SCLKCR_RST      	((CSP_REGISTER_T)0x00000800)
#define SYSCON_PCLKCR_RST     	((CSP_REGISTER_T)0x00000100)
#define SYSCON_PCER0_RST   		((CSP_REGISTER_T)0x00000000)
#define SYSCON_PCDR0_RST  		((CSP_REGISTER_T)0x00000000)
#define SYSCON_PCSR0_RST  		((CSP_REGISTER_T)0x00000001)
#define SYSCON_PCER1_RST     	((CSP_REGISTER_T)0x00000000)
#define SYSCON_PCDR1_RST     	((CSP_REGISTER_T)0x00000000)
#define SYSCON_PCSR1_RST 		((CSP_REGISTER_T)0x00000000)
#define SYSCON_OSTR_RST  		((CSP_REGISTER_T)0x00FF03FF)
#define SYSCON_LVDCR_RST  		((CSP_REGISTER_T)0x0000CC01)
#define	SYSCON_IMFT_RST			((CSP_REGISTER_T)0x000001FF)
#define	SYSCON_PWRCR_RST		((CSP_REGISTER_T)0x00001F09)
//#define	SYSCON_OPT1_RST;		((CSP_REGISTER_T)0x00)
//#define	SYSCON_OPT2_RST;		((CSP_REGISTER_T)0x00)
#define	SYSCON_IECR_RST			((CSP_REGISTER_T)0x00000000)
#define	SYSCON_IDCR_RST			((CSP_REGISTER_T)0x00000000)
#define	SYSCON_IMSR_RST			((CSP_REGISTER_T)0x00000000)
#define	SYSCON_IAR_RST			((CSP_REGISTER_T)0x00000000)
#define	SYSCON_ICR_RST			((CSP_REGISTER_T)0x00000000)
#define	SYSCON_RISR_RST			((CSP_REGISTER_T)0x00000000)	
#define	SYSCON_ISR_RST			((CSP_REGISTER_T)0x00000000)
#define	SYSCON_RSR_RST			((CSP_REGISTER_T)0x00000000)	
#define	SYSCON_EXIRT_RST		((CSP_REGISTER_T)0x00000000)
#define	SYSCON_EXIFT_RST		((CSP_REGISTER_T)0x00000000)
#define	SYSCON_EXIER_RST		((CSP_REGISTER_T)0x00000000)
#define	SYSCON_EXIDR_RST		((CSP_REGISTER_T)0x00000000)
#define	SYSCON_EXIMR_RST		((CSP_REGISTER_T)0x00000000)
#define	SYSCON_EXIAR_RST		((CSP_REGISTER_T)0x00000000)
#define	SYSCON_EXICR_RST		((CSP_REGISTER_T)0x00000000)
#define	SYSCON_EXIRS_RST		((CSP_REGISTER_T)0x00000000)
#define	SYSCON_IWDCR_RST		((CSP_REGISTER_T)0x00007000)
#define	SYSCON_IWDCNT_RST		((CSP_REGISTER_T)0x0003FFFF)
//#define	SYSCON_IWDEDR_RST		((CSP_REGISTER_T)0x00)
//#define	SYSCON_CINF0_RST		((CSP_REGISTER_T)0x00)
//#define	SYSCON_CINF1_RST		((CSP_REGISTER_T)0x00)
//#define	SYSCON_FINF1_RST		((CSP_REGISTER_T)0x00)
#define	SYSCON_ERRINF_RST		((CSP_REGISTER_T)0x00000000)

//SCLKCR
#define SYSCLK_KEY 		(0xD22Dul<<16)

//PCLK CONTROL
#define PCLK_KEY 	 	(0xC33Cul<<16)

//IDCCR 
#define	CLKEN			(0X01ul)
#define	SWRST			(0X01ul<<7)
#define IDCCR_KEY 		(0xE11Eul<<16)

//LVDCR		
#define	LVDFLAG			(0x01ul<<15)					//0: VDD is higher than LVD threshold selected with INTDET_LVL bits.   1: VDD is lower than LVD threshold selected with INTDET_LVL bits
#define LVD_KEY 		(0xB44Bul<<16)

//IECR IEDR IAR ICR IMSR RISR ISR
//Interrupt Enable/Disable/Active/Clear Control Register
//Interrupt Masking/Raw Interrupt/Masked Status Register
#define ISOSC_ST 		(0x01ul)
#define IMOSC_ST 		(0x01ul<<1)
//#define ESOSC_ST 		(0x01ul<<2)
#define EMOSC_ST 		(0x01ul<<3)
#define SYSCLK_ST 		(0x01ul<<7)
#define	IWDT_INT_ST 	(0x01ul<<8)
#define	LVD_INT_ST 		(0x01ul<<11)
#define	EM_CMFAIL_ST 	(0x01ul<<18)
#define	EM_CMRCV_ST 	(0x01ul<<19)
#define	CMD_ERR_ST		(0x01ul<<29)

//RSR
//SYSCON Reset Status Register
#define	PORST			(0X01ul)
#define	LVRRST			(0X01ul<<1)
#define	EXTRST			(0X01ul<<2)
#define	IWDRST			(0X01ul<<4)
#define	EMCMRST			(0X01ul<<6)
#define	CPURSTREQ		(0X01ul<<7)
#define	SWRST_RSR		(0X01ul<<8)

//IWDCR
#define	Enable_SHORT_IWDT		(0X00ul)						//Configure IWDTCNT as 0x00040 when enabled. This is only for debug usage. Under this mode, the CNT value is fixed at 0x0040
#define	Disable_SHORT_IWDT		(0X01ul)
#define	Check_IWDT_BUSY			(0x01ul<<12)					//Indicates the independent watchdog operation
#define IWDT_KEY 				(0x8778ul<<16)

//IWDCNT
#define	CLR_IWDT				(0x5aul<<24)

//IWDEDR
#define	Enable_IWDT				(0x0)
#define	Disable_IWDT			(0x5555)
#define IWDTEDR_KEY 			(0x7887ul<<16)

/**
  * @brief  SYSCON General Control
  */
typedef enum
{
	ENDIS_ISOSC			=	 	(CSP_REGISTER_T)(0x01ul),
	ENDIS_IMOSC 		=		(CSP_REGISTER_T)(0x01ul<<1),
	ENDIS_EMOSC 		=		(CSP_REGISTER_T)(0x01ul<<3),
	ENDIS_IDLE_PCLK 	=		(CSP_REGISTER_T)(0x01ul<<8),
	ENDIS_SYSTICK 		=		(CSP_REGISTER_T)(0x01ul<<11),
	ENDIS_EM_CM 		=		(CSP_REGISTER_T)(0x01ul<<18),
	ENDIS_EM_CMRST 		=		(CSP_REGISTER_T)(0x01ul<<19),
}SYSCON_General_CMD_TypeDef;

/**
  * @brief  Selected SYSCON CLK
  */
typedef enum
{
	SYSCLK_IMOSC	=		(CSP_REGISTER_T)0x0ul,				//IMOSC  selected
	SYSCLK_EMOSC	=		(CSP_REGISTER_T)0x1ul,				//EMOSC  selected
	SYSCLK_ISOSC 	=		(CSP_REGISTER_T)0x4ul				//ISOSC	 selected
}SystemCLK_TypeDef;

/**
  * @brief  SYSCON CLK Div
  */
typedef enum
{
	HCLK_DIV_1   =		(CSP_REGISTER_T)(0x1ul<<8),				
	HCLK_DIV_2   =		(CSP_REGISTER_T)(0x2ul<<8),
	HCLK_DIV_3   =		(CSP_REGISTER_T)(0x3ul<<8),
	HCLK_DIV_4   =		(CSP_REGISTER_T)(0x4ul<<8),
	HCLK_DIV_5   =		(CSP_REGISTER_T)(0x5ul<<8),
	HCLK_DIV_6   =		(CSP_REGISTER_T)(0x6ul<<8),
	HCLK_DIV_7   =		(CSP_REGISTER_T)(0x7ul<<8),
	HCLK_DIV_8   =		(CSP_REGISTER_T)(0x8ul<<8),
	HCLK_DIV_12  =		(CSP_REGISTER_T)(0x9ul<<8),
	HCLK_DIV_16  =		(CSP_REGISTER_T)(0xAul<<8),
	HCLK_DIV_24  =		(CSP_REGISTER_T)(0xBul<<8),
	HCLK_DIV_32  =		(CSP_REGISTER_T)(0xCul<<8),
	HCLK_DIV_64  =		(CSP_REGISTER_T)(0xDul<<8),
	HCLK_DIV_128 =		(CSP_REGISTER_T)(0xEul<<8),
	HCLK_DIV_256 =		(CSP_REGISTER_T)(0xFul<<8)
}SystemCLK_Div_TypeDef;

/**
  * @brief  PCLK Div
  */
typedef enum
{
	PCLK_DIV_1   =		(CSP_REGISTER_T)(0x00ul<<8),				
	PCLK_DIV_2   =		(CSP_REGISTER_T)(0x01ul<<8),
	PCLK_DIV_4   =		(CSP_REGISTER_T)(0x02ul<<8),
	PCLK_DIV_8   =		(CSP_REGISTER_T)(0x04ul<<8),
	PCLK_DIV_16  =		(CSP_REGISTER_T)(0x08ul<<8)
}PCLK_Div_TypeDef;
  
/**
  * @brief  SYSON Peripheral clock enable and disable 
  */
//PCER0 PCDR0 PCSR0
typedef enum
{
	//PCXR0
	IFC_CLK 	=	(CSP_REGISTER_T)(0x01ul),
	ADC_CLK 	=	(CSP_REGISTER_T)(0x01ul<<4),
	ICSI_CLK	=	(CSP_REGISTER_T)(0x01ul<<5),
	TKEY_CLK	=	(CSP_REGISTER_T)(0x01ul<<6),
	UART0_CLK	=	(CSP_REGISTER_T)(0x01ul<<16),
	//UART1_CLK	=	(CSP_REGISTER_T)(0x01ul<<17),
	I2C_CLK		=	(CSP_REGISTER_T)(0x01ul<<22)
}SYSCON_PCLK0_CMD_TypeDef;

/**
  * @brief  SYSON Peripheral clock enable and disable 
  */
//PCER1 PCDR1 PCSR1
typedef enum
{
	//PCXR1
	GTC0_CLK 	=	(CSP_REGISTER_T)(0x01ul<<8),
	GTC1_CLK 	=	(CSP_REGISTER_T)(0x01ul<<9),
	GTC2_CLK 	=	(CSP_REGISTER_T)(0x01ul<<10),
	GTC3_CLK 	=	(CSP_REGISTER_T)(0x01ul<<11),
	CFG_CLK 	=	(CSP_REGISTER_T)(0x01ul<<13),
	LED_CLK 	=	(CSP_REGISTER_T)(0x01ul<<16),
	PIOA_CLK 	=	(CSP_REGISTER_T)(0x01ul<<24),
	PIOB_CLK 	=	(CSP_REGISTER_T)(0x01ul<<25)
}SYSCON_PCLK1_CMD_TypeDef;

/**
  * @brief  LVD enable and disable
  */
typedef enum
{
	ENABLE_LVDEN		=		(CSP_REGISTER_T)1,			//Power down LVD module
	DISABLE_LVDEN		=		(CSP_REGISTER_T)0			//Power down LVD module
}X_LVDEN_TypeDef;

/**
  * @brief  Detection voltage level to trigger the LVD interrupt
  */
typedef enum
{
	INTDET_LVL_2_5V		=	(CSP_REGISTER_T)(0X00ul<<8),					//2.5V	
	INTDET_LVL_3_0V		=	(CSP_REGISTER_T)(0X01ul<<8),					//3.0V
	INTDET_LVL_3_9V		=	(CSP_REGISTER_T)(0X02ul<<8),					//3.3V
	INTDET_LVL_4_1V		=	(CSP_REGISTER_T)(0X03ul<<8)						//4.1V
}INTDET_LVL_X_TypeDef;

/**
  * @brief  Detection voltage level to generate reset
  */
typedef enum
{
	RSTDET_LVL_2_2V		=		(CSP_REGISTER_T)(0X00ul<<12),			//2.2V
	RSTDET_LVL_2_7V		=		(CSP_REGISTER_T)(0X01ul<<12),			//2.7V
    RSTDET_LVL_3_3V		=		(CSP_REGISTER_T)(0X02ul<<12),			//3.3V
	RSTDET_LVL_3_6V		=		(CSP_REGISTER_T)(0X03ul<<12)			//3.6V
}RSTDET_LVL_X_TypeDef;

/**
  * @brief  Detection voltage level to trigger the LVD interrupt
  */
typedef enum
{
	ENABLE_LVD_INT		=		(CSP_REGISTER_T)(0X01ul<<11),			//ENABLE LVD INT
	DISABLE_LVD_INT		=		(CSP_REGISTER_T)(0X00ul<<11)			//DISABLE LVD INT
}X_LVD_INT_TypeDef;

/**
  * @brief  EXI PIN
  */
typedef enum
{
	EXI_PIN0		=		(CSP_REGISTER_T)(0X01ul),						
	EXI_PIN1		=		(CSP_REGISTER_T)(0X01ul<<1),
	EXI_PIN2		=		(CSP_REGISTER_T)(0X01ul<<2),
	EXI_PIN3		=		(CSP_REGISTER_T)(0X01ul<<3),
	EXI_PIN4		=		(CSP_REGISTER_T)(0X01ul<<4),
	EXI_PIN5		=		(CSP_REGISTER_T)(0X01ul<<5),
	EXI_PIN6		=		(CSP_REGISTER_T)(0X01ul<<6),
	EXI_PIN7		=		(CSP_REGISTER_T)(0X01ul<<7),
	EXI_PIN8		=		(CSP_REGISTER_T)(0X01ul<<8),
	EXI_PIN9		=		(CSP_REGISTER_T)(0X01ul<<9),
	EXI_PIN10		=		(CSP_REGISTER_T)(0X01ul<<10),
	EXI_PIN11		=		(CSP_REGISTER_T)(0X01ul<<11),
	EXI_PIN12		=		(CSP_REGISTER_T)(0X01ul<<12),
	EXI_PIN13		=		(CSP_REGISTER_T)(0X01ul<<13),
}SYSCON_EXIPIN_TypeDef;

/**
  * @brief  EXT register
  */
typedef enum
{
	_EXIRT		=		0,
	_EXIFT		=		1,	
}EXI_tringer_mode_TypeDef;

/**
  * @brief  SYSON WDT SHORT MODE enable and disable 
  */
typedef enum
{
	ENABLE_IWDT_SHORT	=		(CSP_REGISTER_T)1,
	DISABLE_IWDT_SHORT	=		(CSP_REGISTER_T)0	
}IWDT_SHORT_TypeDef;

/**
  * @brief  SYSON IWDT TIME SET
  */
typedef enum
{
	IWDT_TIME_128MS		=	(CSP_REGISTER_T)(0x00ul<<8),						//IWDT_TIME      0x00fff			
	IWDT_TIME_256MS		=	(CSP_REGISTER_T)(0x01ul<<8),						//IWDT_TIME      0x01fff
	IWDT_TIME_500MS		=	(CSP_REGISTER_T)(0x02ul<<8),						//IWDT_TIME      0x03fff	
	IWDT_TIME_1S		=	(CSP_REGISTER_T)(0x03ul<<8),						//IWDT_TIME      0x07fff
	IWDT_TIME_2S		=	(CSP_REGISTER_T)(0x04ul<<8),						//IWDT_TIME      0x0ffff			//2M ISOSC  2sec
	IWDT_TIME_3S		=	(CSP_REGISTER_T)(0x05ul<<8),						//IWDT_TIME      0x16fff
	IWDT_TIME_4S		=	(CSP_REGISTER_T)(0x06ul<<8),						//IWDT_TIME      0x1ffff
	IWDT_TIME_8S		=	(CSP_REGISTER_T)(0x07ul<<8)							//IWDT_TIME      0x3ffff
}IWDT_TIME_TypeDef;  

/**
  * @brief  SYSON IWDT TIME DIV SET
  */
typedef enum
{
	IWDT_INTW_DIV_7		=	(0x00ul<<2),						//7/8 of IWDT_TIME
	IWDT_INTW_DIV_6		=	(0x01ul<<2),						//6/8 of IWDT_TIME	
	IWDT_INTW_DIV_5		=	(0x02ul<<2),						//5/8 of IWDT_TIME
	IWDT_INTW_DIV_4		=	(0x03ul<<2),						//4/8 of IWDT_TIME
	IWDT_INTW_DIV_3		=	(0x04ul<<2),						//3/8 of IWDT_TIME
	IWDT_INTW_DIV_2		=	(0x05ul<<2),						//2/8 of IWDT_TIME
	IWDT_INTW_DIV_1		=	(0x06ul<<2)							//1/8 of IWDT_TIME
}IWDT_TIMEDIV_TypeDef;
typedef enum
{
	CLO_PA00		=	0,						//PA0.0 as clo
	CLO_PA08		=	1,						//PA0.8 as clo	
}CLO_IO_TypeDef;

/* 电源功耗控制 */
typedef enum
{
	POWER_DRIVE_SOTP_HIGH =	(0x00ul),	
	POWER_DRIVE_SOTP_LOW =	(0x01ul),	
	POWER_DRIVE_IDLE_HIGH =	(0x00ul),	
	POWER_DRIVE_IDLE_LOW =	(0x01ul<<1),
	POWER_DRIVE_SIDLE_HIGH =(0x00ul),	
	POWER_DRIVE_SIDLE_LOW =	(0x01ul<<2),	
	POWER_DRIVE_SUBOPT_EN =	(0x01ul<<3),	
	POWER_DRIVE_SUBOPT_DIS =(0x00ul)
}Power_Drive_TypeDef;
/** @addtogroup SYSCON_Exported_functions
  * @{
  */	
extern void SYSCON_init(void);
extern void SYSCON_RST_VALUE(void);
void SYSCON_General_CMD(FunctionalStatus NewState, SYSCON_General_CMD_TypeDef ENDIS_X );
extern void SystemCLK_HCLKDIV_PCLKDIV_Config(SystemCLK_TypeDef SYSCLK_X , SystemCLK_Div_TypeDef HCLK_DIV_X , PCLK_Div_TypeDef PCLK_DIV_X);
extern void SYSCON_PCLK1_CMD(SYSCON_PCLK1_CMD_TypeDef X_PCLK1 , FunctionalStatus NewState);
extern void SYSCON_PCLK0_CMD( SYSCON_PCLK0_CMD_TypeDef X_PCLK0 , FunctionalStatus NewState);
extern void SYSCON_WDT_CMD(FunctionalStatus NewState);
extern U32_T SYSCON_GET_IWDCNT(void );
extern B_T 	SYSCON_GET_IWDT_CLRBSY(void);
extern void SYSCON_IWDCNT_Reload(void);
extern void SYSCON_IWDCNT_Config(IWDT_SHORT_TypeDef NewStateE_IWDT_SHORT , IWDT_TIME_TypeDef IWDT_TIME_X , IWDT_TIMEDIV_TypeDef IWDT_INTW_DIV_X );
extern void SYSCON_LVD_Config(X_LVDEN_TypeDef X_LVDEN , INTDET_LVL_X_TypeDef INTDET_LVL_X , RSTDET_LVL_X_TypeDef RSTDET_LVL_X , X_LVD_INT_TypeDef X_LVD_INT);
extern U8_T SYSCON_LVDFLAG(void);	
extern U8_T SYSCON_POR_STATUS(void);
extern U8_T SYSCON_LVR_STATUS(void);
extern U8_T SYSCON_EXTRST_STATUS(void);
extern U8_T SYSCON_IWDRST_STATUS(void);
extern U8_T SYSCON_EMCMRST_STATUS(void);
extern U8_T SYSCON_CPURSTREQ_STATUS(void);
extern U8_T SYSCON_SWRST_RSR_STATUS(void);
extern void EXTI_trigger_CMD(FunctionalStatus NewState , SYSCON_EXIPIN_TypeDef  EXIPIN , EXI_tringer_mode_TypeDef EXI_tringer_mode);
extern void EXTI_interrupt_CMD(FunctionalStatus NewState , SYSCON_EXIPIN_TypeDef  EXIPIN);
extern void EXI0_Int_Enable(void);
extern void EXI0_Int_Disable(void);
extern void EXI1_Int_Enable(void);
extern void EXI1_Int_Disable(void);
extern void EXI2_Int_Enable(void);
extern void EXI2_Int_Disable(void);
extern void EXI3_Int_Enable(void);
extern void EXI3_Int_Disable(void);
extern void EXI4_Int_Enable(void);
extern void EXI4_Int_Disable(void);
extern void SYSCON_Int_Enable(void);
extern void SYSCON_Int_Disable(void);
extern void PCLK_goto_idle_mode(void);
extern void PCLK_goto_deepsleep_mode(void);
extern void LVD_Int_Enable(void);
extern void LVD_Int_Disable(void);
extern void IWDT_Int_Enable(void);
extern void IWDT_Int_Disable(void);
extern void EXI0_WakeUp_Enable(void);
extern void EXI0_WakeUp_Disable(void);
extern void EXI1_WakeUp_Enable(void);
extern void EXI1_WakeUp_Disable(void);
extern void EXI2_WakeUp_Enable(void);
extern void EXI2_WakeUp_Disable(void);
extern void EXI3_WakeUp_Enable(void);
extern void EXI3_WakeUp_Disable(void);
extern void EXI4_WakeUp_Enable(void);
extern void EXI4_WakeUp_Disable(void);
extern void SYSCON_WakeUp_Enable(void);
extern void SYSCON_WakeUp_Disable(void);
extern void I2C_SLAVE_CONFIG(void);
extern void GPIO_CONFIG();                                
extern void LED_CONFIG();                                   
extern void COUNTA_CONFIG();                              
extern void GTC0_CONFIG();                                
extern void GTC1_CONFIG();                     
extern void GTC2_CONFIG();                     
extern void GTC3_CONFIG();                        
extern void ADC12_CONFIG();                      
extern void I2C_MASTER_CONFIG();                                                   
extern void UART_CONFIG();  
extern void EPT_CONFIG();                      
extern void GPIO_EXTI_interrupt(CSP_GPIO_T * GPIOX,U32_T GPIO_IECR_VALUE);   
extern void SYSCON_PWRCR_CONFIG(Power_Drive_TypeDef PWRCR_dat);
extern void SYSCON_IMFT_CONFIG(U32_T IMFT_dat);
extern void SYSCON_CLO_CONFIG(CLO_IO_TypeDef clo_io);
extern U32_T Read_Reset_Status(void); 
extern U32_T SYSCON_Read_CINF0(void); 
extern U32_T SYSCON_Read_CINF1(void);
extern void SYSCON_Soft_Reset(void);
#endif   /**< asm32s003_syscon_H */

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/