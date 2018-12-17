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

//-----------------------------------------------------------
//start of Configuration
//-----------------------------------------------------------
const device_audio_configuration_t All_Descriptors_of_Configuration1 = {
    /*device_config_descriptor_t Configuration_Descriptor =*/
    {
        sizeof(device_config_descriptor_t), //size of descriptor
        CONFIG, //descriptor type
        sizeof(device_audio_configuration_t), //total length
#ifdef _USB_RECORD_
        0x04,
#else
        0x03, //number of interface = 3,for audio class
#endif
        0x01, //configuration value
        0x00, //configuration string index
        0x80, //attribute (bus powered, remote wakeup disable)
        0x32, //max power (500mA),96h(300mA)
    },

    /*usb_interface_descriptor_t Interface0_Descriptor =*/
    {
        sizeof(usb_interface_descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        INDEX_INTERFACE_CTRL,//0x00, //interface number,
        0x00, //alternative setting
        0x00, //number of endpoint
        0x01, //interface class code, Audio Interface class.
        0x01, //interface sub-class code, AUDIOCONTROL Subclass.
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*Audio_Class_Head_t Audio_Class_Head =*/
    {
        sizeof(Audio_Class_Head_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x01, //HEADER descriptor subtype.
        0x0100, //Audio Device compliant to the USB Audio specification version 1.00.
#ifdef _USB_RECORD_
        (sizeof(Audio_Class_Head_t) + (sizeof(Audio_Class_Input_t) * 2) + (sizeof(Audio_Class_Output_t) * 2)
         + sizeof(Audio_Class_Selector_t) + sizeof(Audio_Class_Feature1_t) + sizeof(Audio_Class_Feature2_t)),
        0x02, //The number of AudioStreaming and MIDIStreaming interfaces in the Audio Interface Collection
        //to which this AudioControl interface belongs.
        0x01, //interface number of first AudioStreaming or MIDIStreaming interface in the collection.
        0x02, //interface number of second AudioStreaming or MIDIStreaming interface in the collection.
#else
        (sizeof(Audio_Class_Head_t) + sizeof(Audio_Class_Input_t) + sizeof(Audio_Class_Output_t)
         + sizeof(Audio_Class_Feature1_t)), //total length.
        0x01, //The number of AudioStreaming and MIDIStreaming interfaces in the Audio Interface Collection to which this AudioControl interface belongs.
        0x01, //interface number of first AudioStreaming or MIDIStreaming interface in the collection.
        //0x02,//interface number of second AudioStreaming or MIDIStreaming interface in the collection.
#endif
    },

    /*Audio_Class_Input_t Audio_Class_Input1 =*/
    {
        sizeof(Audio_Class_Input_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x02, //INPUT_TERMINAL  descriptor subtype.
        0x01, //Terminal ID.
        0x0101, //Terminal Type. USB streaming
        0x00, //This Input Terminal has no association.
        0x02, //This Terminal's output audio channel cluster has 2 logical output channels.
        0x0003, //Channel Config bitmap.
        0x00, //Channel Name.
        0x00, //Terminal name.
    },

#ifdef _USB_RECORD_
    /*Audio_Class_Input_t Audio_Class_Input2 =*/
    {
        sizeof(Audio_Class_Input_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x02, //INPUT_TERMINAL  descriptor subtype.
        0x02, //Terminal ID.
        0x0201, //Terminal Type.Microphone
        0x00, //This Input Terminal has no association.
        0x01, //This Terminal's output audio channel cluster has 2 logical output channels.
        0x0001, //Channel Config bitmap.
        0x00, //Channel Name.
        0x00, //Terminal name.
    },
#endif

    /*Audio_Class_Output_t Audio_Class_Output1 =*/
    {
        sizeof(Audio_Class_Output_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x03, //OUTPUT_TERMINAL  descriptor subtype.
        0x06, //Terminal ID.
        0x0301, //Terminal Type.Speaker
        0x00, //This Output Terminal has no association.
        0x09, //ID of the Unit or Terminal to which this Terminal is connected.
        0x00, //Terminal name.
    },
#ifdef _USB_RECORD_

    /*Audio_Class_Output_t Audio_Class_Output2 =*/
    {
        sizeof(Audio_Class_Output_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x03, //OUTPUT_TERMINAL  descriptor subtype.
        0x07, //Terminal ID.
        0x0101, //Terminal Type. USB streaming
        0x00, //This Output Terminal has no association.
        SELECTOR_UNIT_ID, //ID of the Unit or Terminal to which this Terminal is connected.
        0x00, //Terminal name.
    },

    /*Audio_Class_Seclector_t Audio_Class_Selector =*/
    {
        sizeof(Audio_Class_Selector_t),
        0x24, //CS_INTERFACE descriptor type.
        0x05, //SELECTOR_UNIT descriptor subtype.
        SELECTOR_UNIT_ID, //SelectorID
        0x01, //Number of Input Pins of this Unit:p
        0x0a, //bSourceID
        0X00, //index of a string descriptor,describing the select unit
    },
#endif

    /*Audio_Class_Feature1_t Audio_Class_Feature1 =*/
    {
        sizeof(Audio_Class_Feature1_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x06, //FEATURE_UNIT descriptor subtype.
        FEATURE_UNIT_ID1, //Unit ID.
        0x01, //ID of the Unit or Terminal to which this Feature Unit is connected.
        0x01, //Size in bytes of an element of the bmaControls() array.
        0x01, //bmaControls(0), bitmap of master channel 0.
        0x02, //bmaControls(1), bitmap of master channel 1.
        0x02, //bmaControls(2), bitmap of master channel 2.
        0x00, //Feature name.
    },

#ifdef _USB_RECORD_
    /*Audio_Class_Feature2_t Audio_Class_Feature2 =*/
    {
        sizeof(Audio_Class_Feature2_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x06, //FEATURE_UNIT descriptor subtype.
        FEATURE_UNIT_ID2, //Unit ID.
        0x02, //ID of the Unit or Terminal to which this Feature Unit is connected.
        0x01, //Size in bytes of an element of the bmaControls() array.
        0x03, //bmaControls(0), bitmap of master channel 0.
        0x00, //bmaControls(1), bitmap of master channel 1.
        0x00, //Feature name.
    },
#endif

    /*Standard_AS_Interface_Descriptor_t Interface1_Setting0 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        INDEX_INTERFACE_PLAY,//0x01, //interface value
        0x00, //alternative setting
        0x00, //number of endpoint
        0x01, //interface class code, Audio Interface class.
        0x02, //interface sub-class code, AUDIOSTREAMING Subclass.
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*Standard_AS_Interface_Descriptor_t Interface1_Setting1 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        INDEX_INTERFACE_PLAY,//0x01, //interface value
        0x01, //alternative setting
        0x01, //number of endpoint
        0x01, //interface class code, Audio Interface class.
        0x02, //interface sub-class code, AUDIOSTREAMING Subclass.
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*ClassSpecific_AS_Interface_Descriptor_t Audio_Class_General_Setting1 =*/
    {
        sizeof(ClassSpecific_AS_Interface_Descriptor_t), //size of descriptor
        0x24, //descriptor type, CS_INTERFACE .
        0x01, //AS_GENERAL descriptor subtype.
        0x01, //The Terminal ID of the Terminal to which the endpoint of this interface is connected.
        0x01, //Delay introduced by the data path. Expressed in number of frames.
        0x0001, //Format Tag, PCM.
    },

    /*Format_Type_Descriptor_t Audio_Class_Format_Setting1 =*/
    {
        sizeof(Format_Type_Descriptor_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x02, //FORMAT_TYPE descriptor subtype.
        0x01, //FormatType, FORMAT_TYPE .
        0x02, //bNrChannels, Indicates the number of physical channels in the audio data stream.
        0x02, //bSubframeSize, The number of bytes occupied by one audio subframe. Can be 1, 2, 3 or 4.
        0x10, //bBitResolution, The number of effectively used bits from the available bits in an audio subframe.
        0x02, //bSamFreqType, Indicates how the sampling frequency can be programmed.
        { 0x80, 0xbb, 0x00, },
        { 0x44, 0xAC, 0x00, },
    },

    /*Standard_Endpiont_Descriptor_t Endpoint_Descriptor_Setting1 =*/
    {
        sizeof(Standard_Endpiont_Descriptor_t), //size of descriptor
        ENDPOINT, //descriptor type
        0x01, //endpoint address (out endpoint addressed at 2)
        0x09, //endpoint type (ISOCHRONOUS, adaptive)
        ISO_MAX_PACKET_SIZE, //max packet size (192 bytes)
        0x01, //The polling interval value is every 1 Frames.
        0x00, /* unused.  */
        0x00, /* unused.  */
    },

    /*ClassSpecific_Endpiont_Descriptor_t Audio_Data_Endpoint_Setting1 =*/
    {
        sizeof(ClassSpecific_Endpiont_Descriptor_t), //size of descriptor
        0x25, //descriptor type, CS_ENDPOINT
        0x01, //descriptor subtype
        0x01, //bmAttributes, sampling frequency control, no pitch control.
        0x01, //bLockDelayUnits,
        0x0001, //wLockDelay
    },

#ifdef _USB_RECORD_
    /*Standard_AS_Interface_Descriptor_t Interface2_Setting0 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        INDEX_INTERFACE_RECORD,//0x02, //interface value
        0x00, //alternative setting
        0x00, //number of endpoint
        0x01, //interface class code, Audio Interface class.
        0x02, //interface sub-class code, AUDIOSTREAMING Subclass.
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*Standard_AS_Interface_Descriptor_t Interface2_Setting1 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        INDEX_INTERFACE_RECORD,//0x02, //interface value
        0x01, //alternative setting
        0x01, //number of endpoint
        0x01, //interface class code, Audio Interface class.
        0x02, //interface sub-class code, AUDIOSTREAMING Subclass.
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*ClassSpecific_AS_Interface_Descriptor_t Audio_Class_General_Setting2 =*/
    {
        sizeof(ClassSpecific_AS_Interface_Descriptor_t), //size of descriptor
        0x24, //descriptor type, CS_INTERFACE .
        0x01, //AS_GENERAL descriptor subtype.
        0x07, //The Terminal ID of the Terminal to which the endpoint of this interface is connected.
        0x01, //Delay introduced by the data path. Expressed in number of frames.
        0x0001, //Format Tag, PCM.
    },

    /*Format_Type_Descriptor_t Audio_Class_Format_Setting2 =*/
    {
        sizeof(Format_Type_Descriptor_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x02, //FORMAT_TYPE descriptor subtype.
        0x01, //FormatType, FORMAT_TYPE .
        0x01, //bNrChannels, Indicates the number of physical channels in the audio data stream.
        0x02, //bSubframeSize, The number of bytes occupied by one audio subframe. Can be 1, 2, 3 or 4.
        0x10, //bBitResolution, The number of effectively used bits from the available bits in an audio subframe.
        0x02, //bSamFreqType, Indicates how the sampling frequency can be programmed.

        {   0x80, 0xbb, 0x00,},
        {   0x44, 0xAC, 0x00,},
    },

    /*Standard_Endpiont_Descriptor_t Endpoint_Descriptor_Setting2 =*/
    {
        sizeof(Standard_Endpiont_Descriptor_t), //size of descriptor
        ENDPOINT, //descriptor type
        0x81, //endpoint address (in endpoint addressed at 1)
        0x05, //endpoint type (ISOCHRONOUS)
        0x0064, //max iso out packet
        0x01, //The polling interval value is every 1 Frames.
        0x00, /* unused.  */
        0x00, /* unused.  */
    },

    /*ClassSpecific_Endpiont_Descriptor_t Audio_Data_Endpoint_Setting2 =*/
    {
        sizeof(ClassSpecific_Endpiont_Descriptor_t), //size of descriptor
        0x25, //descriptor type, CS_ENDPOINT
        0x01, //descriptor subtype
        0x00, //bmAttributes, sampling frequency control, no pitch control.
        0x00, //bLockDelayUnits,
        0x0000, //wLockDelay
    },
#endif

    /*Standard_AS_Interface_Descriptor_t Interface3_Setting0 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        INDEX_INTERFACE_HID,//0x03, //interface value
        0x00, //alternative setting
        0x01, //number of endpoint
        0x03, //HID Interface Class
        0x00, //
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*Standard_HID_Descriptor_t HID_Descriptor_Setting0 =*/
    {
        sizeof(Standard_Endpiont_Descriptor_t), //size of descriptor
        0x21, //descriptor type
        0x0100, //bcdHID
        0x00, //bCountryCode
        0x01, //bNumDescriptors
        0x22, //bDescriptorType
        0x0021, //wDescriptorLength
    },

    /*Standard_Endpiont_Descriptor1_t Endpiont_Descriptor_Setting3 =*/
    {
        sizeof(Standard_Endpiont_Descriptor1_t), //size of descriptor
        ENDPOINT, //descriptor type
        0x82, //bEndpointAddress:-> Direction: IN - EndpointID: 3
        0x03, //bmAttributes:-> Interrupt Transfer Type
        0x0004, //wMaxPacketSize
        0x20, //bInterval
    },

};
//-----------------------------------------------------------
//end of Configuration

//-----------------------------------------------------------
//start of Configuration
//-----------------------------------------------------------
const device_audio_configuration_sub_t All_Descriptors_of_Configuration2 = {
    /*device_config_descriptor_t Configuration_Descriptor =*/
    {
        sizeof(device_config_descriptor_t), //size of descriptor
        CONFIG, //descriptor type
        sizeof(device_audio_configuration_sub_t), //total length
#ifdef _USB_RECORD_
        0x04,
#else
        0x03, //number of interface = 3,for audio class
#endif
        0x01, //configuration value
        0x00, //configuration string index
        0x80, //attribute (bus powered, remote wakeup disable)
        0x32, //max power (500mA),96h(300mA)
    },

    /*usb_interface_descriptor_t Interface0_Descriptor =*/
    {
        sizeof(usb_interface_descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        INDEX_INTERFACE_CTRL,//0x00, //interface number,
        0x00, //alternative setting
        0x00, //number of endpoint
        0x01, //interface class code, Audio Interface class.
        0x01, //interface sub-class code, AUDIOCONTROL Subclass.
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*Audio_Class_Head_t Audio_Class_Head =*/
    {
        sizeof(Audio_Class_Head_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x01, //HEADER descriptor subtype.
        0x0100, //Audio Device compliant to the USB Audio specification version 1.00.
#ifdef _USB_RECORD_
        (sizeof(Audio_Class_Head_t) + (sizeof(Audio_Class_Input_t) * 2) + (sizeof(Audio_Class_Output_t) * 2)
         + sizeof(Audio_Class_Selector_t) + sizeof(Audio_Class_Feature1_t) + sizeof(Audio_Class_Feature2_t)),
        0x02, //The number of AudioStreaming and MIDIStreaming interfaces in the Audio Interface Collection
        //to which this AudioControl interface belongs.
        0x01, //interface number of first AudioStreaming or MIDIStreaming interface in the collection.
        0x02, //interface number of second AudioStreaming or MIDIStreaming interface in the collection.
#else
        (sizeof(Audio_Class_Head_t) + sizeof(Audio_Class_Input_t) + sizeof(Audio_Class_Output_t)
         + sizeof(Audio_Class_Feature1_t)), //total length.
        0x01, //The number of AudioStreaming and MIDIStreaming interfaces in the Audio Interface Collection to which this AudioControl interface belongs.
        0x01, //interface number of first AudioStreaming or MIDIStreaming interface in the collection.
        //0x02,//interface number of second AudioStreaming or MIDIStreaming interface in the collection.
#endif
    },

    /*Audio_Class_Input_t Audio_Class_Input1 =*/
    {
        sizeof(Audio_Class_Input_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x02, //INPUT_TERMINAL  descriptor subtype.
        0x01, //Terminal ID.
        0x0101, //Terminal Type. USB streaming
        0x00, //This Input Terminal has no association.
        0x02, //This Terminal's output audio channel cluster has 2 logical output channels.
        0x0003, //Channel Config bitmap.
        0x00, //Channel Name.
        0x00, //Terminal name.
    },

#ifdef _USB_RECORD_
    /*Audio_Class_Input_t Audio_Class_Input2 =*/
    {
        sizeof(Audio_Class_Input_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x02, //INPUT_TERMINAL  descriptor subtype.
        0x02, //Terminal ID.
        0x0201, //Terminal Type.Microphone
        0x00, //This Input Terminal has no association.
        0x01, //This Terminal's output audio channel cluster has 2 logical output channels.
        0x0001, //Channel Config bitmap.
        0x00, //Channel Name.
        0x00, //Terminal name.
    },
#endif

    /*Audio_Class_Output_t Audio_Class_Output1 =*/
    {
        sizeof(Audio_Class_Output_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x03, //OUTPUT_TERMINAL  descriptor subtype.
        0x06, //Terminal ID.
        0x0301, //Terminal Type.Speaker
        0x00, //This Output Terminal has no association.
        0x09, //ID of the Unit or Terminal to which this Terminal is connected.
        0x00, //Terminal name.
    },
#ifdef _USB_RECORD_

    /*Audio_Class_Output_t Audio_Class_Output2 =*/
    {
        sizeof(Audio_Class_Output_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x03, //OUTPUT_TERMINAL  descriptor subtype.
        0x07, //Terminal ID.
        0x0101, //Terminal Type. USB streaming
        0x00, //This Output Terminal has no association.
        SELECTOR_UNIT_ID, //ID of the Unit or Terminal to which this Terminal is connected.
        0x00, //Terminal name.
    },

    /*Audio_Class_Seclector_t Audio_Class_Selector =*/
    {
        sizeof(Audio_Class_Selector_t),
        0x24, //CS_INTERFACE descriptor type.
        0x05, //SELECTOR_UNIT descriptor subtype.
        SELECTOR_UNIT_ID, //SelectorID
        0x01, //Number of Input Pins of this Unit:p
        0x0a, //bSourceID
        0X00, //index of a string descriptor,describing the select unit
    },
#endif

    /*Audio_Class_Feature1_t Audio_Class_Feature1 =*/
    {
        sizeof(Audio_Class_Feature1_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x06, //FEATURE_UNIT descriptor subtype.
        FEATURE_UNIT_ID1, //Unit ID.
        0x01, //ID of the Unit or Terminal to which this Feature Unit is connected.
        0x01, //Size in bytes of an element of the bmaControls() array.
        0x01, //bmaControls(0), bitmap of master channel 0.
        0x02, //bmaControls(1), bitmap of master channel 1.
        0x02, //bmaControls(2), bitmap of master channel 2.
        0x00, //Feature name.
    },

#ifdef _USB_RECORD_
    /*Audio_Class_Feature2_t Audio_Class_Feature2 =*/
    {
        sizeof(Audio_Class_Feature2_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x06, //FEATURE_UNIT descriptor subtype.
        FEATURE_UNIT_ID2, //Unit ID.
        0x02, //ID of the Unit or Terminal to which this Feature Unit is connected.
        0x01, //Size in bytes of an element of the bmaControls() array.
        0x03, //bmaControls(0), bitmap of master channel 0.
        0x00, //bmaControls(1), bitmap of master channel 1.
        0x00, //Feature name.
    },
#endif

    /*Standard_AS_Interface_Descriptor_t Interface1_Setting0 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        INDEX_INTERFACE_PLAY,//0x01, //interface value
        0x00, //alternative setting
        0x00, //number of endpoint
        0x01, //interface class code, Audio Interface class.
        0x02, //interface sub-class code, AUDIOSTREAMING Subclass.
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*Standard_AS_Interface_Descriptor_t Interface1_Setting1 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        INDEX_INTERFACE_PLAY,//0x01, //interface value
        0x01, //alternative setting
        0x01, //number of endpoint
        0x01, //interface class code, Audio Interface class.
        0x02, //interface sub-class code, AUDIOSTREAMING Subclass.
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*ClassSpecific_AS_Interface_Descriptor_t Audio_Class_General_Setting1 =*/
    {
        sizeof(ClassSpecific_AS_Interface_Descriptor_t), //size of descriptor
        0x24, //descriptor type, CS_INTERFACE .
        0x01, //AS_GENERAL descriptor subtype.
        0x01, //The Terminal ID of the Terminal to which the endpoint of this interface is connected.
        0x01, //Delay introduced by the data path. Expressed in number of frames.
        0x0001, //Format Tag, PCM.
    },

    /*Format_Type_Descriptor_t Audio_Class_Format_Setting1 =*/
    {
        sizeof(Format_Type_Descriptor_sub_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x02, //FORMAT_TYPE descriptor subtype.
        0x01, //FormatType, FORMAT_TYPE .
        0x02, //bNrChannels, Indicates the number of physical channels in the audio data stream.
        0x02, //bSubframeSize, The number of bytes occupied by one audio subframe. Can be 1, 2, 3 or 4.
        0x10, //bBitResolution, The number of effectively used bits from the available bits in an audio subframe.
        0x01, //bSamFreqType, Indicates how the sampling frequency can be programmed.
        { 0x80, 0xbb, 0x00, },
    },

    /*Standard_Endpiont_Descriptor_t Endpoint_Descriptor_Setting1 =*/
    {
        sizeof(Standard_Endpiont_Descriptor_t), //size of descriptor
        ENDPOINT, //descriptor type
        0x01, //endpoint address (out endpoint addressed at 2)
        0x09, //endpoint type (ISOCHRONOUS, adaptive)
        ISO_MAX_PACKET_SIZE, //max packet size (192 bytes)
        0x01, //The polling interval value is every 1 Frames.
        0x00, /* unused.  */
        0x00, /* unused.  */
    },

    /*ClassSpecific_Endpiont_Descriptor_t Audio_Data_Endpoint_Setting1 =*/
    {
        sizeof(ClassSpecific_Endpiont_Descriptor_t), //size of descriptor
        0x25, //descriptor type, CS_ENDPOINT
        0x01, //descriptor subtype
        0x01, //bmAttributes, sampling frequency control, no pitch control.
        0x01, //bLockDelayUnits,
        0x0001, //wLockDelay
    },

#ifdef _USB_RECORD_
    /*Standard_AS_Interface_Descriptor_t Interface2_Setting0 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        INDEX_INTERFACE_RECORD,//0x02, //interface value
        0x00, //alternative setting
        0x00, //number of endpoint
        0x01, //interface class code, Audio Interface class.
        0x02, //interface sub-class code, AUDIOSTREAMING Subclass.
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*Standard_AS_Interface_Descriptor_t Interface2_Setting1 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        INDEX_INTERFACE_RECORD,//0x02, //interface value
        0x01, //alternative setting
        0x01, //number of endpoint
        0x01, //interface class code, Audio Interface class.
        0x02, //interface sub-class code, AUDIOSTREAMING Subclass.
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*ClassSpecific_AS_Interface_Descriptor_t Audio_Class_General_Setting2 =*/
    {
        sizeof(ClassSpecific_AS_Interface_Descriptor_t), //size of descriptor
        0x24, //descriptor type, CS_INTERFACE .
        0x01, //AS_GENERAL descriptor subtype.
        0x07, //The Terminal ID of the Terminal to which the endpoint of this interface is connected.
        0x01, //Delay introduced by the data path. Expressed in number of frames.
        0x0001, //Format Tag, PCM.
    },

    /*Format_Type_Descriptor_t Audio_Class_Format_Setting2 =*/
    {
        sizeof(Format_Type_Descriptor_sub_t),
        0x24, //descriptor type, CS_INTERFACE .
        0x02, //FORMAT_TYPE descriptor subtype.
        0x01, //FormatType, FORMAT_TYPE .
        0x01, //bNrChannels, Indicates the number of physical channels in the audio data stream.
        0x02, //bSubframeSize, The number of bytes occupied by one audio subframe. Can be 1, 2, 3 or 4.
        0x10, //bBitResolution, The number of effectively used bits from the available bits in an audio subframe.
        0x01, //bSamFreqType, Indicates how the sampling frequency can be programmed.

        {   0x80, 0xbb, 0x00,},
    },

    /*Standard_Endpiont_Descriptor_t Endpoint_Descriptor_Setting2 =*/
    {
        sizeof(Standard_Endpiont_Descriptor_t), //size of descriptor
        ENDPOINT, //descriptor type
        0x81, //endpoint address (in endpoint addressed at 1)
        0x05, //endpoint type (ISOCHRONOUS)
        0x0064, //max iso out packet
        0x01, //The polling interval value is every 1 Frames.
        0x00, /* unused.  */
        0x00, /* unused.  */
    },

    /*ClassSpecific_Endpiont_Descriptor_t Audio_Data_Endpoint_Setting2 =*/
    {
        sizeof(ClassSpecific_Endpiont_Descriptor_t), //size of descriptor
        0x25, //descriptor type, CS_ENDPOINT
        0x01, //descriptor subtype
        0x00, //bmAttributes, sampling frequency control, no pitch control.
        0x00, //bLockDelayUnits,
        0x0000, //wLockDelay
    },
#endif

    /*Standard_AS_Interface_Descriptor_t Interface3_Setting0 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        INDEX_INTERFACE_HID,//0x03, //interface value
        0x00, //alternative setting
        0x01, //number of endpoint
        0x03, //HID Interface Class
        0x00, //
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*Standard_HID_Descriptor_t HID_Descriptor_Setting0 =*/
    {
        sizeof(Standard_Endpiont_Descriptor_t), //size of descriptor
        0x21, //descriptor type
        0x0100, //bcdHID
        0x00, //bCountryCode
        0x01, //bNumDescriptors
        0x22, //bDescriptorType
        0x0021, //wDescriptorLength
    },

    /*Standard_Endpiont_Descriptor1_t Endpiont_Descriptor_Setting3 =*/
    {
        sizeof(Standard_Endpiont_Descriptor1_t), //size of descriptor
        ENDPOINT, //descriptor type
        0x82, //bEndpointAddress:-> Direction: IN - EndpointID: 3
        0x03, //bmAttributes:-> Interrupt Transfer Type
        0x0004, //wMaxPacketSize
        0x20, //bInterval
    },

};
//-----------------------------------------------------------
//end of Configuration

