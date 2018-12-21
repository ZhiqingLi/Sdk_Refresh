#include "include.h"
#include "func.h"
#include "func_idle.h"

#if FUNC_USBDEV_EN
AT(.text.func.usbdev.msg)
void func_usbdev_message(u16 msg)
{
    switch (msg) {
        case KU_PLAY:
        case KU_PLAY_POWER:
        case KU_PLAY_MODE:
            usb_device_hid_send(UDE_HID_PLAYPAUSE);
            break;

        case KL_PLAY:
        case KU_MUTE:
            if (sys_cb.mute) {
                bsp_sys_unmute();
            } else {
                bsp_sys_mute();
            }
            usb_device_hid_send(UDE_HID_MUTE);
            break;

        case KU_PREV_VOL_DOWN:
        case KL_VOL_DOWN_PREV:
        case KU_PREV:
            usb_device_hid_send(UDE_HID_PREVFILE);
            break;

        case KU_NEXT_VOL_UP:
        case KL_VOL_UP_NEXT:
        case KU_NEXT:
            usb_device_hid_send(UDE_HID_NEXTFILE);
            break;

        case KU_VOL_DOWN:
        case KL_VOL_DOWN:
        case KH_VOL_DOWN:
            usb_device_hid_send(UDE_HID_VOLDOWN);
            break;

        case KU_VOL_UP:
        case KL_VOL_UP:
        case KH_VOL_UP:
            usb_device_hid_send(UDE_HID_VOLUP);
            break;

        case KU_STOP:
            usb_device_hid_send(UDE_HID_STOP);
            break;

#if UDE_STORAGE_EN
        case EVT_SD_INSERT:
            ude_sdcard_switch(DEV_SDCARD);
            break;

        case EVT_SD_REMOVE:
    #if MUSIC_SDCARD1_EN
            if ((f_ude.cur_dev == DEV_SDCARD) && dev_is_online(DEV_SDCARD1)) {
                ude_sdcard_switch(DEV_SDCARD1);
            }
    #endif // MUSIC_SDCARD1_EN
            break;

    #if MUSIC_SDCARD1_EN
        case EVT_SD1_INSERT:
            ude_sdcard_switch(DEV_SDCARD1);
            break;

        case EVT_SD1_REMOVE:
            if ((f_ude.cur_dev == DEV_SDCARD1) && dev_is_online(DEV_SDCARD)) {
                ude_sdcard_switch(DEV_SDCARD);
            }
            break;

        case MSG_SYS_1S:
            ude_sdcard_change_process();
            break;
    #endif // MUSIC_SDCARD1_EN
#endif // UDE_STORAGE_EN

        case EVT_PC_REMOVE:
            func_cb.sta = func_cb.last;
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif
