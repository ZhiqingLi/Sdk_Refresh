/**
 *******************************************************************************
 * @file    bt_hfp_api.h
 * @author  Halley
 * @version V1.0.1
 * @date    27-Apr-2016
 * @brief   HFP related api
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
* @addtogroup Bluetooth
* @{
* @defgroup bt_hfp_api bt_hfp_api.h
* @{
*/


#include "type.h"

#ifndef __BT_HFP_API_H__
#define __BT_HFP_API_H__

/**
 * The audio data format received from HFP of Bluetooth stack.
 *
 * If it supports WBS, two types of audio format can be selected
 * i). PCM means - The mSBC data in SCO has been decoded in the stack.
 * ii).mSBC means - The mSBC data in SCO has not been decoded yet.
 * 
 * If it doesn't support WBS, only PCM data can be received by application
 *
 */
typedef uint8_t	HfpAudioDataFormat;

#define HFP_AUDIO_DATA_PCM			1	/*!< HFP audio data is decoded PCM data*/

#define HFP_AUDIO_DATA_mSBC			2	/*!< HFP audio data is undecoded mSBC data*/



typedef struct _HfpAppFeatures
{
	bool				wbsSupport;
	HfpAudioDataFormat	hfpAudioDataFormat;
}HfpAppFeatures;

/**
 * @brief  HFP connect command.
 * @param  addr The address of bt device to connect.
 * @return True for the command implement successful 
 * @Note If the device has been connected with handsfree profile, the event 
 *		BT_STACK_EVENT_HFP_CONNECTED will be received in the callback. Otherwise
 *		BT_STACK_EVENT_PAGE_TIMEOUT will be received.
 *
 */
bool HfpConnect(uint8_t * addr);


/**
 * @brief  HFP disconnect command.
 * @param  None
 * @return True for the command implement successful 
 * @Note If the device has been disconnected. the event BT_STACK_EVENT_HFP_DISCONNECTED
 *		will be received in the callback. 
 */
bool HfpDisconnect(void);

/**
 * @brief  Answer a incoming call.
 * @param  None
 * @return True for the command implement successful 
 * @Note
 *
 */

bool HfpAnswerCall(void);

/**
 * @brief  Dial a number.
 * @param  number The string of the number to be dialed
 * @param  numberLen The string length of the number
 * @return True for the command implement successful 
 * @Note
 *
 */

bool HfpDialNumber(uint8_t * number, uint8_t numberLen);

/**
 * @brief  Redial the last outgoing number.
 * @param  None
 * @return True for the command implement successful 
 * @Note
 *
 */
bool HfpRedialNumber(void);

/**
 * @brief  Terminates an active call, cancels an outgoing call 
 *			or rejects an incoming call.
 * @param  None
 * @return True for the command implement successful 
 * @Note
 *
 */
bool HfpHangup(void);

/**
 * @brief  Get current calls in Audio Gateway(Phone)
 * @param  None
 * @return True for the command implement successful 
 * @Note
 *		The event BT_STACK_EVENT_HFP_CURRENT_CALLS with phone number
 *		will be received
 */
bool HfpGetCurrentCalls(void);

/**
 * @brief  Generate a DTMF code to the network.
 * @param  dtmfTone A single ASCII character in the set 0-9, #, *, A-D.
 * @return True for the command implement successful 
 * @Note
 *
 */

bool HfpDtmf(uint8_t dtmfTone);

/**
 * @brief  Enables or disables voice recognition on the Audio Gateway
 * @param  enable Set to TRUE if voice recognition is enabled, and FALSE
 *					if it is disabled.
 * @return True for the command implement successful 
 * @Note
 *
 */
bool HfpVoiceRecognition(bool enable);

/**
 * @brief  Disables noise reduction and echo canceling in Audio Gateway(Phone)
 * @param  None
 * @return True for the command implement successful 
 * @Note
 *		
 */
bool HfpDisableNREC(void);

/**
 * @brief  Gets the current speaker volume of the Hands-Free device
 * @param  gain The current gain level (0-15).  
 * @return True for the command implement successful 
 * @Note
 *		
 */
bool HfpSpeakerVolume(uint8_t gain);

/**
 * @brief  Gets the manufactory information of remote device
 * @param  NONE 
 * @return True for the command implement successful 
 * @Note
 *		
 */
bool HfpGetRemoteDeviceManufactory(void);

/**
 * @brief  Gets the type of remote device
 * @param  gain The current gain level (0-15).  
 * @return True for the command implement successful 
 * @Note
 *		
 */
bool HfpGetRemoteDeviceType(void);


/**
 * @brief  Sends any AT command..
 * @param  atCommand The string of the atCommand to be sent
 * @param  commandLen The string length of the atCommand
 * @return True for the command implement successful 
 * @Note
 *		
 */
bool HfpSendAtCommand(const char * atCommand, uint8_t commandLen);

/**
 * @brief  Disconnect audio connection (SCO) between Hands Free device and Audio Gateway
 * @param  None
 * @return True for the command implement successful 
 * @Note
 *		
 */
bool HfpAudioDisconnect(void);

/**
 * @brief  Create an audio connection (SCO) between Hands Free device and Audio Gateway
 * @param  None
 * @return True for the command implement successful 
 * @Note
 *		
 */
bool HfpAudioConnect(void);

/**
 * @brief  Transfer audio between Hands Free device and Audio Gateway
 * @param  None
 * @return True for the command implement successful 
 * @Note
 *		
 */
bool HfpAudioTransfer(void);

/**
 * @brief Send hfp audio data
 * @param data The audio data to be sent
 * @param dataLen The length of audio data to be sent
 * @return TRUE means the data has been insert to send
 *			queue The event BT_STACK_EVENT_HFP_SCO_DATA_SENT
 *			will be received when the audio data has been 
 *			sent successful
 * @Note
 *		
 */
bool HfpSendScoData(uint8_t * data, uint16_t dataLen);


/**
 * @brief  Set HFP enable/disable WBS function
 * @param  enable TRUE for eanbel WBS function
 * @return NONE
 * @Note
 *		
 */
void WbsSupport(bool enable);

/**
 * @brief  Get HFP enable/disable WBS function
 * @param  None
 * @return TRUE for enable WBS function
 * @Note
 *		
 */
bool GetWbsSupport(void);
#endif /*__BT_HFP_API_H__*/

/**
 * @}
 * @}
 */

