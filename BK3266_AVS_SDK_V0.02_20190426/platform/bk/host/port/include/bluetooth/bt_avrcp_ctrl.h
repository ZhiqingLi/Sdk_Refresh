/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_AVRCP_CTRL_H_
#define _BT_AVRCP_CTRL_H_

#include <bt_avrcp_types.h>
#include <bt_avctp.h>
#include <bt_api.h>

typedef struct bt_avrcp_session_s *bt_avrcp_ctrl_session_h;
typedef struct bt_avrcp_session_s *bt_avrcp_ctrl_server_h;

/***********************************************************************
 *  Bluetooth AVRCP Control Service Connection Establishment Functions *
 ***********************************************************************/

typedef void (*bt_avrcp_ctrl_connected_cb_t)(
    bt_avrcp_ctrl_session_h session_h, bt_app_ctx_h app_ctx_h);
typedef void (*bt_avrcp_ctrl_disconnected_cb_t)(
    bt_avrcp_ctrl_session_h session_h, result_t status, bt_app_ctx_h app_ctx_h);

typedef struct {
    bt_avrcp_ctrl_connected_cb_t    connected_cb;
    bt_avrcp_ctrl_disconnected_cb_t disconnected_cb;
} bt_avrcp_ctrl_conn_cbs_t;

result_t bt_avrcp_ctrl_conn_create(bt_avrcp_ctrl_session_h *session_h,
    bt_avrcp_ctrl_conn_cbs_t *conn_cbs, bt_app_ctx_h app_ctx_h);
void bt_avrcp_ctrl_conn_destroy(bt_avrcp_ctrl_session_h *session_h);

result_t bt_avrcp_ctrl_conn_connect(bt_avrcp_ctrl_session_h session_h,
    const btaddr_t *laddr, const btaddr_t *raddr);
result_t bt_avrcp_ctrl_conn_accept(bt_avrcp_ctrl_session_h session_h,
    bt_conn_req_h conn_req_h, bt_link_modes_mask_t mode);
void bt_avrcp_ctrl_conn_reject(bt_conn_req_h conn_req_h);
result_t bt_avrcp_ctrl_conn_disconnect(bt_avrcp_ctrl_session_h session_h);

typedef void (*bt_avrcp_ctrl_newconn_cb_t)(bt_conn_req_h conn_req_h,
    const btaddr_t *laddr, const btaddr_t *raddr, bt_link_modes_mask_t mode,
    bt_app_ctx_h app_ctx_h);

result_t bt_avrcp_ctrl_server_start(bt_avrcp_ctrl_server_h *server_h,
    const btaddr_t *laddr, bt_avrcp_ctrl_newconn_cb_t newconn_cb,
    bt_app_ctx_h app_ctx_h);
void bt_avrcp_ctrl_server_stop(bt_avrcp_ctrl_server_h *server_h);

/*****************************************************
 *  Bluetooth AVRCP Control Service Driver Functions *
 *****************************************************/

typedef void (*bt_avrcp_ctrl_pass_through_cb_t)(
    bt_avrcp_ctrl_session_h session_h, avrcp_tid_t tid, avc_response_t rsp,
    bt_app_ctx_h app_ctx_h);

typedef void (*bt_avrcp_ctrl_getcaps_events_cb_t)(
    bt_avrcp_ctrl_session_h session_h, avrcp_tid_t tid, avc_response_t response,
    avrcp_event_id_t *events, uint8_t count, avrcp_pkt_type_t packet_type,
    avrcp_op_t bt_avrcp_continuation_tag, bt_app_ctx_h app_ctx_h);

typedef void (*bt_avrcp_ctrl_event_cb_t)(bt_avrcp_ctrl_session_h session_h,
    avrcp_tid_t tid, avc_response_t response, avrcp_event_rsp_t *ev,
    avrcp_pkt_type_t packet_type, avrcp_op_t bt_avrcp_continuation_tag,
    bt_app_ctx_h app_ctx_h);

typedef void (*bt_avrcp_ctrl_get_element_attr_cb_t)(
    bt_avrcp_ctrl_session_h session_h,
    avrcp_tid_t tid, avc_response_t rsp, uint32_t attr_id, uint16_t char_set_id,
    uint16_t attr_total_len, char *attr_val, uint16_t attr_val_len,
    avrcp_pkt_type_t packet_type, avrcp_op_t bt_avrcp_continuation_tag,
    bt_app_ctx_h app_ctx_h);

typedef void (*bt_avrcp_ctrl_get_play_status_cb_t)(
    bt_avrcp_ctrl_session_h session_h, avrcp_tid_t tid, avc_response_t rsp,
    avrcp_get_play_status_rsp_t *status, avrcp_pkt_type_t packet_type,
    avrcp_op_t bt_avrcp_continuation_tag, bt_app_ctx_h app_ctx_h);

#ifdef CONFIG_TWS
typedef void (*bt_avrcp_ctrl_get_passthrough_req_cb_t) (
    bt_avrcp_ctrl_session_h session_h, avrcp_tid_t tid, uint8_t op_id,
    void *payload, uint8_t len );
#endif

typedef struct {
    bt_avrcp_ctrl_pass_through_cb_t         recv_pass_through_resp_cb;
    bt_avrcp_ctrl_getcaps_events_cb_t       recv_getcaps_events_resp_cb;
    bt_avrcp_ctrl_event_cb_t                recv_event_resp_cb;
    bt_avrcp_ctrl_get_element_attr_cb_t     recv_get_element_attr_resp_cb;
    bt_avrcp_ctrl_get_play_status_cb_t      recv_get_play_status_resp_cb;
#ifdef CONFIG_TWS
	bt_avrcp_ctrl_get_passthrough_req_cb_t	recv_get_passthough_req_cb;
#endif
} bt_avrcp_ctrl_cbs_t;

result_t bt_avrcp_ctrl_register(const bt_avrcp_ctrl_cbs_t *cbs);
void bt_avrcp_ctrl_unregister(void);

result_t bt_avrcp_send_pass_through_cmd(bt_avrcp_ctrl_session_h session_h,
    avrcp_tid_t tid, avc_op_t avc_op, avc_click_t click);
#ifdef CONFIG_TWS
// result_t bt_avrcp_send_sync_cmd(bt_avrcp_ctrl_session_h session,
//    avrcp_tid_t tid, avc_op_t avc_op, uint16_t timestamp );

// result_t avrcp_send_vendor_cmd(bt_avrcp_ctrl_session_h session, avrcp_tid_t tid,
//    avc_ctype_t ctype, avrcp_op_t pdu_id, void *payload, uint16_t payload_size);
result_t avrcp_send_bk_data(bt_avrcp_ctrl_session_h session, avrcp_tid_t tid,
   void *payload, uint16_t payload_size);

#endif

#ifdef CONFIG_PRODUCT_TEST_INF
result_t avrcp_send_bk_product_test(bt_avrcp_ctrl_session_h session, avrcp_tid_t tid,uint8 type,
    void* payload, uint16_t payload_size);
#endif

result_t bt_avrcp_send_get_capabilities_cmd(bt_avrcp_ctrl_session_h session_h,
    avrcp_tid_t tid, avrcp_capability_id_t cap);

result_t bt_avrcp_send_register_notification_cmd(
    bt_avrcp_ctrl_session_h session_h, avrcp_tid_t tid, avrcp_event_id_t evt,
    uint32_t playback_pos_changed_interval);

result_t bt_avrcp_send_get_element_attr_cmd(bt_avrcp_ctrl_session_h session_h,
    avrcp_tid_t tid, avrcp_media_attr_id_t attr_id);

result_t bt_avrcp_send_get_play_status_cmd(bt_avrcp_ctrl_session_h session_h,
    avrcp_tid_t tid);

result_t bt_avrcp_send_request_continuing_response_cmd(
    bt_avrcp_ctrl_session_h session_h, avrcp_tid_t tid,
    avrcp_op_t bt_avrcp_continuation_tag);

result_t bt_avrcp_send_abort_continuing_response_cmd(
    bt_avrcp_ctrl_session_h session_h, avrcp_tid_t tid,
    avrcp_op_t bt_avrcp_continuation_tag);

#endif

