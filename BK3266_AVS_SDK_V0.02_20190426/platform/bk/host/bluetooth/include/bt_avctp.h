/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_AVCTP_H_
#define _BT_AVCTP_H_

#include <uw_types.h>

/* Message command and response headers */
typedef   struct{
    uw_u8_t     b0;          /* Transaction label, packet type, C/R, IPID */
    uw_u16_t    pid;         /* Profile Identifier (UUID-16) */
} __PACKED_POST__ avctp_hdr_single_t;

typedef   struct {
    uw_u8_t     b0;          /* Transaction label, packet type, C/R */
    uw_u8_t     packets_num; /* Number of AVCTP Packets */
    uw_u16_t    pid;         /* Profile Identifier (UUID-16) */
} __PACKED_POST__ avctp_hdr_start_t;

typedef   struct {
    uw_u8_t     b0;          /* Transaction label, packet type, C/R */
} __PACKED_POST__ avctp_hdr_cont_end_t;

/* Packet type */
#define AVCTP_PKT_SINGLE        0x00
#define AVCTP_PKT_START         0x01
#define AVCTP_PKT_CONT          0x02
#define AVCTP_PKT_END           0x03
typedef uint8_t avctp_pkt_type_t;

/* Message type */
#define AVCTP_MSG_COMMAND       0
#define AVCTP_MSG_RESPONSE      1
typedef uint8_t avctp_msg_type_t;

/* AVCTP header - byte 0 */
#define AVCTP_B0_IPID(ipid)     (((ipid) & 0x1) << 0)
#define AVCTP_B0_MSG(msg)       (((msg)  & 0x1) << 1)
#define AVCTP_B0_PKT(pkt)       (((pkt)  & 0x3) << 2)
#define AVCTP_B0_TID(tid)       (((tid)  & 0xF) << 4)

#define AVCTP_B0_IPID_GET(b)    (((b) >> 0) & 0x1)
#define AVCTP_B0_MSG_GET(b)     (((b) >> 1) & 0x1)
#define AVCTP_B0_PKT_GET(b)     (((b) >> 2) & 0x3)
#define AVCTP_B0_TID_GET(b)     (((b) >> 4) & 0xF)

#define AVCTP_TID_MAX           0xF
typedef uint8_t avctp_tid_t;

/* AVCTP Profile id (UUID-16) */
typedef uint16_t avctp_pid_t;

#endif
