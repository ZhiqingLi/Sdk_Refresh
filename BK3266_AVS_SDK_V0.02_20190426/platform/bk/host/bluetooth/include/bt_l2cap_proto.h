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

#ifndef _BT_L2CAP_PROTO_H_
#define _BT_L2CAP_PROTO_H_

/*************************************************************************
 **            Contains L2CAP APIs definitions and structs              **
 *************************************************************************/

#include <jos/jos_mbuf.h>
#include "bluetooth.h"
#include <bt_l2cap.h>

typedef struct l2cap_channel *l2cap_channel_h;

/* Socket options */
#define SO_L2CAP_IMTU       1   /* incoming MTU */
#define SO_L2CAP_OMTU       2   /* outgoing MTU */
#define SO_L2CAP_IQOS       3   /* incoming QoS */
#define SO_L2CAP_OQOS       4   /* outgoing QoS */
#define SO_L2CAP_FLUSH      5   /* flush timeout */
#define SO_L2CAP_LM         6   /* link mode */

/* l2cap_upper.c APIs */
result_t l2cap_attach(l2cap_channel_h *, const struct btproto *, void *);
result_t l2cap_bind(l2cap_channel_h, bt_sockaddr_t *);
result_t l2cap_sockaddr(l2cap_channel_h, bt_sockaddr_t *);
result_t l2cap_connect(l2cap_channel_h, bt_sockaddr_t *, bt_link_modes_mask_t);
result_t l2cap_peeraddr(l2cap_channel_h, bt_sockaddr_t *);
result_t l2cap_disconnect(l2cap_channel_h, uint16_t);
result_t l2cap_detach(l2cap_channel_h *);
result_t l2cap_reset(l2cap_channel_h *);
result_t l2cap_listen(l2cap_channel_h);
result_t l2cap_accept(l2cap_channel_h, bt_sockaddr_t *, bt_sockaddr_t *,
    bt_link_modes_mask_t);
result_t l2cap_reject(bt_sockaddr_t *, bt_sockaddr_t *);
result_t l2cap_send(l2cap_channel_h, struct mbuf *);
result_t l2cap_setopt(l2cap_channel_h, uint8_t, void *, uint32_t);
result_t l2cap_getopt(l2cap_channel_h, uint8_t, void *, uint32_t);
result_t l2cap_set_laddr_psm(struct l2cap_channel *, bt_sockaddr_t *);
#endif  /* _BT_L2CAP_PROTO_H_ */

