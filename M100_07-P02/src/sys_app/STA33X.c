#include "os.h"
#include "uart.h"
#include "sd_card.h"
#include "gpio.h"
#include "irqs.h"
#include "uart.h"
#include "timer.h"
#include "watchdog.h"
#include "clk.h"
#include "cache.h"
#include "sdio.h"
#include "app_config.h"
#include "line_in_control.h"
#include "TimeOut.h"
#include "sys_app.h"
#include "i2c.h"
#include "STA33X.h"

#ifdef FUNC_ST_DDX33X_EN
//typedef enum
//{
//  PORT_A, /**< Use GPIOA for I2C */
//	PORT_B,	/**< Use GPIOB for I2C */
//	PORT_C	/**< Use GPIOC for I2C */
//} PORT_SEL;

//extern void* I2cMasterCreate(PORT_SEL SclPortSel, uint8_t SclIndex, PORT_SEL SdaPortSel, uint8_t SdaIndex);
//extern void I2cStart(void* I2cMasterHandle);
//extern void I2cStop(void* I2cMasterHandle);
//extern void I2cSendAck(void* I2cMasterHandle);
//extern void I2cSendNoAck(void* I2cMasterHandle);
void 	AudioModuleSetVolume(unsigned char volume);


extern bool I2cChkAck(void* I2cMasterHandle);
extern bool I2cWriteByte(void* I2cMasterHandle, uint8_t Val);
extern uint8_t I2cReadByte(void* I2cMasterHandle);

uint8_t  gSTA33XOKFlag = FALSE;
unsigned char MUTEVolSave;
uint8_t	STA33XAddr = DDX_I2C_ADDR ;
static void * STA33XI2cHandler = NULL;

const  unsigned char g_STA33X_init_tbl[0x16]=
{
    0x63,/*0, DDX_CONFIGURE_A, the master clock select, 256fs, fault detect enable */
    0x81,/*1,DDX_CONFIGURE_B, the serial input format select, I2s format, MSB first*/
    0x97,/*2,DDX_CONFIGURE_C, Use default output mode*/
    0x40,/*3,DDX_CONFIGURE_D, Use default output mode*/
    0xC2,/*4,DDX_CONFIGURE_E, Use default output mode*/
    0x5D,/*5,DDX_CONFIGURE_F, Use default output mode*/
    0x10,/*6, flat mode EQ,Use default output mode*/	
    0x00,/*7,DDX_MAIN_VOLUME,master gain 0dB*/
    0x53,/*8,DDX_CHANNEL1_CFG, gain 0dB*/
    0x53,/*9,DDX_CHANNEL2_CFG, gain 0dB*/
    0x53,/*A,DDX_CHANNEL3_CFG, gain 0dB*/
/*EQ*/
    0x80,/*B,Audio preset , Use default output mode*/
    0x90,/*C,Audio preset , Use default output mode*/
    0x00,/* D*/
    0x00,/*E,channel configuration ,eq,tone,volume  by pass*/
    0x40,/*F,channel configuration, eq,tone,volumeby pass*/
    0x80,/*10, channel configuration,eq,tone,volume by pass*/
    
    0x77,/*11,tone control ,0dB*/
    0x6A,/*12,dynamic control ,Use default output mode*/
    0x69,/*13,dynamic control ,Use default output mode*/
    0x6A,/*14,dynamic control ,Use default output mode*/
    0x69,/*15,dynamic control ,Use default output mode*/
/*  beyone 0X15, use default data*/

};

const uint8_t STA33X_vol_tbl[MAX_VOLUME+1] = 
{       
    //0x00,0x40,0x47,0x4A,0x4C,0x4F,0x51,0x55,0x57,0x5A,
    //0x60,0x67,0x6D,0x74,0x78,0x7A
    0xEF,
    0x50,0x40,0x3C,0x3A,0x38,0x36,0x34,0x32,0x30,0x2E,
    0x2C,0x2A,0x28,0x26,0x24,0x22,0x20,0x1E,0x1C,0x1A,
    0x18,0x16,0x14,0x12,0x10,0x0E,0x0A,0x08,0x06,0x04,
    0x02,0x00      
};


bool I2Cm_Rx(unsigned char *p,unsigned char RegAddress,unsigned char length ,unsigned charDeviceAddress)		
{
	uint8_t i;
	bool WrDevOK = 0;
	bool WrRegOK = 0;
	bool ReadOK = 0;		

	I2cStart(STA33XI2cHandler);
	WrDevOK = I2cWriteByte(STA33XI2cHandler,charDeviceAddress);
	WrRegOK = I2cWriteByte(STA33XI2cHandler,RegAddress);				//sta369随机地址读要先写
    WaitMs(10);
	I2cStart(STA33XI2cHandler);
	ReadOK = I2cWriteByte(STA33XI2cHandler,charDeviceAddress+1);      
	if(length==1)
	{
		*p= I2cReadByte(STA33XI2cHandler);
		I2cSendNoAck(STA33XI2cHandler);
		I2cStop(STA33XI2cHandler);
		WaitMs(10);
		return (WrDevOK && WrRegOK && ReadOK);
	}
	length--;
	for(i=0;i<length;i++)
	{
		*p++= I2cReadByte(STA33XI2cHandler);
		I2cSendNoAck(STA33XI2cHandler);
	}
	*p= I2cReadByte(STA33XI2cHandler);
	I2cSendNoAck(STA33XI2cHandler);
	I2cStop(STA33XI2cHandler);	
	return (WrDevOK && WrRegOK && ReadOK);
}

bool I2Cm_Tx(unsigned char *p,unsigned char RegAddress,unsigned char length ,unsigned charDeviceAddress)
{
	uint8_t i;
	bool DevOK, RegOK;
		
	I2cStart(STA33XI2cHandler);
	DevOK = I2cWriteByte(STA33XI2cHandler,charDeviceAddress);
	RegOK = I2cWriteByte(STA33XI2cHandler,RegAddress);
	for(i=0;i<length;i++)
	{
		I2cWriteByte(STA33XI2cHandler,*p++) ;
	}
	I2cStop(STA33XI2cHandler);
	WaitMs(10);

	if (DevOK && RegOK)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//void AudioModuleSetSample(uint32_t  sample)
//{
//	unsigned char i;
//	uint8_t buf;

//	I2Cm_Rx(&buf,0,1,DDX_I2C_ADDR);
//	if((sample == 32000) || (sample == 16000)) 
//	{
//		i = 0x62;  //--384*fs
//	}
//	else
//	{
//		i = 0x63;  //--256*fs
//	}
//	
//	i |= buf & 0xF8;	
//	I2Cm_Tx(&i,0,1,DDX_I2C_ADDR);	
//	I2Cm_Rx(&buf,0,1,DDX_I2C_ADDR);
//	APP_DBG("STA33X REG[0]=%x\n", buf);
//}

void STA33X_init(void) 
{
	unsigned char i;
	bool x;

	STA369_POWER_ON();
	WaitMs(100);
	
	STA33XAddr = DDX_I2C_ADDR;

	STA369_REST_LOW();
	WaitMs(50);
	STA369_REST_HIGH();  
	WaitMs(50);	
	
	if (!STA33XI2cHandler)
	{			
		STA33XI2cHandler= I2cMasterCreate(PORT_A, 24, PORT_A, 25);	
		if (!STA33XI2cHandler)
		{
			APP_DBG("STA33X I2cMasterCreate Error!\n");
		}
		else
		{
			DBG("STA33X I2cMasterCreate OK!\n");
		}
	}
   //I2cStop(STA33XI2cHandler);	
   
	I2cStart(STA33XI2cHandler);
	x = I2cWriteByte(STA33XI2cHandler, DDX_I2C_ADDR);
	if(x == 0)
	{
		 APP_DBG("STA33X err\n");	
		 gSTA33XOKFlag = 0;	
		 I2cStop(STA33XI2cHandler);	
		 return;
	}
	APP_DBG("STA33X OK\n");	
	I2cStop(STA33XI2cHandler);	
	WaitMs(50);
//STA369寄存器配置
	i = 0x63;  	//CONFIGURE_A		0x63
	x = I2Cm_Tx(&i, 0x00, 1, DDX_I2C_ADDR);

	i = 0x80; //0x40 //CONFIGURE_B
	x = I2Cm_Tx(&i, 0x01, 1, DDX_I2C_ADDR);

	i = 0x9F;//0x9F;		/*  */		//CONFIGURE_C		0xC2
	x = I2Cm_Tx(&i, 0x02, 1, DDX_I2C_ADDR);	

	i = 0x60;	//0x70		//CONFIGURE_D
	x = I2Cm_Tx(&i, 0x03, 1, DDX_I2C_ADDR);	

	i = 0xC0;	 		//CONFIGURE_E
	x = I2Cm_Tx(&i, 0x04, 1, DDX_I2C_ADDR);	

	i = 0xDD;//0x5D;	//Write 0x5D first		//CONFIGURE_F
	x = I2Cm_Tx(&i, 0x05, 1, DDX_I2C_ADDR);	

	i = 0x1F; 		//DDX_MUTE
	x = I2Cm_Tx(&i, 0x06, 1, DDX_I2C_ADDR);	

	i = 0x14;// 0xFE;		//主音量不衰减，为最大0DB //DDX_MAIN_VOLUME
	x = I2Cm_Tx(&i, 0x07, 1, DDX_I2C_ADDR);	
	
  MUTEVolSave = i;

	i = 0x3C;//0x09;//0x50;		//0x50通道1音量+8DB		0x60为0DB	//DDX_CHANNEL1_VOL
	x = I2Cm_Tx(&i, 0x08, 1, DDX_I2C_ADDR);	
	
	i = 0x3C;//0x09;//0x50;		//0x50通道2音量+8DB		//DDX_CHANNEL2_VOL
	x = I2Cm_Tx(&i, 0x09, 1, DDX_I2C_ADDR);	
	
	i = 0x38;//0x50;	//通道3音量			//DDX_CHANNEL3_VOL 
	I2Cm_Tx(&i, 0x0A, 1, DDX_I2C_ADDR);	

	i = 0x80;		/*  */		//DDX_AUTO1		0x77
	x = I2Cm_Tx(&i, 0x0B, 1, DDX_I2C_ADDR);	

  i = 0xF0;	/*  */			//DDX_AUTO2		0x77
	x = I2Cm_Tx(&i, 0x0C, 1, DDX_I2C_ADDR);	

	i = 0x10;		//0x00		//DDX_CHANNEL1_CFG 
	x = I2Cm_Tx(&i, 0x0E, 1, DDX_I2C_ADDR);	

	i = 0x50;		//0x40		//DDX_CHANNEL2_CFG 
	x = I2Cm_Tx(&i, 0x0F, 1, DDX_I2C_ADDR);

	i = 0xA0;		//通道3		//DDX_CHANNEL3_CFG 
	I2Cm_Tx(&i, 0x10, 1, DDX_I2C_ADDR);
	
	i = 0x77;
	I2Cm_Tx(&i, 0x11, 1, DDX_I2C_ADDR);
	
	i = 0x6A;
	I2Cm_Tx(&i, 0x12, 1, DDX_I2C_ADDR);
	
	i = 0xF9;
	I2Cm_Tx(&i, 0x13, 1, DDX_I2C_ADDR);
	
	i = 0x6A;
	I2Cm_Tx(&i, 0x14, 1, DDX_I2C_ADDR);
	
	i = 0xF9;
	I2Cm_Tx(&i, 0x15, 1, DDX_I2C_ADDR);
	
	i = 0x00;
	I2Cm_Tx(&i, 0x16, 1, DDX_I2C_ADDR);

  i = 0x80;
	I2Cm_Tx(&i, 0x31, 1, DDX_I2C_ADDR);
	
	for(i = 0;i <= 0x3F;i++)
	{
		uint8_t Temp;
		I2Cm_Rx(&Temp,i,1,DDX_I2C_ADDR);
		APP_DBG("STA339BW Reg[%x] = %x;\n", i,Temp);
	}
	gSTA33XOKFlag = 1;
}	

/* Set MUTE
	Mute=1	system unmute
	Mute=0	System mute
   */

void STA33X_SetMasterMute(unsigned char Mute)
{
#if 1
	unsigned char Tempdata1,Tempdata2,Tempdata3,Tempdata;
	uint8_t I2C_buf1;

	if(Mute==0)
	{
		I2Cm_Rx(&I2C_buf1,DDX_MAIN_VOLUME,1,DDX_I2C_ADDR);
		DBG("Save MUTEVolSave=%d\n",I2C_buf1);
		//MUTEVolSave = I2C_buf1; // Save the current Gain
		I2C_buf1=0xFE;
		I2Cm_Tx(&I2C_buf1,DDX_MAIN_VOLUME,1,DDX_I2C_ADDR);
		WaitMs(10);//10-20ms delay
	}
	else
	{
		I2Cm_Rx(&I2C_buf1,DDX_CHANNEL1_CFG,1,DDX_I2C_ADDR);
		Tempdata1=I2C_buf1;
		I2Cm_Rx(&I2C_buf1,DDX_CHANNEL2_CFG,1,DDX_I2C_ADDR);
		Tempdata2=I2C_buf1;
		I2Cm_Rx(&I2C_buf1,DDX_CHANNEL3_CFG,1,DDX_I2C_ADDR);
		Tempdata3=I2C_buf1;

		Tempdata=Tempdata1&0xCF;								//clear the CxLSx bit as 00
		I2Cm_Tx(&Tempdata,DDX_CHANNEL1_CFG,1,DDX_I2C_ADDR);
		Tempdata=Tempdata2&0xCF;
		I2Cm_Tx(&Tempdata,DDX_CHANNEL2_CFG,1,DDX_I2C_ADDR);
		Tempdata=Tempdata3&0xCF;
		I2Cm_Tx(&Tempdata,DDX_CHANNEL3_CFG,1,DDX_I2C_ADDR);
		WaitMs(1);//10-20ms delay

		Tempdata=Tempdata1|0x10;								//Set the CxLSx bit as 01
		I2Cm_Tx(&Tempdata,DDX_CHANNEL1_CFG,1,DDX_I2C_ADDR);
		Tempdata=Tempdata2|0x10;
		I2Cm_Tx(&Tempdata,DDX_CHANNEL2_CFG,1,DDX_I2C_ADDR);
		Tempdata=Tempdata3|0x10;
		I2Cm_Tx(&Tempdata,DDX_CHANNEL3_CFG,1,DDX_I2C_ADDR);
		WaitMs(1);//10-20ms delay
		Tempdata=Tempdata1&0xCF;								//clear the CxLSx bit as 00
		I2Cm_Tx(&Tempdata,DDX_CHANNEL1_CFG,1,DDX_I2C_ADDR);
		Tempdata=Tempdata2&0xCF;
		I2Cm_Tx(&Tempdata,DDX_CHANNEL2_CFG,1,DDX_I2C_ADDR);
		Tempdata=Tempdata3&0xCF;
		I2Cm_Tx(&Tempdata,DDX_CHANNEL3_CFG,1,DDX_I2C_ADDR);
		WaitMs(1);//10-20ms delay
		I2Cm_Tx(&Tempdata1,DDX_CHANNEL1_CFG,1,DDX_I2C_ADDR);
		I2Cm_Tx(&Tempdata2,DDX_CHANNEL2_CFG,1,DDX_I2C_ADDR);
		I2Cm_Tx(&Tempdata3,DDX_CHANNEL3_CFG,1,DDX_I2C_ADDR);
		I2Cm_Tx(&MUTEVolSave,DDX_MAIN_VOLUME,1,DDX_I2C_ADDR);
		WaitMs(1);//10-20ms delay
		DBG("MUTEVolSave=%d\n",MUTEVolSave);
	}
#else
		uint8_t I2C_buf1;
//zy test++++++++++++++++++++++++
//				I2Cm_Rx(&I2C_buf1,0,1,DDX_I2C_ADDR);
//				APP_DBG("STA33X REG[0]=%x\n",I2C_buf1);
//				I2Cm_Rx(&I2C_buf1,DDX_CONFIGURE_B,1,DDX_I2C_ADDR);
//				APP_DBG("STA33X REG[1]=%x\n",I2C_buf1);
//				I2Cm_Rx(&I2C_buf1,DDX_CONFIGURE_F,1,DDX_I2C_ADDR);
//				APP_DBG("STA33X REG[5]=%x\n",I2C_buf1);			  
//+++++++++++++++++++++++++++++++++			  
		if(Mute==0)
		{
			I2C_buf1=0xFE;
			I2Cm_Tx(&I2C_buf1,DDX_MAIN_VOLUME,1,DDX_I2C_ADDR);
		}
		else
		{
			I2C_buf1=STA33X_vol_tbl[gSys.Volume];
			I2Cm_Tx(&I2C_buf1,DDX_MAIN_VOLUME,1,DDX_I2C_ADDR);

		}
#endif		
}

void STA33X_SetVolume(unsigned char volume)
{
	unsigned char temp ;

	temp = STA33X_vol_tbl[volume];
	I2Cm_Tx(&temp, DDX_MAIN_VOLUME, 1, DDX_I2C_ADDR);
}

void	ExMuteOn(void)
{
	if(gSTA33XOKFlag == 1)   
	{
		STA369_POWER_OFF();
		STA33X_SetMasterMute(0);
	}
}
void	ExMuteOff(void)
{
	if(gSTA33XOKFlag == 1)  
	{
		STA33X_SetMasterMute(1);
	}
}
//Mute控制
bool STA33X_MuteControl(bool Mute)
{
	uint8_t IIC_Buf,Temp;
	if(gSTA33XOKFlag == FALSE)
	{
		return FALSE;
	}
	if(Mute)   
	{
		/*I2Cm_Rx(&IIC_Buf,DDX_CONFIGURE_F,1,DDX_I2C_ADDR);
		IIC_Buf &= 0x7F;
		do
		{
		  I2Cm_Tx(&IIC_Buf,DDX_CONFIGURE_F,1,DDX_I2C_ADDR);
			I2Cm_Rx(&Temp,DDX_CONFIGURE_F,1,DDX_I2C_ADDR);
		}while(Temp != IIC_Buf);*/
		
    IIC_Buf = 0x1F;
		do
		{
		  I2Cm_Tx(&IIC_Buf,DDX_MUTE,1,DDX_I2C_ADDR);
			I2Cm_Rx(&Temp,DDX_MUTE,1,DDX_I2C_ADDR);
		}while(Temp != IIC_Buf);
	}
	else
	{
		I2Cm_Rx(&IIC_Buf,DDX_CONFIGURE_F,1,DDX_I2C_ADDR);
		IIC_Buf |= 0x80;
		do
		{
		  I2Cm_Tx(&IIC_Buf,DDX_CONFIGURE_F,1,DDX_I2C_ADDR);
			I2Cm_Rx(&Temp,DDX_CONFIGURE_F,1,DDX_I2C_ADDR);
		}while(Temp != IIC_Buf);
		
		if(gSys.CurModuleID == MODULE_ID_BT_HF)
		{
			IIC_Buf = 0x12;
		}
		else
		{
			IIC_Buf = 0x10;
		}
		do
		{
		  I2Cm_Tx(&IIC_Buf,DDX_MUTE,1,DDX_I2C_ADDR);
			I2Cm_Rx(&Temp,DDX_MUTE,1,DDX_I2C_ADDR);
		}while(Temp != IIC_Buf);
	}
	return TRUE;
}

unsigned char STA33x_Normal_index0[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00};	
unsigned char STA33x_Normal_index1[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00};	
unsigned char STA33x_Normal_index2[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00};	
unsigned char STA33x_Normal_index3[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00};	

unsigned char STA33x_Pop_index0[]={0x85,0xB2,0xC2,0x74,0xF2,0x37,0x7A,0x4D,0x3E,0x89,0x2D,0x10,0x40,0xF0,0x5C};	
unsigned char STA33x_Pop_index1[]={0x89,0x11,0xFA,0x6F,0x62,0x48,0x76,0xEE,0x06,0x8D,0x57,0x2A,0x41,0xA3,0x46};	
unsigned char STA33x_Pop_index2[]={0x8F,0x4A,0x83,0x66,0xE6,0x9F,0x70,0xB5,0x7D,0x93,0xB1,0xDA,0x42,0xB3,0xC3};	
unsigned char STA33x_Pop_index3[]={0x99,0x54,0xE1,0x5E,0xFE,0xF2,0x66,0xAB,0x1F,0x9B,0xB6,0x29,0x42,0xA5,0x72};	

unsigned char STA33x_Classic_index0[]={0x80,0x65,0x24,0x7F,0x08,0xEB,0x7F,0x9A,0xDC,0x80,0xC6,0x5F,0x40,0x18,0x5A};	
unsigned char STA33x_Classic_index1[]={0x80,0x8E,0x03,0x7E,0xAB,0x74,0x7F,0x71,0xFD,0x81,0x14,0x6F,0x40,0x20,0x0E};	
unsigned char STA33x_Classic_index2[]={0x98,0x59,0xF0,0x55,0x01,0x1C,0x67,0xA6,0x10,0x9B,0x04,0xA3,0x47,0xFD,0x20};	
unsigned char STA33x_Classic_index3[]={0xA9,0xAE,0xEE,0x43,0x11,0xEE,0x56,0x51,0x12,0xA5,0x4C,0x20,0x4B,0xD0,0xF8};	

unsigned char STA33x_Jazz_index0[]={0x88,0x96,0x2D,0x72,0xD7,0x11,0x77,0x69,0xD3,0x8C,0xC7,0xFF,0x40,0x30,0x78};	
unsigned char STA33x_Jazz_index1[]={0x8D,0x64,0x42,0x63,0x84,0x42,0x72,0x9B,0xBE,0x91,0xE6,0x12,0x45,0x4A,0xD6};	
unsigned char STA33x_Jazz_index2[]={0x99,0x54,0xE1,0x52,0xBA,0xCB,0x66,0xAB,0x1F,0x9B,0xB6,0x29,0x48,0xC7,0x86};	
unsigned char STA33x_Jazz_index3[]={0xAE,0x14,0x52,0x58,0x3E,0x46,0x51,0xEB,0xAE,0xA7,0x5E,0x21,0x40,0x31,0xCC};	

unsigned char STA33x_Rock_index0[]={0x80,0x2B,0x34,0x7F,0x61,0xDD,0x7F,0xD4,0xCC,0x80,0x55,0xAD,0x40,0x24,0x3A};	
unsigned char STA33x_Rock_index1[]={0x80,0x3B,0x8F,0x7F,0x78,0x51,0x7F,0xC4,0x71,0x80,0x75,0xBF,0x40,0x08,0xF7};	
unsigned char STA33x_Rock_index2[]={0x9B,0x69,0x78,0x5D,0x4E,0x3F,0x64,0x96,0x88,0x9D,0x21,0x79,0x42,0xC8,0x24};	
unsigned char STA33x_Rock_index3[]={0xAB,0x63,0x3F,0x3B,0x9B,0x24,0x54,0x9C,0xC1,0xA6,0x1F,0x19,0x4F,0x22,0xE1};	

unsigned char STA33x_Bass_index0[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00};	
unsigned char STA33x_Bass_index1[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00};	
unsigned char STA33x_Bass_index2[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00};	
unsigned char STA33x_Bass_index3[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00};	


/*
	Intial the EQ curve for coefficient data Write
	write Filter data from STA339_EQ[],Filter address FilterIndex=0~4
*/
void STA33X_CoefficientWrite(unsigned char FilterIndex,unsigned char *STA33x_EQ)
{
	unsigned char STA33x_tempj;
	uint8_t I2C_buf1;
	//clear 0x17~0x25 IIC register
	for(STA33x_tempj=0;STA33x_tempj<15;STA33x_tempj++)
	{
		I2C_buf1=0x00;
		I2Cm_Tx(&I2C_buf1,(0x17+STA33x_tempj),1,DDX_I2C_ADDR);
	}
	//Set coefficient data address
	I2C_buf1=FilterIndex*5;
	I2Cm_Tx(&I2C_buf1,0x16,1,DDX_I2C_ADDR);
	//write bank data to 0x17~0x25
	for(STA33x_tempj=0;STA33x_tempj<15;STA33x_tempj++)
	{
		I2C_buf1=*STA33x_EQ++;
		I2Cm_Tx(&I2C_buf1,(0x17+STA33x_tempj),1,DDX_I2C_ADDR);
	}
	//Write the command to 0x26(3 times)
	I2C_buf1=0x02;
	I2Cm_Tx(&I2C_buf1,0x26,1,DDX_I2C_ADDR);
	I2Cm_Tx(&I2C_buf1,0x26,1,DDX_I2C_ADDR);
	I2Cm_Tx(&I2C_buf1,0x26,1,DDX_I2C_ADDR);
}

void AudioModuleSetEq(unsigned char eq)
{

	if(gSTA33XOKFlag==0) 
	{
		return;
	}
	if(eq==1)
	{
		STA33X_CoefficientWrite(0,STA33x_Rock_index0);
		STA33X_CoefficientWrite(1,STA33x_Rock_index1);
		STA33X_CoefficientWrite(2,STA33x_Rock_index2);
		STA33X_CoefficientWrite(3,STA33x_Rock_index3);
	}
	else if(eq==2)
	{
		STA33X_CoefficientWrite(0,STA33x_Pop_index0);
		STA33X_CoefficientWrite(1,STA33x_Pop_index1);
		STA33X_CoefficientWrite(2,STA33x_Pop_index2);
		STA33X_CoefficientWrite(3,STA33x_Pop_index3);
	}
	else if(eq==3)
	{
		STA33X_CoefficientWrite(0,STA33x_Classic_index0);
		STA33X_CoefficientWrite(1,STA33x_Classic_index1);
		STA33X_CoefficientWrite(2,STA33x_Classic_index2);
		STA33X_CoefficientWrite(3,STA33x_Classic_index3);
	}
	else if(eq==4)
	{
		STA33X_CoefficientWrite(0,STA33x_Jazz_index0);
		STA33X_CoefficientWrite(1,STA33x_Jazz_index1);
		STA33X_CoefficientWrite(2,STA33x_Jazz_index2);
		STA33X_CoefficientWrite(3,STA33x_Jazz_index3);
	}
	else if(eq==5)
	{
		STA33X_CoefficientWrite(0,STA33x_Bass_index0);
		STA33X_CoefficientWrite(1,STA33x_Bass_index1);
		STA33X_CoefficientWrite(2,STA33x_Bass_index2);
		STA33X_CoefficientWrite(3,STA33x_Bass_index3);
	}
	else	   
	{
		STA33X_CoefficientWrite(0,STA33x_Normal_index0);
		STA33X_CoefficientWrite(1,STA33x_Normal_index1);
		STA33X_CoefficientWrite(2,STA33x_Normal_index2);
		STA33X_CoefficientWrite(3,STA33x_Normal_index3);
	}
}
#endif

