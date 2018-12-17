/**
 *******************************************************************************
 * @file    bt_common_api.h
 * @author  Halley
 * @version V1.0.1
 * @date    27-Apr-2016
 * @brief   common related api
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
* @defgroup bt_common_api bt_common_api.h
* @{
*/

/**
* Inquiry mode define
*/
typedef uint8_t					BTInquiryMode;

#define INQUIRY_MODE_NORMAL		0 /*!< Normal Inquiry Response format			*/
#define INQUIRY_MODE_RSSI		1 /*!< RSSI Inquiry Response format				*/
#define INQUIRY_MODE_EXTENDED	2 /*!< Extended or RSSI Inquiry Response format	*/


typedef struct _InquriyResultParam
{
	const uint8_t	*addr;
	signed short	rssi;
	const uint8_t	*extResp;
	uint16_t		extRespLen;
}InquriyResultParam;


typedef struct _RequestRemNameParam
{
	const uint8_t	*addr;
	const uint8_t	*name;
	uint16_t		nameLen;
}RequestRemNameParam;


/**
 * BtAccessibleMode type
 */
typedef uint8_t		BtAccessMode;

#define BtAccessModeNotAccessible		0x00 /*!< Non-discoverable or connectable		*/
#define BtAccessModeDiscoverbleOnly		0x01 /*!< Discoverable but not connectable		*/
#define BtAccessModeConnectableOnly		0x02 /*!< Connectable but not discoverable		*/
#define BtAccessModeGeneralAccessible	0x03 /*!< General discoverable and connectable	*/

/**
 * BTLinkMode type
 */
typedef uint8_t		BTLinkMode;

#define BTLinkModeActive		0x00
#define BTLinkModeHold			0x01
#define BTLinkModeSniff			0x02
#define BTLinkModePark			0x03
#define BTLinkModeScatter		0x04

typedef struct _ModeChange
{
	BTLinkMode	mode;
	uint8_t		*addr;
}ModeChange;

/**
 * @brief  Inquiry command, the bluetooth device will start inquiry with inquiry mode.
 * @param  inquiryMode The inquiry mode, see BTInquiryMode.
 * @return True for the command implement successful 
 * @Note 
 *
 */
bool BTInquiry(BTInquiryMode inquiryMode);

/**
 * @brief  Cancel.
 * @param  NONE.
 * @return True for the command implement successful 
 * @Note 
 *
 */
bool BTInquiryCancel(void);

/**
 * @brief  Get remote device name.
 * @param  addr The bt address of remote device.
 * @return True for the command implement successful 
 * @Note 
 *
 */
bool BTGetRemoteDeviceName(uint8_t * addr);

/**
 * @brief  Set local bluetooth access mode.
 * @param  accessMode The local bluetooth access mode.
 * @return True for the command implement successful 
 * @Note 
 *
 */
bool BTSetAccessMode(BtAccessMode accessMode);


/**
 * @brief  Info remote device into sniff mode
 * @param  remAddr The bluetooth address of remote device.
 * @return True for the command implement successful 
 * @Note 
 *
 */

void BTSetRemDevIntoSniffMode(uint8_t * remAddr);

/**
 * @brief  Info remote device exit from sniff mode
 * @param  remAddr The bluetooth address of remote device.
 * @return True for the command implement successful 
 * @Note 
 *
 */	
void BTSetRemDevExitSniffMode(uint8_t * remAddr);


/**
 * @}
 * @}
 */
 

