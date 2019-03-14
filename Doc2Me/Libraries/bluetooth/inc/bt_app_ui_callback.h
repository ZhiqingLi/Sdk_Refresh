/**
 *******************************************************************************
 * @file    bt_app_ui_callback.h
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
* @defgroup h_bt_app_ui_callback h_bt_app_ui_callback
* @{
*/

#ifndef __H_BT_APP_UI_CALLBACK_H__
#define __H_BT_APP_UI_CALLBACK_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * Advanced AVRCP releated values 
 */
#define ADV_AVRCP_ATTRIBUTE_MASK_SHUFFLE 0x04
#define ADV_AVRCP_ATTRIBUTE_MASK_REPEAT  0x02
#define ADV_AVRCP_ATTRIBUTE_MASK_EQ      0x01

#define ADV_AVRCP_ATTRIBUTE_ID_EQ        0x01
#define ADV_AVRCP_ATTRIBUTE_ID_REPEAT    0x02
#define ADV_AVRCP_ATTRIBUTE_ID_SHUFFLE   0x03
#define ADV_AVRCP_ATTRIBUTE_ID_SCAN      0x04


/**
 * BT stack callback events
 */
enum
{
    BT_UI_CB_STATUS_ERROR = 0,                        /**< BT stack unknown error */
    BT_UI_CB_STATUS_OUT_OF_MEM_ERROR,                 /**< BT stack out of memory error */
    BT_UI_CB_STATUS_IDLE,                             /**< BT stack idle state. Stack can be paired and connected */
    BT_UI_CB_STATUS_PAIRED,                           /**< BT device has been paired or unpaired with remote devices. */
    BT_UI_CB_STATUS_A2DP_CONNECTED,                   /**< A2DP has been connected. */
    BT_UI_CB_STATUS_PLAY_CONTROL_CONNECTED,           /**< AVRCP has been connected. */
    BT_UI_CB_STATUS_DISCONNECTED,                     /**< BT device has been disconnected A2DP and AVRCP with remote devices. */
    BT_UI_CB_STATUS_BUART_RECV_ERR,                   /**< BT receive error data. HCI chip might have some problem. */
    BT_UI_CB_STATUS_AUTHENTICATE_FAILURE,             /**< Authentication failed. */

    BT_UI_CB_STATUS_HID_OUT_DATA_IND = 0x30,          /**< HID host data out event, one may call function BtHidGetOutData() */
    
//followings are advanced avrcp events
    BT_UI_CB_STATUS_ADV_AVRCP_SUPPORTED = 0x40,       /**< The remote device supports advanced AVRCP*/
    BT_UI_CB_STATUS_ADV_AVRCP_NOT_SUPPORTED,					/**< The remote device does not support advanced AVRCP*/
    BT_UI_CB_STATUS_PLAYING_STATUS,                   /**< play status reported, call function [AdvAvrcpPlayStatus*]BtGetAdvAvrcpResponse()*/
    BT_UI_CB_STATUS_TRACK_INFO,                       /**< track info reported, call function [AdvAvrcpTrackInfo*]BtGetAdvAvrcpResponse()*/
    BT_UI_CB_STATUS_AVAILABLE_ATTRIBUTE_TYPES,        /**< source bt supported attribute types, such as report mode, shuffle mode, .... 
    																											call function [ADV_AVRCP_ATTRIBUTE_MASK_XXX]BtGetAdvAvrcpResponse()*/
    BT_UI_CB_STATUS_AVAILABLE_SETTINGS_FOR_ONE_ATTR,  /**< supported settings for one attribute,such as repeat all, shuffle on/off
    																											call function [void*]BtGetAdvAvrcpResponse(),
    																											for inquiry shuffle, then returns [_AdvAvrcpSupportedSettings*] one may judge based on the pre-send cmd.
    																											current used setting, call function [AdvAvrcpCurSetting*]BtGetAdvAvrcpResponse() */
    BT_UI_CB_STATUS_CUR_SETTING_FOR_ONE_ATTR,         /**< current settings for one attribute,such as repeat all, shuffle on/off */
    
	BT_UI_CB_STATUS_ADV_ABSOLUTE_VOLUME,

    BT_UI_CB_STATUS_HID_CONNECTED = 0x78,             /**< hid interrupt channel is connected(NO event for reporting control channle connected status)*/
    BT_UI_CB_STATUS_HID_DISCONNECTED = 0x79,          /**< hid interrupt or control channle is disconnected*/


    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_PLAY = 0x81,/**< The remote device does not support Play func */
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_PAUSE,      /**< The remote device does not support Pause func */
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_STOP,       /**< The remote device does not support Stop func */
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_VOL_UP,     /**< The remote device does not support Volume Up func */
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_VOL_DOWN,   /**< The remote device does not support Volume Down func */
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_PRE_SONG,   /**< The remote device does not support Pre-song func */
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_NEXT_SONG,  /**< The remote device does not support Next-song func */
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_FAST_FORWARD,/**< The remote device does not support FF func */
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_FAST_BACKWARD,/**< The remote device does not support FB func */

/**
 * folowings are the messages for Phonebook on handfree channel(AT cmd), so if HF not suported then phonebook also disabled.
 */
    BT_UI_CB_STATUS_PB_SUPPORTED = 0xA0,   /**< The remote device supports phonebook feature */
    BT_UI_CB_STATUS_PB_ENTRY_SIZE,         /**< Selected phone book's total entry size info(total record number) */
    BT_UI_CB_STATUS_PB_ONE_ENTRY_DATA,     /**< Selected phone book's record data received */
    
    BT_UI_CB_STATUS_CLCC_RESULT,           /**< Current call list number data received */

    BT_UI_CB_STATUS_HF_COMMAND_END = 0xDF, /**< HF command runs end(OK), including PB commands */

/**
 * followings are the msg for Handfree(mono speech headphone) ONLY
 */
    BT_UI_CB_STATUS_HF_CONNECTED = 0xE0,   /**< Handsfree Profile connected */
    BT_UI_CB_STATUS_HF_DISCONNECTED,       /**< Handsfree Profile disconnected */

    BT_UI_CB_STATUS_HF_CALL_SETUP_NONE,     /**< HFP call setup none event*/
    BT_UI_CB_STATUS_HF_CALL_SETUP_INCOMING,      /**< A incoming call event */
    BT_UI_CB_STATUS_HF_CALL_SETUP_OUTCALL_ALERT, /**< A outgoing call alerting on AG */

    BT_UI_CB_STATUS_HF_INCOME_RING,        /**< A incoming call event, ringing on local AG */
    BT_UI_CB_STATUS_HF_INCOME_CALL_NUM,    /**< A incoming call with caller number event, we can get the number with function BTGetCurInCallPhoneNum()*/

    BT_UI_CB_STATUS_HF_CALL_CONNECTED,     /**< A call is active on the AG */
    BT_UI_CB_STATUS_HF_CALL_DISCONNECTED,  /**< No call exists on the AG */
    BT_UI_CB_STATUS_HF_CALL_COMMAND_ERROR, /**< command invoking error at AG */

    BT_UI_CB_STATUS_GET_REMOTE_BT_DEV_NAME,/**< Got a remote connected bt device's name, we can get the string with function BTGetRemoteDevName() */

    BT_UI_CB_STATUS_CONNECTION_LOST,	/**< Baseband is lost */

    BT_UI_CB_STATUS_CONNECTION_ABORTED, /**< Stack aborted connection */
    
    BT_UI_CB_STATUS_HF_CALL_SETUP_OUT,  /**< A outgoing call event*/

    BT_UI_CB_STATUS_HF_PHONE_SPEAKER_VOL, /**< HF speaker vlume sync,one may invoke function BtGetRemoteSpeakerVol() to get the value[0-15]*/

	BT_UI_CB_STATUS_HF_FACTORY_NAME, /**< phone's factory name >**/

	BT_UI_CB_STATUS_HF_PHONE_TYPE, /**< phone type >**/
	
	BT_UI_CB_STATUS_HF_WAIT_INCOME_CALL_NUM,    /**< 2th income phone call number is coming >**/

    BT_UI_CB_STATUS_SPP_CONNECTED = 0xF2,  /**< SPP profile connected*/
    BT_UI_CB_STATUS_SPP_DISCONNECTED,      /**< SPP profile disconnected*/

    BT_UI_CB_STATUS_BT_MFi_CHANNEL_OPEN = 0xF6, /**< MFi bt channel is ready, one can do the data exchange */
    BT_UI_CB_STATUS_BT_MFi_DATA_IND,            /**< Apple iAp data is coming, one may invoke function BtGetMfiDataInfo(...) to get the data */
    BT_UI_CB_STATUS_BT_MFi_DATA_SENT,           /**< Data has been sent through iAp channel */

    BT_UI_CB_STATUS_RSSI_LINK_QUALITY_IND,      /**< Indicate RSSI and link quality value are updated, one may invoke function BtGetLinkqualityValue(),BtGetRssiValue()*/

    BT_UI_CB_STATUS_INQUIRY_RESULT = 0xFA, /**< Test mode event(no bd addr returned*/ 
	//Bukk-20170807
	BT_UI_CB_STATUS_INQUIRY_COMPLETE,
	BT_UI_CB_STATUS_INQUIRY_CANCELED,

    BT_UI_CB_STATUS_GET_NAME_PAGE_TIMEOUT = 0xFD,/**< Get remote device name page timeout*/ 
    BT_UI_CB_STATUS_BB_NO_RESPONSE = 0xFE, /**< BT BB has no response, so one may reboot the BB and stack. */ 

    BT_UI_CB_STATUS_PAGE_TIME_OUT  = 0xFF, /**< Page timeout(HF) or src not ready(ACL waiting for timeout)*/ 
};

/**
 * Phone Book list ids (AT Cmd @ HFP)
 */
#define BT_PB_LIST_ID_DIALLED          0x0001    /**< Dialled calls list,DC*/
#define BT_PB_LIST_ID_PHONE_MEMORY     0x0010    /**< Phone book list saved in phone memory,NOT the SIM card, ME*/
#define BT_PB_LIST_ID_SIM_CARD         0x0080    /**< Phone book list saved in the current selected/active SIM card, SM*/
#define BT_PB_LIST_ID_RECEIVED_CALL    0x0040    /**< Received calls list, RC*/
#define BT_PB_LIST_ID_MISSED_CALL      0x0008    /**< Missed calls list,MC*/

/**
 * BT_UI_CB_STATUS_PB_ONE_ENTRY_DATA
 */
typedef struct _OnePhonebookEntry 
{
    unsigned short Idx;             /**< Location in current(idx from 1) selected phone book */ 
    unsigned char  Type;            /**< Reserved */
    const char     *PhoneNumber;    /**< The phone number pointer */ 
    const char     *PhoneText;      /**< The text part in the format of UTF8 */ 
} OnePhonebookEntry;

/**
 * BT_UI_CB_STATUS_PB_ENTRY_SIZE
 */
typedef struct _CurPhonebookSize 
{
    unsigned short         IdxFirst;         /**<  First Entry Idx        */ 
    unsigned short         IdxLast;          /**<  Last Entry Idx          */ 
    unsigned short         MaxNumberStrLen;  /**<  Maximum number length */ 
    unsigned short         MaxTextStrLen;    /**<  Maximum text length   */ 
} CurPhonebookSize;

/**
 * @brief
 *  Check the AG whether it supports phonebook commands
 *	If the AG support phonebook commands, the BT_UI_CB_STATUS_PB_SUPPORTED event
 *	will be received
 *
 * @param NONE
 *
 * @return
 *		NONE
 * @note
 *	Must returns [BT_UI_CB_STATUS_PB_SUPPORTED],call BTGetSupportedPhonebookStorageType
 */
void BTQueryPhonebooks(void);

/**
 * @brief
 *  Select the storage of phonebook we want
 *
 * @param PhonebookBitID The stroage id(See Phone Book list ids )
 *
 * @return
 *		NONE
 * @note
 * 	The id must be one of the following values
 *	BT_PB_LIST_ID_DIALLED
 *	BT_PB_LIST_ID_PHONE_MEMORY
 *	BT_PB_LIST_ID_SIM_CARD
 *	BT_PB_LIST_ID_RECEIVED_CALL
 *	BT_PB_LIST_ID_MISSED_CALL
 */
void BTSelectPhonebook(unsigned short PhonebookBitID);

/**
 * @brief
 *  A command of get the numbers of phone calls
 *
 * @param NONE
 *
 * @return
 *		NONE
 * @note
 *	After call this command, we will receive event BT_UI_CB_STATUS_PB_ENTRY_SIZE
 *	and then we should call BtGetCurrentPbResponseData() to get the size
 */
void BTGetPhonebookSize(void);

/**
 * @brief
 *  A command of get list of phone calls
 *
 * @param StartIdx The start index(from number 1)
 * @param EndIdx	The end index(from number 1)
 *
 * @return
 *		NONE
 *
 * @note
 *	After call this command, we will receive event BT_UI_CB_STATUS_PB_ONE_ENTRY_DATA
 *	and then we should call BtGetCurrentPbResponseData()
 */
void BTGetPBListEntry(unsigned short StartIdx, unsigned short EndIdx);

/**
 * @brief
 *  Get phonebook storage type which AG support
 *
 * @param NONE
 *
 * @return
 *		BT_PB_LIST_ID
 *
 * @note
 *	
 */
unsigned short BTGetSupportedPhonebookStorageType(void);

/**
 * @brief
 *  Get the response data
 *
 * @param NONE
 *
 * @return
 *		The pointer to the received data
 *
 * @note
 *	
 */
void* BtGetCurrentPbResponseData(void);

/**
 * @brief
 *  Set the characters format to UTF8
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *		0 means failed
 *
 * @note
 *	
 */
unsigned char BTSetPhonebookCharacterSet2UTF8(void);

/**
 * @brief
 *  A command to get current call number
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *		0 means failed
 *
 * @note
 *	Event [BT_UI_CB_STATUS_CLCC_RESULT] may be received after the command is sent.
 */
unsigned char BtGetCurCallListNumber(void); 

/**
 * @brief
 *  Get the call number
 *
 * @param NONE
 *
 * @return
 *	Call number
 *
 * @note
 *	Can ONLY be invoked when reveived event [BT_UI_CB_STATUS_CLCC_RESULT].
 */
const char* BtGetCurCallListNumberResponseData(void);

/**
 * Current status of play state (only support in Advanced AVRCP )
 */
#define CurStatus_STOPPED  0x00	/**< Stopped */ 
#define CurStatus_PLAYING  0x01	/**< Playing */ 
#define CurStatus_PAUSED   0x02 /**< Paused */ 
#define CurStatus_FWD_SEEK 0x03 /**< Foward Seek */ 
#define CurStatus_REV_SEEK 0x04 /**< Back Seek */ 
#define CurStatus_ERROR    0xff /**< Error*/ 

/**
 * A struct of play status. 
 */
typedef struct _AdvAvrcpPlayStatus
{
    unsigned int  TrackLengthInMs;
    unsigned int  CurPosInMs;
    unsigned char CurStatus;
}AdvAvrcpPlayStatus;

/**
 * Track attribute defines
 */
#define TRACK_ATTR_TITLE       0x00000001 /**< title */ 
#define TRACK_ATTR_ARTIST      0x00000002 /**< artist */ 
#define TRACK_ATTR_ALBUM       0x00000003 /**< album */ 
#define TRACK_ATTR_TRACK       0x00000004 /**< track */ 
#define TRACK_ATTR_NUM_TRACKS  0x00000005 /**< number of track */ 
#define TRACK_ATTR_GENRE       0x00000006 /**< genre */ 
#define TRACK_ATTR_DURATION    0x00000007 /**< duration */ 

/**
 * Attribute info
 */
typedef struct _OneAttrInfo
{
    unsigned int   TackAttrID;					/**< Track attribute ID , TRACK_ATTR_XXXX */
    unsigned short CharacterSetID;			/**< Character set ID, such as UTF8--0x006a */
    unsigned short PropertyStringLen;		/**< The length of property string */
    const char*    PropertyString;			/**< The property string */
}OneAttrInfo;

/**
 * Track info
 */
typedef struct _AdvAvrcpTrackInfo
{
    unsigned char ItemNum;        /**< total property item number */
    OneAttrInfo   Property[7];  /**< max 7 track item info saved */
}AdvAvrcpTrackInfo;

/**
 * Shuffle defines
 */
#define ADV_AVRCP_ID_SHUFFLE_OFF          1
#define ADV_AVRCP_ID_SHUFFLE_ALL          2
#define ADV_AVRCP_ID_SHUFFLE_GROUP        3

/**
 * Repeat defines
 */
#define ADV_AVRCP_ID_REPEAT_OFF           1
#define ADV_AVRCP_ID_REPEAT_SINGLE        2
#define ADV_AVRCP_ID_REPEAT_ALL           3
#define ADV_AVRCP_ID_REPEAT_GROUP         4

/**
 * EQ defines
 */
#define ADV_AVRCP_ID_EQ_OFF               1
#define ADV_AVRCP_ID_EQ_ON                2

/**
 * Support settings. 
 * Event:BT_UI_CB_STATUS_AVAILABLE_SETTINGS_FOR_ONE_ATTR
 */
typedef struct _AdvAvrcpSupportedSettings
{
    unsigned char SupportedSettingNum;	/**< The number of support settings */
    unsigned char *SettingIds; 					/**< The settings, ADV_AVRCP_ID_SHUFFLE_XXX,...,ADV_AVRCP_ID_REPEAT_XXX,... */ 
}AdvAvrcpSupportedSettings;

/**
 * Current settings. 
 * Event:BT_UI_CB_STATUS_CUR_SETTING_FOR_ONE_ATTR
 */
typedef struct _AdvAvrcpCurSetting 
{
    unsigned short AdvAvrcpAttrMask; /**< it can be one of ADV_AVRCP_ATTRIBUTE_MASK_XXXX */
    unsigned char SettingEq;         /**< it can be one of ADV_AVRCP_ID_EQ_XXX for mask ADV_AVRCP_ATTRIBUTE_MASK_EQ */
    unsigned char SettingRepeat;     /**< it can be one of ADV_AVRCP_ID_REPEAT_XXX for mask ADV_AVRCP_ATTRIBUTE_MASK_REPEAT */
    unsigned char SettingShuffle;    /**< it can be one of ADV_AVRCP_ID_SHUFFLE_XXX for mask ADV_AVRCP_ATTRIBUTE_MASK_SHUFFLE */
    unsigned char Reserved;
}AdvAvrcpCurSetting;

/**
 * @brief
 *  	Disable advanced avrcp
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void          DisableAdvancedAvrcpFeature(void);

/**
 * @brief
 *  	Disable advanced avrcp auto report play status 
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void          DisableAdvancedAvrcpPlayStatusAutoReport(void);

/**
 * @brief
 *  	An advanced avrcp command to get play status
 *
 * @param NONE
 *
 * @return
 *		1 means the command sending successful
 *		0 means failed
 *
 * @note
 *		Event BT_UI_CB_STATUS_PLAYING_STATUS will be received when get the play status
 */
unsigned char BTPlayCtrlAdvAvrcpGetPlayStauts(void);

/**
 * @brief
 *  	An advanced avrcp command to get track info
 *
 * @param NONE
 *
 * @return
 *		1 means the command sending successful
 *		0 means failed
 *
 * @note
 *		Event BT_UI_CB_STATUS_TRACK_INFO will be received when get the track info
 */
unsigned char BTPlayCtrlAdvAvrcpGetTrackInfo(void);

/**
 * @brief
 *  	An advanced avrcp command to set play settings
 *
 * @param IsRepeatModeFlag 1 means repeat mode, 0 means shuffle mode
 * @param ModeParameter 
 *
 * @return
 *		1 means the command sending successful
 *		0 means failed
 *
 * @note
 *		
 */
unsigned char BTPlayCtrlAdvAvrcpSetPlayerSettingValues(unsigned char IsRepeatModeFlag /*repeat flag(1) or shuffle flag(0)*/, unsigned char ModeParameter);

/**
 * @brief
 *  	Get play settings command
 *
 * @param AttrMask ADV_AVRCP_ATTRIBUTE_MASK_XXX
 *
 * @return
 *		1 means the command sending successful
 *		0 means failed
 *
 * @note
 *		Event BT_UI_CB_STATUS_CUR_SETTING_FOR_ONE_ATTR will received
 */
unsigned char BTPlayCtrlAdvAvrcpGetPlayerSettingValues(unsigned short AttrMask); 

/**
 * @brief
 *  	An advanced avrcp command to set play settings list
 *
 * @param AttrID ADV_AVRCP_ATTRIBUTE_ID_XXXX
 *
 * @return
 *		1 means the command sending successful
 *		0 means failed
 *
 * @note
 *		Event BT_UI_CB_STATUS_AVAILABLE_SETTINGS_FOR_ONE_ATTR will received
 */
unsigned char BTPlayCtrlAdvAvrcpListPlayerSettingValues(unsigned char AttrID);

/**
 * @brief
 *  	An advanced avrcp command to set play settings list attributes
 *
 * @param NONE
 *
 * @return
 *		1 means the command sending successful
 *		0 means failed
 *
 * @note
 *		Event BT_UI_CB_STATUS_AVAILABLE_ATTRIBUTE_TYPES will received
 */
unsigned char BTPlayCtrlAdvAvrcpListPlayerSettingAttrs(void);

/**
 * @brief
 *  	Get response data
 *
 * @param NONE
 *
 * @return
 *		The pointer to the received data
 *
 * @note
 *		
 */
void* BtGetAdvAvrcpResponse(void);

/**
 * MFi releated functions
 */

/**
 * @brief
 *  	Connect MFi device with given address
 *
 * @param addr The bluetooth address of remote device
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void ConnectMfi(unsigned char* addr);


/**
 * @brief
 *  	Send apple device(MFi) compatible data through BT iAp interface.
 *
 * @param Data Data to be send. Maximum size is 640 bytes
 * @param DataLen The length of data to be send
 *
 * @return
 *		1 means the data has been pushed to the sending queue.
 *
 * @note
 *	  1)Parameter [Data] can be dropped after invoking this function.
 *		2)On CAN ONLY send other data after received the event [BT_UI_CB_STATUS_BT_MFi_DATA_SENT].
 *		3)Event[BT_UI_CB_STATUS_BT_MFi_CHANNEL_OPEN] means apple iAp channel is ready.
 */
int BTiApDataSend(unsigned char* Data , unsigned short DataLen);

/**
 * @brief
 *  	Get the MFi data info, including pointer of data and the length of data
 *
 * @param Data The pointer of the data pointer
 * @param Len The pointer of the length of data
 *
 * @return
 *		1 means successful
 *
 * @note
 *		This function can ONLY be invoked right after received event [BT_UI_CB_STATUS_BT_MFi_DATA_IND]
 */

int BtGetMfiDataInfo(unsigned char **Data, int *Len);


/**
 * No use for this version
 */
enum
{
    BT_ROM_CB_FUNC_UserProcSppData = 0,
    BT_ROM_CB_FUNC_BTDeviceInit,
    BT_ROM_CB_FUNC_BTStackStatusCallBackFunc,
    BT_ROM_CB_FUNC_BtStackSendDataRom,
    BT_ROM_CB_FUNC_BTStackDataStatusCallBackFunc,
    BT_ROM_CB_FUNC_OS_GetSystemTime,
    BT_ROM_CB_FUNC_BtStackRcvData,
    BT_ROM_CB_FUNC_NVRAM_READ,
    BT_ROM_CB_FUNC_NVRAM_WRITE,
    BT_ROM_CB_FUNC_BT_DEBUG,
    BT_ROM_CB_FUNC_CODE_FIX,
    BT_ROM_CB_FUNC_CODE_FIX_FLAG,
};

/**
 * No use for this version
 */
void BTSetRomOtpFunc(uint8_t FuncID/*BT_ROM_CB_FUNC_XXX*/, uint32_t FuncPointer);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
