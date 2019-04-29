/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_INIT_H_
#define _BT_INIT_H_

#include <uw_types.h>

#define INIT_BT_DEV     (1 << 0)
#define INIT_BT_SEC     (1 << 1)
#define INIT_SDP_CLIENT (1 << 2)
#define INIT_SDP_SERVER (1 << 3)
#define INIT_BT_KERNEL  (1 << 4)
#define INIT_BT_USER    (1 << 5)
#define INIT_BT_HCI     (1 << 6)
#define INIT_BT_AVDTP   (1 << 7)
#define INIT_BT_SDAP    (1 << 8)
#define INIT_BT_PAN     (1 << 9)

result_t bt_init(void);
void bt_uninit(void);
result_t btdev_init(void);
void btdev_uninit(void);

void BT_Initialise(void);
void BT_init_After_set_BDADDR(void);

#endif
