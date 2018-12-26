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
#ifndef __UD_INTERN_H__
#define __UD_INTERN_H__

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
#define    NOERR                 0
#define    CMDRESET              1
#define    MEDIAPROTECT          2
#define    RESETUDISK            3
#define    MEDIANOTPRESENT       4
#define    INVALIDCMD            5
#define    INVALIDFIELD          6

#define    USB_IS_CONNECT        0x00
#define    USB_CONNECT_RESET     0x01
#define    USB_DISCONNECT        0x02

//set disconnect
#define     SET_DISCONNECT()  act_writeb((1<<6)|act_readb(UsbCTRL_STUS),UsbCTRL_STUS)
//set connect
#define     SET_CONNECT()     act_writeb(act_readb(UsbCTRL_STUS)|~(1<<6),UsbCTRL_STUS)
//#define     clear_watchdog()    act_writel((0x00000001|act_readl(WD_CTL)),WD_CTL)
//check connect
#ifndef _FPGA_VERIFY_
#define     USB_CONNECT_STATUS (act_readb(DPDMCTRL)&(1<<6))
#else
#define     USB_CONNECT_STATUS ((uint8)((act_readb(LINESTATUS)&0x18) != 0x18))
#endif
#define     WRITE_DATA_IN_FIFO(x) \
                act_writeb((x),Fifo1dat)
//set busy status ep
#define     SET_EP_BUSY()   \
                act_writeb(act_readb(In1cs_hcout1cs)|0x02,In1cs_hcout1cs)

extern device_hid_configuration_t All_Descriptors_of_Configuration;

extern uint8 g_hid_descriptor[REPORT_DESCRIPTOR_SIZE];

extern device_descriptor_t Device_Descriptor;

extern serialnum_descriptor_t SerialNumber;

extern string_manufacturer_t manufacure_string;

extern string_product_t device_name_string;

extern string_language_id_t language_string;

extern device_requests_t g_setup_data;

extern uint8 g_usb_vid[6];

extern uint8 g_usb_pid[6];

extern uint16 g_interface_status;

extern uint8 g_alter_setting;

extern uint8 g_usb_state;

extern uint32 g_hid_change;

extern uint8 g_hid_in_buf[HID_REPORT_SIZE];

extern uint8 g_hid_out_buf[HID_REPORT_SIZE];

extern void deal_setup(void)__FAR__;

extern void descriptor_init(void) __FAR__;

extern void access_usbfifo_by_cpu(uint8 epsel, uint32 *address, uint32 data_lenth) __FAR__;

extern void uhid_isr(void) __FAR__;

extern void usb_hid_init(void) __FAR__;

extern void usb_hid_exit(void) __FAR__;

extern int set_usb_cfg_param(uint32 index, uint32 *cfg_info, uint32 cfg_len) __FAR__;

extern int uhid_inner_get_status(uhid_status_t *pstatus) __FAR__;

extern int uhid_inner_module_start(void) __FAR__;

extern int uhid_inner_module_stop(void) __FAR__;

extern int uhid_inner_set_cfg(uint32 cfg_index, uint32 *pcfg, uint32 cfg_len) __FAR__;

extern int uhid_inner_hid_get(void) __FAR__;

extern int uhid_inner_hid_set(void) __FAR__;

extern int uhid_inner_set_cmd(uint32 cmd, uint32 cmd_param) __FAR__;

extern int uhid_inner_hid_deal(void) __FAR__;

#endif //__UD_INTERN_H__
