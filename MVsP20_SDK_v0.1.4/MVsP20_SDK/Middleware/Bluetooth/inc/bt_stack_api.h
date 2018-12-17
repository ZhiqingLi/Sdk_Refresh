////////////////////////////////////////////////////////////////////////////////////////////
//					Mountain View Silicon Tech. Inc.
//	Copyright 2012, Mountain View Silicon Tech. Inc., ShangHai, China
//					All rights reserved.
//
//  bt stack related API
//  filename: bt_stack_api.h
//
////////////////////////////////////////////////////////////////////////////////////////////

/**
* @addtogroup Bluetooth
* @{
* @defgroup bt_stack_api bt_stack_api.h
* @{
*/

#ifndef __BT_STACK_API_H__
#define __BT_STACK_API_H__

#include "type.h"
#include "bt_stack_api.h"
#include "bt_stack_callback.h"

#include "bt_a2dp_api.h"
#include "bt_avrcp_api.h"
#include "bt_hfp_api.h"

typedef void (*BTStackCallbackFunc)(uint32_t event, BT_STACK_CALLBACK_PARAMS * param);

/* support profiles */

#define BT_PROFILE_SUPPORTED_A2DP				0x0001

#define BT_PROFILE_SUPPORTED_AVRCP				0x0002

#define BT_PROFILE_SUPPORTED_HFP				0x0004

#define BT_PROFILE_SUPPORTED_MFI				0x0008

#define BT_PROFILE_SUPPORTED_SPP				0x0010

#define BT_PROFILE_SUPPORTED_OPP				0x0020

#define BT_PROFILE_SUPPORTED_HID				0x0040


/**
* Stack Debug Info mask
*/
typedef uint32_t			BtStackDbgInfo;

#define BT_STATCK_DEBUG_INFO_COMMON				(1 << 0)
#define BT_STATCK_DEBUG_INFO_HFP				(1 << 1)
#define BT_STATCK_DEBUG_INFO_A2DP				(1 << 2)
#define BT_STATCK_DEBUG_INFO_AVRCP				(1 << 3)
#define BT_STATCK_DEBUG_INFO_SPP				(1 << 4)
#define BT_STATCK_DEBUG_INFO_MFI				(1 << 5)
#define BT_STATCK_DEBUG_INFO_HID				(1 << 6)

/**
 * Stack Debug infomations output
 */
void SetBtStackDebugInfo(BtStackDbgInfo mask);

/** 
 * Max length of local bluetooth device name
 * DON'T modify this value!!!
 */
#define DEVICE_NAME_MAX_LEN				50

typedef struct _BtStackParams
{
	uint8_t				localDevName[DEVICE_NAME_MAX_LEN];
	uint32_t			supportProfiles;
	BTStackCallbackFunc	callback;

	/*HFP releated features*/
	HfpAppFeatures		hfpFeatures;
	
	/*A2DP releated features*/
	A2dpAppFeatures		a2dpFeatures;

	/*AVRCP releated features*/
	AvrcpAppFeatures	avrcpFeatures;
}BtStackParams;

/*
* Bt stack init
*/
int32_t BTStackRunInit(BtStackParams *stackParams);

/*
*
*/
void  BTStackRun(void);

/*
*
*/
void  BTStackRunUninit(void);

/*
*
*/
bool HasBtDataToProccess(void);

/*
*
*/
const unsigned char * GetLibVersionBt(void);


/*
*
*/
bool GetLocalBtAddr(uint8_t * bt_addr);

/*
*
*/
bool GetLocalBtName(uint8_t * bt_name, uint8_t * bt_name_len);


/*
*
*/
typedef uint8_t (*BTStackRecvHookFunc) (uint8_t* data, uint32_t data_len);
void BTStackSetReceivedDataHook(BTStackRecvHookFunc HookFunc);


typedef void (*BTStackSendHookFunc)(void);
void BTStackSetSendDataHook(BTStackSendHookFunc HookFunc);



#endif

