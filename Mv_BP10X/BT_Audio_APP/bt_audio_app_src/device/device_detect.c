/**
 **************************************************************************************
 * @file    dev_detect.c
 * @brief   
 *
 * @author  pi
 * @version V1.0.0
 *
 * $Created: 2018-01-08 13:30:47$
 *
 * @Copyright (C) 2018, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include "type.h"
#include "app_config.h"
//driver
#include "gpio.h"
#include "sdio.h"
#include "timeout.h"
#include "otg_detect.h"
#include "delay.h"
#include "debug.h"
#include "device_detect.h"
#include "rtos_api.h" //for SDIOMutex
#include "otg_device_standard_request.h"
#include "mode_switch_api.h"
#include "irqn.h"
#include "misc.h"
#include "main_task.h"
#include "otg_device_hcd.h"
#include "otg_device_standard_request.h"

//#include "sd_card.h"
#define		DETECT_INIT_TIME			20
#define 	DETECT_JITTER_TIME			100
#define		DETECT_OUT_WAIT_TIME		150 //拔出 屏蔽期，避免事件频繁。取代消抖，意在模式硬件重新初始化。

typedef enum _DETECT_STATE
{
	DETECT_STATE_OUT			= 0,
	DETECT_STATE_IN				= 1,
	DETECT_STATE_OUT_JITTER		= 2,
	DETECT_STATE_IN_JITTER		= 3,
	DETECT_STATE_IDLE			= 4,
} DETECT_STATE;
#ifdef CFG_FUNC_CARD_DETECT
#ifndef CARD_DETECT_GPIO
/*#define SDIO_Clk_Disable		SDIO1_ClkDisable
#define SDIO_Clk_Eable			SDIO1_ClkEnable
#define CARD_DETECT_GPIO		GPIOA6
#define CARD_DETECT_GPIO_IN		GPIO_A_IN
#define CARD_DETECT_BIT_MASK	GPIO_INDEX6
#define CARD_DETECT_GPIO_IE		GPIO_A_IE
#define CARD_DETECT_GPIO_OE		GPIO_A_OE
#define CARD_DETECT_GPIO_PU		GPIO_A_PU
#define CARD_DETECT_GPIO_PD		GPIO_A_PD
*/
#endif
static DETECT_STATE CardState = DETECT_STATE_IDLE;
#endif
TIMER		DetectTimer;
#ifdef CFG_FUNC_UDISK_DETECT
static DETECT_STATE UDiskState = DETECT_STATE_IDLE;
#endif
#ifdef CFG_FUNC_USB_DEVICE_EN
static DETECT_STATE USB_Device_State = DETECT_STATE_IDLE;
#endif

#ifdef CFG_LINEIN_DET_EN
static DETECT_STATE LineInState = DETECT_STATE_IDLE;
#endif


//static uint32_t sd_in_count=0;
//static uint32_t sd_out_count=0;

#ifdef CFG_RADIO_I2C_SD_SAME_PIN_EN
extern bool i2c_work;
#endif

bool IsCardIn(void)// 10ms 检测有问题
{
#ifdef CFG_FUNC_CARD_DETECT
	bool FindCard;
	static bool FindCard_bak;
	bool BackupPD;
	uint8_t BackupMode;

#ifdef CFG_RADIO_I2C_SD_SAME_PIN_EN
	if(i2c_work)
		return FindCard_bak;
#endif
	osMutexLock(SDIOMutex);
	SDIO_Clk_Disable();
	BackupMode = GPIO_PortAModeGet(CARD_DETECT_GPIO);
	BackupPD = GPIO_RegOneBitGet(CARD_DETECT_GPIO_PD, CARD_DETECT_BIT_MASK);	
#ifdef CFG_RADIO_I2C_SD_SAME_PIN_EN//开启IO现场恢复时，识别检测时间较长。
	bool BackupIE,BackupOE,BackupPU;
	BackupPU = GPIO_RegOneBitGet(CARD_DETECT_GPIO_PU, CARD_DETECT_BIT_MASK);
	BackupIE = GPIO_RegOneBitGet(CARD_DETECT_GPIO_IE, CARD_DETECT_BIT_MASK);
	BackupOE = GPIO_RegOneBitGet(CARD_DETECT_GPIO_OE, CARD_DETECT_BIT_MASK);
	GPIO_RegOneBitClear(CARD_DETECT_GPIO_OE, CARD_DETECT_BIT_MASK);
#endif
	GPIO_PortAModeSet(CARD_DETECT_GPIO, 0x0);
	GPIO_RegOneBitSet(CARD_DETECT_GPIO_PU, CARD_DETECT_BIT_MASK);
	GPIO_RegOneBitClear(CARD_DETECT_GPIO_PD, CARD_DETECT_BIT_MASK); 
	GPIO_RegOneBitSet(CARD_DETECT_GPIO_IE, CARD_DETECT_BIT_MASK);
#ifdef CFG_RADIO_I2C_SD_SAME_PIN_EN
	__udelay(29);//延时等电平稳定,阻值设定相关。
#endif
	//__udelay(1);//R83阻值设定相关
	if(GPIO_RegOneBitGet(CARD_DETECT_GPIO_IN, CARD_DETECT_BIT_MASK))
	{
		//DBG(".\n");
		FindCard = FALSE;
	}
	else
	{
		//DBG("|\n");

		FindCard = TRUE;
	}
#ifdef CFG_RADIO_I2C_SD_SAME_PIN_EN
	FindCard_bak = FindCard;
#endif
	GPIO_PortAModeSet(CARD_DETECT_GPIO, BackupMode);
	if(BackupPD)
	{
		GPIO_RegOneBitSet(CARD_DETECT_GPIO_PD, CARD_DETECT_BIT_MASK);	
	}

#ifdef CFG_RADIO_I2C_SD_SAME_PIN_EN
	if(!BackupPU)
	{
		GPIO_RegOneBitClear(CARD_DETECT_GPIO_PU, CARD_DETECT_BIT_MASK);
	}

	if(!BackupIE)
	{
		GPIO_RegOneBitClear(CARD_DETECT_GPIO_IE, CARD_DETECT_BIT_MASK);
	}
	if(BackupOE)
	{
		GPIO_RegOneBitSet(CARD_DETECT_GPIO_OE, CARD_DETECT_BIT_MASK);
	}
#endif

	SDIO_Clk_Eable(); //recover
	osMutexUnlock(SDIOMutex);

	return FindCard;

#else
	return FALSE;
#endif
}

bool IsLineInLink(void)
{
#ifdef CFG_LINEIN_DET_EN
	//#define LINEIN_JETTER_TIMES		10	//连接检测消抖时间：10次，100ms

	//static uint8_t LineInLinkState = 0;

	//设为输入，带上拉
	GPIO_RegOneBitClear(LINEIN_DET_GPIO_OE, LINEIN_DET_BIT_MASK);
	GPIO_RegOneBitSet(LINEIN_DET_GPIO_PU, LINEIN_DET_BIT_MASK);
	GPIO_RegOneBitClear(LINEIN_DET_GPIO_PD, LINEIN_DET_BIT_MASK);

	GPIO_RegOneBitSet(LINEIN_DET_GPIO_IE, LINEIN_DET_BIT_MASK);

	if(GPIO_RegOneBitGet(LINEIN_DET_GPIO_IN, LINEIN_DET_BIT_MASK))
	{
		//LineInLinkState = 0;						//断开状态不做消抖处理
		return FALSE;
	}
	//else
	//{
		//if(LineInLinkState < LINEIN_JETTER_TIMES)	//连接状态做消抖处理
		//{
		//	LineInLinkState++;
		//}
	//}
	//return (LineInLinkState >= LINEIN_JETTER_TIMES);
	return TRUE;
#else
    return TRUE;
#endif		
}


/**
 * @func        DeviceDetectCard
 * @brief       DeviceDetectCard no jitter
 * @param       void  
 * @Output      None
 * @return      PlugEvent
 * @Others      
 * Record
 * 1.Date        : 20180122
 *   Author      : pi.wang
 *   Modification: Created function
*/
#ifdef CFG_FUNC_CARD_DETECT
//static uint8_t sd_card_detect_xms_polling=0; //bkd add 2019.4.27
#endif

#ifdef CFG_COMMUNICATION_BY_USB
static uint32_t sDevice_Init_state=0;// 1:inited    0:not init
#endif

#ifdef CFG_APP_USB_AUDIO_MODE_EN
static uint8_t sUSB_Audio_Exit_Flag=0;
#endif

extern void NVIC_DisableIRQ(IRQn_Type IRQn);

uint32_t DeviceDetect(void)
{
	uint32_t Ret = 0;
	bool InOrOut;

	HWDeviceDected();
#ifdef CFG_LINEIN_DET_EN
	InOrOut = IsLineInLink();
	if(InOrOut)
	{
		Ret |= LINEIN_STATE_BIT;
	}
	switch(LineInState)
	{
		case DETECT_STATE_IDLE:
			if(InOrOut)
			{
				LineInState = DETECT_STATE_IN_JITTER;
				TimeOutSet(&DetectTimer, DETECT_JITTER_TIME);
			}
			else
			{
				Ret |= LINEIN_EVENT_BIT;
				LineInState = DETECT_STATE_OUT_JITTER;
				TimeOutSet(&DetectTimer, DETECT_OUT_WAIT_TIME);
			}
			break;
		case DETECT_STATE_IN:
			if(!InOrOut)
			{
				Ret |= LINEIN_EVENT_BIT;
				LineInState = DETECT_STATE_OUT_JITTER;
				TimeOutSet(&DetectTimer, DETECT_OUT_WAIT_TIME);
			}
			break;
		case DETECT_STATE_OUT_JITTER:
			if(InOrOut)
			{
				LineInState = DETECT_STATE_IN_JITTER;
				TimeOutSet(&DetectTimer, DETECT_JITTER_TIME);
			}
			else if(IsTimeOut(&DetectTimer))//卡弹出消抖完成
			{
				LineInState = DETECT_STATE_OUT;
			}
			break;
		case DETECT_STATE_OUT:
			if(InOrOut)
			{
				LineInState = DETECT_STATE_IN_JITTER;
				TimeOutSet(&DetectTimer, DETECT_JITTER_TIME);
			}
			break;
		case DETECT_STATE_IN_JITTER://卡插入消抖完成
			if(!InOrOut)
			{
				LineInState = DETECT_STATE_OUT_JITTER;
				TimeOutSet(&DetectTimer, DETECT_OUT_WAIT_TIME);
			}
			else if(IsTimeOut(&DetectTimer))
			{
				Ret |= LINEIN_EVENT_BIT;
				LineInState = DETECT_STATE_IN;
			}
	}
#endif

	//CARD插拔检测
#ifdef CFG_FUNC_CARD_DETECT
//if(sd_card_detect_xms_polling<6)sd_card_detect_xms_polling++;
//else
	{
	
		//sd_card_detect_xms_polling=0;
		
		InOrOut = IsCardIn();
		if(InOrOut)
		{
			Ret |= CARDIN_STATE_BIT;
		}
		switch(CardState)
		{
			case DETECT_STATE_IDLE:
				if(InOrOut)
				{
					CardState = DETECT_STATE_IN_JITTER;
					TimeOutSet(&DetectTimer, DETECT_JITTER_TIME);
					//				DBG("Detect:CardPlugInJitter\n");
				}
				else
				{
					Ret |= CARDIN_EVENT_BIT;
					CardState = DETECT_STATE_OUT_JITTER;
					TimeOutSet(&DetectTimer, DETECT_OUT_WAIT_TIME);
					//				DBG("Detect:CardPlugOutJitter\n");
				}
				break;
			case DETECT_STATE_IN:
				if(!InOrOut)
				{
					Ret |= CARDIN_EVENT_BIT;
					CardState = DETECT_STATE_OUT_JITTER;
					TimeOutSet(&DetectTimer, DETECT_OUT_WAIT_TIME);
					//				DBG("Detect:CardPlugOutJitter\n");
				}
				break;
			case DETECT_STATE_OUT_JITTER:
				if(InOrOut)
				{
					CardState = DETECT_STATE_IN_JITTER;
					TimeOutSet(&DetectTimer, DETECT_JITTER_TIME);
					//				DBG("CardPlugInJitter\n");
				}
				else if(IsTimeOut(&DetectTimer))//卡弹出消抖完成
				{
					//				DBG("Detect:CardPlugOut\n");
					CardState = DETECT_STATE_OUT;
				}
				break;
			case DETECT_STATE_OUT:
				if(InOrOut)
				{
					CardState = DETECT_STATE_IN_JITTER;
					TimeOutSet(&DetectTimer, DETECT_JITTER_TIME);
					//				DBG("Detect:CardPlugInJitter\n");
				}
				break;
			case DETECT_STATE_IN_JITTER://卡插入消抖完成
				if(!InOrOut)
				{
					CardState = DETECT_STATE_OUT_JITTER;
					TimeOutSet(&DetectTimer, DETECT_OUT_WAIT_TIME);
					//				DBG("Detect:CardPlugOutJitter\n");
				}
				else if(IsTimeOut(&DetectTimer))
				{
					//				DBG("Detect:CardPlugIn\n");
					Ret |= CARDIN_EVENT_BIT;
					CardState = DETECT_STATE_IN;
				}
				break;
		}
	}
#endif

#ifdef CFG_FUNC_UDISK_DETECT
#ifdef CFG_FUNC_USB_DEVICE_EN
	if(USB_Device_State != DETECT_STATE_IN)
#endif
	{
		//USB插拔检测
		InOrOut = IsUDiskLink();
		if(InOrOut)
		{
			Ret |= UDISKIN_STATE_BIT;
		}	
		switch(UDiskState)
		{
			case DETECT_STATE_IDLE:
				if(InOrOut)
				{
					UDiskState = DETECT_STATE_IN;
					Ret |= UDISKIN_EVENT_BIT;
					DBG("Detect:UDiskInPlug\n");
				}
				else
				{
					UDiskState = DETECT_STATE_OUT;
					DBG("Detect:NoUDisk\n");
				}

				break;
			case DETECT_STATE_IN:
				if(!InOrOut)
				{
					Ret |= UDISKIN_EVENT_BIT;
					UDiskState = DETECT_STATE_OUT;
					DBG("Detect:UDiskOut\n");
				}			
				break;
			case DETECT_STATE_OUT:
				if(InOrOut)
				{
					Ret |= UDISKIN_EVENT_BIT;
					UDiskState = DETECT_STATE_IN;
					DBG("Detect:UDiskIn\n");
				}		
				break;
			default:
				break;
		}
	}
#endif

#ifdef CFG_FUNC_USB_DEVICE_EN
#ifdef CFG_FUNC_UDISK_DETECT
	if(UDiskState != DETECT_STATE_IN)
#endif
	{		
	
		//USB插拔检测
		InOrOut = OTG_PortDeviceIsLink();
		if(InOrOut)
		{
			Ret |= USB_DEVICE_STATE_BIT;
		}
		switch(USB_Device_State)
		{
			case DETECT_STATE_IDLE:
				if(InOrOut)
				{
					USB_Device_State = DETECT_STATE_IN;
					Ret |= USB_DEVICE_EVENT_BIT;
					DBG("Detect:Is usb device\n");

#ifdef CFG_COMMUNICATION_BY_USB
					if(sDevice_Init_state==0)
					{
						if(GetSystemMode() == AppModeUsbDevicePlay)
							OTG_DeviceModeSel(AUDIO_HID,0x8888,0x1234);
						else
							OTG_DeviceModeSel(HID,0x8888,0x1234);

						OTG_DeviceInit();
						NVIC_EnableIRQ(Usb_IRQn);
						sDevice_Init_state=1;
					}
#endif
				}
				else
				{
					USB_Device_State = DETECT_STATE_OUT;
					DBG("Detect:Not usb device\n");

#ifdef CFG_COMMUNICATION_BY_USB
					if(sDevice_Init_state==1)
					{
						NVIC_DisableIRQ(Usb_IRQn);
						OTG_DeviceDisConnect();
						sDevice_Init_state=0;
					}
#endif
				}
				break;
			case DETECT_STATE_IN:
				if(!InOrOut)
				{
					Ret |= USB_DEVICE_EVENT_BIT;
					USB_Device_State = DETECT_STATE_OUT;
					DBG("Detect:Out as usb device\n");

#ifdef CFG_COMMUNICATION_BY_USB
					if(sDevice_Init_state==1)
					{
						NVIC_DisableIRQ(Usb_IRQn);
						OTG_DeviceDisConnect();
						sDevice_Init_state=0;
					}
#endif
				}
				break;
			case DETECT_STATE_OUT:
				if(InOrOut)
				{
					Ret |= USB_DEVICE_EVENT_BIT;
					USB_Device_State = DETECT_STATE_IN;
					DBG("Detect:Plus as usb device\n");

#ifdef CFG_COMMUNICATION_BY_USB
					if(sDevice_Init_state==0)
					{
						if(GetSystemMode() == AppModeUsbDevicePlay)
							OTG_DeviceModeSel(AUDIO_HID,0x8888,0x1234);
						else
							OTG_DeviceModeSel(HID,0x8888,0x1234);

						OTG_DeviceInit();
						NVIC_EnableIRQ(Usb_IRQn);
						sDevice_Init_state=1;
					}
#endif
				}
				break;
			default:
				break;
		}
	}
#endif
	return Ret;
}


#ifdef CFG_FUNC_UDISK_DETECT
bool IsUDiskLink(void)
{
//extern osMutexId MuteReadBlock;// = NULL;
//extern osMutexId MuteWriteBlock;// = NULL;

	bool IsUDiskLink;

	IsUDiskLink = OTG_PortHostIsLink();

//    if(IsUDiskLink == FALSE)
//    {
//        osMutexUnlock(MuteWriteBlock);
//        osMutexUnlock(MuteReadBlock);
//    }

    return IsUDiskLink;
}
#endif



// 上电时，硬件扫描消抖
void InitDeviceDetect(void)
{
#ifdef FUNC_OS_EN
	if(SDIOMutex == NULL)
		SDIOMutex = osMutexCreate();
#endif

/*
 * give 20 ms enough to device detection
 */

//#ifdef CFG_FUNC_CARD_DETECT
//	IsCardInFlag = IsCardIn();
//#endif

//#ifdef CFG_FUNC_UDISK_DETECT
//	UDiskState = DETECT_STATE_IDLE;
//	IsUDiskLinkFlag = IsUDiskLink();
//	DBG("IsUDiskLinkFlag = %d\n", IsUDiskLinkFlag);
//#endif
//		IsLineInLink();
#ifdef FUNC_MIC_DET_EN
	IsMicLinkFlag = IsMicInLink();
#endif
	HWDeviceDected_Init();
}

#if (defined(CFG_APP_USB_AUDIO_MODE_EN))&&(defined(CFG_COMMUNICATION_BY_USB))
void SetDeviceDetectVarReset(void)
{
	UDiskState=DETECT_STATE_IDLE;
	USB_Device_State=DETECT_STATE_IDLE;
	sDevice_Init_state=0;
}
#endif
#ifdef CFG_APP_USB_AUDIO_MODE_EN
void SetDeviceInitState(uint32_t init_state)
{
	sDevice_Init_state=init_state;
}
#endif
#ifdef CFG_COMMUNICATION_BY_USB
uint32_t GetDeviceInitState(void)
{
	return sDevice_Init_state; 	
}
#endif

#if (defined(CFG_APP_USB_AUDIO_MODE_EN))&&(defined(CFG_COMMUNICATION_BY_USB))
void SetUSBAudioExitFlag(uint8_t flag)
{
	sUSB_Audio_Exit_Flag=flag;
}
uint8_t GetUSBAudioExitFlag(void)
{
	return sUSB_Audio_Exit_Flag;
}
#endif



