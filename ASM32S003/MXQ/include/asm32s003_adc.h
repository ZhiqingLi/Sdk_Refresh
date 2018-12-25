/*
  ******************************************************************************
  * @file    asm32s003_adc.h
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _asm32s003_adc_H
#define _asm32s003_adc_H

/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"

/******************************************************************************
************************* ADC12 Registers Definition **************************
******************************************************************************/
/******************************************************************************
***************************** ADC12 Registers mask ***************************
******************************************************************************/
 #define ADC_ECR_MASK          (0x80000002ul)         /**< ECR  mask         */
 #define ADC_DCR_MASK          (0x80000002ul)         /**< DCR  mask         */
 #define ADC_PMSR_MASK         (0xBFFFFFF2ul)         /**< PMSR mask         */
 #define ADC_CR_MASK           (0x8000007Ful)         /**< CR   mask         */
 #define ADC_MR_MASK           (0x8FFFFC1Ful)         /**< MR   mask         */
 #define ADC_SR_MASK           (0x000003F7ul)         /**< SR   mask         */
 #define ADC_IER_MASK          (0x000000F7ul)         /**< IER  mask         */
 #define ADC_IDR_MASK          (0x000000F7ul)         /**< IDR  mask         */
 #define ADC_IMR_MASK          (0x000000F7ul)         /**< IMR  mask         */
 #define ADC_CMR0_MASK         (0x3F3F3F3Ful)         /**< CMR0  mask        */
 #define ADC_CMR1_MASK         (0x3F3F3F3Ful)         /**< CMR1  mask        */
 #define ADC_CMR2_MASK         (0x3F3F3F3Ful)         /**< CMR2  mask        */
 #define ADC_DR_MASK           (0x00000FFFul)         /**< DR   mask         */
 #define ADC_CMP1_MASK         (0x00000FFFul)         /**< CMP1 mask         */
 #define ADC_CMP2_MASK         (0x00000FFFul)         /**< CMP2 mask         */
/******************************************************************************
************************* ADC12 Registers reset value ************************
******************************************************************************/
 #define ADC_ECR_RST          (0x00000000ul)          /**< ECR  reset value  */
 #define ADC_DCR_RST          (0x00000000ul)          /**< DCR  reset value  */
 #define ADC_PMSR_RST         (0x00000000ul)   		  /**< PMSR reset value  */
 #define ADC_CR_RST           (0x80000040ul)          /**< CR   reset value  */
 #define ADC_MR_RST           (0x00000000ul)          /**< MR   reset value  */
 #define ADC_SR_RST           (0x00000000ul)          /**< SR   reset value  */
 #define ADC_IER_RST          (0x00000000ul)          /**< IER  reset value  */
 #define ADC_IDR_RST          (0x00000000ul)          /**< IDR  reset value  */
 #define ADC_IMR_RST          (0x00000000ul)          /**< IMR  reset value  */
 #define ADC_CMR0_RST         (0x00000000ul)          /**< CMR0  reset value */
 #define ADC_CMR1_RST         (0x00000000ul)          /**< CMR1  reset value */
 #define ADC_CMR2_RST         (0x00000000ul)          /**< CMR2  reset value */
 #define ADC_DR_RST           (0x00000000ul)          /**< DR   reset value  */
 #define ADC_CMP1_RST         (0x00000000ul)          /**< CMP1 reset value  */
 #define ADC_CMP2_RST         (0x00000000ul)          /**< CMP2 reset value  */
 
/**
  * @brief  ADC12 Control register
  */
typedef enum
{
	ADC12_SWRST     =		((CSP_REGISTER_T)(0x01ul << 0)),            /**< Software Reset        */
	ADC12_ADCEN     =		((CSP_REGISTER_T)(0x01ul << 1)),            /**< ADC Enable            */
	ADC12_ADCDIS    =		((CSP_REGISTER_T)(0x01ul << 2)),            /**< ADC Disable           */
	ADC12_START     =		((CSP_REGISTER_T)(0x01ul << 3)),            /**< Start Conversion      */
	ADC12_STOP      =		((CSP_REGISTER_T)(0x01ul << 4)),            /**< Stop Conversion       */
}ADC12_Control_TypeDef;

/**
  * @brief  ADC12 Bit slection
  */
typedef enum
{
	ADC12_12BIT     	=		1,           								/**< 12bit mode            */
	ADC12_10BIT    	 	=		0,           								/**< 10bit mode            */
	ADC12_10BITor12BIT	=		((CSP_REGISTER_T)(0x01ul<<31))
}ADC12_10bitor12bit_TypeDef;

/**
  * @brief  ADC12 sampling cycles selection
  */
typedef enum
{
	ADC12_3CYCLES   =	((CSP_REGISTER_T)(0x00ul << 5)),            /**< Sampling 3cycles     */
	ADC12_4CYCLES   =	((CSP_REGISTER_T)(0x01ul << 5)),            /**< Sampling 4cycles     */
	ADC12_6CYCLES   =	((CSP_REGISTER_T)(0x02ul << 5)),            /**< Sampling 6cycles     */
	ADC12_8CYCLES   =	((CSP_REGISTER_T)(0x03ul << 5))             /**< Sampling 8cycles     */
}ADC12_Sampleing_TypeDef;

/**
  * @brief  ADC12 Convertion mode
  */
typedef enum
{
	One_shot_mode	=	0,
	Continuous_mode =	1,
	CONTCV			=	(CSP_REGISTER_T)0x01<<31					//Continuous Conversion	 0: One shot mode.  1: Continuous mode.
}ADC12_ConverMode_TypeDef;

/**
  * @brief  ADC12 CMRx register
  */
typedef enum
{
	 ADC12_ADCIN0       =	(CSP_REGISTER_T)(0x0ul),                   /**< ADC Analog Input 0 */
	 ADC12_ADCIN1       =	(CSP_REGISTER_T)(0x1ul),                    /**< ADC Analog Input 1 */
	 ADC12_ADCIN2       =	(CSP_REGISTER_T)(0x2ul),                   /**< ADC Analog Input 2 */
	 ADC12_ADCIN3       =	(CSP_REGISTER_T)(0x3ul),                    /**< ADC Analog Input 3 */
	 ADC12_ADCIN4       =	(CSP_REGISTER_T)(0x4ul),                   /**< ADC Analog Input 4 */
	 ADC12_ADCIN5       =	(CSP_REGISTER_T)(0x5ul),                    /**< ADC Analog Input 5 */
	 ADC12_ADCIN6       =	(CSP_REGISTER_T)(0x6ul),                    /**< ADC Analog Input 6 */
	 ADC12_ADCIN7       =	(CSP_REGISTER_T)(0x7ul),                    /**< ADC Analog Input 7 */
	 ADC12_ADCIN8       =	(CSP_REGISTER_T)(0x8ul),                   /**< ADC Analog Input 8 */
	 ADC12_ADCIN9       =	(CSP_REGISTER_T)(0x9ul),                    /**< ADC Analog Input 9 */
	 ADC12_ADCIN10      =	(CSP_REGISTER_T)(0x0Aul),                   /**< ADC Analog Input 10 */
	 ADC12_ADCIN11      =	(CSP_REGISTER_T)(0x0Bul),                    /**< ADC Analog Input 11 */
	 ADC12_ADCIN12      =	(CSP_REGISTER_T)(0x0Cul)                    /**< ADC Analog Input 12 */
}
ADC12_InputSet_TypeDef;

/**
  * @brief  ADC12 IMR register 
  */
typedef enum
{
	//CSR SR, IER, IDR, IMR Registers
	ADC12_EOC        =		((CSP_REGISTER_T)(0x01ul << 0)),     /**< End Of Conversion           */
	ADC12_READY      =		((CSP_REGISTER_T)(0x01ul << 1)),     /**< Ready to Start              */
	ADC12_OVR        =		((CSP_REGISTER_T)(0x01ul << 2)),     /**< Over Run                    */
	ADC12_CMP0H      =		((CSP_REGISTER_T)(0x01ul << 4)),     /**< Higher than CMP1            */
	ADC12_CMP0L      =		((CSP_REGISTER_T)(0x01ul << 5)),     /**< Lower than CMP1             */
	ADC12_CMP1H      =		((CSP_REGISTER_T)(0x01ul << 6)),     /**< Higher than CMP2            */
	ADC12_CMP1L      =		((CSP_REGISTER_T)(0x01ul << 7)),     /**< Lower than CMP2              */
	/* SR Register Only                                                          */
	ADC12_ADCENS     =		((CSP_REGISTER_T)(0x01ul << 8)),     /**< ADC Enable Status           */
	ADC12_CTCVS      =		((CSP_REGISTER_T)(0x01ul << 9))      /**< Continuous Conversion Status*/
}ADC12_IMR_TypeDef;
/**
  * @brief  ADC12 NBRCMPx selection 
  */
typedef enum
{
	NBRCMP0_TypeDef		=	0,
	NBRCMP1_TypeDef		=	1
}ADC12_NBRCMPx_TypeDef;
/**
  * @brief  ADC12 NBRCMPx_HorL selection 
  */
typedef enum
{
	NBRCMPX_L_TypeDef	=	0,
	NBRCMPX_H_TypeDef	=	1
}ADC12_NBRCMPx_HorL_TypeDef;

/**
  * @brief  ADC12 CLK ENABLE AND DISABLE 
  */
  //ECR
typedef enum
{
	ADC_CLK_CR 			=		((CSP_REGISTER_T)(0x01ul << 1)),				/**< ADC Clock             */
	ADC12_IPIDCODE_MASK =		((CSP_REGISTER_T)(0x3FFFFFFul << 4)),			/**< ADC IPIDCODE mask     */
	ADC_DEBUG_MODE		=		((CSP_REGISTER_T)(0x01ul << 31))				/**< Debug Mode Enable     */
}ADC12_CLK_TypeDef;

#define ADC12_CV(cv, val) (((val) & 0x3Ful) << (((cv) & 0x3ul) << 3))
#define CSP_ADC12_SET_CMRx(index, val) ((adc)->CMR[index] = (val))
/******************************************************************************
********************* ADC12 External Functions Declaration ********************
******************************************************************************/
extern void ADC12_Init(void);
extern void ADC12_RESET_VALUE(void);
extern void ADC12_MASK_VALUE(void);
extern U32_T ADC12_read_IPIDCODE(void);
extern void ADC12_Control(ADC12_Control_TypeDef ADC12_Control_x );
extern void ADC12_ConfigInterrupt_CMD( ADC12_IMR_TypeDef ADC_IMR_X , FunctionalStatus NewState);
extern void ADC12_CLK_CMD(ADC12_CLK_TypeDef ADC_CLK_CMD , FunctionalStatus NewState);
extern void ADC12_Software_Reset(void);
extern void ADC12_CMD(FunctionalStatus NewState);
extern void ADC12_ready_wait(void);  
extern void ADC12_EOC_wait(void);
extern U32_T ADC12_DATA_OUPUT(void);
extern void ADC12_Configure_Mode(ADC12_10bitor12bit_TypeDef ADC12_BIT_SELECTED  , ADC12_Sampleing_TypeDef ADC12_Sample_SELECTED , ADC12_ConverMode_TypeDef  ADC12_ConverMode  , U8_T ADC12_DIV , U8_T NumConver ); 
extern void ADC12_ConversionChannel_Config(ADC12_InputSet_TypeDef ADC12_ADCINX,U8_T CVx);
extern void ADC12_CompareFunction_set(U8_T ConverNum_CM0, U8_T ConverNum_CM1 , U32_T CMP0_data , U32_T CMP1_data ); 
extern U8_T ADC12_Compare_statue(ADC12_NBRCMPx_TypeDef NBRCMPx_TypeDef , ADC12_NBRCMPx_HorL_TypeDef NBRCMPX_L_TypeDef);
extern void ADC_Int_Enable(void);
extern void ADC_Int_Disable(void);
extern void ADC_Wakeup_Enable(void);
extern void ADC_Wakeup_Disable(void);
#endif   /**< asm32s003_adc_H */

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/