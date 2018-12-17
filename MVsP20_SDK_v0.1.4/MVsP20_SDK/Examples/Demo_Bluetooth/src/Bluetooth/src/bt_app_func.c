///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: bt_app_func.c
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////

#include "type.h"
#include "gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bt_stack_api.h"
#include "bt_platform_interface.h"
#include "bt_manager.h"
#include "bt_app_func.h"
#include "bt_config.h"
#include "delay.h"

extern PLATFORM_INTERFACE_BT_UART_T	pfiBtUart;
extern PLATFORM_INTERFACE_BT_DDB_T	pfiBtDdb;

static void * osMalloc(uint32_t size)
{
	return pvPortMalloc(size);
}

static void osFree(void * ptr)
{
	vPortFree(ptr);
}

PLATFORM_INTERFACE_OS_T		pfiOS = {
	osMalloc,
	osFree,
	NULL,
	NULL
};


void BtUartWorkModeSet(uint8_t workMode)
{
	if(workMode == BT_UART_WORKMODE_LOOP)
	{
		
	}
	else
	{
//		BuartIOctl(UART_IOCTL_RXINT_SET,1);
	}
}
uint32_t GetSupportProfiles(void)
{
	uint32_t		profiles = 0;

	
#if BT_HFP_SUPPORT == ENABLE
	profiles |= BT_PROFILE_SUPPORTED_HFP;
#endif

#if BT_A2DP_SUPPORT == ENABLE
	profiles |= BT_PROFILE_SUPPORTED_A2DP;
#endif

#if BT_AVRCP_SUPPORT == ENABLE
	profiles |= BT_PROFILE_SUPPORTED_AVRCP;
#endif

#if BT_SPP_SUPPORT == ENABLE
	profiles |= BT_PROFILE_SUPPORTED_SPP;
#endif

#if BT_OPP_SUPPORT == ENABLE
	profiles |= BT_PROFILE_SUPPORTED_OPP;
#endif

#if BT_HID_SUPPORT == ENABLE
	profiles |= BT_PROFILE_SUPPORTED_HID;
#endif

#if BT_MFI_SUPPORT == ENABLE
	profiles |= BT_PROFILE_SUPPORTED_MFI;
#endif

	return profiles;
}


void ConfigBtStackParams(BtStackParams *stackParams)
{
	if(stackParams == NULL)
		return ;

	memset(stackParams, 0 ,sizeof(BtStackParams));

	/* Set support profiles */
	stackParams->supportProfiles = GetSupportProfiles();

	/* Set local device name */
	strcpy(stackParams->localDevName, BT_NAME);

	/* Set callback function */
	stackParams->callback = BtStackCallback;

	/* HFP features */
	stackParams->hfpFeatures.wbsSupport = BT_HFP_SUPPORT_WBS;
	stackParams->hfpFeatures.hfpAudioDataFormat = BT_HFP_AUDIO_DATA;

	/* A2DP features */
	stackParams->a2dpFeatures.a2dpAudioDataFormat = BT_A2DP_AUDIO_DATA;

	/* AVRCP features */
	stackParams->avrcpFeatures.supportAdvanced = BT_AVRCP_ADVANCED;
	stackParams->avrcpFeatures.supportTgSide = BT_AVRCP_TG_SIDE;
}

bool InitBt(void)
{
	bool ret;


	BtStackParams	stackParams;


	memset(GetBtManager(), 0, sizeof(BT_MANAGER_ST));

	SetBtPlatformInterface(&pfiBtUart, &pfiOS, &pfiBtDdb);

	ret = BtDeviceInit();
	if(!ret)
	{
		return FALSE;
	}

	BtUartWorkModeSet(BT_UART_WORKMODE);

//	SetBtStackDebugInfo(BT_STATCK_DEBUG_INFO_COMMON);

	ConfigBtStackParams(&stackParams);

	BTStackRunInit(&stackParams);

#if 0
	{
		#include "ble_api.h"
		extern BLE_APP_CONTEXT			g_playcontrol_app_context;
		extern GATT_SERVER_PROFILE		g_playcontrol_profile;
		InitBlePlaycontrolProfile();
		InitBleStack(&g_playcontrol_app_context, &g_playcontrol_profile);
	}
#endif
	return TRUE;
}

void UninitBt(void)
{
	BTStackRunUninit();
}


void BtStackProcess(void)
{
/*
	if(HasBtDataToProccess() || BuartIOctl(BUART_IOCTL_RXFIFO_DATLEN_GET, 0) != 0)
	{
		BTStackRun();
	}
	else
	{
		BuartIOctl(UART_IOCTL_RXINT_SET,1);
		OSQueueMsgRecv(NULL, NULL, 0xFFFFFFFF); // wait for buart intterupt ( new buart data arrived)
	}
*/
	BTStackRun();
	WaitMs(1);
}

void BtStackRunNotify(void)
{
//	OSQueueMsgSend(MSG_NEED_BT_STACK_RUN, NULL, 0, MSGPRIO_LEVEL_HI, 0);
}
