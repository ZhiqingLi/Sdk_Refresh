/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_SECURITY_H_
#define _BT_SECURITY_H_

/******************************************************************************
 * Bluetooth security API                                                     *
 *****************************************************************************/

#include <bt_l2cap.h>
#include <bt_sdp.h>
#include <bt_hci_types.h>

/****************************
 * Service security mode    *
 ***************************/

result_t bt_sec_add_service_mode(bt_svc_type_t svc_type, const btclass_t uclass,
    bt_link_modes_mask_t mode);

result_t bt_sec_remove_service_mode(bt_svc_type_t svc_type,
    const btclass_t uclass);

result_t bt_sec_lookup_service_mode(bt_svc_type_t svc_type,
    const btclass_t uclass, bt_link_modes_mask_t *mode);

/****************************
 * Device pairing           *
 ***************************/
result_t bt_sec_add_pin(const btaddr_t *raddr, const btpin_t pin);

result_t bt_sec_remove_pin(const btaddr_t *raddr);

void bt_sec_reject_pin(const btaddr_t *raddr);

const btpin_t *bt_sec_lookup_pin(const btaddr_t *raddr);

result_t bt_sec_add_key(const btaddr_t *laddr, const btaddr_t *raddr,
    const btkey_t key, uint8_t key_type);

result_t bt_sec_remove_key(const btaddr_t *laddr, const btaddr_t *raddr);

const btkey_t *bt_sec_lookup_key(const btaddr_t *laddr, const btaddr_t *raddr,
    uint8_t *key_type);

#ifdef CONFIG_BLUETOOTH_SSP
/****************************
 * Secure Simple Pairing    *
 ***************************/
result_t bt_sec_add_pin(const btaddr_t *raddr, const btpin_t pin);

void bt_sec_set_io_caps(uint8_t io_caps);
result_t bt_sec_set_bonding_mode(const btaddr_t *raddr, uint8_t mode);

result_t bt_sec_read_local_oob(const btaddr_t *raddr);

/* TODO: If user is responsible for OOB response from remote_oob_data_request
 * callback, this positive/negative API should exist. negative given by NULL
 * parameters? (possibly just C(hash), as it is always required to be present)
 * */
/* Sends a positive or negative OOB data reply, pass a NULL hash pointer for
 * negative response */
result_t bt_sec_remote_oob_reply(const btaddr_t *raddr, const uint8_t *hash,
    const uint8_t *randomizer);

result_t bt_sec_user_confirmation_reply(const btaddr_t *raddr, bool_t positive);

/* Negative response can be sent by using HCI_SEC_NEG_PASSKEY as value */
result_t bt_sec_user_passkey_reply(const btaddr_t *raddr, uint32_t value);

/* Used to send passkey keypress notifications during passkey entry */
result_t bt_sec_keypress_notification(const btaddr_t *raddr, uint8_t type);

#endif

/****************************
 * Linked Keys Cache        *
 ***************************/
result_t bt_sec_keys_count(j_size_t *count);
result_t bt_sec_keys_raddrs(btaddr_t *raddrs, j_size_t count);

#endif
