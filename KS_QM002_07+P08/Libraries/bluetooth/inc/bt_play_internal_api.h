/**
 *******************************************************************************
 * @file    bt_play_internal_api.h
 * @author  Halley
 * @version V1.0.0
 * @date    26-June-2012
 * @brief   Bluetooth LE releated APIs
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
* @defgroup bt_play_internal_api bt_play_internal_api
* @{
*/

#ifndef __BT_PLAY_INTERNAL_API_H__
#define __BT_PLAY_INTERNAL_API_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
* IMPORTAMT:
* For a phone call procedure, stack will invoke the following status functions in step of:
*  1) BT_DS_CALL_START
*  2) BT_DS_SCO_STREAM_START (if any)
*  3) BT_DS_CALL_SCO_PAUSE
*  4) BT_DS_CALL_END
*/

enum
{
    BT_DS_A2DP_STREAM_START = 1,	/**< A2DP stream start event*/
    BT_DS_A2DP_STREAM_SUSPEND,		/**< A2DP stream stop event*/

    BT_DS_CALL_START,      /**< Phone call started. */
    BT_DS_SCO_STREAM_START,/**< SCO stream start */
    BT_DS_CALL_SCO_STREAM_PAUSE, /**< SCO stream stop */
    BT_DS_CALL_END,        /**< Phone call ended. */
    BT_DS_CALL_AEC_SUPPORTED, /**< AEC support event */

    BT_DS_SNIFF_MODE, /**< BT device in sniff mode */
    BT_DS_ACTIVE_MODE, /**< BT device in active mode */
};

/**
 * @brief
 *  	all back function used internally for users cfg sco and a2dp playing channel
 *
 * @param CBParameter OUT, can be one of BT_DS_XXXXXX
 * @param HistoryStatus IN/OUT,User used ONLY, size = 1 Byte.
 *
 * @return
 *		BT_FEATURE_SUPPORTED_ALL
 *		BT_FEATURE_SUPPORTED_NORMAL
 *		BT_FEATURE_SUPPORTED_HIFI_ONLY
 *
 * @note
 *		will be AUTOMATICALLY reset to zero when new BT device is connected.
 *		is located in internal bt stack memory space.
 */

void BTStackDataStatusCallBackFunc(uint8_t CBParameter,
								uint8_t* HistoryStatus
								);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
