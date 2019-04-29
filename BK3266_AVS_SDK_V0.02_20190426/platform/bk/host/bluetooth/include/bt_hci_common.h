/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_HCI_COMMON_H_
#define _BT_HCI_COMMON_H_

#include <bluetooth.h>
#include <bt_api.h>

/*
 * HCI layer function prototypes
 */

bt_unit_h bt_unit_find_devname(const char *name);
bt_unit_h bt_unit_find_btaddr(const btaddr_t *laddr);
void bt_unit_populate_unit_info(bt_unit_h unit_h, bt_unit_info_t *info);

void hci_update_ext_inq_rsp(const btaddr_t *laddr);

#endif
