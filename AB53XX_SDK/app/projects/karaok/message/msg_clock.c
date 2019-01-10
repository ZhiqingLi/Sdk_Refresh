#include "include.h"
#include "func.h"
#include "func_clock.h"

#if FUNC_CLOCK_EN
AT(.text.func.clock.msg)
void func_clock_message(u16 msg)
{
    switch (msg) {
        case KU_PLAY:
        case KU_PLAY_POWER:
        case KU_PLAY_MODE:
            if (tm_cb.setting) {
                tm_cb.setting = 0;
                rtc_tm.tm_sec = 0;
                irtc_time_write(RTCCNT_CMD, rtc_tm_to_time(&rtc_tm));
            } else {
                tm_cb.setting = 1;
                tm_cb.index = 1;
                gui_box_flicker_set(5, 0xff, tm_cb.index);        //flicker freq 100ms*5
                rtc_time_to_tm(irtc_time_read(RTCCNT_CMD), &rtc_tm);
            }
            break;

        case KU_NEXT:
            tm_cb.index = (tm_cb.index == 1) ? 2 : 1;
            gui_box_flicker_set(5, 0xff, tm_cb.index);
            break;

        case KU_PREV:
            tm_cb.index = (tm_cb.index == 2) ? 1 : 2;
            gui_box_flicker_set(5, 0xff, tm_cb.index);
            break;

        case KL_VOL_UP:
        case KH_VOL_UP:
        case KU_VOL_UP:
            if (tm_cb.index == 1) {
                rtc_tm.tm_min++;
                if (rtc_tm.tm_min > 59) {
                    rtc_tm.tm_min = 0;
                }
            } else if (tm_cb.index == 2) {
                rtc_tm.tm_hour++;
                if (rtc_tm.tm_hour > 23) {
                    rtc_tm.tm_hour = 0;
                }
            }
            break;

        case KL_VOL_DOWN:
        case KH_VOL_DOWN:
        case KU_VOL_DOWN:
            if (tm_cb.index == 1) {
                if (rtc_tm.tm_min > 0) {
                    rtc_tm.tm_min--;
                } else {
                    rtc_tm.tm_min = 59;
                }
            } else if (tm_cb.index == 2) {
                if (rtc_tm.tm_hour > 0) {
                    rtc_tm.tm_hour--;
                } else {
                    rtc_tm.tm_hour = 23;
                }
            }
            break;

        case MSG_SYS_500MS:
            if ((!tm_cb.setting) && (!tm_cb.type)) {
                rtc_time_to_tm(irtc_time_read(RTCCNT_CMD), &rtc_tm);
            }
            break;


        default:
            func_message(msg);
            break;
    }
}
#endif //FUNC_CLOCK_EN
