/**
 *******************************************************************************
 * @file    bt_control_api.h
 * @author  Halley
 * @version V1.0.0
 * @date    26-June-2012
 * @brief   bt stack callback releated functions
 * @maintainer Halley
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

/**
* @addtogroup А¶СА
* @{
* @defgroup bt_control_api bt_control_api
* @{
*/ 

#ifndef __BT_CONTROL_API_H__
#define __BT_CONTROL_API_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * @brief
 *		A command to play song
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *		0 means failed
 * @note
 *
 */
uint8_t BTPlayCtrlPlay(void);

/**
 * @brief
 *		A command to pause song
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *		0 means failed
 * @note
 *
 */
uint8_t BTPlayCtrlPause(void);

/**
 * @brief
 *		A command to play next song
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *		0 means failed
 * @note
 *
 */
uint8_t BTPlayCtrlNextSong(void);

/**
 * @brief
 *		A command to play previous song
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *		0 means failed
 * @note
 *
 */
uint8_t BTPlayCtrlPreSong(void);

/**
 * @brief
 *		A command to volume up
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *		0 means failed
 * @note
 *
 */
uint8_t BTPlayCtrlVolUp(void);

/**
 * @brief
 *		A command to volume down
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *		0 means failed
 * @note
 *
 */
uint8_t BTPlayCtrlVolDown(void);

/**
 * @brief
 *		A command to set absolute volume
 *
 * @param volPercent	The value of volume in percent style
 *						Value range is (0 ~ 100)
 *
 * @return
 *		1 means successful
 *		0 means failed
 * @note
 *		
 */
uint8_t BTPlayCtrlSetAbsVol(uint8_t volPercent);


/**
 * @brief
 *		A command to stop
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *		0 means failed
 * @note
 *
 */
uint8_t BTPlayCtrlStop(void);

/**
 * @brief
 *		A command to start fast foward 
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *		0 means failed
 * @note
 *
 */
uint8_t BTPlayCtrlFastForward(void);

/**
 * @brief
 *		A command to start back foward 
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *		0 means failed
 * @note
 *
 */
uint8_t BTPlayCtrlFastBackward(void);

/**
 * @brief
 *		A command to end fast/back foward 
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *		0 means failed
 * @note
 *
 */uint8_t BTPlayCtrlEndFastBackForward(void);


/**
 * @brief
 *		Get current call number 
 *
 * @param NONE
 *
 * @return
 *		the number of current call
 * @note
 *
 */
uint8_t* BTGetCurInCallPhoneNum(void);

/**
 * @brief
 *		Accept incoming call
 *
 * @param NONE
 *
 * @return
 *		NONE
 * @note
 *
 */
void BTInCallAccept(void);

/*---------------------------------------------------------------------------
 * HfHoldAction type
 *
 * HfHoldAction enumerates the possible actions that can be taken when 
 * calling the HF_CallHold() function.
 */
typedef uint8_t HfHoldAction;

/** Indicates that the code should release all held calls, or set the User Determined User Busy (UDUB) indication for a waiting call.  
 */
#define HF_HOLD_RELEASE_HELD_CALLS   0

/** Indicates that the code should release all active calls (if any exist) and accepts the other (held or waiting) call.  
 *
 *  If a call index is specified, the code should release the specific call.  
 */
#define HF_HOLD_RELEASE_ACTIVE_CALLS 1

/** Indicates that the code should place all active calls (if any exist) on hold and accepts the other (held or waiting) call.  
 *
 *  If a call index is specified, the code should put all calls on hold except the specified call.  
 */
#define HF_HOLD_HOLD_ACTIVE_CALLS    2

/** Indicates that the code should add a held call to the conversation.  
 */
#define HF_HOLD_ADD_HELD_CALL        3

/** Indicates that the code should connects the two calls and disconnect the Audio Gateway from both calls.  
 *  In other words, the code should perform an Explicit Call Transfer.  
 */
#define HF_HOLD_CALL_TRANSFER        4
/* End of HfHoldAction */ 

/**
 * @brief
 *		call hold and multiparty handling (three-way call)
 *
 * @param 
 *		HfHoldAction
 *
 * @return
 *		NONE
 * @note
 *
 */
void BTCallHold(int param);

/**
 * @brief
 *		Reject incoming call
 *
 * @param NONE
 *
 * @return
 *		NONE
 * @note
 *
 */
void BTInCallReject(void);

/**
 * @brief
 *		Hang up an active call
 *
 * @param NONE
 *
 * @return
 *		NONE
 * @note
 *
 */
void BTCallHangUp(void);

/**
 * @brief
 *		Disable AEC on AG
 *
 * @param NONE
 *
 * @return
 *		NONE
 * @note
 *
 */
void BTCallDisableAEC(void);

/**
 * @brief
 *		Calling one phone number
 *
 * @param PhoneNumString It's NULL ended string and maximum length is 19 bytes
 *
 * @return
 *		NONE
 * @note
 *
 */
void BTDialOnePhoneNumber(uint8_t *PhoneNumString);

/**
 * @brief
 *		Redial last call
 *
 * @param NONE
 *
 * @return
 *		NONE
 * @note
 *
 */
void BTRedialLastCalledNum(void);

/**
 * @brief
 *		Synchronize speaker volume between phone and hf(bt music device) when phone call is on the way. 
 *
 * @param Vol The value of volume to be set
 *
 * @return
 *		1 means successful
 *
 * @note
 *		Phones(AG) just display picture, and HF local MUST do the volume change's implementation.
 */
uint8_t BTPhoneCallVolSyncSet(uint8_t Vol);

/**
 * @brief
 *		Get the sync remote(AG) speaker volume value [0-15] for HF mode
 *
 * @param NONE
 *
 * @return
 *		The value of volume
 *
 * @note
 *		
 */
uint8_t BtGetRemoteSpeakerVol(void);

/**
 * @brief
 *		Get current connected remote BT device's address pointer, NULL means NO bt device connected
 *
 * @param NONE
 *
 * @return
 *		The address of remote BT device
 *
 * @note
 *		
 */
uint8_t* GetCurConnectedRemDevAdd(void);

/**
 * @brief
 *		Disconnect connected BT device
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *
 * @note
 *		
 */
uint8_t BTDisConnect(void);

/**
 * @brief
 *		Connect remote BT device with given address
 *
 * @param addr The address of the remote BT device
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void BTConnect(uint8_t* addr);

/**
 * @brief
 *		Disconnect A2dp and HF, SPP may keep connected status.
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *
 * @note
 *		
 */
uint8_t BTDisConnectA2dpHf(void);

/**
 * @brief
 *		Disconnect A2dp .
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *
 * @note
 *		
 */
uint8_t BTDisConnectA2dp(void);

/**
 * @brief
 *		Disconnect HF .
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *
 * @note
 *		
 */
uint8_t BTDisConnectHf(void);

/**
 * @brief
 *		Disconnect SPP .
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *
 * @note
 *		
 */
uint8_t BTDisConnectSpp(void);

/**
 * @brief
 *		Set scan and page enabled when the device is in idle mode.
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *
 * @note
 *		If the device is in the connected status, this function returns 0.
 *		one may invoke GetCurBtConnectedFlag() to get the connected status.
 */
uint8_t BTSetScanEnable(void);

/**
 * @brief
 *		Set scan and page disabled when the device is in idle mode.
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *
 * @note
 *		If the device is in the connected status, this function returns 0.
 *		after invoking this function, the device will NOT be found or connected.
 *		one may invoke GetCurBtConnectedFlag() to get the connected status.
 */
uint8_t BTSetScanDisable(void);


/**
 * Bt access mode
 */
typedef uint8_t BtAccessMode;

#define BtAccessModeNone			0x00 /*!< Non-discoverable or connectable */
#define BtAccessModeGeneral			0x03 /*!< General discoverable and connectable */
#define BtAccessModeConnectOnly		0x02 /*!< Connectable but not discoverable */
#define BtAccessModeDiscoverOnly	0x01 /*!< Discoverable but not connectable */


/**
 * @brief
 *		Set local bluetooth accessible mode when there is no any connection.
 *		If the device is in the connected status, the accessbile mode will
 *		be changed when the connection down.
 *
 * @param mode 
 *		Accessible mode
 *
 * @return
 *		1 means successful
 *
 * @note
 *		
 */

uint8_t BTSetAccessMode(BtAccessMode mode);



/**
 * @brief
 *		Disable BLE event.Some 2.1 chip doesn't support BLE event, one should disable BLE event
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This function should be called before BT stack initialized
 */
void BTDisableBLEEvent(void);


/**
 * @brief
 *		Get current connected flag. If BT device is connected, then this function returns 1. 
 *
 * @param NONE
 *
 * @return
 *		1 means remote BT devices has been connected
 *
 * @note
 *		
 */
uint8_t GetCurBtConnectedFlag(void);

/**
 * @brief
 *		Set apple battery status
 *
 * @param BatteryLevel 0-9
 * @param DockingState 0-1
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void BTSetBatteryState(uint8_t BatteryLevel, uint8_t DockingState);

/**
 * @brief
 *		Disable apple battery status reporting function
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void BtDisableBatteryState(void);

/**
 * @brief
 *		Get remote bt device(history) name in format of UTF8 with double-NULL ended.
 *
 * @param NONE
 *
 * @return
 *		The name of the remote device
 *
 * @note
 *		Zero length means not found, it may refresh at any time of A2DP or HF connected
 */
uint8_t* BTGetRemoteDevName(void);

/**
 * @brief
 *		Set the local BT device to inquiry mode
 *
 * @param MaxScanTimeInSecond	The maximum time to scan
 *
 * @return
 *		1 means successful
 *
 * @note
 *		This function only can be called in test mode
 */
uint8_t BTDoDeviceInquiry(uint8_t MaxScanTimeInSecond);

/**
 * @brief
 *		Get the name of BT device
 *
 * @param BdAddrLE	The address of the BT device
 *
 * @return
 *		1 means successful
 *
 * @note
 *		bd addr 6Bytes in LE format(normal format)
 */
uint8_t BTGetDeviceName(const uint8_t* BdAddrLE);

/**
 * @brief
 *		Send one HF DTMF character
 *
 * @param HfDtmfNum	The DTMF number
 *
 * @return
 *		1 means successful
 *
 * @note
 */
uint8_t BTHfDtmf(unsigned char HfDtmfNum); //send one HF DTMF character, return 1 means OK.

/**
 * @brief
 *		Transfer hf audio between speaker and phone
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *	
 */
void BTStartHfTransfer(void);

//test function
//void BTStartOneHfConnection(void);

//test fucntion
//void BTBqbAvrcpConnect(void);


/**
 * @brief
 *		Get factory name
 *
 * @param NONE
 *
 * @return
 *		the factory name
 * @note
 *
 */
unsigned char * BTHfGetFactoryName(void);


/**
 * @brief
 *		Get phone type
 *
 * @param NONE
 *
 * @return
 *		the phone type
 * @note
 *
 */
unsigned char * BTHfGetPhoneType(void);

/**
 * @brief
 *		Bluetooth Voice Recognition Activation
 *		Enables/Disables the voice recognition function in the AG
 *
 * @param 
 *		0=Disable Voice Recognition in the AG
 *		1=Enable Voice Recognition in the AG
 *
 * @return
 *		NONE
 *
 * @note
 *	
 */
void BTCallVoiceReg(bool enable);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
