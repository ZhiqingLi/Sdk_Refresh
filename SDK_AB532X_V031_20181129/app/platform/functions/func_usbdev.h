#ifndef _FUNC_USBDEV_H
#define _FUNC_USBDEV_H

#define UDE_C_VU                (0 << 8)    //V+
#define UDE_C_VD                (1 << 8)    //V-
#define UDE_C_PP                (2 << 8)    //PP
#define UDE_C_NT                (3 << 8)    //NEXT
#define UDE_C_PV                (4 << 8)    //PREV

//USB Device HID Comsumer usage id
#define UDE_HID_FAST_FORWARD    BIT(0)                  //0xb3
#define UDE_HID_NEXTFILE        UDE_C_NT | BIT(1)       //0xb5
#define UDE_HID_PREVFILE        UDE_C_PV | BIT(2)       //0xb6
#define UDE_HID_STOP            BIT(3)                  //0xb7
#define UDE_HID_PLAYPAUSE       UDE_C_PP | BIT(4)       //0xcd
#define UDE_HID_MUTE            BIT(5)                  //0xe2
#define UDE_HID_VOLUP           UDE_C_VU | BIT(6)       //0xe9
#define UDE_HID_VOLDOWN         UDE_C_VD | BIT(7)       //0xea

typedef struct {
    u8 vol;
    u8 cur_dev;
    u8 dev_change;
} f_ude_t;
extern f_ude_t f_ude;

void func_usbdev_message(u16 msg);
void ude_sdcard_switch(u8 dev);
void ude_sdcard_change_process(void);

#if (GUI_SELECT != GUI_NO)
void func_usbdev_display(void);
void func_usbdev_enter_display(void);
void func_usbdev_exit_display(void);
#else
#define func_usbdev_display()
#define func_usbdev_enter_display()
#define func_usbdev_exit_display()
#endif

#endif // _FUNC_USBDEV_H
