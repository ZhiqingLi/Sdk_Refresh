/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _BT_RFCOMM_CHANNEL_H_
#define _BT_RFCOMM_CHANNEL_H_

#include <uw_types.h>

enum {
    RFCOMM_CHANNEL_HSP = 2,
    RFCOMM_CHANNEL_HFP,
    RFCOMM_CHANNEL_OPUSH,
    RFCOMM_CHANNEL_PBAP,
    RFCOMM_CHANNEL_WIAP,
    RFCOMM_CHANNEL_SPP,
    RFCOMM_CHANNEL_DUN
};
typedef uint8_t bt_rfcomm_channel_t;

#endif

