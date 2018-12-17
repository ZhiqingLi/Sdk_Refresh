/**
 **************************************************************************************
 * @file    bt_manager.c
 * @brief   management of all bluetooth event and apis
 *
 * @author  Halley
 * @version V1.1.0
 *
 * $Created: 2016-07-18 16:24:11$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
 

#include "type.h"
#include "bt_manager.h"


static BT_MANAGER_ST		btManager;

void BtStackCallback(uint32_t event, BT_STACK_CALLBACK_PARAMS * param)
{
	if(event > BT_STACK_EVENT_COMMON_EVENT_START && event < BT_STACK_EVENT_COMMON_EVENT_END)
	{
		BtCommandEvent(event, param);
	}
	else if(event > BT_STACK_EVENT_TRUST_LIST_EVENT_START && event < BT_STACK_EVENT_TRUST_LIST_EVENT_END)
	{
		BtTrustListEvent(event, param);
	}
	else if(event > BT_STACK_EVENT_A2DP_EVENT_START && event < BT_STACK_EVENT_A2DP_EVENT_END)
	{
		BtA2dpEvent(event, param);
	}
	else if(event > BT_STACK_EVENT_AVRCP_EVENT_START && event < BT_STACK_EVENT_AVRCP_EVENT_END)
	{
		BtAvrcpEvent(event, param);
	}
	else if(event > BT_STACK_EVENT_HFP_EVENT_START && event < BT_STACK_EVENT_HFP_EVENT_END)
	{
		BtHfpEvent(event, param);
	}
	else if(event > BT_STACK_EVENT_MFI_EVENT_START && event < BT_STACK_EVENT_MFI_EVENT_END)
	{
		BtMfiEvent(event, param);
	}
	else if(event > BT_STACK_EVENT_HID_EVENT_START && event < BT_STACK_EVENT_HID_EVENT_END)
	{
		
	}
	else
	{
		// unknown event
	}
}




BT_MANAGER_ST * GetBtManager(void)
{
	return &btManager;
}

bool SetLocalBtDeviceAddr(const uint8_t * devAddr)
{
	uint8_t					i;
	if(devAddr == NULL)
		return FALSE;

	for(i = 0; i < 6; i++)
	{
		GetBtManager()->btDevAddr[i] = *(devAddr+i);
	}

	return TRUE;
}

bool GetLocalBtDeviceAddr(uint8_t * devAddr)
{
	uint8_t					i;
	
	if(devAddr == NULL)
		return FALSE;

	for(i = 0; i < 6; i++)
	{
		*(devAddr+i) = GetBtManager()->btDevAddr[i];
	}

	return FALSE;
}

bool SetLocalBtDevName(const uint8_t * devName, uint16_t nameLen)
{
	if(devName == NULL || nameLen == 0)
		return FALSE;

	if(nameLen > MAX_DEVICE_NAME_LENGTH)
		return FALSE;

	memset(GetBtManager()->btDevName, 0 , MAX_DEVICE_NAME_LENGTH);

	memcpy(GetBtManager()->btDevName, devName, nameLen);

	return TRUE;
}

bool GetLocalBtDevName(uint8_t * devName, uint16_t * nameLen)
{
	uint8_t			i;
	if(devName == NULL || nameLen == NULL)
		return FALSE;

	for(i = 0; GetBtManager()->btDevName[i]; i++)
	{
		*(devName+i) = GetBtManager()->btDevName[i];
	}

	*nameLen = i;

	return TRUE;
}

void SetBtStackState(BT_STACK_STATE state)
{
	GetBtManager()->stackState = state;
}

BT_STACK_STATE GetBtStackState(void)
{
	return GetBtManager()->stackState;
}

bool SetBtDeviceConnMode(BT_DEVICE_CONNECTION_MODE mode)
{
	if(GetBtStackState() != BT_STACK_STATE_READY)
		return FALSE;

	GetBtManager()->deviceConMode = mode;

	return TRUE;
}

BT_DEVICE_CONNECTION_MODE GetBtDeviceConnMode(void)
{
	if(GetBtStackState() != BT_STACK_STATE_READY)
		return BT_DEVICE_CONNECTION_MODE_NONE;

	return GetBtManager()->deviceConMode;
}


