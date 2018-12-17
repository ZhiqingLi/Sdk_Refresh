#include "type.h"

#define USB_DT_DEVICE		1
#define USB_DT_CONFIG		2
#define USB_DT_STRING		3
#define USB_DT_INTERFACE	4
#define USB_DT_ENDPOINT		5
#define USB_DT_DEVICE_QUALIFIER			6
#define USB_HID_REPORT		0x22

#define USB_REQ_GET_STATUS				0
#define USB_REQ_CLEAR_FEATURE			1
#define USB_REQ_SET_FEATURE				3
#define USB_REQ_SET_ADDRESS				5
#define USB_REQ_GET_DESCRIPTOR			6
#define USB_REQ_SET_DESCRIPTOR			7
#define USB_REQ_GET_CONFIGURATION		8
#define USB_REQ_SET_CONFIGURATION		9
#define USB_REQ_GET_INTERFACE			10
#define USB_REQ_SET_INTERFACE			11
#define USB_REQ_SYNCH_FRAME				12


// Max packet size. Fixed, user should not modify.
#define	DEVICE_FS_CONTROL_MPS		64
#define	DEVICE_FS_INT_IN_MPS		64
#define	DEVICE_FS_BULK_IN_MPS		64
#define	DEVICE_FS_BULK_OUT_MPS		64
#define	DEVICE_FS_ISO_IN_MPS		192
#define	DEVICE_FS_ISO_OUT_MPS		192

// Endpoint number. Fixed, user should not modify.
//#define	DEVICE_CONTROL_EP			0x00
//#define	DEVICE_INT_IN_EP			0x81
//#define	DEVICE_BULK_IN_EP			0x82
//#define	DEVICE_BULK_OUT_EP			0x03
//#define	DEVICE_ISO_IN_EP			0x84
//#define	DEVICE_ISO_OUT_EP			0x05
#define	DEVICE_CONTROL_EP			0x00
#define	DEVICE_INT_IN_EP			0x81


#define	DEVICE_BULK_OUT_EP			0x01
#define	DEVICE_BULK_IN_EP			0x82


//#define	DEVICE_BULK_OUT_EP			0x03
#define	DEVICE_ISO_IN_EP			0x82
#define	DEVICE_ISO_OUT_EP			0x03

#define MSC_INTERFACE_NUM			0
#define AUDIO_ATL_INTERFACE_NUM		1
#define AUDIO_SRM_OUT_INTERFACE_NUM	2
#define AUDIO_SRM_IN_INTERFACE_NUM	3
#define HID_CTL_INTERFACE_NUM		4
#define HID_DATA_INTERFACE_NUM		5

#define READER						0//¶Á¿¨Æ÷
#define AUDIO						1//Éù¿¨
#define HID						2//HIDÊý¾Ý´«Êä
#define READER_ADUIO				3//¶Á¿¨Æ÷ & Éù¿¨
#define READER_HID					4//¶Á¿¨Æ÷ & HIDÊý¾Ý´«Êä
#define AUDIO_HID					5//Éù¿¨ & HIDÊý¾Ý´«Êä
#define READER_AUDIO_HID			6//¶Á¿¨Æ÷ & Éù¿¨& HIDÊý¾Ý´«Êä

void OTG_DeviceModeSel(uint8_t Mode);
void OTG_DeviceRequestProcess(void);

