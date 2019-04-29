/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _HCI_INTERNAL_H_
#define _HCI_INTERNAL_H_

#include <bluetooth.h>
#include <bt_api.h>
#include <bt_hci_types.h>
#include <jos/jos_mbuf.h>

struct bt_unit_s;
struct l2cap_channel;
struct mbuf;
struct sco_pcb;
struct sco_params;

/**************************************************************************
 **                 HCI Internal Definitions
 **************************************************************************/

/* TODO: Make this an enum? */
/* hci_link state */
#define HCI_LINK_CLOSED         0  /* closed */
#define HCI_LINK_WAIT_CONNECT   1  /* waiting to connect */
#define HCI_LINK_WAIT_FEATURES  2  /* waiting for remote features */
#define HCI_LINK_WAIT_AUTH      3  /* waiting for auth */
#define HCI_LINK_WAIT_ENCRYPT   4  /* waiting for encrypt */
#define HCI_LINK_WAIT_SECURE    5  /* waiting for secure */
#define HCI_LINK_OPEN           6  /* ready and willing */
#define HCI_LINK_BLOCK          7  /* open but blocking (see hci_acl_start) */
#define HCI_LINK_FREE           8  /* link is being freed */
/*beken defined*/
#define HCI_LINK_WAIT_CLOCK     9

/* hci_link flags */
#define HCI_LINK_AUTH_REQ       (1<<0)  /* authentication requested */
#define HCI_LINK_ENCRYPT_REQ    (1<<1)  /* encryption requested */
#define HCI_LINK_SECURE_REQ     (1<<2)  /* secure link requested */
#define HCI_LINK_AUTH           (1<<3)  /* link is authenticated */
#define HCI_LINK_ENCRYPT        (1<<4)  /* link is encrypted */
#define HCI_LINK_SECURE         (1<<5)  /* link is secured */
#define HCI_LINK_CREATE_CON     (1<<6)  /* "Create Connection" pending */

/* TODO: not sure this is the correct place for this declaration */

/* TODO: BTF_XMIT flags are unused, may possibly be replaced? */
/* bt_unit & btr_flags */
#define BTF_RUNNING             (1<<0)  /* unit is running */
#define BTF_XMIT_CMD            (1<<1)  /* unit is transmitting CMD packets */
#define BTF_XMIT_ACL            (1<<2)  /* unit is transmitting ACL packets */
#define BTF_XMIT_SCO            (1<<3)  /* unit is transmitting SCO packets */
#define BTF_XMIT                (BTF_XMIT_CMD | BTF_XMIT_ACL | BTF_XMIT_SCO)
#define BTF_INIT_BTADDR         (1<<4)  /* waiting for btaddr */
#define BTF_INIT_BUFFER_SIZE    (1<<5)  /* waiting for buffer size */
#define BTF_INIT_FEATURES       (1<<6)  /* waiting for features */
#define BTF_POWER_UP_NOOP       (1<<7)  /* should wait for No-op on power up */
#define BTF_INIT_COMMANDS       (1<<8)  /* waiting for supported commands */
#define BTF_INIT_DELETE_KEYS    (1<<9) /* waiting for stored keys deletion */
#define BTF_INIT_EVENTMASK      (1<<10) /* waiting for set event mask */

#define BTF_INIT                (BTF_INIT_BTADDR        \
                                | BTF_INIT_BUFFER_SIZE  \
                                | BTF_INIT_FEATURES     \
                                | BTF_INIT_COMMANDS     \
                                | BTF_INIT_DELETE_KEYS  \
                                | BTF_INIT_EVENTMASK)

/**************************************************************************
 **                         Events and event parameters
 **************************************************************************/

struct bt_stats {
    uint32_t    err_tx;
    uint32_t    err_rx;
    uint32_t    cmd_tx;
    uint32_t    evt_rx;
    uint32_t    acl_tx;
    uint32_t    acl_rx;
    uint32_t    sco_tx;
    uint32_t    sco_rx;
    uint32_t    byte_tx;
    uint32_t    byte_rx;
};

typedef struct sco_xfer {
    /* PCM frames */
    void                       *buf;
    int32_t                     buflen;

    /* PCM frames that were not sent yet */
    uint8_t                     *data;
    int32_t                     datalen;

    struct hci_link             *link;

    void                        *cb;      /* callback */
    void                        *cb_ctx;  /* completion callback */
    result_t                    err;      /* completion error */
    STAILQ_ENTRY(sco_xfer)      next;
} sco_xfer_t;

typedef STAILQ_HEAD(,sco_xfer) sco_xfer_queue_t;

typedef void (*sco_enable_disable_cb_t)(void *, result_t);

/*
 * The Bluetooth HCI interface attachment structure
 */
struct hci_if {
    result_t    (*enable)(j_device_h self);
    void       (*disable)(j_device_h self);
    void       (*output_cmd)(j_device_h self, struct mbuf *m);
    void       (*output_acl)(j_device_h self, struct mbuf *m);
    result_t    (*output_sco)(j_device_h self, struct sco_xfer *xfer);
    result_t    (*sco_enable)(j_device_h self, sco_enable_disable_cb_t cb,
       			void *cb_ctx);
    void       (*sco_disable)(j_device_h self, sco_enable_disable_cb_t cb,
      			 void *cb_ctx);
    void       (*sco_stop)(j_device_h self);
    void       (*get_stats)(j_device_h self, struct bt_stats *dest,
        int_t flush);
};

/*
 * Bluetooth Memo
 *      cached device information for remote devices that this unit has seen
 */
struct hci_memo {
    jtime_t             time;   /* time of last response */
    btaddr_t            btaddr;
    uint8_t             page_scan_rep_mode;
    uint8_t             page_scan_mode;
    uint16_t            clock_offset;
    uint8_t             inq_count; /* count of appearances in inquiry result */
#ifdef CONFIG_BLUETOOTH_SSP
    uint8_t             ssp_mode;
#endif
    LIST_ENTRY(hci_memo)        next;
};

/*
 * HCI Connection Information
 */
typedef struct hci_link {
    struct bt_unit_s            *hl_unit;      /* our unit */
    TAILQ_ENTRY(hci_link)       hl_next;       /* next link on unit */

    /* common info */
    uint16_t                    hl_state;      /* connection state */
    uint16_t                    hl_flags;      /* link flags */
    btaddr_t                    hl_btaddr;     /* dest address */
    uint16_t                    hl_handle;     /* connection handle */
    uint8_t                     hl_type;       /* link type */
    uint16_t                    hl_sco_mask;   /* SCO packet capabilities */
    
#ifdef CONFIG_BLUETOOTH_SSP
    uint8_t                     hl_auth_fail;   /* consecutive auth failures */
    uint8_t                     hl_auth_req;    /* authentication reqs */
    uint8_t                     hl_rem_io_caps; /* remote IO caps */
    uint8_t                     hl_rem_auth_req;/* remote auth reqs */
    uint8_t                     hl_ssp_mode;  /* SSP mode */
    uint8_t                     hl_ssp_known; /* Indicates remote SSP mode is known */
#endif

    /* ACL link info */
    uint8_t                     hl_lastid;     /* last id used */
    __VOLATILE__ int16_t        hl_refcnt;     /* reference count */
    uint16_t                    hl_mtu;        /* signalling mtu for link */
    uint16_t                    hl_flush;      /* flush timeout */
    uint16_t                    hl_clock;      /* remote clock offset */

    TAILQ_HEAD(,l2cap_pdu)       hl_txq;        /* queue of outgoing PDUs */
    int_t                        hl_txqlen;     /* number of fragments */
    jtask_h                      hl_expire;     /* connection expiry timer */
    TAILQ_HEAD(,l2cap_req)       hl_reqs;       /* pending outgoing requests */

    TAILQ_HEAD(,l2cap_con_req)   hl_con_reqs;    /* pending incoming requests */
    struct mbuf                 *hl_rxp;        /* incoming PDU (accumulating)*/

    /* SCO link info */
    struct hci_link             *hl_link;       /* SCO ACL link */
    struct sco_pcb              *hl_sco;        /* SCO pcb */

    result_t                    hl_err;         /* hci_link_free() error */

#ifdef BT_ONE_TO_MULTIPLE
    uint32_t                    support_role_switch;
#endif
#if (CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
    uint32_t                    flow_ctrl;
#endif
} hci_link_t;

/*
 * The Bluetooth HCI device unit structure
 */

typedef struct bt_unit_s {
    /* device interface */
    j_device_h          hci_dev;                /* device ctx (lower) */
    const struct hci_if *hci_cbs;               /* device interface */

    /* unit info */
    bt_unit_info_t      info;
    bt_app_ctx_h        app_ctx;                /* app context */
    bt_unit_app_cbs_t   app_cbs;                /* app interface */
    uint8_t             local_name[HCI_UNIT_NAME_SIZE + 1];
    uint8_t             tx_power_level;

    /* device info */
    btaddr_t            hci_btaddr;             /* device address */
    uint16_t            hci_flags;              /* see BTF_ above */
    hci_state_t         hci_state;              /* see BT_STATE_ above */

    uint16_t            hci_packet_type;        /* packet types */
    uint16_t            hci_acl_mask;           /* ACL packet capabilities */
    uint16_t            hci_sco_mask;           /* SCO packet capabilities */

    uint16_t            hci_link_policy;        /* link policy */
    uint16_t            hci_lmp_mask;           /* link policy capabilities */

#ifdef CONFIG_BLUETOOTH_SSP
    uint8_t             hci_ssp_supported;    /* HCI SSP support */
    uint8_t             hci_ssp_mode;         /* Current SSP mode */
#endif

    uint8_t             hci_version;                     /* HCI version */
    bt_lmp_features_t   hci_lmp_feats;                   /* LMP features */
    uint8_t             hci_cmds[HCI_COMMANDS_SIZE];     /* opcode bitmask */
    uint8_t             hci_evmask[HCI_EVENT_MASK_SIZE]; /* event mask (LE) */

    /* flow control */
    uint16_t            hci_max_acl_size;       /* ACL payload mtu */
    uint16_t            hci_num_acl_pkts;       /* free ACL packet buffers */
    uint8_t             hci_num_cmd_pkts;       /* free CMD packet buffers */
    uint8_t             hci_max_sco_size;       /* SCO payload mtu */
    uint16_t            hci_num_sco_pkts;       /* free SCO packet buffers */

    TAILQ_HEAD(,hci_link) hci_links;            /* list of ACL/SCO links */
    LIST_HEAD(,hci_memo)  hci_memos;            /* cached memo list */

    /* output queues */
    MBUFQ_HEAD()          hci_cmdwait;          /* pending commands */

    jtask_h               hci_disable_task;     /* deffered disable task */
    jtask_h               hci_unit_to_task;

    LIST_ENTRY(bt_unit_s) hci_next;
} hci_unit_t;

extern LIST_HEAD(hci_unit_list, bt_unit_s) hci_unit_list;

#define DEVNAME(dev) j_device_get_nameunit(dev)

/*
 * HCI layer function prototypes
 */

/* hci_event.c */
void hci_event(struct mbuf *, hci_unit_t *);

/* hci_link.c */
result_t hci_accept_con_req(hci_unit_t *, const btaddr_t *);
void hci_reject_con_req(hci_unit_t *, uint16_t, const btaddr_t *, hci_error_t);
result_t hci_acl_echo(hci_link_t *link);
result_t hci_remote_name_req(hci_unit_t *unit, const btaddr_t *btaddr);
hci_link_t *hci_acl_open(hci_unit_t *, const btaddr_t *);
result_t hci_acl_newconn(hci_unit_t *, hci_con_req_ep *);
void hci_acl_close(hci_link_t *, result_t);
result_t hci_acl_setmode(hci_link_t *);
void hci_acl_linkmode(hci_link_t *);
void hci_acl_recv(struct mbuf *, hci_unit_t *);
result_t hci_acl_send(struct mbuf *, hci_link_t *, struct l2cap_channel *);
void hci_acl_start(hci_link_t *);
void hci_acl_complete(hci_link_t *, uint16_t);
result_t hci_sco_newconn(hci_unit_t *, hci_con_req_ep *);
void hci_sco_recv(struct mbuf *, hci_unit_t *);
void hci_sco_complete(hci_link_t *, struct sco_xfer *);
hci_link_t *hci_link_alloc(hci_unit_t *, const btaddr_t *, uint8_t);
void hci_link_free(hci_link_t *, result_t);
void hci_link_free_sync(hci_link_t *, result_t);
hci_link_t *hci_link_lookup_btaddr(hci_unit_t *, const btaddr_t *, uint8_t);
hci_link_t *hci_link_lookup_handle(hci_unit_t *, uint16_t);
hci_link_t *hci_sco_link_lookup_btaddr(hci_unit_t *unit,const btaddr_t *btaddr, uint8_t type);

result_t hci_sco_accept(hci_unit_t *unit, const btaddr_t *raddr,
    uint8_t link_type, const struct sco_params *params,
    struct sco_pcb *pcb_new);
result_t hci_sco_reject(hci_unit_t *unit, const btaddr_t *raddr,
    hci_error_t hci_err);

/* hci_misc.c */
result_t hci_route_lookup(btaddr_t *, const btaddr_t *);
struct hci_memo *hci_memo_find(hci_unit_t *, const btaddr_t *);
struct hci_memo *hci_memo_new(hci_unit_t *, const btaddr_t *);
void hci_memo_free(struct hci_memo *);

/* hci_unit.c */
hci_unit_t *hci_attach(const struct hci_if *, j_device_h, uint16_t);
void hci_disable_task(void *arg);
void hci_enable_disable_handler(hci_unit_t *unit, hci_error_t hci_err,
    result_t err);
void hci_detach(hci_unit_t *);
hci_unit_t *hci_unit_lookup(const btaddr_t *);
result_t hci_send_cmd(hci_unit_t *, uint16_t, void *, uint8_t);
void hci_num_cmds(hci_unit_t *, uint8_t);
void hci_input_event(hci_unit_t *, struct mbuf *);
void hci_input_acl(hci_unit_t *, struct mbuf *);
void hci_input_sco(hci_unit_t *, struct mbuf *);
void hci_complete_sco(hci_unit_t *, struct sco_xfer *xfer);
void hci_output_acl(hci_unit_t *, struct mbuf *);
uint16_t hci_create_pcm_settings(const hci_pcm_settings_t *pcm_settings);

/* hci_security */
void hci_security_process(uint8_t event, hci_unit_t *unit, struct mbuf *m);
void hci_security_pin_code_reply(const btaddr_t *raddr, const btpin_t *pin);
void hci_security_pin_code_reject(const btaddr_t *raddr);
#ifdef CONFIG_BLUETOOTH_SSP
void hci_security_set_io_caps(uint8_t io_caps);
uint8_t hci_security_get_io_caps(void);
uint8_t hci_security_get_bonding_mode(const btaddr_t *raddr);
result_t hci_security_set_bonding_mode(const btaddr_t *raddr, uint8_t mode);
result_t hci_security_remote_oob_reply(const btaddr_t *raddr,
    const uint8_t *hash, const uint8_t *randomizer);
result_t hci_security_read_local_oob(const btaddr_t *raddr);
result_t hci_security_user_confirmation_reply(const btaddr_t *raddr,
    bool_t positive);
result_t hci_security_user_passkey_reply(const btaddr_t *raddr, uint32_t value);
result_t hci_security_keypress_notification(const btaddr_t *raddr,
    uint8_t type);
#endif
result_t hci_security_init(void);
void hci_security_uninit(void);
void hci_security_disable(void);

/* Utility functions */
static __INLINE__ hci_unit_t *hci_unit_from_raddr(const btaddr_t *raddr)
{
    btaddr_t laddr;
    result_t err;

    err = hci_route_lookup(&laddr, raddr);
    if(err)
        return NULL;

    return hci_unit_lookup(&laddr);
}

/* hci_unit.c callbacks */
result_t unit_enable(hci_unit_t * hci_unit);
result_t unit_disable(hci_unit_t * hci_unit);
result_t unit_acl_connect(hci_unit_t * hci_unit, const btaddr_t *raddr);
void unit_acl_disconnect(hci_unit_t * hci_unit, const btaddr_t *raddr);
result_t unit_acl_accept(hci_unit_t * hci_unit, const btaddr_t *raddr);
result_t unit_acl_reject(hci_unit_t * hci_unit, const btaddr_t *raddr);
#ifdef CONFIG_TWS
result_t unit_acl_reject_stereo(hci_unit_t *unit, const btaddr_t *raddr, uint8_t reason);
#endif
result_t unit_acl_echo(hci_unit_t * hci_unit, const btaddr_t *raddr);
result_t unit_send_cmd(hci_unit_t *hci_unit, uint16_t opcode, void *buf,
    uint8_t len);
result_t unit_send_cmd_m(hci_unit_t * hci_unit, uint16_t opcode,
    struct mbuf *m);
void unit_set_link_policy(hci_unit_t * hci_unit, uint16_t link_policy_req,
    uint16_t *link_policy_actual);
void unit_set_packet_type(hci_unit_t * hci_unit, uint16_t packet_type_req,
    uint16_t *packet_type_actual);
bool_t unit_feature_supported(hci_unit_t * hci_unit, uint8_t octet,
    uint8_t bit);
result_t unit_command_supported(hci_unit_t * hci_unit, uint8_t octet,
    uint8_t bit);

void unit_update_xmit_power(hci_unit_t * unit, struct mbuf *m);
void unit_update_local_name(hci_unit_t * unit, struct mbuf *m);

#endif
