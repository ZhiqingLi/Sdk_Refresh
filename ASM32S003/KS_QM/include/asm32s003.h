/*
  ******************************************************************************
  * @file    asm32s003.h
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/

  
/* Define to prevent recursive inclusion -------------------------------------*/ 
#ifndef _asm32s003_H
#define _asm32s003_H

/* Includes ------------------------------------------------------------------*/
#include "asm32s003_types_local.h"
#include "asm32s003_ck802.h"

/******************************************************************************/
/*                          IP registers structures                           */
/******************************************************************************/
/**
  * @brief  CPU Internal Structure
  */
typedef struct {
	unsigned int ReservedA[4];		//0xE000E000
	unsigned int CORET_CSR;			//0xE000E010
	unsigned int CORET_RVR;			//0xE000E014
	unsigned int CORET_CVR;			//0xE000E018
	unsigned int CORET_CALIB;		//0xE000E01C
	unsigned int ReservedB[56];		//0xE000E020
	unsigned int ISER;				//0xE000E100
	unsigned int ReservedC[15];		//
	unsigned int IWER;				//0xE000E140
	unsigned int ReservedD[15];		//
	unsigned int ICER;				//0xE000E180
	unsigned int ReservedE[15];		//
	unsigned int IWDR;				//0xE000E1C0
	unsigned int ReservedF[15];		//
	unsigned int ISPR;				//0xE000E200
	unsigned int ReservedG[31];		//
	unsigned int ICPR;				//0xE000E280
	unsigned int ReservedH[31];		//
	unsigned int IABR;				//0xE000E300
	unsigned int ReservedI[63];		//
	unsigned int IPR[8];			//0xE000E400 ~ 0xE000E41C
	unsigned int ReservedJ[504];	//
	unsigned int ISR; 				//0xE000EC00
	unsigned int IPTR; 				//0xE000EC04
} CSP_CK802_T;

/**
  * @brief  IFC Structure
  */
typedef volatile struct 
{
        CSP_REGISTER_T	IDR ;
        CSP_REGISTER_T	CEDR ;
        CSP_REGISTER_T	SRR ;
        CSP_REGISTER_T	CMR ;
        CSP_REGISTER_T	CR ;
        CSP_REGISTER_T	MR ;
        CSP_REGISTER_T	PF_AR ;
        CSP_REGISTER_T	PF_DR ;
        CSP_REGISTER_T	KR ;
        CSP_REGISTER_T	ICR ;
        CSP_REGISTER_T	RISR ;
        CSP_REGISTER_T	MISR ;
		CSP_REGISTER_T	ICLR ;
} CSP_IFC_T ;

/**
  * @brief  SYSCON Structure
  */
typedef volatile struct 
{                            /*!< SYSCON Structure                         */
		 CSP_REGISTER_T	IDCCR;                        /*!< Identification & System Controller Clock Control Register */
		 CSP_REGISTER_T	GCER;                         /*!< System Controller General Control Enable Register */
		 CSP_REGISTER_T GCDR;                         /*!< System Controller General Control Disable Register */
		 CSP_REGISTER_T GCSR;                         /*!< System Controller General Control Status Register */
		 CSP_REGISTER_T _RSVD0;                       /*!< System Controller Power Control Enable Register */
		 CSP_REGISTER_T _RSVD1;                       /*!< System Controller Power Control Disable Register */
		 CSP_REGISTER_T _RSVD2;                       /*!< System Controller Power Control Status Register */
		 CSP_REGISTER_T SCLKCR;                       /*!< System Controller System Clock Selection & Division Register */
		 CSP_REGISTER_T PCLKCR;                       /*!< System Controller Peripheral Clock Selection & Division Register */
		 CSP_REGISTER_T _RSVD3;                       /*!< System Controller Peripheral Function Clock Selection & Division Register */
		 CSP_REGISTER_T PCER0;                        /*!< System Controller Peripheral Clock Enable Register */
		 CSP_REGISTER_T PCDR0;                        /*!< System Controller Peripheral Clock Disable Register */
		 CSP_REGISTER_T PCSR0;                        /*!< System Controller Peripheral Clock Status Register */
		 CSP_REGISTER_T PCER1;                        /*!< System Controller Peripheral Clock Enable Register */
		 CSP_REGISTER_T PCDR1;                        /*!< System Controller Peripheral Clock Disable Register */
		 CSP_REGISTER_T PCSR1;                        /*!< System Controller Peripheral Clock Status Register */
		 CSP_REGISTER_T OSTR;                         /*!< System Controller External OSC Stable Time Control Register */
		 CSP_REGISTER_T PSTR;                         /*!< System Controller PLL Stable Time Control Register */
		 CSP_REGISTER_T PLLCR;                        /*!< System Controller PLL PMS Value Control Register */
		 CSP_REGISTER_T LVDCR;                        /*!< System Controller LVD Control Register */
		 CSP_REGISTER_T IMFT;                         /*!< System Controller Wake-up Time Control Register */
		 CSP_REGISTER_T	PWRCR;                       /*!< System Controller Power Stable Time Control Register */
		 CSP_REGISTER_T OPT1;                         /*!< System Controller OSC Trim Control Register */
		 CSP_REGISTER_T OPT0;                         /*!< System Controller Protection Control Register */
		 CSP_REGISTER_T CQCR;                         /*!< System Controller Clock Quality Check Control Register */
		 CSP_REGISTER_T CQSR;                         /*!< System Controller Clock Quality Check Control Register */
		 CSP_REGISTER_T IECR;                         /*!< System Controller Interrupt Enable Register */
		 CSP_REGISTER_T IDCR;                         /*!< System Controller Interrupt Disable Register */
		 CSP_REGISTER_T IMSR;                         /*!< System Controller Interrupt Mask Register */
		 CSP_REGISTER_T IAR;                          /*!< System Controller Interrupt Active Register */
		 CSP_REGISTER_T ICR;                          /*!< System Controller Clear Status Register */
		 CSP_REGISTER_T RISR;                         /*!< System Controller Raw Interrupt Status Register */
		 CSP_REGISTER_T ISR;                          /*!< System Controller Raw Interrupt Status Register */
		 CSP_REGISTER_T RSR;                          /*!< System Controller Raw Interrupt Status Register */
		 CSP_REGISTER_T EXIRT;                        /*!< System Controller Reset Status Register */
		 CSP_REGISTER_T EXIFT;                        /*!< System Controller External Interrupt Mode 1 (Positive Edge) Register */
		 CSP_REGISTER_T EXIER;                        /*!< System Controller External Interrupt Mode 2 (Negative Edge) Register */
		 CSP_REGISTER_T EXIDR;                        /*!< System Controller External Interrupt Enable Register */
		 CSP_REGISTER_T EXIMR;                        /*!< System Controller External Interrupt Disable Register */
		 CSP_REGISTER_T EXIAR;                        /*!< System Controller External Interrupt Mask Register */
		 CSP_REGISTER_T EXICR;                        /*!< System Controller External Interrupt Active Register */
		 CSP_REGISTER_T EXIRS;                        /*!< System Controller External Interrupt Clear Status Register */
		 CSP_REGISTER_T IWDCR;                        /*!< System Controller Independent Watchdog Control Register */
		 CSP_REGISTER_T IWDCNT;                       /*!< System Controller Independent Watchdog Counter Value Register */
		 CSP_REGISTER_T IWDEDR;                       /*!< System Controller Independent Watchdog Enable/disable Register*/
		 CSP_REGISTER_T CINF0;                        /*!< Customer Information Content mirror of 1st byte*/
		 CSP_REGISTER_T CINF1;                        /*!< Customer Information Content mirror of 1st byte*/
		 CSP_REGISTER_T FINF0;                        /*!< Customer Information Content mirror of 1st byte*/
		 CSP_REGISTER_T FINF1;                        /*!< Customer Information Content mirror of 1st byte*/
		 CSP_REGISTER_T _RSVD6;                       /*! @0C4*/
		 CSP_REGISTER_T _RSVD7;                       /*! @0C8*/
		 CSP_REGISTER_T _RSVD8;                       /*! @0CC*/
		 CSP_REGISTER_T _RSVD9;                       /*! @0D0*/
		 CSP_REGISTER_T _RSVD10;                      /*! @0D4*/
		 CSP_REGISTER_T _RSVD11;                      /*! @0D8*/
		 CSP_REGISTER_T _RSVD12;                      /*! @0DC*/
		 CSP_REGISTER_T ERRINF;                       /*!< System Controller Error Information */
		 CSP_REGISTER_T _RSVD13;                      /*! @0E4*/
		 CSP_REGISTER_T _RSVD14;                      /*! @0E8*/
		 CSP_REGISTER_T _RSVD15;                      /*! @0EC*/
		 CSP_REGISTER_T _RSVD16;                      /*! @0F0*/
		 CSP_REGISTER_T _RSVD17;                      /*! @0F4*/
		 CSP_REGISTER_T _RSVD18;                      /*! @0F8*/
} CSP_SYSCON_T;

typedef enum 
{
		ISOSC = 0,
		IMOSC,
		ESOSC,
		EMOSC,
		PLL
} OSCTYPE;

/**
  * @brief  TOUCH KEY Structure
  */
typedef volatile struct
{
	   CSP_REGISTER_T	TCH_CR0;               /* Control Register */
	   CSP_REGISTER_T  	TCH_CR1;               /* Control Register */
	   CSP_REGISTER_T  	TCH_HWPCR0;            /* HWP Control Register */
	   CSP_REGISTER_T  	TCH_HWPCR1;            /* HWP Control Register */
	   CSP_REGISTER_T  	TCH_BLFUCR;            /* Baseline Force Updating Control Register*/
	   CSP_REGISTER_T  	TCH_START;             /* Start Register */
	   CSP_REGISTER_T  	TCH_SCTCR;             /* Scanning Time Limitation */
	   CSP_REGISTER_T  	TCH_CHDST;             /* Touch Disable Channel Status Register */
	   CSP_REGISTER_T  	TCH_TSCCR;             /* Touch Sensing Counter Config Register */
	   CSP_REGISTER_T  	TCH_TSCDR;             /* Touch Sensing Counter Data Register */
	   CSP_REGISTER_T  	TCH_CHCFG;             /* Channel Config Register */
	   CSP_REGISTER_T  	TCH_TSR;               /* Touch Sensitivity Selection Register */
	   CSP_REGISTER_T  	TCH_GSR;               /* Touch Global Sensitivity Control Reg */
	   CSP_REGISTER_T  	TCH_OSCR0;             /* Offset Threshold Register */
	   CSP_REGISTER_T  	TCH_OSCR1;             /* Offset Threshold Register */
	   CSP_REGISTER_T  	TCH_OSCR2;             /* Offset Threshold Register */
	   CSP_REGISTER_T  	TCH_OSCR3;             /* Offset Threshold Register */
	   CSP_REGISTER_T  	TCH_TKCR;
	   CSP_REGISTER_T  	TCH_RISR;             /* Raw Interrupt Status Register */
	   CSP_REGISTER_T  	TCH_IMCR;             /* Interrupt Masking Control Register */
	   CSP_REGISTER_T  	TCH_MISR;             /* Masked Interrupt Status Register */
	   CSP_REGISTER_T  	TCH_ICR;              /* Interrupt Clear Register */
	   CSP_REGISTER_T  	TCH_CHxCNT[16];        /* Channel Counter Register */
	   CSP_REGISTER_T  	TCH_CHxBL[16];         /* Channel Baseline Register */
	   CSP_REGISTER_T  	TCH_TKEYST;            /* TKEY Status Register */
	   CSP_REGISTER_T  	TCH_BLUPINF;           /* Baseline update channel information */
} CSP_TKEY_T, *CSP_TKEY_PTR; 

/**
  * @brief  ADC Structure
  */
 typedef volatile struct
 {
		CSP_REGISTER_T	ReservedA[20];
		CSP_REGISTER_T  ECR;              /**< Clock Enable Register             */
		CSP_REGISTER_T  DCR;              /**< Clock Disable Register            */
		CSP_REGISTER_T  PMSR;             /**< Power Management Status Register  */
		CSP_REGISTER_T  ReservedB;
		CSP_REGISTER_T  CR;               /**< Control Register                  */
		CSP_REGISTER_T  MR;               /**< Mode Register                     */
		CSP_REGISTER_T  ReservedC;
		CSP_REGISTER_T  CSR;              /**< Clear Status Register             */
		CSP_REGISTER_T  SR;               /**< Status Register                   */
		CSP_REGISTER_T  IER;              /**< Interrupt Enable Register         */
		CSP_REGISTER_T  IDR;              /**< Interrupt Disable Register        */
		CSP_REGISTER_T  IMR;              /**< Interrupt Mask Register           */
		CSP_REGISTER_T  CMR[10];          /**< Conversion Mode Register 0~9      */
		CSP_REGISTER_T  ReservedD[22];
		CSP_REGISTER_T  DR;               /**< Convert Data Register             */
		CSP_REGISTER_T  CMP0;             /**< Comparison Data Register             */
		CSP_REGISTER_T  CMP1;             /**< Comparison Data Register             */
 } CSP_ADC12_T, *CSP_ADC12_PTR;
 
/**
  * @brief  GPIO Structure
  */
 typedef volatile struct
 {
		CSP_REGISTER_T  CONLR;                /**< Control Low  Register                */
		CSP_REGISTER_T  CONHR;                /**< Control High Register                */
		CSP_REGISTER_T  WODR;                 /**< Write Output Data Register           */              
		CSP_REGISTER_T  SODR;                 /**< Set Output Data (bit-wise) Register  */
		CSP_REGISTER_T  CODR;                 /**< Clear Output Data (bit-wise) Register*/
		CSP_REGISTER_T  ODSR;                 /**< Output Data Status Register          */
		CSP_REGISTER_T  PSDR;                 /**< Pin Data Status Register             */
		CSP_REGISTER_T  ReservedA;         
		CSP_REGISTER_T  PUDR;                 /**< IO Pullup_Pulldown Register          */
		CSP_REGISTER_T  DSCR;                 /**< Output Driving Strength Register     */
		CSP_REGISTER_T  OMCR;                 /**< Slew-rate, Open-Drain Control        */
		CSP_REGISTER_T  IECR;                	/**< EXI enable control                   */
		CSP_REGISTER_T  IGRP;	                /**< EXI group control                    */
 } CSP_GPIO_T, *CSP_GPIO_PTR; 
/**
  * @brief  GPIO Structure
  */
 typedef volatile struct
 {
		CSP_REGISTER_T  IGRP;	                /**< EXI group control                    */	
 } CSP_GPIOGRP_T, *CSP_GPIOGRP_PTR; 

/**
  * @brief  GTC Structure
  */
 typedef struct
 {
		CSP_REGISTER_T	IDR;		/**< ID Register	              */
		CSP_REGISTER_T	CSSR;        	/**< Clock Source Selection Register  */
		CSP_REGISTER_T	CEDR;        	/**< Clock Enable/Disable Register    */
		CSP_REGISTER_T	SRR;         	/**< Software Reset Register          */
		CSP_REGISTER_T	CSR;        	/**< Control Set Register             */
		CSP_REGISTER_T	CCR;        	/**< Control Clear Register           */
		CSP_REGISTER_T	SR;             /**< Status Register                  */
		CSP_REGISTER_T	IMSCR;       	/**< Interrupt Mask Set/Clear Register*/
		CSP_REGISTER_T	RISR;        	/**< Raw Interrupt Status Register    */
		CSP_REGISTER_T	MISR;        	/**< Masked Interrupt Status Register */
		CSP_REGISTER_T	ICR;        	/**< Interrupt Clear Register         */
		CSP_REGISTER_T	CDR;        	/**< Clock Divider Register           */
		CSP_REGISTER_T	CSMR;        	/**< Count Size Mask Register         */
		CSP_REGISTER_T	PRDR;           /**< Period Register                  */
		CSP_REGISTER_T	PULR;           /**< Pulse Register                   */
		CSP_REGISTER_T	UCDR;           /**< Updated Clock  Register*/
		CSP_REGISTER_T	UCSMR;           /**< Updated Counter Size Mask Register*/
		CSP_REGISTER_T	UPRDR;           /**< Updated Period Register*/
		CSP_REGISTER_T	UPULR;           /**< Updated Counter Pulse Register*/
		CSP_REGISTER_T	CUCR;           /**< Capture Up Count Register        */
		CSP_REGISTER_T	CDCR;           /**< Capture Down Count Register      */
		CSP_REGISTER_T	CVR;            /**< Counter Value Register           */
 } CSP_GTC_T, *CSP_GTC_PTR;

/**
  * @brief  COUNTA Structure
  */
 typedef struct
 {
		CSP_REGISTER_T	CADATAH;        /**< DATA High Register            */
		CSP_REGISTER_T	CADATAL;        /**< DATA Low Register             */
		CSP_REGISTER_T	CACON;          /**< Control Register              */
		CSP_REGISTER_T	INTMASK;        /**< Interrupt Mask CR             */
 } CSP_CA_T, *CSP_CA_PTR;

/**
  * @brief  UART Structure
  */
 typedef volatile struct
 {
		CSP_REGISTER_T  DATA;               /**< Write and Read Data Register    */
		CSP_REGISTER_T  SR;                 /**< Status Register                 */
		CSP_REGISTER_T  CTRL;                /**< Control Register                */
		CSP_REGISTER_T  ISR;                /**< Interrupt Status Register       */
		CSP_REGISTER_T  BRDIV;               /**< Baud Rate Generator Register    */
		CSP_REGISTER_T  ReservedA[20];
 } CSP_UART_T, *CSP_UART_PTR;

/**
  * @brief  LED Structure
  */	
 typedef struct
 {
		CSP_REGISTER_T	CR;             /**< Control Register                 */
		CSP_REGISTER_T  BRIGHT;         /**< Brightness Control Register      */
		CSP_REGISTER_T  RISR;           /**< Raw Interrupt Status Register    */
		CSP_REGISTER_T	IMCR;           /**< Interrupt Masking Control Reg.   */
		CSP_REGISTER_T	MISR;           /**< Masked Interrupt Status Register */
		CSP_REGISTER_T	ICR;            /**< Interrupt Clear Register         */
		CSP_REGISTER_T	SEGDATA0;       /**< Segment Data0                    */
		CSP_REGISTER_T	SEGDATA1;       /**< Segment Data1                    */
		CSP_REGISTER_T	SEGDATA2;       /**< Segment Data2                    */
		CSP_REGISTER_T	SEGDATA3;       /**< Segment Data3                    */
		CSP_REGISTER_T	KEYST;          /**< Key Status                       */
		CSP_REGISTER_T	TIMCR;          /**< Timing Control                   */
 } CSP_LED_T, *CSP_LED_PTR;

/**
  * @brief  I2C Structure
  */			
 typedef volatile struct
 {
		CSP_REGISTER_T  ReservedA[20];
		CSP_REGISTER_T  ECR;           /* Enable Clock Register                  */
		CSP_REGISTER_T  DCR;           /* Disable Clock Register                 */
		CSP_REGISTER_T  PMSR;          /* Power Management Status Register       */
		CSP_REGISTER_T  ReservedD;     
		CSP_REGISTER_T  CR;            /* Control Register                       */
		CSP_REGISTER_T  MR;            /* Mode Register                          */
		CSP_REGISTER_T  ReservedE[2];
		CSP_REGISTER_T  SR;            /* Status Register                        */
		CSP_REGISTER_T  IER;           /* Interrupt Enable Register              */
		CSP_REGISTER_T  IDR;           /* Interrupt Enable Register              */
		CSP_REGISTER_T  IMR;           /* Interrupt Enable Register              */
		CSP_REGISTER_T  DAT;           /* Serial Data Register                   */
		CSP_REGISTER_T  ADR;           /* Serial Slave Address Register          */
		CSP_REGISTER_T  THOLD;         /* Hold/Setup Delay Register              */
 } CSP_I2C_T, *CSP_I2C_PTR;

//--Flash Address Setting
 #define FLASHBase 0x00000000
 #define FLASHSize 0x00008000
 #define FLASHLimit (FLASHBase + FLASHSize) 
 #define DFLASHBase 0x80000000
 #define DFLASHSize 0x80001000
 #define DFLASHLimit (FLASHBase + FLASHSize) 

//--SRAM Address Setting
#ifdef REMAP
  #define SRAMBase 0x00000000
  #define SRAMSize 0x00000800
  #define SRAMLimit (SRAMBase + SRAMSize) 
  #define MEMVectorBase 0x00000700
  #define MEMVectorSize (0x50<<2)
#else
  #define SRAMBase 0x20000000
  #define SRAMSize 0x00000800
  #define SRAMLimit (SRAMBase + SRAMSize) 
  #define MEMVectorBase 0x20000700
  #define MEMVectorSize (0x50<<2)
#endif

//--Peripheral Address Setting
#define APBPeriBase     0x40000000 
//--Each Peripheral Address Setting
#define APB_SFMBase     (APBPeriBase + 0x10000)
#define APB_IFCBase    	(APBPeriBase + 0x11000)
#define APB_SYSCONBase  (APBPeriBase + 0x12000)

#define APB_TKEYBase    (APBPeriBase + 0x20000)
#define APB_ADC0Base    (APBPeriBase + 0x30000)

#define APB_GPIOA0Base  (APBPeriBase + 0x40000) //A0  
#define APB_GPIOB0Base  (APBPeriBase + 0x41000) //B0 
#define APB_GPIOC0Base  (APBPeriBase + 0x42000) //C0 

#define APB_GPIOGRPBase    (APBPeriBase + 0x44000)

#define APB_GTC0Base    (APBPeriBase + 0x50000)
#define APB_GTC1Base    (APBPeriBase + 0x51000)
#define APB_GTC2Base    (APBPeriBase + 0x52000)
#define APB_GTC3Base    (APBPeriBase + 0x53000)

#define APB_LED0Base    (APBPeriBase + 0x60000)
#define APB_CNTABase    (APBPeriBase + 0x70000)

#define APB_UART0Base   (APBPeriBase + 0x80000)
#define APB_UART1Base   (APBPeriBase + 0x81000)
#define APB_I2C0Base    (APBPeriBase + 0xA0000)

//#define CK802_BASEADDR        	((unsigned int) 0xE000E000)
#define CK802_BASEADDR        	0xE000E000

#define INTC_ISER     CK802_BASEADDR+0x100    //INTC interrupt enable register
#define INTC_IWER     CK802_BASEADDR+0x140    //INTC wake-up interrupt enable register
#define INTC_ICER     CK802_BASEADDR+0x180    //INTC interrupt enable clear register
#define INTC_IWDR     CK802_BASEADDR+0x1C0    //INTC wake-up interrupt enable clear register
#define INTC_ISPR     CK802_BASEADDR+0x200    //INTC interrupt pending register
#define INTC_ICPR     CK802_BASEADDR+0x280    //INTC interrupt pending clear register
#define INTC_IABR     CK802_BASEADDR+0x300    //INTC interrupt acknowledge status register
#define INTC_IPR0     CK802_BASEADDR+0x400    //INTC interrupt priority register
#define INTC_IPR1     CK802_BASEADDR+0x404    //INTC interrupt priority register
#define INTC_IPR2     CK802_BASEADDR+0x408    //INTC interrupt priority register
#define INTC_IPR3     CK802_BASEADDR+0x40C    //INTC interrupt priority register
#define INTC_IPR4     CK802_BASEADDR+0x410    //INTC interrupt priority register
#define INTC_IPR5     CK802_BASEADDR+0x414    //INTC interrupt priority register
#define INTC_IPR6     CK802_BASEADDR+0x418    //INTC interrupt priority register
#define INTC_IPR7     CK802_BASEADDR+0x41C    //INTC interrupt priority register
#define INTC_ISR      CK802_BASEADDR+0xC00    //INTC interrupt status register
#define INTC_IPTR     CK802_BASEADDR+0xC04    //INTC interrupt pending threshold register

//--Interrupt Bit Position
#define EPT_INT     (0x01ul<<0)       //IRQ0
#define SYSCON_INT  (0x01ul<<1)       //IRQ1
#define IFC_INT     (0x01ul<<2)       //IRQ2
#define ADC_INT     (0x01ul<<3)       //IRQ3
#define GTC0_INT    (0x01ul<<4)       //IRQ4
#define GTC1_INT    (0x01ul<<5)       //IRQ5
#define GTC2_INT    (0x01ul<<6)		  //IRQ6
#define EXI0_INT    (0x01ul<<7)       //IRQ7
#define EXI1_INT    (0x01ul<<8)       //IRQ8
#define GTC3_INT    (0x01ul<<9)       //IRQ9
#define UART0_INT   (0x01ul<<13)      //IRQ13
#define UART1_INT   (0x01ul<<14)      //IRQ14
#define I2C_INT     (0x01ul<<17)      //IRQ17
#define EXI2_INT    (0x01ul<<21)      //IRQ21
#define EXI3_INT    (0x01ul<<22)      //IRQ22
#define EXI4_INT    (0x01ul<<23)      //IRQ23
#define CA_INT      (0x01ul<<24)      //IRQ24
#define TKEY0_INT   (0x01ul<<25)      //IRQ25
#define TKEY1_INT   (0x01ul<<26)      //IRQ26
#define LED_INT     (0x01ul<<27)      //IRQ27

//--External Structure pointer variable declarations
extern CSP_CK802_T  *CK802    ;
extern CSP_IFC_T 	*IFC      ;
extern CSP_SYSCON_T *SYSCON   ;
extern CSP_TKEY_T 	*TKEY     ;
extern CSP_ADC12_T 	*ADC      ;
extern CSP_GPIO_T 	*GPIOA0   ;
extern CSP_GPIO_T 	*GPIOB0   ;
extern CSP_GPIO_T 	*GPIOC0   ;
extern CSP_GPIOGRP_T 	*GPIOGRP     ;
extern CSP_GTC_T 	*GTC0	  ;
extern CSP_GTC_T 	*GTC1	  ;
extern CSP_GTC_T 	*GTC2	  ;
extern CSP_GTC_T 	*GTC3	  ;
extern CSP_CA_T   	*CA0      ;
extern CSP_LED_T  	*LED0     ;
extern CSP_UART_T 	*UART0    ;
extern CSP_UART_T 	*UART1    ;
extern CSP_I2C_T  	*I2C0     ;

//ISR Define for generating special interrupt related ASM (CK802), with compile option -mistack
void CORETHandler(void) 		__attribute__((isr));
void SYSCONIntHandler(void) 	__attribute__((isr));
void IFCIntHandler(void) 		__attribute__((isr));
void ADCIntHandler(void) 		__attribute__((isr));
void GTC0IntHandler(void) 		__attribute__((isr));
void GTC1IntHandler(void) 		__attribute__((isr));
void GTC2IntHandler(void) 		__attribute__((isr));
void EXI0IntHandler(void) 		__attribute__((isr));
void EXI1IntHandler(void) 		__attribute__((isr));
void GTC3IntHandler(void) 		__attribute__((isr));
void UART0IntHandler(void) 		__attribute__((isr));
void UART1IntHandler(void) 		__attribute__((isr));
void I2CIntHandler(void) 		__attribute__((isr));
void EXI2to3IntHandler(void) 	__attribute__((isr));
void EXI4to8IntHandler(void) 	__attribute__((isr));
void EXI9to13IntHandler(void) 	__attribute__((isr));
void CNTAIntHandler(void) 		__attribute__((isr));
void TKEYGRP0IntHandler(void)	__attribute__((isr)); 
void TKEYGRP1IntHandler(void)	__attribute__((isr)); 
void TKEYIntHandler(void) 		__attribute__((isr));
/*****************************************************************************
************************** Timer delay ***************************************
******************************************************************************/

extern	void delay_nms(unsigned int t);

 /** Function variable declarations  									    */
extern void my_printf(const char *fmt, ...);

#endif   /**< asm32s003_H */

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/