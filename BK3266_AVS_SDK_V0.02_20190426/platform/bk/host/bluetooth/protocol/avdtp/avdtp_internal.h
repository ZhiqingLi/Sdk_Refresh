/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _AVDTP_INTERNAL_H_
#define _AVDTP_INTERNAL_H_

#include <bt_avdtp_proto.h>
#include <jos/jos_mbuf.h>
#include <bt_l2cap_proto.h>

typedef LIST_HEAD(, avdtp_session)      avdtp_session_list_t;
typedef LIST_HEAD(, avdtp_stream)       avdtp_stream_list_t;

typedef struct {
    struct mbuf        *m;          /* Response data or NULL */
    uint8_t             acp_sep_id; /* Acceptor for this response */
    avdtp_err_t         avdtp_err;  /* Protocol error, send a reject */
} recv_req_rsp_t;

extern avdtp_session_list_t avdtp_sessions;

typedef struct avdtp_req {
    TAILQ_ENTRY(avdtp_req)      next;
    uint8_t                     tid;

    /* Acceptor endpoint, not set for DISCOVERY */
    uint8_t                     acp_sep_id;

    /* Stream, not set for DISCOVERY and GET_CAPABILITIES */
    struct avdtp_stream         *st;
} avdtp_req_t;

typedef TAILQ_HEAD(, avdtp_req) avdtp_req_list_t;

typedef struct avdtp_session {
    avdtp_stream_list_t         ss_listen_streams;
    avdtp_stream_list_t         ss_active_streams;
    LIST_ENTRY(avdtp_session)   ss_next;

    const struct btproto        *ss_proto;
    void                        *ss_upper;

    l2cap_channel_h             ss_l2cap;
    uint8_t                     ss_state;
    uint8_t                     ss_flags;
    uint16_t                    ss_mtu;         /* default mtu */
    uint8_t                     ss_id;          /* session id */

    /* Outgoing signals waiting for responses */
    avdtp_req_list_t            ss_reqs;

    TAILQ_HEAD(,avdtp_stream_con_req) ss_con_reqs; /* connection requests */
} avdtp_session_t;

/* Session state */
#define AVDTP_SESSION_CLOSED            0
#define AVDTP_SESSION_WAIT_CONNECT      1
#define AVDTP_SESSION_CONNECTED         2
#define AVDTP_SESSION_WAIT_DISCONNECT   3
#define AVDTP_SESSION_LISTEN            4

/* Session flags */
#define AVDTP_SESSION_INITIATOR         (1 << 0)
#define AVDTP_SESSION_FREE              (1 << 1)

typedef struct avdtp_stream {
    LIST_ENTRY(avdtp_stream)    st_next;

    const struct btproto        *st_proto;
    void                        *st_upper;
    uint8_t                     st_state;
    uint8_t                     st_state_preclose;

    sockaddr_avdtp_stream_t     st_laddr;    /* Local SEP id/type/media type */
    sockaddr_avdtp_stream_t     st_raddr;    /* Remote SEP id/type/media type */

    /* Service capabilities, the meaning depends on the state of the stream.
     * 1. For listening stream, this is the list of locally supported service
     *    capabilities. The list is sent to the remote as a respone to an
     *    incoming GET_CAPABILITIES signal.
     *
     * 2. For active stream, this is the list of configured service
     *    capabilities. Which is set either by the remote using an incoming
     *    SET_CONFIGURATION signal. Or is set by the upper using
     *    avdtp_stream_setopt() before sending an outgoing SET_CONFIGURATION.
     */
    avdtp_cap_list_t            caps;

    avdtp_session_t             *st_session;
    l2cap_channel_h             st_l2cap;
    uint8_t                     st_flags;
} avdtp_stream_t;

typedef struct avdtp_stream_con_req {
    uint8_t                             acp_sep_id;
    uint8_t                             tid;
    uint8_t                             sig_id;
    avdtp_cap_list_t                    caps;
    TAILQ_ENTRY(avdtp_stream_con_req)   next;
} avdtp_stream_con_req_t;

/* Stream states */
#define AVDTP_STREAM_CLOSED          0 /* Disconnected, also the initial state */
#define AVDTP_STREAM_LISTEN          1 /* Local SEP is discoverable */
#define AVDTP_STREAM_WAIT_CONFIGURE  2
#define AVDTP_STREAM_CONFIGURED      3
#define AVDTP_STREAM_WAIT_OPEN       4
#define AVDTP_STREAM_OPEN            5
#define AVDTP_STREAM_WAIT_CONNECT    6
#define AVDTP_STREAM_CONNECTED       7 /* Ready to stream */
#define AVDTP_STREAM_WAIT_CLOSE      8
#define AVDTP_STREAM_WAIT_DISCONNECT 9

/*
 * State machine of incoming stream establishment:
 *
 * state        |  next state   |  event
 * -------------+---------------+-------
 * CLOSED       |  CONFIGURED   |  got SET_CONFIGURATION signal
 * CONFIGURED   |  OPEN         |  got OPEN signal
 * OPEN         |  WAIT_CONNECT |  got L2CAP con req
 * WAIT_CONNECT |  CONNECTED    |  L2CAP channel connected
 */

/*
 * State machine of outgoing stream establishment:
 *
 * state          |  next state     |  event
 * ---------------+-----------------+-------
 * CLOSED         |  WAIT_CONFIGURE |  sent SET_CONFIGURATION signal
 * WAIT_CONFIGURE |  CONFIGURED     |  got SET_CONFIGURATION response
 * CONFIGURED     |  WAIT_OPEN      |  sent OPEN signal
 * WAIT_OPEN      |  OPEN           |  got OPEN response
 * OPEN           |  WAIT_CONNECT   |  sent L2CAP con req (l2cap_connect)
 * WAIT_CONNECT   |  CONNECTED      |  L2CAP channel connected
 */

/*
 * State machine of incoming stream release:
 *
 * state          |  next state      |  event
 * ---------------+------------------+-------
 * OPEN           |  CLOSED          |  got CLOSE signal
 */

/*
 * State machine of outgoing stream release (L2CAP connected):
 *
 * state           |  next state      |  event
 * ----------------+------------------+-------
 * CONNECTED       |  WAIT_CLOSE      |  sent CLOSE signal
 * WAIT_CLOSE      |  WAIT_DISCONNECT |  got CLOSE accept, sent L2CAP disc req
 * WAIT_DISCONNECT |  CLOSE           |  L2CAP channel disconnected
 *
 *
 * State machine of outgoing stream release (L2CAP not connected):
 *
 * state              |  next state   |  event
 * -------------------+---------------+-------
 * CONFIGURED or OPEN |  WAIT_CLOSE   |  sent CLOSE signal
 * WAIT_CLOSE         |  CLOSE        |  got CLOSE accept
 */

/* Stream flags */
#define AVDTP_STREAM_SHUTDOWN           (1 << 0)
#define AVDTP_STREAM_FREE               (1 << 1)
#define AVDTP_STREAM_DISCOVERABLE       (1 << 2) /* Local SEP is discoverable */
#define AVDTP_STREAM_STARTED            (1 << 3) /* START_CMD was called */

/* AVDTP session */
result_t avdtp_init(void);
void avdtp_uninit(void);
result_t avdtp_session_init(void);
void avdtp_session_uninit(void);
avdtp_session_t *avdtp_session_alloc(const struct btproto *proto, void *upper);
void avdtp_session_free(avdtp_session_t *ss);
void avdtp_session_close(avdtp_session_t *ss, result_t err);

avdtp_stream_t *avdtp_stream_alloc(const struct btproto *proto, void *upper);
void avdtp_stream_free(avdtp_stream_t *st);
void avdtp_stream_close(avdtp_stream_t *st, result_t err);

avdtp_cap_t *avdtp_cap_find(avdtp_cap_list_t *head, uint8_t category);
result_t avdtp_cap_add_single(avdtp_cap_list_t *head, avdtp_cap_generic_t *hdr);
avdtp_err_t avdtp_caps_from_m(avdtp_cap_list_t *head, struct mbuf *m, bool_t *cp_flag);
result_t avdtp_caps_to_m(struct mbuf **m, avdtp_cap_list_t *head);
void avdtp_caps_free(avdtp_cap_list_t *head);
const avdtp_cap_t *avdtp_cap_audio_codec_first(const avdtp_cap_list_t *caps,
    int16_t codec_type);
const avdtp_cap_t *avdtp_cap_audio_codec_next(const avdtp_cap_t *cap,
    int16_t codec_type);

avdtp_stream_t *avdtp_stream_lookup(avdtp_stream_list_t *list,
    uint8_t local_sep_id);

#ifdef J_DEBUG
void avdtp_cap_dump_all(const avdtp_cap_list_t *caps,
    const char *file, int_t line);
void avdtp_cap_dump(const avdtp_cap_t *cap, const char *file, int_t line);
#endif

/* AVDTP signaling messages */
result_t avdtp_sig_init(void);
void avdtp_sig_uninit(void);
void avdtp_recv_signal(avdtp_session_t *ss, struct mbuf *m);

result_t avdtp_send_discover_req(avdtp_session_t *ss);
result_t avdtp_send_get_capabilities_req(avdtp_session_t *ss, uint8_t sep_id);

#if UPGRADED_VERSION
result_t avdtp_send_get_all_capabilities_req(avdtp_session_t *ss, uint8_t sep_id);
result_t avdtp_send_delay_report_req(avdtp_session_t *ss, uint8_t sep_id);
#endif

result_t avdtp_send_set_configuration_req(avdtp_stream_t *st);
result_t avdtp_send_get_configuration_req(avdtp_stream_t *st);
result_t avdtp_send_reconfigure_req(avdtp_stream_t *st);
result_t avdtp_send_open_req(avdtp_stream_t *st);
result_t avdtp_send_start_req(avdtp_stream_t *st);
result_t avdtp_send_close_req(avdtp_stream_t *st);
result_t avdtp_send_suspend_req(avdtp_stream_t *st);
result_t avdtp_send_abort_req(avdtp_stream_t *st);
result_t avdtp_send_security_control_req(avdtp_stream_t *st);
result_t avdtp_send_rsp(avdtp_session_t *ss, uint8_t tid, uint8_t sig_id,
    recv_req_rsp_t *rsp);
#endif
