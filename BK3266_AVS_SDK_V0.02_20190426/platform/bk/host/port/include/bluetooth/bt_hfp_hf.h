/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef __BT_HFP_HF_H__
#define __BT_HFP_HF_H__

/***************************************************************
 * Bluetooth Hands-Free Profile (HFP) Hands-Free (HF) Role API *
 ***************************************************************/

#include "bt_at_types.h"
#include <bt_api.h>

/* Handle to a Bluetooth session object */
typedef struct bt_hfp_hf_session_s *bt_hfp_hf_session_h;
/* Handle to a local Bluetooth server */
typedef struct bt_hfp_hf_session_s *bt_hfp_hf_server_h;
typedef struct {
    uint32_t                    svc_id;
    uint32_t                    server_sdp;
    bt_hfp_hf_server_h          server;
    bt_hfp_hf_session_h         session;

    btaddr_t                    laddr;
    btaddr_t                    raddr;
    bt_conn_req_h               conn_req;
    bt_link_modes_mask_t        conn_req_mode;

    int8_t                      is_used;
    int8_t                     	dir;
    int8_t                     	stat;

    int8_t                      volume;
    int8_t                      pending_wave_id;
    int8_t                      ring_count;
    int8_t                      ring_num;

    int8_t                      vnum[32];
	
    /* sco_utils_h                 priv; */
	void *						priv;
	uint32_t                      freq;
    uint32_t                    flag;
    uint32                      flag_establish_call;
} hfp_hf_app_t;

/**************************
 * Bluetooth Connectivity *
 **************************/

/**
 * Function type:  bt_hfp_hf_connected_cb_t
 * Description:    Notifies that a Bluetooth connection with a remote device
 *                 has been established.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @app_ctx_h: (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_hfp_hf_connected_cb_t)(
    bt_hfp_hf_session_h session_h, bt_app_ctx_h app_ctx_h);

/**
 * Function type:  bt_hfp_hf_disconnected_cb_t
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
typedef void (*bt_hfp_hf_disconnected_cb_t)(
    bt_hfp_hf_session_h session_h, result_t status, bt_app_ctx_h app_ctx_h);

/* Bluetooth connection-status application notification callback functions */
typedef struct {
    bt_hfp_hf_connected_cb_t    connected_cb;
    bt_hfp_hf_disconnected_cb_t disconnected_cb;
} bt_hfp_hf_conn_cbs_t;

/**
 * Function name:  bt_hfp_hf_conn_create
 * Description:    Creates a new Bluetooth session object, and registers the
 *                 application's Bluetooth connectivity callback functions for
 *                 the related session with the driver.
 *                 NOTE: The application must call this function before calling
 *                 bt_hfp_hf_conn_connect() to initiate a new connection, or
 *                 bt_hfp_hf_conn_accept() to accept a connection request.
 *                 The application is responsible for destroying the session
 *                 object, by calling bt_hfp_hf_conn_destroy().
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
result_t bt_hfp_hf_conn_create(bt_hfp_hf_session_h *session_h,
    bt_hfp_hf_conn_cbs_t *conn_cbs, bt_app_ctx_h app_ctx_h);

/**
 * Function name:  bt_hfp_hf_conn_destroy
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
void bt_hfp_hf_conn_destroy(bt_hfp_hf_session_h *session_h);

/**
 * Function name:  bt_hfp_hf_conn_disconnect
 * Description:    Initiates a disconnect of a Bluetooth connection with a
 *                 remote device.
 *                 NOTE: The driver will respond by calling the application's
 *                 bt_hfp_hf_disconnected_cb_t callback.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_conn_disconnect(bt_hfp_hf_session_h session_h);

/*--------------------------------
   Client-Specific Connection API
  --------------------------------*/

/**
 * Function name:  bt_hfp_hf_conn_connect
 * Description:    Issues a Bluetooth connection request to a remote device.
 *                 NOTE: The driver will respond by calling the application's
 *                 bt_hfp_hf_connected_cb_t callback if a connection is
 *                 established, or the bt_hfp_hf_disconnected_cb_t callback
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
result_t bt_hfp_hf_conn_connect(bt_hfp_hf_session_h session_h,
    const btaddr_t *laddr, const btaddr_t *raddr, uint8_t channel);

/*--------------------------------
   Server-Specific Connection API
  --------------------------------*/

/**
 * Function type:  bt_hfp_hf_newconn_cb_t
 * Description:    Notifies of a request from a remote Bluetooth device to
 *                 connect to the local Bluetooth server.
 *                 NOTE: The application must either accept the request by
 *                 calling bt_hfp_hf_conn_accept(), or reject it by calling
 *                 bt_hfp_hf_conn_reject().
 * Parameters:
 *     @conn_req_h: (IN) Handle to a Bluetooth connection request
 *     @laddr:      (IN) Pointer to a local Bluetooth address
 *     @raddr:      (IN) Pointer to a remote Bluetooth address
 *     @mode:       (IN) Bluetooth link modes mask -- see bt_link_modes_mask_t
 *     @app_ctx_h:  (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_hfp_hf_newconn_cb_t)(bt_conn_req_h conn_req_h,
    const btaddr_t *laddr, const btaddr_t *raddr, bt_link_modes_mask_t mode,
    bt_app_ctx_h app_ctx_h);

/**
 * Function name:  bt_hfp_hf_server_start
 * Description:    Starts a local Bluetooth server.
 * Parameters:
 *     @server_h:       (OUT) Pointer to a handle to a local Bluetooth server,
 *                            to be set by the function
 *     @laddr:          (IN)  Pointer to a local Bluetooth address
 *     @rfcomm_channel: (IN)  RFCOMM channel
 *     @newconn_cb:     (IN)  Remote Bluetooth connection request notification
 *                            callback -- see bt_hfp_hf_newconn_cb_t
 *     @app_ctx_h:      (IN)  Handle to a Bluetooth application context
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_server_start(bt_hfp_hf_server_h *server_h,
    const btaddr_t *laddr, uint8_t rfcomm_channel,
    bt_hfp_hf_newconn_cb_t newconn_cb, bt_app_ctx_h app_ctx_h);

/**
 * Function name:  bt_hfp_hf_server_stop
 * Description:    Stops a local Bluetooth server.
 * Parameters:
 *     @server_h: (IN) Handle to a local Bluetooth server
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_hfp_hf_server_stop(bt_hfp_hf_server_h *server_h);

/**
 * Function name:  bt_hfp_hf_conn_accept
 * Description:    Accepts a Bluetooth connection request from a remote device.
 *                 NOTE: Before calling this function, the application must
 *                 call bt_hfp_hf_conn_create().
 *                 The driver will respond by calling the application's
 *                 bt_hfp_hf_connected_cb_t callback -- if a connection
 *                 was established -- or its bt_hfp_hf_disconnected_cb_t
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
result_t bt_hfp_hf_conn_accept(bt_hfp_hf_session_h session_h,
    bt_conn_req_h conn_req_h, bt_link_modes_mask_t mode);

/**
 * Function name:  bt_hfp_hf_conn_reject
 * Description:    Rejects a Bluetooth connection request from a remote device.
 * Parameters:
 *     @conn_req_h: (IN) Handle to a Bluetooth connection request
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_hfp_hf_conn_reject(bt_conn_req_h conn_req_h);

/*********************************************
 * HFP Hands-Free Role-Specific Functionality
 *********************************************/

/**
 * Function type:  bt_hfp_hf_get_features_cb_t
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
typedef void (*bt_hfp_hf_get_features_cb_t)(bt_hfp_hf_session_h session_h,
    uint16_t *supported_features, bt_app_ctx_h app_ctx);

/**
 * Function type:  bt_hfp_hf_at_result_cb_t
 * Description:    Notifies that a Bluetooth connection with a remote device
 *                 has been established.
 * Parameters:
 *     @session:     (IN) Handle to a Bluetooth session object
 *     @at_result_t: (IN) Pointer to an AT result structure
 *     @app_ctx_h:   (IN) Handle to a Bluetooth application context
 *
 * Return value:   None
 **/
typedef void (*bt_hfp_hf_at_result_cb_t)(bt_hfp_hf_session_h session_h,
    at_result_t *at_result, bt_app_ctx_h app_ctx);

/* HFP HF role-specific application callback functions */
typedef struct {
    bt_hfp_hf_at_result_cb_t            at_result_cb;
    bt_hfp_hf_get_features_cb_t         get_features_cb;
} bt_hfp_hf_cbs_t;

/**
 * Function name:  bt_hfp_hf_register
 * Description:    Registers the application to use the HFP HF driver's
 *                 role-specific APIs.
 * Parameters:
 *     @cbs: (IN)  Pointer to a structure of HFP HF role-specific application
 *                 callback functions
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_register(const bt_hfp_hf_cbs_t *cbs);

/**
 * Function name:  bt_hfp_hf_unregister
 * Description:    Unregisters the application from using the HFP HF driver's
 *                 role-specific APIs.
 * Parameters:     None
 *
 * Return value:   None
 * Scope:          Global
 **/
void bt_hfp_hf_unregister(void);

/**
 * Function name:  bt_hfp_hf_vgm
 * Description:    Issues a gain of microphone command (AT_COMMAND_VGM) from
 *                 the HF to the AG, to report the current HF microphone gain
 *                 level.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @mic_gain:  (IN) Microphone gain level (0--15)
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_vgm(bt_hfp_hf_session_h session_h, uint8_t mic_gain);

/**
 * Function name:  bt_hfp_hf_vgs
 * Description:    Issues a gain of speaker command (AT_COMMAND_VGS) from the
 *                 HF to the AG, to report the current HF speaker gain level.
 * Parameters:
 *     @session_h:    (IN) Handle to a Bluetooth session object
 *     @speaker_gain: (IN) Speaker gain level (0--15)
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_vgs(bt_hfp_hf_session_h session_h, uint8_t speaker_gain);

/**
 * Function name:  bt_hfp_hf_accept_call
 * Description:    Issues a standard call answer command (AT_COMMAND_ATA) from
 *                 the HF to the AG, to accept an incoming call.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_accept_call(bt_hfp_hf_session_h session);

/**
 * Function name:  bt_hfp_hf_hangup_call
 * Description:    Issues a standard call hang-up command (AT_COMMAND_CHUP)
 *                 from the HF to the AG, to terminate an active call.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_hangup_call(bt_hfp_hf_session_h session);

/**
 * Function name:  bt_hfp_hf_place_call
 * Description:    Issues a standard dial command (AT_COMMAND_ATD) from the HF
 *                 to the AG, to place a call to a phone number.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @number:    (IN) Phone number string
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_place_call(bt_hfp_hf_session_h session_h, char *number);

/**
 * Function name:  bt_hfp_hf_redial
 * Description:    Issues a Bluetooth last dialed number command
 *                 (AT_COMMAND_BLDN) from the HF to the AG, to call the last
 *                 dialed phone number.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_redial(bt_hfp_hf_session_h session);

/**
 * Function name:  bt_hfp_hf_resp_and_hold_set
 * Description:    Issues a Bluetooth Response and Hold feature set command
 *                 (AT_COMMAND_BTRH_SET) from the HF to the AG, to set the AG's
 *                 Response and Hold state.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @command:   (IN) AG Response and Hold feature set command -- see
 *                      at_cmd_btrh_t
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_resp_and_hold_set(bt_hfp_hf_session_h session_h,
    at_cmd_btrh_t command);

/**
 * Function name:  bt_hfp_hf_resp_and_hold_read
 * Description:    Issues a Bluetooth Response and Hold feature read current
 *                 status command (AT_COMMAND_BTRH_READ) from the HF to the AG,
 *                 to query the AG's current Response and Hold state.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_resp_and_hold_read(bt_hfp_hf_session_h session);

/**
 * Function name:  bt_hfp_hf_chld
 * Description:    Issues a standard call-hold and multi-party handling command
 *                 (AT_COMMAND_CHLD) from the HF to the AG, to handle a
 *                 three-way call.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @command:   (IN) Call-hold and multi-party handling command -- see
 *                      at_cmd_chld_t
 *     @call_id: (IN) ID of the call to which the command applies -- for
 *                    commands that apply to a specific call (see
 *                    at_cmd_chld_t); -1 if N/A
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_chld(bt_hfp_hf_session_h session_h, at_cmd_chld_t command,
    int8_t call_id);

/**
 * Function name:  bt_hfp_hf_list_current_calls
 * Description:    Issues a standard list of current calls command
 *                 (AT_COMMAND_CLCC) from the HF to the AG, to query the list
 *                 of current calls in the AG.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_list_current_calls(bt_hfp_hf_session_h session);

/**
 * Function name:  bt_hfp_hf_set_call_wait_notif
 * Description:    Issues a Call Waiting notification command (AT_COMMAND_CCWA)
 *                 from the HF to the AG, to enable or disable this
 *                 notification (according to the @enable parameter).
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @enable:    (IN) 1 -- Enable the Call Waiting notification
 *                      0 -- Disable the Call Waiting notification
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_set_call_wait_notif(bt_hfp_hf_session_h session_h,
    uint8_t enable);

/**
 * Function name:  bt_hfp_hf_set_cli_notif
 * Description:    Issues a Call Line Identification (CLI) notification command
 *                 (AT_COMMAND_CLIP) from the HF to the AG, to enable or disable
 *                 this notification (according to the @enable parameter).
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @enable:    (IN) 1 -- Enable the Call Line Identification notification
 *                      0 -- Disable the Call Line Identification notification
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_set_cli_notif(bt_hfp_hf_session_h session_h, uint8_t enable);

result_t bt_hfp_hf_set_voice_recog(bt_hfp_hf_session_h session, uint8_t enable);
result_t bt_hfp_hf_trans_DTMF(bt_hfp_hf_session_h session, char *code);
#ifdef CONFIG_PRODUCT_TEST_INF
result_t bt_hfp_hf_trans_buttoncode(bt_hfp_hf_session_h session, uint32 value);
#endif
result_t bt_hfp_hf_xtalapp(bt_hfp_hf_session_h session);
result_t bt_hfp_hf_iphoneaccev(bt_hfp_hf_session_h session, int8_t level);

/**
 * Function name:  bt_hfp_hf_set_vr
 * Description:    Issues a Bluetooth voice recognition activation command
 *                 (AT_COMMAND_BVRA) from the HF to the AG, to enable or
 *                 disable the AG's voice recognition function (according to
 *                 the @enable parameter).
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *     @enable:    (IN) 1 -- Enable the AG's voice recognition function
 *                      0 -- Disable the AG's voiced recognition function
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_set_vr(bt_hfp_hf_session_h session_h, uint8_t enable);

/**
 * Function name:  bt_hfp_hf_get_operator
 * Description:    Issues a read network operator command (AT_COMMAND_COPS_READ)
 *                 from the HF to the AG, to read the name of the network
 *                 operator.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_get_operator(bt_hfp_hf_session_h session);

/**
 * Function name:  bt_hfp_hf_get_subscriber
 * Description:    Issues a retrieve subscriber number information command
 *                 (AT_COMMAND_CNUM) from the HF to the AG.
 * Parameters:
 *     @session_h: (IN) Handle to a Bluetooth session object
 *
 * Return value:   UWE_OK on success, otherwise an error code
 * Scope:          Global
 **/
result_t bt_hfp_hf_get_subscriber(bt_hfp_hf_session_h session);
result_t bt_hfp_hf_set_codec(bt_hfp_hf_session_h session,uint8_t codec_id);
result_t bt_hfp_hf_establishment_codec(bt_hfp_hf_session_h session);
hfp_hf_app_t *hfp_get_app_from_session(bt_hfp_hf_session_h session);
#endif

