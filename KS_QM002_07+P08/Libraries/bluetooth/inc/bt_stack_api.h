/**
 *******************************************************************************
 * @file    bt_stack_api.h
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
* @addtogroup 蓝牙
* @{
* @defgroup bt_stack_api bt_stack_api
* @{
*/

#ifndef __BT_STACK_API_H__
#define __BT_STACK_API_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * BT support profiles
 */
 
#define  BT_FEATURE_SUPPORTED_ALL               0x00  /**< supported all features:A2DP+HF+SPP */

#define  BT_FEATURE_SUPPORTED_NORMAL            0x01	/**< supported all featrues except the SPP */

#define  BT_FEATURE_SUPPORTED_HIFI_ONLY         0x02	/**< supported A2DP ONLY */

#define  BT_FEATURE_SUPPORTED_HIFI_SPP          0x04	/**< supported A2DP + SPP */

#define  BT_FEATURE_SUPPORTED_SPP               0x08	/**< support SPP server ONLY */

#define  BT_FEATURE_SUPPORTED_HF_SPP            0x10	/**< support HF + SPP(server) */

/**
 * BT_FEATURE_SUPPORTED_XXXX | BT_REATURE_WITHOUT_MIC_SUPPORT means DO NOT support mic communication function!
 */
#define  BT_FEATURE_HAS_MIC_SUPPORT             0x00  /**< default */
#define  BT_FEATURE_WITHOUT_MIC_SUPPORT         0x80  /**< disable BT phone-call communication*/

/*---------------------------------------------------------------------------
 * BtClassOfDevice type
 *
 *     Bit pattern representing the class of device along with the 
 *     supported services. There can be more than one supported service.
 *     Service classes can be ORed together. The Device Class is composed
 *     of a major device class plus a minor device class. ORing together
 *     each service class plus one major device class plus one minor device
 *     class creates the class of device value. The minor device class is
 *     interpreted in the context of the major device class.
 */

typedef uint32_t BtClassOfDevice;

/* Group: Major Service Classes. Can be ORed together */
#define COD_LIMITED_DISCOVERABLE_MODE 0x00002000
#define COD_POSITIONING               0x00010000
#define COD_NETWORKING                0x00020000
#define COD_RENDERING                 0x00040000
#define COD_CAPTURING                 0x00080000
#define COD_OBJECT_TRANSFER           0x00100000
#define COD_AUDIO                     0x00200000
#define COD_TELEPHONY                 0x00400000
#define COD_INFORMATION               0x00800000

/* Group: Major Device Classes (Select one) */
#define COD_MAJOR_MISCELLANEOUS       0x00000000
#define COD_MAJOR_COMPUTER            0x00000100
#define COD_MAJOR_PHONE               0x00000200
#define COD_MAJOR_LAN_ACCESS_POINT    0x00000300
#define COD_MAJOR_AUDIO               0x00000400
#define COD_MAJOR_PERIPHERAL          0x00000500
#define COD_MAJOR_IMAGING             0x00000600
#define COD_MAJOR_UNCLASSIFIED        0x00001F00

/* Group: Minor Device Class - Audio Major class */
#define COD_MINOR_AUDIO_UNCLASSIFIED  0x00000000
#define COD_MINOR_AUDIO_HEADSET       0x00000004
#define COD_MINOR_AUDIO_HANDSFREE     0x00000008
#define COD_MINOR_AUDIO_MICROPHONE    0x00000010
#define COD_MINOR_AUDIO_LOUDSPEAKER   0x00000014
#define COD_MINOR_AUDIO_HEADPHONES    0x00000018
#define COD_MINOR_AUDIO_PORTABLEAUDIO 0x0000001C
#define COD_MINOR_AUDIO_CARAUDIO      0x00000020
#define COD_MINOR_AUDIO_SETTOPBOX     0x00000024
#define COD_MINOR_AUDIO_HIFIAUDIO     0x00000028
#define COD_MINOR_AUDIO_VCR           0x0000002C
#define COD_MINOR_AUDIO_VIDEOCAMERA   0x00000030
#define COD_MINOR_AUDIO_CAMCORDER     0x00000034
#define COD_MINOR_AUDIO_VIDEOMONITOR  0x00000038
#define COD_MINOR_AUDIO_VIDEOSPEAKER  0x0000003C
#define COD_MINOR_AUDIO_CONFERENCING  0x00000040
#define COD_MINOR_AUDIO_GAMING        0x00000048

/* Group: Minor Device Class - Peripheral Major class */
#define COD_MINOR_PERIPH_KEYBOARD     0x00000040
#define COD_MINOR_PERIPH_POINTING     0x00000080
#define COD_MINOR_PERIPH_COMBOKEY     0x000000C0

/* End of BtClassOfDevice */


/**
 * @brief
 *  	Get bt stack package supported features ID
 *
 * @param NONE
 *
 * @return
 *		BT_FEATURE_SUPPORTED_ALL
 *		BT_FEATURE_SUPPORTED_NORMAL
 *		BT_FEATURE_SUPPORTED_HIFI_ONLY
 *
 * @note
 *		
 */

uint8_t BTStackGetPackageFeatures(void);

/**
 * @brief
 *  	Set Bt Class of Device infor
 *
 * @param BtCod
 *		see: BtClassOfDevice(me.h)
 *
 * @return
 *		
 *
 * @note
 *		在BTStackRunInit之前调用
 *
 */
void SetBtClassOfDevice(uint32_t BtCod);

/**
 * @brief
 *  	Set User Pin Code
 *
 * @param pinCode
 *
 * @return
 *		
 *
 * @note
 *		在BTStackRunInit之前调用
 *
 */
void SetUserPinCode(uint8_t *pinCode);

/**
 * @brief
 * 		Write Inquiry Scan Params: Interval and Window
 * 
 * @param
 *		Interval: 0x0012~0x1000 (default: 0x1000); only even values are valid 
 *		Window: 0x0011~Inquiry Scan Interval(Max:0x1000) (default: 0x0012);
 *		time = N*0.625ms
 *	
 * @note
 *		在BT_UI_CB_STATUS_IDLE里面调用
 */
void WriteInquiryScanActivity(uint16_t newInterval, uint16_t newWindow);

/**
 * @brief
 *  	Initialize BT stack 
 *
 * @param BTDeviceType BT RF chipset
 * @param UserDefinedDevLocalName Local BT device name
 * @param BtFeatureID BT support features
 *
 * @return
 *		1 means initialize successful
 *		0 means failed
 *
 * @note
 *		
 */
uint32_t BTStackRunInit(uint8_t BTDeviceType, uint8_t *UserDefinedDevLocalName, uint8_t BtFeatureID);

/**
 * @brief
 *  	Invoke once for running one bt stack step.
 *		One may invoking this function one after another
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void  BTStackRun(void);

/**
 * @brief
 *  	Free BT stack allocated memory resource(allocated by function BTStackRunInit(...)).
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void  BTStackRunEnd(void);


/**
 * @brief
 *  	This function is used to close dongle at any time.
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		One may invoke this function at the end of closing bt stack but before releasing the stack memory.
 *		For this case:
 *				Dongle is in plugin status, but app has switched to none-bt mode,
 *				then, ONE need to stop the dongle BB working.
 *		For dongle is removed, then this function may NOT invoked.
 */
void BTStackCloseDongle(void);


/**
 * @brief
 *  	Set play cfg flag
 *
 * @param CfgFlag TRUE for indicating BT stack decode has been ready
 *
 * @return
 *		NONE
 *
 * @note
 *		This functions are used internal ONLY, pls use them carefully.
 *		one may invoke this function after cfg the sbc playing cfg
 *		(i.e. after invoking function audio_init())
 *
 */
void SetSbcPlayCfgFlag(bool CfgFlag);

/**
 * @brief
 *  	Get play cfg flag
 *
 * @param NONE
 *
 * @return
 *		0 means not configurated
 *		1 means has been configurated
 *
 * @note
 *		
 */
uint8_t GetSbcPlayCfgFlag(void);

/**
 * @brief
 *  	Inform BT stack that it can get a new SBC frame
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		After hw decoder returns error [SBC_ERROR_STREAM_EMPTY], one may invoke this function
 *		to enable refill sbc data by the bt stack.
 */
void SetSbcDecDoneFlag(void);

/**
 * @brief
 *  	Get the flag of SBC data status. If there are SBC data in BT stack, it will
 *		return a none-zero value.
 *
 * @param NONE
 *
 * @return
 *		0 means NOT sbc data, none-zero means has sbc data.
 *
 * @note
 *		
 */
uint8_t GetHasSbcDataFlag(void);

/**
 * @brief
 *  	Get sbc stream handle, which is used in hw.
 *
 * @param NONE
 *
 * @return
 *		The pointer to the buffer which receive the SBC data
 *
 * @note
 *		
 */
void* GetSbcStreamHandle(void);

/**
 * @brief
 *  	Get sco hci handle
 *
 * @param NONE
 *
 * @return
 *		None-zeror means OK
 *
 * @note
 *		
 */
uint16_t GetScoHciHandle(void);

/**
 * @brief
 *  	Get list item number for bt receiving buffer list inside the bt stack.
 *
 * @param NONE
 *
 * @return
 *		The number of the items in BT stack
 *
 * @note
 *		
 */
int32_t GetRcvListItemNum(void);

/**
 * OBEX Subsystem
 */

typedef struct _ObexCallbackInfo ObexCallbackInfo;

typedef void (*ObexCallback)(const ObexCallbackInfo *info);

/**
 * OBEX releate values
 */
enum
{
    BT_OBEX_NAME         = 1, /**< File Name */
    BT_OBEX_LENGTH       = 3, /**< File Length */
    BT_OBEX_BODY         = 8, /**< File Data */
    BT_OBEX_BODY_END     = 9, /**< File Data (Last Part) */
};

/**
 * OBEX connection releate values
 */
enum
{
    BT_OBEX_CONNECTED,     /**< OBEX connection is established */
    BT_OBEX_DISCONNECTED,  /**< OBEX connection is destroyed */
    BT_OBEX_DATA_IND,      /**< Received a OBEX data segment */
};

/**
 * OBEX callback info
 */
struct _ObexCallbackInfo
{
	uint8_t  event;      /**< OBEX Event */

	/**
	 *  Following fields are valid when event is DATA_IND
	 *      A file is splitted into multiple parts
	 *      A part is further splitted into multiple segments
	 *      Each time a segment is received
	 *
	 *      type & total describes the part
	 *      segoff/segdata/seglen describes the current segment
	 */

	uint8_t   type;       /**< Name, Length, Body ... */
	uint16_t  total;      /**< total length of this part of data */
	uint16_t  segoff;     /**< offset of this segment */
	uint8_t*  segdata;    /**< pointer to data */
	uint16_t  seglen;     /**< Segment Length */
};

/**
 * @brief
 *  	Initialize OBEX
 *
 * @param Callback	OBEX callback function
 *
 * @return
 *		NONE
 *
 * @note
 *		This function must be called after BTStackRunInit and before BTStackRun
 */
void BTStackInitObex(ObexCallback Callback);

/**
 * @brief
 *  	Lock OBEX.Call this function if app wants to use OBEX data after callback returns
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void BTStackLockObex(void);

/**
 * @brief
 *  	Unlock OBEX. Call this function when app finishes using OBEX data
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void BTStackUnlockObex(void);

/**
 * @brief
 *  	Abort OBEX transfer
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void BTStackAbortObex(void);

/**
 * @brief
 *  	Reject OBEX data
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void BTStackRejectObexData(void);

/**
 * bt stack data hook for receive or send, one can use this hook to view or do something intresting work on the stack.
 * return 1 means one wants to drop the received/send data from the stack(go round the stack data).
 * return 0 means do nothing about the stack.
 * Important Remarks: Do NOT modify data saved in [DataPointer].
 */
typedef uint8_t (*BTStackHookFunc)(uint8_t* DataPointer, int32_t DataSize);

/**
 * @brief
 *  	Set the hook function for sending data
 *
 * @param HookFunc The hook function for sending data
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void BTStackSetSendDataHook(BTStackHookFunc HookFunc);

/**
 * @brief
 *  	Set the hook function for receiving data
 *
 * @param HookFunc The hook function for receiving data
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void BTStackSetReceivedDataHook(BTStackHookFunc HookFunc);


/**
 * For user mem alloc purpose, allocated will never be free during the stack's whole life.
 */
typedef void* (*BTStackMemAllocHookFunc)(int32_t Size);

/**
 * @brief
 *  	Set the hook function for memory allocation
 *
 * @param HookFunc The hook function for memory allocation
 *
 * @return
 *		NONE
 *
 * @note
 *		
 */
void BTStackSetMemAllocHook(BTStackMemAllocHookFunc HookFunc);

/**
 * @brief
 *  	Get updated RSSI of BB connected, -127dB ~ +127dB, bigger is better
 *
 * @param NONE
 *
 * @return
 *		The value of RSSI
 *
 * @note
 *		
 */
int BtGetRssiValue(void);

/**
 * @brief
 *  	Get updated linkquality of BB connecetd, 0~255, bigger is better
 *
 * @param NONE
 *
 * @return
 *		The value of link quality
 *
 * @note
 *		
 */
int BtGetLinkqualityValue(void);

/**
 * @brief
 *  	HID send one byte of key
 *
 * @param Key The value of the key
 * @param Mode 
 *
 * @return
 *		1 means successful
 *
 * @note
 *		
 */
uint8_t BTHidKeySendByte(uint8_t Key, uint8_t Mode);

/**
 * @brief
 *  	HID send multi-key at one time
 *
 * @param Key  An array of the keys array, max is 6 bytes
 * @param Len The length of the key array
 * @param Mode 
 *
 * @return
 *		1 means successful
 *
 * @note
 *		
 */
uint8_t BTHidMultiKeySend(uint8_t *KeyData, uint8_t Len, uint8_t Mode);

/**
 * @brief
 *  	HID send data through key intrrupt channel
 *
 * @param Data  The data, max is 36 bytes
 * @param Len The length of the data
 *
 * @return
 *		1 means successful
 *
 * @note
 *		
 */
uint8_t BTHidDataSend(uint8_t *Data, uint8_t Len);

/**
 * @brief
 *  	HID send key down and keep
 *
 * @param KeyData  An array of the keys array, max is 6 bytes
 * @param Len The length of the key array
 * @param Mode 
 *
 * @return
 *		1 means successful
 *
 * @note
 *		
 */
uint8_t BTHidMultiKeySendDown(uint8_t *KeyData, uint8_t Len, uint8_t Mode);

/**
 * @brief
 *  	HID send key up
 *
 * @param NONE 
 *
 * @return
 *		1 means successful
 *
 * @note
 *		
 */
uint8_t BTHidMultiKeySendUp(void);

/**
 * @brief
 *  	Send user defined hid descriptor list
 *
 * @param ListData  The data, max is 256 bytes
 * @param Len The length of the data
 *
 * @return
 *		1 means successful
 *
 * @note
 *		Change hid description, details please refer to the related doc.
 */
void BTSetUserDefinedHidDescriptorList(uint8_t * ListData, uint16_t ListDataLen);

/**
 * @brief
 *  	Get the HID received data
 *
 * @param Data  The data buffer
 * @param Len The length of the received data
 *
 * @return
 *		1 means successful
 *
 * @note
 *		Receive the data in event BT_UI_CB_STATUS_HID_OUT_DATA_IND
 */
uint8_t BtHidGetOutData(uint8_t **Data, uint16_t* Len);

/**
 * @brief
 *  	Send raw data via HID channel
 *
 * @param RawData  The data to be sent, max 46 bytes
 * @param Len The length the data
 *
 * @return
 *		1 means successful
 *
 * @note
 *		
 */
uint8_t BTHidSendRawData(uint8_t *RawData, uint16_t Len);

/**
 * @brief
 *  	Judge the HID user mode whether is ready
 *
 * @param NONE
 *
 * @return
 *		1 means ready
 *
 * @note
 *		This function shuold be called before BTHidDataSend(),BTHidMultiKeySendDown(),
 *		BTHidMultiKeySendUp(),BTHidSendRawData().
 */
uint8_t BTHidIsReadyUserMode(void);

/**
 * @brief
 *  	Force to clear related flags
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *		0 means means error found,such as hid is not connected.
 *
 * @note
 */
uint8_t BTHidUserDataModeReset(void);

/**
* HID SDP database
*/
typedef struct _BT_HID_SDP_DATABASE_ITEM
{
    uint16_t AttributID;          /**< such as 0x0001 for "Service Class List" */
    uint16_t LenOfAttributeData;  /**< length of "AttributeData" */
    const uint8_t *AttributeData; /**< attribute data, such as: "0x35,0x03,0x19,0x11,0x24" for "Service Class List" */
    uint32_t WorkBuf;             /**< will be updated by BT stack     */
}BT_HID_SDP_DATABASE_ITEM;

/**
 * @brief
 *  	Set the user defined HID sdp database
 *
 * @param HidSdpDatabase Pointer to a sdp database buffer
 * @param HidSdpDatabaseItemCount The count of item in sdp database
 *
 * @return
 *		NONE
 *
 * @note
 */
void BTSetUserDefinedHidSdpDatabase(BT_HID_SDP_DATABASE_ITEM *HidSdpDatabase, uint16_t HidSdpDatabaseItemCount);



/**
 * @brief
 *  	Get the BT stack version
 *
 * @param NONE
 *
 * @return
 *		The string of BT stack version
 *
 * @note
 */
const unsigned char *BTStackGetVersion(void);
#define GetLibVersionBt()  BTStackGetVersion()

/**
 * @brief
 *  	Disable simple paring
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void BTDisableSimplePairing(void);

/**
 * @brief
 *  	Enable role switch when local BT devices being connected by remote one
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void BTEnableRoleSwitch(void);

/**
 * @brief
 *  	Disable QoS setup
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void BTDisableQosSetup(void);

/**
 * @brief
 *  	Disable local bt device enter into auto sniff mode
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void BTDisableAutoSniffMode(void);

/**
 * @brief
 *  	Enable BT debug information
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void EnableBtDbgInfo(void);

/**
 * @brief
 *  	Set MTK RF chipset EV3 property
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void SetMtEV3Property(void);

/**
 * @brief
 *  	Set MTK RF chipset 3EV3 property
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void SetMt3EV3Property(void);

/**
 * @brief
 *  	Enable role switch function. That means local BT device
 *		can be switched role by remote devices
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void SetEnableRoleSwitchFlag(void);

/**
 * @brief
 *  	Disable role switch function. That means local BT device
 *		can not be switched role by remote devices
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void SetDisableRoleSwitchFlag(void);

/**
 * @brief
 *  	NO USE in this version
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void UnSetRdaEscoProperty(void);

/**
 * @brief
 *  	Set MTK RF chipset to 6626. NO USE in this veriosn.
 *		You could EnableMtkRfAutoDetect() in MTK RF chipset
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void SetMTK6626RF(void);

/**
 * @brief
 *  	Enable detect MTK RF chipset.
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 * 		If this function is invoked, then function SetMTK6626RF() is disabled automatically.
 */
void EnableMtkRfAutoDetect(void);

/**
 * @brief
 *  	Set the pin code to BT stack
 *
 * @param pinCode PIN code string. The length must be 4 bytes
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void BTStackSetPinCode(uint8_t* pinCode);


/**
 * @brief
 *  	Set the timeout value in paging
 *
 * @param TimeOutValueInMs The value of timeout in paging
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void BTStatckSetPageTimeOutValue(uint16_t TimeOutValueInMs);

/**
 * @brief
 *  	IO cap setting for SPP connection ONLY case
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void BTSetIoCapSyncFlag(void);

/**
 * @brief
 *  	Enable apple devices iAP functions
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void EnableAppleiApFunction(void);

/**
 * @brief
 *  	Enable HID funcitons
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This funciton must be called before BTStackRunInit()
 */
void BtEnableHidFunc(void);

/**
 * @brief
 *  	Get current connected device's addr or the last connected(inq) device's addr
 *
 * @param NONE
 *
 * @return
 *		The name of the BT device
 *
 * @note
 *		
 */
uint8_t* BTGetCurDeviceNamesAddr(void);

/**
 * @brief
 *  	Set local BT device in inquiry mode
 *
 * @param NONE
 *
 * @return
 *		1 means successful
 *
 * @note
 *		This function ONLY can be called in test mode
 */
uint8_t BTDoTestModeDeviceInquiry(uint8_t MaxTimeInSecond);

/**
 * @brief
 *  	Enable MV BT testermode name="MVBTTester01"
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 */
void BTEnableBluetoothTestMode(void);

/**
 * @brief
 *  	Remove all paired devices in local database
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *
 */
void BtStackRemoveAllDevice(void);

/*pbap*/

enum {
	PBAP_CONNECT_START = 0,
	PBAP_CONNECT_OK,
	PBAP_CONNECT_ERROR,
	PBAP_CLOSE_OK,
	PBAP_CLOSE_START,
	PBAP_DATA_START,
	PBAP_DATA,
	PBAP_DATA_END,
	PBAP_DATA_END1,
	PBAP_NOT_ACCEPTABLE,
};

struct _PbapCallbackInfo {
    uint8_t  event;		// PBAP Event
    uint8_t  *buffer;	// pointer to data buffer
    uint32_t length;	// data length
};

typedef struct _PbapCallbackInfo PbapCallbackInfo;

typedef void (*PbapCallback)(const PbapCallbackInfo *info);

/**
 * @brief
 *  	Initialize pbap
 *
 * @param 
 *		NONE
 *
 * @return
 *		TURE = SUCCESS
 *
 * @note
 *		This function must be called after BTStackRunInit and before BTStackRun
 */
bool BTStackInitPbap(PbapCallback Callback);

/**
 * @brief
 *  	pbap connect
 *
 * @param 
 *		NONE
 *
 * @return
 *		1 = connect success
 *		0 = fail
 *
 * @note
 *		This function must be called after BTStackRunInit and before BTStackRun
 */
bool PBAPConnect(void);

/**
 * @brief
 *  	pbap disconnect
 *
 * @param 
 *		NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		NONE
 */
void PBAPDisconnect(void);

/**
 * @brief
 *  	pbap pull phone book
 *
 * @param 
 *		sel: SIM/PHONE
 *		buf: pointer to type info 
 *
 * @return
 *		NONE
 *
 * @note
 *		NONE
 */
void PBAP_PullPhoneBook(uint8_t Sel,uint8_t *buf);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
