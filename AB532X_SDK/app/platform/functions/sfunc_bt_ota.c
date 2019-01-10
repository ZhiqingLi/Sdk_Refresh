#include "include.h"
#include "func.h"
#include "func_bt.h"

#if FUNC_BT_EN

AT(.text.func.btring)
void sfunc_bt_ota(void)
{
    int res;
    printf("%s\n", __func__);
    //bt_audio_bypass();
    led_off();
    rled_off();
    res = updateota_init();
    if (res != 1) {
        //bt_audio_enable();
        while(bt_get_status() == BT_STA_OTA) {
            delay_5ms(4);
        }
        return;
    }
    updateproc();
    while (1);
/*
    while ((f_bt.disp_status == BT_STA_OTA) && (func_cb.sta == FUNC_BT)) {
        sfunc_bt_ota_process();
        func_bt_message(msg_dequeue());
        func_bt_display();
    }

    sfunc_bt_ota_exit();
    */
}

#endif //FUNC_BT_EN
