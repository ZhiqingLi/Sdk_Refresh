/* Modifications by Jungo Ltd. Copyright (c) 2012 Jungo Ltd. All Rights reserved */

/*
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
/*
 * server.h
 *
 * Copyright (c) 2004 Maksim Yevmenkin <m_evmenkin@yahoo.com>
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

#ifndef _SDP_SERVER_H_
#define _SDP_SERVER_H_

#include <bt_l2cap_proto.h>

/* TODO: change order of sdp_conn and sdp_server structs declaration */
struct sdp_server;

/*
 * File descriptor index entry
 */

typedef struct sdp_conn {
    uint16_t                imtu;
    uint16_t                omtu;
    btaddr_t                laddr;
    uint16_t                rsp_cs;     /* response continuation state */
    uint16_t                rsp_size;   /* response size */
    uint16_t                rsp_limit;  /* response limit */
    uint8_t                 *rsp;       /* outgoing buffer */
    l2cap_channel_h         chan;
    struct sdp_server       *srv;
    LIST_ENTRY(sdp_conn)    next;
} sdp_conn_t;

/*
 * SDP server
 */

typedef struct sdp_server {
    uint8_t                 *req;       /* incoming buffer */
    uint8_t                 flag;
    uint16_t                transaction_id;     /* current transaction id. */
    l2cap_channel_h         chan_l;
    l2cap_channel_h         chan_r;
    uint8_t                 chan_r_used;

    LIST_HEAD(, sdp_conn)   sdp_conns;  /* active connections list */
} sdp_server_t;

/*
 * External API
 */

result_t sdp_server_init(void);
void sdp_server_uninit(void);

result_t sdp_server_do(void);

int_t server_get_service_search_pattern(uint8_t const **buf,
    uint8_t const *end, uuid_t *uuid);

int_t server_prepare_service_search_response(sdp_conn_t *sdp_conn);
result_t server_send_service_search_response(sdp_conn_t *sdp_conn);

int_t server_prepare_service_attribute_response(sdp_conn_t *sdp_conn);
result_t server_send_service_attribute_response(sdp_conn_t *sdp_conn);

int_t server_prepare_service_search_attribute_response(
    sdp_conn_t *sdp_conn);
#define server_send_service_search_attribute_response \
    server_send_service_attribute_response

result_t sdp_connect(btaddr_t *laddr, btaddr_t *raddr);

int analyze_sdp_serviceSearchResponse(uint8_t *buff);
int analyze_sdp_serviceAttributeResponse(uint8_t *buff);
inline uint16_t change2bigEndian16(uint16_t in_16);

#endif
