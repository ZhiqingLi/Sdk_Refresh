/********************************************************************************
 *                              US280A
 *                            Module: USB device
 *                 Copyright(c) 2001-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>            <version >             <desc>
 *      dengtaiping 2013-01-18 16:40     1.0                build this file
 ********************************************************************************/
#ifndef __USOUND_H__
#define __USOUND_H__

#include "typeext.h"

//#define     _USB_RECORD_
#define HID_REPORT_SIZE                512

//uint8 pa_mute = 0;
#if (HID_REPORT_SIZE <0x100)
#define REPORT_DESCRIPTOR_SIZE         29
#else
#define REPORT_DESCRIPTOR_SIZE         30
#endif

/* bRequest */
#define GET_STATUS                     0x00
#define CLEAR_FEATURE                  0x01
#define SET_FEATURE                    0x03
#define SET_ADDRESS                    0x05
#define GET_DESCRIPTOR                 0x06
#define SET_DSCRPTOR                   0x07
#define GET_CONFIGURATION              0x08
#define SET_CONFIGURATION              0x09
#define GET_INTERFACE                  0x0a
#define SET_INTERFACE                  0x0b

//standard feature
#define EP_HALT                        0x00
#define DEV_RMT_WAKE                   0x01
#define TEST_MODE                      0x02

/* Audio class-specific request codes.  */
#define REQUEST_CODE_UNDEFINED         0x00
#define SET_CUR                        0x01
#define GET_CUR                        0x81
/* channel number.  */
#define MAIN_CHANNEL_NUMBER0           0x00
#define MAIN_CHANNEL_NUMBER1           0x01
#define MAIN_CHANNEL_NUMBER2           0x02

#define SAMPLE_FREQUENCY_LENGTH        0x03

/* wValue */
#define VALUE_DEVICE                   0x0100
#define VALUE_CONFIG                   0x0200
#define VALUE_STRING                   0x0300

#define VALUE_STRING_LANGID            0x0300
#define VALUE_STRING_MANUFACTURER      0x0301
#define VALUE_STRING_PRODUCT           0x0302
#define VALUE_STRING_SERIAL_NUMBER     0x0303

/* descriptor type */
#define DEVICE                         0x01
#define CONFIG                         0x02
#define STRING                         0x03
#define INTERFACE                      0x04
#define ENDPOINT                       0x05

#define VALUE_ADDRESS                  0x0001
#define VALUE_SET_CONFIGURATION        0x0001
#define VALUE_ALTERNATE_SETTING0       0x0000
#define VALUE_ALTERNATE_SETTING1       0x0001
#define VALUE_ALTERNATE_SETTING2       0x0002
#define VALUE_ALTERNATE_SETTING3       0x0003
#define INDEX_INTERFACE0               0x0000
#define INDEX_INTERFACE1               0x0001
#define INDEX_INTERFACE2               0x0002
#define INDEX_INTERFACE3               0x0003

/* bRequest */
#define RD_STD_DEV                     0x80
#define RD_STD_IF                      0x81
#define RD_STD_EP                      0x82

#define WR_STD_DEV                     0x00
#define WR_STD_IF                      0x01
#define WR_STD_EP                      0x02

#define SET_UNIT_CONTROL               0x21
#define GET_UNIT_CONTROL               0xA1
//#define SPECIFIC_REQUEST_OUT           0x21
//#define SPECIFIC_REQUEST_IN            0xA1
#define SET_ENDPOINT_CONTROL           0x22
#define SAMPLING_FREQ_CONTROL          0x0100

#define GET_REPORT                     0x01
#define SET_REPORT                     0x09
#define GET_IDLE                       0x02
#define SET_IDLE                       0x0a

#define EP0_MAX_PACKET_SIZE            0x40
//#define ISO_MAX_PACKET_SIZE            0xC8
//#define MIC_ISO_MAX_PACKET_SIZE        0x16//0x2C
#define SAMPLE_BYTE                    0x02
#define CHANNEL_NUMBER                 0x02

#pragma pack(1)
/* SETUP data for a USB device control request  */
typedef struct {
    uint8 bmRequestType;
    uint8 bRequest;
    uint16 wValue;
    uint16 wIndex;
    uint16 wLength;
} device_requests_t;

/* descriptor of device */
typedef struct {
    uint8 bLength;
    uint8 bDescriptorType;

    uint16 bcdUSB;
    uint8 bDeviceClass;
    uint8 bDeviceSubClass;
    uint8 bDeviceProtocol;
    uint8 bMaxPacketSize0;
    uint16 idVendor;
    uint16 idProduct;
    uint16 bcdDevice;
    uint8 iManufacturer;
    uint8 iProduct;
    uint8 iSerialNumber;
    uint8 bNumConfigurations;
} device_descriptor_t;

/* descriptor of configuration */
typedef struct {
    uint8 bLength;
    uint8 bDescriptorType;
    uint16 wTotalLength;
    uint8 bNumInterfaces;
    uint8 bConfigurationValue;
    uint8 iConfiguration;
    uint8 bmAttributes;
    uint8 bMaxPower;
} device_config_descriptor_t;

typedef struct {
    uint8 bLength; //描述符大小
    uint8 bDescriptorType; //IAD描述符类型
    uint8 bFirstInterface; //起始接口
    uint8 bInterfaceCount; //接口数
    uint8 bFunctionClass; //类型代码
    uint8 bFunctionSubClass; //子类型代码
    uint8 bFunctionProtocol; //协议代码
    uint8 iFunction; //描述字符串索引
} usb_IAD_descriptor_t;

/* Interface descriptor */
typedef struct {
    uint8 bLength;
    uint8 bDescriptorType;

    uint8 bInterfaceNumber;
    uint8 bAlternateSetting;
    uint8 bNumEndpoints;
    uint8 bInterfaceClass;
    uint8 bInterfaceSubClass;
    uint8 bInterfaceProtocol;
    uint8 iInterface;
} usb_interface_descriptor_t;

typedef struct {
    uint8 bLength; //Size of this descriptor, in bytes: 9
    uint8 bDescriptorType; //INTERFACE descriptor type
    uint8 bInterfaceNumber; /*Number of interface. A zero-based value identifying the index
     in the array of concurrent interfaces supported by this configuration.*/
    uint8 bAlternateSetting; /*Number Value used to select an alternate setting
     for the interface identified in the prior field.*/
    uint8 bNumEndpoints; //Number of endpoints used by this interface (excluding endpoint 0).
    uint8 bInterfaceClass; //Audio Interface Class code(assigned by the USB).0x01 Audio.
    uint8 bInterfaceSubClass; //Audio Interface Subclass code.0x02 AUDIOSTREAMING.
    uint8 bInterfaceProtocol; //Protocol Not used. Must be set to 0.
    uint8 iInterface; //Index of a string descriptor that describes this interface.
} Standard_AS_Interface_Descriptor_t; //Table 4-18: Standard AS Interface Descriptor


typedef struct {
    uint8 bLength; //Size of this descriptor in bytes: 7
    uint8 bDescriptorType; //CS_INTERFACE descriptor type.
    uint8 bDescriptorSubtype; //AS_GENERAL descriptor subtype.
    uint8 bTerminalLink; //The Terminal ID of the Terminal to which the endpoint of this interface is connected.
    uint8 bDelay; //Delay (d) introduced by the data path. Expressed in number of frames.
    uint16 wFormatTag; /*The Audio Data Format that has to be used to communicate with this interface.
     0x0001 PCM Format.*/
} ClassSpecific_AS_Interface_Descriptor_t; //Table 4-19: Class-Specific AS Interface Descriptor

typedef struct {
    uint8 bLength; //Size of this descriptor, in bytes
    uint8 bDescriptorType; //CS_INTERFACE descriptor.
    uint8 bDescriptorSubtype; //FORMAT_TYPE subtype.
    uint8 bFormatType; //FORMAT_TYPE_I.
    uint8 bNrChannels; //N channel.
    uint8 bSubFrameSize; //N bytes per audio subframe.
    uint8 bBitResolution; //N bits per sample.
    uint8 bSamFreqType; //N frequency supported.
    uint8 tSamFreq1[3]; //Sample frequency.
    uint8 tSamFreq2[3]; //Sample frequency.
} Format_Type_Descriptor_t; //Table B-10: USB Microphone Type I Format Type Descriptor

typedef struct {
    uint8 bLength; //Size of this descriptor, in bytes: 9
    uint8 bDescriptorType; //ENDPOINT descriptor type
    uint8 bEndpointAddress; /*Endpoint The address of the endpoint on the USB device described by this descriptor.
     The address is encoded as follows:
     D7: Direction.0 = OUT endpoint 1 = IN endpoint
     D6..4: Reserved, reset to zero
     D3..0: The endpoint number,determined by the designer. */
    uint8 bmAttributes; /*Bit Map
     D3..2: Synchronization type 01 = Asynchronous 10 = Adaptive 11 = Synchronous
     D1..0: Transfer type 01 = Isochronous
     All other bits are reserved.*/
    uint16 wMaxPacketSize; /*Number Maximum packet size this endpoint is capable of sending or receiving
     when this configuration is selected.
     This is determined by the audio bandwidth constraints of the endpoint.*/
    uint8 bInterval; /*Interval for polling endpoint for data transfers expressed in milliseconds.
     Must be set to 1.*/
    uint8 bRefresh; //Reset to 0.
    uint8 bSynchAddress; /*The address of the endpoint used to communicate synchronization information
     if required by this endpoint. Reset to zero
     if no synchronization pipe is used. */
} Standard_Endpiont_Descriptor_t;//Table 4-20: Standard AS Isochronous Audio Data Endpoint Descriptor

typedef struct {
    uint8 bLength; //Size of this descriptor, in bytes: 7
    uint8 bDescriptorType; //CS_ENDPOINT descriptor type.
    uint8 bDescriptorSubtype; //EP_GENERAL descriptor subtype.
    uint8 bmAttributes; /*A bit in the range D6..0 set to 1 indicates
     that the mentioned Control is supported by this endpoint.
     D0: Sampling Frequency
     D1: Pitch
     D6..2: Reserved
     Bit D7 indicates a requirement for wMaxPacketSize packets.
     D7: MaxPacketsOnly*/
    uint8 bLockDelayUnits; /*Number Indicates the units used for the wLockDelay field:
     0: Undefined 1: Milliseconds 2: Decoded PCM samples 3..255: Reserved*/
    uint16 wLockDealy; /*Indicates the time it takes this endpoint
     to reliably lock its internal clock recovery circuitry.
     Units used depend on the value of the bLockDelayUnits field.*/
} ClassSpecific_Endpiont_Descriptor_t; //Table 4-21: Class-Specific AS Isochronous Audio Data Endpoint Descriptor

typedef struct {
    uint8 bLength; //Size of this descriptor, in bytes: 7
    uint8 bDescriptorType; //CS_ENDPOINT descriptor type.
    uint16 bcdHID; //
    uint8 bCountryCode;
    uint8 bNumDescriptors;
    uint8 bDescriptorType1;
    uint16 wDescriptorLength;
} Standard_HID_Descriptor_t;

typedef struct {
    uint8 bLength; //Size of this descriptor, in bytes: 9
    uint8 bDescriptorType; //ENDPOINT descriptor type
    uint8 bEndpointAddress; /*Endpoint The address of the endpoint on the USB device described by this descriptor.
     The address is encoded as follows:
     D7: Direction.0 = OUT endpoint 1 = IN endpoint
     D6..4: Reserved, reset to zero
     D3..0: The endpoint number,determined by the designer. */
    uint8 bmAttributes; /*Bit Map
     D3..2: Synchronization type 01 = Asynchronous 10 = Adaptive 11 = Synchronous
     D1..0: Transfer type 01 = Isochronous
     All other bits are reserved.*/
    uint16 wMaxPacketSize; /*Number Maximum packet size this endpoint is capable of sending or receiving
     when this configuration is selected.
     This is determined by the audio bandwidth constraints of the endpoint.*/
    uint8 bInterval; /*Interval for polling endpoint for data transfers expressed in milliseconds.
     Must be set to 1.*/
} Standard_Endpiont_Descriptor1_t;

typedef struct {
    //config
    device_config_descriptor_t Configuration_Descriptor;
    //interface descriptor
    usb_interface_descriptor_t Interface_Descriptor;
    //hid descriptor
    Standard_HID_Descriptor_t HID_Descriptor_Setting;
    //endpiont input
    Standard_Endpiont_Descriptor1_t Endpiont_Descriptor_input;
    //endpoint output
    Standard_Endpiont_Descriptor1_t Endpiont_Descriptor_output;
} device_hid_configuration_t;

/* string descriptor */
typedef struct {
    uint8 bLength;
    uint8 bDescriptorType;
    uint16 wLanguageID;
} string_language_id_t;

typedef struct {
    uint8 bLength;
    uint8 bDescriptorType;
    uint8 wProductName[32];
} string_product_t;

typedef struct {
    uint8 bLength;
    uint8 bDescriptorType;
    uint8 wManufacturerName[16];
} string_manufacturer_t;

typedef struct {
    uint8 bLength;
    uint8 bDescriptorType;
    uint8 wManufacturerName[32];
} serialnum_descriptor_t;

/*dma*/
typedef struct {
    uint8 *source_address;
    uint8 *destination_address;
    uint16 data_length;
} data_information_for_move_t;

#pragma pack() //保存对齐状态
#endif

