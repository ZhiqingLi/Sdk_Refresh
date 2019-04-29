/* Modifications by Jungo Ltd. Copyright (c) 2012 Jungo Ltd. All Rights reserved */

/*-
 * Copyright (c) 2005 Iain Hibbert.
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
/*-
 * Copyright (c) Maksim Yevmenkin <m_evmenkin@yahoo.com>
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
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _BT_RFCOMM_PROTO_H_
#define _BT_RFCOMM_PROTO_H_

/*************************************************************************
 **            Contains RFCOMM APIs definitions and structs             **
 *************************************************************************/

#include <jos/jos_mbuf.h>
#include "bluetooth.h"
#include <bt_rfcomm.h>

/* Socket options */
#define SO_RFCOMM_MTU           1   /* mtu */
#define SO_RFCOMM_FC_INFO       2   /* flow control info (below) */
#define SO_RFCOMM_LM            3   /* link mode */
#define SO_RFCOMM_MSC           4   /* modem status command */
#define SO_RFCOMM_FCON_FCOFF    5   /* flow control */
#define SO_RFCOMM_RPN           6   /* remote port negotiation */
#define SO_RFCOMM_TEST          7   /* test frame */

/* Flow control information */
struct so_rfcomm_fc_info {
    uint8_t             lmodem;         /* modem signals (local) */
    uint8_t             rmodem;         /* modem signals (remote) */
    int_t               tx_cred;        /* TX credits */
    int_t               rx_cred;        /* RX credits */
    uint8_t             cfc;            /* credit flow control */
};

/* Modem control signals */
struct so_rfcomm_msc {
    uint8_t             clear;  /* bits to clear */
    uint8_t             set;    /* bits to set */
    int8_t              brk;    /* 0-15: length of break in units of 200ms,
                                   -1: ignored */
};

/* Remote port negotiation */
struct so_rfcomm_rpn {
    uint16_t            param_mask;
    uint8_t             bit_rate;
    uint8_t             line_settings;
    uint8_t             flow_control;
    uint8_t             xon_char;
    uint8_t             xoff_char;
};

#define RFCOMM_SPACE_MTU    -1      /* use dlc's MTU as space parameter for
                                     * rfcomm_rcvd() */

typedef struct rfcomm_dlc *rfcomm_dlc_h;

/* rfcomm_upper.c */
result_t rfcomm_attach(rfcomm_dlc_h *, const struct btproto *, void *);
result_t rfcomm_bind(rfcomm_dlc_h, bt_sockaddr_t *);
result_t rfcomm_sockaddr(rfcomm_dlc_h, bt_sockaddr_t *);
result_t rfcomm_connect(rfcomm_dlc_h, bt_sockaddr_t *, bt_link_modes_mask_t);
result_t rfcomm_peeraddr(rfcomm_dlc_h, bt_sockaddr_t *);
result_t rfcomm_disconnect(rfcomm_dlc_h, int_t);
result_t rfcomm_detach(rfcomm_dlc_h *);
result_t rfcomm_listen(rfcomm_dlc_h);
result_t rfcomm_accept(rfcomm_dlc_h, bt_sockaddr_t *, bt_sockaddr_t *,
    bt_link_modes_mask_t);
result_t rfcomm_reject(bt_sockaddr_t *, bt_sockaddr_t *);
result_t rfcomm_send(rfcomm_dlc_h, struct mbuf *);
result_t rfcomm_rcvd(rfcomm_dlc_h, int32_t);
result_t rfcomm_setopt(rfcomm_dlc_h, uint8_t, void *, uint32_t);
result_t rfcomm_getopt(rfcomm_dlc_h, uint8_t, void *, uint32_t);

#endif /* _BT_RFCOMM_PROTO_H_ */

