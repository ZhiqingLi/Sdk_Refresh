///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: bt_app_func.h
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////

#ifndef __BT_APP_FUNC_H__
#define __BT_APP_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

#define BT_CONNECTED_NONE_FLAG		(0)
#define BT_CONNECTED_A2DP_FLAG		(1)
#define BT_CONNECTED_HF_FLAG		(1 << 1)
#define BT_CONNECTED_SPP_FLAG		(1 << 2)
#define BT_CONNECTED_AVRCP_FLAG		(1 << 3)

#ifdef FAST_POWER_ON_OFF_BT
#define BT_SCAN_PAGE_ENABLE			1
#define BT_SCAN_PAGE_DISABLE		0
#define BT_SCAN_PAGE_UNKNOW			-1
#endif

enum
{
	BT_ACTIVE_NONE = 0,
	BT_ACTIVE_HFP,
	BT_ACTIVE_A2DP
};

enum
{
	BT_HFP_STATE_NONE = 0,
	BT_HFP_STATE_CONNECTING,
	BT_HFP_STATE_CONNECTED,
	BT_HFP_STATE_INCOMING_CALL,
	BT_HFP_STATE_OUTGOING_CALL,
	BT_HFP_STATE_ATCTIVE_CALL,
	BT_HFP_STATE_3WAY_INCOMING_CALL,		//1CALL ACTIVE, 1CALL INCOMING
	BT_HFP_STATE_3WAY_OUTGOING_CALL,		//1CALL ACTIVE, 1CALL OUTGOING
	BT_HFP_STATE_3WAY_ATCTIVE_CALL			//2CALL ACTIVE
};

enum
{
	BT_A2DP_STATE_NONE = 0,
	BT_A2DP_STATE_CONNECTING,
	BT_A2DP_STATE_CONNECTED,
	BT_A2DP_STATE_STREAMING
};

enum
{
	BT_AVRCP_STATE_NONE = 0,
	BT_AVRCP_STATE_CONNECTING,
	BT_AVRCP_STATE_CONNECTED
};

enum
{
	BT_MODE_NONE_STATUS = 0,
	BT_MODE_ACTIVE_STATUS,
	BT_MODE_SNIFF_STATUS
};

enum
{
	BT_HF_MODE_STATE_NONE = 0,
	BT_HF_MODE_STATE_ENTERING,
	BT_HF_MODE_STATE_ACTIVE,
	BT_HF_MODE_STATE_EXITING
};

enum
{
	BT_SOUND_TIP_CONNECTED,
	BT_SOUND_TIP_DISCONNECTED,
	BT_SOUND_TIP_PHONE_CALL_INCOMING
};

#ifdef BT_HF_RECORD_FUNC
enum
{
	BT_RECORD_STATE_NONE,
	BT_RECORD_STATE_INITIALIZING,
	BT_RECORD_STATE_RECORDING,
	BT_RECORD_STATE_PAUSE,
	BT_RECORD_STATE_EXITING
};
#endif

#ifdef FAST_POWER_ON_OFF_BT
enum
{
	BT_SCAN_PAGE_STATE_NONE = 0,
	BT_SCAN_PAGE_STATE_CLOSING,
	BT_SCAN_PAGE_STATE_DISCONNECTING,
	BT_SCAN_PAGE_STATE_DISABLE,
	BT_SCAN_PAGE_STATE_OPENING,
	BT_SCAN_PAGE_STATE_ENABLE
};
#endif

#if 1//def BT_RF_POWER_ON_OFF
enum
{
	BT_HW_POWER_CMD_NONE,
	BT_HW_POWER_CMD_POWER_ON,
	BT_HW_POWER_CMD_POWER_OFF,
	BT_HW_POWER_CMD_POWER_OFF_PENDING
};
#endif

typedef enum _SBC_DATA_STATE{
	SBC_DATA_STATE_NONE = 0,
	SBC_DATA_STATE_NEED_MORE,
	SBC_DATA_READY
} SBC_DATA_STATE;

typedef struct
{
//	uint8_t			active_profile;
	
	uint8_t			hfp_state;
	uint8_t			a2dp_state;
	uint8_t			avrcp_state;
	uint8_t			mode_status;
	
	uint32_t		connected_profile;
	bool			sco_conneted_flag;
	uint8_t			hf_mode_state;
#ifdef BT_HF_RECORD_FUNC
	uint8_t			bt_record_state;
#endif
#ifdef BT_ADVANCED_AVRCP_SUPPORT
	bool			adv_avrcp_support_flag;
	uint8_t			adv_avrcp_play_status;
#endif
} BT_CONTEXT_DATA;

/**
 * @brief
 *  	Set BT task status
 *
 * @param Status BT task status. TRUE for start, FALSE for stop
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void BtSetLinkStatus(bool Status);

/**
 * @brief
 *  	Check whether BT task is start
 *
 * @param NONE
 *
 * @return
 *		TRUE for start, FALSE for stop
 *
 * @note
 *		
 */
bool IsBtLink(void);

/**
 * @brief
 *  	Init BT app context
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void InitBtApp(void);

/**
 * @brief
 *  	Set BT mode status. Sniff or Active
 *
 * @param status BT_MODE_SNIFF_STATUS or BT_MODE_ACTIVE_STATUS
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void SetBtModeStatus(uint8_t status);

/**
 * @brief
 *  	Get BT mode status.
 *
 * @param NONE
 *
 * @return
 *		BT_MODE_SNIFF_STATUS or BT_MODE_ACTIVE_STATUS
 *
 * @note
 *		
 */
uint8_t GetBtModeStatus(void);

/**
 * @brief
 *  	Set HFP state
 *
 * @param hfp_state BT_HFP_STATE_XXX
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void SetHfpState(uint8_t hfp_state);

/**
 * @brief
 *  	Get HFP state
 *
 * @param NONE
 *
 * @return
 *		BT_HFP_STATE_XXX
 *
 * @note
 *		
 */
uint8_t GetHfpState(void);

/**
 * @brief
 *  	Set A2DP state
 *
 * @param a2dp_state BT_A2DP_STATE_XXX
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void SetA2dpState(uint8_t a2dp_state);

/**
 * @brief
 *  	Get A2DP state
 *
 * @param NONE
 *
 * @return
 *		BT_A2DP_STATE_XXX
 *
 * @note
 *		
 */
uint8_t GetA2dpState(void);

/**
 * @brief
 *  	Set AVRCP state
 *
 * @param avrcp_state BT_AVRCP_STATE_XXX
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void SetAvrcpState(uint8_t avrcp_state);

/**
 * @brief
 *  	Get AVRCP state
 *
 * @param NONE
 *
 * @return
 *		BT_AVRCP_STATE_XXX
 *
 * @note
 *		
 */
uint8_t GetAvrcpState(void);

#ifdef BT_ADVANCED_AVRCP_SUPPORT

/**
 * @brief
 *  	Set Advanced AVRCP support flag
 *
 * @param flag TRUE means support advanced avrcp
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void SetAdvAvrcpSupportFlag(bool flag);

/**
 * @brief
 *  	Check whether device support advanced AVRCP
 *
 * @param NONE
 *
 * @return
 *		TRUE means BT device support advanced AVRCP
 *
 * @note
 *		
 */
bool IsSupportAdvAvrcp(void);

/**
 * @brief
 *  	Set advanced AVRCP play status ( See Current 
 *		status of play state in bt_app_ui_callback.h)
 *
 * @param status play status
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void SetAdvAvrcpPlayStatus(uint8_t status);

/**
 * @brief
 *  	Get advanced AVRCP play status ( See Current 
 *		status of play state in bt_app_ui_callback.h)
 *
 * @param NONE
 *
 * @return
 *		Play status
 *
 * @note
 *		
 */
uint8_t GetAdvAvrcpPlayStatus(void);
#endif


/**
 * @brief
 *  	Set HF mode state
 *
 * @param state BT_HF_MODE_XXX
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void SetHfModeState(uint8_t state);

/**
 * @brief
 *  	Get HF mode state
 *
 * @param NONE
 *
 * @return
 *		BT_HF_MODE_XXX
 *
 * @note
 *		
 */
uint8_t GetHfModeState(void);

/**
 * @brief
 *  	It should be called when one profile has been connected
 *
 * @param profile BT_CONNECTED_XXXX_FLAG
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void ConnectedProfile(uint32_t profile);

/**
 * @brief
 *  	Get the number of connected profile
 *
 * @param NONE
 *
 * @return
 *		The number of connected profile
 *
 * @note
 *		
 */
uint8_t GetNumberOfConnectedProfile(void);

/**
 * @brief
 *  	Disconnect one profile.
 *
 * @param profile BT_CONNECTED_XXXX_FLAG
 *
 * @return
 *		NONE
 *
 * @note
 *		This function only remove the connected 
 *		profile's flag in app context
 */
void DisconnectProfile(uint32_t profile);

/**
 * @brief
 *  	Remove all flag in app context.
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void DisconnectAllProfile(void);

/**
 * @brief
 *  	Check whether A2DP is connected
 *
 * @param NONE
 *
 * @return
 *		TRUE for connected
 *		FALSE for connectless
 *
 * @note
 *		
 */
bool IsA2dpConnected(void);

/**
 * @brief
 *  	Check whether HFP is connected
 *
 * @param NONE
 *
 * @return
 *		TRUE for connected
 *		FALSE for connectless
 *
 * @note
 *		
 */
bool IsHfpConnected(void);

/**
 * @brief
 *  	Check whether AVRCP is connected
 *
 * @param NONE
 *
 * @return
 *		TRUE for connected
 *		FALSE for connectless
 *
 * @note
 *		
 */
bool IsAvrcpConnected(void);

/**
 * @brief
 *  	Check whether SPP is connected
 *
 * @param NONE
 *
 * @return
 *		TRUE for connected
 *		FALSE for connectless
 *
 * @note
 *		
 */
bool IsSppConnected(void);

/**
 * @brief
 *  	Set SCO connect flag
 *
 * @param flag TRUE for SCO is connected
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void SetScoConnectFlag(bool flag);

/**
 * @brief
 *  	Check whether SCO is connected
 *
 * @param NONE
 *
 * @return
 *		TRUE means SCO is connected
 *
 * @note
 *		
 */
bool IsScoConneted(void);

#ifdef BT_HF_RECORD_FUNC
/**
 * @brief
 *  	Start record a HF call voice
 *
 * @param NONE
 *
 * @return
 *		TRUE means recording HF start successful
 *
 * @note
 *		
 */
bool BtHfRecordStart(void);

/**
 * @brief
 *  	Stop record a HF call voice
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void BtHfRecordExit(void);

/**
 * @brief
 *  	Set HF record state
 *
 * @param state BT_RECORD_STATE_XXXX
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void SetBtRecordState(uint8_t state);

/**
 * @brief
 *  	Get HF record state
 *
 * @param NONE
 *
 * @return
 *		BT_RECORD_STATE_XXXX
 *
 * @note
 *		
 */
uint8_t GetBtRecordState(void);
#endif



/**
 * @brief
 *  	Start a sound tip
 *
 * @param sound_tip The sound tip id
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void BtSoundTip(uint8_t sound_tip);

/**
 * @brief
 *  	LOCK the semaphore 
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		Because the HF mode can be enter from
 *		any mode when a call incoming. So we
 *		should lock the HF mode semaphore when
 *		switch HF mode
 */
void LockHfMode(void);

/**
 * @brief
 *  	UNLOCK the semaphore 
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void UnLockHfMode(void);

/**
 * @brief
 *  	Start to the HF mode
 *
 * @param NONE
 *
 * @return
 *		TRUE means the task is switching to
 *		entry HF mode
 *
 * @note
 *		
 */
bool EnterHfMode(void);

/**
 * @brief
 *  	Exit from the HF mode
 *
 * @param NONE
 *
 * @return
 *		TRUE means the task is switching to
 *		exiting HF mode
 *
 * @note
 *		
 */
bool ExitHfMode(void);

/**
 * @brief
 *  	Get the history mode when entering HF mode,
 *		so that We can recover the previous mode when
 *		exit from HF mode
 *
 * @param NONE
 *
 * @return
 *		History mode id
 *
 * @note
 *		
 */
uint32_t GetHistroyModeId(void);

/**
 * @brief
 *		NO USE in this version
 *
 * @param NONE
 *
 * @return
 *		bool
 *
 * @note
 *		
 */
bool IsHfModeEvnOK(void);

/*
* phone number reminder
*/
#ifdef BT_HF_NUMBER_REMIND
#define END_OF_PHONE_NUM					0xFF

/**
 * @brief
 *		Start remind phone numbers
 *
 * @param phone_numbers The phone numbers to be reminded
 * @param phone_numbers_len The length of phone numbers to be reminded
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void RemindPhoneNumbers(uint8_t * phone_numbers, uint8_t phone_numbers_len);

/**
 * @brief
 *		Get the next number of the phone numbers
 *
 * @param NONE
 *
 * @return
 *		The number will be reminded
 *
 * @note
 *		
 */
uint8_t GetNextNumber(void);

/**
 * @brief
 *		Remind a number. It will start a sound tip.
 *
 * @param number The number to be reminded
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void RemindNumber(uint8_t number);

/**
 * @brief
 *		Stop remind. 
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		It will stop the reminding.
 */
void StopRemindingPhoneNumbers(void);

/**
 * @brief
 *		Check the reminding whether in process
 *
 * @param NONE
 *
 * @return
 *		TRUE means reminding
 *
 * @note
 *		
 */
bool IsRemindingPhoneNumbers(void);
#endif


/**
 * @brief
 *		Initialize a timer for BT call display
 *
 * @param NONE
 *
 * @return
 *		TRUE means successful
 *
 * @note
 *		
 */
bool InitBtHfTimer(void);

/**
 * @brief
 *		Start the timer for BT call display
 *
 * @param NONE
 *
 * @return
 *		TRUE means successful
 *
 * @note
 *		
 */
bool StartBtHfTimer(void);

/**
 * @brief
 *		Check the timer
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void CheckBtHfTimer(void);

/**
 * @brief
 *		Deinit the timer for BT call display
 *
 * @param NONE
 *
 * @return
 *		TRUE means successful
 *
 * @note
 *		
 */
bool DeinitBtHfTimer(void);

/**
 * @brief
 *		Check the timer whether initialized
 *
 * @param NONE
 *
 * @return
 *		TRUE means the timer has been initialized
 *
 * @note
 *		
 */
bool IsBtHfTimerInited(void);

/**
 * @brief
 *		Set the volume of HF call
 *
 * @param vol The value should be between 0 and 15
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void SetBtHfVolume(uint8_t vol);

/**
 * @brief
 *		Get the volume of HF call
 *
 * @param NONE
 *
 * @return
 *		The value should be between 0 and 15
 *
 * @note
 *		
 */
uint8_t GetBtHfVolume(void);

/**
 * @brief
 *		Set LED on or off
 *
 * @param OnOffFlag On or off
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void SetBtLedBlue(bool OnOffFlag); //tx,blue
/**
 * @brief
 *		Set LED on or off
 *
 * @param OnOffFlag On or off
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void SetBtLedRed(bool OnOffFlag);

/**
 * @brief
 *		Start flashing LED 
 *
 * @param NONE
 *
 * @return
 *		TRUE means successful
 *
 * @note
 *		
 */
bool StartBtLed(void);

/**
 * @brief
 *		Stop flashing LED 
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void StopBtLed(void);


// bt Name & Address
#define MAX_BT_DEV_NAME_LENGTH   50 /*DO NOT Modify this value*/
extern int8_t BtAddr[6];
extern uint8_t BtDevName[MAX_BT_DEV_NAME_LENGTH];
extern SW_TIMER 		BtLedTimer;

extern bool power_on_reconnection_flag;
extern bool BtReConnect(void);
    
/**
 * @brief
 *		Get the name of local bt device
 *
 * @param Name The name of local bt device
 * @param NameMaxLen The lenght of the name
 *
 * @return
 *		NONE
 *
 * @note
 *		The name will be used in BT initializing
 */
void GetBtName(uint8_t* Name, uint32_t NameMaxLen);

/**
 * @brief
 *		Get the bt address of local bt device
 *
 * @param BtAddr The address of local bt device
 *
 * @return
 *		NONE
 *
 * @note
 *		The address will be used in BT initializing
 */
void GetBtAddr(int8_t* BtAddr);

/**
 * @brief
 *		Get the bt address of local bt device
 *
 * @param NONE
 *
 * @return
 *		The address of local bt device
 *
 * @note
 *		The address will be used in BT test mode
 */
uint8_t* GetMassTestDevAddr(void);


/**
 * @brief
 *		Set the flag of audio play start
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		NONE
 */
void BTAudioPlayStart(void);

/**
 * @brief
 *		Set the flag of audio play stop
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		NONE
 */
void BTAudioPlayStop(void);

/**
 * @brief
 *		Get the flag of audio play
 *
 * @param NONE
 *
 * @return
 *		TRUE means in BT audio playing
 *
 * @note
 *		NONE
 */
bool IsBtAudioPlay(void);

// reconnection
#ifdef BT_RECONNECTION_FUNC
/**
 * @brief
 *		Start a reconnection to last bt device and 
 *		set the number of tries with "reconnection_count".
 *
 * @param reconnection_count	The maximum count of trying reconnection
 * @param internal_time	The internal time between two connecting
 *
 * @return
 *		NONE
 *
 * @note
 *		NONE
 */
void StartBtReConnection(uint8_t reconnection_count, uint8_t internal_time);

/**
 * @brief
 *		Stop the reconnection process
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		NONE
 */
void StopBtReConnection(void);


/**
 * @brief
 *		Check timer
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This function shuold be called in a loop
 */
void BtReConnectDetect(void);

/**
 * @brief
 *		Check whether reconnection is finished 
 *
 * @param NONE
 *
 * @return
 *		TRUE means reconncetion has been finished
 *
 * @note
 */
bool IsBtReConnectionFinish(void);

#endif

#ifdef FAST_POWER_ON_OFF_BT

void FastDisableBT(void);

void FastEnableBT(void);
/**
* Get the BT scan & page status
*
* Returns :
*	BT_SCAN_PAGE_ENABLE - means BT is discoverable & connectable
*	BT_SCAN_PAGE_DISABLE - means BT can not be discovered & can not be connected
*	BT_SCAN_PAGE_UNKNOW - means BT is in a middle statement
*
* Comments:
*	When it returns BT_SCAN_PAGE_UNKNOW, it means the BT is in the process swithing 
*	mode from enable to disable or in opposite process. When get the value 
*	BT_SCAN_PAGE_UNKNOW, one should check this value again.
*/
int8_t IsBTScanPageEnable(void);

#endif

#if 1//def BT_RF_POWER_ON_OFF

/**
 * @brief
 *		Set the flag to BT hardware power on
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		It will be start the bt stack
 */
void BtHwPowerOn(void);

/**
 * @brief
 *		Set the flag to BT hardware power off
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		It will be stop the bt stack
 */
void BtHwPowerOff(void);

/**
 * @brief
 *		Set the command state of BT
 *
 * @param cmd_state BT_HW_POWER_CMD_XXX
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void SetBtHwPowerCmdState(uint8_t cmd_state);

/**
 * @brief
 *		Get the command state of BT
 *
 * @param NONE
 *
 * @return
 *		BT_HW_POWER_CMD_XXX
 *
 * @note
 *		
 */
uint8_t GetBtHwPowerCmdState(void);

/**
 * @brief
 *		Power pin of user defined.
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void UserBtRfPowerPlugIn(void);

/**
 * @brief
 *		Power pin of user defined
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void UserBtRfPowerPlugOut(void);
#endif

/**
 * @brief
 *		For BT test hook. No use in actual project
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void SetBtTestHook(void);

/**
 * @brief
 *		Check scan & page state
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void CheckScanPageFunc(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif/*__BT_APP_FUNC_H__*/

