/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _SCO_INTERNAL_H_
#define _SCO_INTERNAL_H_

#include <bt_sco.h>

typedef enum {
    SP_IDLE,
    SP_BUSY,
    SP_DONE
} sp_state_t;

/*
 * SCO protocol control block
 */
struct sco_pcb {
    hci_link_t          *sp_link;      /* SCO link */
    sp_state_t          sp_state;      /* state */
    btaddr_t            sp_laddr;      /* local address */
    btaddr_t            sp_raddr;      /* remote address */
    uint16_t            sp_mtu;        /* link MTU */
    int_t               sp_pending;    /* number of packets pending */

    scoproto_t          sp_proto;      /* upper layer protocol */
    void                *sp_upper;     /* upper layer argument */

    LIST_ENTRY(sco_pcb) sp_next;
};

void sco_connected(struct sco_pcb *pcb);
void sco_disconnected(struct sco_pcb *pcb, result_t err);
void sco_newconn(hci_unit_t *unit, hci_con_req_ep *ep, uint16_t pkt_type);
void sco_complete(struct sco_pcb *pcb, result_t err, sco_xfer_t *xfer);
void sco_input(struct sco_pcb *pcb, struct mbuf *m);

struct sco_pcb *sco_pcb_lookup(const btaddr_t *laddr, const btaddr_t *raddr);

#endif
