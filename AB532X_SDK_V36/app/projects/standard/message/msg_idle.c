#include "include.h"
#include "func.h"
#include "func_idle.h"

#if FUNC_IDLE_EN
AT(.text.func.idle.msg)
void func_idle_message(u16 msg)
{
    switch (msg) {
		case KU_MODE:
        case KU_MODE_POWER:
        case KL_PLAY_MODE:
			f_idle.cur_status++;
			f_idle.cur_status %= 4;
			printf ("func ilde cur status = %d!!!\n", f_idle.cur_status);
			break;

		case EVT_ADC_DET_HIG:
		case EVT_ADC_DET_LOW:
			f_idle.cur_status = 4;
			break;

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_IDLE_EN
