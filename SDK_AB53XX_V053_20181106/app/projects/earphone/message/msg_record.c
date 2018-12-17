#include "include.h"
#include "func.h"
#include "sfunc_record.h"

#if FUNC_REC_EN
AT(.text.func.record.msg)
void sfunc_record_message(u16 msg)
{
    switch (msg) {
        case KU_MODE:
        case KL_PLAY_MODE:
            msg_enqueue(msg);
        case KU_REC:
            sfunc_record_stop();
            break;

        case KU_PLAY:
            if (sfunc_is_recording()) {
                sfunc_record_pause();
            } else {
                sfunc_record_start();
            }
            break;

        case KL_REC:
            //切换录音设备
            if (sfunc_record_switch_device()) {
                sfunc_record_stop();
                if (!sfunc_record_start()) {
                    if (sfunc_record_switch_device()) {
                        sfunc_record_start();
                    }
                }
            }
            break;

        case MSG_SYS_1S:
            if (!sfunc_is_recording()) {
                break;
            }
            if(!sfunc_fwrite_sync()) {
                sfunc_record_stop();
            }
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif //FUNC_REC_EN
