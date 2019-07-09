///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: line_in_control.c
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "task_decoder.h"
#include "bt_app_func.h"
#include "bt_control_api.h"
#include "bt_stack_api.h"
#include "dev_detect_driver.h"
#include "sd_card.h"
#include "eq.h"
#include "eq_params.h"
#include "clk.h"
#include "fsinfo.h"
#include "sys_vol.h"
#include "sound_remind.h"
#include "mixer.h"
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
#include "player_control.h"
#include "random.h"
#include "wifi_uart_com.h"
#include "wifi_control.h"
#include "wifi_init_setting.h"
#include "wifi_mcu_command.h"
#include "wifi_wifi_command.h"
#include "uart.h"


#ifdef FUNC_WIFI_BT_CONTROL_EN

WIFI_CTRL_BT BtCtrl;

//蓝牙地址ACSII码转换
void BtAddrAcsiiToHex(uint8_t* AcsiiInBuf, uint8_t* HexOutBuf)
{
	uint8_t Temp,AddrIndex,TempBuf;

	Temp = 0;
	AddrIndex = 0;
	for(Temp = 0; Temp < 17; ++Temp)
	{
		TempBuf = AcsiiInBuf[Temp];
		
		if(TempBuf < 0x3a)							//数字0-9
		{
			TempBuf = TempBuf-0x30;
		}
		else if(TempBuf < 0x47)					//大写字符A-F
		{
			TempBuf = TempBuf-0x41+10;
		}
		else												//小写字符a-f
		{
			TempBuf = TempBuf-0x61+10;
		}

		if(Temp%3 == 0)
		{
			HexOutBuf[AddrIndex] = TempBuf;
		}
		else if(Temp%3 == 1)
		{
			HexOutBuf[AddrIndex] = (HexOutBuf[AddrIndex]<<4) + TempBuf;
		}
		else if(Temp%3 == 2)									//跳过“-”
		{
//			++Temp;
			++AddrIndex;
		}	
	}

	APP_DBG("BtAddrAcsiiToHex = %x:%x:%x:%x:%x:%x;\n",HexOutBuf[0],HexOutBuf[1],HexOutBuf[2],HexOutBuf[3],HexOutBuf[4],HexOutBuf[5]);
}

//蓝牙地址ACSII码转换
void BtAddrHexToAcsii(uint8_t* HexInBuf, uint8_t* AcsiiOutBuf)
{
	uint8_t Temp,AddrIndex,HexTempBuf;

	APP_DBG("BtAddrHexToAcsii = %x:%x:%x:%x:%x:%x;\n",HexInBuf[0],HexInBuf[1],HexInBuf[2],HexInBuf[3],HexInBuf[4],HexInBuf[5]);
	AddrIndex = 0;
	for(Temp = 0; Temp < 6; ++Temp)
	{
		HexTempBuf = HexInBuf[Temp];
		AcsiiOutBuf[AddrIndex] = (HexTempBuf>>4);
		if(AcsiiOutBuf[AddrIndex] < 10)							//数字0-9
		{
			AcsiiOutBuf[AddrIndex++] = AcsiiOutBuf[AddrIndex]+0x30;
		}
		else													//字符A-F
		{
			AcsiiOutBuf[AddrIndex++] = AcsiiOutBuf[AddrIndex]-10+0x61;
		}

		HexTempBuf = (HexTempBuf&0x0F);
		if(HexTempBuf < 10)
		{
			AcsiiOutBuf[AddrIndex++] = HexTempBuf+0x30;
		}
		else
		{
			AcsiiOutBuf[AddrIndex++] = HexTempBuf-10+0x61;
		}
		
		if(Temp < 5)											//增加“-”
		{
			AcsiiOutBuf[AddrIndex++] = '-';
		}	
	}
}

//MCU发送给WiFi模组的命令数据
void Mcu_SendBtCmdToWiFi(uint16_t McuCmd, uint8_t* DataBuf)
{	
	uint16_t Temp = 0;
	uint8_t Len = 0;
	uint8_t CmdBuf[255];
	int8_t BtAddrBuf[6];

	if(!gWiFi.InitState && (WiFiPowerOnInitStateGet() == FALSE))
	{
		BtCtrl.State = BT_CTRL_STATE_IDLE;
		return;
	}
	
	Len = 0;
	switch(McuCmd)
	{
		case MCU_BT_EDMS:
			memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+EDMS", 11);
			Len = 11;
			break;

		case MCU_BT_EDMF:
			memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+EDMF", 11);
			Len = 11;
			break;

		case MCU_BT_PARF:
			if(BtCtrl.State == BT_CTRL_STATE_PAIR)
			{
				memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+PRDF", 11);
			}
			else if(BtCtrl.State == BT_CTRL_STATE_UNPR)
			{
				memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+UPRF", 11);		
			}
			Len = 11;
			BtCtrl.State = BT_CTRL_STATE_IDLE;
			break;

		case MCU_BT_CTIF:
			memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+CTIF:UUID:", 17);		
			Len = 17;
			BtAddrHexToAcsii((void*)&BtCtrl.ScanBtDevice[0], (void*)&CmdBuf[Len]);
			Len += 17;
			memcpy((void*)&CmdBuf[Len], (void*)":FRIENDLYNAME:", 14);
			Len += 14;
			for(Temp = 0;Temp < 60;Temp++)
			{
				if(BtCtrl.ScanBtDevice[6+Temp] != 0)
					CmdBuf[Len++] = BtCtrl.ScanBtDevice[6+Temp];
				else
					break;
			}
			CmdBuf[Len++] = '&';
			BtCtrl.State = BT_CTRL_STATE_IDLE;
			break;

		case MCU_BT_CTPS:
			memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+CTPS:UUID:", 17);
			Len = 17;
			BtAddrHexToAcsii((void*)&BtCtrl.ScanBtDevice[0], (void*)&CmdBuf[Len]);
			Len += 17;
			memcpy((void*)&CmdBuf[Len], (void*)":PROFILE_NAME:A2DP-SOURCE:FRIENDLYNAME:", 39);
			Len += 39;
			for(Temp = 0;Temp < 60;Temp++)
			{
				if(BtCtrl.ScanBtDevice[6+Temp] != 0)
					CmdBuf[Len++] = BtCtrl.ScanBtDevice[6+Temp];
				else
					break;
			}
			CmdBuf[Len++] = '&';
			BtCtrl.State = BT_CTRL_STATE_IDLE;
			break;

		case MCU_BT_CTPF:
			memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+CTPF:PROFILE_NAME:A2DP-SOURCE", 36);			
			Len = 36;
			CmdBuf[Len++] = '&';
			BtCtrl.State = BT_CTRL_STATE_IDLE;
			break;

		case MCU_BT_DCTS:
			memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+DCTS:UUID:", 17);
			Len = 17;
			BtAddrHexToAcsii(GetCurConnectedRemDevAdd(), (void*)&CmdBuf[Len]);
			Len += 17;
			memcpy((void*)&CmdBuf[Len], (void*)":FRIENDLYNAME:", 14);
			Len += 14;
 		    {
				uint8_t			src_len = 0,i;
				uint8_t	*		p_src = BTGetRemoteDevName();
				uint8_t			des[60] = {0};

				while(1)
				{
					if(*(p_src + src_len) == 0)
						break;
					src_len++;
				}

				StringConvert(des, 60, p_src, src_len, 0x02);
				for(i = 0;i < src_len;i++)
				{
					CmdBuf[Len++] = des[i];
				}
			}
			CmdBuf[Len++] = '&';
			BtCtrl.State = BT_CTRL_STATE_IDLE;
			break;

		case MCU_BT_DCTF:
			memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+DCTF:UUID:", 17);
			Len = 17;
			memcpy((void*)&CmdBuf[Len], (void*)&DataBuf[0], 17);
			Len += 17;
			CmdBuf[Len++] = '&';
			BtCtrl.State = BT_CTRL_STATE_IDLE;
			break;

		case MCU_BT_GPRL:
			memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+GPRL:UUID:", 17);
			Len = 17;
			BtAddrHexToAcsii(&(btPairedList[DataBuf[0]].bdAddr[0]), (void*)&CmdBuf[Len]);
			Len += 17;
			memcpy((void*)&CmdBuf[Len], (void*)":PROFILENUM:1:NAME:A2DP-SOURCE:VER:V2.0:FRIENDLYNAME:", 53);
			Len += 53;
			for(Temp = 0;Temp < 60;Temp++)
			{
				if(btPairedList[DataBuf[0]].deviceName[Temp] != 0)
					CmdBuf[Len++] = btPairedList[DataBuf[0]].deviceName[Temp];
				else
					break;
			} 
			CmdBuf[Len++] = '&';	
			break;

		case MCU_BT_PRDS:
			if(BtCtrl.State == BT_CTRL_STATE_PAIR)
			{
				memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+PRDS:UUID:", 17);
			}
			else if(BtCtrl.State == BT_CTRL_STATE_UNPR)
			{
				memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+UPRS:UUID:", 17);
			}
			else if(BtCtrl.State == BT_CTRL_STATE_CTDI)
			{
				memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+CTIS:UUID:", 17);
			}
			Len = 17;
			BtAddrHexToAcsii((void*)&BtCtrl.ScanBtDevice[0], (void*)&CmdBuf[Len]);
			Len += 17;
			memcpy((void*)&CmdBuf[Len], (void*)":FRIENDLYNAME:", 14);
			Len += 14;
			for(Temp = 0;Temp < 60;Temp++)
			{
				if(BtCtrl.ScanBtDevice[6+Temp] != 0)
					CmdBuf[Len++] = BtCtrl.ScanBtDevice[6+Temp];
				else
					break;
			}
			CmdBuf[Len++] = '&';
			BtCtrl.State = BT_CTRL_STATE_IDLE;
			break;

		case MCU_BT_STAS:
			if(GetPlayState() == PLAYER_STATE_PLAYING)
			{
				memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+PLAY", 11);
			}
			else
			{
				memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+STOP", 11);
			}
			Len = 11;
			break;

		case MCU_BT_SNDU:
			memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+SNDU:UUID:", 17);
			Len = 17;
			BtAddrHexToAcsii((void*)&BtCtrl.ScanBtDevice[0], (void*)&CmdBuf[Len]);
			Len += 17;
			memcpy((void*)&CmdBuf[Len], (void*)":More:", 6);
			Len += 6;
			if(BtCtrl.Scan_Enable) 
				CmdBuf[Len++]='1';
			else
				CmdBuf[Len++]='0';
			memcpy((void*)&CmdBuf[Len], (void*)":FRIENDLYNAME:", 14);
			Len += 14;
			for(Temp = 0;Temp < 60;Temp++)
			{
				if(BtCtrl.ScanBtDevice[6+Temp] != 0)
					CmdBuf[Len++] = BtCtrl.ScanBtDevice[6+Temp];
				else
					break;
			}
			CmdBuf[Len++] = '&';
			break;

		case MCU_BT_SNDF:
			memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+SNDF", 11);
			Len = 11;
			break;
			
		case MCU_BT_NAME:
			memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+NAME:", 12);
			Len = 12;
			for(Temp = 0;Temp < 60;Temp++)
			{
				if(BtDevName[Temp] != 0)
					CmdBuf[Len++] = BtDevName[Temp];
				else
					break;
			}
			CmdBuf[Len++] = '_';
			GetBtAddr(BtAddrBuf);
			for(Temp = 0;Temp < 3;Temp++)
			{	
				CmdBuf[Len] = (BtAddrBuf[Temp]>>4);
				if(CmdBuf[Len] < 10)							//数字0-9
				{
					CmdBuf[Len++] = CmdBuf[Len]+0x30;
				}
				else													//字符A-F
				{
					CmdBuf[Len++] = CmdBuf[Len]-10+0x61;
				}

				CmdBuf[Len] = (BtAddrBuf[Temp]&0x0F);
				if(CmdBuf[Len] < 10)							//数字0-9
				{
					CmdBuf[Len++] = CmdBuf[Len]+0x30;
				}
				else													//字符A-F
				{
					CmdBuf[Len++] = CmdBuf[Len]-10+0x61;
				}
			}
			CmdBuf[Len++] = '&';
			break;

		case MCU_BT_PLAY:
			if(DataBuf[0] == FALSE)
			{
				memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+PLYF", 11);
			}
			else
			{
				memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+PLYS", 11);
			}
			Len = 11;
			break;
			
		case MCU_BT_STOP:
			if(DataBuf[0] == FALSE)
			{
				memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+STPF", 11);
			}
			else
			{
				memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+STPS", 11);
			}
			Len = 11;
			break;

		case MCU_BT_PREV:
			if(DataBuf[0] == FALSE)
			{
				memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+PRVF", 11);
			}
			else
			{
				memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+PRVS", 11);
			}
			Len = 11;
			break;

		case MCU_BT_NEXT:
			if(DataBuf[0] == FALSE)
			{
				memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+NXTF", 11);
			}
			else
			{
				memcpy((void*)&CmdBuf[Len], (void*)"MCU+BT+NXTS", 11);
			}
			Len = 11;
			break;

		default:
			break;
	}

	CmdBuf[Len] = '\0';

#ifdef WIFI_SELECT_FUART_COM
	FuartSend(CmdBuf, Len);
#else
	BuartSend(CmdBuf, Len);
#endif

	APP_DBG("Send gWiFiBtCmd is:%s; Len = %d\n", CmdBuf, Len);	
}

//WiFi模组发送给MCU的命令功能处理
void WiFi_SendBtCmdToMcu(uint16_t WiFiCmd, uint8_t* CmdData)
{
	uint8_t  WorkStatus = FALSE;
	uint8_t  Temp_Cnt;
	uint8_t  BtAddr[6];
		
	switch(WiFiCmd)
	{		
		case AXX_BT_ENDM:
			if(GetCurBtConnectedFlag())
			{
				BTDisConnect();
				WaitMs(500);
			}
			
			if(BTSetScanEnable())	
			{ 
			  	BtCtrl.ScanEnable_Timer = (CmdData[0]-0x30)*100;
				BtCtrl.ScanEnable_Timer += (CmdData[0]-0x30)*10;
				BtCtrl.ScanEnable_Timer += (CmdData[0]-0x30);
				BtCtrl.ScanEnable_Timer = (BtCtrl.ScanEnable_Timer*1000);
				Mcu_SendBtCmdToWiFi(MCU_BT_EDMS, NULL);
			}				
			else
			{	
				Mcu_SendBtCmdToWiFi(MCU_BT_EDMF, NULL);	
				BtCtrl.ScanEnable_Timer = 0;	
			}				
			break;

		case AXX_BT_EXDM:
			BTSetScanDisable();
			BtCtrl.ScanEnable_Timer = 0;
			break;

		case AXX_BT_PAIR:
			BtAddrAcsiiToHex((void*)&CmdData[0], (void*)&BtAddr[0]);
			if(GetNumberOfConnectedProfile() && (memcmp(BtAddr, GetCurConnectedRemDevAdd(), 6) == 0))
			{
				Mcu_SendBtCmdToWiFi(MCU_BT_PRDS, NULL);
			}
			else
			{
				if(GetNumberOfConnectedProfile())
				{
					BTDisConnect();
					WaitMs(500);
				}
				BtCtrl.ScanEnable_Timer = 20*1000;
				BTConnect(BtAddr);
				APP_DBG("Bt start paired!\n");
			}
			break;

		case AXX_BT_UNPR:
			BtAddrAcsiiToHex((void*)&CmdData[0], (void*)&BtAddr[0]);
			for(Temp_Cnt = 0;Temp_Cnt < BT_DEVICE_NUM;Temp_Cnt++)
			{
		    	if(memcmp(btPairedList[Temp_Cnt].bdAddr, BtAddr, 6) == 0)
		    	{
					memcpy((void*)&BtCtrl.ScanBtDevice[0], btPairedList[Temp_Cnt].bdAddr, 6);
					memcpy((void*)&BtCtrl.ScanBtDevice[6], btPairedList[Temp_Cnt].deviceName, 60);
					APP_DBG("found UNPR bluetooth Addr!!\n");
					break;
		    	}
			}

			if((Temp_Cnt < BT_DEVICE_NUM) && !RemoveBtDevice(BtAddr))
			{
				Mcu_SendBtCmdToWiFi(MCU_BT_PRDS, NULL);
			}
			else
			{
				Mcu_SendBtCmdToWiFi(MCU_BT_PARF, NULL);
			}
			break;

		case AXX_BT_CTDI:
			BtAddrAcsiiToHex((void*)&CmdData[0], (void*)&BtAddr[0]);
			for(Temp_Cnt = 0;Temp_Cnt < BT_DEVICE_NUM;Temp_Cnt++)
			{
		    	if(memcmp(btPairedList[Temp_Cnt].bdAddr, BtAddr, 6) == 0)
		    	{
					memcpy((void*)&BtCtrl.ScanBtDevice[0], btPairedList[Temp_Cnt].bdAddr, 6);
					memcpy((void*)&BtCtrl.ScanBtDevice[6], btPairedList[Temp_Cnt].deviceName, 60);
					APP_DBG("found CTDI bluetooth Device!!\n");
					break;
		    	}
			}
			
			if(GetNumberOfConnectedProfile() && (memcmp(BtAddr, GetCurConnectedRemDevAdd(), 6) == 0))
			{
				Mcu_SendBtCmdToWiFi(MCU_BT_PRDS, NULL);
			}
			else if(Temp_Cnt < BT_DEVICE_NUM)
			{
				BtCtrl.ScanEnable_Timer = 20*1000;
				BTConnect(BtAddr);
			}
			else
			{
				memcpy((void*)&BtCtrl.ScanBtDevice[0], BtAddr, 6);
				memset((void*)&BtCtrl.ScanBtDevice[6], 0, 60);
				Mcu_SendBtCmdToWiFi(MCU_BT_CTIF, NULL);
			}
			break;

		case AXX_BT_CTPL:
			if(GetNumberOfConnectedProfile())
			{
				Mcu_SendBtCmdToWiFi(MCU_BT_CTPF, NULL);
			}
			else
			{
				BtCtrl.ScanEnable_Timer = 20*1000;
				BtTryToConnect();
			}
			break;

		case AXX_BT_DSCT:
			BtAddrAcsiiToHex((void*)&CmdData[0], (void*)&BtAddr[0]);
			for(Temp_Cnt = 0;Temp_Cnt < BT_DEVICE_NUM;Temp_Cnt++)
			{
		    	if(memcmp(btPairedList[Temp_Cnt].bdAddr, BtAddr, 6) == 0)
		    	{
					memcpy((void*)&BtCtrl.ScanBtDevice[0], btPairedList[Temp_Cnt].bdAddr, 6);
					memcpy((void*)&BtCtrl.ScanBtDevice[6], btPairedList[Temp_Cnt].deviceName, 60);
					APP_DBG("found DSCT bluetooth Device!!\n");
					break;
		    	}
			}

			if(GetNumberOfConnectedProfile() && (memcmp(BtAddr, GetCurConnectedRemDevAdd(), 6) == 0))
			{
				Mcu_SendBtCmdToWiFi(MCU_BT_DCTS, NULL);
				BTDisConnect();
			}
			else
			{
				Mcu_SendBtCmdToWiFi(MCU_BT_DCTF, (void*)&CmdData[0]);
			}
			break;

		case AXX_BT_GPRL:
			for(Temp_Cnt = 0;Temp_Cnt < 8;Temp_Cnt++)
			{
				if(btPairedList[Temp_Cnt].linkedProfile)
				{	
					Mcu_SendBtCmdToWiFi(MCU_BT_GPRL, &Temp_Cnt);
				}	 
			}
			break;

		case AXX_BT_SACN:
			if(!GetNumberOfConnectedProfile())
			{
				BTSetInquiryMode(2); 
				BTDoDeviceInquiry(10); 
				BtCtrl.Scan_Enable = TRUE;
			}
			else
			{
				Mcu_SendBtCmdToWiFi(MCU_BT_SNDF, NULL);
			}
			break;

		case AXX_BT_NAME:
			Mcu_SendBtCmdToWiFi(MCU_BT_NAME, NULL);
			break;

		case AXX_BT_PLAY:
			if(IsAvrcpConnected())// && (GetPlayState() != PLAYER_STATE_PLAYING))
			{
				WorkStatus = BTPlayCtrlPlay();
				WaitMs(500);
			}
			
			Mcu_SendBtCmdToWiFi(MCU_BT_PLAY, &WorkStatus);
			break;
			
		case AXX_BT_STOP:
			if(IsAvrcpConnected())// && (GetPlayState() == PLAYER_STATE_PLAYING))
			{
				WorkStatus =  BTPlayCtrlPause();
				WaitMs(500);
			}
			
			Mcu_SendBtCmdToWiFi(MCU_BT_STOP, &WorkStatus);
			break;

		case AXX_BT_PREV:
			if(IsAvrcpConnected())// && (GetPlayState() == PLAYER_STATE_PLAYING))
			{
				WorkStatus =  BTPlayCtrlPreSong();
				WaitMs(500);
			}
			
			Mcu_SendBtCmdToWiFi(MCU_BT_PREV, &WorkStatus);
			break;
		
		case AXX_BT_NEXT:
			if(IsAvrcpConnected())// && (GetPlayState() == PLAYER_STATE_PLAYING))
			{
				WorkStatus =  BTPlayCtrlNextSong();
				WaitMs(500);
			}
			
			Mcu_SendBtCmdToWiFi(MCU_BT_NEXT, &WorkStatus);
			break;

		default:
			break;
	}
}

//WiFi模组命令的接送解析
void WiFi_CtrlBtCmdProcess(uint8_t* gCtrlCmd)
{
	if(memcmp(gCtrlCmd, "+BT+ENDM", 8) == 0)
	{		
		WiFi_SendBtCmdToMcu(AXX_BT_ENDM, &gCtrlCmd[18]);
	}
	else if(memcmp(gCtrlCmd, "+BT+EXDM", 8) == 0)
	{
		WiFi_SendBtCmdToMcu(AXX_BT_EXDM, NULL);
	}
	else if(memcmp(gCtrlCmd, "+BT+PAIR", 8) == 0)
	{
		BtCtrl.State = BT_CTRL_STATE_PAIR;
		WiFi_SendBtCmdToMcu(AXX_BT_PAIR, &gCtrlCmd[14]);
	}
	else if(memcmp(gCtrlCmd, "+BT+UNPR", 8) == 0)
	{
		BtCtrl.State = BT_CTRL_STATE_UNPR;
		WiFi_SendBtCmdToMcu(AXX_BT_UNPR, &gCtrlCmd[14]);
	}
	else if(memcmp(gCtrlCmd, "+BT+CTDI", 8) == 0)
	{
		BtCtrl.State = BT_CTRL_STATE_CTDI;
		WiFi_SendBtCmdToMcu(AXX_BT_CTDI, &gCtrlCmd[14]);
	}
	else if(memcmp(gCtrlCmd, "+BT+CTPL", 8) == 0)
	{
		BtCtrl.State = BT_CTRL_STATE_CTPL;
		WiFi_SendBtCmdToMcu(AXX_BT_CTPL, NULL);
	}
	else if(memcmp(gCtrlCmd, "+BT+DSCT", 8) == 0)
	{
		BtCtrl.State = BT_CTRL_STATE_DSCT;
		WiFi_SendBtCmdToMcu(AXX_BT_DSCT, &gCtrlCmd[14]);
	}
	else if(memcmp(gCtrlCmd, "+BT+GPRL", 8) == 0)
	{
		memset(&BtCtrl, 0, sizeof(BtCtrl));
		WiFi_SendBtCmdToMcu(AXX_BT_GPRL, NULL);
	}
	else if(memcmp(gCtrlCmd, "+BT+PLAY", 8) == 0)
	{
		WiFi_SendBtCmdToMcu(AXX_BT_PLAY, NULL);
	}
	else if(memcmp(gCtrlCmd, "+BT+STOP", 8) == 0)
	{
		WiFi_SendBtCmdToMcu(AXX_BT_STOP, NULL);
	}
	else if(memcmp(gCtrlCmd, "+BT+PREV", 8) == 0)
	{
		WiFi_SendBtCmdToMcu(AXX_BT_PREV, NULL);
	}
	else if(memcmp(gCtrlCmd, "+BT+NEXT", 8) == 0)
	{
		WiFi_SendBtCmdToMcu(AXX_BT_NEXT, NULL);
	}
	else if(memcmp(gCtrlCmd, "+BT+GETS", 8) == 0)
	{
		Mcu_SendBtCmdToWiFi(MCU_BT_STAS, NULL);
	}
	else if(memcmp(gCtrlCmd, "+BT+SACN", 8) == 0)
	{
		WiFi_SendBtCmdToMcu(AXX_BT_SACN, NULL);
	}
	else if(memcmp(gCtrlCmd, "+BT+NAME", 8) == 0)
	{
		WiFi_SendBtCmdToMcu(AXX_BT_NAME, NULL);
	}
}
#endif


