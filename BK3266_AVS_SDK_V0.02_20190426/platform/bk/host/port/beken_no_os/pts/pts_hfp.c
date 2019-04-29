#include "pts.h"
#if PTS_TESTING

extern void pts_util_sco_connect(void);
extern void pts_util_sco_disconnect(void);
extern void pts_util_initiate_slc(void);

void pts_hfp_entry(unsigned char cmd)
{
    os_printf("pts_hfp_entry, 0x%x \r\n", cmd);
    
    switch (cmd) {
        case PTS_HFP_CMD_SCO_CONN:
            pts_util_sco_connect();
            break;

        case PTS_HFP_CMD_SCO_DISCONN:
            pts_util_sco_disconnect();
            break;
            
        case PTS_HFP_CMD_INITIATE_SLC:
            pts_util_initiate_slc();
            break;

        default:
            os_printf("pts_hfp_entry, CMD error! 0x%x \r\n", cmd);
            break;
    }    
}

#endif

