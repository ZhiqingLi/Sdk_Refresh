#include "pts.h"
#if PTS_TESTING

#if UPGRADED_VERSION
extern result_t a2dp_send_delayReport(uint16_t dr_ms);
#endif

void pts_a2dp_entry(unsigned char cmd)
{
    os_printf("pts_a2dp_entry, 0x%x \r\n", cmd);
    
    switch (cmd) {
#if UPGRADED_VERSION
        case PTS_A2DP_CMD_INITIATE_DELAYREPORT:
            a2dp_send_delayReport(150); /* the value 150 just only be used for testing, it means nothing. */
            break;
#endif
        default:
            os_printf("pts_a2dp_entry, CMD error! 0x%x \r\n", cmd);
            break;
    }
}

#endif

