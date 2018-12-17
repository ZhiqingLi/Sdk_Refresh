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
#include "ud_setup.h"
#include "ud_intern.h"

//for buffer address set
uint8 __section__(".data.buf_ram") g_hid_in_buf[HID_REPORT_SIZE] =
{   0};

//for buffer address set
uint8 __section__(".data.buf_ram") g_hid_out_buf[HID_REPORT_SIZE] =
{   0};

device_requests_t g_setup_data;

uint8 g_usb_state = 0;

uint8 g_alter_setting = 0;

uint16 g_interface_status = 0;

uint32 g_hid_change = 0;

uint8 g_usb_vid[6] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

uint8 g_usb_pid[6] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

//uint32* hid_in_report;
//uint32* hid_out_report;

//uint8 hid_Descriptor[0x21] =
uint8 g_hid_descriptor[REPORT_DESCRIPTOR_SIZE] = {
    0x06, 0x00, 0xff, //USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,     //USAGE (Vendor Usage 1)
    0xA1, 0x01,     //COLLECTION (Application)
    0x19, 0x01,     //(USAGE_MINIMUM 1)
    0x29, 0xff,     //(USAGE_MAXIMUM 255)
    0x15, 0x00,     //LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x00, //LOGICAL_MAXIMUM (255)
    0x75, 0x08,     //REPORT_SIZE (8)
#if(HID_REPORT_SIZE <0x100)
    0x95, HID_REPORT_SIZE,     //REPORT_COUNT (<256)
#else
    0x96, 0x00, (HID_REPORT_SIZE >> 8),  //REPORT_COUNT (512)
#endif
    0x81, 0x02,     //INPUT (Data,Var,Abs)
    0x19, 0x01,     //(USAGE_MINIMUM 1)
    0x29, 0xff,     //(USAGE_MAXIMUM 255)
    0x91, 0x02,     //OUTPUT (Data,Var,Abs)
    0xC0            // END_COLLECTION
};


device_descriptor_t Device_Descriptor = {
    sizeof(device_descriptor_t), //size of descriptor
    DEVICE,     //descriptor type
    0x0200,     //USB release version 2.0
    0x00,       //class code
    0x00,       //sub-class code
    0x00,       //protocol code
    0x40,       //max packet size = 64byte
    0x10d6,     //vendor ID
    0x000a,     //product ID
    0x0100,     //device release number
    0x01,       //index of string descriptor of manufacturer
    0x02,       //index of string descriptor of product
    0x03,       //index of string descriptor of serial number
    0x01,       //number of possible configuration
};


//-----------------------------------------------------------
//start of Configuration
//-----------------------------------------------------------
device_hid_configuration_t  All_Descriptors_of_Configuration = {

    /*device_config_descriptor_t Configuration_Descriptor =*/
    {
        sizeof(device_config_descriptor_t), //size of descriptor
        CONFIG,     //descriptor type
        sizeof(device_hid_configuration_t),  //total length
        0x01,       //number of interface = 1,hid deviece
        0x01,       //configuration value
        0x00,       //configuration string index
        0xC0,       //attribute (bus powered, remote wakeup disable)
        0x96,       //max power (500mA),96h(300mA)
    },

    /*Standard_AS_Interface_Descriptor_t Interface=*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE,  //descriptor type
        0x00,       //interface value
        0x00,       //alternative setting
        0x02,       //number of endpoint
        0x03,       //HID Interface Class
        0x00,       //
        0x00,       //interface protocol code, doesn't use any class-specific protocols.
        0x00,       //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*Standard_HID_Descriptor_t HID_Descriptor_Setting =*/
    {
        sizeof(Standard_HID_Descriptor_t), //size of descriptor
        0x21,       //descriptor type
        0x0100,     //bcdHID
        0x00,       //bCountryCode
        0x01,       //bNumDescriptors
        0x22,       //bDescriptorType
        REPORT_DESCRIPTOR_SIZE,     //wDescriptorLength
    },

    /*Standard_Endpiont_Descriptor1_t Endpiont_Descriptor_Setting=*/
    {
        sizeof(Standard_Endpiont_Descriptor1_t), //size of descriptor
        ENDPOINT,   //descriptor type
        0x81,       //bEndpointAddress:-> Direction: IN - EndpointID: 1
        0x03,       //bmAttributes:-> Interrupt Transfer Type
        0x200,     //wMaxPacketSize
        0x80,       //bInterval
    },

    /*Standard_Endpiont_Descriptor1_t Endpiont_Descriptor_Setting =*/
    {
        sizeof(Standard_Endpiont_Descriptor1_t), //size of descriptor
        ENDPOINT,   //descriptor type
        0x01,       //bEndpointAddress:-> Direction: out - EndpointID: 1
        0x03,       //bmAttributes:-> Interrupt Transfer Type
        0x200,     //wMaxPacketSize
        0x80,       //bInterval
    },

};
//-----------------------------------------------------------
//end of Configuration
//-----------------------------------------------------------

string_language_id_t language_string = {
    0x04,       /* length*/
    0x03,       /* Second Byte of this descriptor.  */
    0x0409,     /* LANGID code array, US english,0x409 */
};

string_product_t device_name_string = {
    0x22,
    0x03,
    'U', 'S', 'B', ' ', 'H', 'I', 'D', 'D', 'E', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

string_manufacturer_t manufacure_string = {
    0x12,
    0x03,
    'A', 'c', 't', 'i', 'o', 'n', 's', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

serialnum_descriptor_t SerialNumber = {
    0x12,
    0x03,
    'U', '2', 'd', 'c', '0', 'z', '7', 'w', 's', '5', '3', 'a', '7', '9', 'r', 'n',
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

