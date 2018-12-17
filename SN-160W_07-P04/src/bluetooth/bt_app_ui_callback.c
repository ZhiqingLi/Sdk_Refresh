///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: bt_app_ui_callback.c
//  maintainer: Halley
//BT application UI callback functions. This function is called by the BT stack.
//One can modify/implement this function but MUST NOT change the interface.
//
//用户必需参考下面的实现方式按需实现本函数，蓝牙协议栈内会调用此函数。
//

#include "type.h" 
#include "pcm_sync.h"
#include "app_config.h"
#include "fat_file.h"
#include "audio_decoder.h"
#include "player_control.h"
#include "audio_adc.h"
#include "sys_app.h"
#include "sys_vol.h"
#include "bt_app_ui_callback.h"
#include "bt_app_func.h"
#include "bt_control_api.h"
#include "bt_stack_api.h"
#include "string_convert.h"
#include "wifi_mcu_command.h"
#include "wifi_control.h"

#ifdef FUNC_BT_EN

#ifdef MASS_PRODUCTION_TEST_FUNC
static bool BtReadyFlag = FALSE;
void OtherMassTestFunc(void);
bool GetMassTestFlag(void);
#endif

#ifdef BT_RECONNECTION_FUNC
#ifdef BT_POWERON_RECONNECTION
#ifdef FUNC_WIFI_BT_CONTROL_EN
bool power_on_reconnection_flag = FALSE;
#else
bool power_on_reconnection_flag = TRUE;
#endif
#endif
//extern bool BtReConnect(void);
#endif

#ifdef AUTO_MV_BT_TEST_FUNC
static unsigned char MvBtAutoTestFlag = 0; //1 means enable auto test, one MAY implement oneself to enable it.
extern bool FlshUpdateBtLatestDevAddr(uint8_t* BtAddr);
#endif

bool restart_btstack = FALSE;

void BTStackStatusCallBackFunc(uint8_t CBParameter)
{
	switch(CBParameter)
	{
		case BT_UI_CB_STATUS_IDLE:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_IDLE\n");
#ifdef MASS_PRODUCTION_TEST_FUNC
			if(!BtReadyFlag)
			{
				BtReadyFlag = TRUE;

				if(GetMassTestFlag())
				{
					BTDoDeviceInquiry(10); //for i/o connection test function ONLY
					break;
				}
			}
#endif
			APP_DBG("[BT] WaitForPairing/Connection ... \n");

#ifdef FAST_POWER_ON_OFF_BT
			if(gSys.CurModuleID != MODULE_ID_BLUETOOTH)
			{
			#ifdef BT_POWERON_RECONNECTION
				power_on_reconnection_flag = FALSE;
			#endif
				FastDisableBT();
			}
#endif
#ifdef BT_RF_POWER_ON_OFF
			if(gSys.CurModuleID != MODULE_ID_BLUETOOTH)
			{
			#ifdef BT_POWERON_RECONNECTION
				power_on_reconnection_flag = FALSE;
			#endif
			}
#endif
#ifdef BT_POWERON_RECONNECTION
			if(power_on_reconnection_flag)
			{
#ifdef AUTO_MV_BT_TEST_FUNC
				if(MvBtAutoTestFlag)
				{
					BTDoTestModeDeviceInquiry(30);
					DBG("[BT] MvBtAutoTestFlag = 1 and start inq.\n");
				}
				else
#endif
				{
					StartBtReConnection(BT_POR_TRY_COUNTS, BT_POR_INTERNAL_TIME); //start bt reconnection
				}
				power_on_reconnection_flag = FALSE;
			}
			else
			{
#ifdef FUNC_WIFI_BT_CONTROL_EN
				if(BtCtrl.ScanEnable_Timer == FALSE)
				{
					if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
					{
						BTSetScanEnable();
						APP_DBG("Bluetooth enter ScanEnable!\n");
					}
					else
					{
						BTSetScanDisable();
						APP_DBG("Bluetooth enter ScanDisable!\n");
					}
				}
#else
				if(!IsBtReConnectionFinish())
				{
					BtReConnect();
				}
				else if(!GetCurBtConnectedFlag())
				{
					BTSetScanEnable();
					APP_DBG("Bluetooth enter ScanEnable!\n");
				}
#endif
			}
#endif
			break;

		case BT_UI_CB_STATUS_PAIRED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_PAIRED\n");
			{
				uint8_t			src_len = 0;
				uint8_t	*		p_src = BTGetRemoteDevName();
				uint8_t			des[60] = {0};
				uint8_t			*addr = GetCurConnectedRemDevAdd();

				while(1)
				{
					if(*(p_src + src_len) == 0)
						break;

					src_len++;
				}

				StringConvert(des, 60, p_src, src_len, 0x02);
#ifdef FUNC_WIFI_BT_CONTROL_EN
				memcpy((void*)&BtCtrl.ScanBtDevice[0], addr, 6);
				memcpy((void*)&BtCtrl.ScanBtDevice[6], des, src_len);
#endif
				APP_DBG("[BT] UI :GET_REMOTE_BT_DEV_NAME : %s\n", des);
				
				FlshUpdatePairedName(addr, &des[0]);
			}

			break;

		case BT_UI_CB_STATUS_A2DP_CONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_A2DP_CONNECTED\n");
#ifdef FUNC_WIFI_BT_CONTROL_EN
			BtCtrl.ScanEnable_Timer = 0;
			{
				uint8_t			  src_len = 0;
				uint8_t	  * 	  p_src = BTGetRemoteDevName();
				uint8_t			  des[60] = {0};
				uint8_t			  *addr = GetCurConnectedRemDevAdd();

				while(1)
				{
					if(*(p_src + src_len) == 0)
						break;
					src_len++;
				}

				StringConvert(des, 60, p_src, src_len, 0x02);
				memcpy((void*)&BtCtrl.ScanBtDevice[0], addr, 6);
				memcpy((void*)&BtCtrl.ScanBtDevice[6], des, src_len);
				
				if(BtCtrl.State == BT_CTRL_STATE_CTPL)
				{
					Mcu_SendBtCmdToWiFi(MCU_BT_CTPS, NULL);
				}
				else
				{
					if(BtCtrl.State == BT_CTRL_STATE_IDLE)
					{
						//同步蓝牙连接状态
						BtCtrl.State = BT_CTRL_STATE_CTDI;
					}
					Mcu_SendBtCmdToWiFi(MCU_BT_PRDS, NULL);
				}
			}				  
#endif

#ifdef BT_RECONNECTION_FUNC
			StopBtReConnection();
#endif
			if(!IsA2dpConnected())
			{
				APP_DBG("[BT] A2DP Connected\n");
				ConnectedProfile(BT_CONNECTED_A2DP_FLAG);
				{
					uint8_t	*addr = GetCurConnectedRemDevAdd();
					FlshUpdatePairedProfile(addr, PROFILE_A2DP);
				}
			}
			BTAudioPlayStart();
			SetA2dpState(BT_A2DP_STATE_CONNECTED);
			break;

		case BT_UI_CB_STATUS_PLAY_CONTROL_CONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_PLAY_CONTROL_CONNECTED\n");

			if(!IsAvrcpConnected())
			{
				ConnectedProfile(BT_CONNECTED_AVRCP_FLAG);
				
				{
					uint8_t	*addr = GetCurConnectedRemDevAdd();
					FlshUpdatePairedProfile(addr, PROFILE_AVRCP);
				}
			}
			SetAvrcpState(BT_AVRCP_STATE_CONNECTED);
			break;

		case BT_UI_CB_STATUS_DISCONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_DISCONNECTED\n");
			if(IsA2dpConnected())
			{
				DisconnectProfile(BT_CONNECTED_A2DP_FLAG);

				APP_DBG("[BT] A2DP DisConnected\n");
				BTAudioPlayStop();
				if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
				{
					SetPlayState(PLAYER_STATE_PAUSE);
				}

				SetA2dpState(BT_A2DP_STATE_NONE);
			}

			if(IsAvrcpConnected())
			{
				DisconnectProfile(BT_CONNECTED_AVRCP_FLAG);
				SetAvrcpState(BT_AVRCP_STATE_NONE);
			}
#ifdef BT_ADVANCED_AVRCP_SUPPORT
			SetAdvAvrcpSupportFlag(FALSE);
			SetAdvAvrcpPlayStatus(CurStatus_ERROR);
#endif
#ifdef BT_RECONNECTION_FUNC
			if(!IsBtReConnectionFinish()) 
			{
				BtReConnect();
			}
			else 
			{
				StopBtReConnection();
			}
#endif
#ifdef FUNC_WIFI_BT_CONTROL_EN
			if(BtCtrl.State == BT_CTRL_STATE_IDLE)
			{
				Mcu_SendBtCmdToWiFi(MCU_BT_DCTS, NULL);
			}
#endif
			break;

		case BT_UI_CB_STATUS_ERROR:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_ERROR\n");
			break;

		case BT_UI_CB_STATUS_PAGE_TIME_OUT:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_PAGE_TIME_OUT\n");
#ifdef BT_RECONNECTION_FUNC
			if(!IsBtReConnectionFinish())
			{
				BtReConnect();
			}
			else
			{
				StopBtReConnection();
			}
#endif
			break;

#ifdef FUNC_BT_HF_EN
////HF////////////////////////////////////////////////////////////////
		case BT_UI_CB_STATUS_HF_DISCONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_DISCONNECTED\n");
			if(IsHfpConnected())
			{
				APP_DBG("[BT] HF disconnected\n");

				DisconnectProfile(BT_CONNECTED_HF_FLAG);

				if(gSys.CurModuleID == MODULE_ID_BT_HF || GetHfModeState() == BT_HF_MODE_STATE_ENTERING || GetHfModeState() == BT_HF_MODE_STATE_ACTIVE)
				{
					ExitHfMode();
				}
			}
#if 0 //def BT_RECONNECTION_FUNC
			if(!IsBtReConnectionFinish())
			{
				StopBtReConnection();
			}
#endif
#ifdef BT_HF_NUMBER_REMIND
			if(IsRemindingPhoneNumbers())
				StopRemindingPhoneNumbers();
#endif
			SetHfpState(BT_HFP_STATE_NONE);
			break;

		case BT_UI_CB_STATUS_HF_CONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_CONNECTED\n");
#ifdef BT_RECONNECTION_FUNC
			StopBtReConnection();
#endif
			if(!IsHfpConnected())
			{
				APP_DBG("[BT] HF connected \n");
				ConnectedProfile(BT_CONNECTED_HF_FLAG);
				{
					uint8_t	*addr = GetCurConnectedRemDevAdd();
					FlshUpdatePairedProfile(addr, PROFILE_HFP);
				}
			}

			BTQueryPhonebooks();

			if(GetHfpState() > BT_HFP_STATE_CONNECTED)
			{
				if(gSys.CurModuleID != MODULE_ID_BT_HF)
				{
					EnterHfMode();
				}
			}
			else
			{
				SetHfpState(BT_HFP_STATE_CONNECTED);
			}
			break;

		case BT_UI_CB_STATUS_HF_INCOME_RING:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_INCOME_RING\n");
			if(IsHfpConnected())
			{
				//只有在呼入第一个电话时,会有该MSG
				SetHfpState(BT_HFP_STATE_INCOMING_CALL);
			}
			break;

		case BT_UI_CB_STATUS_HF_CALL_SETUP_NONE:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_CALL_SETUP_NONE\n");
			if(IsHfpConnected())
			{
				switch(GetHfpState())
				{
					case BT_HFP_STATE_3WAY_INCOMING_CALL:
					case BT_HFP_STATE_3WAY_OUTGOING_CALL:
						DBG("[BT]: 3way calling,\n");
						SetHfpState(BT_HFP_STATE_3WAY_ATCTIVE_CALL);
						break;
					
					case BT_HFP_STATE_3WAY_ATCTIVE_CALL:
						break;
				
					case BT_HFP_STATE_INCOMING_CALL:
					case BT_HFP_STATE_OUTGOING_CALL:
						if(gSys.CurModuleID == MODULE_ID_BT_HF || GetHfModeState() == BT_HF_MODE_STATE_ENTERING || GetHfModeState() == BT_HF_MODE_STATE_ACTIVE)
						{
							ExitHfMode();
						}
#ifdef BT_HF_NUMBER_REMIND
						if(IsRemindingPhoneNumbers())
							StopRemindingPhoneNumbers();
#endif
						SetHfpState(BT_HFP_STATE_CONNECTED);
						
						{
							uint8_t	*addr = GetCurConnectedRemDevAdd();
							FlshUpdatePairedProfile(addr, PROFILE_A2DP);
						}
						
						break;
					case BT_HFP_STATE_ATCTIVE_CALL:
					default:
						break;
				}
			}
			break;

		case BT_UI_CB_STATUS_HF_CALL_SETUP_INCOMING:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_CALL_SETUP_INCOMING\n");
			
			if(GetHfpState() >= BT_HFP_STATE_ATCTIVE_CALL)
			{
				SetHfpState(BT_HFP_STATE_3WAY_INCOMING_CALL);
				//将第二个呼入电话hold
				//BTCallHold(HF_HOLD_RELEASE_HELD_CALLS);
			}
			else
			{
				SetHfpState(BT_HFP_STATE_INCOMING_CALL);
			}
			
			//BtSoundTip(BT_SOUND_TIP_PHONE_CALL_INCOMING);
			if(IsHfpConnected())
			{
				if(gSys.CurModuleID != MODULE_ID_BT_HF)
				{
					EnterHfMode();
				}
			}
			break;

		case BT_UI_CB_STATUS_HF_CALL_SETUP_OUTCALL_ALERT:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_CALL_SETUP_OUTCALL_ALERT\n");
			
			if(GetHfpState() >= BT_HFP_STATE_ATCTIVE_CALL)
			{
				SetHfpState(BT_HFP_STATE_3WAY_OUTGOING_CALL);
			}
			else
			{
				SetHfpState(BT_HFP_STATE_OUTGOING_CALL);
			}
			
			if(IsHfpConnected())
			{
				if(gSys.CurModuleID != MODULE_ID_BT_HF)
				{
					EnterHfMode();
				}
			}
			break;

		case BT_UI_CB_STATUS_HF_INCOME_CALL_NUM:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_INCOME_CALL_NUM\n");
			if(IsHfpConnected())
			{
				APP_DBG("[BT] UI : Call number = %s\n", BTGetCurInCallPhoneNum());
				SetHfpState(BT_HFP_STATE_INCOMING_CALL);
				if(gSys.CurModuleID != MODULE_ID_BT_HF)
				{
					EnterHfMode();
				}

#ifdef BT_HF_NUMBER_REMIND
				//if(!IsRemindingPhoneNumbers() && !IsScoConneted())
				if(!IsRemindingPhoneNumbers())
				{
					RemindPhoneNumbers(BTGetCurInCallPhoneNum(), strlen((char*)BTGetCurInCallPhoneNum()));
				}
#else
				if((GetHfpState() == BT_HFP_STATE_INCOMING_CALL) && !IsScoConneted())
				{
					BtSoundTip(BT_SOUND_TIP_PHONE_CALL_INCOMING);
				}
#endif
			}
			break;

		//在通话中,呼入新电话,只会反馈1次电话号码上来
		case BT_UI_CB_STATUS_HF_WAIT_INCOME_CALL_NUM:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_WAIT_INCOME_CALL_NUM\n");
			if(IsHfpConnected())
			{
				APP_DBG("[BT] UI : Wait Call number = %s\n", BTGetCurInCallPhoneNum());
				
				if(gSys.CurModuleID != MODULE_ID_BT_HF)
				{
					EnterHfMode();
				}
			}
			break;

		case BT_UI_CB_STATUS_HF_CALL_CONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_CALL_CONNECTED\n");
			
			if(GetHfpState() > BT_HFP_STATE_ATCTIVE_CALL)
			{
				SetHfpState(BT_HFP_STATE_3WAY_ATCTIVE_CALL);
			}
			else
			{
				SetHfpState(BT_HFP_STATE_ATCTIVE_CALL);
			}
			
			if(IsHfpConnected())
			{
				if(gSys.CurModuleID != MODULE_ID_BT_HF)
				{
					EnterHfMode();
				}
				else
				{
					if((PcmSyncAecGetStatus() == PCMSYNC_AEC_NONE) || (PcmSyncAecGetStatus() == PCMSYNC_AEC_MV))
					{
						BTCallDisableAEC();
						APP_DBG("[BT] BTCallDisableAEC\n");
					}
				}

				// start bt hf timer
				if(IsBtHfTimerInited())
				{
					StartBtHfTimer();
				}
#ifdef BT_HF_NUMBER_REMIND
				if(IsRemindingPhoneNumbers())
					StopRemindingPhoneNumbers();
#endif
			}
			break;

		case BT_UI_CB_STATUS_HF_CALL_DISCONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_CALL_DISCONNECTED\n");
			if(IsHfpConnected())
			{
				if(gSys.CurModuleID == MODULE_ID_BT_HF || GetHfModeState() == BT_HF_MODE_STATE_ENTERING || GetHfModeState() == BT_HF_MODE_STATE_ACTIVE)
				{
					ExitHfMode();
				}
#ifdef BT_HF_NUMBER_REMIND
				if(IsRemindingPhoneNumbers())
					StopRemindingPhoneNumbers();
#endif
				SetHfpState(BT_HFP_STATE_CONNECTED);
			}
			break;

		case BT_UI_CB_STATUS_HF_CALL_SETUP_OUT:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_CALL_SETUP_OUT\n");
			
			if((GetHfpState() >= BT_HFP_STATE_ATCTIVE_CALL))
			{
				SetHfpState(BT_HFP_STATE_3WAY_OUTGOING_CALL);
			}
			else
			{
				SetHfpState(BT_HFP_STATE_OUTGOING_CALL);
			}
			
			if(IsHfpConnected())
			{
				if(gSys.CurModuleID != MODULE_ID_BT_HF)
				{
					EnterHfMode();
				}
			}
			break;
		case BT_UI_CB_STATUS_HF_PHONE_SPEAKER_VOL:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_HF_PHONE_SPEAKER_VOL vol = %d\n", BtGetRemoteSpeakerVol());
			if(gSys.CurModuleID == MODULE_ID_BT_HF && IsHfpConnected())
			{
				SetBtHfVol(BtGetRemoteSpeakerVol());
			}
			break;

		case BT_UI_CB_STATUS_HF_FACTORY_NAME:
			APP_DBG("BT_UI_CB_STATUS_HF_FACTORY_NAME : %s\n", BTHfGetFactoryName());
			break;
		
		case BT_UI_CB_STATUS_HF_PHONE_TYPE:
			APP_DBG("BT_UI_CB_STATUS_HF_PHONE_TYPE : %s\n", BTHfGetPhoneType());
			break;
		
#endif// FUNC_BT_HF_EN
		case BT_UI_CB_STATUS_GET_NAME_PAGE_TIMEOUT:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_GET_NAME_PAGE_TIMEOUT\n");
		case BT_UI_CB_STATUS_GET_REMOTE_BT_DEV_NAME:
			{
				uint8_t			src_len = 0;
				uint8_t	*		p_src = BTGetRemoteDevName();
				uint8_t			des[60] = {0};

				while(1)
				{
					if(*(p_src + src_len) == 0)
						break;

					src_len++;
				}
				
#ifdef FUNC_STRING_CONVERT_EN
				StringConvert(des, 60, p_src, src_len, 0x02);
#endif
				
				APP_DBG("[BT] UI : BT_UI_CB_STATUS_GET_REMOTE_BT_DEV_NAME : %s\n", des);
			}
//			APP_DBG("[BT] UI : BT_UI_CB_STATUS_GET_REMOTE_BT_DEV_NAME : %s\n", BTGetRemoteDevName());
#ifdef AUTO_MV_BT_TEST_FUNC
			if(strstr((const char*)BTGetRemoteDevName(),"MVBTTester01")) /*NOICE: [MVBTTester01] is the KEY string for MV BT tester*/
			{
				BTEnableBluetoothTestMode();
			}

			if(MvBtAutoTestFlag && (strstr((const char*)BTGetRemoteDevName(),"MVBTTester01"))) /*NOICE: [MVBTTester01] is the KEY string for MV BT tester*/
			{              
				APP_DBG("MVBTTester01 is found.\n");
				BTEnableBluetoothTestMode();
				FlshUpdateBtLatestDevAddr(BTGetCurDeviceNamesAddr());
				MvBtAutoTestFlag = 0;//clear
				BTConnect(BTGetCurDeviceNamesAddr());              
			}
			else
			{
				if(MvBtAutoTestFlag)
				{
					//not found MV tester
					//inq another one
					BTDoTestModeDeviceInquiry(30);
					APP_DBG("inq once more.\n");
				}
			}
#endif
			break;
///////////////////// phonebook ////////////////////////////////////////
		case BT_UI_CB_STATUS_PB_SUPPORTED:
			APP_DBG("supported PB bitmap = 0x%02x\n",BTGetSupportedPhonebookStorageType());
			APP_DBG("bit0-7: DC FD LD MC, ME MT RC SM\n\n");
//			BTGetPBListEntry(1,10);//One may select phonebook first, I am here just load the default PB's data for testing purpose.

		break;

		case BT_UI_CB_STATUS_PB_ENTRY_SIZE:
			{
				CurPhonebookSize *info;
				info = (CurPhonebookSize*)BtGetCurrentPbResponseData();
				APP_DBG("pb size==>%d\n",info->IdxLast);
			}
			break;

		case BT_UI_CB_STATUS_PB_ONE_ENTRY_DATA:
			{
				OnePhonebookEntry *data;
				data = (OnePhonebookEntry*)BtGetCurrentPbResponseData();
				APP_DBG("call list==>[%d]%s\n",data->Idx,data->PhoneNumber);
			}
			break;

		case BT_UI_CB_STATUS_CLCC_RESULT:
			APP_DBG("cur call list ==> %s\n",BtGetCurCallListNumberResponseData());
			break;

////////SPP//////////////////////////////////////////////////////////////
		case BT_UI_CB_STATUS_SPP_CONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_SPP_CONNECTED\n");
#ifdef BT_RECONNECTION_FUNC
			StopBtReConnection();
#endif
			if(!IsSppConnected())
			{
				APP_DBG("[BT] SPP connected \n");
				ConnectedProfile(BT_CONNECTED_SPP_FLAG);
				{
					uint8_t			*addr = GetCurConnectedRemDevAdd();
					FlshUpdatePairedProfile(addr, PROFILE_SPP);
				}
			}
			break;

		case BT_UI_CB_STATUS_SPP_DISCONNECTED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_SPP_DISCONNECTED\n");
			if(IsSppConnected())
			{
				APP_DBG("[BT] SPP Disconnected \n");

				DisconnectProfile(BT_CONNECTED_SPP_FLAG);
			}
			break;

		case BT_UI_CB_STATUS_INQUIRY_RESULT:
			//return Inquiry result params
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_INQUIRY_RESULT\n");
#ifdef MASS_PRODUCTION_TEST_FUNC
			if(GetMassTestFlag())
			{
				OtherMassTestFunc();
			}
#endif
			{
				InquriyResultParam inquriyResultParam;
				inquriyResultParam = BtGetInquiryResultParams();

				{
					uint8_t src_len = 0;
					uint8_t	des[60] = {0};
					uint8_t	* p_src = &inquriyResultParam.extResp[2];
					src_len = inquriyResultParam.extResp[0] - 1;
					
					if(src_len<=60)
					{
						APP_DBG("Inquriy Result:\n");
						APP_DBG("\t%02x:%02x:%02x:%02x:%02x:%02x",
								(inquriyResultParam.addr)[0],
								(inquriyResultParam.addr)[1],
								(inquriyResultParam.addr)[2],
								(inquriyResultParam.addr)[3],
								(inquriyResultParam.addr)[4],
								(inquriyResultParam.addr)[5]);
						APP_DBG("\t %d dB \n", inquriyResultParam.rssi);
					
						//printf device name
						StringConvert(des, 60, p_src, src_len, 0x02);
						APP_DBG("GET_REMOTE_BT_DEV_INFOR : LEN = [%d]; NAME = [%s]\n", src_len, des);
#ifdef FUNC_WIFI_BT_CONTROL_EN
						if(BtCtrl.Scan_Enable)
						{
							memset((void*)&BtCtrl.ScanBtDevice[0], 0, 67);
							memcpy((void*)&BtCtrl.ScanBtDevice[0], (void*)&inquriyResultParam.addr[0], 6);
							memcpy((void*)&BtCtrl.ScanBtDevice[6], des, src_len);
							Mcu_SendBtCmdToWiFi(MCU_BT_SNDU, NULL);	
						}	
#endif	
					}
					else
					{
						APP_DBG("Err: Get inquriy Result Params Err\n");
					}
				}
			}
			break;

		case BT_UI_CB_STATUS_INQUIRY_COMPLETE:
#ifdef FUNC_WIFI_BT_CONTROL_EN
			BtCtrl.Scan_Enable = 0;
			Mcu_SendBtCmdToWiFi(MCU_BT_SNDU, NULL);
#endif
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_INQUIRY_COMPLETE\n");
			break;

		case BT_UI_CB_STATUS_INQUIRY_CANCELED:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_INQUIRY_CANCELED\n");
			break;

		case BT_UI_CB_STATUS_CONNECTION_LOST:
			APP_DBG("[BT] UI : BT_UI_CB_STATUS_CONNECTION_LOST\n");
			if(IsHfpConnected())
			{
				APP_DBG("[BT] HF disconnected\n");

				DisconnectProfile(BT_CONNECTED_HF_FLAG);

				if(gSys.CurModuleID == MODULE_ID_BT_HF || GetHfModeState() == BT_HF_MODE_STATE_ENTERING || GetHfModeState() == BT_HF_MODE_STATE_ACTIVE)
				{
					ExitHfMode();
				}
				
				SetHfpState(BT_HFP_STATE_NONE);
			}

			if(IsA2dpConnected())
			{
				DisconnectProfile(BT_CONNECTED_A2DP_FLAG);

				APP_DBG("[BT] A2DP DisConnected\n");
				BTAudioPlayStop();
				if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
				{
					SetPlayState(PLAYER_STATE_PAUSE);
				}

				SetA2dpState(BT_A2DP_STATE_NONE);
			}

			if(IsAvrcpConnected())
			{
				DisconnectProfile(BT_CONNECTED_AVRCP_FLAG);
				SetAvrcpState(BT_AVRCP_STATE_NONE);
			}
			
			if(IsSppConnected())
			{
				APP_DBG("[BT] SPP Disconnected \n");

				DisconnectProfile(BT_CONNECTED_SPP_FLAG);
			}
			DisconnectAllProfile();
#ifdef BT_BB_LOST_RECONNECTION
			StartBtReConnection(BT_BLR_TRY_COUNTS, BT_BLR_INTERNAL_TIME);
#endif
			break;
////Hid///////////////////////////////////////////////////////////////
		case BT_UI_CB_STATUS_HID_OUT_DATA_IND:
			APP_DBG("[BT] BT_UI_CB_STATUS_HID_OUT_DATA_IND \n");
			break;
		case BT_UI_CB_STATUS_BT_MFi_CHANNEL_OPEN:
			APP_DBG("[BT] BT_UI_CB_STATUS_BT_MFi_CHANNEL_OPEN OK\n");
			break;
		case BT_UI_CB_STATUS_BT_MFi_DATA_IND:
			APP_DBG("[BT] BT_UI_CB_STATUS_BT_MFi_DATA_IND \n");
			{
				uint8_t *Data;
				int32_t Len;
				BtGetMfiDataInfo(&Data,&Len);//get the incoming data
			}
			break;
		case BT_UI_CB_STATUS_BT_MFi_DATA_SENT:
			APP_DBG("[BT] BT_UI_CB_STATUS_BT_MFi_DATA_SENT\n");//data has just sent, so one can send another package if any.
			break;

//////////////////// advanced avrcp //////////////////////////
#ifdef BT_ADVANCED_AVRCP_SUPPORT
		case BT_UI_CB_STATUS_ADV_AVRCP_SUPPORTED:
			APP_DBG("BT_UI_CB_STATUS_ADV_AVRCP_SUPPORTED\n");
			SetAdvAvrcpSupportFlag(TRUE);
			break;
		case BT_UI_CB_STATUS_ADV_AVRCP_NOT_SUPPORTED:
			APP_DBG("BT_UI_CB_STATUS_ADV_AVRCP_NOT_SUPPORTED\n");
			SetAdvAvrcpSupportFlag(FALSE);
			break;

		#ifdef BT_SONG_PLAY_STATUS
		case BT_UI_CB_STATUS_PLAYING_STATUS:
			{
				AdvAvrcpPlayStatus *CurStatus;
				CurStatus = (AdvAvrcpPlayStatus *)BtGetAdvAvrcpResponse();
				if(CurStatus)
				{
					SetAdvAvrcpPlayStatus(CurStatus->CurStatus);

					switch(CurStatus->CurStatus)
					{
						case CurStatus_STOPPED:
							break;
							
						case CurStatus_PLAYING:
							APP_DBG("Playing > %02d:%02d / %02d:%02d \n",
	                        		(CurStatus->CurPosInMs/1000)/60,
	                        		(CurStatus->CurPosInMs/1000)%60,
	                        		(CurStatus->TrackLengthInMs/1000)/60,
	                        		(CurStatus->TrackLengthInMs/1000)%60);
							break;

						case CurStatus_PAUSED:
							APP_DBG("Paused || %02d:%02d / %02d:%02d  \n",
	                        		(CurStatus->CurPosInMs/1000)/60,
	                        		(CurStatus->CurPosInMs/1000)%60,
	                        		(CurStatus->TrackLengthInMs/1000)/60,
	                        		(CurStatus->TrackLengthInMs/1000)%60);
							break;

						case CurStatus_FWD_SEEK:
							APP_DBG("FF >> %02d:%02d / %02d:%02d  \n",
	                        		(CurStatus->CurPosInMs/1000)/60,
	                        		(CurStatus->CurPosInMs/1000)%60,
	                        		(CurStatus->TrackLengthInMs/1000)/60,
	                        		(CurStatus->TrackLengthInMs/1000)%60);
							break;

						case CurStatus_REV_SEEK:
							APP_DBG("FB << %02d:%02d / %02d:%02d  \n",
	                        		(CurStatus->CurPosInMs/1000)/60,
	                        		(CurStatus->CurPosInMs/1000)%60,
	                        		(CurStatus->TrackLengthInMs/1000)/60,
	                        		(CurStatus->TrackLengthInMs/1000)%60);
							break;

						default:
							break;
					}
				}
			}
			break;
		#endif

		#ifdef BT_SONG_TRACK_INFOR
		case BT_UI_CB_STATUS_TRACK_INFO:
			APP_DBG("BT_UI_CB_STATUS_TRACK_INFO\n");
			{
				#define StringMaxLen 60
				AdvAvrcpTrackInfo	*CurTranckInfo;
				uint8_t i;
				uint8_t StringData[StringMaxLen];
				uint8_t ConvertStringData[StringMaxLen];
				CurTranckInfo = (AdvAvrcpTrackInfo *)BtGetAdvAvrcpResponse();
				if((CurTranckInfo)&&(CurTranckInfo->ItemNum))
				{
					for(i=0;i<CurTranckInfo->ItemNum;i++)
					{
						memset(StringData, 0, StringMaxLen);
						memset(ConvertStringData, 0, StringMaxLen);
						
						//Attribute ID
						switch(CurTranckInfo->Property[i].TackAttrID)
						{
							case 1:
								APP_DBG("Title of the media\n");
								break;

							case 2:
								APP_DBG("Name of the artist\n");
								break;

							case 3:
								APP_DBG("Name of the Album\n");
								break;

							case 4:
								APP_DBG("Number of the media\n");
								break;

							case 5:
								APP_DBG("Totle number of the media\n");
								break;

							case 6:
								APP_DBG("Genre\n");
								break;

							case 7:
								APP_DBG("Playing time in millisecond\n");
								break;
							
							case 8:
								APP_DBG("Default cover art\n");
								break;

							default:
								break;
						}

						if(CurTranckInfo->Property[i].PropertyStringLen)
						{
							if(CurTranckInfo->Property[i].CharacterSetID == 0x006a)
							{
								APP_DBG("Character Set Id: UTF-8\n");
								
								if(CurTranckInfo->Property[i].PropertyStringLen > StringMaxLen)
								{
									memcpy(StringData, CurTranckInfo->Property[i].PropertyString, StringMaxLen);
								#ifdef FUNC_STRING_CONVERT_EN
									StringConvert(ConvertStringData, 60, StringData, StringMaxLen ,UTF8_TO_GBK);
								#endif
								}
								else
								{
									memcpy(StringData, CurTranckInfo->Property[i].PropertyString, CurTranckInfo->Property[i].PropertyStringLen);
								#ifdef FUNC_STRING_CONVERT_EN
									StringConvert(ConvertStringData, 60, StringData, CurTranckInfo->Property[i].PropertyStringLen ,UTF8_TO_GBK);
								#endif
								}
								
								DBG("%s\n", ConvertStringData);
							}
							else
							{
								APP_DBG("Other Character Set Id: 0x%x\n", CurTranckInfo->Property[i].CharacterSetID);
							}
						}
					}
				}
				
			}

			break;
		#endif

		#ifdef BT_VOLUME_SYNC
		case BT_UI_CB_STATUS_ADV_ABSOLUTE_VOLUME:
			{
				uint8_t volPercent;
				extern uint8_t BtGetAdvAbsVolume(void);

				volPercent = BtGetAdvAbsVolume();
				APP_DBG("BT_UI_CB_STATUS_ADV_ABSOLUTE_VOLUME %d\n", volPercent);
				gSys.Volume = MAX_VOLUME * volPercent / 100;
				SetSysVol();
			}
			break;
		#endif
#endif
///////////////////////////////////////////////////////////////////////
		case BT_UI_CB_STATUS_BB_NO_RESPONSE:
			APP_DBG("[BT] BT_UI_CB_STATUS_BB_NO_RESPONSE, One MAY reboot...\n");
			DisconnectAllProfile();
			restart_btstack = TRUE;
			BtHwPowerOff();
			break;
			
        case BT_UI_CB_STATUS_RSSI_LINK_QUALITY_IND:
            APP_DBG("rssi = %d dB; linkq = %d.\n", BtGetRssiValue(),BtGetLinkqualityValue());
            break;

		default:
			APP_DBG("[BT] UI : unsupported code = 0x%02x\n", CBParameter);
			break;

	}
}

#else

void BTStackStatusCallBackFunc(uint8_t CBParameter)
{
	
}

#endif


