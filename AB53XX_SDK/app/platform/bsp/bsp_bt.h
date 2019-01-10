#ifndef _BSP_BT_H
#define _BSP_BT_H

//标准HID键
#define HID_KEY_ENTER       0x28

//自定义HID键
#define HID_KEY_VOL_DOWN    0x00EA
#define HID_KEY_VOL_UP      0x00E9

void bsp_bt_init(void);
void bsp_bt_close(void);
void bsp_bt_vol_change(void);
void bsp_bt_hid_photo(void);
bool bsp_bt_pwrkey5s_check(void);
void bsp_bt_pwrkey5s_clr(void);
void bsp_bt_hid_tog_conn(void);
const char *bt_get_local_name(void);

#endif //_BSP_BT_H
