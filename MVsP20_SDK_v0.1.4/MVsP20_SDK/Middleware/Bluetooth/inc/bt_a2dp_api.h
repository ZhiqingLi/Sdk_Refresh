/**
 *******************************************************************************
 * @file    bt_a2dp_api.h
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
* @defgroup bt_a2dp_api bt_a2dp_api.h
* @{
*/

#include "type.h"

#ifndef __BT_A2DP_API_H__
#define __BT_A2DP_API_H__

/**
 * The audio data format received from A2DP of Bluetooth stack.
 *
 * PCM means the audio data has been decoded in the stack.
 *
 * SBC means the audio data has not been decoded yet, and the SBC
 * data can be decoded in the application level or stored and so on.
 *
 */
typedef uint8_t	A2dpAudioDataFormat;

#define A2DP_AUDIO_DATA_PCM			1	/*!< A2DP audio data is decoded PCM data*/

#define A2DP_AUDIO_DATA_SBC			2	/*!< A2DP audio data is undecoded SBC data*/


typedef struct _A2dpAppFeatures
{
	A2dpAudioDataFormat	a2dpAudioDataFormat;
}A2dpAppFeatures;

/**
 * @brief  A2DP connect command.
 * @param  addr The address of bt device to connect.
 * @return True for the command implement successful 
 * @Note If the device has been connected with a2dp profile, the event 
 *		BT_STACK_EVENT_A2DP_CONNECTED will be received in the callback. Otherwise
 *		BT_STACK_EVENT_PAGE_TIMEOUT will be received.
 *
 */
bool A2dpConnect(uint8_t * addr);


/**
 * @brief  A2DP disconnect command.
 * @param  None
 * @return True for the command implement successful 
 * @Note If the device has been disconnected. the event BT_STACK_EVENT_A2DP_DISCONNECTED
 *		will be received in the callback. 
 */
bool A2dpDisconnect(void);

#endif /*__BT_A2DP_API_H__*/

/**
 * @}
 * @}
 */
 
