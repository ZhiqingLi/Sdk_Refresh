#ifndef _API_USB_H
#define _API_USB_H

enum {
    USBCHK_ONLY_HOST,
    USBCHK_ONLY_DEVICE,
    USBCHK_OTG,
};

enum {
    USB_DISCONNECT,
    USB_UDISK_CONNECTED,
    USB_PC_CONNECTED,
};

void usb_init(void);
void usb_disable(void);
u8 usbchk_connect(u8 mode);
void udisk_remove(void);
void udisk_insert(void);

void usb_device_enter(u8 enum_type);
void usb_device_exit(void);
void usb_device_process(void);
void ude_tmr_isr(void);
void pc_remove(void);
bool usb_device_hid_send(u16 hid_val);
#endif // _API_USB_H
