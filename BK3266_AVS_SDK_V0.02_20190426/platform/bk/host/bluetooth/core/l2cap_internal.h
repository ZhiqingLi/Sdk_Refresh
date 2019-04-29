/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_L2CAP_INTERNAL_H_
#define _BT_L2CAP_INTERNAL_H_

#include <include/uw_types.h>
#include <bluetooth.h>
#include <bt_l2cap.h>
#include "hci_internal.h"

/**************************************************************************
 **          Contains L2CAP Internal definitions and structs             **
 **************************************************************************/

LIST_HEAD(l2cap_channel_list, l2cap_channel);

/* global variables */
extern struct l2cap_channel_list l2cap_active_list;
extern struct l2cap_channel_list l2cap_listen_list;
extern struct pool l2cap_pdu_pool;
extern struct pool l2cap_req_pool;
extern const l2cap_opt_qos_t l2cap_default_qos;

/* sysctl variables */
extern uint32_t l2cap_response_timeout;
extern uint32_t l2cap_response_extended_timeout;
extern int32_t l2cap_sendspace, l2cap_recvspace;

/*
 * L2CAP Channel
 */
typedef struct l2cap_channel {
    hci_link_t          *lc_link;       /* ACL connection (down) */
    uint16_t            lc_state;       /* channel state */
    uint16_t            lc_flags;       /* channel flags */
    uint8_t             lc_ident;       /* cached request id */

    uint16_t            lc_lcid;        /* local channel ID */
    bt_sockaddr_t       lc_laddr;       /* local address */

    uint16_t            lc_rcid;        /* remote channel ID */
    bt_sockaddr_t       lc_raddr;       /* remote address */

    bt_link_modes_mask_t       lc_mode;        /* link mode */
    uint16_t            lc_imtu;        /* incoming mtu */
    uint16_t            lc_omtu;        /* outgoing mtu */
    uint16_t            lc_flush;       /* flush timeout */
    l2cap_opt_qos_t     lc_iqos;        /* incoming QoS flow control */
    l2cap_opt_qos_t     lc_oqos;        /* outgoing Qos flow control */

    uint8_t             lc_pending;     /* num of pending PDUs */
    MBUFQ_HEAD()        lc_txq; /* transmit queue */

    const struct btproto        *lc_proto;      /* upper layer callbacks */
    void                        *lc_upper;      /* upper layer argument */

    LIST_ENTRY(l2cap_channel)   lc_ncid;/* next channel (ascending CID) */
} l2cap_channel_t;

/* l2cap_channel state */
#define L2CAP_CLOSED                0 /* closed */
#define L2CAP_WAIT_SEND_CONNECT_REQ 1 /* waiting to send connect request */
#define L2CAP_WAIT_RECV_CONNECT_RSP 2 /* waiting to recv connect response */
#define L2CAP_WAIT_SEND_CONNECT_RSP 3 /* waiting to send connect response */
#define L2CAP_WAIT_CONFIG           4 /* waiting for configuration */
#define L2CAP_OPEN                  5 /* user data transfer state */
#define L2CAP_WAIT_DISCONNECT       6 /* have sent disconnect request */

/* l2cap_channel flags */
#define L2CAP_SHUTDOWN          (1<<0)  /* channel is closing */
#define L2CAP_WAIT_CONFIG_REQ   (1<<1)  /* waiting for config request */
#define L2CAP_WAIT_CONFIG_RSP   (1<<2)  /* waiting for config response */

/* L2CAP Incoming Connection Request */
typedef struct l2cap_con_req {
    bt_link_modes_mask_t        mode; /* Listening channel security mode */
    uint8_t                     ident;
    uint16_t                    psm;
    uint16_t                    scid;
    TAILQ_ENTRY(l2cap_con_req)  next;
} l2cap_con_req_t;

/*
 * L2CAP Request
 */
struct l2cap_req {
    hci_link_t                  *lr_link;   /* ACL connection */
    struct l2cap_channel        *lr_chan;   /* channel pointer */
    uint8_t                     lr_code;    /* request code */
    uint8_t                     lr_id;      /* request id */
    jtask_h                     lr_rtx;     /* response timer */
    TAILQ_ENTRY(l2cap_req)      lr_next;    /* next request on link */
};

/*
 * L2CAP Protocol Data Unit
 */
struct l2cap_pdu {
    struct l2cap_channel       *lp_chan;    /* PDU owner */
    MBUFQ_HEAD()                lp_data;    /* PDU data */
    TAILQ_ENTRY(l2cap_pdu)      lp_next;    /* next PDU on link */
    uint16_t                    lp_pending; /* # of fragments pending */
};

/*
 * L2CAP function prototypes
 */

struct mbuf;

/* l2cap_lower.c */
void l2cap_close(struct l2cap_channel *, result_t);
void l2cap_recv_frame(struct mbuf *, hci_link_t *);
result_t l2cap_start(struct l2cap_channel *);

/* l2cap_misc.c */
result_t l2cap_setmode(struct l2cap_channel *);
result_t l2cap_cid_alloc(struct l2cap_channel *);
struct l2cap_channel *l2cap_cid_lookup(uint16_t);
result_t l2cap_request_alloc(hci_link_t *, struct l2cap_channel *,
    uint8_t);
struct l2cap_req *l2cap_request_lookup(hci_link_t *, uint8_t);
void l2cap_request_free(struct l2cap_req *);
void l2cap_rtx(void *);

/* l2cap_signal.c */
void l2cap_recv_signal(struct mbuf *, hci_link_t *);
result_t l2cap_send_connect_req(struct l2cap_channel *);
result_t l2cap_send_config_req(struct l2cap_channel *);
result_t l2cap_send_disconnect_req(struct l2cap_channel *);
result_t l2cap_send_connect_rsp(hci_link_t *, uint8_t, uint16_t, uint16_t,
    uint16_t);
result_t l2cap_send_echo_req(hci_link_t *lc_link);

#ifdef J_DEBUG
const char *l2cap_psm_str(uint16_t psm);
void l2cap_listen_list_dump(const char *file, int_t line);
void l2cap_active_list_dump(const char *file, int_t line);
#define DBG_L2CAP_LISTEN_LIST(level) \
    DBG_RUN(DBT_L2CAP, level, l2cap_listen_list_dump(__FILE__, __LINE__))
#define DBG_L2CAP_ACTIVE_LIST(level) \
    DBG_RUN(DBT_L2CAP, level, l2cap_active_list_dump(__FILE__, __LINE__))
#else
#define DBG_L2CAP_LISTEN_LIST(level) do {} while(0)
#define DBG_L2CAP_ACTIVE_LIST(level) do {} while(0)
#endif

#endif  /* _BT_L2CAP_INTERNAL_H_ */

