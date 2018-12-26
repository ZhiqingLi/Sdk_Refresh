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
#ifndef __USOUND_INTERN_H__
#define __USOUND_INTERN_H__

#include "ud_setup.h"
#include "irq.h"
#include "typeext.h"
#include "kernel_interface.h"
#include "storage_device.h"
#include "driver_manager.h"
#include "libc_interface.h"
#include "pm.h"
#include "vm_fwsp_def.h"
#include "sysdef.h"
#include "sys_board_def.h"

#include "audio_device.h"
#include "ud_interface.h"

//define ep num
#define     EP0                 0
#define     EP1IN               1
#define     EP1OUT              2
#define     EP2IN               3
#define     EP2OUT              4

//define err type
#define    NOERR                0
#define    CMDRESET             1
#define    MEDIAPROTECT         2
#define    RESETUDISK           3
#define    MEDIANOTPRESENT      4
#define    INVALIDCMD           5
#define    INVALIDFIELD         6

#define    HID_REPORT_DESC_LEN  48
//#define    PLAY_BUFF_START      0x34000 //front enhance (4k)播放和录音均使用2K buff
//#define    RECORD_BUFF_START    0x34800
#define    PLAY_BUFF_SIZE       0x1000
#define    CONVERT_BUFF_SIZE     0x3000

#define    USB_IS_CONNECT       0x00
#define    USB_CONNECT_RESET    0x01
#define    USB_DISCONNECT       0x02

#define    RECORD_PACKET_SIZE   96

//set disconnect
#define     SET_DISCONNECT()  act_writeb((1<<6)|act_readb(UsbCTRL_STUS),UsbCTRL_STUS)
//set connect
#define     SET_CONNECT()     act_writeb(act_readb(UsbCTRL_STUS)|~(1<<6),UsbCTRL_STUS)
//#define     clear_watchdog()    act_writel((0x00000001|act_readl(WD_CTL)),WD_CTL)

//check connect
//#if((SYS_BOARD_TYPE == FOR_FPGA)||(SYS_BOARD_TYPE >= FOR_IC_EVB))
#if(SYS_BOARD_TYPE == FOR_FPGA)

#define     USB_CONNECT_STATUS 1

#else

//#define     USB_CONNECT_STATUS (act_readb(DPDMCTRL)&(1<<6))
#define     USB_CONNECT_STATUS (act_readb(Usbirq_hcusbirq)&(1<<Usbirq_hcusbirq_NTRIRQ))

#endif

//for buffer address set
extern uint8 g_play_buf[PLAY_BUFF_SIZE];
extern uint8 g_convert_buf[CONVERT_BUFF_SIZE];


//for buffer address set
//extern uint8 g_record_buf[RECORD_BUFF_SIZE];

//extern uint32 prev_freq;
//extern uint8 g_mic_config_val;

//for volume chg
extern uint32 g_volume_chg;

extern uint32 g_wr_play_ptr;

extern uint32 g_rd_play_ptr;

extern int32 g_play_data_count;

extern uint32 g_no_data_count;

extern uint32 g_manul_stop;

extern uint16 g_volume_value;

extern uint16 g_current_volume_ch0;

extern uint16 g_current_volume_ch1;

extern uint16 g_current_volume_ch2;

extern uint8 g_current_mute_ch0;

extern uint32 g_play_sample;

extern uint8 g_cur_selector;

extern uint8 g_set_sample_flag;

extern uint8 g_ud_stop_by_tts;

extern int32 g_asrc_adjust_timer;

extern uint8 g_start_play;

extern device_requests_t g_setup_data;

extern device_audio_configuration_t All_Descriptors_of_Configuration;

extern uint8 g_hid_Descriptor[HID_REPORT_DESC_LEN];

extern device_descriptor_t Device_Descriptor;

extern serialnum_descriptor_t SerialNumber;

extern string_manufacturer_t manufacure_string;

extern string_product_t device_name_string;

extern string_language_id_t language_string;

extern uint8 g_usb_vid[6];

extern uint8 g_usb_pid[6];

extern uint8 g_usb_bcd[6];

extern uint8 g_count;
extern uint8 g_clear_rate[3];

//extern uint8 wav_head[44];
extern uint32 g_usb_state;

extern uint32 g_volume_flag;

extern uint8 g_hid_report;

extern uint8 g_hid_change;

extern uint16 g_hid_idle_time;

extern uint16 g_low_energy_count;

extern uint16 g_energy_count;

extern uint8 tmp_play_status;

extern uint8 g_pc_switch_flag;

extern uint8 g_mute_flag;

extern uint8 g_mute_delay;

extern uint8 g_datain_flag;

extern int set_usb_cfg_param(uint32 index, uint32 *cfg_info, uint32 cfg_len) __FAR__;

extern void deal_setup(void)__FAR__;

extern void usound_isr(void) __FAR__;

extern void usound_isr_cbk(void) __FAR__;

extern void descriptor_init(void) __FAR__;

extern void reset_data_pos(void) __FAR__;

//extern void usb_reset_isr(void) __FAR__;

extern void usb_disconnect_isr(void) __FAR__;

//extern void iso_out_token_isr(void) __FAR__;

extern void hid_token_check(void) __FAR__;

extern void access_usbfifo_by_cpu(uint8 epsel, uint32 *address, uint32 data_lenth) __FAR__;

typedef void (*isr_cbk)(void);
extern void usb_sound_init(isr_cbk *p_isr_cbk) __FAR__;

extern void usb_device_exit(void) __FAR__;

extern int usound_inner_get_status(usound_status_t *pstatus) __FAR__;

extern int usound_inner_module_start(isr_cbk *p_isr_cbk) __FAR__;

extern int usound_inner_module_stop(void) __FAR__;

extern int usound_inner_set_cfg(uint32 cfg_index, uint32 *pcfg, uint32 cfg_len) __FAR__;

extern int usound_inner_get_info(uint32 info_index, uint32 *pinfor, uint32 *info_len) __FAR__;

extern int usound_inner_set_info(uint32 info_index, uint32 *pinfor, uint32 info_len) __FAR__;

extern int usound_inner_set_cmd(uint32 cmd, uint32 cmd_param) __FAR__;

extern int usound_inner_hid_deal(void) __FAR__;

extern void dac_start(void);

extern void dac_stop(void);

extern void set_dac_sample_rate(uint32 sample_rate);

extern void cap_dac_dma_isr(void);

extern void cap_dac_dma_cfg(void);

extern void cap_dac_dma_stop(void);
#endif

