/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_RFCOMM_INTERNAL_H_
#define _BT_RFCOMM_INTERNAL_H_

#include <include/uw_types.h>
#include <bt_rfcomm.h>
#include <bt_l2cap_proto.h>
#include "bluetooth.h"

/*************************************************************************
 **          Contains RFCOMM Internal definitions and structs           **
 *************************************************************************/

/* sysctl variables */
extern int32_t rfcomm_sendspace;
extern int32_t rfcomm_recvspace;
extern uint16_t rfcomm_mtu_default;
extern int_t rfcomm_ack_timeout;
extern int_t rfcomm_mcc_timeout;

/*
 * Bluetooth RFCOMM session data
 * One L2CAP connection == one RFCOMM session
 */

/* Credit note */
struct rfcomm_credit {
    STAILQ_ENTRY(rfcomm_credit) rc_next;       /* next credit */
    struct rfcomm_dlc           *rc_dlc;        /* owner */
    uint16_t                     rc_len;        /* length */
};

/* RFCOMM session data (one L2CAP channel) */
typedef struct rfcomm_session {
    l2cap_channel_h              rs_l2cap;      /* L2CAP pointer */
    uint16_t                     rs_flags;      /* session flags */
    uint16_t                     rs_state;      /* session state */
    uint16_t                     rs_mtu;        /* default MTU */

    STAILQ_HEAD(,rfcomm_credit)  rs_credits;    /* credit notes */
    LIST_HEAD(,rfcomm_dlc)       rs_dlcs;       /* DLC list */
    TAILQ_HEAD(,rfcomm_con_req)  rs_con_req;    /* Connection requests list */

    jtask_h                      rs_timeout;    /* timeout */

    LIST_ENTRY(rfcomm_session)   rs_next;       /* next session */
} rfcomm_session_t;

LIST_HEAD(rfcomm_session_list, rfcomm_session);
extern struct rfcomm_session_list rfcomm_session_active;
extern struct rfcomm_session_list rfcomm_session_listen;

/* Session state */
#define RFCOMM_SESSION_CLOSED           0
#define RFCOMM_SESSION_WAIT_CONNECT     1
#define RFCOMM_SESSION_OPEN             2
#define RFCOMM_SESSION_WAIT_DISCONNECT  3
#define RFCOMM_SESSION_LISTEN           4

/* Session flags */
#define RFCOMM_SESSION_INITIATOR        (1 << 0) /* we are initiator */
#define RFCOMM_SESSION_CFC              (1 << 1) /* credit flow control */
#define RFCOMM_SESSION_LFC              (1 << 2) /* local flow control */
#define RFCOMM_SESSION_RFC              (1 << 3) /* remote flow control */
#define RFCOMM_SESSION_FREE             (1 << 4) /* self lock out for free */

#define IS_INITIATOR(rs)        ((rs)->rs_flags & RFCOMM_SESSION_INITIATOR)

/* Bluetooth RFCOMM DLC data (connection) */
struct rfcomm_dlc {
    rfcomm_session_t     *rd_session; /* RFCOMM session */
    uint8_t              rd_dlci;    /* RFCOMM DLCI */

    uint16_t             rd_flags;   /* DLC flags */
    uint16_t             rd_state;   /* DLC state */
    uint16_t             rd_mtu;     /* MTU */
    bt_link_modes_mask_t        rd_mode;    /* link mode */

    bt_sockaddr_t        rd_laddr;   /* local address */
    bt_sockaddr_t        rd_raddr;   /* remote address */

    uint8_t              rd_lmodem;  /* local modem signals */
    uint8_t              rd_rmodem;  /* remote modem signals */
    int8_t               rd_brk;

    uint8_t              rd_rxcred;  /* receive credits (sent) */
    int32_t              rd_rxsize;  /* receive buffer (bytes, avail) */
    int_t                rd_txcred;  /* transmit credits (unused) */
    int_t                rd_pending; /* packets sent but not complete */

    jtask_h              rd_timeout; /* timeout */
    struct mbuf          *rd_txbuf;  /* transmit buffer */

    const struct btproto *rd_proto;  /* upper layer callbacks */
    void                 *rd_upper;  /* upper layer argument */

    LIST_ENTRY(rfcomm_dlc)       rd_next;    /* next dlc on session */
};

typedef result_t (*rfcomm_dlc_newconn_cb_t)(struct rfcomm_dlc *dlc,
    struct rfcomm_session *rs, uint8_t dlci, struct rfcomm_mcc_pn *pn);

/* Bluetooth RFCOMM Connection Request Data */
typedef struct rfcomm_con_req {
    bt_link_modes_mask_t               mode;
    uint8_t                     dlci;
    rfcomm_dlc_newconn_cb_t     cb;
    struct rfcomm_mcc_pn        pn;
    TAILQ_ENTRY(rfcomm_con_req) next; /* next connection request */
} rfcomm_con_req_t;

/*
 * Credit Flow Control works in the following way.
 *
 * txcred is how many packets we can send. Received credit
 * is added to this value, and it is decremented each time
 * we send a packet.
 *
 * rxsize is the number of bytes that are available in the
 * upstream receive buffer.
 *
 * rxcred is the number of credits that we have previously
 * sent that are still unused. This value will be decreased
 * for each packet we receive and we will add to it when we
 * send credits. We calculate the amount of credits to send
 * by the cunning formula "(space / mtu) - sent" so that if
 * we get a bunch of small packets, we can continue sending
 * credits without risking buffer overflow.
 */

/* DLC flags */
#define RFCOMM_DLC_TIMEDOUT             (1 << 0) /* DLC timed out */
#define RFCOMM_DLC_SHUTDOWN             (1 << 1) /* DLC to be shutdown */
#define RFCOMM_DLC_RCVD_MSC_COMMAND     (1 << 2) /* Received MSC command */
#define RFCOMM_DLC_RCVD_MSC_REPONSE     (1 << 3) /* Received MSC response */
#define RFCOMM_DLC_CFC                  (1 << 4) /* prefer not to use cfc */

/* DLC state */
#define RFCOMM_DLC_CLOSED               0       /* no session */
#define RFCOMM_DLC_WAIT_SESSION         1       /* waiting for session */
#define RFCOMM_DLC_WAIT_CONNECT         2       /* waiting for connect */
#define RFCOMM_DLC_WAIT_SEND_SABM       3       /* waiting to send SABM */
#define RFCOMM_DLC_WAIT_SEND_UA         4       /* waiting to send UA */
#define RFCOMM_DLC_WAIT_RECV_UA         5       /* waiting to receive UA */
#define RFCOMM_DLC_OPEN                 6       /* SABM stage done */
#define RFCOMM_DLC_WAIT_DISCONNECT      7       /* waiting for disconnect */
#define RFCOMM_DLC_LISTEN               8       /* listening DLC */

/* rfcomm_dlc.c */
struct rfcomm_dlc *rfcomm_dlc_lookup(rfcomm_session_t *, uint8_t);
void rfcomm_dlc_newconn(rfcomm_session_t *, uint8_t, rfcomm_dlc_newconn_cb_t,
    struct rfcomm_mcc_pn *);
void rfcomm_dlc_close(struct rfcomm_dlc *, result_t);
void rfcomm_dlc_timeout(void *);
result_t rfcomm_dlc_setmode(struct rfcomm_dlc *);
result_t rfcomm_dlc_connect(struct rfcomm_dlc *);
result_t rfcomm_dlc_open(struct rfcomm_dlc *);
void rfcomm_dlc_start(struct rfcomm_dlc *);
result_t rfcomm_dlc_send_mcc_msc(struct rfcomm_dlc *dlc, uint8_t modem_set,
    uint8_t modem_clear, int8_t brk);
result_t rfcomm_dlc_send_mcc_rpn(struct rfcomm_dlc *dlc, uint16_t mask,
    uint8_t rate, uint8_t line, uint8_t fc, uint8_t xon, uint8_t xoff);

/* rfcomm_session.c */
rfcomm_session_t *rfcomm_session_alloc(struct rfcomm_session_list *,
    bt_sockaddr_t *);
rfcomm_session_t *rfcomm_session_lookup(bt_sockaddr_t *, bt_sockaddr_t *);
void rfcomm_session_free(rfcomm_session_t *);
result_t rfcomm_session_send_frame(rfcomm_session_t *, int_t, uint8_t);
result_t rfcomm_session_send_uih(rfcomm_session_t *, struct rfcomm_dlc *,
    int_t, struct mbuf *);
result_t rfcomm_session_send_mcc(rfcomm_session_t *, int_t, uint8_t,
    void *, int32_t);
void rfcomm_session_timeout(void *);

#ifdef J_DEBUG
void rfcomm_dlc_dump(const char *file, int_t line, struct rfcomm_dlc *dlc);
void rfcomm_session_dump(const char *file, int_t line,
    struct rfcomm_session *rs, uint8_t all);
void rfcomm_active_list_dump(const char *file, int_t line, uint8_t all);

#define RFCOMM_DLC_DUMP(dlc) \
    rfcomm_dlc_dump(__FILE__, __LINE__, (dlc))

#define RFCOMM_SESSION_DUMP(rs, all) \
    rfcomm_session_dump(__FILE__, __LINE__, (rs), (all))

#define RFCOMM_ACTIVE_LIST_DUMP(all) \
    rfcomm_active_list_dump(__FILE__, __LINE__, (all))

#endif

#endif /* _BT_RFCOMM_INTERNAL_H_ */

