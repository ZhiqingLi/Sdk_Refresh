#ifndef _USB_H_
#define _USB_H_

#define RET_USB_SUCCESS                             (0)
#define RET_USB_FAILURE                             (-1)

// endpoint number
#define  EP_0                                   0x0
#define  EP_1                                   0x1
#define  EP_2                                   0x2
#define  EP_3                                   0x3
#define  EP_4                                   0x4
#define  EP_5                                   0x5
#define  EP_6                                   0x6
#define  EP_7                                   0x7

// Standard Descriptor Types
#define  DSC_DEVICE                             0x01   // Device Descriptor
#define  DSC_CONFIG                             0x02   // Configuration Descriptor
#define  DSC_STRING                             0x03   // String Descriptor
#define  DSC_INTERFACE                          0x04   // Interface Descriptor
#define  DSC_ENDPOINT                           0x05   // Endpoint Descriptor

// Standard Request Codes
#define  GET_STATUS                             0x00   // Code for Get Status
#define  CLEAR_FEATURE                          0x01   // Code for Clear Feature
#define  SET_FEATURE                            0x03   // Code for Set Feature
#define  SET_ADDRESS                            0x05   // Code for Set Address
#define  GET_DESCRIPTOR                         0x06   // Code for Get Descriptor
#define  SET_DESCRIPTOR                         0x07   // Code for Set Descriptor(not used)
#define  GET_CONFIGURATION                      0x08   // Code for Get Configuration
#define  SET_CONFIGURATION                      0x09   // Code for Set Configuration
#define  GET_INTERFACE                          0x0A   // Code for Get Interface
#define  SET_INTERFACE                          0x0B   // Code for Set Interface
#define  SYNCH_FRAME                            0x0C   // Code for Synch Frame(not used)

// Define wIndex bitmaps
#define  IN_EP1                                 0x81   // Index values used by Set and Clear
#define  OUT_EP1                                0x01   // commands for Endpoint_Halt
#define  IN_EP2                                 0x82
#define  OUT_EP2                                0x02
#define  IN_EP3                                 0x83
#define  OUT_EP3                                0x03
#define  IN_EP4                                 0x84
#define  OUT_EP4                                0x04
#define  IN_EP5                                 0x85
#define  OUT_EP5                                0x05
#define  IN_EP6                                 0x86
#define  OUT_EP6                                0x06
#define  IN_EP7                                 0x87
#define  OUT_EP7                                0x07

#define EP_IDLE                                 0x00
#define EP_TX                                   0x11
#define EP_RX                                   0x10
#define EP_SET_ADDRESS                          0x21
#define EP_WAIT_STATUS                          0x20
#define EP_STALL                                0xaa

/*full-speed devices is 8, 16, 32, or 64 bytes; for high-speed devices, it is 64 bytes and for low-speed devices, it is 8 bytes*/
#define EP0_FS_MAX_PACKET_SIZE                  0x40   // this value can be 8/16/32/64, depending on device speed, see USB spec

// HID Descriptor Types
#define DSC_HID                                 0x21   // HID Class Descriptor
#define DSC_HID_REPORT                          0x22   // HID Report Descriptor

// HID Request Codes
#define GET_REPORT                              0x01   // Code for Get Report
#define GET_IDLE                                0x02   // Code for Get Idle
#define GET_PROTOCOL                            0x03   // Code for Get Protocol
#define SET_REPORT                              0x09   // Code for Set Report
#define SET_IDLE                                0x0A   // Code for Set Idle
#define SET_PROTOCOL                            0x0B   // Code for Set Protocol

// mass storage class
#define MASS_STORAGE_RESET                      0xFF
#define GET_MAX_LUN                             0xFE


// Define device states
#define DEV_ATTACHED                            (0 << 0)  // Device is in Attached State
#define DEV_POWERED                             (1 << 0)  // Device is in Powered State
#define DEV_DEFAULT                             (1 << 1)  // Device is in Default State
#define DEV_ADDRESS                             (1 << 2)  // Device is in Addressed State
#define DEV_CONFIGURED                          (1 << 3)  // Device is in Configured State
#define DEV_SUSPENDED                           (1 << 4)  // Device is in Suspended State

// Define bmRequestType bitmaps
#define IN_DEVICE                               0x00  // Request made to device, direction is IN
#define OUT_DEVICE                              0x80  // Request made to device, direction is OUT
#define IN_INTERFACE                            0x01  // Request made to interface, direction is IN
#define OUT_INTERFACE                           0x81  // Request made to interface, direction is OUT
#define IN_ENDPOINT                             0x02  // Request made to endpoint, direction is IN
#define OUT_ENDPOINT                            0x82  // Request made to endpoint, direction is OUT

//define bmRequestType bit5, bit6
#define REQUESTTYPE_STANDARD                    0x00
#define REQUESTTYPE_CLASS                       0x20
#define REQUESTTYPE_VENDOR                      0x40

// Define wValue bitmaps for Standard Feature Selectors
#define DEVICE_REMOTE_WAKEUP                    0x01  // Remote wakeup feature(not used)
#define ENDPOINT_HALT                           0x00  // Endpoint_Halt feature selector

#define SET_CURRENT                             0x01
#define GET_CURRENT                             0x81
#define GET_MINIUM                              0x82
#define GET_MAXIUM                              0x83
#define GET_RES                                 0x84

#ifndef  MSB
#define  MSB                                    0
#endif

#ifndef  LSB
#define  LSB                                    1
#endif

// SETUP Packet Type Definition
#define SETUP_PKT_LEN                          (8)
typedef struct
{
//8B
   uint8 bmRequestType;                         // Request recipient, type, and dir
   uint8 bRequest;                              // Specific standard request number
   HF_WORD wValue;                                 // varies according to request
   HF_WORD wIndex;                                 // varies according to request
   HF_WORD wLength;                                // Number of bytes to transfer
}__attribute__((packed)) setup_buffer;                               // End of SETUP Packet Type

typedef  struct
{
   uint8 bLength;               // Size of this Descriptor in Bytes
   uint8 bDescriptorType;       // Descriptor Type (=1)
   uint16 bcdUSB;               // USB Spec Release Number in BCD
   uint8 bDeviceClass;          // Device Class Code
   uint8 bDeviceSubClass;       // Device Subclass Code
   uint8 bDeviceProtocol;       // Device Protocol Code
   uint8 bMaxPacketSize0;       // Maximum Packet Size for EP0
   uint16 idVendor;             // Vendor ID
   uint16 idProduct;            // Product ID
   uint16 bcdDevice;            // Device Release Number in BCD
   uint8 iManufacturer;         // Index of String Desc for Manufacturer
   uint8 iProduct;              // Index of String Desc for Product
   uint8 iSerialNumber;         // Index of String Desc for SerNo
   uint8 bNumConfigurations;    // Number of possible Configurations
} struct_usb_device_descriptor;  

// Standard Configuration Descriptor Type Definition
typedef  struct
{
    uint8 bLength;              // Size of this Descriptor in Bytes
    uint8 bDescriptorType;      // Descriptor Type (=2)
    uint8 length_l;
    uint8 length_h;
    uint8 bNumInterfaces;       // No of Interfaces supported by this Conf
    uint8 bConfigurationValue;  // Designator Value for this Configuration
    uint8 iConfiguration;       // Index of String Desc for this Conf
    uint8 bmAttributes;         // Configuration Characteristics (see below)
    uint8 bMaxPower;            // Max. Power Consumption in this Conf (*2mA)
} struct_usb_configuration_descriptor;            // End of Configuration Descriptor Type

// Standard Interface Descriptor Type Definition
typedef  struct
{
    uint8 bLength;              // Size of this Descriptor in Bytes
    uint8 bDescriptorType;      // Descriptor Type (=4)
    uint8 bInterfaceNumber;     // Number of *this* Interface (0..)
    uint8 bAlternateSetting;    // Alternative for this Interface (if any)
    uint8 bNumEndpoints;        // No of EPs used by this IF (excl. EP0)
    uint8 bInterfaceClass;      // Interface Class Code
    uint8 bInterfaceSubClass;   // Interface Subclass Code
    uint8 bInterfaceProtocol;   // Interface Protocol Code
    uint8 iInterface;           // Index of String Desc for this Interface
} struct_usb_interface_descriptor;                // End of Interface Descriptor Type

// Standard Class Descriptor Type Definition
typedef struct
{
    uint8 bLength;               // Size of this Descriptor in Bytes (=9)
    uint8 bDescriptorType;       // Descriptor Type (HID=0x21)
    uint8 bcdHID_l;              // release number (=1.01)
    uint8 bcdHID_h;
    uint8 bCountryCode;          // Localized country 
    uint8 bNumDescriptors;       // Number of class descriptors to follow
    uint8 bReportDescriptorType; // Report descriptor type (HID=0x22)
    uint8 wItemLength_l;
    uint8 wItemLength_h;
} struct_usb_class_descriptor;                     // End of Class Descriptor Type

// Standard Endpoint Descriptor Type Definition
typedef  struct
{
    uint8 bLength;              // Size of this Descriptor in Bytes
    uint8 bDescriptorType;      // Descriptor Type (=5)
    uint8 bEndpointAddress;     // Endpoint Address (Number + Direction)
    uint8 bmAttributes;         // Endpoint Attributes (Transfer Type)
    uint8 wMaxPacketSize_l;
    uint8 wMaxPacketSize_h;
    uint8 bInterval;            // Polling Interval (Interrupt) ms
} struct_usb_endpoint_descriptor;                 // End of Endpoint Descriptor Type

typedef struct
{
   uint8 Audio_Descriptor[214]; // Descriptor Type (=5)
}struct_usb_configuration_audio_descriptor;

#endif
