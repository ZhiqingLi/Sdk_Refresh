/*
  ******************************************************************************
  * @file    asm32s003_i2c.h
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _asm32s003_i2c_H
#define _asm32s003_i2c_H

/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"

#define I2C_IDLE            0
#define I2C_STARTED         1
#define I2C_RESTARTED       2
#define I2C_REPEATED_START  3
#define DATA_ACK            4
#define DATA_NACK           5
#define I2C_WR_STARTED      6
#define I2C_RD_STARTED      7
#define BUFSIZE             16
#define RGB_CLOUR_BASE      0
#define KEY_MSG_BASE        8

/**
  * @brief  I2C IO selection
  */
typedef enum
{
    I2C_G0= 0,
    I2C_G1 = 1,
    I2C_G2 = 2,
}I2C_IO_TypeDef;
/**
  * @brief  I2C mode
  */
typedef enum
{
    FAST_MODE = 1,
    STANDARD_MODE = 0,
}I2C_MODE_TypeDef;
/**
  * @brief  I2C SWRST enable
  */
typedef enum
{
    SWRST_EN = 1,
    SWRST_DIS = 0,
}I2C_EN_DIS_TypeDef;
/**
  * @brief  I2C ack
  */
typedef enum
{
  I2C_ACK_NONE = 0,  /*!< No acknowledge */
  I2C_ACK = 1,  /*!< Acknowledge  */
}I2C_Ack_TypeDef;
/**
  * @brief  I2C TX/RX selection
  */
typedef enum
{
  I2C_TX = 0,  /*!< Transmission direction */
  I2C_RX = 1,  /*!< Reception direction */
} I2C_Direction_TypeDef;
/******************************************************************************
************************** I2C Registers Definition ***************************
******************************************************************************/
#define I2C_RESET_VALUE  (0x00000000)
#define I2C_Address      (0x56) //7 bit

/******************************************************************************
* ECR, DCR, PMSR : I2C Power Management Registers
******************************************************************************/
 #define I2C_I2C            (0x01ul << 1)    /**< I2C Clock                 */
 #define I2C_IPIDCODE       (0x01ul << 4)    /**< I2C version number        */
 #define I2C_DBGEN          (0x01ul << 31)   /**< Debug Mode Enable         */
/******************************************************************************
* CR : I2C Control Register 
******************************************************************************/
 #define I2C_SWRST          (0x01ul << 0)   /**< I2C Software Reset          */
 #define I2C_AA             (0x01ul << 1)   /**< I2C Acknowledge Mask        */ 
 #define I2C_AA_NO          (0x00ul << 1)   /**< I2C No Acknowledge          */ 
 #define I2C_AA_RET         (0x01ul << 1)   /**< I2C Acknowledge Returned    */ 
 #define I2C_STO            (0x01ul << 2)   /**< I2C Stop                    */
 #define I2C_STA            (0x01ul << 3)   /**< I2C Start Mask              */             
 #define I2C_STA_MASTER     (0x01ul << 3)   /**< I2C Start Master            */
 #define I2C_STA_SLAVE      (0x00ul << 3)   /**< I2C Start Slave             */
 #define I2C_SI             (0x01ul << 4)   /**< I2C Interrupt Mask          */
 #define I2C_SI_SET         (0x01ul << 4)   /**< I2C Interrupt Mask          */
 #define I2C_SI_CLR         (0x00ul << 4)   /**< I2C Interrupt Mask          */
 #define I2C_ENA            (0x01ul << 8)   /**< I2C Enable Mask             */
 #define I2C_ENABLE         (0x01ul << 8)   /**< I2C Enable                  */
 #define I2C_DISABLE        (0x00ul << 8)   /**< I2C Disable                 */  


    
/******************************************************************************
* MR : I2C Mode Register
******************************************************************************/
/** PRV : Prescaler Value                                                    */
 #define I2C_PRV_MASK  (0xFFFul << 0)           /**< Prescaler Value Mask    */
 #define I2C_PRV(val)  (((val) & 0xFFFul) << 0) /**< Prescaler Value           
                                                    Writing Macro            */
 #define I2C_FAST      (0x01ul  << 12)          /**< Enable/Disable Fast Mode*/

/*****************************************************************************
* SR : I2C Status Register
******************************************************************************/                     
 #define I2C_SC_MASK   (0xFFul << 0)              /* Status Code Mask        */

/******************************************************************************
* Master Transmitter mode status codes
******************************************************************************/
 #define I2C_MTX_START                0x08u  /**< START condition has been    
                                                  transmitted                */
 #define I2C_MTX_RSTART               0x10u  /**< REPEAT START condition has  
                                                  been transmitted           */
 #define I2C_MTX_SADDR_ACK            0x18u  /**< Slave address and WRITE has
                                                  been sent, ACK received    */
 #define I2C_MTX_SADDR_NACK           0x20u  /**< Slave address and WRITE has 
                                                  been sent, No ACK received */
 #define I2C_MTX_DATATX_ACK           0x28u  /**< Data byte transmitted,      
                                                  ACK received               */
 #define I2C_MTX_DATATX_NACK          0x30u  /**< Data byte transmitted,      
                                                  No ACK received            */
 #define I2C_MTX_AL                   0x38u  /**< Arbitration lost           */

/******************************************************************************
* Master Receiver mode status codes
******************************************************************************/
 #define I2C_MRX_START                0x08u  /**< START condition has been    
                                                  transmitted                */
 #define I2C_MRX_RSTART               0x10u  /**< REPEAT START condition has 
                                                  been transmitted           */
 #define I2C_MRX_AL                   0x38u  /**< Arbitration lost           */
 #define I2C_MRX_SADDR_ACK            0x40u  /**< Slave address and Read has  
                                                  been sent, ACK received    */
 #define I2C_MRX_SADDR_NACK           0x48u  /**< Slave address and Read has  
                                                  been sent, No ACK received */ 
 #define I2C_MRX_DATARX_ACK           0x50u  /**< Data byte received,         
                                                  ACK returned               */
 #define I2C_MRX_DATARX_NACK          0x58u  /**< Data byte received,         
                                                  No ACK returned            */

/******************************************************************************
* Slave Receiver mode status codes
******************************************************************************/
 #define I2C_SRX_OADDR_WRI_ACK        0x60u  /**< Own Slave address + WRITE   
                                                  received, ACK returned     */
 #define I2C_SRX_AL_OADDR_ACK         0x68u  /**< Arbitration lost, own slave 
                                                  address received,           
                                                  ACK returned               */                                               
 #define I2C_SRX_GCA_ACK              0x70u  /**< General Call Address has   
                                                  been received, ACK returned*/
 #define I2C_SRX_AL_GCADDR_ACK        0x78u  /**< Arbitration lost, general   
                                                  call address received,      
                                                  ACK returned               */
 #define I2C_SRX_OADDR_DATARX_ACK     0x80u  /**< Addressed with own address  
                                                  and W, data byte received,  
                                                  ACK returned               */
 #define I2C_SRX_OADDR_DATARX_NACK    0x88u  /**< Addressed with own address, 
                                                  data byte received,         
                                                  no ACK returned            */
 #define I2C_SRX_GCADDR_DATARX_ACK    0x90u  /**< Addressed by general call   
                                                  address, data byte received,
                                                  ACK returned               */
 #define I2C_SRX_GCADDR_DATARX_NACK   0x98u  /**< Addressed by general call   
                                                  address, data byte received,
                                                  no ACK returned            */
 #define I2C_SRX_STOP_START           0xA0u  /**< A stop or repeated start     
                                                  has been received while     
                                                  still addressed as slave   */

/*****************************************************************************
* Slave Transmitter mode status codes
******************************************************************************/
 #define I2C_STX_OADDR_READ_ACK       0xA8u  /**< Own Slave address +        */
                                             /**< R received, ACK returned   */ 
 #define I2C_STX_AL_OADDR_ACK         0xB0u  /**< Arbitration lost, own slave*/
                                             /**< address has been received, */
                                             /**< ACK returned               */
 #define I2C_STX_DATATX_ACK           0xB8u  /**< Data has been transmitted, */
                                             /**< ACK has been received      */
 #define I2C_STX_DATATX_NACK          0xC0u  /**< Data has been transmitted, */
                                             /**< No ACK has been received   */
 #define I2C_STX_LDATATX_ACK          0xC8u  /**< Last data has been 
                                                  transmitted, ACK received  */

/*****************************************************************************
* Miscellaneous status codes
******************************************************************************/
 #define I2C_NO_INFORMATION           0xF8u  /**< No revelant state 
                                                  information                */
 #define I2C_BUS_ERROR                0x00u  /**< Bus error due to an illegal*/
                                             /**< START or STOP condition    */

/******************************************************************************
*  IER, IDR, IMR : I2C Interrupt Registers
******************************************************************************/
/* I2C_SI  : I2C Interrupt - Same as CR Register                             */

/******************************************************************************
*  DAT : I2C Serial Data Registers
******************************************************************************/
 #define I2C_DATA_MASK    (0xFFul << 0)            /**< Data Mask            */
 #define I2C_DATA(val)    (((val) & 0xFFul) << 0)  /**< Data Writing Macro   */
 #define I2C_ADDR_SELET(val)    (((val) & 0x7Ful) << 1)  /**< Select Address */
 #define I2C_WRITE        (0x01ul << 0)            /**< WRITE                */
 #define I2C_READ         (0x00ul << 0)            /**< READ                 */

/******************************************************************************
*  ADR : I2C Serial Slave Address Registers
******************************************************************************/
 #define I2C_GC         (0x01ul << 0)            /**< General call           */  
 #define I2C_ADDR_MASK   (0x7Ful << 1)           /**< Address Mask           */
 #define I2C_ADDR(val)   (((val) & 0x7Ful) << 1) /**< Writing Macro          */

/******************************************************************************
*  ADR : I2C Hold/Setup Delay Registers
******************************************************************************/
 #define I2C_DL_MASK    (0xFFul << 0)             /**< Hold/Setup Delay Mask */
 #define I2C_DL(val)    (((val) & 0xFFul) << 0)   /**< Hold/Setup Delay      */


/******************************************************************************
***************************** I2C Registers mask ******************************
******************************************************************************/
 #define I2C_ECR_MASK          (0x80000002ul)    /**< ECR mask               */
 #define I2C_DCR_MASK          (0x80000002ul)    /**< DCR mask               */
 #define I2C_PMSR_MASK         (0x00000002ul)    /**< PMSR mask              */
 #define I2C_CR_MASK           (0x0000000Ful)    /**< CR mask                */
 #define I2C_MR_MASK           (0x00001FFFul)    /**< MR mask                */
 #define I2C_SR_MASK           (0x000000F8ul)    /**< SR mask                */
 #define I2C_IER_MASK          (0x00000010ul)    /**< IER mask               */
 #define I2C_IDR_MASK          (0x00000010ul)    /**< IDR mask               */
 #define I2C_IMR_MASK          (0x00000010ul)    /**< IMR mask               */
 #define I2C_DAT_MASK          (0x0000000Ful)    /**< DAT mask               */
 #define I2C_ADR_MASK          (0x0000000Ful)    /**< ADR mask               */
 #define I2C_THOLD_MASK        (0x0000000Ful)    /**< THOLD mask             */                    



/******************************************************************************
************************* I2C Registers reset value ***************************
******************************************************************************/
 #define I2C_ECR_RST          (0x00000000ul)     /**< ECR reset value         */
 #define I2C_DCR_RST          (0x00000000ul)     /**< DCR reset value         */
 #define I2C_PMSR_RST         (IPIDCODE_I2C0)    /**< PMSR reset value        */
 #define I2C_CR_RST           (0x00000000ul)     /**< CR reset value          */
 #define I2C_MR_RST           (0x000001F4ul)     /**< MR reset value          */
 #define I2C_SR_RST           (0x000000F8ul)     /**< SR reset value          */
 #define I2C_IER_RST          (0x00000000ul)     /**< IER reset value         */
 #define I2C_IDR_RST          (0x00000000ul)     /**< IDR reset value         */
 #define I2C_IMR_RST          (0x00000000ul)     /**< IMR reset value         */
 #define I2C_DAT_RST          (0x00000000ul)     /**< DAT reset value         */
 #define I2C_ADR_RST          (0x00000000ul)     /**< ADR reset value         */
 #define I2C_THOLD_RST        (0x00000001ul)     /**< THOLD reset value       */                  


/******************************************************************************
*************************** I2C Macros Definition *****************************
******************************************************************************/

/* ECR, DCR, PMSR : I2C Power Management Registers                           */
/** Set ECR register (Enable)                                                */
 #define CSP_I2C_SET_ECR(i2c, val)      ((i2c)->ECR = (val))
/** Set DCR register (Disable)                                               */
 #define CSP_I2C_SET_DCR(i2c, val)      ((i2c)->DCR = (val))
/** Get PMSR register (Status)                                               */
 #define CSP_I2C_GET_PMSR(i2c)          ((i2c)->PMSR)

/* CR : I2C Control Register                                                 */
/** Get CR register                                                          */
 #define CSP_I2C_GET_CR(i2c)            ((i2c)->CR)
/** Set CR register                                                          */
 #define CSP_I2C_SET_CR(i2c, val)       ((i2c)->CR = (val))

/* MR : I2C Mode Register                                                    */
/** Get MR register                                                          */
 #define CSP_I2C_GET_MR(i2c)            ((i2c)->MR)
/** Set MR register                                                          */
 #define CSP_I2C_SET_MR(i2c, mode)      ((i2c)->MR = (mode))

/* CSR, SR : I2C Status Register                                             */
/** Set CSR register (Clear)                                                 */
 #define CSP_I2C_SET_CSR(i2c, val)      ((i2c)->CSR = (val))
/** Get SR register (Status)                                                 */
 #define CSP_I2C_GET_SR(i2c)            ((i2c)->SR)         

/* IER, IDR, IMR : I2C Interrupt Registers                                   */
/** Set IER register (Enable)                                                */
 #define CSP_I2C_SET_IER(i2c, val)      ((i2c)->IER = (val))
/** Set IDR register (Disable)                                               */
 #define CSP_I2C_SET_IDR(i2c, val)      ((i2c)->IDR = (val))
/** Get IMR register (Mask)                                                  */
 #define CSP_I2C_GET_IMR(i2c)           ((i2c)->IMR)

/* DAT : I2C Serial Data Register                                            */
/** Get DAT register                                                         */
 #define CSP_I2C_GET_DAT(i2c)          ((i2c)->DAT)
/** Set DAT register                                                         */
 #define CSP_I2C_SET_DAT(i2c, val)      ((i2c)->DAT = (val))

/* ADR : I2C Serial Slave Address Register                                   */
/** Get ADR register                                                         */
 #define CSP_I2C_GET_ADR(i2c)           ((i2c)->ADR)
/** Set ADR register                                                         */
 #define CSP_I2C_SET_ADR(i2c, val)      ((i2c)->ADR = (val))

/* THOLD : I2C Hold/Setup Delay Register                                     */
/** Get THOLD register                                                       */
 #define CSP_I2C_GET_THOLD(i2c)         ((i2c)->THOLD)
/** Set THOLD register                                                       */
 #define CSP_I2C_SET_THOLD(i2c, val)    ((i2c)->THOLD = (val))


/******************************************************************************
********************* I2C External Functions Declaration **********************
******************************************************************************/
extern void I2C_Master_Init(I2C_IO_TypeDef I2C_IO, I2C_MODE_TypeDef I2C_MDOE,U32_T MR_u32, U32_T thold_u32);
extern void I2C_DeInit(void);
extern void I2C_Init2(void);
extern void I2C_Int_Enable(void);
extern void I2C_Int_Disable(void);
extern void I2C_Slave_Init(I2C_IO_TypeDef I2C_IO,I2C_MODE_TypeDef I2C_MDOE, U32_T MR_u32, U32_T thold_u32,U8_T adr_u8);
extern void I2C_WriteByte(U8_T addr, U8_T data);
extern void I2C_WriteByte_Int(U8_T addr, U8_T data);
extern U8_T I2C_ReadByte(U8_T addr);
extern U8_T I2C_ReadByte_Int(U8_T addr);
extern void I2C_Slave_Receive(void);
extern void I2C_Enable(void);
extern void I2C_Disable(void);
extern void I2C_GenerateSTART(void);
extern void I2C_GenerateSTOP(void);
extern void I2C_SoftwareResetCmd(I2C_EN_DIS_TypeDef EN_DIS);
extern void I2C_AcknowledgeConfig(I2C_Ack_TypeDef Ack);
extern U8_T I2C_ReceiveData(void);
extern void I2C_SendData(uint8_t Data);
extern void I2C_Send7bitAddress(uint8_t Address, I2C_Direction_TypeDef Direction);
extern int readbyte(char add);
extern void writebyte(int add,int dat);
extern void writeRegister(int add,int dat)	  ;
extern void I2C_Simulate_init(void);
extern void I2C_WakeUp_Enable(void);
extern void I2C_WakeUp_Disable(void);
extern void I2C_Slave_Receive(void);
#endif   /**< asm32s003_i2c_H */

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/
