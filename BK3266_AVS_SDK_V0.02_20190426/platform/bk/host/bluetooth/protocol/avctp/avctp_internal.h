/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _AVCTP_INTERNAL_H_
#define _AVCTP_INTERNAL_H_

#include <bt_avctp.h>
#include <bt_l2cap_proto.h>
#include "bt_avctp_proto.h"
#include "bt_avrcp_ctrl.h"

typedef struct avctp_lower *avctp_lower_h;
typedef struct avctp_upper *avctp_upper_h;

typedef struct avctp_lower {
    avctp_upper_h   upper;
    l2cap_channel_h l2cap;
} avctp_lower_t;

typedef struct avctp_upper {
    const btproto_t            *proto;
    void                       *ctx;
    uint16_t                   pid;

    avctp_lower_h             lower;
    result_t                  fatal_err;
} avctp_upper_t;

typedef enum {
    AVRCP_SESSION_DETACHED = 0,
    AVRCP_SESSION_ATTACHED
} avrcp_session_state_t;

typedef struct bt_avrcp_session_s {
    LIST_ENTRY(bt_avrcp_session_s)  next;
    avrcp_session_state_t           state;
    avctp_channel_h                 avctp_h;

    /* TODO: move to server specific struct */
    bt_avrcp_ctrl_newconn_cb_t      newconn_cb;

    bt_avrcp_ctrl_conn_cbs_t        conn_cbs;
    bt_app_ctx_h                    app_ctx;
} avrcp_session_t;

/* avctp_lower.c functions */
result_t avctp_lower_attach(avctp_upper_h upper, avctp_lower_h *lower);
void avctp_lower_detach(avctp_lower_h lower);

result_t avctp_lower_connect(avctp_lower_h lower, bt_sockaddr_t *raddr,
    bt_link_modes_mask_t mode);
result_t avctp_lower_bind(avctp_lower_h lower, bt_sockaddr_t *laddr);
result_t avctp_lower_listen(avctp_lower_h lower);

result_t avctp_lower_accept(avctp_lower_h lower, bt_sockaddr_t *laddr,
    bt_sockaddr_t *raddr, bt_link_modes_mask_t mode);
result_t avctp_lower_reject(bt_sockaddr_t *laddr, bt_sockaddr_t *raddr);
result_t avctp_lower_disconnect(avctp_lower_h lower, int_t linger);

result_t avctp_lower_send(avctp_lower_h lower, bool_t ipid, uint16_t pid,
    avctp_tid_t tid, avctp_msg_type_t type, struct mbuf *m);

/* avctp_upper.c functions */
void avctp_upper_connecting(avctp_upper_h upper);
void avctp_upper_connected(avctp_upper_h upper);
void avctp_upper_disconnected(avctp_upper_h upper, result_t err);
void avctp_upper_newconn(avctp_upper_h upper, bt_sockaddr_t *laddr,
    bt_sockaddr_t *raddr, bt_link_modes_mask_t mode, void *arg);
void avctp_upper_complete(avctp_upper_h upper, int32_t count, void *arg);
void avctp_upper_linkmode(avctp_upper_h upper, bt_link_modes_mask_t new_mode);

void avctp_upper_input(avctp_upper_h upper, bool_t ipid, avctp_pid_t pid,
    avctp_tid_t tid, avctp_msg_type_t type, struct mbuf *m);

result_t avctp_lower_peeraddr(avctp_lower_t *lower, bt_sockaddr_t *raddr);
result_t avctp_peeraddr(avctp_upper_t *avctp, bt_sockaddr_t *raddr);
result_t avrcp_peeraddr(avrcp_session_t *session, btaddr_t *raddr);

#endif

