/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef __BT_HSP_HS_H__
#define __BT_HSP_HS_H__

/*********************************************************
 * Bluetooth Headset Profile (HSP) Headset (HS) Role API *
 *********************************************************/

#include "bt_at_types.h"
#include <bt_api.h>
#ifdef CONFIG_BLUETOOTH_HSP
/* Handle to a Bluetooth session object */
typedef struct bt_hsp_hs_session_s *bt_hsp_hs_session_h;
/* Handle to a local Bluetooth server */
typedef struct bt_hsp_hs_session_s *bt_hsp_hs_server_h;

/**************************
 * Bluetooth Connectivity *
 **************************/

/**
 * Function type:  bt_hsp_hs_connected_cb_t
 * Description:    Notifies that a Bluetooth connection with a remote device
 *                 has been established.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @app_ctx_h: (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_hsp_hs_connected_cb_t)(
    bt_hsp_hs_session_h session_h, bt_app_ctx_h app_ctx_h);

/**
 * Function type:  bt_hsp_hs_disconnected_cb_t
 * Description:    Notifies that a Bluetooth connection with a remote device
 *                 has been disconnected, or that a previous connection attempt
 *                 has failed.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @status:    (IN) Disconnect status
 *     @app_ctx_h: (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_hsp_hs_disconnected_cb_t)(
    bt_hsp_hs_session_h session_h, result_t status, bt_app_ctx_h app_ctx_h);

/* Bluetooth connection-status application notification callback functions */
typedef struct {
    bt_hsp_hs_connected_cb_t    connected_cb;
    bt_hsp_hs_disconnected_cb_t disconnected_cb;
} bt_hsp_hs_conn_cbs_t;


/**
 * Function name:  bt_hsp_hs_conn_create
 * Description:    Creates a new Bluetooth session object, and registers the
 *                 application's Bluetooth connectivity callback functions for
 *                 the related session with the driver.
 *                 NOTE: The application must call this function before calling
 *                 bt_hfp_hf_conn_connect() to initiate a new connection, or
 *                 bt_hfp_hf_conn_accept() to accept a connection request.
 *                 The application is responsible for destroying the session
 *                 object, by calling bt_hsp_hs_conn_destroy().
 * Parameters:
 *     @session_h: (OUT) Pointer to a handle to a Bluetooth session object, to
 *                       bet set by the function
 *     @conn_cbs:  (IN)  Pointer to a structure of Bluetooth connection-status
 *                       application notification callback functions
 *     @app_ctx_h: (IN)  Handle to a Bluetooth application context
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hsp_hs_conn_create(bt_hsp_hs_session_h *session_h,
    bt_hsp_hs_conn_cbs_t *conn_cbs, bt_app_ctx_h app_ctx_h);

/**
 * Function name:  bt_hsp_hs_conn_destroy
 * Description:    Destroys a Bluetooth session object.
 *                 NOTE: The application must call this function to destroy the
 *                 session handle for a disconnected or unestablished
 *                 connection.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_hsp_hs_conn_destroy(bt_hsp_hs_session_h *session_h);

/**
 * Function name:  bt_hsp_hs_conn_disconnect
 * Description:    Initiates a disconnect of a Bluetooth connection with a
 *                 remote device.
 *                 NOTE: The driver will respond by calling the application's
 *                 bt_hsp_hs_disconnected_cb_t callback.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hsp_hs_conn_disconnect(bt_hsp_hs_session_h session_h);

/*--------------------------------
   Client-Specific Connection API
  --------------------------------*/

/**
 * Function name:  bt_hsp_hs_conn_connect
 * Description:    Issues a Bluetooth connection request to a remote device.
 *                 NOTE: The driver will respond by calling the application's
 *                 bt_hsp_hs_connected_cb_t callback if a connection is
 *                 established, or the bt_hsp_hs_disconnected_cb_t callback
 *                 otherwise.
 * Parameters:
 *     @session_h:      (OUT) Pointer to a handle to a Bluetooth session object,
 *                            to be set by the function
 *     @laddr:          (IN)  Pointer to a local Bluetooth address
 *     @raddr:          (IN)  Pointer to a remote Bluetooth address
 *     @rfcomm_channel: (IN)  RFCOMM channel
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hsp_hs_conn_connect(bt_hsp_hs_session_h session_h,
    const btaddr_t *laddr, const btaddr_t *raddr, uint8_t channel);

/*--------------------------------
   Server-Specific Connection API
  --------------------------------*/

/**
 * Function type:  bt_hsp_hs_newconn_cb_t
 * Description:    Notifies of a request from a remote Bluetooth device to
 *                 connect to the local Bluetooth server.
 *                 NOTE: The application must either accept the request by
 *                 calling bt_hsp_hs_conn_accept(), or reject it by calling
 *                 bt_hsp_hs_conn_reject().
 * Parameters:
 *     @conn_req_h: (IN) Handle to a Bluetooth connection request
 *     @laddr:      (IN) Pointer to a local Bluetooth address
 *     @raddr:      (IN) Pointer to a remote Bluetooth address
 *     @mode:       (IN) Bluetooth link modes mask -- see bt_link_modes_mask_t
 *     @app_ctx_h:  (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_hsp_hs_newconn_cb_t)(bt_conn_req_h conn_req_h,
    const btaddr_t *laddr, const btaddr_t *raddr, bt_link_modes_mask_t mode,
    bt_app_ctx_h app_ctx_h);

/**
 * Function name:  bt_hsp_hs_server_start
 * Description:    Starts a local Bluetooth server.
 * Parameters:
 *     @server_h:       (OUT) Pointer to a handle to a local Bluetooth server,
 *                            to be set by the function
 *     @laddr:          (IN)  Pointer to a local Bluetooth address
 *     @rfcomm_channel: (IN)  RFCOMM channel
 *     @newconn_cb:     (IN)  Remote Bluetooth connection request notification
 *                            callback -- see bt_hsp_hs_newconn_cb_t
 *     @app_ctx_h:      (IN)  Handle to a Bluetooth application context
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hsp_hs_server_start(bt_hsp_hs_server_h *server_h,
    const btaddr_t *laddr, uint8_t rfcomm_channel,
    bt_hsp_hs_newconn_cb_t newconn_cb, bt_app_ctx_h app_ctx_h);

/**
 * Function name:  bt_hsp_hs_server_stop
 * Description:    Stops a local Bluetooth server.
 * Parameters:
 *     @server_h: (IN) Handle to a local Bluetooth server
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_hsp_hs_server_stop(bt_hsp_hs_server_h *server_h);

/**
 * Function name:  bt_hsp_hs_conn_accept
 * Description:    Accepts a Bluetooth connection request from a remote device.
 *                 NOTE: Before calling this function, the application must
 *                 call bt_hsp_hs_conn_create().
 *                 The driver will respond by calling the application's
 *                 bt_hsp_hs_connected_cb_t callback -- if a connection
 *                 was established -- or its bt_hsp_hs_disconnected_cb_t
 *                 callback otherwise.
 * Parameters:
 *     @session_h:  (OUT) Pointer to a handle to a Bluetooth session object, to
 *                        bet set by the function
 *     @conn_req_h: (IN) Handle to a Bluetooth connection request
 *     @mode:       (IN) Bluetooth link modes mask -- see bt_link_modes_mask_t
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hsp_hs_conn_accept(bt_hsp_hs_session_h session_h,
    bt_conn_req_h conn_req_h, bt_link_modes_mask_t mode);

/**
 * Function name:  bt_hsp_hs_conn_reject
 * Description:    Rejects a Bluetooth connection request from a remote device.
 * Parameters:
 *     @conn_req_h: (IN) Handle to a Bluetooth connection request
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_hsp_hs_conn_reject(bt_conn_req_h conn_req_h);

/******************************************
 * HSP Headset Role-Specific Functionality
 ******************************************/

/**
 * Function type:  bt_hsp_hs_get_features_cb_t
 * Description:    Retrieves HS supported features -- support for remote volume
 *                 control.
 *                 The driver calls this function as part of the process of
 *                 establishing a Bluetooth connection.
 * Parameters:
 *     @session:   (IN)  Handle to a Bluetooth session object
 *     @is_remote_volume_control_supported:
 *                 (OUT) Pointer to a value indicating the support for remote
 *                       volume control: 1 if supported; 0 otherwise
 *     @app_ctx_h: (IN)  Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_hsp_hs_get_features_cb_t)(bt_hsp_hs_session_h session_h,
    bool_t *is_remote_volume_control_supported, bt_app_ctx_h app_ctx);

/**
 * Function type:  bt_hsp_hs_at_result_cb_t
 * Description:    Notifies that a Bluetooth connection with a remote device
 *                 has been established.
 * Parameters:
 *     @session:     (IN) Handle to a Bluetooth session object
 *     @at_result_t: (IN) Pointer to an AT result structure
 *     @app_ctx_h:   (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_hsp_hs_at_result_cb_t)(bt_hsp_hs_session_h session_h,
    at_result_t *at_result, bt_app_ctx_h app_ctx);

/* HSP HS role-specific application callback functions */
typedef struct {
    bt_hsp_hs_at_result_cb_t            at_result_cb;
    bt_hsp_hs_get_features_cb_t         get_features_cb;
} bt_hsp_hs_cbs_t;

/**
 * Function name:  bt_hsp_hs_register
 * Description:    Registers the application to use the HSP HS driver's
 *                 role-specific APIs.
 * Parameters:
 *     @cbs: (IN)  Pointer to a structure of HSP HS role-specific application
 *                 callback functions
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hsp_hs_register(const bt_hsp_hs_cbs_t *cbs);

/**
 * Function name:  bt_hsp_hs_unregister
 * Description:    Unregisters the application from using the HSP HS driver's
 *                 role-specific APIs.
 * Parameters:     None
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_hsp_hs_unregister(void);

/**
 * Function name:  bt_hsp_hs_vgm
 * Description:    Issues a gain of microphone command (AT_COMMAND_VGM) from the
 *                 HS to the AG, to report the current HS microphone gain level.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @mic_gain:  (IN) Microphone gain level (0--15)
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hsp_hs_vgm(bt_hsp_hs_session_h session_h, uint8_t mic_gain);

/**
 * Function name:  bt_hsp_hs_vgs
 * Description:    Issues a gain of speaker command (AT_COMMAND_VGS) from the
 *                 HS to the AG, to report the current HS speaker gain level.
 * Parameters:
 *     @session_h:    (IN) Handle to a Bluetooth session object
 *     @speaker_gain: (IN) Speaker gain level (0--15)
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hsp_hs_vgs(bt_hsp_hs_session_h session_h, uint8_t speaker_gain);

/**
 * Function name:  bt_hsp_hs_button_pressed
 * Description:    Issues a headset button press command
 *                 (AT_COMMAND_BUTTON_PRESSED) from the HS to the AG, to
 *                 indicate that the HS button has been pressed.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hsp_hs_button_pressed(bt_hsp_hs_session_h session);
#endif
#endif

