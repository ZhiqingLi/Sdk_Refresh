#include "include.h"
#include "func.h"

#if FUNC_FMAM_FREQ_EN
AT(.text.func.fmam.msg)
void func_fmam_message(u16 msg)
{
    switch (msg) {

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_FMAM_FREQ_EN
