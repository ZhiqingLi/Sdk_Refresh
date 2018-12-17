/*
  ******************************************************************************
  * @file    asm32s003_i2c.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "asm32s003_i2c.h"
#include "asm32s003_gpio.h"
#include "asm32s003_uart.h"
#include "asm32s003_syscon.h"
/* define --------------------------------------------------------------------*/
volatile U8_T I2C_INTFlag ;
volatile U8_T restart=0 ;
volatile uint8_t I2CSlaveState = I2C_IDLE;
volatile uint8_t I2CWrBuffer[BUFSIZE];
volatile uint8_t I2CRdBuffer[BUFSIZE];
volatile uint8_t RdIndex = 0;
volatile uint8_t WrIndex = 0;
volatile uint8_t I2C_Data_Adress;	
volatile uint8_t I2C_St_Adress;	
volatile uint8_t I2C_mode;
/* externs--------------------------------------------------------------------*/



/*************************************************************/
//I2C RESET,CLEAR ALL REGISTER
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void I2C_DeInit(void)
{
    I2C0->ECR   = I2C_RESET_VALUE;
    I2C0->DCR   = I2C_RESET_VALUE;
    I2C0->CR    = I2C_RESET_VALUE;
    I2C0->MR    = I2C_RESET_VALUE;
    I2C0->IER   = I2C_RESET_VALUE;
    I2C0->IDR   = I2C_RESET_VALUE;
    I2C0->DAT   = I2C_RESET_VALUE;
    I2C0->ADR   = I2C_RESET_VALUE;
    I2C0->THOLD = I2C_RESET_VALUE;
}  
/*************************************************************/
//I2C Initial
//EntryParameter:I2C_IO、I2C_MDOE、MR_u32、MR_u32
//I2C_IO:I2C_G0 (PA0.13->SCL (AF4), PA0.12->SDA(AF4)) I2C_G1 PA0.1>SCL (AF1), PA0.2->SDA(AF1)
//I2C_G2 (PA0.5->SDA, PA0.4->SCL)
//I2C_MDOE:FAST_MODE(>100K),STANDARD_MODE(<100K)
//MR_u32：Frequency/（MR_u32+4�?I2C clk
//thold_u32：hold/setup time=thold_u32*PCLK
//ReturnValue:NONE
/*************************************************************/
void I2C_Master_Init(I2C_IO_TypeDef I2C_IO, I2C_MODE_TypeDef I2C_MDOE,U32_T MR_u32, U32_T thold_u32)
{
    if (I2C_IO==I2C_G0)
    {
    GPIOA0->CONHR = (GPIOA0->CONHR&0XFF00FFFF)  | 0x00770000;            //PA0.12->SDA, PA0.13->SCL
    }
    else if(I2C_IO==I2C_G1)
    {
    GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFFF00F)  | 0x00000440;            //PA0.2->SDA, PA0.1->SCL
    }
    else if(I2C_IO==I2C_G2)
    {
    GPIOA0->CONLR = (GPIOA0->CONLR&0XFF00FFFF)  | 0x00660000;            //PA0.5->SDA, PA0.4->SCL   
    }
    I2C0->ECR = I2C_I2C;                                                //Enable I2C clock
	I2C0->CR = I2C_SWRST;                                               //I2C Generate a software reset
    if (I2C_MDOE==FAST_MODE)
    {    
    I2C0->MR = MR_u32+0X1000;                                           //76KHz @PCLK 24MHz  e.g:24000000/(0x138+4)=76000   MR_u32=0x138            
    }                                                                   //I2C pre-scalar，and ，mode select
    else I2C0->MR = MR_u32;
    I2C0->THOLD = thold_u32;                                            //I2C Hold/Setup delays
    I2C0->IER = I2C_IER_MASK;                                           //Enable Si interrupt
	I2C0->CR = I2C_ENA;                                                 //Enable I2C
	I2C_mode=0;
}
/*************************************************************/
//I2C Slave Initial
//EntryParameter:I2C_IO、I2C_MDOE、MR_u32、thold_u32 、adr_u8
//I2C_IO:I2C_G0 (PA0.13->SCL (AF4), PA0.12->SDA(AF4)) I2C_G1 PA0.1>SCL (AF1), PA0.2->SDA(AF1)
//I2C_G2 (PA0.5->SDA, PA0.4->SCL)
//MR_u32：Frequency/（MR_u32+4�?I2C clk
//thold_u32：hold/setup time=thold_u32*PCLK
//adr_u8：slave address （bit1~bit7�?
//ReturnValue:NONE
/*************************************************************/
void I2C_Slave_Init(I2C_IO_TypeDef I2C_IO,I2C_MODE_TypeDef I2C_MDOE, U32_T MR_u32, U32_T thold_u32,U8_T adr_u8)
{
    if (I2C_IO==I2C_G0)
    {
    GPIOA0->CONHR = (GPIOA0->CONHR&0XFF00FFFF)  | 0x00770000;            //PA0.12->SDA, PA0.13->SCL
    }
    else if(I2C_IO==I2C_G1)
    {
    GPIOA0->CONLR = (GPIOA0->CONLR&0XFFFFF00F)  | 0x00000440;            //PA0.2->SDA, PA0.1->SCL
    }
    else if(I2C_IO==I2C_G2)
    {
    GPIOA0->CONLR = (GPIOA0->CONLR&0XFF00FFFF)  | 0x00660000;            //PA0.5->SDA, PA0.4->SCL   
    }
    I2C0->ECR = I2C_I2C;                                //Enable I2C clock
	I2C0->CR = I2C_SWRST;                               //I2C Generate a software reset
     if (I2C_MDOE==FAST_MODE)
    {    
    I2C0->MR = MR_u32+0X1000;                           //76KHz @PCLK 24MHz  e.g:24000000/(0x38+4)=76000   MR_u32=0x38         
    }                                                   //I2C pre-scalar，and ，mode select
    else I2C0->MR = MR_u32;
    I2C0->THOLD = thold_u32;                            //I2C Hold/Setup delays
    I2C0->ADR = adr_u8;
    I2C0->IER = I2C_IER_MASK;                           //Enable Si interrupt
    I2C0->CR = I2C0->CR | I2C_STA_SLAVE;                //I2C as Slave
	I2C0->CR = I2C0->CR | I2C_ENA;                      //Enable I2C
    I2C0->CR = I2C0->CR | I2C_AA;
	I2C_mode=1;
}
/*************************************************************/
//I2C enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void I2C_Enable(void)
{
    I2C0->CR = I2C0->CR | I2C_ENABLE;
}
/*************************************************************/
//I2C disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void I2C_Disable(void)
{
    I2C0->CR = I2C0->CR | I2C_DISABLE;
}
/*************************************************************/
//I2C Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void I2C_Int_Enable(void)
{
    INTC_ISER_WRITE(I2C_INT);                             //Enable I2C interrupt
}
/*************************************************************/
//I2C Interrupt disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void I2C_Int_Disable(void)
{
    INTC_ICER_WRITE(I2C_INT);                             //Disable I2C interrupt
}
/*************************************************************/
//I2C wake up enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void I2C_WakeUp_Enable(void)
{
    INTC_IWER_WRITE(I2C_INT);                             //Enable I2C wake up
}
/*************************************************************/
//I2C wake up disable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void I2C_WakeUp_Disable(void)
{
    INTC_IWDR_WRITE(I2C_INT);                             //Disable I2C wake up
}
/*************************************************************/
//I2C Generate START single
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void I2C_GenerateSTART(void)
{
    I2C0->CR = I2C0->CR | I2C_STA;
}
/*************************************************************/
//I2C Generate STOP single
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void I2C_GenerateSTOP(void)
{
    I2C0->CR = I2C0->CR | I2C_STO;
}
/*************************************************************/
//I2C ENABLE/DISABLE software reset
//EntryParameter:SWRST_EN/SWRST_DIS
//ReturnValue:NONE
/*************************************************************/
void I2C_SoftwareResetCmd(I2C_EN_DIS_TypeDef EN_DIS)
{
  if (EN_DIS==SWRST_DIS)
  {
    I2C0->CR |= I2C_SWRST;
  }
  else
  {
    I2C0->CR &= (~I2C_SWRST);
  }
}
/*************************************************************/
//I2C ACK Generate
//EntryParameter:I2C_ACK_NONE/I2C_ACK
//ReturnValue:NONE
/*************************************************************/
void I2C_AcknowledgeConfig(I2C_Ack_TypeDef Ack)
{

  if (Ack == I2C_ACK_NONE)
  {
    /* Disable the acknowledgement */
    I2C0->CR &= I2C_AA_NO;
  }
  else
  {
    /* Enable the acknowledgement */
    I2C0->CR |= I2C_AA;
  }
}
/*************************************************************/
//I2C READ DATA
//EntryParameter:NONE
//ReturnValue:I2C0->DAT
/*************************************************************/
U8_T I2C_ReceiveData(void)
{
  /* Return the data present in the DR register */
  return I2C0->DAT;
}
/*************************************************************/
//I2C SEND DATA
//EntryParameter:DATA
//ReturnValue:NONE
/*************************************************************/
void I2C_SendData(uint8_t Data)
{
  /* Write in the DR register the data to be sent */
  I2C0->DAT = Data;
}
/*************************************************************/
//I2C SEND 7BIT ADDRESS
//EntryParameter:Address,I2C_TX/I2C_RX
//ReturnValue:NONE
/*************************************************************/
void I2C_Send7bitAddress(uint8_t Address, I2C_Direction_TypeDef Direction)
{
  /* Clear bit0 (direction) just in case */
  Address &= 0xFE;
  /* Send the Address + Direction */
  I2C0->DAT= Address | Direction;
}
/*************************************************************/
//I2C Write byte
//EntryParameter:U8_T addr ,U8_T data
//ReturnValue:NONE
/*************************************************************/
void I2C_WriteByte(U8_T addr, U8_T data)
{
// Write Byte
	I2C0->CR = I2C0->CR | I2C_STA;
	while((CSP_I2C_GET_SR(I2C0) & I2C_MTX_START) != I2C_MTX_START );
	while((CSP_I2C_GET_CR(I2C0) & I2C_SI) != I2C_SI);


//	if(I2C0->SR==0x08) 
		I2C0->CR = I2C0->CR & (~I2C_SI);
		I2C0->DAT = I2C_Address<<1;//0xA2;									// Device ID, Write
		while((CSP_I2C_GET_CR(I2C0) & I2C_SI) != I2C_SI);
//		I2C0->CR = I2C0->CR & (~I2C_SI);

//		if(I2C0->SR==0x18) 
			I2C0->CR = I2C0->CR & (~I2C_SI);
			I2C0->DAT = addr;									// Address
			while((CSP_I2C_GET_CR(I2C0) & I2C_SI) != I2C_SI);

//			if(I2C0->SR==0x28) 
				I2C0->CR = I2C0->CR & (~I2C_SI);
				I2C0->DAT = data;								// Data
				while((CSP_I2C_GET_CR(I2C0) & I2C_SI) != I2C_SI);
				I2C0->CR = I2C0->CR & (~I2C_SI);
                
	I2C0->CR = I2C0->CR | I2C_STO;
}
/*************************************************************/
//I2C Write MORE byte
//EntryParameter:U8_T addr ,U8_T data,NumByteToWrite(defined by slave )
//ReturnValue:NONE
/*************************************************************/
void I2C_Write_More_Byte(U8_T addr, U8_T *data,U8_T NumByteToWrite)
{
// Write Byte
	I2C0->CR = I2C0->CR | I2C_STA;
	while((CSP_I2C_GET_SR(I2C0) & I2C_MTX_START) != I2C_MTX_START );
	while((CSP_I2C_GET_CR(I2C0) & I2C_SI) != I2C_SI);


//	if(I2C0->SR==0x08) 
		I2C0->CR = I2C0->CR & (~I2C_SI);
		I2C0->DAT = I2C_Address<<1;//0xA2;									// Device ID, Write
		while((CSP_I2C_GET_CR(I2C0) & I2C_SI) != I2C_SI);
//		I2C0->CR = I2C0->CR & (~I2C_SI);

//		if(I2C0->SR==0x18) 
			I2C0->CR = I2C0->CR & (~I2C_SI);
			I2C0->DAT = addr;									// Address
			while((CSP_I2C_GET_CR(I2C0) & I2C_SI) != I2C_SI);
                I2C0->CR = I2C0->CR & (~I2C_SI);
                while(NumByteToWrite!=0)
//			if(I2C0->SR==0x28) 
            {
				I2C0->DAT = *data;
                data++;// Data
				while((CSP_I2C_GET_CR(I2C0) & I2C_SI) != I2C_SI);
				I2C0->CR = I2C0->CR & (~I2C_SI);
                NumByteToWrite--;
            }                
	I2C0->CR = I2C0->CR | I2C_STO;
}
/*************************************************************/
//I2C Write byte in interruption
//EntryParameter:U8_T addr ,U8_T data
//ReturnValue:NONE
/*************************************************************/
void I2C_WriteByte_Int(U8_T addr, U8_T data)
{

	I2C_INTFlag=0;

// Write Byte
	I2C0->CR = I2C0->CR | I2C_STA;
//	while((CSP_I2C_GET_SR(I2C0) & I2C_MTX_START) != I2C_MTX_START );
	while(I2C_INTFlag!=1);
	I2C_INTFlag=0;

//	I2C0->CR = I2C0->CR & (~I2C_SI);
	I2C0->DAT = I2C_Address<<1;//0xA2;									// Device ID, Write
	while(!I2C_INTFlag);
	I2C_INTFlag=0;

//	I2C0->CR = I2C0->CR & (~I2C_SI);
	I2C0->DAT = addr;									// Address
	while(!I2C_INTFlag);
	I2C_INTFlag=0;

//	I2C0->CR = I2C0->CR & (~I2C_SI);
	I2C0->DAT = data;									// Data
	while(!I2C_INTFlag);
	I2C_INTFlag=0;

//	I2C0->CR = I2C0->CR & (~I2C_SI);
	I2C0->CR = I2C0->CR | I2C_STO;
}

/*************************************************************/
//I2C Read byte
//EntryParameter:U8_T addr
//ReturnValue:I2C0->DAT
/*************************************************************/
U8_T I2C_ReadByte(U8_T addr)
{

	// Verify
	I2C0->CR = I2C0->CR | I2C_STA;
	while((CSP_I2C_GET_SR(I2C0) & I2C_MTX_START) != I2C_MTX_START );
	while((CSP_I2C_GET_CR(I2C0) & I2C_SI) != I2C_SI);
	I2C0->CR = I2C0->CR & (~I2C_SI);

	I2C0->DAT = I2C_Address<<1;//0xA2;									// Device ID, Write
//	I2C0->CR = I2C0->CR & (~I2C_SI);
	while((CSP_I2C_GET_CR(I2C0) & I2C_SI) != I2C_SI);

	I2C0->DAT = addr;									// Address
	I2C0->CR = (I2C0->CR & (~I2C_SI));
	while((CSP_I2C_GET_CR(I2C0) & I2C_SI) != I2C_SI);


	I2C0->CR = (I2C0->CR & (~I2C_SI)) | I2C_STA;		// re-start
//	while((CSP_I2C_GET_SR(I2C0) & I2C_MTX_START) != I2C_MTX_START );
	while((CSP_I2C_GET_CR(I2C0) & I2C_SI) != I2C_SI);


	I2C0->DAT = (I2C_Address<<1)+1;//0xA3;									// Device ID, Read
	I2C0->CR = I2C0->CR & (~I2C_SI);
	while((CSP_I2C_GET_CR(I2C0) & I2C_SI) != I2C_SI);

	I2C0->CR = (I2C0->CR & (~I2C_SI));
	while((CSP_I2C_GET_CR(I2C0) & I2C_SI) != I2C_SI);

	I2C0->CR = (I2C0->CR & (~I2C_SI)) | I2C_STO;

	return I2C0->DAT;
}
/*************************************************************/
//I2C Read byte in interruption
//EntryParameter:U8_T addr
//ReturnValue:i2c_received_data
/*************************************************************/
U8_T I2C_ReadByte_Int(U8_T addr)
{

	I2C_INTFlag=0;
	U8_T i2c_received_data;

	// Verify
	I2C0->CR = I2C0->CR | I2C_STA;
//	while((CSP_I2C_GET_SR(I2C0) & I2C_MTX_START) != I2C_MTX_START );
	while(!I2C_INTFlag);

	I2C0->DAT = I2C_Address<<1;//0xA2;									// Device ID, Write
	I2C_INTFlag=0;
	while(!I2C_INTFlag);

	I2C0->DAT = addr;									// Address
	I2C_INTFlag=0;
	restart=1;
	while(!I2C_INTFlag);

//	I2C0->CR = I2C0->CR | I2C_STA;						// re-start
	I2C_INTFlag=0;
	while(!I2C_INTFlag);

	I2C0->DAT = (I2C_Address<<1)+1;//0xA3;									// Device ID, Read
	I2C_INTFlag=0;
	while(!I2C_INTFlag);

	I2C_INTFlag=0;
	while(!I2C_INTFlag);

	i2c_received_data = I2C0->DAT;

	I2C0->CR = I2C0->CR | I2C_STO;

//	return I2C0->DAT;
	return i2c_received_data;
}
/*************************************************************/
//I2C slave Receive
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void I2C_Slave_Receive(void)
{
    uint8_t SR_Value;
    SR_Value=I2C0->SR;
    switch(SR_Value)
    {
        case I2C_SRX_OADDR_WRI_ACK:                                                        
        case I2C_SRX_AL_OADDR_ACK:                              //slave receive address and ACK returned
        RdIndex=0;
		I2C0->CR = I2C0->CR | I2C_AA;
		I2C0->CR = I2C0->CR & (~I2C_SI);
        I2CSlaveState = I2C_WR_STARTED;
        break;
		
        case I2C_SRX_OADDR_DATARX_ACK:                          
        case I2C_SRX_GCADDR_DATARX_ACK:                        //Address data received
        if ( I2CSlaveState == I2C_WR_STARTED )
        {
			/*I2CRdBuffer[RdIndex] = I2C0->DAT;
			RdIndex++;
			I2C0->CR = I2C0->CR | I2C_AA;*/
			if(RdIndex==0)
			{
				//I2CRdBuffer[RdIndex] = I2C0->DAT;
				I2C_Data_Adress=I2C0->DAT;
				I2C_St_Adress=I2C_Data_Adress;
				RdIndex++;
				I2C0->CR = I2C0->CR | I2C_AA;
			}
			else
			{
				//I2CRdBuffer[I2CRdBuffer[0]]= I2C0->DAT;
				I2CRdBuffer[I2C_Data_Adress]= I2C0->DAT;
				I2C_Data_Adress++;
				I2C0->CR = I2C0->CR | I2C_AA;
			}
        }
        else
        {
          I2C0->CR = I2C0->CR | I2C_AA_NO;  
        }
		I2C0->CR = I2C0->CR & (~I2C_SI);
		break;
		
		case I2C_STX_OADDR_READ_ACK:
		case I2C_STX_AL_OADDR_ACK:                             //Slave address and read bit received，ACK returned
		RdIndex=0;
		I2CSlaveState = I2C_RD_STARTED;
		WrIndex = I2C_St_Adress;
		I2C0->DAT= I2CWrBuffer[WrIndex];
		I2C0->CR = I2C0->CR | I2C_AA;
		I2C0->CR = I2C0->CR & (~I2C_SI);
		break;
		
		case I2C_STX_DATATX_ACK:
		case I2C_STX_LDATATX_ACK:
		if ( I2CSlaveState == I2C_RD_STARTED )
		{
            I2C0->DAT= I2CWrBuffer[WrIndex+1];
            WrIndex++;
			I2C0->CR = I2C0->CR | I2C_AA;
		}
		else
		{
             I2C0->CR = I2C0->CR | I2C_AA_NO;
		}
		I2C0->CR = I2C0->CR & (~I2C_SI);
		break;
		
		case I2C_STX_DATATX_NACK:                             //Data has been transmitted，and no ack
		I2C0->CR = I2C0->CR | I2C_AA_NO;
		I2C0->CR = I2C0->CR & (~I2C_SI);
		I2CSlaveState = DATA_NACK;
		break;
          
		case I2C_SRX_STOP_START:                              //A stop or repeated start received
		I2C0->CR = I2C0->CR | I2C_AA;
		I2C0->CR = I2C0->CR & (~I2C_SI);
		I2CSlaveState = I2C_IDLE;
		break;

		default:
		I2C_SLAVE_CONFIG();                                     //if error，reset I2C
		I2C0->CR = I2C0->CR & (~I2C_SI);
    }
	return;
}
/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/