#include "include.h"
#include "func.h"
#include "func_bt_hid.h"

AT(.com_text.bthid)
bool is_bthid_mode(void)
{
    if ((sys_cb.bthid_mode) && (xcfg_cb.func_bthid_en)) {
        return true;
    }
    return false;
}

#if FUNC_BTHID_EN

void func_bt_hid_warning(void)
{
    if(func_bt_chkclr_warning(BT_WARN_DISCON)) {
#if WARNING_BT_DISCONNECT
    #if (FUNC_BTHID_EN && WARNING_BTHID_CONN)
        mp3_res_play(RES_BUF_DISCON_HID_MP3, RES_LEN_DISCON_HID_MP3);
    #else
        mp3_res_play(RES_BUF_DISCONNECT_MP3, RES_LEN_DISCONNECT_MP3);
    #endif // FUNC_BTHID_EN
#endif // WARNING_BT_DISCONNECT

#if WARNING_BT_WAIT_CONNECT
    #if !(FUNC_BTHID_EN && WARNING_BTHID_CONN)
        mp3_res_play(RES_BUF_WAIT4CONN_MP3, RES_LEN_WAIT4CONN_MP3);
    #endif // FUNC_BTHID_EN
#endif // WARNING_BT_WAIT_CONNECT
    }

	if(func_bt_chkclr_warning(BT_WARN_CON)) {
#if WARNING_BT_CONNECT
    #if (FUNC_BTHID_EN && WARNING_BTHID_CONN)
        mp3_res_play(RES_BUF_CONN_HID_MP3, RES_LEN_CONN_HID_MP3);
    #else
        mp3_res_play(RES_BUF_CONNECTED_MP3, RES_LEN_CONNECTED_MP3);
    #endif // FUNC_BTHID_EN
#endif
    }
}

AT(.text.func.bt)
static void func_bthid_process(void)
{
    func_process();
    func_bt_status();

    if(sleep_process(bt_is_sleep)) {
        f_bt.disp_status = 0xff;
    }
}

AT(.text.func.bt)
static void func_bthid_enter(void)
{
    sys_cb.bthid_mode = 1;
    dis_auto_pwroff();
    func_cb.mp3_res_play = mp3_res_play;
    led_bt_init();
    func_bt_enter_display();

#if WARNING_FUNC_BT
    mp3_res_play(RES_BUF_CAMERA_MODE_MP3, RES_LEN_CAMERA_MODE_MP3);
#endif // WARNING_FUNC_BT

#if !BT_BACKSTAGE_EN
    bsp_bt_init();
#endif
    f_bt.disp_status = 0xff;

#if BT_PWRKEY_5S_DISCOVER_EN
    if(bsp_bt_pwrkey5s_check()) {
        func_bt_disp_status();
        func_bt_mp3_res_play(RES_BUF_PAIRING_MP3, RES_LEN_PAIRING_MP3);
    } else {
        func_bt_disp_status();
#if WARNING_BT_PAIR
        if (xcfg_cb.warning_bt_pair) {
            func_bt_mp3_res_play(RES_BUF_PAIRING_MP3, RES_LEN_PAIRING_MP3);
        }
#endif // WARNING_BT_PAIR
    }
#endif
}

AT(.text.func.bt)
static void func_bthid_exit(void)
{
    dac_fade_out();
    func_bt_exit_display();
    bt_disconnect();
    bt_off();
    func_cb.last = FUNC_BTHID;
    sys_cb.bthid_mode = 0;
}

AT(.text.func.bt)
void func_bthid(void)
{
    if (!xcfg_cb.func_bthid_en) {
        func_cb.sta = FUNC_NULL;
        func_cb.last = FUNC_BTHID;
        return;
    }

    printf("%s\n", __func__);

    func_bthid_enter();

    while (func_cb.sta == FUNC_BTHID) {
        func_bthid_process();
        func_bthid_message(msg_dequeue());
        func_bt_display();
    }

    func_bthid_exit();
}

#endif //FUNC_BTHID_EN
