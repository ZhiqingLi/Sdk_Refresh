/****************************************************************************
**
** Copyright (C) 2011 Beken Corporation, All rights reserved.
**
** Contact: Beken Corporation (www.beken.com)
**
** Author:  river
**
** History: 2012/05/15 
**
** Version: 1.0
**
****************************************************************************/
/*! \file driver_usb_standard_requests.h
    \brief The driver headfile of usb standard requests module.
*/

#ifndef _USB_STANDARD_REQUESTS_H_
#define _USB_STANDARD_REQUESTS_H_

//#define USB_TEST_SPEED

#ifdef USB_TEST_SPEED
#define BK3260_PID                0x3260
#define BEKEN_VID                 0xAAAA
#else
#define BK3260_PID                0x3260
#define BEKEN_VID                 0xA825
#endif

#define USB_BCD                   0x0110    // USB Specification Release Number in Binary-Coded Decimal
#define DEVICE_DESCRIPTOR_SIZE    18
#define CONFIG_DESCRIPTOR_SIZE    9

/* This must be counted manually and updated with the descriptor */
/* 1*Config(9) + 1*Interface(9) + 2*Endpoint(7) = 32 bytes */
#define CONFIG_DESC_SIZE          (32)
#define MASS_STORAGE_INTERFACE    (0)

/*The USB defines the allowable maximum bulk data payload sizes to be only 8, 16,
32, or 64 bytes for full-speed endpoints and 512 bytes for high-speed endpoints. A low-speed device must
not have bulk endpoints.*/
#define  EP_FS_MAX_PACKET_SIZE   (64)

#define EP_BULK_OUT              (1)
#define EP_BULK_IN               (2)
#define DIRECTION_EP_OUT         (0)
#define DIRECTION_EP_IN          (0x80)

#define LEN_USB_STRING            3
#define LEN_STRING_LANGID         4
#define LEN_STRING_MANUFACTURER   36 //sizeof("BEKEN Corporation") * 2
#define LEN_STRING_PRODUCT        38 //sizeof("BEKEN Udisk Device") * 2

#define LEN_USB_CONFIGURATION_DESCRIPTOR_LE     0x1101
#define LEN_USB_CONFIGURATION_DESCRIPTOR        0x0111

#define HID_MEDIA_REPORT_DESCRIPTOR_SIZE        0x0026
#define HID_MEDIA_REPORT_DESCRIPTOR_SIZE_LE     0x2600

#define HID_CONTROL_REPORT_DESCRIPTOR_SIZE      0x0039
#define HID_CONTROL_REPORT_DESCRIPTOR_SIZE_LZ   0x3900

#define USB_UINT_16_LOW(x)                   ((x) & 0xFF)
#define USB_UINT_16_HIGH(x)                  (((x) >> 8) & 0xFF)

void driver_usb_get_status(EP_ENTITY_PTR ctrl_ep_ptr);
void driver_usb_clear_feature(EP_ENTITY_PTR ctrl_ep_ptr);
void driver_usb_set_feature(EP_ENTITY_PTR ctrl_ep_ptr);
void driver_usb_set_address(EP_ENTITY_PTR ctrl_ep_ptr);
void driver_usb_get_descriptor(EP_ENTITY_PTR ctrl_ep_ptr);
void driver_usb_get_configuration(EP_ENTITY_PTR ctrl_ep_ptr);
void driver_usb_set_configuration(EP_ENTITY_PTR ctrl_ep_ptr);
void driver_usb_get_interface(EP_ENTITY_PTR ctrl_ep_ptr);
void driver_usb_set_interface(EP_ENTITY_PTR ctrl_ep_ptr);

#ifdef HID_CLASS
void driver_usb_get_idle(void);
void driver_usb_get_protocol(void);
void driver_usb_set_protocol(void);
void driver_usb_set_idle(void);
void driver_usb_get_report(void);
void driver_usb_set_report(void);
void driver_usb_set_interface_current(void);
void driver_usb_get_interface_current(void);
void driver_usb_get_interface_min(void);
void driver_usb_get_interface_max(void);
void driver_usb_get_interface_resolution(void);
#endif
#endif

/*  end file**/
