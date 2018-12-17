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

#include "bt_stack_callback.h"



#define MAX_DEVICE_NAME_LENGTH		50
#define MAX_PHONE_NUMBER_LENGTH		20

typedef enum
{
	BT_STACK_STATE_NONE,
	BT_STACK_STATE_READY	
} BT_STACK_STATE;

/**
* Bt device connectable mode. 
*/
typedef enum
{
	BT_DEVICE_CONNECTION_MODE_NONE,				/* cann't be discovered and connected*/
	BT_DEVICE_CONNECTION_MODE_DISCOVERIBLE, 	/* can be discovered*/
	BT_DEVICE_CONNECTION_MODE_CONNECTABLE,		/* can be connected*/
	BT_DEVICE_CONNECTION_MODE_ALL,				/* can be discovered and connected*/
} BT_DEVICE_CONNECTION_MODE;

typedef enum 
{
	BT_HFP_STATE_NONE,
	BT_HFP_STATE_CONNECTING,
	BT_HFP_STATE_CONNECTED,
	BT_HFP_STATE_INCOMING,
	BT_HFP_STATE_OUTGOING,
	BT_HFP_STATE_ACTIVE	
} BT_HFP_STATE;

typedef enum
{
	BT_A2DP_STATE_NONE,
	BT_A2DP_STATE_CONNECTING,
	BT_A2DP_STATE_CONNECTED,
	BT_A2DP_STATE_STREAMING,
} BT_A2DP_STATE;

typedef enum
{
	BT_AVRCP_STATE_NONE,
	BT_AVRCP_STATE_CONNECTING,
	BT_AVRCP_STATE_CONNECTED,	
} BT_AVRCP_STATE;


void BtStackCallback(uint32_t event, BT_STACK_CALLBACK_PARAMS * param);

typedef struct _BT_MANAGER_ST
{

	uint8_t					btDevAddr[6];
	uint8_t					btDevName[MAX_DEVICE_NAME_LENGTH];
	BT_STACK_STATE			stackState;
	BT_DEVICE_CONNECTION_MODE	deviceConMode;

	BT_HFP_STATE			hfpState;
	uint8_t					remoteHfpAddr[6];
	bool					scoConnected;
	uint8_t					phoneNumber[MAX_PHONE_NUMBER_LENGTH];
	bool					callWaitingFlag;
	uint8_t					batteryLevel;
	uint8_t					signalLevel;
	bool					roamFlag;
	bool					voiceRecognition;
	uint8_t					volGain;


	BT_A2DP_STATE			a2dpState;
	uint8_t					remoteA2dpAddr[6];


	BT_AVRCP_STATE			avrcpState;
	uint8_t					remoteAvrcpAddr[6];
} BT_MANAGER_ST;



BT_MANAGER_ST * GetBtManager(void);

bool SetLocalBtDeviceAddr(const uint8_t * devAddr);

bool GetLocalBtDeviceAddr(uint8_t * devAddr);

bool SetLocalBtDevName(const uint8_t * devName, uint16_t nameLen);

bool GetLocalBtDevName(uint8_t * devName, uint16_t * nameLen);

void SetBtStackState(BT_STACK_STATE state);

BT_STACK_STATE GetBtStackState(void);

bool SetBtDeviceConnMode(BT_DEVICE_CONNECTION_MODE mode);

BT_DEVICE_CONNECTION_MODE GetBtDeviceConnMode(void);


/*HFP*/

void BtHfpConnect(uint8_t * addr);

void BtHfpDisconnect(void);

BT_HFP_STATE GetHfpState(void);

int16_t GetHfpConnectedAddr(uint8_t * addr);

bool GetScoConnectFlag(void);


/*A2DP*/
BT_A2DP_STATE GetA2dpState(void);
int16_t GetA2dpConnectedAddr(uint8_t * addr);


/*AVRCP*/
BT_AVRCP_STATE GetAvrcpState(void);
int16_t GetAvrcpConnectedAddr(uint8_t * addr);

