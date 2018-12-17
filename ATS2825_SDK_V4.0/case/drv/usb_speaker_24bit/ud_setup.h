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
#define SET_MIN                        0x02
#define GET_MIN                        0x82
#define SET_MAX                        0x03
#define GET_MAX                        0x83
#define SET_RES                        0x04
#define GET_RES                        0x84
#define SET_MEM                        0x05
#define GET_MEM                        0x85
#define GET_STAT                       0xFF

#define GET_REPORT                     0x01
#define GET_IDLE                       0x02
#define SET_IDLE                       0x0a

/* feature unit control selectors.  */
#define FU_CONTROL_UNDEFINED           0x00
#define MUTE_CONTROL                   0x01
#define VOLUME_CONTROL                 0x02
#define BASS_CONTROL                   0x03
#define MID_CONTROL                    0x04
#define TREBLE_CONTROL                 0x05
#define GRAPHIC_EQUALIZER_CONTROL      0x06
#define AUTOMATIC_GAIN_CONTROL         0x07
#define DELAY_CONTROL                  0x08
#define BASS_BOOST_CONTROL             0x09
#define LOUDNESS_CONTROL               0x0A

#define FEATURE_UNIT_ID1               0x09
#define FEATURE_UNIT_INDEX1            0x0900
#define FEATURE_UNIT_ID2               0x0A
#define FEATURE_UNIT_INDEX2            0x0A00
#define SELECTOR_UNIT_ID               0x0B
#define SELECTOR_UNIT_INDEX            0x0B00

/* channel number.  */
#define MAIN_CHANNEL_NUMBER0           0x00
#define MAIN_CHANNEL_NUMBER1           0x01
#define MAIN_CHANNEL_NUMBER2           0x02

/* volume configuration of max, min, resolution.  */
#define MAXIMUM_VOLUME                 0xfff0
#define MINIMUM_VOLUME                 0xe3a0
#define RESOTION_VOLUME                0x0030
#define MUTE_LENGTH                    0x01
#define VOLUME_LENGTH                  0x02
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

//#define INDEX_INTERFACE0               0x0000 //composite
//#define INDEX_INTERFACE1               0x0001 //speaker
//#define INDEX_INTERFACE2               0x0002 //record
//#define INDEX_INTERFACE3               0x0003 //hid

#define INDEX_INTERFACE_CTRL           0x00
#define INDEX_INTERFACE_PLAY           0x01
#define INDEX_INTERFACE_HID            0x02

/* bRequest */
#define RD_STD_DEV                     0x80
#define RD_STD_IF                      0x81
#define RD_STD_EP                      0x82

#define WR_STD_DEV                     0x00
#define WR_STD_IF                      0x01
#define WR_STD_EP                      0x02

#define SPECIFIC_REQUEST_OUT           0x21
#define SPECIFIC_REQUEST_IN            0xA1
#define GET_SAMPLERATE                 0xA2
#define SET_ENDPOINT_CONTROL           0x22
#define SAMPLING_FREQ_CONTROL          0x0100

#define EP0_MAX_PACKET_SIZE            0x40
#define ISO_MAX_PACKET_SIZE            0x240
#define MIC_ISO_MAX_PACKET_SIZE        0x16//0x2C
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

/*audio class description*/
typedef struct {
    uint8 bLength; //Size of this descriptor, in bytes: 8+n
    uint8 bDescriptorType; //CS_INTERFACE descriptor type.
    uint8 bDescriptorSubtype; //HEADER descriptor subtype.
    uint16 bcdADC; //Audio Device Class Specification Release Number in Binary-Coded Decimal.
    uint16 wTotalLength; /*Total number of bytes returned for the class-specific AudioControl
     interface descriptor. Includes the combined length of this descriptor header
     and all Unit and Terminal descriptors.*/
    uint8 bInCollection; /*The number of AudioStreaming and MIDIStreaming interfaces
     in the Audio Interface Collection to which this AudioControl interface belongs: n*/
    uint8 baInterfaceNr1; /*Interface number of the first AudioStreaming or MIDIStreaming interface
     in the Collection.*/
} Audio_Class_Head_t; //Table 4-2: Class-Specific AC Interface Header Descriptor

typedef struct {
    uint8 bLength; //Size of this descriptor, in bytes: 12
    uint8 bDescriptorType; //CS_INTERFACE descriptor type.
    uint8 bDescriptorSubtype; //INPUT_TERMINAL descriptor subtype.
    uint8 bTerminalID; /*Constant uniquely identifying the Terminal within the audio function.
     This value is used in all requests to address this Terminal.*/
    uint16 wTerminalType; //Constant characterizing the type of Terminal.See USB Audio Terminal Types.
    uint8 bAssocTerminal; //ID of the Output Terminal to which this Input Terminal is associated.
    uint8 bNrChannels; /*Number of logical output channels
     in the Terminal’s output audio channel cluster.*/
    uint16 wChannelConfig; //Describes the spatial location of the logical channels.
    uint8 iChannelNames; //Index of a string descriptor, describing the name of the first logical channel.
    uint8 iTerminal; //Index of a string descriptor, describing the Input Terminal.
} Audio_Class_Input_t; //Table 4-3: Input Terminal Descriptor

typedef struct {
    uint8 bLength; //Size of this descriptor, in bytes: 9
    uint8 bDescriptorType; //CS_INTERFACE descriptor type.
    uint8 bDescriptorSubtype; //OUTPUT_TERMINAL descriptor subtype.
    uint8 bTerminalID; /*Constant uniquely identifying the Terminal within the audio function.
     This value is used in all requests to address this Terminal.*/
    uint16 wTerminalType; //Constant characterizing the type of Terminal.See USB Audio Terminal Types.
    uint8 bAssocTerminal; //Constant,identifying the Input Terminal to which this Output Terminal is associated.
    uint8 bSourceID; //ID of the Unit or Terminal to which this Terminal is connected.
    uint8 iTerminal; //Index of a string descriptor,describing the Output Terminal.
} Audio_Class_Output_t; //Table 4-4: Output Terminal Descriptor

typedef struct {
    uint8 bLength; //Size of this descriptor, in bytes:7+(ch+1)*n
    uint8 bDescriptorType; //CS_INTERFACE descriptor type.
    uint8 bDescriptorSubtype; //FEATURE_UNIT descriptor subtype.
    uint8 bUnitID; /*Constant uniquely identifying the Unit within the audio function.
     This value is used in all requests to address this Unit.*/
    uint8 bSourceID; //ID of the Unit or Terminal to which this Feature Unit is connected.
    uint8 bControlSize; //Size in bytes of an element of the bmaControls() array: n
    uint8 bmaControls0; /*A bit set to 1 indicates that the mentioned Control is supported for master channel 0
     D0: Mute D1: Volume D2: Bass D3: Mid D4: Treble D5: Graphic Equalizer
     D6: Automatic Gain D7: Delay D8: Bass Boost D9: Loudness D10..(n*8-1): Reserved */
    uint8 bmaControls1; /*A bit set to 1 indicates
     that the mentioned Control is supported for logical channel 1*/
    uint8 bmaControls2; /*A bit set to 1 indicates
     that the mentioned Control is supported for logical channel 2*/
    uint8 iFeature; //Index of a string descriptor, describing this Feature Unit.
} Audio_Class_Feature1_t; //Table 4-7: Feature Unit Descriptor

typedef struct {
    uint8 bLength; //Size of this descriptor, in bytes:6+p
    uint8 bDescriptorType; //CS_INTERFACE descriptor type.
    uint8 bDescriptorSubtype; //FEATURE_UNIT descriptor subtype.
    uint8 bUnitID; /*Constant uniquely identifying the Unit within the audio function.
     This value is used in all requests to address this Unit.*/
    uint8 bNrInPins; //Number of Input Pins of this Unit:p
    uint8 baSourceID; /*ID of the unit or terminal to which the first input pin of this
     selector unit is connected*/
    uint8 bSelector; //index of a string descriptor,describing the select unit
} Audio_Class_Selector_t;

typedef struct {
    uint8 bLength; //Size of this descriptor, in bytes:7+(ch+1)*n
    uint8 bDescriptorType; //CS_INTERFACE descriptor type.
    uint8 bDescriptorSubtype; //FEATURE_UNIT descriptor subtype.
    uint8 bUnitID; /*Constant uniquely identifying the Unit within the audio function.
     This value is used in all requests to address this Unit.*/
    uint8 bSourceID; //ID of the Unit or Terminal to which this Feature Unit is connected.
    uint8 bControlSize; //Size in bytes of an element of the bmaControls() array: n
    uint8 bmaControls0; /*A bit set to 1 indicates that the mentioned Control is supported for master channel 0
     D0: Mute D1: Volume D2: Bass D3: Mid D4: Treble D5: Graphic Equalizer
     D6: Automatic Gain D7: Delay D8: Bass Boost D9: Loudness D10..(n*8-1): Reserved */
    uint8 bmaControls1; /*A bit set to 1 indicates
     that the mentioned Control is supported for logical channel 1*/
    uint8 iFeature; //Index of a string descriptor, describing this Feature Unit.
} Audio_Class_Feature2_t; //Table 4-7: Feature Unit Descriptor


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
    uint8 tSamFreq3[3]; //Sample frequency.
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
    device_config_descriptor_t Configuration_Descriptor;
    usb_interface_descriptor_t Interface0_Descriptor;
    Audio_Class_Head_t Audio_Class_Head;
    Audio_Class_Input_t Audio_Class_Input1;

    Audio_Class_Output_t Audio_Class_Output1;

    Audio_Class_Feature1_t Audio_Class_Feature1;

    Standard_AS_Interface_Descriptor_t Interface1_Setting0;
    Standard_AS_Interface_Descriptor_t Interface1_Setting1;
    ClassSpecific_AS_Interface_Descriptor_t Audio_Class_General_Setting1;
    Format_Type_Descriptor_t Audio_Class_Format_Setting1;
    Standard_Endpiont_Descriptor_t Endpoint_Descriptor_Setting1;
    ClassSpecific_Endpiont_Descriptor_t Audio_Data_Endpoint_Setting1;

    Standard_AS_Interface_Descriptor_t Interface3_Setting0;
    Standard_HID_Descriptor_t HID_Descriptor_Setting0;
    Standard_Endpiont_Descriptor1_t Endpiont_Descriptor_Setting3;
} device_audio_configuration_t;

typedef struct {
    device_config_descriptor_t Configuration_Descriptor;
} device_HID_configuration_t;

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
#endif  //__USOUND_H__
