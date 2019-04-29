/* Modifications by Jungo Ltd. Copyright (c) 2012 Jungo Ltd. All Rights reserved */

/*-
 * Copyright (c) 2006 Itronix Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of Itronix Inc. may not be used to endorse
 *    or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ITRONIX INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL ITRONIX INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _BT_SCO_H_
#define _BT_SCO_H_

#define SO_SCO_MTU              1

#include <bt_hci_types.h>

/*
 * SCO callback methods
 */
struct mbuf;

typedef enum {
    SCO_LINK_TYPE_SCO  = HCI_LINK_SCO,
    SCO_LINK_TYPE_ESCO = HCI_LINK_eSCO
} sco_link_type_t;

typedef void (*scoproto_newconn_t)(void *upper, sco_link_type_t link_type,
    uint16_t pkt_type);

typedef struct {
    btproto_connected_t     connected;
    btproto_disconnected_t  disconnected;
    scoproto_newconn_t      newconn;
    btproto_complete_t      complete;
    btproto_input_t         input;
} scoproto_t;

#define SCO_RETRANSMIT_EFFORT_POWER_CONSUMPTION 0x01
#define SCO_RETRANSMIT_EFFORT_LINK_QUALITY      0x02
#define SCO_RETRANSMIT_ERFFOT_DONT_CARE         0xFF

typedef struct sco_params {
    uint32_t            tx_bandwidth;  /* Transmit Bandwidth (bytes / second) */
    uint32_t            rx_bandwidth;  /* Receive Bandwidth (bytes / second) */
    uint16_t            max_latency;   /* Upper limit of the synchronous
                                        * interval and eSCO window */
    hci_pcm_settings_t  pcm_settings;

    uint8_t             retransmit_effort;
    uint16_t            pkt_type;
} sco_params_t;

typedef struct sco_pcb *sco_h;

result_t sco_open(const btaddr_t *laddr, const btaddr_t *raddr,
    const scoproto_t *proto, void *app_ctx, sco_h *ep);
void sco_close(sco_h ep);

result_t sco_getopt(sco_h ep, uint8_t type, void *sopt, uint32_t size);
result_t sco_send(sco_h ep, uint8_t *buff, uint32_t length);
void     sco_stop(sco_h ep);

result_t sco_connect(sco_h ep, sco_link_type_t type,
    const sco_params_t *params);
result_t sco_accept(sco_h ep, sco_link_type_t type, const sco_params_t *params);
result_t sco_reject(sco_h ep);
result_t sco_disconnect(sco_h ep, int_t linger);

#endif
