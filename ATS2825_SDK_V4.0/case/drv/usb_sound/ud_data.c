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
uint8 __section__(".ply_ram.data") g_play_buf[PLAY_BUFF_SIZE] =
{   0};

#ifdef _USB_RECORD_
//for buffer address set
uint8 __section__(".rec_ram.data") g_record_buf[RECORD_BUFF_SIZE] =
{   0};

//for buffer address set
uint8 __section__(".rec_ram.data") g_rec_no_up[RECORD_BUFF_SIZE] =
{   0};
#endif

//uint8 pa_mute = 0;
uint32 g_volume_flag;

//for volume chg
uint32 g_volume_chg = 0;

//uint8 irq_type = 0,
//uint8 g_mic_config_val;

uint8 g_current_mute_ch0 = 0;

uint8 g_cur_selector = 0;

uint8 g_start_play;
uint8 tmp_play_status = 0;

uint8 g_set_sample_flag = 0;
uint8 g_set_adc_sample_flag = 0;
uint8 g_set_dac_sample_flag = 0;
//FOR esd
uint8 g_no_empty_count = 0;

//uint32 timer_id = 0;
uint32 g_usb_state = 0;

uint16 g_volume_value = 0;

uint16 g_current_volume_ch0 = 0;

uint16 g_current_volume_ch1 = 0;

uint16 g_current_volume_ch2 = 0;

uint32 g_play_sample = 44100;

uint32 g_adc_sample = 44100;

uint32 g_dac_sample = 44100;

int32 g_asrc_adjust_timer = -1;

uint32 g_wr_play_ptr = 0;

uint32 g_rd_play_ptr = 0;

uint32 g_play_data_count = 0;

uint32 g_no_data_count = 0;

uint32 g_manul_stop = 0;
uint32 low_energy = 0;
#ifdef _USB_RECORD_

uint32 g_start_record = 0;

uint32 g_wr_record_ptr = 0;

uint32 g_rd_record_ptr = 0;

uint32 g_record_data_count = 0;

uint32 g_record_packet_size = 88;

#endif
uint8 g_fix_sample_flg = 0;

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
uint8 g_set_rate[3] = {0x80, 0xbb, 0x00,};

uint8  g_asrc_adj_flag;

uint8 g_mute_flag;

uint8 g_mute_delay;

uint8 g_datain_flag;

uint8 g_hid_report = 0;

uint8 g_hid_change = 0;

uint16 g_hid_idle_time = 0;

uint8 g_tts_start_flag = 0;

uint8 g_pc_switch_flag = 0;

uint8 g_reset_flag;

uint8 g_set_adc_enable = 0;

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
uint8 audio_descriptors_of_configuration[sizeof(device_audio_configuration_t)];

//void* g_audio_config_data_ptr;

uint8 g_audio_config_data_size;

uint8 g_audio_sample_set;
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
    0x12, 0x03, 'U', '2', 'd', 'c', '0', 'z',
    '7', 'w', 's', '5', '3', 'a', '7', '9', 'r', 'n',
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

