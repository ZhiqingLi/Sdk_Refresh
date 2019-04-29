/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_API_H_
#define _BT_API_H_

#include <bluetooth.h>
#include <bt_hci_types.h>

/* Bluetooth application context */
typedef void *bt_app_ctx_h;

/* Bluetooth connection request */
typedef void *bt_conn_req_h;

/* Bluetooth controller unit context */
typedef struct bt_unit_s *bt_unit_h;

/******************************************************************************
 * Bluetooth Device Manager - Driver -> Application API
 *****************************************************************************/

typedef void (*bt_pin_code_request_cb_t)(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr);

typedef void (*bt_acl_con_request_cb_t)(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr, btclass_t uclass);

typedef void (*bt_inq_result_cb_t)(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr, btclass_t uclass, int8_t rssi);

typedef void (*bt_ext_inq_result_cb_t)(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr, btclass_t uclass, int8_t rssi, uint8_t *eir_data);

typedef void (*bt_inq_complete_cb_t)(bt_app_ctx_h app_ctx_h);

typedef void (*bt_remote_name_complete_cb_t)(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr, uint8_t status, const uint8_t *name,
    uint8_t name_len);

typedef void (*bt_read_remote_features_complete_cb_t)(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr, uint8_t status, bt_lmp_features_t features);

typedef void (*bt_sec_db_changed_cb_t)(bt_app_ctx_h app_ctx_h);

typedef void (*bt_set_name_complete_cb_t)(bt_app_ctx_h app_ctx_h,
    hci_error_t hci_err);

typedef void (*bt_set_class_complete_cb_t)(bt_app_ctx_h app_ctx_h,
    hci_error_t hci_err);

typedef void (*bt_auth_enable_complete_cb_t)(bt_app_ctx_h app_ctx_h,
    hci_error_t hci_err);

typedef void (*bt_scan_enable_complete_cb_t)(bt_app_ctx_h app_ctx_h,
    hci_error_t hci_err);

typedef void (*bt_enable_complete_cb_t)(bt_app_ctx_h app_ctx_h,
    hci_error_t hci_err, result_t err);

typedef void (*bt_disable_complete_cb_t)(bt_app_ctx_h app_ctx_h,
    hci_error_t hci_err, result_t err);

typedef void (*bt_write_linkkey_complete_cb_t)(bt_app_ctx_h app_ctx_h,
	hci_error_t hci_err);

#ifdef CONFIG_BLUETOOTH_SSP

typedef void (*bt_simple_pairing_enable_complete_cb_t)(bt_app_ctx_h app_ctx_h,
    hci_error_t status);

/* TODO: Determine how local OOB complete callback should return
 * R(randomizer) and C(hash) values, both are 16 byte values */
/* Used to obtain local data for OOB transfer */
typedef void (*bt_read_local_oob_complete_cb_t)(bt_app_ctx_h app_ctx_h,
    hci_error_t status, const uint8_t *hash, const uint8_t *randomizer);

/* Called if user specified OOB data was available from the remote device */
typedef void (*bt_remote_oob_data_request_cb_t)(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr);

/* TODO: Either an additional callback is needed to specify that the
 * information is available (as it's needed during IO capability exchange)
 * or this callback can be called for each attempted connection, in which
 * case the core will need to cache the values */

/* Called for user confirmation requests, user should send positive/negative
 * reply before timeout */
typedef void (*bt_user_confirmation_request_cb_t)(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr, uint32_t value);

/* User must reply with passkey reply or negative */
typedef void (*bt_user_passkey_request_cb_t)(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr);

/* Called to display 6 digit passkey to user */
typedef void (*bt_user_passkey_notification_cb_t)(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr, uint32_t passkey);

/* Notifies of remote passkey keypress events */
typedef void (*bt_keypress_notification_cb_t)(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr, uint8_t notification_type);

/* Called for simple pairing success/failure, UI may clear displayed
 * values if needed */
typedef void (*bt_simple_pairing_complete_cb_t)(bt_app_ctx_h app_ctx_h,
    hci_error_t status, const btaddr_t *raddr);

/* TODO: Does anybody need to know of remote IO capabilities? */
#endif

typedef struct bt_unit_app_cbs_s {
    bt_pin_code_request_cb_t               pin_code_request_cb;
    bt_acl_con_request_cb_t                acl_con_request_cb;
    bt_inq_result_cb_t                     inq_result_cb;
    bt_ext_inq_result_cb_t                 ext_inq_result_cb;
    bt_inq_complete_cb_t                   inq_complete_cb;
    bt_remote_name_complete_cb_t           remote_name_complete_cb;
    bt_read_remote_features_complete_cb_t  read_remote_features_complete_cb;
    bt_sec_db_changed_cb_t                 sec_db_changed_cb;
    bt_set_name_complete_cb_t              set_name_complete_cb;
    bt_set_class_complete_cb_t             set_class_complete_cb;
    bt_auth_enable_complete_cb_t           auth_enable_complete_cb;
    bt_scan_enable_complete_cb_t           scan_enable_complete_cb;
    bt_enable_complete_cb_t                enable_complete_cb;
    bt_disable_complete_cb_t               disable_complete_cb;
    bt_write_linkkey_complete_cb_t      write_linkkey_complete_cb;
#ifdef CONFIG_BLUETOOTH_SSP
    bt_simple_pairing_enable_complete_cb_t simple_pairing_enable_complete_cb;
    bt_read_local_oob_complete_cb_t        read_local_oob_complete_cb;
    bt_remote_oob_data_request_cb_t        remote_oob_data_request_cb;
    bt_user_confirmation_request_cb_t      user_confirmation_request_cb;
    bt_user_passkey_request_cb_t           user_passkey_request_cb;
    bt_user_passkey_notification_cb_t      user_passkey_notification_cb;
    bt_keypress_notification_cb_t          keypress_notification_cb;
    bt_simple_pairing_complete_cb_t        simple_pairing_complete_cb;
#endif
} bt_unit_app_cbs_t;

result_t bt_app_unit_attach(bt_unit_h unit_h, bt_app_ctx_h *app_h,
    bt_unit_app_cbs_t *cbs);
result_t bt_app_unit_detach(bt_app_ctx_h app_ctx_h);

/******************************************************************************
 * Service discovery and connection
 *****************************************************************************/

/**
 * Function name:  bt_sdp_service_register
 * Description:    Registers the specified SDP record in the local SDP server.
 * Parameters:
 *     @laddr:     (IN)  Pointer to a local Bluetooth address
 *     @svc_type:  (IN)  SDP service class
 *     @data:      (IN)  Pointer to profile data for service discovery - refer
 *                       to the specific Jungo Bluetooth Profile Driver document
 *     @datalen:   (IN)  The size of the profile data, in bytes
 *     @handle:    (OUT) SDP record handle
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_sdp_service_register(const btaddr_t *laddr, bt_svc_type_t type,
    void *data, uint32_t len, uint32_t *handle);

/**
 * Function name:  bt_sdp_service_unregister
 * Description:    Removes the given service record from the local SDP server.
 * Parameters:
 *     @handle:    (IN) SDP record handle
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_sdp_service_unregister(uint32_t handle);

#endif

