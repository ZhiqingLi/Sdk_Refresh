/**
 **************************************************************************************
 * @file    bt_stack_service.c
 * @brief   
 *
 * @author  KK
 * @version V1.0.0
 *
 * $Created: 2018-2-9 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <string.h>
#include "type.h"
#include "app_config.h"
#include "gpio.h" //for BOARD
#include "debug.h"
#include "rtos_api.h"
#include "app_message.h"
#include "uarts.h"
#include "uarts_interface.h"
#include "dma.h"
#include "timeout.h"
#include "irqn.h"
//#include "ble_api.h"
#include "bt_config.h"
#include "bt_stack_api.h"
#include "bt_app_func.h"
#include "bt_stack_service.h"
#include "bt_play_mode.h"
#include "bt_play_api.h"
#include "bt_app_interface.h"
#include "ble_api.h"
#include "ble_app_func.h"
#include "bt_avrcp_api.h"
#include "bt_manager.h"
#include "mode_switch_api.h"
#include "main_task.h"
#include "bt_pbap_api.h"
#include "bt_platform_interface.h"

#include "bb_api.h"

#include "clk.h"
#include "reset.h"

#ifdef CFG_FUNC_AI
#include "ai.h"
#endif

#ifdef CFG_APP_BT_MODE_EN

#ifdef CFG_BT_BACKGROUND_RUN_EN
uint8_t gBtHostStackMemHeap[BT_STACK_MEM_SIZE];
#endif

//BR/EDR STACK SERVICE
#define BT_STACK_SERVICE_STACK_SIZE		640//768
#define BT_STACK_SERVICE_PRIO			4
#define BT_STACK_NUM_MESSAGE_QUEUE		10

//USER SERVICE //用来处理协议栈callback中用户需要处理的msg
#define BT_USER_SERVICE_STACK_SIZE		256
#define BT_USER_SERVICE_PRIO			3
#define BT_USER_NUM_MESSAGE_QUEUE		10

typedef struct _BtStackServiceContext
{
	xTaskHandle			taskHandle;
	MessageHandle		msgHandle;
	TaskState			serviceState;

	uint8_t				serviceWaitResume;	//1:蓝牙不在后台运行时,开启通话,退出播放模式,不能kill蓝牙协议栈

	uint8_t				bbErrorMode;
	uint8_t				bbErrorType;
}BtStackServiceContext;

static BtStackServiceContext	*btStackServiceCt = NULL;

BT_CONFIGURATION_PARAMS		*btStackConfigParams = NULL;

extern uint8_t bt_powerup_flag;

//TIMER btRfTimerHandle;
//static uint16_t btRfChipId = 0x0080;//default
//#define BTRF_CHIPID			0x0080

static uint32_t bbIsrCnt = 0;

/***********************************************************************************
 * 蓝牙测试盒校准频偏完成回调函数
 **********************************************************************************/
void BtFreqOffsetAdjustComplete(unsigned char offset)
{
	int8_t ret = 0;
	BT_DBG("++++++[BT_OFFSET]  offset:0x%x ++++++\n", offset);

	btManager.btLastAddrUpgradeIgnored = 1;

	//判断是否和当前默认值一致,不一致更新保存到flash
	if(offset != btStackConfigParams->bt_trimValue)
	{
		btStackConfigParams->bt_trimValue = offset;
	
		//save to flash
		ret = BtDdb_SaveBtConfigurationParams(btStackConfigParams);
		
		if(ret)
			BT_DBG("[BT_OFFSET]update Error!!!\n");
		else
			BT_DBG("$$$[BT_OFFSET] update $$$\n");
	}

	//清除最后1次配对记录
	BtDdb_LastBtAddrErase();
}

/***********************************************************************************
 * 
 **********************************************************************************/
void BtMidMessageManage(BtMidMessageId messageId, uint8_t Param)
{
	MessageContext		msgSend;
	MessageHandle 		msgHandle;

	switch(messageId)
	{
		case MSG_BT_MID_UART_RX_INT:
			msgHandle = GetBtStackServiceMsgHandle();
			msgSend.msgId = MSG_BTSTACK_RX_INT;
			MessageSend(msgHandle, &msgSend);
			break;

		case MSG_BT_MID_ACCESS_MODE_IDLE:
#ifdef BT_RECONNECTION_FUNC
			BtReconnectDevice();
#endif
			break;

		case MSG_BT_MID_PLAY_STATE_CHANGE:
			if((Param == BT_PLAYER_STATE_PLAYING)&&(GetA2dpState() == BT_A2DP_STATE_STREAMING))
			{
				msgHandle = GetMainMessageHandle();
				msgSend.msgId		= MSG_BT_A2DP_STREAMING;
				MessageSend(msgHandle, &msgSend);
			}
			
			msgHandle = GetBtPlayMessageHandle();
			if(msgHandle == NULL)
				break;

			if(GetBtPlayState() == Param)
				break;
			
			SetBtPlayState(Param);

			// Send message to bt play mode
			msgSend.msgId		= MSG_BT_PLAY_STATE_CHANGED;
			MessageSend(msgHandle, &msgSend);
			break;

		case MSG_BT_MID_VOLUME_CHANGE:
			msgHandle = GetBtPlayMessageHandle();
			if(msgHandle == NULL)
				break;
			
#if (BT_AVRCP_VOLUME_SYNC == ENABLE)
			SetBtSyncVolume(Param);
#endif
			// Send message to bt play mode
			msgSend.msgId		= MSG_BT_PLAY_SYNC_VOLUME_CHANGED;
			MessageSend(msgHandle, &msgSend);
			break;
			
		default:
			break;
	}
}

static void BtErrorReport2Stack(void)
{
	MessageContext		msgSend;
	MessageHandle 		msgHandle;
	msgHandle = btStackServiceCt->msgHandle;
	msgSend.msgId = MSG_BTSTACK_BB_ERROR;

	MessageSend(msgHandle, &msgSend);
	DBG("read the rf chip id error!\n");
}

static void CheckBtEventTimer(void)
{
	//获取蓝牙播放状态
	if(GetBtManager()->avrcpPlayStatusTimer.timerFlag)
	{
		if(IsTimeOut(&GetBtManager()->avrcpPlayStatusTimer.timerHandle))
		{
			BT_A2DP_STATE state = GetA2dpState();
			if(state == BT_A2DP_STATE_STREAMING)
			{
				BTCtrlGetPlayStatus();
				TimerStart_BtPlayStatus();
			}
			else
			{
				TimerStop_BtPlayStatus();
			}
		}
	}

#ifdef BT_RECONNECTION_FUNC
	if(GetBtManager()->btReconnectTimer.timerFlag & TIMER_STARTED)
	{
		if(IsTimeOut(&GetBtManager()->btReconnectTimer.timerHandle))
		{
			CheckBtReconnectTimer();
		}
	}
#endif

#ifdef BT_FAST_POWER_ON_OFF_FUNC
	BtScanPageStateCheck();
#endif

#if 0
	//bb/rf error
	if(IsTimeOut(&btRfTimerHandle))
	{
		TimeOutSet(&btRfTimerHandle, 2000);

		//DBG("t\n");

		if(GetRfChipId() != BTRF_CHIPID)
		{
			/*//reset bb
			MessageContext		msgSend;
			MessageHandle 		msgHandle;
			msgHandle = btStackServiceCt->msgHandle;
			msgSend.msgId = MSG_BTSTACK_BB_ERROR;

			MessageSend(msgHandle, &msgSend);
			DBG("read the rf chip id error!\n");
			*/

			DBG("rf error!/n");
			BtErrorReport2Stack();
		}

		if(!bbIsrCnt)
		{
			DBG("bb isr error!/n");
			BtErrorReport2Stack();
		}
		else
		{
			bbIsrCnt = 0;
		}

		if(GetRfPllLockState() != 0)
		{
			DBG("error:rf pll unlocked!/n");
			BtErrorReport2Stack();
		}
	}
#endif
}

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**
 * @brief	Get message receive handle of bt stack manager
 * @param	NONE
 * @return	MessageHandle
 */
MessageHandle GetBtStackServiceMsgHandle(void)
{
	if(!btStackServiceCt)
		return NULL;
	
	return btStackServiceCt->msgHandle;
}

TaskState GetBtStackServiceState(void)
{
	if(!btStackServiceCt)
		return 0;
	
	return btStackServiceCt->serviceState;
}

static void BtStackServiceEntrance(void * param)
{
	MessageContext		msgRecv;
	
	BT_DBG("BtStackServiceEntrance.\n");

	//BR/EDR init
	if(!BtStackInit())
	{
		BT_DBG("error init bt device\n");
		//出现初始化异常时,蓝牙协议栈任务挂起
		while(1)
		{
			MessageRecv(btStackServiceCt->msgHandle, &msgRecv, 0xFFFFFFFF);
		}
	}
	else
	{
		BT_DBG("bt device init success!\n");
	}

	//BLE init
#if (BLE_SUPPORT == ENABLE)
	{
		InitBlePlaycontrolProfile();
		
		if(!InitBleStack(&g_playcontrol_app_context, &g_playcontrol_profile))
		{
			BT_DBG("error ble stack init\n");
		}
	}
#endif

	while(1)
	{
		MessageRecv(btStackServiceCt->msgHandle, &msgRecv, 1);

		switch(msgRecv.msgId)
		{
			case MSG_BTSTACK_BB_ERROR:
				{
					MessageContext		msgSend;
					MessageHandle 		msgHandle;
					msgHandle = GetMainMessageHandle();
					msgSend.msgId = MSG_BTSTACK_BB_ERROR;

					MessageSend(msgHandle, &msgSend);

					if(btStackServiceCt->bbErrorMode == 1)
					{
						DBG("BT ERROR:0x%x\n", btStackServiceCt->bbErrorType);
					}
					else if(btStackServiceCt->bbErrorMode == 2)
					{
						DBG("BLE ERROR:0x%x\n", btStackServiceCt->bbErrorType);
					}
				}
				break;
		}
		
		rw_main();
		BTStackRun();
		CheckBtEventTimer();
		#ifdef CFG_FUNC_AI
		ai_run_loop();
		#endif
	}
}

/**
 * @brief	Start bluetooth stack service initial.
 * @param	NONE
 * @return	
 */
static bool BtStackServiceInit(void)
{
	BT_DBG("bluetooth stack service init.\n");

	btStackServiceCt = (BtStackServiceContext*)osPortMalloc(sizeof(BtStackServiceContext));
	if(btStackServiceCt == NULL)
	{
		return FALSE;
	}
	memset(btStackServiceCt, 0, sizeof(BtStackServiceContext));
	
	btStackConfigParams = (BT_CONFIGURATION_PARAMS*)osPortMalloc(sizeof(BT_CONFIGURATION_PARAMS));
	if(btStackConfigParams == NULL)
	{
		return FALSE;
	}
	memset(btStackConfigParams, 0, sizeof(BT_CONFIGURATION_PARAMS));

	btStackServiceCt->msgHandle = MessageRegister(BT_STACK_NUM_MESSAGE_QUEUE);
	if(btStackServiceCt->msgHandle == NULL)
	{
		return FALSE;
	}
	btStackServiceCt->serviceState = TaskStateCreating;

	//register bt middleware message send interface
	BtAppiFunc_MessageSend(BtMidMessageManage);

	//bt rf module check
	//TimeOutSet(&btRfTimerHandle, 2000);

	return TRUE;
}

/**
 * @brief	Start bluetooth stack service.
 * @param	NONE
 * @return	
 */
 
bool BtStackServiceStart(void)
{
	bool		ret = TRUE;
	BtBbParams bbParams;
	if((btStackServiceCt->serviceWaitResume)&&(btStackServiceCt))
	{
		btStackServiceCt->serviceWaitResume = 0;
		return ret;
	}
	
	memset((uint8_t*)BB_EM_MAP_ADDR, 0, BB_EM_SIZE);//clear em erea
	
	ClearBtManagerReg();

	bt_powerup_flag = 1;
	
	SetBtStackState(BT_STACK_STATE_INITAILIZING);
	
	ret = BtStackServiceInit();
	if(ret)
	{
		btStackServiceCt->taskHandle = NULL;
		
		//load bt stack all params
		LoadBtConfigurationParams();
		
		//BB init
		ConfigBtBbParams(&bbParams);
		Bt_init((void*)&bbParams);

		//host memory init
		SetBtPlatformInterface(&pfiOS, &pfiBtDdb);
		
#ifdef CFG_BT_BACKGROUND_RUN_EN
		//在蓝牙开启后台运行时,host的内存采用数组,避免存在申请/释放带来碎片化的风险
		BTStackMemAlloc(BT_STACK_MEM_SIZE, gBtHostStackMemHeap, 0);
#else
		BTStackMemAlloc(BT_STACK_MEM_SIZE, NULL, 1);//PTS测试时需要开大内存
#endif
		xTaskCreate(BtStackServiceEntrance, 
					"BtStack", 
					BT_STACK_SERVICE_STACK_SIZE, 
					NULL, 
					BT_STACK_SERVICE_PRIO, 
					&btStackServiceCt->taskHandle);
		if(btStackServiceCt->taskHandle == NULL)
		{
			ret = FALSE;
		}
	}
	if(!ret)
		BT_DBG("BtStack service create fail!\n");
	return ret;
}

/**
 * @brief	Kill bluetooth stack service.
 * @param	NONE
 * @return	
 */
bool BtStackServiceKill(void)
{
	int32_t ret = 0;
	if(btStackServiceCt == NULL)
	{
		return FALSE;
	}

	//btStackService
	//Msgbox
	if(btStackServiceCt->msgHandle)
	{
		MessageDeregister(btStackServiceCt->msgHandle);
		btStackServiceCt->msgHandle = NULL;
	}
	
	//task
	if(btStackServiceCt->taskHandle)
	{
		vTaskDelete(btStackServiceCt->taskHandle);
		btStackServiceCt->taskHandle = NULL;
	}

	//deregister bt middleware message send interface
	BtAppiFunc_MessageSend(NULL);

#if (BLE_SUPPORT == ENABLE)
	UninitBleStack();
	UninitBlePlaycontrolProfile();
#endif

	//stack deinit
	ret = BtStackUninit();
	if(!ret)
	{
		DBG("Bt Stack Uninit fail!!!\n");
		return FALSE;
	}

	if(btStackConfigParams)
	{
		osPortFree(btStackConfigParams);
		btStackConfigParams = NULL;
	}
	//
	if(btStackServiceCt)
	{
		osPortFree(btStackServiceCt);
		btStackServiceCt = NULL;
	}
	DBG("!!btStackServiceCt\n");
	

	return TRUE;
}

//
void BtStackServiceWaitResume(void)
{
	btStackServiceCt->serviceWaitResume = 1;
}

//注:需要判断当前是否在中断中，需要调用不同的消息发送函数接口
extern uint32_t GetIPSR( void );
void WakeupBtStackService(void)
{
/*	MessageContext		msgSend;
	MessageHandle 		msgHandle;
	msgHandle = btStackServiceCt->msgHandle;
	msgSend.msgId = MSG_BTSTACK_RX_INT;

	if(GetIPSR())
		MessageSendFromISR(msgHandle, &msgSend);
	else
		MessageSend(msgHandle, &msgSend);
*/
}

void BBErrorReport(uint8_t mode, uint32_t errorType)
{
	MessageContext		msgSend;
	MessageHandle 		msgHandle;
	if(btStackServiceCt == NULL)
		return;
	
	msgHandle = btStackServiceCt->msgHandle;
	msgSend.msgId = MSG_BTSTACK_BB_ERROR;

	btStackServiceCt->bbErrorMode = mode;
	btStackServiceCt->bbErrorType = (uint8_t)errorType;

	//isr
	MessageSendFromISR(msgHandle, &msgSend);
}

void BBIsrReport(void)
{
	bbIsrCnt++;
}

void BT_IntDisable(void)
{
	NVIC_DisableIRQ(18);//BT_Interrupt =18
	NVIC_DisableIRQ(19);//BLE_Interrupt =19
}

void BT_ModuleClose(void)
{
	Reset_RegisterReset(MDM_REG_SEPA);
	Reset_FunctionReset(BTDM_FUNC_SEPA|MDM_FUNC_SEPA|RF_FUNC_SEPA);
	Clock_Module2Disable(ALL_MODULE2_CLK_SWITCH); //close clock
}


/***********************************************************************************
 * 
 **********************************************************************************/
uint8_t GetBtStackCt(void)
{
	if(btStackServiceCt)
		return 1;
	else
		return 0;
}

/***********************************************************************************
 * 快速开关蓝牙
 * 断开蓝牙连接，蓝牙进入不可被搜索，不可被连接状态
 **********************************************************************************/
#ifdef BT_FAST_POWER_ON_OFF_FUNC
void BtScanPageStateSet(BT_SCAN_PAGE_STATE state)
{
	btManager.btScanPageState = state;
}

BT_SCAN_PAGE_STATE BtScanPageStateGet(void)
{
	return btManager.btScanPageState;
}

void BtFastPowerOff(void)
{
	BtScanPageStateSet(BT_SCAN_PAGE_STATE_CLOSING);
}

void BtFastPowerOn(void)
{
	BtScanPageStateSet(BT_SCAN_PAGE_STATE_OPENING);
}

void BtScanPageStateCheck(void)
{
	static uint8_t bt_disconnect_count = 0;
	switch(btManager.btScanPageState)
	{
		case BT_SCAN_PAGE_STATE_CLOSING:
#ifdef BT_RECONNECTION_FUNC
			// If there is a reconnectiong process, stop it
			if(btManager.btReconnectTimer.timerFlag)
			{
				BtStopReconnect();
			}
#endif
			// If there is a bt link, disconnect it
			if(GetBtCurConnectFlag())
			{
				BTDisconnect();
				BtScanPageStateSet(BT_SCAN_PAGE_STATE_DISCONNECTING);
				break;
			}

			//不可被搜索不可被连接
			BTSetAccessMode(BtAccessModeNotAccessible);
			BtScanPageStateSet(BT_SCAN_PAGE_STATE_DISABLE);
			break;
			
		case BT_SCAN_PAGE_STATE_DISCONNECTING:
			if(bt_disconnect_count > 200)	// wait about 200ms
			{
				if(GetBtDeviceConnMode() != BT_DEVICE_CONNECTION_MODE_NONE)
				{
					BTSetAccessMode(BtAccessModeNotAccessible);
					BtScanPageStateSet(BT_SCAN_PAGE_STATE_DISABLE);
				}
				else
				{
					if(GetBtCurConnectFlag())
					{
						BTDisconnect();
						BtScanPageStateSet(BT_SCAN_PAGE_STATE_DISCONNECTING);
					}
				}
				bt_disconnect_count = 0;
			}
			else
				bt_disconnect_count++;
			break;
			
		case BT_SCAN_PAGE_STATE_DISABLE:
			// double check wether there is a bt link, if any, disconnect again
			if(GetBtCurConnectFlag())
			{
				BTDisconnect();
				BtScanPageStateSet(BT_SCAN_PAGE_STATE_DISCONNECTING);
			}
			break;

		case BT_SCAN_PAGE_STATE_OPENING:
			BTSetAccessMode(BtAccessModeGeneralAccessible);
			BtScanPageStateSet(BT_SCAN_PAGE_STATE_ENABLE);

			btManager.BtPowerOnFlag = 0;
			BtReconnectDevice();
			break;
			
		case BT_SCAN_PAGE_STATE_ENABLE:
			break;
			
		default:
			break;
	}
}

#endif
/***********************************************************************************
 * 开关蓝牙
 * 断开蓝牙连接，删除蓝牙协议栈任务，关闭蓝牙晶振
 **********************************************************************************/
void BtPowerOff(void)
{
	uint8_t btDisconnectTimeout = 0;
	if(!btStackServiceCt)
		return;
	
	BT_DBG("[Func]:Bt off\n");
	
	if(GetBtStackState() == BT_STACK_STATE_INITAILIZING)
	{
		while(GetBtStackState() == BT_STACK_STATE_INITAILIZING)
		{
			vTaskDelay(10);
			btDisconnectTimeout++;
			if(btDisconnectTimeout>=100)
				break;
		}

		//快速在BT模式和其他模式(共2个模式)切换，需要delay(500);避免蓝牙初始化和反初始化状态未完成导致的错误
		//vTaskDelay(500);
		vTaskDelay(50);
	}
	
	if(GetBtDeviceConnMode() == BT_DEVICE_CONNECTION_MODE_NONE)
	{
		BTDisconnect();
	}

	//在蓝牙回连时,需要先取消蓝牙回连行为
	if(GetBtManager()->btReconnectTimer.timerFlag)
	{
		BtStopReconnect();
		vTaskDelay(50);
	}

	//wait for bt disconnect, 2S timeout
	while(GetBtDeviceConnMode() == BT_DEVICE_CONNECTION_MODE_NONE)
	{
		vTaskDelay(10);
		btDisconnectTimeout++;
		if(btDisconnectTimeout>=200)
			break;
	}
	
	//bb reset
	rwip_reset();
	BT_IntDisable();
	//Kill bt stack service
	BtStackServiceKill();
	vTaskDelay(10);
	//reset bt module and close bt clock
	BT_ModuleClose();
}

void BtPowerOn(void)
{
	BT_DBG("[Func]:Bt on\n");
	vTaskDelay(50);
	Clock_Module2Enable(ALL_MODULE2_CLK_SWITCH);
	vTaskDelay(50);

	//bt stack restart
	BtStackServiceStart();
}


/***********************************************************************************
 * 
 **********************************************************************************/

#else

void WakeupBtStackService(void)
{
}

void BBErrorReport(void)
{
}

void BBIsrReport(void)
{
}

#endif

