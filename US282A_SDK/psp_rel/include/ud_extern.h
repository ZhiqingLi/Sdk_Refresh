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
#ifndef __USOUND_EXTERN_H__
#define __USOUND_EXTERN_H__

#define USB_IDLE         0x00
#define PLAY_RUNNING     0x01
#define RECORD_RUNNING   0x02
#define USOUND_PLUG_OUT  0x04


extern uint8 usb_state;
extern uint8 card_detect_state;
extern uint8 install_card_flag;
extern uint32 timer_id;
extern uint8 SwitchToAdfuFlag;
extern uint8 UsbIdleCounter;
extern uint8 UpOrDownLoad;
extern uint8 usb_reset_count;
extern uint8 usb_state;
extern uint8 suspend_flag;
extern uint8 hid_report;
extern uint8 card_exist_flag;
extern uint8 hid_change;
extern uint16 hid_idle_time;
extern uint32 play_sample;
extern uint8 set_sample_flag; 
extern uint8 mic_config_val;
//extern uint8 g_packet_cnt;
extern uint8 g_start_play;
extern int32 asrc_adjust_timer;
extern asrc_adjust_record_t asrc_out1_record;

extern void usb_device_exit(void) __FAR__;
extern void usb_device_init(void) __FAR__;
extern void msc_cmd_process(void) __FAR__;
extern void set_auip_vol(uint8 set_type) __FAR__;
extern void deal_card_out(void) __FAR__;
extern uint8 *get_usb_data_pointer(uint8 index);
extern void init_channel_record(asrc_adjust_record_t *pCh) __FAR__;

extern void deal_timer_process(void) __FAR__;

extern void card_cap_check(void) __FAR__;

#endif 
