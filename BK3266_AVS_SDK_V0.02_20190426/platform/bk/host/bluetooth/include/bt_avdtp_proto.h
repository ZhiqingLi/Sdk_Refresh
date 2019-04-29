/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_AVDTP_PROTO_H_
#define _BT_AVDTP_PROTO_H_

/*
 * Header for working with AVDTP protocol directly.
 */

#include <bt_avdtp.h>

typedef struct avdtp_session *avdtp_session_h;
typedef struct avdtp_stream *avdtp_stream_h;

/*
 * Upper layers custom commands
 */

#define BT_AVDTP_CMD_START_REQ      1
#define BT_AVDTP_CMD_SUSPEND_REQ    2
#define BT_AVDTP_CMD_START_RSP      3
#define BT_AVDTP_CMD_SUSPEND_RSP    4
#ifdef CONFIG_TWS
#define BT_AVDTP_CMD_RECONFIG_REQ   5
#endif
/* AVDTP session protocol */
result_t avdtp_session_attach(avdtp_session_h *ss, const struct btproto *proto,
    void *upper);
result_t avdtp_session_detach(avdtp_session_h *ss);
result_t avdtp_session_bind(avdtp_session_h ss, bt_sockaddr_t *laddr);
result_t avdtp_session_sockaddr(avdtp_session_h ss, bt_sockaddr_t *laddr);
result_t avdtp_session_connect(avdtp_session_h ss, bt_sockaddr_t *raddr,
    bt_link_modes_mask_t mode);
result_t avdtp_session_peeraddr(avdtp_session_h ss, bt_sockaddr_t *raddr);
result_t avdtp_session_disconnect(avdtp_session_h ss, int_t linger);
result_t avdtp_session_listen(avdtp_session_h ss);
result_t avdtp_session_accept(avdtp_session_h ss, bt_sockaddr_t *laddr,
    bt_sockaddr_t *raddr, bt_link_modes_mask_t mode);
result_t avdtp_session_reject(bt_sockaddr_t *laddr, bt_sockaddr_t *raddr);
result_t avdtp_session_send(avdtp_session_h ss, struct mbuf *m);
result_t avdtp_session_rcvd(avdtp_session_h ss, int32_t space);
result_t avdtp_session_setopt(avdtp_session_h ss, uint8_t type, void *sopt,
    uint32_t size);
result_t avdtp_session_getopt(avdtp_session_h ss, uint8_t type, void *sopt,
    uint32_t size);

/* AVDTP stream protocol */
result_t avdtp_stream_attach(avdtp_stream_h *st, const struct btproto *proto,
    void *upper);
result_t avdtp_stream_detach(avdtp_stream_h *st);
result_t avdtp_stream_bind(avdtp_stream_h st, bt_sockaddr_t *laddr);
result_t avdtp_stream_sockaddr(avdtp_stream_h st, bt_sockaddr_t *laddr);
result_t avdtp_stream_connect(avdtp_stream_h st, bt_sockaddr_t *raddr);
result_t avdtp_stream_peeraddr(avdtp_stream_h st, bt_sockaddr_t *raddr);
result_t avdtp_stream_disconnect(avdtp_stream_h st, int_t linger);
result_t avdtp_stream_listen(avdtp_stream_h st);
result_t avdtp_stream_accept(avdtp_stream_h st, bt_sockaddr_t *laddr,
    bt_sockaddr_t *raddr, bt_link_modes_mask_t mode);
result_t avdtp_stream_reject(bt_sockaddr_t *laddr, bt_sockaddr_t *raddr);
result_t avdtp_stream_send(avdtp_stream_h st, struct mbuf *m);
result_t avdtp_stream_rcvd(avdtp_stream_h st, int32_t space);
result_t avdtp_stream_setopt(avdtp_stream_h st, uint8_t type, void *sopt,
    uint32_t size);
result_t avdtp_stream_getopt(avdtp_stream_h st, uint8_t type, void *sopt,
    uint32_t size);
result_t avdtp_stream_start(avdtp_stream_h st);
result_t avdtp_stream_suspend(avdtp_stream_h st);
#ifdef CONFIG_TWS
result_t avdtp_stream_reconfig_res(avdtp_stream_h st,
    int_t tid, int_t remote_ep_id, int_t acp_or_rej );
result_t avdtp_stream_cap_clear( avdtp_stream_h st );
#endif


#endif
