/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef __BT_SPP_H__
#define __BT_SPP_H__

/***************************************************************
 * Bluetooth Serial Port Profile (SPP) API *
 ***************************************************************/

#include "bt_at_types.h"
#include <bt_api.h>


/* Handle to a Bluetooth session object */
typedef struct bt_spp_session_s *bt_spp_session_h;
/* Handle to a local Bluetooth server */
typedef struct bt_spp_session_s *bt_spp_server_h;

/**************************
 * Bluetooth Connectivity *
 **************************/

/**
 * Function type:  bt_spp_connected_cb_t
 * Description:    Notifies that a Bluetooth connection with a remote device
 *                 has been established.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @app_ctx_h: (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_spp_connected_cb_t)(
    bt_spp_session_h session_h, bt_app_ctx_h app_ctx_h);

/**
 * Function type:  bt_spp_disconnected_cb_t
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
typedef void (*bt_spp_disconnected_cb_t)(
    bt_spp_session_h session_h, result_t status, bt_app_ctx_h app_ctx_h);

/* Bluetooth connection-status application notification callback functions */
typedef struct {
    bt_spp_connected_cb_t    connected_cb;
    bt_spp_disconnected_cb_t disconnected_cb;
} bt_spp_conn_cbs_t;

/**
 * Function name:  bt_spp_conn_create
 * Description:    Creates a new Bluetooth session object, and registers the
 *                 application's Bluetooth connectivity callback functions for
 *                 the related session with the driver.
 *                 NOTE: The application must call this function before calling
 *                 bt_spp_conn_connect() to initiate a new connection, or
 *                 bt_spp_conn_accept() to accept a connection request.
 *                 The application is responsible for destroying the session
 *                 object, by calling bt_spp_conn_destroy().
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
result_t bt_spp_conn_create(bt_spp_session_h *session_h,
    bt_spp_conn_cbs_t *conn_cbs, bt_app_ctx_h app_ctx_h);

/**
 * Function name:  bt_spp_conn_destroy
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
void bt_spp_conn_destroy(bt_spp_session_h *session_h);

/**
 * Function name:  bt_spp_conn_disconnect
 * Description:    Initiates a disconnect of a Bluetooth connection with a
 *                 remote device.
 *                 NOTE: The driver will respond by calling the application's
 *                 bt_spp_disconnected_cb_t callback.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_spp_conn_disconnect(bt_spp_session_h session_h);

/*--------------------------------
   Client-Specific Connection API
  --------------------------------*/

/**
 * Function name:  bt_spp_conn_connect
 * Description:    Issues a Bluetooth connection request to a remote device.
 *                 NOTE: The driver will respond by calling the application's
 *                 bt_spp_connected_cb_t callback if a connection is
 *                 established, or the bt_spp_disconnected_cb_t callback
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
result_t bt_spp_conn_connect(bt_spp_session_h session_h,
    const btaddr_t *laddr, const btaddr_t *raddr, uint8_t channel);

/*--------------------------------
   Server-Specific Connection API
  --------------------------------*/

/**
 * Function type:  bt_spp_newconn_cb_t
 * Description:    Notifies of a request from a remote Bluetooth device to
 *                 connect to the local Bluetooth server.
 *                 NOTE: The application must either accept the request by
 *                 calling bt_spp_conn_accept(), or reject it by calling
 *                 bt_spp_conn_reject().
 * Parameters:
 *     @conn_req_h: (IN) Handle to a Bluetooth connection request
 *     @laddr:      (IN) Pointer to a local Bluetooth address
 *     @raddr:      (IN) Pointer to a remote Bluetooth address
 *     @mode:       (IN) Bluetooth link modes mask -- see bt_link_modes_mask_t
 *     @app_ctx_h:  (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_spp_newconn_cb_t)(bt_conn_req_h conn_req_h,
    const btaddr_t *laddr, const btaddr_t *raddr, bt_link_modes_mask_t mode,
    bt_app_ctx_h app_ctx_h, uint8_t channel);

/**
 * Function name:  bt_spp_server_start
 * Description:    Starts a local Bluetooth server.
 * Parameters:
 *     @server_h:       (OUT) Pointer to a handle to a local Bluetooth server,
 *                            to be set by the function
 *     @laddr:          (IN)  Pointer to a local Bluetooth address
 *     @rfcomm_channel: (IN)  RFCOMM channel
 *     @newconn_cb:     (IN)  Remote Bluetooth connection request notification
 *                            callback -- see bt_spp_newconn_cb_t
 *     @app_ctx_h:      (IN)  Handle to a Bluetooth application context
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_spp_server_start(bt_spp_server_h *server_h,
    const btaddr_t *laddr, uint8_t rfcomm_channel,
    bt_spp_newconn_cb_t newconn_cb, bt_app_ctx_h app_ctx_h);

/**
 * Function name:  bt_spp_server_stop
 * Description:    Stops a local Bluetooth server.
 * Parameters:
 *     @server_h: (IN) Handle to a local Bluetooth server
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_spp_server_stop(bt_spp_server_h *server_h);

/**
 * Function name:  bt_spp_conn_accept
 * Description:    Accepts a Bluetooth connection request from a remote device.
 *                 NOTE: Before calling this function, the application must
 *                 call bt_spp_conn_create().
 *                 The driver will respond by calling the application's
 *                 bt_spp_connected_cb_t callback -- if a connection
 *                 was established -- or its bt_spp_disconnected_cb_t
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
result_t bt_spp_conn_accept(bt_spp_session_h session_h,
    bt_conn_req_h conn_req_h, bt_link_modes_mask_t mode);

/**
 * Function name:  bt_spp_conn_reject
 * Description:    Rejects a Bluetooth connection request from a remote device.
 * Parameters:
 *     @conn_req_h: (IN) Handle to a Bluetooth connection request
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_spp_conn_reject(bt_conn_req_h conn_req_h);

/*********************************************
 * HFP Hands-Free Role-Specific Functionality
 *********************************************/

/**
 * Function type:  bt_spp_get_features_cb_t
 * Description:    Retrieves HF supported features.
 *                 The driver calls this function as part of the process of
 *                 establishing a Bluetooth connection.
 * Parameters:
 *     @session:            (IN)  Handle to a Bluetooth session object
 *     @supported_features: (OUT) Pointer to a bitmask of supported features
 *                                available in the HF
 *     @app_ctx_h:          (IN)  Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_spp_get_features_cb_t)(bt_spp_session_h session_h,
    uint16_t *supported_features, bt_app_ctx_h app_ctx);

/**
 * Function type:  bt_spp_at_result_cb_t
 * Description:    Notifies that a Bluetooth connection with a remote device
 *                 has been established.
 * Parameters:
 *     @session:     (IN) Handle to a Bluetooth session object
 *     @at_result_t: (IN) Pointer to an AT result structure
 *     @app_ctx_h:   (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_spp_at_result_cb_t)(bt_spp_session_h session_h,
    char *result, uint16_t length);//bt_app_ctx_h app_ctx);

/* HFP HF role-specific application callback functions */
typedef struct {
    bt_spp_at_result_cb_t            at_result_cb;
    bt_spp_get_features_cb_t         get_features_cb;
} bt_spp_cbs_t;

/**
 * Function name:  bt_spp_register
 * Description:    Registers the application to use the HFP HF driver's
 *                 role-specific APIs.
 * Parameters:
 *     @cbs: (IN)  Pointer to a structure of HFP HF role-specific application
 *                 callback functions
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_spp_register(const bt_spp_cbs_t *cbs);

/**
 * Function name:  bt_spp_unregister
 * Description:    Unregisters the application from using the HFP HF driver's
 *                 role-specific APIs.
 * Parameters:     None
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_spp_unregister(void);

result_t spp_connect_rfcomm(btaddr_t raddr, uint8_t channel);
result_t bt_spp_conn_send(bt_spp_session_h session_h, char *buf, uint8_t len);
#endif

