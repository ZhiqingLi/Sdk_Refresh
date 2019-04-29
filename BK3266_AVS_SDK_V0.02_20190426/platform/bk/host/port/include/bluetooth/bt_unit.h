/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_UNIT_H_
#define _BT_UNIT_H_

#include <bluetooth.h>
#include <bt_hci_types.h>
#include <bt_api.h>

/******************************************************************************
 * Bluetooth Device Manager - Application API -> Driver
 *****************************************************************************/

/*
 * HCI layer function prototypes
 */

/* hci_unit.c */
/***************/
/* Initiate acl connection to a device */
result_t bt_unit_acl_connect(bt_unit_h unit_h, const btaddr_t *raddr);

/* Initiate acl disconnection from a device */
void bt_unit_acl_disconnect(bt_unit_h unit_h, const btaddr_t *raddr);

/* Accept incoming ACL connection request */
result_t bt_unit_acl_accept(bt_unit_h unit_h, const btaddr_t *raddr);

/* Reject incoming ACL connection request */
result_t bt_unit_acl_reject(bt_unit_h unit_h, const btaddr_t *raddr);

/* Send ECHO over existing ACL connection */
result_t bt_unit_acl_echo(bt_unit_h unit_h, const btaddr_t *raddr);

/******************************************************************************
 * Bluetooth local unit
 *****************************************************************************/
/* bt_config.c */
/***************/
/* unit_enable\disable wrapper: */
result_t bt_unit_enable(bt_unit_h unit_h);
result_t bt_unit_disable(bt_unit_h unit_h);

/* unit_send_cmd wrapper: */
result_t bt_unit_authentication_enable(bt_unit_h unit_h, bool_t enable);
result_t bt_unit_encryption_enable(bt_unit_h unit_h, bool_t enable);
result_t bt_unit_set_pin_type(bt_unit_h unit_h, uint8_t type);
result_t bt_unit_set_page_scan_type(bt_unit_h unit_h,uint8_t type);
result_t bt_unit_set_inquiry_scan_type(bt_unit_h unit_h,uint8_t type);

result_t bt_unit_set_scan_enable(bt_unit_h unit_h, uint8_t scan_enable);
result_t bt_unit_set_class(bt_unit_h unit_h, uint16_t service, uint8_t major,
    uint8_t minor);
result_t bt_unit_set_name(bt_unit_h unit_h, const char *name);
result_t bt_unit_set_voice_settings(bt_unit_h unit_h, uint16_t settings);
result_t bt_unit_set_page_timeout(bt_unit_h unit_h, uint16_t page_timeout);
result_t bt_unit_set_inquiry_mode(bt_unit_h unit_h, uint8_t mode);
result_t bt_unit_inquiry(bt_unit_h unit_h, uint8_t seconds,
    uint8_t max_responses);
#ifdef CONFIG_TWS
result_t bt_unit_write_inquiry_IAC(bt_unit_h unit, uint8_t mode );
result_t bt_unit_set_page_activity(bt_unit_h unit, uint16_t interval_slot, uint16_t window_slot);
#endif

result_t bt_unit_inquiry_stop(bt_unit_h unit_h);
result_t bt_unit_read_stored_link_key(bt_unit_h unit_h, const btaddr_t *raddr);
#ifdef CONFIG_BLUETOOTH_SSP
result_t bt_unit_simple_pairing_enable(bt_unit_h unit_h);
#ifdef CONFIG_BLUETOOTH_SSP_DEBUG
/* Enables debug equipment to monitor encrypted connections */
result_t bt_unit_simple_pairing_debug_enable(bt_unit_h unit_h, bool_t enable);
#endif
#endif

/* unit_send_cmd_m wrapper: */
result_t bt_unit_write_stored_link_key(bt_unit_h unit_h,
    uint8_t num_keys_write, hci_key_pair_t *key_pair);
result_t bt_unit_delete_stored_link_key(bt_unit_h unit_h,
    const btaddr_t *raddr);

/* unit_set_link_policy wrapper: */
void bt_unit_set_link_policy(bt_unit_h unit_h, uint16_t link_policy_req,
    uint16_t *link_policy_actual);

/* unit_set_packet_type wrapper: */
void bt_unit_set_packet_type(bt_unit_h unit_h, uint16_t packet_type,
    uint16_t *packet_type_actual);

/* btunit getters: */
/* copy the btunit name (NOT calling the send_msg) */
void bt_unit_get_name(bt_unit_h unit_h, char *buf, uint32_t len);
/* copy the btunit addr (NOT calling the send_msg) */
void bt_unit_get_addr(bt_unit_h unit_h, btaddr_t *laddr);

result_t bt_unit_get_remote_name(bt_unit_h unit_h, const btaddr_t *raddr);

#endif
