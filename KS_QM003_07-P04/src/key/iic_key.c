////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2013, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:ir_key_driver.c
//
//            maintainer: Halley
//
//		Description	:
//					ir keyboard driver
//
//		Changelog	:
//					2013-01-23	Created by Robert
///////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "ir.h"
#include "app_config.h"
#include "os.h"
#include "gpio.h"
#include "irqs.h"
#include "msgq.h"
#include "app_msg.h"	

#ifdef	FUNC_IIC_KEY_EN

#define 	IIC_KEY_SCAN_TIME			10
#define 	IIC_KEY_JITTER_TIME	  50
#define		IIC_KEY_CP_TIME				1000	//CP condition is 1s
#define 	IIC_KEY_CPH_TIME		  350

#define		IIC_KEY_SUM					12		//IR KEY SUM
#define		IIC_KEY_MORE_TIME				100		//IR KEY SUM

typedef enum _IIC_KEY_STATE
{
    IIC_KEY_STATE_IDLE,
    IIC_KEY_STATE_JITTER,
    IIC_KEY_STATE_PRESS_DOWN,
    IIC_KEY_STATE_CP

} IIC_KEY_STATE;

static TIMER			I2cKeyScanTimer;
static TIMER			I2cKeyWaitTimer;
static TIMER			I2cKeyMoreTimer;
static IIC_KEY_STATE		I2cKeyState;


#ifdef I2C_KEY_SCL_PORT_BIT
  #define I2C_KEY_SCL_PORT_IN				  (I2C_KEY_SCL_PORT_OUT - 1)
  #define I2C_KEY_SCL_PORT_IE				  (I2C_KEY_SCL_PORT_OUT + 1)
  #define I2C_KEY_SCL_PORT_OE				  (I2C_KEY_SCL_PORT_OUT + 2)
  #define I2C_KEY_SCL_PORT_PU				  (I2C_KEY_SCL_PORT_OUT + 5)
  #define I2C_KEY_SCL_PORT_PD				  (I2C_KEY_SCL_PORT_OUT + 6)
#endif

#ifdef I2C_KEY_SDA_PORT_BIT
  #define I2C_KEY_SDA_PORT_IN				  (I2C_KEY_SDA_PORT_OUT - 1)
  #define I2C_KEY_SDA_PORT_IE				  (I2C_KEY_SDA_PORT_OUT + 1)
  #define I2C_KEY_SDA_PORT_OE				  (I2C_KEY_SDA_PORT_OUT + 2)
  #define I2C_KEY_SDA_PORT_PU				  (I2C_KEY_SDA_PORT_OUT + 5)
  #define I2C_KEY_SDA_PORT_PD				  (I2C_KEY_SDA_PORT_OUT + 6)
#endif

// Customer IR key value table, fix it by case.
#if 0
static const uint16_t gI2cVal[IIC_KEY_SUM] =
{
	0x001,		// POWER
	0x002,		// MODE
	0x004,		// MUTE

	0x008,		// PLAY/PAUSE
	0x010,		// PRE
	0x020,		// NEXT

	0x040,		// EQ
	0x080,		// VOL-
	0x100,		// VOL+

	0x200,		// 0
	0x018,		// REPEAT
	0x0FF,		// SCN
};

#else
static const uint16_t gI2cVal[IIC_KEY_SUM] =
{
	0x001,		// NONE
	0x002,		// NONE
	0x004,		// NONE

	0x040,		// VOL-
	0x0C0,		// ......
	0x100,		// VOL+

	0x008,		// MODE
	0x010,		// TALK
	0x020,		// p/p

	0x200,		// MIC
	0x080,		// REPEAT
	0x180,		// SCN
};

#endif

static const uint16_t I2cKeyEvent[][5] = 
{
// PDS(按键开始)		      SPR(短按松开)		        CPS(长按开始)		       CPH(长按保持)	        	CPR(长按松开)
	{MSG_NONE,			  MSG_VOL_DW, 		    MSG_VOL_DW, 		   MSG_VOL_DW, 		        MSG_VOL_DW		},	//VOL-
	{MSG_NONE,			  MSG_EQ, 			    MSG_3D, 			   MSG_NONE, 			    MSG_NONE		},	//EQ
	{MSG_NONE,			  MSG_MUTE, 			MSG_REPEAT_AB, 		   MSG_NONE, 			    MSG_NONE		},	//MUTE	

	{MSG_NONE,			  MSG_VOL_DW, 		    MSG_VOL_DW, 		   MSG_VOL_DW, 		        MSG_NONE		},	//VOL-
	{MSG_NONE,			  MSG_NONE, 			MSG_NONE, 		       MSG_NONE,	 	        MSG_NONE        },	//PRE	
	{MSG_NONE,			  MSG_VOL_UP, 		    MSG_VOL_UP, 		   MSG_VOL_UP, 		        MSG_NONE		},	//VOL+	

	{MSG_NONE,			  MSG_MODE, 			MSG_NONE, 			   MSG_NONE, 			    MSG_NONE		},	//MODE
	{MSG_NONE,			  MSG_WIFI_TALK, 		MSG_NONE, 		       MSG_NONE, 		        MSG_NONE		},	//VOL-
	{MSG_NONE,			  MSG_PLAY_PAUSE, 	    MSG_STOP, 			   MSG_NONE,	 		    MSG_NONE		},	//PLAY/PAUSE

	{MSG_NONE,			  MSG_WIFI_MIC, 		MSG_NONE, 		       MSG_NONE, 		        MSG_NONE		},	//VOL-
	{MSG_NONE,			  MSG_NONE, 		    MSG_REPEAT_AB,		   MSG_NONE, 			    MSG_NONE		},	//REPEAT
	{MSG_NONE,			  MSG_NONE,   		    MSG_LANG, 			   MSG_REMIND,  		    MSG_NONE		},	//USB/SD
};	

uint16_t	GetI2cKeyState(void)
{
	return I2cKeyState;
}

//从数据线上读入一个位。
bool I2cKeySdaInput(void)
{
	//设为输入，带上拉
	GpioClrRegBits(I2C_KEY_SDA_PORT_OE, I2C_KEY_SDA_PORT_BIT);
	GpioSetRegBits(I2C_KEY_SDA_PORT_PU, I2C_KEY_SDA_PORT_BIT);
	GpioClrRegBits(I2C_KEY_SDA_PORT_PD, I2C_KEY_SDA_PORT_BIT);

	GpioSetRegBits(I2C_KEY_SDA_PORT_IE, I2C_KEY_SDA_PORT_BIT);

	if(GpioGetReg(I2C_KEY_SDA_PORT_IN) & I2C_KEY_SDA_PORT_BIT)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//设置时钟线输出
void SetI2cKeySclOut(bool OutFlag)
{
	//设置为输出，带下拉；
	GpioSetRegOneBit(I2C_KEY_SCL_PORT_PU, I2C_KEY_SCL_PORT_BIT);
	GpioSetRegOneBit(I2C_KEY_SCL_PORT_PD, I2C_KEY_SCL_PORT_BIT);
	GpioClrRegOneBit(I2C_KEY_SCL_PORT_IE, I2C_KEY_SCL_PORT_BIT);
	GpioSetRegOneBit(I2C_KEY_SCL_PORT_OE, I2C_KEY_SCL_PORT_BIT);
	if(OutFlag)
	{
		GpioSetRegOneBit(I2C_KEY_SCL_PORT_OUT, I2C_KEY_SCL_PORT_BIT);
	}
	else
	{
	  GpioClrRegOneBit(I2C_KEY_SCL_PORT_OUT, I2C_KEY_SCL_PORT_BIT);
	}
}

//设置数据线输出
void SetI2cKeySdaOut(bool OutFlag)
{
	//设置为输出，带下拉；
	GpioSetRegOneBit(I2C_KEY_SDA_PORT_PU, I2C_KEY_SDA_PORT_BIT);
	GpioSetRegOneBit(I2C_KEY_SDA_PORT_PD, I2C_KEY_SDA_PORT_BIT);
	GpioClrRegOneBit(I2C_KEY_SDA_PORT_IE, I2C_KEY_SDA_PORT_BIT);
	GpioSetRegOneBit(I2C_KEY_SDA_PORT_OE, I2C_KEY_SDA_PORT_BIT);
	if(OutFlag)
	{
		GpioSetRegOneBit(I2C_KEY_SDA_PORT_OUT, I2C_KEY_SDA_PORT_BIT);
	}
	else
	{
	  GpioClrRegOneBit(I2C_KEY_SDA_PORT_OUT, I2C_KEY_SDA_PORT_BIT);
	}
}

//设置数据线输出
void SetI2cKeySdaInit(void)
{
	GpioClrRegOneBit(I2C_KEY_SDA_PORT_OE, I2C_KEY_SDA_PORT_BIT);
	GpioSetRegOneBit(I2C_KEY_SDA_PORT_PU, I2C_KEY_SDA_PORT_BIT);
	GpioClrRegOneBit(I2C_KEY_SDA_PORT_PD, I2C_KEY_SDA_PORT_BIT);
	GpioSetRegOneBit(I2C_KEY_SDA_PORT_IE, I2C_KEY_SDA_PORT_BIT);
}

//IIC开始
bool I2cKeyIICStart(void)
{
	bool StartFlag = FALSE;
	SetI2cKeySdaInit();
	SetI2cKeySclOut(FALSE);
	WaitUs(65);
	StartFlag = I2cKeySdaInput();
	SetI2cKeySclOut(TRUE);
	WaitUs(30);
	return StartFlag;
}

//IIC开始
bool I2cKeyIICStop(void)
{
	bool StartFlag = FALSE;
	SetI2cKeySdaInit();
	SetI2cKeySclOut(FALSE);
	WaitUs(30);
	StartFlag = I2cKeySdaInput();
	SetI2cKeySclOut(TRUE);
	SetI2cKeySdaOut(TRUE);
	return StartFlag;
}

//get ir key index
uint8_t GetI2cKeyIndex(void)
{
	static uint16_t KeyTempVal = FALSE;
	static uint8_t  KeyIndex = 0xFF;
	uint8_t RevDataCnt;

	if(I2cKeyIICStart())
	{
		//APP_DBG("IIC Key start  fail\n");
		return 0xFF;
	}
	
	for(RevDataCnt = 0; RevDataCnt < 10; RevDataCnt++)
	{
		KeyTempVal <<= 1;
		SetI2cKeySclOut(FALSE);
		WaitUs(30);
		if(I2cKeySdaInput())
		{
			KeyTempVal |= 0x01;
		}
		else
		{
			KeyTempVal &= 0xFFE;
		}
		SetI2cKeySclOut(TRUE);
		WaitUs(30);
	}
	I2cKeyIICStop();


	//APP_DBG("Get iic KeyTempVal = %x;\n", KeyTempVal);
	for(KeyIndex = 0; KeyIndex < IIC_KEY_SUM; KeyIndex++)
	{
		if(KeyTempVal == gI2cVal[KeyIndex])
		{
			//APP_DBG("Get iic Key KeyIndex = %d;\n", KeyIndex);
			return KeyIndex;
		}
	}
	
	return 0xFF;
}


// check ir key input
// return : low 14 bits -- keyid，high 2 bits -- keystate
uint16_t I2cKeyScan(void)
{
	static 	uint8_t 	PreKeyIndex = 0xFF;
	static 	uint8_t 	TempKeyIndex = FALSE;
	uint8_t				KeyIndex;

	if(!IsTimeOut(&I2cKeyScanTimer))
	{
		return MSG_NONE;
	}
	TimeOutSet(&I2cKeyScanTimer, IIC_KEY_SCAN_TIME);

	KeyIndex = GetI2cKeyIndex();
	
	if(KeyIndex == 0xFF)
	{		
		if(IsTimeOut(&I2cKeyMoreTimer))
		{
			TempKeyIndex = FALSE;
		}
	}
	else
	{
		TimeOutSet(&I2cKeyMoreTimer, IIC_KEY_MORE_TIME);
		if(TempKeyIndex == FALSE)
		{
			TempKeyIndex = KeyIndex;
		}
	}

	switch(I2cKeyState)
	{
		case IIC_KEY_STATE_IDLE:
			if(KeyIndex == 0xFF)
			{
				return MSG_NONE;
			}

			//DBG("IR KEY PDS!\n");
			PreKeyIndex = KeyIndex;
			TimeOutSet(&I2cKeyWaitTimer, IIC_KEY_JITTER_TIME);
			I2cKeyState = IIC_KEY_STATE_JITTER;
			break;

		case IIC_KEY_STATE_JITTER:
			if(PreKeyIndex != KeyIndex)
			{
				uint16_t Msg = MSG_NONE;
				
				if(((TempKeyIndex == 3) && (KeyIndex == 4))
				|| ((TempKeyIndex == 4) && (KeyIndex == 10))
				|| ((TempKeyIndex == 10) && (KeyIndex == 11))
				|| ((TempKeyIndex == 11) && (KeyIndex == 5))
				|| ((TempKeyIndex == 3) && (KeyIndex == 10))
				|| ((TempKeyIndex == 10) && (KeyIndex == 5)))
				{
					Msg = MSG_VOL_UP;
				}
				else if(((TempKeyIndex == 5) && (KeyIndex == 11))
				     || ((TempKeyIndex == 11) && (KeyIndex == 10))
				     || ((TempKeyIndex == 10) && (KeyIndex == 4))
				     || ((TempKeyIndex == 4) && (KeyIndex == 3))
				     || ((TempKeyIndex == 5) && (KeyIndex == 10))
				     || ((TempKeyIndex == 10) && (KeyIndex == 3)))
				{
					Msg = MSG_VOL_DW;
				}
				else if(TempKeyIndex == FALSE)
                {				
	               I2cKeyState = IIC_KEY_STATE_IDLE;
                }
					
				if(KeyIndex != 0xFF)
				{
				  TempKeyIndex = KeyIndex;
					PreKeyIndex = KeyIndex;
				}
				
				return Msg;
			}
			else if(IsTimeOut(&I2cKeyWaitTimer))
			{
				//return key cp value
				//DBG("IR KEY CPS!\n");
				TimeOutSet(&I2cKeyWaitTimer, IIC_KEY_CP_TIME);
				I2cKeyState = IIC_KEY_STATE_PRESS_DOWN;
				return I2cKeyEvent[PreKeyIndex][0];
			}
			break;	
			
		case IIC_KEY_STATE_PRESS_DOWN:
			if(PreKeyIndex != KeyIndex)
			{
				uint16_t Msg = MSG_NONE;
				
				if(((TempKeyIndex == 3) && (KeyIndex == 10))
				|| ((TempKeyIndex == 10) && (KeyIndex == 4))
				|| ((TempKeyIndex == 4) && (KeyIndex == 11))
				|| ((TempKeyIndex == 11) && (KeyIndex == 5))
				|| ((TempKeyIndex == 3) && (KeyIndex == 4))
				|| ((TempKeyIndex == 4) && (KeyIndex == 5)))
				{
					Msg = MSG_VOL_UP;
				}
				else if(((TempKeyIndex == 5) && (KeyIndex == 11))
				     || ((TempKeyIndex == 11) && (KeyIndex == 4))
				     || ((TempKeyIndex == 4) && (KeyIndex == 10))
				     || ((TempKeyIndex == 10) && (KeyIndex == 3))
				     || ((TempKeyIndex == 5) && (KeyIndex == 4))
				     || ((TempKeyIndex == 4) && (KeyIndex == 3)))
				{
					Msg = MSG_VOL_DW;
				}
				else if(TempKeyIndex == FALSE)
				{
					I2cKeyState = IIC_KEY_STATE_IDLE;
					Msg = I2cKeyEvent[PreKeyIndex][1];
				}
				
				if(KeyIndex != 0xFF)
				{
				  TempKeyIndex = KeyIndex;
					PreKeyIndex = KeyIndex;
				}
				return Msg;
			}
			else if(IsTimeOut(&I2cKeyWaitTimer))
			{
				//return key cp value
				//DBG("IR KEY CPS!\n");
				TimeOutSet(&I2cKeyWaitTimer, IIC_KEY_CPH_TIME);
				I2cKeyState = IIC_KEY_STATE_CP;
				return I2cKeyEvent[PreKeyIndex][2];
			}
			break;

		case IIC_KEY_STATE_CP:
			if(PreKeyIndex != KeyIndex)
			{
				uint16_t Msg = MSG_NONE;
				
				if(((TempKeyIndex == 3) && (KeyIndex == 10))
				|| ((TempKeyIndex == 10) && (KeyIndex == 4))
				|| ((TempKeyIndex == 4) && (KeyIndex == 11))
				|| ((TempKeyIndex == 11) && (KeyIndex == 5))
				|| ((TempKeyIndex == 3) && (KeyIndex == 4))
				|| ((TempKeyIndex == 4) && (KeyIndex == 5)))
				{
					Msg = MSG_VOL_UP;
				}
				else if(((TempKeyIndex == 5) && (KeyIndex == 11))
				     || ((TempKeyIndex == 11) && (KeyIndex == 4))
				     || ((TempKeyIndex == 4) && (KeyIndex == 10))
				     || ((TempKeyIndex == 10) && (KeyIndex == 3))
				     || ((TempKeyIndex == 5) && (KeyIndex == 4))
				     || ((TempKeyIndex == 4) && (KeyIndex == 3)))
				{
					Msg = MSG_VOL_DW;
				}
				else if(TempKeyIndex == FALSE)
				{
					I2cKeyState = IIC_KEY_STATE_IDLE;
					Msg = I2cKeyEvent[PreKeyIndex][4];
				}
				
				if(KeyIndex != 0xFF)
				{
				  TempKeyIndex = KeyIndex;
					PreKeyIndex = KeyIndex;
				}
				return Msg;
			}
			else if(IsTimeOut(&I2cKeyWaitTimer))
			{
				TimeOutSet(&I2cKeyWaitTimer, IIC_KEY_CPH_TIME);
				return I2cKeyEvent[PreKeyIndex][3];
			}
			break;

		default:
			I2cKeyState = IIC_KEY_STATE_IDLE;
			break;
	}
	
	return MSG_NONE;
}

int32_t I2cKeyInit(void)
{
	I2cKeyState = IIC_KEY_STATE_IDLE;
	TimeOutSet(&I2cKeyScanTimer, 0);
	TimeOutSet(&I2cKeyWaitTimer, 0);
	TimeOutSet(&I2cKeyMoreTimer, 0);
	SetI2cKeySclOut(TRUE);
	SetI2cKeySdaOut(TRUE);
	return 0;
}
#endif


