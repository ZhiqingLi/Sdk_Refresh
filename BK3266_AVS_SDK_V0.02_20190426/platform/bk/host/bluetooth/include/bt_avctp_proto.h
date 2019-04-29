/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_AVCTP_PROTO_H_
#define _BT_AVCTP_PROTO_H_

#include <bt_avctp.h>
#include <bluetooth.h>

typedef struct {
    uint8_t       len;
    btaddr_t      btaddr;
    uint8_t       browse;         /* control/browsing */
    uint16_t      pid;            /* profile uuid-16 */
    uint8_t       reserved_2[7];
} bt_sockaddr_avctp_t;

/*
 * For optimal performance, the size of this struct should be the
 * same as the size of avctp_hdr_single_t
 */
typedef   struct {
   avctp_tid_t         tid;
   avctp_msg_type_t    msg;
   uint8_t             reserved;
} __PACKED_POST__ avctp_upper_hdr_t;

typedef struct avctp_upper *avctp_channel_h;

result_t avctp_attach(avctp_channel_h *, const struct btproto *, void *);
result_t avctp_bind(avctp_channel_h, bt_sockaddr_avctp_t *);
result_t avctp_connect(avctp_channel_h, bt_sockaddr_avctp_t *, bt_link_modes_mask_t);
result_t avctp_disconnect(avctp_channel_h, int_t);
result_t avctp_detach(avctp_channel_h *);
result_t avctp_listen(avctp_channel_h);
result_t avctp_accept(avctp_channel_h, bt_sockaddr_avctp_t *,
    bt_sockaddr_avctp_t *, bt_link_modes_mask_t);
result_t avctp_reject(bt_sockaddr_avctp_t *, bt_sockaddr_avctp_t *);
result_t avctp_send(avctp_channel_h, struct mbuf *);

#endif

