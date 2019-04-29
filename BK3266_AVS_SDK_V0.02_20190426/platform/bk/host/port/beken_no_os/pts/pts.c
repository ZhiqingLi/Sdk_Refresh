#include "pts.h"
#if PTS_TESTING

void pts_entry(unsigned char cmd)
{
    os_printf("pts_entry, 0x%x \r\n", cmd);
    
    if (cmd < PTS_A2DP_CMD_INITIATE_DELAYREPORT) {
        pts_hfp_entry(cmd);
    } else if (cmd >= PTS_A2DP_CMD_INITIATE_DELAYREPORT) {
        pts_a2dp_entry(cmd);
    }
}

#endif

