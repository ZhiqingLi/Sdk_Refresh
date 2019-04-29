/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_A2DP_SINK_H_
#define _BT_A2DP_SINK_H_

/* ********************************************************************
 *  Bluetooth Advanced Audio Distribution Profile (A2DP) Sink Role API
 * ******************************************************************** */

#include <bt_a2dp_types.h>
#include <bt_avdtp.h>
#include <bt_api.h>
#include <bluetooth.h>

/* Handle to a Bluetooth session object */
typedef struct bt_a2dp_session_s *bt_a2dp_sink_session_h;
/* Handle to a local Bluetooth server */
typedef struct bt_a2dp_session_s *bt_a2dp_sink_server_h;

/**************************
 * Bluetooth Connectivity *
 *************************/

/**
 * Function type:  bt_a2dp_sink_connected_cb_t
 * Description:    Notifies that a Bluetooth connection with a remote device
 *                 has been established.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @app_ctx_h: (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_a2dp_sink_connected_cb_t)(bt_a2dp_sink_session_h session_h,
    bt_app_ctx_h app_ctx_h);

/**
 * Function type:  bt_a2dp_sink_disconnected_cb_t
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
typedef void (*bt_a2dp_sink_disconnected_cb_t)(bt_a2dp_sink_session_h session_h,
    result_t status, bt_app_ctx_h app_ctx_h);

/* Bluetooth connection-status application notification callback functions */
typedef struct {
    bt_a2dp_sink_connected_cb_t    connected_cb;
    bt_a2dp_sink_disconnected_cb_t disconnected_cb;
} bt_a2dp_sink_conn_cbs_t;

/**
 * Function name:  bt_a2dp_sink_conn_create
 * Description:    Creates a new Bluetooth session object, and registers the
 *                 application's Bluetooth connectivity callback functions for
 *                 the related session with the driver.
 *                 NOTE: The application must call this function before calling
 *                 bt_a2dp_sink_conn_connect() to initiate a new connection, or
 *                 bt_a2dp_sink_conn_accept() to accept a connection request.
 *                 The application is responsible for destroying the session
 *                 object, by calling bt_a2dp_sink_conn_destroy().
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
result_t bt_a2dp_sink_conn_create(bt_a2dp_sink_session_h *session_h,
    const bt_a2dp_sink_conn_cbs_t *conn_cbs, bt_app_ctx_h app_ctx_h);

/**
 * Function name:  bt_a2dp_sink_conn_destroy
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
void bt_a2dp_sink_conn_destroy(bt_a2dp_sink_session_h *session_h);

/**
 * Function name:  bt_a2dp_sink_conn_disconnect
 * Description:    Initiates a disconnect of a Bluetooth connection with a
 *                 remote device.
 *                 NOTE: The driver will respond by calling the application's
 *                 bt_a2dp_sink_disconnected_cb_t callback.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_a2dp_sink_conn_disconnect(bt_a2dp_sink_session_h server_h);

/*--------------------------------
   Client-Specific Connection API
  --------------------------------*/
/**
 * Function name:  bt_a2dp_sink_conn_connect
 * Description:    Issues a Bluetooth connection request to a remote device.
 *                 NOTE: The driver will respond by calling the application's
 *                 bt_a2dp_sink_connected_cb_t callback if a connection is
 *                 established, or the bt_a2dp_sink_disconnected_cb_t callback
 *                 otherwise.
 * Parameters:
 *     @session_h:      (OUT) Pointer to a handle to a Bluetooth session object,
 *                            to be set by the function
 *     @laddr:          (IN)  Pointer to a local Bluetooth address
 *     @raddr:          (IN)  Pointer to a remote Bluetooth address
 *     @ep:             (IN)  Pointer to an audio stream endpoint descriptor
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_a2dp_sink_conn_connect(bt_a2dp_sink_session_h session_h,
    const btaddr_t *laddr, const btaddr_t *raddr, bt_a2dp_endpoint_desc_t *ep);

/*--------------------------------
   Server-Specific Connection API
  --------------------------------*/

/**
 * Function type:  bt_a2dp_sink_newconn_cb_t
 * Description:    Notifies of a request from a remote Bluetooth device to
 *                 connect to the local Bluetooth server.
 *                 NOTE: The application must either accept the request by
 *                 calling bt_a2dp_sink_conn_accept(), or reject it by calling
 *                 bt_a2dp_sink_conn_reject().
 * Parameters:
 *     @conn_req_h: (IN) Handle to a Bluetooth connection request
 *     @laddr:      (IN) Pointer to a local Bluetooth address
 *     @raddr:      (IN) Pointer to a remote Bluetooth address
 *     @mode:       (IN) Bluetooth link modes mask -- see bt_link_modes_mask_t
 *     @app_ctx_h:  (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_a2dp_sink_newconn_cb_t)(bt_conn_req_h conn_req_h,
    const btaddr_t *laddr, const btaddr_t *raddr, bt_link_modes_mask_t mode,
    bt_app_ctx_h app_ctx_h);

/**
 * Function name:  bt_a2dp_sink_server_start
 * Description:    Starts a local Bluetooth server.
 * Parameters:
 *     @server_h:       (OUT) Pointer to a handle to a local Bluetooth server,
 *                            to be set by the function
 *     @laddr:          (IN)  Pointer to a local Bluetooth address
 *     @rfcomm_channel: (IN)  RFCOMM channel
 *     @newconn_cb:     (IN)  Remote Bluetooth connection request notification
 *                            callback -- see bt_a2dp_sink_newconn_cb_t
 *     @app_ctx_h:      (IN)  Handle to a Bluetooth application context
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_a2dp_sink_server_start(bt_a2dp_sink_server_h *server_h,
    const btaddr_t *laddr, bt_a2dp_sink_newconn_cb_t newconn_cb,
    bt_app_ctx_h app_ctx_h);

/**
 * Function name:  bt_a2dp_sink_server_stop
 * Description:    Stops a local Bluetooth server.
 * Parameters:
 *     @server_h: (IN) Handle to a local Bluetooth server
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_a2dp_sink_server_stop(bt_a2dp_sink_server_h *server_h);

/**
 * Function name:  bt_a2dp_sink_conn_accept
 * Description:    Accepts a Bluetooth connection request from a remote device.
 *                 NOTE: Before calling this function, the application must
 *                 call bt_a2dp_sink_conn_create().
 *                 The driver will respond by calling the application's
 *                 bt_a2dp_sink_connected_cb_t callback -- if a connection
 *                 was established -- or its bt_a2dp_sink_disconnected_cb_t
 *                 callback otherwise.
 * Parameters:
 *     @session_h:  (OUT) Pointer to a handle to a Bluetooth session object, to
 *                        bet set by the function
 *     @conn_req_h: (IN) Handle to a Bluetooth connection request
 *     @mode:       (IN) Bluetooth link modes mask -- see bt_link_modes_mask_t
 *     @ep:             (IN)  Pointer to an audio stream endpoint descriptor
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_a2dp_sink_conn_accept(bt_a2dp_sink_session_h session_h,
    bt_conn_req_h conn_req_h, bt_link_modes_mask_t mode,
    bt_a2dp_endpoint_desc_t *ep);

/**
 * Function name:  bt_a2dp_sink_conn_reject
 * Description:    Rejects a Bluetooth connection request from a remote device.
 * Parameters:
 *     @conn_req_h: (IN) Handle to a Bluetooth connection request
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_a2dp_sink_conn_reject(bt_conn_req_h conn_req_h);

/****************************************
 * A2DP Sink Role-Specific Functionality
 ****************************************/

/**
 * Function type:  bt_a2dp_sink_set_configuration_req_cb_t
 * Description:    Notifies that an AVDTP Set Configuration Command
 *                 (AVDTP_SET_CONFIGURATION_CMD) was received from the remote
 *                 device.
 *                 NOTE: The application must respond either by immediately
 *                 calling bt_a2dp_sink_set_configuration_rsp() to accept the
 *                 Set Configuration request, or by calling
 *                 bt_a2dp_sink_set_configuration_rej() to reject it.
 * Parameters:
 *     @session:      (IN) Handle to a Bluetooth session object
 *     @codec:        (IN) Pointer toa codec descriptor
 *     @local_ep_id:  (IN) Local endpoint ID
 *     @remote_ep_id: (IN) Remote endpoint ID
 *     @app_ctx_h:    (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_a2dp_sink_set_configuration_req_cb_t)(
    bt_a2dp_sink_session_h session_h, const bt_a2dp_codec_t *codec,
    int_t local_ep_id, int_t remote_ep_id, bt_app_ctx_h app_ctx_h);

/**
 * Function type:  bt_a2dp_sink_stream_start_cb_t
 * Description:    Notifies that an audio stream has been started.
 * Parameters:
 *     @session:     (IN) Handle to a Bluetooth session object
 *     @app_ctx_h:   (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_a2dp_sink_stream_start_cb_t)(bt_a2dp_sink_session_h session_h,
    bt_app_ctx_h app_ctx_h);

/**
 * Function type:  bt_a2dp_sink_stream_suspend_cb_t
 * Description:    Notifies that an audio stream has been suspended.
 * Parameters:
 *     @session:     (IN) Handle to a Bluetooth session object
 *     @app_ctx_h:   (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_a2dp_sink_stream_suspend_cb_t)(
    bt_a2dp_sink_session_h session_h, bt_app_ctx_h app_ctx_h);

/**
 * Function type:  bt_a2dp_sink_stream_input_cb_t
 * Description:    Notifies of input data received on the audio stream.
 * Parameters:
 *     @session:     (IN) Handle to a Bluetooth session object
 *     @buf:         (IN) Pointer to a buffer containing the stream's input data
 *     @len:         (IN) The size data buffer (@buf), in bytes
 *     @app_ctx_h:   (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_a2dp_sink_stream_input_cb_t)(bt_a2dp_sink_session_h session_h,
    void *buf, uint32_t len, bt_app_ctx_h app_ctx_h);

/* A2DP sink role-specific application callback functions */
typedef struct {
    bt_a2dp_sink_set_configuration_req_cb_t set_configuration_req_cb;
#ifdef CONFIG_TWS
	bt_a2dp_sink_set_configuration_req_cb_t  reconfiguration_req_cb;
#endif
    bt_a2dp_sink_stream_start_cb_t          stream_started_cb;
    bt_a2dp_sink_stream_suspend_cb_t        stream_suspended_cb;
    bt_a2dp_sink_stream_input_cb_t          input_cb;
} bt_a2dp_sink_cbs_t;

/**
 * Function name:  bt_a2dp_sink_register
 * Description:    Registers the application to use the A2dP sink driver's
 *                 role-specific APIs.
 * Parameters:
 *     @cbs: (IN)  Pointer to a structure of A2dP sink role-specific
 *                 application callback functions
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_a2dp_sink_register(const bt_a2dp_sink_cbs_t *cbs);

/**
 * Function name:  bt_a2dp_sink_unregister
 * Description:    Unregisters the application from using the A2dP sink
 *                 driver's role-specific APIs.
 * Parameters:     None
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_a2dp_sink_unregister(void);

/* Stream input method */
enum {
    A2DP_INPUT_METHOD_RAW,       /* Raw data */
    A2DP_INPUT_METHOD_SBC_TO_PCM /* Pulse code modulation (PCM) - use Jungo's
                                    SBC codec implementation to decode SBC data
                                    to PCM. This method is available only for
                                    SBC data, when CONFIG_PKG_SBC is set. */
};
typedef uint8_t bt_a2dp_sink_input_method_t;

/**
 * Function name:  bt_a2dp_sink_set_configuration_rsp
 * Description:    Issues an AVDTP Set Configuration Response
 *                 (AVDTP_SET_CONFIGURATION_RSP) to accept a previous Set
 *                 Configuration request from the remote device (see
 *                 bt_a2dp_sink_set_configuration_req_cb_t).
 * Parameters:
 *     @session_h:    (IN) Handle to a Bluetooth session object
 *     @method:       (IN) Stream input method (see bt_a2dp_sink_input_method_t)
 *     @local_ep_id:  (IN) Local endpoint ID
 *     @remote_ep_id: (IN) Remote endpoint ID
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_a2dp_sink_set_configuration_rsp(bt_a2dp_sink_session_h session_h,
    bt_a2dp_sink_input_method_t method, int_t local_ep_id, int_t remote_ep_id);

/**
 * Function name:  bt_a2dp_sink_set_configuration_rej
 * Description:    Issues an AVDTP Set Configuration Reject
 *                 (AVDTP_SET_CONFIGURATION_REJ) to reject a previous Set
 *                 Configuration request from the remote device (see
 *                 bt_a2dp_sink_set_configuration_req_cb_t).
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @local_ep_id:  (IN) Local endpoint ID
 *     @remote_ep_id: (IN) Remote endpoint ID
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_a2dp_sink_set_configuration_rej(bt_a2dp_sink_session_h session_h,
    int_t local_ep_id, int_t remote_ep_id);

/**
 * Function name:  bt_a2dp_sink_get_default_sbc
 * Description:    Returns SBC codec-specific information elements.
 *                 NOTE: The application can use the returned structure for the
 *                 SBC codec descriptor of the stream endpoint that it passes
 *                 to bt_a2dp_sink_conn_connect() or bt_a2dp_sink_conn_accept()
 *                 (ep->codecs[i].u.sbc).

 * Parameters:
 *     @sbc: (OUT) Pointer to a preallocated SBC codec-specific information
 *                 elements structure, to be filled by the function
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_a2dp_sink_get_default_sbc(bt_a2dp_sbc_t *sbc);
#ifdef CONFIG_TWS
result_t bt_a2dp_sink_reconfig_res(bt_a2dp_sink_session_h session,
    int_t tid, int_t remote_ep_id, int_t acp_or_rej );

#endif

#endif

