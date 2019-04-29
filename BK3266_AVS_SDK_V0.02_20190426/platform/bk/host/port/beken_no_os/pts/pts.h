#include "includes.h"
#if PTS_TESTING
#ifndef _PTS_H_
#define _PTS_H_

void pts_entry(unsigned char cmd);


/*******************************************
 * HFP 
 * uart cmd: 0x01 e0 40 01 10 
 *                         10~4F for hfp
 *******************************************/
#define PTS_HFP_CMD_SCO_CONN        0x10
#define PTS_HFP_CMD_SCO_DISCONN     0x11
#define PTS_HFP_CMD_INITIATE_SLC    0x12

void pts_hfp_entry(unsigned char cmd);


/*******************************************
 * A2DP 
 * uart cmd: 0x01 e0 40 01 50 
 *                         50~8F for a2dp
 *******************************************/
#define PTS_A2DP_CMD_INITIATE_DELAYREPORT   0x50

void pts_a2dp_entry(unsigned char cmd);


#endif

#endif

