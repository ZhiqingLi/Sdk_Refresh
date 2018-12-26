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
uint8 __section__(".data.buf_ram") g_play_buf[PLAY_BUFF_SIZE] =
{   0};
uint8 __section__(".data.buf_ram_convert") g_convert_buf[CONVERT_BUFF_SIZE] =
{   0};


//for buffer address set
//uint8 __section__(".data.buf_ram") g_record_buf[RECORD_BUFF_SIZE] = {0};

//uint8 pa_mute = 0;
uint32 g_volume_flag;

//for volume chg
uint32 g_volume_chg = 0;

//uint8 irq_type = 0,
//uint8 g_mic_config_val;

uint8 g_current_mute_ch0 = 0;

uint8 g_cur_selector = 0;

uint8 g_start_play;

uint8 g_set_sample_flag = 0;

uint8 g_ud_stop_by_tts = 0;

//uint32 timer_id = 0;
uint32 g_usb_state = 0;

uint16 g_volume_value = 0;

uint16 g_current_volume_ch0 = 0;

uint16 g_current_volume_ch1 = 0;

uint16 g_current_volume_ch2 = 0;

uint32 g_play_sample = 44100;

int32 g_asrc_adjust_timer = -1;

uint32 g_wr_play_ptr = 0;

uint32 g_rd_play_ptr = 0;

int32 g_play_data_count = 0;

uint32 g_no_data_count = 0;

uint32 g_manul_stop = 0;
device_requests_t g_setup_data;

//uint8 start_record_flag = 0;

uint8 g_usb_vid[6] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

uint8 g_usb_pid[6] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

uint8 g_usb_bcd[6] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

uint8 g_count = 0;
uint8 g_clear_rate[3] = {0};

uint8 g_hid_report = 0;

uint8 g_hid_change = 0;

uint16 g_low_energy_count = 0;

uint16 g_energy_count = 0;

uint8 tmp_play_status = 0;

uint16 g_hid_idle_time = 0;

uint8 g_pc_switch_flag = 0;

uint8 g_mute_flag;

uint8 g_mute_delay;

uint8 g_datain_flag;

uint8 g_hid_Descriptor[HID_REPORT_DESC_LEN] = {
    0x05, 0x0c, //USAGE_PAGE (Consumer)
    0x09, 0x01, //USAGE (Consumer Control)
    0xa1, 0x01, //COLLECTION (Application)
    0x15, 0x00, //Logical Minimum (0x00)
    0x25, 0x01, //Logical Maximum (0x01)
    0x09, 0xe9, //USAGE (Volume Up)
    0x09, 0xea, //USAGE (Volume Down)
    0x09, 0xe2, //USAGE (Mute)
    0x09, 0xcd, //USAGE (Play/Pause)
    0x09, 0xb5, //USAGE (Scan Next Track)
    0x09, 0xb6, //USAGE (Scan Previous Track)
    0x09, 0xb3, //USAGE (Fast Forward)
    0x09, 0xb7, //USAGE (Stop)
    0x75, 0x01, //Report Size (0x01)
    0x95, 0x08, //Report Count (0x08),报告的个数为8，即总共有8个bits
    0x81, 0x42, //Input (Data,Variable,Absolute,No Wrap,Linear,Preferred State,Null state)
    0xc0, //END_COLLECTION           //END_COLLECTION
    0xa1, 0x01, //COLLECTION (Application)
    0x15, 0x00, //Logical Minimum (0x00)
    0x25, 0x01, //Logical Maximum (0x01)
    0x09, 0x08,
    0x09, 0x0f,
    0x09, 0x18,
    0x09, 0x1f,
    0xc0, //END_COLLECTION           //END_COLLECTION
};

device_descriptor_t Device_Descriptor = {
    sizeof(device_descriptor_t), //size of descriptor
    DEVICE, //descriptor type
    0x0100, //USB release version 2.0
    0x00, //class code
    0x00, //sub-class code
    0x00, //protocol code
    0x40, //max packet size = 64byte
    0x10d6, //vendor ID
    0xdd00, //product ID
    0x0100, //device release number
    0x01, //index of string descriptor of manufacturer
    0x02, //index of string descriptor of product
    0x03, //index of string descriptor of serial number
    0x01, //number of possible configuration
};

//-----------------------------------------------------------
//start of Configuration
//-----------------------------------------------------------
device_audio_configuration_t All_Descriptors_of_Configuration = {

    /*device_config_descriptor_t Configuration_Descriptor =*/
    {
        sizeof(device_config_descriptor_t), //size of descriptor
        CONFIG, //descriptor type
        sizeof(device_audio_configuration_t), //total length

        0x03, //number of interface = 3,for audio class

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

        (sizeof(Audio_Class_Head_t) + sizeof(Audio_Class_Input_t) + sizeof(Audio_Class_Output_t)
         + sizeof(Audio_Class_Feature1_t)), //total length.
        0x01, //The number of AudioStreaming and MIDIStreaming interfaces in the Audio Interface Collection
        //to which this AudioControl interface belongs.
        0x01, //interface number of first AudioStreaming or MIDIStreaming interface in the collection.
        //0x02,//interface number of second AudioStreaming or MIDIStreaming interface in the collection.

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
        0x00, //bmaControls(1), bitmap of master channel 1.
    },

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
        0x03, //每一帧所占字节数，24BIT对应3字节
        0x18, //每一个音频采样点所占字节数，24BIT即为0x18
        0x03, //bSamFreqType, Indicates how the sampling frequency can be programmed.
        { 0x44, 0xAC, 0x00, },       //44.1k
        { 0x80, 0xbb, 0x00, },       //48k
        { 0x00, 0x77, 0x01, },       //96k
    },

    /*Standard_Endpiont_Descriptor_t Endpoint_Descriptor_Setting1 =*/
    {
        sizeof(Standard_Endpiont_Descriptor_t), //size of descriptor
        ENDPOINT, //descriptor type
        0x01, //endpoint address (out endpoint addressed at 2)
        0x09, //endpoint type (ISOCHRONOUS, adaptive)
        ISO_MAX_PACKET_SIZE, //max packet size
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

    /*Standard_AS_Interface_Descriptor_t Interface3_Setting0 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        INDEX_INTERFACE_HID,//0x02, //interface value
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

string_language_id_t language_string = {
    0x04, /* length*/
    0x03, /* Second Byte of this descriptor.  */
    0x0409, /* LANGID code array, US english,0x409 */
};

string_product_t device_name_string = {
    0x22, 0x03, 'U', 'S', 'B', ' ', 'A', 'u', 'd', 'i', 'o',
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
};

string_manufacturer_t manufacure_string = {
    0x12, 0x03, 'A', 'c', 't', 'i', 'o', 'n', 's',
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

serialnum_descriptor_t SerialNumber = {
    0x12, 0x03, 'U', '2', 'd', 'c', '0', 'z', '7',
    'w', 's', '5', '3', 'a', '7', '9', 'r', 'n', 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

