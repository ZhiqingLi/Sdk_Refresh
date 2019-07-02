///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: line_in_control.c
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "breakpoint.h"
#include "i2s.h"
#include "audio_path.h"
#include "pcm_fifo.h"
#include "task_decoder.h"
#include "bt_app_func.h"
#include "bt_control_api.h"
#include "dev_detect_driver.h"
#include "sd_card.h"
#include "eq.h"
#include "eq_params.h"
#include "clk.h"
#include "fsinfo.h"
#include "sys_vol.h"
#include "sound_remind.h"
#include "mixer.h"
#include "dac.h"
#include "breakpoint.h"
#include "recorder_control.h"
#include "nvm.h"
#include "sys_app.h"
#include "browser.h"
#include "lrc.h"
#include "eq.h"
#include "eq_params.h"
#include "timer_api.h"
#include "string_convert.h"
#include "i2s_in_control.h"
#include "player_control.h"
#include "type.h"
#include "gpio.h"
#include "uart.h"
#include "watchdog.h"
#include "get_bootup_info.h"
#include "rtc_control.h"
#include "wifi_uart_com.h"
#include "wifi_control.h"
#include "wifi_init_setting.h"
#include "wifi_mcu_command.h"
#include "wifi_wifi_command.h"

#ifdef FUNC_DISP_EN
#include "led_display.h"
#include "seg_panel.h"
#include "seg_led_disp.h"
#include "singled_display.h"
#endif

#ifdef FUNC_WIFI_EN
#ifdef FUNC_WIFI_POWER_KEEP_ON
#define WIFI_POWER_ON_INIT_TIME		400	//WiFi模组上电后的初始化持续时间(单位: 10ms)
#else
#define WIFI_POWER_ON_INIT_TIME		60	//6s(WiFi模组上电后的初始化持续时间)
#endif
#define WIFI_LED_CB_TIME			50	 //设置WiFiLedCb()函数调用时间间隔(单位:ms)
#define WIFI_LED_SLOW_FLASH_TIME	1000 //设置LED慢闪烁时间(单位:ms)
#define WIFI_LED_FAST_FLASH_TIME	100 //设置LED快闪烁时间(单位:ms)
#define WIFI_POWERON_RECONNECTION_TIME		600	//60s(WiFi模组上电后的自动回连AP时间)

uint16_t McuSoftSdkVerNum = 0;	//MCU软件版本号

WIFI_WORK_STATE gWiFi;
TIMER WiFiCmdRespTimer;	//WiFi模组命令响应时间
TIMER WiFiPowerOffTimer;
TIMER WiFiLedBlinkTimer;
TIMER WiFiSoundRemindTimer;

static bool WiFiPowerOnInitEndFlag;
static uint16_t WiFiPowerOnInitTimeCnt;
uint8_t gCurNumberKeyNum = 0;
bool WiFiTalkOnFlag;
uint8_t McuCurPlayMode = 0;				//Mcu当前播放模式
uint8_t WiFiLedFlashCnt; 				//WiFi LED指示灯闪烁计数
uint16_t PassThroughDataLen = 0;  		//透传数据长度,此数值不为0表示有透传数据接收
uint8_t PassThroughDataState = 0; 		//透传数据状态
int8_t  ChangeSystemTimeZoneFlag = 0;	//MCU修改时区时是否有日期变更，用于修改周设置

#ifdef FUNC_POWER_MONITOR_EN
#ifdef FUNC_WIFI_BATTERY_POWER_CHECK_EN	
uint8_t GetCurBatterLevelAverage(void);
#endif
#endif

#ifdef	FUNC_WIFI_UART_UPGRADE
#define UPGRADE_CODE_BANK_ADDR	0x100000	//升级代码在Spi Flash中存放的起始地址
#define UPGRADE_NVM_ADDR        (176) 	//boot upgrade information at NVRAM address

#define UPGRADE_DATA_BUF_LEN		1024

uint8_t Upgrade_Data_Buf[UPGRADE_DATA_BUF_LEN];
uint8_t UpgradeSoftProjectName[32];   //WiFi 发送的MCU升级软件项目名
uint16_t UpgradeSoftVerMsg;	//升级软件版本
uint32_t UpgradeSoftTotalSize;	//升级软件代码大小
uint16_t SoftBankIndex;
uint32_t UpgradeSoftRecvLen;
static bool UpgradeDataStartEnFlag = FALSE;

static const uint16_t CrcCCITTTable[256] =
{
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
	0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
	0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
	0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
	0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
	0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
	0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
	0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
	0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
	0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
	0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
	0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
	0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
	0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
	0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
	0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
	0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
	0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
	0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
	0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
	0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
	0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
	0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

static uint16_t CRC16(const uint8_t* Buf, uint32_t BufLen, uint16_t CRC)
{
	uint32_t i;

	for (i = 0 ; i < BufLen ; i++) 
	{
		CRC = (CRC << 8) ^ CrcCCITTTable[((CRC >> 8) ^ *(uint8_t*)Buf ++) & 0x00FF];
	}
	return CRC;
}

bool DualBankDataSavenCRC(uint32_t DataLen)
{
	uint32_t Remine_Len, Recv_Len = 0;
	uint32_t Addr;
	uint16_t Crc = 0;

	//flash data crc check
	Remine_Len = DataLen;
	Addr = UPGRADE_CODE_BANK_ADDR;	

	while(Remine_Len > 0)
	{
		Recv_Len = (Remine_Len > UPGRADE_DATA_BUF_LEN)? UPGRADE_DATA_BUF_LEN : Remine_Len;
		
		SpiFlashRead(Addr, Upgrade_Data_Buf, Recv_Len); 
		if(Remine_Len <= UPGRADE_DATA_BUF_LEN)
		{
			Crc = CRC16((const uint8_t*)Upgrade_Data_Buf, Recv_Len - 4, Crc);
		}
		else
		{
			Crc = CRC16((const uint8_t*)Upgrade_Data_Buf, Recv_Len, Crc);
		}
		Addr += Recv_Len;
		Remine_Len -= Recv_Len;
	}	
	
	//compare the crc calculated with CRC in upgarde ball(last 4bytes)
	if(Crc != (*(uint32_t*)(UPGRADE_CODE_BANK_ADDR + UpgradeSoftTotalSize - 4)))
	{
		DBG("CRC error!\r\n");
		return FALSE;
	}
	
	DBG("Dual Bank Update data received successed, CRC16 OK!\r\n");	

	return TRUE;
}

void DualBankDataSaveFlash(uint16_t DataLen)
{
	uint32_t Addr;

	Addr = UPGRADE_CODE_BANK_ADDR + UpgradeSoftRecvLen - DataLen;
	
	SpiFlashWrite(Addr, Upgrade_Data_Buf, DataLen);
}

bool DualBankFlashErase(void)
{
#ifdef FUNC_WATCHDOG_EN
	WdgDis();				// disable watch dog
#endif
#ifdef FUNC_AMP_MUTE_EN
	WiFiSoundRemindStateSet(TRUE);
	gSys.MuteFlag = TRUE;
	AmpMuteControl(TRUE);
#endif

	DBG("DualBankFlashErase Start.........!\r\n");
	//erase dual bank update flash area
	if(SpiFlashErase(UPGRADE_CODE_BANK_ADDR, UPGRADE_CODE_BANK_ADDR) != FLASH_NONE_ERR)
	{
		DBG("Dual Bank Update: Flash erase FAILED!\r\n");
		return FALSE;
	}
    DBG("DualBankFlashErase OK.........!\r\n");
#ifdef FUNC_AMP_MUTE_EN
	gSys.MuteFlag = FALSE;
#endif
	return TRUE;
}

void DualBankDataUpgrade(uint32_t DataLen)
{	
	uint32_t BootNvmInfo;
	
	if(1)//DualBankDataSavenCRC(DataLen)) //加密升级文件做CRC校验不通过，同时BOOT升级时会做CRC校验，因此应用层取消
	{
#ifdef FUNC_AMP_MUTE_EN
		WiFiSoundRemindStateSet(TRUE);
		gSys.MuteFlag = TRUE;
		AmpMuteControl(TRUE);
#endif
#ifdef FUNC_BREAKPOINT_EN	
        {
            BP_SYS_INFO *pSysInfo;

            pSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);
            gSys.UpgradeFileSource = UPGRADE_SOURCE_WIFI;
            BP_SET_ELEMENT(pSysInfo->UpgradeFileSource, gSys.UpgradeFileSource);
            BP_SaveInfo(BP_SAVE2NVM);
#ifdef BP_SAVE_TO_FLASH // 掉电记忆
            BP_SaveInfo(BP_SAVE2FLASH);
#endif
        }
#endif
		APP_DBG("[UPGRADE]: dual bank flash prepare to boot upgrade\n");
		//write upgrade ball offset address to NVM(176), and then reset system
		BootNvmInfo = UPGRADE_CODE_BANK_ADDR;
		NvmWrite(UPGRADE_NVM_ADDR, (uint8_t*)&BootNvmInfo, 4);
	    //if you want PORRESET to reset GPIO only,uncomment it
        GpioPorSysReset(GPIO_RSTSRC_PORREST);
		NVIC_SystemReset();
		while(1);		
	}
}

//WiFi升级MCU软件信息
void WiFiUpgradeMcuSoftMsg(uint8_t* MsgData)
{
	static uint8_t MsgPos, RcvCnt, Temp;
	static bool SoftVerRcvStartFlag;
	static bool SoftSizeRcvStartFlag;
	static bool SoftNameRcvStartFlag;
	uint8_t i;

	if(WiFiDataRcvStartFlag == FALSE)
	{
		MsgPos = 0;
		RcvCnt = 0;
		Temp = 0;
		SoftVerRcvStartFlag = TRUE;
		SoftSizeRcvStartFlag = FALSE;
		SoftNameRcvStartFlag = FALSE;
		WiFiDataRcvStartFlag = TRUE;
		UpgradeSoftVerMsg = 0;
		UpgradeSoftTotalSize = 0;
		return;
	}

//软件版本信息数据
	if(SoftVerRcvStartFlag == TRUE)
	{	
		if(MsgData[Temp] == ':')
		{
			SoftVerRcvStartFlag = FALSE;
			SoftSizeRcvStartFlag = TRUE;
			RcvCnt = 0;	
			Temp += 1;
			MsgPos = Temp;
			return;
		}
		else
		{
			if(RcvCnt == 0)
			{
				UpgradeSoftVerMsg += (MsgData[Temp++] - 0x30) * 1000;
			}
			else if(RcvCnt == 1)
			{
				UpgradeSoftVerMsg += (MsgData[Temp++] - 0x30) * 100;
			}
			else if(RcvCnt == 2)
			{
				UpgradeSoftVerMsg += (MsgData[Temp++] - 0x30) * 10;
			}
			else if(RcvCnt == 3)
			{
				UpgradeSoftVerMsg += MsgData[Temp++] - 0x30;
			}
			RcvCnt++;
		}		
	}

//软件代码文件大小数据
	if(SoftSizeRcvStartFlag == TRUE)
	{			
		if(MsgData[Temp++] == ':')
		{			
			for(i = 0; i < RcvCnt; i++)
			{
				if(i == (RcvCnt - 1))
				{
					UpgradeSoftTotalSize += MsgData[MsgPos++] - 0x30;
				}
				else if(i == (RcvCnt - 2))
				{
					UpgradeSoftTotalSize += (MsgData[MsgPos++] - 0x30) * 10;
				}
				else if(i == (RcvCnt - 3))
				{
					UpgradeSoftTotalSize += (MsgData[MsgPos++] - 0x30) * 100;
				}
				else if(i == (RcvCnt - 4))
				{
					UpgradeSoftTotalSize += (MsgData[MsgPos++] - 0x30) * 1000;
				}
				else if(i == (RcvCnt - 5))
				{
					UpgradeSoftTotalSize += (MsgData[MsgPos++] - 0x30) * 10000;
				}
				else if(i == (RcvCnt - 6))
				{
					UpgradeSoftTotalSize += (MsgData[MsgPos++] - 0x30) * 100000;
				}
				else if(i == (RcvCnt - 7))
				{
					UpgradeSoftTotalSize += (MsgData[MsgPos++] - 0x30) * 1000000;
				}
				else if(i == (RcvCnt - 8))
				{
					UpgradeSoftTotalSize += (MsgData[MsgPos++] - 0x30) * 10000000;
				}
				else if(i == (RcvCnt - 9))
				{
					UpgradeSoftTotalSize += (MsgData[MsgPos++] - 0x30) * 100000000;
				}
			}	

			RcvCnt = 0;
			SoftSizeRcvStartFlag = FALSE;
			SoftNameRcvStartFlag = TRUE;	
			return;
		}
		else
		{
			RcvCnt++;
		}
	}
	
//项目名称数据
	if(SoftNameRcvStartFlag == TRUE)
	{
		if(MsgData[Temp] == '&')
		{
			RcvCnt = 0;
			SoftNameRcvStartFlag = FALSE;			
			//MCU同意升级
			UpgradeSoftRecvLen = 0;
			UpgradeDataStartEnFlag = FALSE;
			WiFiDataRcvStartFlag = FALSE;	
			WaitMs(500);
			RxDataEnableFlag = FALSE;
			WaitMs(2000);
			DualBankFlashErase();
#ifdef FUNC_WATCHDOG_EN
			WdgEn(WDG_STEP_4S);				
#endif
			WaitMs(100);
			ClearRxQueue();
			RxDataEnableFlag = TRUE;
			SlaveRxIndex = -3;		
			WaitMs(100);
			Mcu_SendCmdToWiFi(MCU_UPG_STA, NULL);	
		}
		else
		{
			UpgradeSoftProjectName[RcvCnt++] = MsgData[Temp++];
		}
	}
}

//WiFi进行升级MCU软件
void WiFiUpgradeMcuSoftRunning(uint8_t* UpdateData)
{
	static uint16_t DataLen, RcvCnt, Temp;
	static uint16_t DataPos;
	static bool SoftBankIndexRcvStartFlag;
	static bool SoftBankLenRcvStartFlag;
	static bool SoftBankDataRcvStartFlag;
	static bool SoftBankUpgradeStartFlag;		
	static uint32_t RevCheckSum;
	static uint8_t CheckSumErrCnt;
	uint16_t i, j;	
	uint32_t* Pstr;
	uint32_t CheckSum;	
	
	if(WiFiDataRcvStartFlag == FALSE)
	{
		DataPos = 0;
		Temp = 0;
		RcvCnt = 0;
		DataLen = 0;
		RevCheckSum = 0;
		SoftBankIndex = 0;
		CheckSumErrCnt = 0;
		SoftBankIndexRcvStartFlag = TRUE;
		SoftBankLenRcvStartFlag = FALSE;
		SoftBankDataRcvStartFlag = FALSE;
		SoftBankUpgradeStartFlag = FALSE;
		WiFiDataRcvStartFlag = TRUE;
		return;
	}

//升级软件数据包数
	if(SoftBankIndexRcvStartFlag == TRUE)
	{
		if(UpdateData[Temp++] == ':')
		{
			for(i = 0; i < RcvCnt; i++)
			{
				if(i == (RcvCnt - 1))
				{
					SoftBankIndex += UpdateData[DataPos++] - 0x30;
				}
				else if(i == (RcvCnt - 2))
				{
					SoftBankIndex += (UpdateData[DataPos++] - 0x30) * 10;
				}
				else if(i == (RcvCnt - 3))
				{
					SoftBankIndex += (UpdateData[DataPos++] - 0x30) * 100;
				}	
				else if(i == (RcvCnt - 4))
				{
					SoftBankIndex += (UpdateData[DataPos++] - 0x30) * 1000;
				}	
			}	
			SoftBankIndexRcvStartFlag = FALSE;
			SoftBankLenRcvStartFlag = TRUE;
			RcvCnt = 0;
			DataPos = Temp;
			return;
		}
		else
		{			
			RcvCnt++;
		}
	}

//升级软件数据包长度	
	if(SoftBankLenRcvStartFlag == TRUE)
	{
		if(UpdateData[Temp++] == ':')
		{			
			for(i = 0; i < RcvCnt; i++)
			{
				if(i == (RcvCnt - 1))
				{
					DataLen += UpdateData[DataPos++] - 0x30;
				}
				else if(i == (RcvCnt - 2))
				{
					DataLen += (UpdateData[DataPos++] - 0x30) * 10;
				}
				else if(i == (RcvCnt - 3))
				{
					DataLen += (UpdateData[DataPos++] - 0x30) * 100;
				}	
				else if(i == (RcvCnt - 4))
				{
					DataLen += (UpdateData[DataPos++] - 0x30) * 1000;
				}	
			}	
			
			RcvCnt = 0;
			DataPos = Temp;			
			SoftBankLenRcvStartFlag = FALSE;
			SoftBankDataRcvStartFlag = TRUE;
			return;
		}
		else
		{
			RcvCnt++;
		}
	}

//升级软件数据
	if(SoftBankDataRcvStartFlag == TRUE)
	{		
		if(UpdateData[Temp++] == '&')
		{
			//解析四字节的Checksum
			Temp = 0;
			for(i = 0; i < 8; i++)
			{				
				if((UpdateData[i + DataPos] == '0') && (UpdateData[i + DataPos + 1] == '0'))
				{   
					Upgrade_Data_Buf[Temp++] = '0'; 
					i++; 
				}
				else if((UpdateData[i + DataPos] == '0') && (UpdateData[i + DataPos + 1] == '1'))
				{ 
					Upgrade_Data_Buf[Temp++] = 0; 
					i++;
				}
				else if((UpdateData[i + DataPos] == '0') && (UpdateData[i + DataPos + 1] == '2'))
				{ 
					Upgrade_Data_Buf[Temp++] = '&'; 
					i++; 
				}
				else if((UpdateData[i + DataPos] == '0') && (UpdateData[i + DataPos + 1] == '3'))
				{ 
					Upgrade_Data_Buf[Temp++] = '+'; 
					i++; 
				}
				else 
				{
					Upgrade_Data_Buf[Temp++] = UpdateData[i + DataPos];
				}					
				
				if(Temp >= 4)
				{
					break;
				}				
				
			}
			Pstr = (uint32_t*)&Upgrade_Data_Buf[0];
			RevCheckSum = *Pstr;
			Temp = i + 1;
			memset(Upgrade_Data_Buf, 0, sizeof(Upgrade_Data_Buf));
			j = 0;

			for(i = Temp; i < RcvCnt; i++)
			{				
				if((UpdateData[i + DataPos] == '0') && (UpdateData[i + DataPos + 1] == '0'))
				{   
					Upgrade_Data_Buf[j++] = '0'; 
					i++; 
				}
				else if((UpdateData[i + DataPos] == '0') && (UpdateData[i + DataPos + 1] == '1'))
				{ 
					Upgrade_Data_Buf[j++] = 0; 
					i++;
				}
				else if((UpdateData[i + DataPos] == '0') && (UpdateData[i + DataPos + 1] == '2'))
				{ 
					Upgrade_Data_Buf[j++] = '&'; 
					i++; 
				}
				else if((UpdateData[i + DataPos] == '0') && (UpdateData[i + DataPos + 1] == '3'))
				{ 
					Upgrade_Data_Buf[j++] = '+'; 
					i++; 
				}
				else 
				{
					Upgrade_Data_Buf[j++] = UpdateData[i + DataPos];
				}	
			}	
			
			SoftBankUpgradeStartFlag = TRUE;
			SoftBankDataRcvStartFlag = FALSE;
		}
		else
		{
			RcvCnt++;			
		}
	}
	
//计算升级软件数据校验和
	if(SoftBankUpgradeStartFlag == TRUE)
	{	
		CheckSum = 0;
		for(i = 0; i < DataLen; i+= 4)
		{
			Pstr = (uint32_t*)&Upgrade_Data_Buf[i];
			CheckSum += *Pstr;
		}

		if(RevCheckSum == CheckSum)
		{
			CheckSumErrCnt = 0;
			UpgradeSoftRecvLen += DataLen;
			DualBankDataSaveFlash(DataLen);
			if(UpgradeSoftRecvLen >= UpgradeSoftTotalSize)
			{
			  APP_DBG("UpgradeSoftRecvLen = %d, UpgradeSoftTotalSize = %d\n", UpgradeSoftRecvLen,UpgradeSoftTotalSize);	
				//代码是否加密判断
				//Flash bin文件: 地址0x000000FC的值不同, 未加密是0xFFFFFFFF; 加密是0xFFFFFF55 
				//MVA升级文件:	地址0x00000112的值不同, 未加密是0xFFFFFFFF;  加密是0xFFFFFF55	
				SpiFlashRead(0xFC, (uint8_t*)&CheckSum, 4); 
				SpiFlashRead(UPGRADE_CODE_BANK_ADDR + 0x112, (uint8_t*)&RevCheckSum, 4);
				if(CheckSum != RevCheckSum) //当前固件和升级固件是否加密不一致
				{
					DBG("Upgrade Firmware error!\r\n");
					RcvCnt = 2; //加密标记不匹配
					Mcu_SendCmdToWiFi(MCU_UPG_FAL,(uint8_t *)&RcvCnt);	
				}
				else	
				{
					Mcu_SendCmdToWiFi(MCU_UPG_END, NULL);
					UpgradeDataStartEnFlag = TRUE;
				}
			}
			else
			{
				Mcu_SendCmdToWiFi(MCU_UPG_RUN, UpdateData);			
			}
		}
		else
		{
			CheckSumErrCnt++;
			if(CheckSumErrCnt > 5)	//一个数据包重发次数超过5次终止升级
			{
				RcvCnt = 1; //checksum不正确
				Mcu_SendCmdToWiFi(MCU_UPG_FAL,(uint8_t *)&RcvCnt);	
			}
			else
			{
				if(SoftBankIndex)
				{
					SoftBankIndex--;
				}
				else
				{
					SoftBankIndex = 0x0FFF;
				}
				Mcu_SendCmdToWiFi(MCU_UPG_RUN, UpdateData);		
			}
		}
		
		SoftBankUpgradeStartFlag = FALSE;
		WiFiDataRcvStartFlag = FALSE;
	}
}
#endif

//获取WiFi模组上电初始化时间结束状态
bool WiFiPowerOnInitStateGet(void)
{
	return WiFiPowerOnInitEndFlag; 
}

//WiFi模组工作状态设置
void WiFiWorkStateSet(uint8_t State)
{
	switch(State)
	{
		case WIFI_STATUS_INITING:
			gWiFi.InitState = 0;
			break;
			
		case WIFI_STATUS_INIT_END:
			gWiFi.InitState = 1;
#ifdef	FUNC_WIFI_UART_UPGRADE
			if(UpgradeDataStartEnFlag == TRUE)
			{
				UpgradeDataStartEnFlag = FALSE;
				DualBankDataUpgrade(UpgradeSoftTotalSize);	
			}
#endif
			break;

		case WIFI_STATUS_REBOOT_MCU:
			WiFiFirmwareUpgradeStateSet(0);
#ifdef	FUNC_WIFI_UART_UPGRADE
			if(UpgradeDataStartEnFlag == TRUE)
			{
				UpgradeDataStartEnFlag = FALSE;
				DualBankDataUpgrade(UpgradeSoftTotalSize);	
			}
#endif
			break;			
			
		case WIFI_STATUS_PLAY_PAUSE:
			gWiFi.PlayState = 0;
			break;
			
		case WIFI_STATUS_PLAY_PLAYING:
			gWiFi.PlayState = 1;
			break;
	}
}

//WiFi 凯叔故事机儿童锁设置
void WiFiKaiShuChildLockStateSet(bool State)
{
	gWiFi.KaiShuChildLock = State;
}

//WiFi 获取凯叔故事机儿童锁状态
bool WiFiKaiShuChildLockStateGet(void)
{
	return gWiFi.KaiShuChildLock;
}

//WiFi 凯叔故事机儿童锁设置
void WiFiKaiShuVolumeMaxSet(  uint8_t       Vol)
{
	gWiFi.KaiShuVolumeMax = Vol;
	if(gWiFi.KaiShuVolumeMax < ((100 / MAX_VOLUME) * gSys.Volume))
	{
		gSys.Volume = (MAX_VOLUME * gWiFi.KaiShuVolumeMax) / 100;
		SetSysVol();
		Mcu_SendCmdToWiFi(MCU_CUR_VOL, &gWiFi.KaiShuVolumeMax);
	}
}

//WiFi 获取凯叔故事机儿童锁状态
uint8_t WiFiKaiShuVolumeMaxGet(void)
{
	return gWiFi.KaiShuVolumeMax;
}

//WiFi 凯叔故事机电台设置
void WiFiKaiShuRadioSet(  uint8_t val)
{
	gWiFi.KaiShuRadio = val;
}

//WiFi 凯叔故事机哄睡模式处理
uint8_t WiFiKaiShuSleepModeDeal(bool IsEnter, bool IsPushOut)
{
	#define SLEEP_VOLUME_VAL      3
	static uint8_t TempVol = 0;
	extern const uint16_t gAnaVolArr[MAX_VOLUME + 1];

	if((TRUE == IsEnter) && (FALSE == IsPushOut))
	{
		TempVol = gSys.Volume;	
		APP_DBG("Kai shu Enter sleep mode!!!\n");
	}
	else if((FALSE == IsEnter) && (TRUE == IsPushOut))
	{
		SetSysVol();
		TempVol = FALSE;
		APP_DBG("Kai shu push out sleep mode!!!\n");
	}
	else if(TempVol > SLEEP_VOLUME_VAL)
	{
		TempVol--;
		MixerConfigVolume(MIXER_SOURCE_ANA_MONO, gAnaVolArr[TempVol], gAnaVolArr[TempVol]);
		MixerConfigVolume(MIXER_SOURCE_ANA_STERO, gAnaVolArr[TempVol], gAnaVolArr[TempVol]); 
		APP_DBG("Kai shu sleep mode volume down!!! %d;\n", TempVol);
	}

	return gSys.Volume-SLEEP_VOLUME_VAL;
}

//WiFi模组WPS搜索状态设置
void WiFiWpsStateSet(uint8_t State)
{
	gWiFi.WPSState = State;
}

//WiFi模组连接路由器的状态设置
void WiFiStationStateSet(uint8_t State)
{
	gWiFi.StationState = State;
}

//WiFi模组以太网连接状态设置
void WiFiEthStateSet(uint8_t State)
{
	gWiFi.ETHState = State;
}

//WiFi模组互联网络连接状态设置
void WiFiWwwStateSet(uint8_t State)
{
#ifdef FUNC_BREAKPOINT_EN	
	BP_SYS_INFO *pSysInfo;

	pSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);
	gSys.IsWiFiRepeatPowerOn = TRUE;
	BP_SET_ELEMENT(pSysInfo->IsWiFiRepeatPowerOn, gSys.IsWiFiRepeatPowerOn);
	BP_SaveInfo(BP_SAVE2NVM);
#ifdef BP_SAVE_TO_FLASH // 掉电记忆
	BP_SaveInfo(BP_SAVE2FLASH);
#endif
#endif

	gWiFi.WWWState = State;
}

uint8_t WiFiWwwStateGet(void)
{
	return gWiFi.WWWState;
}

//WiFi模组AP模式热点的被连接状态设置
void WiFiRa0StateSet(uint8_t State)
{
	gWiFi.APState = State;
}

//WiFi模组AP模式热点的被连接状态获取
uint8_t WiFiRa0StateGet()
{
	return gWiFi.APState;
}


//WiFi恢复出厂设置状态
void WiFiFactoryStateSet(bool State)
{
	gWiFi.FactoryState = State;
}

//获取WiFi恢复出厂设置状态
bool WiFiFactoryStateGet(void)
{
	return gWiFi.FactoryState;
}

//WiFi固件升级状态设置
void WiFiFirmwareUpgradeStateSet(uint8_t State)
{
	gWiFi.FirmwareState = State;
}

//获取WiFi固件升级设置状态
uint8_t WiFiFirmwareUpgradeStateGet(void)
{
	return gWiFi.FirmwareState;
}

//WiFi模组服务器连接状态设置
void WiFiLoginStateSet(uint8_t State)
{
	gWiFi.LoginState = State;
}

uint8_t WiFiLoginStateGet(void)
{
	return gWiFi.LoginState;
}

//WiFi请求MCU 关机
void WiFiRequestMcuPowerOff(void)
{
	static uint8_t PowerOffCnt = 0;
	
	if(WiFiFirmwareUpgradeStateGet() != TRUE)
	{	
		//断点信息保存
		AudioSysInfoSetBreakPoint();
		PowerOffCnt++;
		if((PowerOffCnt >= 2) || (gWiFi.InitState == WIFI_STATUS_IDLE))
		{
			APP_DBG("McuWillPowerOff.......\n");
			PowerOffCnt = FALSE;
			gSys.NextModuleID = MODULE_ID_POWEROFF;
			MsgSend(MSG_COMMON_CLOSE);
		}
		else
		{
			APP_DBG("McuRequestWiFiPowerOff.......\n");
			PowerOffCnt = 1;
			Mcu_SendCmdToWiFi(MCU_POW_OFF, NULL);
		}
		gWiFi.WiFiPowerOffRequestFlag = TRUE;
		TimeOutSet(&WiFiPowerOffTimer, 5000);	
	}
}

//WiFi 端上电状态设置[000： 正常  001： 省电模式(RF关闭)    002：固件升级中 003：设备重启中]
void WiFiPowerStateSet(uint8_t* State)
{
	gWiFi.PowerState = State[2] - 0x30;
}

//WiFi 产测模式状态设置
void WiFiTestModeStateSet(void)
{
	gWiFi.TestModeState = 1;
}

//当前WiFi模组连接的子音箱数设置
void WiFiSlaveSounBoxCntSet(uint16_t SoundBoxCnt)
{
#ifdef FUNC_WIFI_MULTIROOM_PLAY_EN
	if((gWiFi.SlaveSoundBoxCnt && (SoundBoxCnt == 0)) || ((gWiFi.SlaveSoundBoxCnt == 0) && SoundBoxCnt))
	{
		if((gSys.CurModuleID != MODULE_ID_WIFI) && (gSys.CurModuleID != MODULE_ID_PLAYER_WIFI_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_WIFI_USB))
		{
			gSys.NextModuleID = gSys.CurModuleID;
			MsgSend(MSG_MODE);
		}
	}
	
	gWiFi.SlaveSoundBoxCnt = SoundBoxCnt;
#endif
}

//获取当前WiFi模组连接的子音箱数
uint8_t WiFiSlaveSounBoxCntGet(void)
{
	return gWiFi.SlaveSoundBoxCnt;
}

//WiFi 多房间子音箱状态设置
void WiFiMutliRoomStateSet(bool State)
{
#ifdef FUNC_WIFI_MULTIROOM_PLAY_EN
	if(State && (gSys.CurModuleID != MODULE_ID_WIFI))
	{
		gSys.NextModuleID = MODULE_ID_WIFI;
		MsgSend(MSG_MODE);
	}
	gWiFi.MutliRoomState = State;
#endif
}

//获取当前WiFi 模组是否处于子音箱状态
bool WiFiMutliRoomStateGet(void)
{
	return gWiFi.MutliRoomState;
}

//WiFi 端USB插入状态设置(插入和拔出都会连续发送两次状态)
void WiFiUSBStateSet(bool State)
{
	
	if(State == WIFI_STATUS_USB_PLUGIN)
	{
		if(gSys.CurModuleID != MODULE_ID_PLAYER_WIFI_USB)
		{
			gSys.NextModuleID = MODULE_ID_PLAYER_WIFI_USB;	
			MsgSend(MSG_MODE);	//拔出TF卡WiFi模组不会发送切换模式命令，需要MCU自动切换
		}
	}
	else if(State == WIFI_STATUS_USB_PLUGOUT)
	{	
		if(gSys.CurModuleID == MODULE_ID_PLAYER_WIFI_USB)
		{
			MsgSend(MSG_MODE);	//拔出USB设备WiFi模组不会发送切换模式命令，需要MCU自动切换
		}
	}
	
  gWiFi.USBInsertState = State;	
}

bool IsWiFiUSBLink(void)
{
	if(gWiFi.USBInsertState == WIFI_STATUS_USB_PLUGIN)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//WiFi 端TF卡插入状态设置(插入和拔出都会连续发送两次状态)
void WiFiCardStateSet(bool State)
{
	static bool IsPowerCardState = FALSE;
	if(gWiFi.CardInsertState != State)
	{
		if(State == WIFI_STATUS_SD_PLUGIN)
		{
			if(IsPowerCardState && gSys.CurModuleID != MODULE_ID_PLAYER_WIFI_SD)
			{
				gSys.NextModuleID = MODULE_ID_PLAYER_WIFI_SD;	
				MsgSend(MSG_MODE);	//拔出TF卡WiFi模组不会发送切换模式命令，需要MCU自动切换
			}
		}
		else if(State == WIFI_STATUS_SD_PLUGOUT)
		{
			if(gSys.CurModuleID == MODULE_ID_PLAYER_WIFI_SD)
			{
				MsgSend(MSG_MODE);	//拔出TF卡WiFi模组不会发送切换模式命令，需要MCU自动切换
			}
		}
	}

	IsPowerCardState = TRUE;
	gWiFi.CardInsertState = State;	
}

bool IsWiFiCardLink(void)
{
	if(gWiFi.CardInsertState == WIFI_STATUS_SD_PLUGIN)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//WiFi 端调节音量时同步MCU端音量
void WiFiSetMcuVolume(uint8_t Vol)
{
	if(Vol >= ((100 / MAX_VOLUME) * MAX_VOLUME))
	{
		Vol = MAX_VOLUME;
	}
	else
	{
		Vol = Vol / (100 / MAX_VOLUME);
	}
	gSys.Volume = Vol;
	SetSysVol();
}

//WiFi 端调节音量时同步MCU端音量
void WiFiGetMcuVolume(void)
{
	uint8_t Temp;
	
	if(gSys.Volume >= MAX_VOLUME)
	{
		Temp = 100;						
	}
	else
	{
		Temp = gSys.Volume * (100 / MAX_VOLUME);
	}
	Mcu_SendCmdToWiFi(MCU_CUR_VOL, &Temp);
}

//WiFi 端功放Mute状态设置
void WiFiMuteStateSet(uint8_t State)
{
	if((gSys.CurModuleID == MODULE_ID_WIFI) || (gSys.CurModuleID == MODULE_ID_PLAYER_WIFI_USB) 
	|| (gSys.CurModuleID == MODULE_ID_PLAYER_WIFI_SD))
	{
		if(State)
		{					
//			DacSoftMuteSet(TRUE, TRUE);	
		}
		else
		{			
#ifdef FUNC_WIFI_TALK_AND_AUDIO_EFFECT_EN
#ifndef FUNC_WIFI_TALK_QUICK_OPEN_MIC_EN
		//此处会在没有快速打开MIC时，语音交互中会因为WiFi MUT命令关闭了MIC！！！
			if(WiFiTalkOnFlag == TRUE)
			{
				gSys.MicEnable = FALSE;
				MixerMute(MIXER_SOURCE_MIC);
			}
#endif
#endif
			DacSoftMuteSet(FALSE, FALSE);		
		}		
		gSys.MuteFlag = State;
		
#ifdef FUNC_WIFI_TALK_QUICK_OPEN_MIC_EN	
		//DacVolumeSet(DAC_DIGITAL_VOL, DAC_DIGITAL_VOL);
#endif
	}
}

//WiFi模组查询当前MCU 语音提示语言
void WiFiGetMcuSoundRemindLanguage(void)
{	
#if (MCU_CAP_LAU_ENGLISH == 1)
	gSys.LanguageMode = 0;
#elif (MCU_CAP_LAU_CHINESE == 1)
	gSys.LanguageMode = 1;
#elif (MCU_CAP_LAU_FRENCH == 1)
	gSys.LanguageMode = 6;
#elif (MCU_CAP_LAU_PORTUGUESE == 1)
	gSys.LanguageMode = 7;
#elif (MCU_CAP_LAU_ITALIAN == 1)
	gSys.LanguageMode = 8;
#elif (MCU_CAP_LAU_GERMANY == 1)
	gSys.LanguageMode = 4;
#elif (MCU_CAP_LAU_SPANISH == 1)
	gSys.LanguageMode = 5;
#endif

	Mcu_SendCmdToWiFi(MCU_LAU_XXX, &gSys.LanguageMode);
}

//MCU查询WiFi 端语音提示语言
void McuGetWiFiSoundRemindLanguage(void)
{
	Mcu_SendCmdToWiFi(MCU_LAU_GET, NULL);
}

//WiFi 端语音提示音量设置[TRUE：固定音量播放提示音；FALSE：返回当前系统音量]
void WiFiSoundRemindVolSet(bool WorkFlag)
{	
	extern const uint16_t gAnaVolArr[MAX_VOLUME + 1];
	
#ifdef 	WIFI_SOUND_REMIND_VOL	
	if(WorkFlag)
	{
		MixerConfigVolume(MIXER_SOURCE_ANA_MONO, gAnaVolArr[WIFI_SOUND_REMIND_VOL], gAnaVolArr[WIFI_SOUND_REMIND_VOL]);
		MixerConfigVolume(MIXER_SOURCE_ANA_STERO, gAnaVolArr[WIFI_SOUND_REMIND_VOL], gAnaVolArr[WIFI_SOUND_REMIND_VOL]);		
		APP_DBG("WiFiSoundRemindVolFixed = %d\n", WIFI_SOUND_REMIND_VOL);
	}
	else
	{		
		SetSysVol();
	}
#else
	if(gSys.Volume >= 7)
		return;
	
	if(WorkFlag)
	{
		MixerConfigVolume(MIXER_SOURCE_ANA_MONO, gAnaVolArr[7], gAnaVolArr[7]);
		MixerConfigVolume(MIXER_SOURCE_ANA_STERO, gAnaVolArr[7], gAnaVolArr[7]);
		APP_DBG(" WiFiSoundRemindVolSet = 7!\n");
	}
	else
	{
		SetSysVol();
	}
#endif
}

//其他模式下播放WiFi提示音
#ifdef FUNC_WIFI_POWER_KEEP_ON
void OtherModulePlayWiFiSoundRemind(bool State)
{
	static bool AudioChannelState = FALSE;
	static uint8_t BtPlayState = PLAYER_STATE_IDLE;
	
	if(AudioChannelState != State)
	{
		if(gSys.CurModuleID != MODULE_ID_WIFI 
#ifdef FUNC_WIFI_PLAY_CARD_EN
		&& gSys.CurModuleID != MODULE_ID_PLAYER_WIFI_SD
#endif
#ifdef FUNC_WIFI_PLAY_USB_EN
		&& gSys.CurModuleID != MODULE_ID_PLAYER_WIFI_USB
#endif
		&& (gWiFi.WiFiAppPlayMode != WIFI_PLAY_MODE_AVS))
		{
			AudioAnaSetChannel(AUDIO_CH_NONE);
#ifndef FUNC_MIXER_SRC_EN
			AudioSampleRateSet(44100);
#endif
			APP_DBG("OtherModulePlayWiFiSoundRemind Switch = %d\n", State);
			if(!State)
			{
				gWiFi.OtherModuleWiFiAudioEn = FALSE;
				switch(gSys.CurModuleID)
				{
					case MODULE_ID_LINEIN:
						AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC_LINEIN : AUDIO_CH_LINEIN);
						break;
				
					case MODULE_ID_RADIO:
						AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC_FM : AUDIO_CH_FM);
						break;
				
					case MODULE_ID_PLAYER_SD:  // enter into module player
					case MODULE_ID_PLAYER_USB: // enter into module player		
					case MODULE_ID_BLUETOOTH:  // BT shares the same entry func.
						AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC : AUDIO_CH_NONE);
						if((BtPlayState == PLAYER_STATE_PLAYING) && (gSys.CurModuleID == MODULE_ID_BLUETOOTH))
						{
							BTAudioPlayStart();
							BtPlayState = PLAYER_STATE_IDLE;
						}
						break;
				
					default:
						AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC : AUDIO_CH_NONE);	
						break;
				}
			}
			else
			{
				switch(gSys.CurModuleID)
				{	
					case MODULE_ID_PLAYER_SD:  // enter into module player
					case MODULE_ID_PLAYER_USB: // enter into module player					
					case MODULE_ID_BLUETOOTH:  // BT shares the same entry func.
						if((GetPlayState() == PLAYER_STATE_PLAYING) && (gSys.CurModuleID == MODULE_ID_BLUETOOTH))
						{
							BtPlayState = PLAYER_STATE_PLAYING;
							BTAudioPlayStop();
							WaitMs(500);
						}
						break;
				
					default:
						break;
				}
				gWiFi.OtherModuleWiFiAudioEn = TRUE;
            #ifdef FUNC_WIFI_TALK_AND_AUDIO_EFFECT_EN
                AudioAnaSetChannel(AUDIO_CH_MIC_I2SIN); 
            #else
                AudioAnaSetChannel(AUDIO_CH_I2SIN); 
            #endif
			}
		}
		else
		{
			gWiFi.OtherModuleWiFiAudioEn = FALSE;
		}
		
		AudioChannelState = State;
		WiFiSoundRemindVolSet(State);
	}
#endif
}
	
//WiFi 端语音提示状态设置[000 开始, 001 结束. 002 禁用, 003 可由MCU触发语音]
void WiFiSoundRemindStateSet(uint16_t State)
{
	gWiFi.WiFiSoundRemindState = State;
	if(!State && gSys.MuteFlag)
	{
		gSys.MuteFlag = FALSE;
		APP_DBG("WiFi SoundRemind UnMute........\n");
	}

	if(State)
	{
		TimeOutSet(&WiFiSoundRemindTimer, 1000);
	}

#ifdef FUNC_SINGLE_LED_EN
	if(State && (WIFI_AVS_STATUS_IDLE == gWiFi.MicState))
	{
		SingleLedDisplayModeSet(LED_DISPLAY_MODE_BREATHING, FALSE, LED_DISPLAY_KEEP);
	}
	else
	{
		SingleLedDisplayModeSet(LED_DISPLAY_MODE_BREATHING, TRUE, LED_DISPLAY_KEEP);
	}
#endif
}

//WiFi端语音播报状态
bool IsWiFiSoundRemindPlaying(void)
{
	if(!WiFiPowerOnInitStateGet())
	{
		//先检查定时器，因为第一次检查，无论时间是否到，都会返回FALSE
		IsTimeOut(&WiFiSoundRemindTimer);
		return FALSE;
	}
	else
	{
		return (!gWiFi.WiFiSoundRemindState || !IsTimeOut(&WiFiSoundRemindTimer));
	}
}

//MCU查询WiFi 端语音提示状态
void McuGetWiFiSoundRemindState(void)
{
	Mcu_SendCmdToWiFi(MCU_LAU_XXX, NULL);
}

//MCU端调节音量时同步WiFi端音量
void McuSyncWiFiVolume(uint8_t Vol)
{	
	uint8_t TempVol;
	
	if(Vol >= MAX_VOLUME)
	{
		TempVol = 100;						
	}
	else
	{
		TempVol = Vol * (100 / MAX_VOLUME);
	}

	if(TempVol > WiFiKaiShuVolumeMaxGet())
	{
		TempVol = WiFiKaiShuVolumeMaxGet();
		gSys.Volume = TempVol / (100 / MAX_VOLUME);
		SetSysVol();
	}

	Mcu_SendCmdToWiFi(MCU_CUR_VOL, &TempVol);
}

//通过App设置播放模式
void WiFiAppSetPlayMode(uint8_t Cmd,uint8_t Mode)
{
	uint8_t SetMcuPlayMode;

	SetMcuPlayMode = 0;

#ifdef FUNC_WIFI_BT_CONTROL_EN
	if((gWiFi.WiFiAppPlayMode == WIFI_PLAY_MODE_AVS) && GetCurBtConnectedFlag() && (Cmd != AXX_PLM_AVS)
	&& ((Mode < WIFI_PLAY_MODE_END_PLAY) || (Mode > WIFI_PLAY_MODE_HIGHT_PLAY)))
	{
		BTDisConnect();
		WaitMs(500);
	}
	
	gWiFi.WiFiAppPlayMode = (Cmd == AXX_PLM_AVS)? WIFI_PLAY_MODE_AVS : Mode;
#endif

	if(Mode == WIFI_PLAY_MODE_AUX)
	{
		SetMcuPlayMode = MODULE_ID_LINEIN;	
	}
	else if(Mode == WIFI_PLAY_MODE_BT)
	{
		SetMcuPlayMode = MODULE_ID_BLUETOOTH;	
	}
	else if(Mode == WIFI_PLAY_MODE_EXT_USBTF)
	{
		SetMcuPlayMode = MODULE_ID_PLAYER_SD;	
	}
	else if(Mode == WIFI_PLAY_MODE_MENU_PLAY_USB)
	{
		SetMcuPlayMode = MODULE_ID_PLAYER_WIFI_USB;	
	}	
	else if(Mode == WIFI_PLAY_MODE_MENU_PLAY_SD)
	{
		SetMcuPlayMode = MODULE_ID_PLAYER_WIFI_SD;	
	}	
	else if((Mode == WIFI_PLAY_MODE_MENU_PLAY_WIFI) || (Mode == WIFI_PLAY_MODE_SLAVE) 
		 || (Mode == WIFI_PLAY_MODE_SPOTIFY_PLAY) || (Mode == WIFI_PLAY_MODE_ALEXA)
		 || (Mode == WIFI_PLAY_MODE_AIRPLAY) || (Mode == WIFI_PLAY_MODE_DLNA))
	{
		SetMcuPlayMode = MODULE_ID_WIFI;
	}	
	
	if(SetMcuPlayMode && (gSys.CurModuleID != SetMcuPlayMode))
	{
		gSys.NextModuleID = SetMcuPlayMode;
		gWiFi.WiFiNotifyChangeModeFlag = TRUE;
		MsgSend(MSG_MODE);
	}
}

//MCU切换播放模式
void McuSetPlayMode(uint8_t ChangeMode)
{
	uint16_t Cmd;
	uint8_t Mode;
	
	Cmd = 0;
	Mode = 0xFF;

	if(WiFiSlaveSounBoxCntGet())  //发送MCU+PLM-xxx存在问题，暂时全部发送MCU+PLM+xxx
	{
		Cmd = MCU_PLM_ADD;
	}
	else
	{
		Cmd = MCU_PLM_SUB;	
	}
	
	if(ChangeMode == MODULE_ID_LINEIN)
	{
		Mode = MCU_PLAY_MODE_AUX;		
	}
	else if((ChangeMode == MODULE_ID_BLUETOOTH) || (ChangeMode == MODULE_ID_BT_HF))
	{
		Mode = MCU_PLAY_MODE_BT;		
	}
	else if(ChangeMode == MODULE_ID_PLAYER_USB)
	{
		Mode = MCU_PLAY_MODE_USB;		
	}
	else if(ChangeMode == MODULE_ID_PLAYER_SD)
	{
		Mode = MCU_PLAY_MODE_SD;		
	}
#ifdef FUNC_WIFI_PLAY_CARD_EN
	else if(ChangeMode == MODULE_ID_PLAYER_WIFI_SD)
	{
		Mode = MCU_PLAY_MODE_SD;	
		Cmd = MCU_PLM_ADD;
	}
#endif
#ifdef FUNC_WIFI_PLAY_USB_EN
	else if(ChangeMode == MODULE_ID_PLAYER_WIFI_USB)
	{
		Mode = MCU_PLAY_MODE_USB;	
		Cmd = MCU_PLM_ADD;
	}
#endif	
	else if((ChangeMode == MODULE_ID_WIFI)
		|| (ChangeMode == MODULE_ID_RTC))
	{
		Mode = MCU_PLAY_MODE_WIFI;	
		Cmd = MCU_PLM_ADD;
	}

	if(Mode == 0xFF)	
	{
		return; //WiFi端APP不支持的模式
	}	
	
	McuCurPlayMode = Mode;
	if(gWiFi.WiFiNotifyChangeModeFlag == FALSE)
	{
		Mcu_SendCmdToWiFi(Cmd, &Mode);
		gWiFi.WiFiAppPlayMode = WIFI_PLAY_MODE_IDLE;
		WaitMs(200); //切换太快会出现WiFi 模组自动切换模式问题
	}
}

//WiFi模组查询当前MCU 播放模式
void WiFiGetMcuCurPlayMode(void)
{
	uint16_t Cmd;
	
	Cmd = 0;	
	if(WiFiSlaveSounBoxCntGet())
	{
		Cmd = MCU_PLM_ADD;
	}
	else
	{
		Cmd = MCU_PLM_SUB;	
	}
	Mcu_SendCmdToWiFi(Cmd, &McuCurPlayMode);
}

//WiF模组设置播放重复模式
void WiFiSetRepeatMode(uint8_t RepeatMode)
{
	if(RepeatMode == 0x00)
	{
		gWiFi.RepeatMode = WIFI_PLAY_MODE_REPEAT_ALL;
	}
	else if(RepeatMode == 0x01)
	{
		gWiFi.RepeatMode = WIFI_PLAY_MODE_REPEAT_ONE;
	}
	else if(RepeatMode == 0x02)
	{
		gWiFi.RepeatMode = WIFI_PLAY_MODE_REPEAT_SHUFFLE;
	}
	else if(RepeatMode == 0x03)
	{
		gWiFi.RepeatMode = WIFI_PLAY_MODE_ONCE_SHFFLE;
	}
	else 
	{
		gWiFi.RepeatMode = WIFI_PLAY_MODE_NO_REPEAT;
	}	
}

//MCU设置WiFi端的播放重复模式
void McuSetWiFiRepeatMode(void)
{
	uint8_t RepMode;
	
	gWiFi.RepeatMode++;
	if(gWiFi.RepeatMode >=  WIFI_PLAY_MODE_TOTAL_NUM)
	{
		gWiFi.RepeatMode = WIFI_PLAY_MODE_REPEAT_ALL;
	}
	RepMode = gWiFi.RepeatMode;
	Mcu_SendCmdToWiFi(MCU_PLP_XXX, &RepMode);
}

//MCU获取WiFi端的播放重复模式
void McuGetWiFiRepeatMode(void)
{	
	Mcu_SendCmdToWiFi(MCU_PLP_GET, NULL);
	WaitMs(50);
}

//WiFi Talk状态设置
void WiFiTalkStateSet(bool State)
{
	WiFiTalkOnFlag = State;

#ifdef FUNC_WIFI_TALK_AND_AUDIO_EFFECT_EN
	if(State)
	{	
#ifdef FUNC_WIFI_TALK_QUICK_OPEN_MIC_EN		
		//提前打开MIC
		DacDigitalMuteSet(TRUE, TRUE);	
		WaitMs(100);
		MixerMute(MIXER_SOURCE_ANA_STERO);	
		MixerMute(MIXER_SOURCE_MIC);		
		if(gSys.Volume > DEFAULT_VOLUME)
		{			
			DacVolumeSet(gDecVolArr[DEFAULT_VOLUME], gDecVolArr[DEFAULT_VOLUME]); 
		}
		else
		{
			DacVolumeSet(gDecVolArr[gSys.Volume], gDecVolArr[gSys.Volume]); 
		}	
		PcmFifoClear();
		PhubPathClose(ALL_PATH_CLOSE); 
		PhubPathSel(ADCIN2PMEM_IISIN2DACOUT_PCMFIFO2IISOUT);				
		MixerUnmute(MIXER_SOURCE_MIC);
		MixerUnmute(MIXER_SOURCE_ANA_STERO);
		gSys.MicEnable = TRUE;		
		DacDigitalMuteSet(FALSE, FALSE);
#else
		DacSoftMuteSet(TRUE, TRUE);	
		gSys.MicEnable = TRUE;
		MixerUnmute(MIXER_SOURCE_MIC);
#endif
#ifdef FUNC_SINGLE_LED_EN
		SingleLedDisplayModeSet(LED_DISPLAY_MODE_TLAK_ON, TRUE, LED_DISPLAY_KEEP);
#endif
	}
	else
	{		
		MixerMute(MIXER_SOURCE_MIC);
#ifdef FUNC_WIFI_TALK_QUICK_OPEN_MIC_EN		
		DacSoftMuteSet(TRUE, TRUE);	
		PcmFifoClear();
		PhubPathClose(ALL_PATH_CLOSE);				
		PhubPathSel(ADCIN2PMEM_PCMFIFO2DACOUT);
#endif
		gSys.MicEnable = FALSE;
#ifdef FUNC_WIFI_TALK_QUICK_OPEN_MIC_EN	
		DacVolumeSet(DAC_DIGITAL_VOL, DAC_DIGITAL_VOL);
#endif
		DacSoftMuteSet(FALSE, FALSE);

#ifdef FUNC_SINGLE_LED_EN
		SingleLedDisplayModeSet(LED_DISPLAY_MODE_TLAK_ON, FALSE, LED_DISPLAY_KEEP);
#endif
	}
#endif
}

//MCU 校准时区，模组下发的时间是UTC时间，需要根据时区校准。
void McuChangeSystemTimeZone(RTC_DATE_TIME *SystemTime)
{
#ifdef FUNC_RTC_EN		
#if (defined(FUNC_WIFI_ALI_PROJECT_EN) || defined(FUNC_WIFI_DUMI_PROJECT_EN))
    #define TIMEZONE			(8)
#else
	#define TIMEZONE			(-7)
#endif

	int8_t TimeHour;
	uint8_t MonDate;
	
	TimeHour = (int8_t)SystemTime->Hour;
	TimeHour += (int8_t)TIMEZONE;

	if(TimeHour >= 24)
	{	
		SystemTime->Hour = (TimeHour%24);
		SystemTime->Date++;
		MonDate = RtcGetMonthDays(SystemTime->Year, SystemTime->Mon);
		if(SystemTime->Date > MonDate)
		{
			SystemTime->Date = (SystemTime->Date - MonDate);
			SystemTime->Mon++;
			if(SystemTime->Mon > 12)
			{
				SystemTime->Mon -= 12;
				SystemTime->Year++;
			}
		}
		ChangeSystemTimeZoneFlag = 1;
	}
	else if(TimeHour < 0)
	{
		SystemTime->Hour = (uint8_t)(24+TimeHour);
		SystemTime->Date--;
		if(SystemTime->Date == 0)
		{
			SystemTime->Mon--;
			if(SystemTime->Mon == 0)
			{
				SystemTime->Mon = 12;
				SystemTime->Year--;
			}
			SystemTime->Date = RtcGetMonthDays(SystemTime->Year, SystemTime->Mon);
		}
		ChangeSystemTimeZoneFlag = -1;
	}
	else
	{
		SystemTime->Hour = (TimeHour%24);
		ChangeSystemTimeZoneFlag = 0;
	}
#endif
}

//WiFi端设置MCU当前系统时间
void WiFiSetMcuSystemTime(uint8_t* DateData)
{
#ifdef FUNC_WIFI_SUPPORT_RTC_EN
#ifdef FUNC_RTC_EN		
	RTC_DATE_TIME SystemTime;
	static uint8_t RcvCnt;
	
	if(WiFiDataRcvStartFlag == FALSE)
	{
		RcvCnt = 0;
		WiFiDataRcvStartFlag = TRUE;
		return;
	}

	if(DateData[RcvCnt++] == '&')
	{
		APP_DBG("WiFiNoticeMcuSystemTime:%s\n", &DateData[0]);
		SystemTime.Year = (DateData[0] - 0x30) * 1000 + (DateData[1] - 0x30) * 100 + (DateData[2] - 0x30) * 10 + (DateData[3] - 0x30);
		SystemTime.Mon = (DateData[4] - 0x30) * 10 + (DateData[5] - 0x30);
		SystemTime.Date = (DateData[6] - 0x30) * 10 + (DateData[7] - 0x30);
		SystemTime.Hour = (DateData[8] - 0x30) * 10 + (DateData[9] - 0x30);
		SystemTime.Min = (DateData[10] - 0x30) * 10 + (DateData[11] - 0x30);
		SystemTime.Sec = (DateData[12] - 0x30) * 10 + (DateData[13] - 0x30);
		SystemTime.WDay = sRtcControl->DataTime.WDay;
		//McuChangeSystemTimeZone(&SystemTime);
		RtcSetCurrTime(&SystemTime);
		WiFiDataRcvStartFlag = FALSE;
		DateData[RcvCnt] = '\0';
		APP_DBG("WiFiSetMcuSystemTime:(%04d-%02d-%02d %02d:%02d:%02d)\n",
		SystemTime.Year, SystemTime.Mon, SystemTime.Date, SystemTime.Hour, SystemTime.Min, SystemTime.Sec);
	}	
#endif
#endif
}

//WiFi端设置MCU当前星期号
void WiFiSetMcuWeekDay(uint8_t* DayData)
{
#ifdef FUNC_WIFI_SUPPORT_RTC_EN
#ifdef FUNC_RTC_EN		
	RTC_DATE_TIME SystemTime;
	static uint8_t RcvCnt;
	
	if(WiFiDataRcvStartFlag == FALSE)
	{
		RcvCnt = 0;
		WiFiDataRcvStartFlag = TRUE;
		return;
	}

	if(DayData[RcvCnt++] == '&')
	{		
		RtcGetCurrTime(&sRtcControl->DataTime);
		if(ChangeSystemTimeZoneFlag > 0)
		{
			SystemTime.WDay = ((DayData[0] - 0x30)+1);
			if(SystemTime.WDay > 6)
			{
				SystemTime.WDay = 0;	
			}
		}
		else if(ChangeSystemTimeZoneFlag < 0)
		{
			if((DayData[0] - 0x30) == 0)
			{
				SystemTime.WDay = 6;	
			}
			else
			{
				SystemTime.WDay = ((DayData[0] - 0x30)-1);
			}
		}
		else
		{
			SystemTime.WDay = DayData[0] - 0x30;
		}
		
		ChangeSystemTimeZoneFlag = 0;
		SystemTime.Year = sRtcControl->DataTime.Year;
		SystemTime.Mon = sRtcControl->DataTime.Mon;
		SystemTime.Date = sRtcControl->DataTime.Date;
		SystemTime.Hour = sRtcControl->DataTime.Hour;
		SystemTime.Min = sRtcControl->DataTime.Min;
		SystemTime.Sec = sRtcControl->DataTime.Sec;
		RtcSetCurrTime(&SystemTime);
		WiFiDataRcvStartFlag = FALSE;
		DayData[RcvCnt] = '\0';
		APP_DBG("WiFiSetMcuWeekDay:%s\n", &DayData[0]);
	}	
#endif
#endif
}

//WiFi模块告知MCU下一个闹钟的时间
//参数: SecondData 表示为离当前时间的秒钟数
void WiFiNoticeMcuNextAlarmTime(uint8_t* SecondData)
{
#ifdef FUNC_WIFI_SUPPORT_ALARM_EN
#ifdef FUNC_WIFI_SUPPORT_RTC_EN
#ifdef FUNC_RTC_EN
	uint32_t TimeSecond, Times;	
	uint32_t  CurRtcTimeSecond;	
	static uint8_t RcvCnt;
	RTC_DATE_TIME AlarmTime;
	
	if(WiFiDataRcvStartFlag == FALSE)
	{
		RcvCnt = 0;
		WiFiDataRcvStartFlag = TRUE;
		return;
	}
	
	if(SecondData[RcvCnt++] == '&')
	{	
		SecondData[RcvCnt] = '\0';
		APP_DBG("WiFiNoticeMcuNextAlarmTime:%s\n", &SecondData[0]);
		
		TimeSecond = 0;
		Times = 1;
		
		while (--RcvCnt > 0) {		
			TimeSecond += (SecondData[RcvCnt-1] - 0x30) * Times;
			Times *= 10;
		}
		
		WiFiDataRcvStartFlag = FALSE;
		sRtcControl->AlarmNum = 1;
		sRtcControl->AlarmMode = ALARM_MODE_PER_DAY;
		
		//闹钟开关状态设置
		if((SecondData[0] == '-') && (SecondData[1] == '1'))	
		{
			if(RtcGetAlarmStatus(sRtcControl->AlarmNum) != ALARM_STATUS_CLOSED)
			{
				RtcAlarmSetStatus(sRtcControl->AlarmNum, ALARM_STATUS_CLOSED);	//关闭闹钟
			}
			return;
		}
		else
		{
			if(RtcGetAlarmStatus(sRtcControl->AlarmNum) != ALARM_STATUS_OPENED)
			{
				RtcAlarmSetStatus(sRtcControl->AlarmNum, ALARM_STATUS_OPENED); //打开闹钟
			}
		}
#if 1
		//RtcGetAlarmTime(sRtcControl->AlarmNum, &sRtcControl->AlarmMode, &sRtcControl->AlarmData, &sRtcControl->AlarmTime);

		AlarmTime.WDay = sRtcControl->AlarmTime.WDay;
		AlarmTime.Year = sRtcControl->AlarmTime.Year;
		AlarmTime.Mon = sRtcControl->AlarmTime.Mon;
		AlarmTime.Date = sRtcControl->AlarmTime.Date;

		CurRtcTimeSecond = (sRtcControl->DataTime.Hour * 3600) + (sRtcControl->DataTime.Min * 60) + sRtcControl->DataTime.Sec;
		
		APP_DBG("Time(%d/%d; %02d:%02d:%02d)!!!\n", TimeSecond, CurRtcTimeSecond, sRtcControl->DataTime.Hour, sRtcControl->DataTime.Min, sRtcControl->DataTime.Sec);
		//计算闹钟时间
		AlarmTime.Hour = (TimeSecond  + CurRtcTimeSecond) / 3600;
		if(AlarmTime.Hour >= 24)
		{
			AlarmTime.Hour -= 24;
		}
		AlarmTime.Min = ((TimeSecond  + CurRtcTimeSecond) % 3600) / 60 ;		
		AlarmTime.Sec = ((TimeSecond  + CurRtcTimeSecond) % 3600) % 60;

		RtcSetAlarmTime(sRtcControl->AlarmNum, sRtcControl->AlarmMode, sRtcControl->AlarmData, &AlarmTime);
		RtcGetAlarmTime(sRtcControl->AlarmNum, &sRtcControl->AlarmMode, &sRtcControl->AlarmData, &sRtcControl->AlarmTime);
		APP_DBG("WifiSerRtcAralmTime(AralmNum:%d; %02d:%02d:%02d)!!!\n", sRtcControl->AlarmNum, 
	        	sRtcControl->AlarmTime.Hour, sRtcControl->AlarmTime.Min, sRtcControl->AlarmTime.Sec);
#endif

	}
#endif
#endif
#endif
}

/*****************************************************************************
 函 数 名  : WiFiSetMcuAlarmTime
 功能描述  : WiFi通知mcu下一个闹钟时间
 输入参数  : uint8_t* AlarmTimeData  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年8月31日
    作    者   : 李治清
    修改内容   : 新生成函数

*****************************************************************************/
void WiFiSetMcuAlarmTime(uint8_t* AlarmTimeData)
{
#ifdef FUNC_WIFI_SUPPORT_RTC_EN
#ifdef FUNC_RTC_EN
#ifdef FUNC_WIFI_SUPPORT_ALARM_EN
	static uint8_t RcvCnt;
	ALARM_BP_INFO  AlarmInfo;
	
	if(WiFiDataRcvStartFlag == FALSE)
	{
		RcvCnt = 0;
		WiFiDataRcvStartFlag = TRUE;
		return;
	}
	
	if(AlarmTimeData[RcvCnt++] == '&')
	{
		APP_DBG("WiFiNoticeMcuNextAlarmTime:%s\n", &AlarmTimeData[0]);
		RcvCnt -= 1;		
		AlarmInfo.AlarmTime.Year = (AlarmTimeData[0] - 0x30) * 1000 + (AlarmTimeData[1] - 0x30) * 100 + (AlarmTimeData[2] - 0x30) * 10 + (AlarmTimeData[3] - 0x30);
		AlarmInfo.AlarmTime.Mon = (AlarmTimeData[4] - 0x30) * 10 + (AlarmTimeData[5] - 0x30);
		AlarmInfo.AlarmTime.Date= (AlarmTimeData[6] - 0x30) * 10 + (AlarmTimeData[7] - 0x30);
		AlarmInfo.AlarmTime.Hour= (AlarmTimeData[8] - 0x30) * 10 + (AlarmTimeData[9] - 0x30);
		AlarmInfo.AlarmTime.Min= (AlarmTimeData[10] - 0x30) * 10 + (AlarmTimeData[11] - 0x30);
		AlarmInfo.AlarmTime.Sec= (AlarmTimeData[12] - 0x30) * 10 + (AlarmTimeData[13] - 0x30);
		//为了方便比较，无实际意义
		AlarmInfo.AlarmTime.WDay = sRtcControl->DataTime.WDay;
		WiFiDataRcvStartFlag = FALSE;
        /* 判断闹钟时间是否合法，小于当前系统时间则丢弃*/
        if(memcmp((void *)&AlarmInfo.AlarmTime, (void *)&sRtcControl->DataTime, sizeof(RTC_DATE_TIME)) <= 0)
        {
            APP_DBG("Alarm time is nullity!!!\n");
            return;
        }
		McuChangeSystemTimeZone((RTC_DATE_TIME *)&AlarmInfo.AlarmTime);
		//停止本地正在闹响的闹钟
		if(sRtcControl->CurAlarmRun)
		{
			RtcCurAlarmSleepAndStop(RTC_ALARM_STATE_STOP);
		}

		APP_DBG("WiFiSetMcuNextAlarmTime:(%04d-%02d-%02d %02d:%02d:%02d)\n",
		AlarmInfo.AlarmTime.Year, AlarmInfo.AlarmTime.Mon, AlarmInfo.AlarmTime.Date,
		AlarmInfo.AlarmTime.Hour, AlarmInfo.AlarmTime.Min, AlarmInfo.AlarmTime.Sec);

#if (defined(FUNC_RTC_EN) && defined(FUNC_RTC_ALARM_SAVE2FLASH))
        /* 设置闹钟常量 */
		AlarmInfo.AlarmMode = ALARM_MODE_ONCE_ONLY;
		AlarmInfo.AlarmState = ALARM_STATUS_OPENED;
		AlarmInfo.AlarmVolume = DEFAULT_VOLUME;
		AlarmInfo.RingType = RTC_RING_WIFISD_TYPE;

		/* 闹钟数据存放到记忆区 */
		SetWiFiAlarmTimeToBpInfo(&AlarmInfo);

		/* 更新到本地闹钟 */
		for(sRtcControl->AlarmNum = 1; sRtcControl->AlarmNum <= MAX_ALARM_NUM; sRtcControl->AlarmNum++)
		{
            if(GetNearAlarmTimeFromBpInfo(&AlarmInfo, sRtcControl->AlarmNum-1))      
            {
			    sNvmRtcInfo.RingType[sRtcControl->AlarmNum] = AlarmInfo.RingType;
    			sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] = AlarmInfo.AlarmVolume;
    			sRtcControl->AlarmMode = AlarmInfo.AlarmMode; 
    			sRtcControl->AlarmTime = AlarmInfo.AlarmTime;
    			sRtcControl->AlarmState = AlarmInfo.AlarmState;
                RtcSetAlarmTime(sRtcControl->AlarmNum, sRtcControl->AlarmMode, sRtcControl->AlarmData, &sRtcControl->AlarmTime);
                RtcAlarmSetStatus(sRtcControl->AlarmNum, sRtcControl->AlarmState);
			}
            else
            {
                RtcAlarmSetStatus(sRtcControl->AlarmNum, ALARM_STATUS_CLOSED);
            }
		}
        
	    sRtcControl->AlarmNum = 0;
        //保存到记忆区
        AudioSysInfoSetBreakPoint();
#endif		

#ifdef FUNC_RTC_ALARM		
		NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#endif

#ifdef FUNC_DISP_EN
        /* 显示WiFi下发的闹钟时间 */
        sRtcControl->AlarmTime.Year = (AlarmTimeData[0] - 0x30) * 1000 + (AlarmTimeData[1] - 0x30) * 100 + (AlarmTimeData[2] - 0x30) * 10 + (AlarmTimeData[3] - 0x30);
		sRtcControl->AlarmTime.Mon = (AlarmTimeData[4] - 0x30) * 10 + (AlarmTimeData[5] - 0x30);
		sRtcControl->AlarmTime.Date= (AlarmTimeData[6] - 0x30) * 10 + (AlarmTimeData[7] - 0x30);
		sRtcControl->AlarmTime.Hour= (AlarmTimeData[8] - 0x30) * 10 + (AlarmTimeData[9] - 0x30);
		sRtcControl->AlarmTime.Min= (AlarmTimeData[10] - 0x30) * 10 + (AlarmTimeData[11] - 0x30);
		sRtcControl->AlarmTime.Sec= (AlarmTimeData[12] - 0x30) * 10 + (AlarmTimeData[13] - 0x30);
        McuChangeSystemTimeZone(&sRtcControl->AlarmTime);
		sRtcControl->AlarmNum = 0;
		DispSendMessage(SET_INTERVL, DISP_MSG_WIFI_SET_ALARM);
#endif
	}
#endif
#endif
#endif
}

//MCU获取WiFi模块下一个闹钟的时间
void McuGetWiFiNextAlarmTime(void)
{
	Mcu_SendCmdToWiFi(MCU_ALM_NXT, NULL);
}
/*****************************************************************************
 函 数 名  : WiFiSetAlarmRemindState
 功能描述  : 设置WiFi提示音状态
 输入参数  : bool State  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年1月28日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void WiFiSetAlarmRemindState(bool State)
{
	gWiFi.AlarmRemindState = State;
}

/*****************************************************************************
 函 数 名  : GetWiFiAlarmRemindState
 功能描述  : 获取WiFi提示音状态
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年1月28日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
bool GetWiFiAlarmRemindState(void)
{
	return gWiFi.AlarmRemindState;
}


//Master设备通过透传方式向处于同一个多房间的MCU传递指令
//参数:Cmd值范围000 ~ 999
void MasterMcuSendPassThroughCmd(uint16_t Cmd)
{
	if(Cmd <= 999)
	{
		Mcu_SendCmdToWiFi(MCU_M2S_NNN, (uint8_t*)&Cmd);
	}
}

//Master设备接收通过透传方式传递的指令
//参数:Cmd值范围000 ~ 999
void MasterMcuRevPassThroughCmd(uint16_t Cmd)
{
//用户可以在此函数中解析接收到的指令
	switch(Cmd)
	{
		default:
			break;
	}
}

//Slave设备通过透传方式向处于同一个多房间的MCU传递指令
//参数:Cmd值范围000 ~ 999
void SlaveMcuSendPassThroughCmd(uint16_t Cmd)
{
	if(Cmd <= 999)
	{
		Mcu_SendCmdToWiFi(MCU_S2M_NNN, (uint8_t*)&Cmd);
	}
}

//Slave设备接收通过透传方式传递的指令
//参数:Cmd值范围000 ~ 999
void SlaveMcuRevPassThroughCmd(uint16_t Cmd)
{
//用户可以在此函数中解析接收到的指令
	switch(Cmd)
	{
		default:
			break;
	}
}

//MCU发送当前Eq Treble 值给WiFi
void McuSendWiFiEqTrebleVal(uint8_t TrebleVal)
{	
#ifdef FUNC_WIFI_PASS_THROUGH_DATA_EN
	PassThroughDataLen = 0;
	memcpy((void*)&Upgrade_Data_Buf[PassThroughDataLen], (void*)"MCU+PAS+EQ:treble:", 18);
	PassThroughDataLen = 18;
	if(TrebleVal / 10)
	{
		Upgrade_Data_Buf[PassThroughDataLen++] = (TrebleVal / 10) + 0x30;
		Upgrade_Data_Buf[PassThroughDataLen++] = (TrebleVal % 10) + 0x30;				
	}
	else
	{
		Upgrade_Data_Buf[PassThroughDataLen++] = TrebleVal + 0x30;	
	}	
	Upgrade_Data_Buf[PassThroughDataLen++] = '&';
	McuSendPassThroughData(Upgrade_Data_Buf, PassThroughDataLen);
#endif
}

//MCU发送当前Eq Bass 值给WiFi
void McuSendWiFiEqBassVal(uint8_t BassVal)
{	
#ifdef FUNC_WIFI_PASS_THROUGH_DATA_EN
	PassThroughDataLen = 0;
	memcpy((void*)&Upgrade_Data_Buf[PassThroughDataLen], (void*)"MCU+PAS+EQ:bass:", 16);
	PassThroughDataLen = 16;
	if(BassVal / 10)
	{
		Upgrade_Data_Buf[PassThroughDataLen++] = (BassVal / 10) + 0x30;
		Upgrade_Data_Buf[PassThroughDataLen++] = (BassVal % 10) + 0x30;				
	}
	else
	{
		Upgrade_Data_Buf[PassThroughDataLen++] = BassVal + 0x30;	
	}	
	Upgrade_Data_Buf[PassThroughDataLen++] = '&';
	McuSendPassThroughData(Upgrade_Data_Buf, PassThroughDataLen);
#endif
}

//MCU接收WiFi 端发送的透传数据
void McuRevPassThroughData(uint8_t* RevData)
{
#ifdef FUNC_WIFI_PASS_THROUGH_DATA_EN
	static uint8_t RcvCnt;
	uint16_t Temp;
	
	if(WiFiDataRcvStartFlag == FALSE)
	{
		RcvCnt = 0;
		WiFiDataRcvStartFlag = TRUE;
		PassThroughDataLen = 0;
		PassThroughDataState = 1;
		memset(Upgrade_Data_Buf, 0, sizeof(Upgrade_Data_Buf));
		return;
	}

	if(RevData[RcvCnt++] == '&')
	{
		PassThroughDataLen = RcvCnt - 1;
		if(PassThroughDataLen <= 1024)
		{
			for(Temp = 0; Temp < PassThroughDataLen; Temp++)
			{
				Upgrade_Data_Buf[Temp] = RevData[Temp];
			}
		}
		PassThroughDataState = 0;
	}
#endif
}

//MCU接收WiFi 端发送的透传数据状态
//返回值: 1--正在接收数据   0--已接收完成0xFF-- 接收失败
uint8_t McuRevPassThroughDataState(void)
{
	return PassThroughDataState;
}

//MCU对接收到的透传数据解析处理
void McuRevPassThroughDataProcess(void)
{
#ifdef FUNC_WIFI_PASS_THROUGH_DATA_EN
	if(PassThroughDataLen && (McuRevPassThroughDataState() == 0))
	{
#ifdef FUNC_TREB_BASS_EN
		uint8_t Temp = 0xFF;
		if(memcmp(Upgrade_Data_Buf, "EQGet", 5) == 0)
		{
			McuSendWiFiEqTrebleVal(gSys.TrebVal);
			WaitMs(10);
			McuSendWiFiEqBassVal(gSys.BassVal);			
		}
		else if(memcmp(Upgrade_Data_Buf, "EQSet:treble:", 13) == 0)
		{
			if(Upgrade_Data_Buf[14])
			{
				Temp = (Upgrade_Data_Buf[13]- 0x30) * 10 + (Upgrade_Data_Buf[14]- 0x30);
			}
			else 
			{
				Temp = Upgrade_Data_Buf[13]- 0x30;
			}

			if(Temp < MAX_TREB_VAL)
			{
				gSys.TrebVal = Temp;
			}
			TrebBassSet(gSys.TrebVal, gSys.BassVal);
			gSys.EqStatus = 0;
		}
		else if(memcmp(Upgrade_Data_Buf, "EQSet:bass:", 11) == 0)
		{
			if(Upgrade_Data_Buf[12])
			{
				Temp = (Upgrade_Data_Buf[11]- 0x30) * 10 + (Upgrade_Data_Buf[12]- 0x30);
			}
			else
			{
				Temp = Upgrade_Data_Buf[11]- 0x30;
			}
			
			if(Temp < MAX_BASS_VAL)
			{
				gSys.BassVal= Temp;
			}
			
			TrebBassSet(gSys.TrebVal, gSys.BassVal);
			gSys.EqStatus = 0;
		}
#endif
		PassThroughDataLen = 0;
	}
#endif
}

//MCU发送透传数据到WiFi 端
void McuSendPassThroughData(uint8_t* SendData, uint16_t DataLen)
{
#ifdef FUNC_WIFI_PASS_THROUGH_DATA_EN
	int32_t Temp = -1;
		
	PassThroughDataState = 1;
	if(DataLen <= 1024)
	{		
#ifdef WIFI_SELECT_BUART_COM
		Temp = BuartSend(SendData, DataLen);
#else
		Temp = FuartSend(SendData, DataLen);
#endif
	}

	if(Temp == DataLen)
	{
		PassThroughDataState = 0;
	}
	else
	{
		PassThroughDataState = 0xFF;
	}
#endif
}

//MCU发送透传数据到WiFi 端
//返回值: 1--正在发送数据   0--已发送完成 0xFF--发送失败
uint8_t McuSendPassThroughDataState(void)
{
	return PassThroughDataState;
}

//WiFi部分状态查询
void WiFiStateCheck(void)
{
#define WIFI_STATE_CHECK_TIME	30000	//WiFi状态查询时间间隔(10s)
	
#ifdef FUNC_WIFI_POWER_KEEP_ON
	WiFiPowerOnInitProcess();          //为了配置WiFi串口。
#endif	

	if(WiFiFactoryStateGet() || WiFiFirmwareUpgradeStateGet())
	{
		return;
	}
	
	if(gWiFi.WiFiPowerOffRequestFlag && (IsTimeOut(&WiFiPowerOffTimer)) && (gSys.CurModuleID <= MODULE_ID_END))
	{
		WiFiRequestMcuPowerOff();	
	}		
	
#ifdef FUNC_POWER_MONITOR_EN
	if((WiFiWwwStateGet() != WIFI_STATUS_WWW_ENABLE) && (WiFiRa0StateGet() == WIFI_STATUS_AP_NO_CONNECTED))
	{
		return;
	}
	
#ifdef FUNC_WIFI_BATTERY_POWER_CHECK_EN
#ifdef OPTION_CHARGER_DETECT
	if(gWiFi.ChargeStatusFlag != IsInCharge())
	{
		gWiFi.ChargeStatusFlag = IsInCharge();
		if(gWiFi.ChargeStatusFlag)
		{
			Mcu_SendCmdToWiFi(MCU_BAT_ON, NULL);
		}
		else
		{
			Mcu_SendCmdToWiFi(MCU_BAT_OFF, NULL);
		}
		WaitMs(10);
	}
#endif
	
	if(IsTimeOut(&WiFiCmdRespTimer))
	{
		if(!(WiFiFactoryStateGet() || WiFiFirmwareUpgradeStateGet()))
		{
			
			gWiFi.BatPowerPercent = GetCurBatterLevelAverage();
			if(gWiFi.BatPowerPercentBak == 0)
			{
				gWiFi.BatPowerPercentBak = gWiFi.BatPowerPercent;
			}
			APP_DBG("Get Cur Batter Level Average = %d\n", gWiFi.BatPowerPercent);

			if(gWiFi.ChargeStatusFlag && (gWiFi.BatPowerPercentBak < gWiFi.BatPowerPercent)) 
			{
				gWiFi.BatPowerPercentBak++;
			}
			else if(!gWiFi.ChargeStatusFlag && (gWiFi.BatPowerPercentBak > gWiFi.BatPowerPercent)) 
			{
				gWiFi.BatPowerPercentBak--;
			}
			
			if(gWiFi.BatPowerPercentBak <= 1)
			{
				gWiFi.BatPowerPercentBak = 1;
				Mcu_SendCmdToWiFi(MCU_BAT_LOW, NULL);
			}
			WaitMs(10);
			Mcu_SendCmdToWiFi(MCU_BAT_VAL, &gWiFi.BatPowerPercentBak);
		}
		TimeOutSet(&WiFiCmdRespTimer, WIFI_STATE_CHECK_TIME);
	}
#endif
#endif
}

//WiFi端设置MCU端控制的LED 显示状态
void WiFiSetMcuLedState(uint16_t State)
{
	gWiFi.LedState = State;
}

//WiFi端MIC状态
void WiFiSetMicState(WIFI_AVS_STATUS State)
{
	gWiFi.MicState = State;
#ifdef FUNC_SINGLE_LED_EN
	if((WIFI_AVS_STATUS_IDLE == State) && gWiFi.WiFiSoundRemindState)
	{
		SingleLedDisplayModeSet(LED_DISPLAY_MODE_BREATHING, FALSE, LED_DISPLAY_KEEP);
	}
	else
	{		
		SingleLedDisplayModeSet(LED_DISPLAY_MODE_BREATHING, TRUE, LED_DISPLAY_KEEP);
	}
#endif
}

//MCU获取WiFi当前播放状态相关参数(注:由于在断电记忆中会关闭串口中断,
//在反复切换模式中可能导致WiFi命令丢失, 此函数可以在断电记忆后调用以
//防止播放状态错误)
void McuGetWiFiPlayStateParams(void)
{
	Mcu_SendCmdToWiFi(MCU_MUT_GET, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_PLY_GET, NULL);
	WaitMs(10);	
}

//WiFi获取产品相关参数配置信息
void WiFiGetProjectParams(void)
{	
	WiFiFactoryStateSet(0);
	WiFiFirmwareUpgradeStateSet(0);
		
	Mcu_SendCmdToWiFi(MCU_CAP_PRJ, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_PTV_XXX, NULL);
	WaitMs(10);
#ifdef FUNC_WIFI_ALI_PROJECT_EN
	Mcu_SendCmdToWiFi(MCU_ALI_PID, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_ALV_NUM, NULL);
	WaitMs(10);
#endif
#ifdef FUNC_WIFI_SPOTIFY_NEED_EN
	Mcu_SendCmdToWiFi(MCU_SPY_NAM, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_SPY_BRN, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_SPY_MOD, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_SPY_BRD, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_SPY_CID, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_SPY_TYP, NULL);
	WaitMs(10);
#endif
	Mcu_SendCmdToWiFi(MCU_CAP_001, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_CAP_002, NULL);
	WaitMs(10);	
	Mcu_SendCmdToWiFi(MCU_CAP_LAU, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_LAULIST, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_CAP_STM, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_CAP_PLM, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_CAP_EXT, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_PAR_XXX, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_PRESETN, NULL);
	WaitMs(10);
#ifdef FUNC_MCU_SET_WIFI_PASS_WORD_EN
	Mcu_SendCmdToWiFi(MCU_SET_PWD, NULL);
	WaitMs(10);
#endif
	Mcu_SendCmdToWiFi(MCU_VMX_XXX, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_FMT_XXX, NULL);
	WaitMs(10);
	Mcu_SendCmdToWiFi(MCU_SID_VAL, NULL);
	WaitMs(10);	
}

//WiFi相关GPIO上电初始化(防止漏电导致WiFi模组不能正常初始化)
void WiFiControlGpioInit(void)
{
#ifdef WIFI_SELECT_BUART_COM
	GpioBuartRxIoConfig(0xFF);	
	GpioBuartTxIoConfig(0xFF);	
#endif
#ifdef WIFI_SELECT_FUART_COM
	GpioFuartRxIoConfig(0xFF);	
	GpioFuartTxIoConfig(0xFF);	
#endif
	WiFiUartTxdOff();
	WiFiUartRxdOff();
	GpioI2sIoConfig(0xFF);    
	GpioMclkIoConfig(0xFF); 
	WiFiI2sLrckOff();
	WiFiI2sBclkOff();
	WiFiI2sDinOff();
	WiFiI2sDoutOff();
	WiFiI2sMclkOff();
	WiFiResetOff();	
	WiFiPowerOff();
	WiFiMicOn();
	TimeOutSet(&WiFiLedBlinkTimer, 0);
	TimeOutSet(&WiFiSoundRemindTimer, 0);
	
#ifdef FUNC_WIFI_POWER_KEEP_ON
	WiFiPowerOnInitTimeCnt = WIFI_POWER_ON_INIT_TIME;
	WiFiPowerOnInitEndFlag = FALSE;
#endif
}

//WiFi测试状态闪烁所有的灯
void WiFiTestStateLedCb(void)
{
	static uint8_t TestLedCnt;
	
	TestLedCnt++;
	LED_ALL_MODE_OFF();
	if(TestLedCnt == 1)
	{
		LED_RED_MODE_ON();
	}
	else if(TestLedCnt == 2)
	{
		LED_GREEN_MODE_ON();
	}
	else if(TestLedCnt == 3)
	{
		LED_BLUE_MODE_ON();
		TestLedCnt = 0;
	}
}

//WiFi模式下的LED显示处理(100ms调用一次，此时间间隔WIFI_LED_CB_TIME 可以用户自己定义)
void WiFiLedCb(void* Param)
{
    static uint32_t  LedBlinkCnt;
	static bool MicErrFlag = FALSE;
	
#ifndef FUNC_WIFI_POWER_KEEP_ON
	if(WiFiPowerOnInitTimeCnt)
	{
		WiFiPowerOnInitTimeCnt--;
	}
#endif	
	
	if(!IsTimeOut(&WiFiLedBlinkTimer) && (gWiFi.MicState != WIFI_AVS_STATUS_LSN))
		return;
	
	if(gWiFi.TestModeState)
	{
		WiFiTestStateLedCb();
		TimeOutSet(&WiFiLedBlinkTimer, 500);
		return;
	}
	
	LedBlinkCnt++;
	if((WiFiFirmwareUpgradeStateGet() == TRUE) || (WiFiFactoryStateGet() == TRUE))
	{
		LED_WHITE_MODE_OFF();
		TimeOutSet(&WiFiLedBlinkTimer, 400);
		if(LedBlinkCnt == 1)
		{
			LED_RED_MODE_ON();
		}
		else if(LedBlinkCnt == 2)
		{
			LED_GREEN_MODE_ON();
		}
		else
		{
			LED_BLUE_MODE_ON();
			LedBlinkCnt = 0;
		}
		return;
	}
	
	if((gWiFi.VisInfState == TRUE) || (gWiFi.MicState != WIFI_AVS_STATUS_IDLE))
	{
		TimeOutSet(&WiFiLedBlinkTimer, 50);
		if(gWiFi.MicState == WIFI_AVS_STATUS_LSN)
		{
			LED_CYAN_MODE_ON();
			LedBlinkCnt = 0;
		}
		else if(gWiFi.MicState == WIFI_AVS_STATUS_THK)
		{
			if(LedBlinkCnt >= 2)
			{
				LED_BLUE_MODE_ON();
				LED_RED_MODE_OFF();
				LED_GREEN_MODE_OFF();
				LedBlinkCnt = 0;
			}
			else
			{
				LED_CYAN_MODE_ON();
			}				
		}
		else if(gWiFi.MicState == WIFI_AVS_STATUS_TTS)
		{
			TimeOutSet(&WiFiLedBlinkTimer, 500);
			if(LedBlinkCnt >= 2)
			{
				LED_BLUE_MODE_ON();
				LED_GREEN_MODE_OFF();
				LED_RED_MODE_OFF();
				LedBlinkCnt = 0;
			}
			else
			{
				LED_CYAN_MODE_ON();
			}
		}
		else if(gWiFi.MicState == WIFI_AVS_STATUS_ERR)
		{
			if(MicErrFlag == FALSE)
			{
				LED_WHITE_MODE_OFF();
				TimeOutSet(&WiFiLedBlinkTimer, 3000);
				LED_ORANGE_MODE_ON();
				MicErrFlag = TRUE;
			}
			else
			{
				WiFiSetMicState(WIFI_AVS_STATUS_IDLE);
			}
		}
		else if((gWiFi.VisInfState == TRUE) || (gWiFi.MicState == WIFI_AVS_STATUS_DIS))
		{
			LED_WHITE_MODE_OFF();
			if(gWiFi.VisInfState == TRUE)
			{
				TimeOutSet(&WiFiLedBlinkTimer, 400);
				if(LedBlinkCnt%2 == TRUE)
				{
					LED_YELLOW_MODE_ON();
				}					
				if((gWiFi.MicState == WIFI_AVS_STATUS_DIS) && (LedBlinkCnt >= 6))
				{
					TimeOutSet(&WiFiLedBlinkTimer, 2500);
					LedBlinkCnt = 0;
					LED_WHITE_MODE_OFF();
					LED_RED_MODE_ON();
				}
			}
			else
			{
			  LED_RED_MODE_ON();
			}
		}
		return;
	}
	else
	{
		MicErrFlag = FALSE;
	}

#ifdef FUNC_SLEEP_LEDOFF_EN
	if(gSys.SleepLedOffFlag)
	{
		LED_WHITE_MODE_OFF();
	}
	else
#endif
	{
		TimeOutSet(&WiFiLedBlinkTimer, 500);
		if(((gSys.CurModuleID == MODULE_ID_WIFI)
		&& (WiFiWwwStateGet() != WIFI_STATUS_WWW_ENABLE)
		&& (WiFiRa0StateGet() == WIFI_STATUS_AP_NO_CONNECTED))
		|| ((gSys.CurModuleID == MODULE_ID_PLAYER_WIFI_SD)
		&& (gWiFi.PlayState == TRUE)))
		{
			if(LedBlinkCnt >= 2)
			{		
				LED_WHITE_MODE_ON();
				LedBlinkCnt = 0;
			}
			else
			{
				LED_WHITE_MODE_OFF();
			}
		}
		else
		{
			LED_WHITE_MODE_ON();
		}
	}
}

//WiFi 上电后部分硬件初始化处理
void WiFiPowerOnInitProcess(void)
{
#ifdef FUNC_WIFI_POWER_KEEP_ON
	if(WiFiPowerOnInitEndFlag == FALSE)
	{
		WiFiPowerOnInitTimeCnt--;
		if(WiFiPowerOnInitTimeCnt < 1)
		{
			WiFi_UartInit();
			WiFiPowerOnInitEndFlag = TRUE;
			APP_DBG("wifi uart init.............\n");
		}
	}
#else

	if(WiFiPowerOnInitTimeCnt)
	{
		WiFiPowerOnInitTimeCnt--;
	}
#endif
}

uint32_t WiFiControl(void)
{
	uint16_t Msg = 0;
	SW_TIMER WiFiInTimer;
	//凯叔哄睡设置参数
	TIMER KaiShuSleepTimer;
	uint32_t SetTimerVal;
#ifndef FUNC_WIFI_POWER_KEEP_ON
	uint8_t WiFiInitStep;
#endif
	static bool FirstPowerOnFlag = TRUE;
	
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif

	// 消息注册
	if(gSys.MuteFlag)
	{
		gSys.MuteFlag = 0;
		AudioPlayerMute(gSys.MuteFlag);
	}
	
#ifdef FUNC_TREB_BASS_EN
	if(gSys.EqStatus == 0)
	{
		TrebBassSet(gSys.TrebVal, gSys.BassVal);
	}
#endif
	TimeOutSet(&WiFiCmdRespTimer, 0);
	TimeOutSet(&WiFiPowerOffTimer, 0);
	TimeOutSet(&KaiShuSleepTimer, 0);
	InitTimer(&WiFiInTimer, WIFI_LED_CB_TIME, WiFiLedCb);
	StartTimer(&WiFiInTimer);    

	if(McuSoftSdkVerNum == 0)
	{
		McuSoftSdkVerNum = GetSdkVer() >> 24;
	}

	if(gWiFi.WiFiNotifyChangeModeFlag == FALSE)
	{
		if(gSys.CurModuleID == MODULE_ID_PLAYER_WIFI_USB)
	    {
		   //SoundRemind(SOUND_UDISC_PLAYBACK); 
		   APP_DBG("enter MODULE_ID_PLAYER_WIFI_USB player\n");		
	    }
		else if(gSys.CurModuleID == MODULE_ID_PLAYER_WIFI_SD)
		{
		   //SoundRemind(SOUND_CARD_PLAYBACK); 
		   APP_DBG("enter MODULE_ID_PLAYER_WIFI_SD player\n");
		}	
		else
		{
		   SoundRemind(SOUND_WIFI_MODE); 
		   APP_DBG("enter  WiFi player\n");
		}
	}

	gWiFi.WiFiNotifyChangeModeFlag = FALSE;
	
	AudioSysInfoSetBreakPoint();
	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	WiFiTalkOnFlag = FALSE;
	gWiFi.OtherModuleWiFiAudioEn = FALSE;
	
#ifdef FUNC_WIFI_POWER_KEEP_ON
	if(FirstPowerOnFlag == FALSE)
	{	
#ifdef FUNC_WIFI_TALK_AND_AUDIO_EFFECT_EN
		AudioAnaSetChannel(AUDIO_CH_MIC_I2SIN);
#else
		AudioAnaSetChannel(AUDIO_CH_I2SIN); 
#endif
	}
#else
	gCurNumberKeyNum = 0;
	WiFiPowerOnInitEndFlag = FALSE;
	WiFiControlGpioInit();		
	WiFiInitStep = 2;
	WiFiPowerOnInitTimeCnt = 10; //WiFi power on delay time
#endif

	while(Msg != MSG_COMMON_CLOSE)
	{
		CheckTimer(&WiFiInTimer);
#ifdef FUNC_WIFI_POWER_KEEP_ON
		if((FirstPowerOnFlag == TRUE) && (WiFiPowerOnInitEndFlag == TRUE))
		{					
#ifdef FUNC_WIFI_TALK_AND_AUDIO_EFFECT_EN
			AudioAnaSetChannel(AUDIO_CH_MIC_I2SIN); 
#else
			AudioAnaSetChannel(AUDIO_CH_I2SIN); 
#endif
			FirstPowerOnFlag = FALSE;
		}
#else
		if(WiFiPowerOnInitEndFlag == FALSE)
		{
			if(WiFiPowerOnInitTimeCnt < 1)
			{		
				if(WiFiInitStep == 2)
				{
					WiFiPowerOn(); 
					WiFiInitStep = 1;
					WiFiPowerOnInitTimeCnt = 5; //WiFi reset delay time
				}
				else if(WiFiInitStep == 1)
				{
					WiFiResetOn(); 
					WiFiInitStep = 0;
					WiFiPowerOnInitTimeCnt = WIFI_POWER_ON_INIT_TIME;
				}
				else
				{
					WiFi_UartInit();
					WiFiPowerOnInitProcess();
#ifdef FUNC_WIFI_TALK_AND_AUDIO_EFFECT_EN
					AudioAnaSetChannel(AUDIO_CH_MIC_I2SIN); 
#else
					AudioAnaSetChannel(AUDIO_CH_I2SIN);	
#endif
					WiFiPowerOnInitEndFlag = TRUE;
					WiFiPowerOnInitTimeCnt = WIFI_POWERON_RECONNECTION_TIME;
				}
			}
		}
#endif
		
#ifdef FUNC_WIFI_PASS_THROUGH_DATA_EN
		McuRevPassThroughDataProcess();
#endif
		
		if(gSys.OtherModuleTalkOn)
		{
			gSys.OtherModuleTalkOn = FALSE;
			Mcu_SendCmdToWiFi(MCU_TALK_ON, NULL);
		}
		//凯叔哄睡模式音量调整。
		if(IsTimeOut(&KaiShuSleepTimer) && (WIFI_PLAY_KAISHU_RADIO_SLEEP == gWiFi.KaiShuRadio))
		{
			SetTimerVal = 3600000/WiFiKaiShuSleepModeDeal(FALSE,FALSE);
			TimeOutSet(&KaiShuSleepTimer, SetTimerVal);
		}

		Msg = MsgRecv(5);

		switch(Msg)
		{	
			case MSG_COMMON_CLOSE:  //WiFi模式只有收到这个消息才会退出
			case MSG_MODE:        //working mode changing
				if(WiFiFirmwareUpgradeStateGet() == TRUE)
				{
					gSys.NextModuleID = MODULE_ID_UNKNOWN;
				  Msg = MSG_NONE;
				}
				else
				{
				  Msg = MSG_COMMON_CLOSE;
				}
				break;

			case MSG_POWER:       //Standy mode
				gSys.NextModuleID = MODULE_ID_STANDBY;
				break;

			case MSG_PLAY_PAUSE:
				Mcu_SendCmdToWiFi(MCU_PLY_PAU, NULL);
				break;

			case MSG_NEXT:
				Mcu_SendCmdToWiFi(MCU_PLY_NXT, NULL);
				break;

			case MSG_PRE:
				Mcu_SendCmdToWiFi(MCU_PLY_PRV, NULL);
				break;	

			/*case MSG_REPEAT:
				McuSetWiFiRepeatMode();				
				break;*/

			case MSG_WIFI_NEXT_CH:	//播放下一个按键预置歌单						
				gWiFi.KaiShuRadio++;
				if((gWiFi.KaiShuRadio > WIFI_PLAY_KAISHU_SDCARD)
				&& (gWiFi.KaiShuRadio < WIFI_PLAY_KAISHU_RADIO_MAX))
				{
					Mcu_SendCmdToWiFi(MCU_RAD_XXX, &gWiFi.KaiShuRadio);
				}
				else
				{
					gWiFi.KaiShuRadio = WIFI_PLAY_KAISHU_SDCARD;
					gSys.NextModuleID = MODULE_ID_PLAYER_WIFI_SD;
					MsgSend(MSG_COMMON_CLOSE);
				}
				break;

			case MSG_WIFI_PREV_CH:	
				if(WIFI_PLAY_KAISHU_SDCARD == gWiFi.KaiShuRadio)
				{
					gWiFi.KaiShuRadio = WIFI_PLAY_KAISHU_RADIO_MAX;
				}
				
				gWiFi.KaiShuRadio--;
				if((gWiFi.KaiShuRadio > WIFI_PLAY_KAISHU_SDCARD)
				&& (gWiFi.KaiShuRadio < WIFI_PLAY_KAISHU_RADIO_MAX))
				{
					Mcu_SendCmdToWiFi(MCU_RAD_XXX, &gWiFi.KaiShuRadio);
				}
				else
				{
					gWiFi.KaiShuRadio = WIFI_PLAY_KAISHU_SDCARD;
					gSys.NextModuleID = MODULE_ID_PLAYER_WIFI_SD;
					MsgSend(MSG_COMMON_CLOSE);
				}
				break;

			case MSG_WIFI_GROUP:
				if(gSys.CurModuleID == MODULE_ID_WIFI)
				{
					Mcu_SendCmdToWiFi(MCU_JNGROUP, NULL);
				}
				break;

			case MSG_WIFI_UNGROUP:
				if(gSys.CurModuleID == MODULE_ID_WIFI)
				{
					Mcu_SendCmdToWiFi(MCU_UNGROUP, NULL);
				}
				break;

			case MSG_REPEAT:
				break;

			case MSG_WIFI_FACTORY: //WiFi模组恢复出厂设置 
				if(WiFiPowerOnInitStateGet() == TRUE)
				{
					if(WiFiPowerOnInitStateGet() == TRUE)
					{
						if(gWiFi.TestModeState)
						{
							Mcu_SendCmdToWiFi(MCU_FCRYPOW, NULL);
						}
						else
						{
							Mcu_SendCmdToWiFi(MCU_FACTORY, NULL);
						}
						gWiFi.InitState = 0;
						gWiFi.StationState = WIFI_STATUS_STATION_DISCONNECTED;
						gWiFi.WPSState = WIFI_STATUS_WPS_SCAN_STOP;
						gWiFi.WWWState = WIFI_STATUS_WWW_DISABLE;
						gWiFi.RepeatMode = WIFI_PLAY_MODE_REPEAT_ALL;
						gCurNumberKeyNum = 0;				
					}
				}
				break;

			case MSG_NUM_1:
				if(gSys.CurModuleID == MODULE_ID_WIFI)
				{
					if((gWiFi.StationState == WIFI_STATUS_STATION_CONNECTED) || (gWiFi.WPSState == WIFI_STATUS_WPS_SCAN_STOP) 
					|| (gWiFi.WWWState == WIFI_STATUS_WWW_ENABLE))
					{		
						gCurNumberKeyNum = 1;	
						Mcu_SendCmdToWiFi(MCU_KEY_XNN, &gCurNumberKeyNum);
					}
				}
				break;

			case MSG_WIFI_PRESET1:
				if(gSys.CurModuleID == MODULE_ID_WIFI)
				{
					if((gWiFi.StationState == WIFI_STATUS_STATION_CONNECTED) || (gWiFi.WPSState == WIFI_STATUS_WPS_SCAN_STOP) 
					|| (gWiFi.WWWState == WIFI_STATUS_WWW_ENABLE))
					{
						gCurNumberKeyNum = 1;
						Mcu_SendCmdToWiFi(MCU_PRE_NNN, &gCurNumberKeyNum);
					}
				}
				break;
				
			case MSG_WIFI_PRESET2:
				if(gSys.CurModuleID == MODULE_ID_WIFI)
				{
					if((gWiFi.StationState == WIFI_STATUS_STATION_CONNECTED) || (gWiFi.WPSState == WIFI_STATUS_WPS_SCAN_STOP) 
					|| (gWiFi.WWWState == WIFI_STATUS_WWW_ENABLE))
					{
						gCurNumberKeyNum = 2;
						Mcu_SendCmdToWiFi(MCU_KEY_XNN, &gCurNumberKeyNum);
					}
				}
				break;

			case MSG_WIFI_SAVE:
				if(gSys.CurModuleID == MODULE_ID_WIFI)
				{
					Mcu_SendCmdToWiFi(MCU_PLY_LKE, NULL);
				}
				break;

			default:
				CommonMsgProccess(Msg);
				break;
		}
	}
	
#if (defined(FUNC_BREAKPOINT_EN) && (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN)))
	{
		BP_SYS_INFO *pBpSysInfo;
		pBpSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);
		BP_SET_ELEMENT(pBpSysInfo->Volume, gSys.Volume);
		BP_SET_ELEMENT(pBpSysInfo->Eq, gSys.Eq);
		BP_SaveInfo(BP_SAVE2NVM);
	}
#endif

#ifndef FUNC_OUTPUT_CHANNEL_AUTO_CHANGE
#if (!defined (OUTPUT_CHANNEL_CLASSD)) && (!defined (OUTPUT_CHANNEL_DAC_CLASSD))//usb 模式下微调
	DacSampRateAdjust(FALSE, 0);
#endif
#endif
#ifdef FUNC_WIFI_TALK_AND_AUDIO_EFFECT_EN
	gSys.MicEnable = FALSE;
#endif
#ifndef FUNC_WIFI_POWER_KEEP_ON
	WiFiPowerOff();
	WiFiControlGpioInit();	
	WaitMs(200);          //适当延时，防止WiFi电源出现自锁现象
#endif
	
#ifdef FUNC_BT_HF_EN
	GpioPcmSyncIoConfig(1);
#endif

	AudioAnaSetChannel(AUDIO_CH_NONE);	
	APP_DBG("leave WiFiControl...\n");
	return TRUE;           
}

#endif

