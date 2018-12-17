/**
 *******************************************************************************
 * @file    bt_hid_api.h
 * @author  Halley
 * @version V1.0.1
 * @date    27-Apr-2016
 * @brief   MFi related api
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
 * @defgroup bt_hid_api bt_hid_api.h
 * @{
 */

#ifndef __BT_HID_API_H__
#define __BT_HID_API_H__

/**
 * SDP_ATTRIBUTE structure
 *
 *     Defines an attribute's ID and value.
 */
typedef struct _SDPAttribute		SDPAttribute;


/**
 * SDP_ATTRIBUTE structure
 *
 *     Defines an attribute's ID and value.
 */
struct _SDPAttribute
{
	/* Attribute ID. */
	uint16_t			id;

	/* Length of the value buffer */
	uint16_t			len;

	/* An array of bytes that contains the value
	* of the attribute. 
	*/
	const uint8_t		*value;

	/*Internal use, should be set 0*/
	uint16_t			flags;
};


/*
 * A set of SDP attributes of HID profiles.
 * Used for SDP records
 */
typedef struct _HidSdpRecord		HidSdpRecord;


struct _HidSdpRecord
{

	/* Total number of attributes in "attribs". */
	uint8_t			num;

	/* Pointer to an array of attributes.
	 *
	 * Store the attributes in ascending order by attribute ID, and
	 * do not add two attributes with the same ID.
	 */
	SDPAttribute	*attribs;

	/* The service class fields of the class of device.
	 * The device class portion is ignored.
	 */
	uint32_t				classOfDevice; 
};

typedef uint8_t			HidChannelType;

/** Hid control channel */
#define HidCtrlChannel			1

/** Hid interrupt channel*/
#define HidIntrChannel			2


typedef struct _HidData		HidData;

struct _HidData
{
	HidChannelType	channel;
	uint8_t 		*data;
	uint16_t		dataLen;
};

bool HidInitWithSdp(HidSdpRecord *hidSdpRecord);

void HidDeinit(void);

bool HidConnect(uint8_t * addr);

bool HidDisconnect(void);

bool HidRegisterSdpRecord(HidSdpRecord *hidSdpRecord);

bool HidSendData(HidData * hidData);


#endif /*__BT_HID_API_H__*/
/**
 * @}
 * @}
 */
  

