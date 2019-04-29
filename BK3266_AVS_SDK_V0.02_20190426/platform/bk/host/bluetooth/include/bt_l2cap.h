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

#ifndef _BT_L2CAP_H_
#define _BT_L2CAP_H_

/**************************************************************************
 **      Contains L2CAP specification definitions and structs            **
 **************************************************************************/

#include "bluetooth.h"
#include <include/uw_types.h>
#include <include/uw_compiler.h>

/*
 * Channel IDs are assigned per machine. So the total number of channels that
 * a machine can have open at the same time is 0xffff - 0x0040 = 0xffbf (65471).
 * This number does not depend on number of HCI connections.
 */

#define L2CAP_NULL_CID          0x0000  /* DO NOT USE THIS CID */
#define L2CAP_SIGNAL_CID        0x0001  /* signaling channel ID */
#define L2CAP_CLT_CID           0x0002  /* connectionless channel ID */
/* 0x0003 - 0x003f Reserved */
#define L2CAP_FIRST_CID         0x0040  /* dynamically alloc. (start) */
#define L2CAP_LAST_CID          0xffff  /* dynamically alloc. (end) */

/* L2CAP MTU */
#define L2CAP_MTU_MINIMUM       48
#define L2CAP_MTU_DEFAULT       672 /* arbitrary value */
#define L2CAP_MTU_MAXIMUM       0x0400

/* L2CAP flush and link timeouts */
#define L2CAP_FLUSH_TIMO_DEFAULT    0xffff /* always retransmit */
#define L2CAP_LINK_TIMO_DEFAULT     0xffff

/* L2CAP Command Reject reasons */
#define L2CAP_REJ_NOT_UNDERSTOOD    0x0000
#define L2CAP_REJ_MTU_EXCEEDED      0x0001
#define L2CAP_REJ_INVALID_CID       0x0002
/* 0x0003 - 0xffff - reserved for future use */

/* Protocol/Service Multiplexor (PSM) values */
#define L2CAP_PSM_ANY               0x0000  /* Any/Invalid PSM */
#define L2CAP_PSM_SDP               0x0001  /* Service Discovery Protocol */
#define L2CAP_PSM_RFCOMM            0x0003  /* RFCOMM protocol */
#define L2CAP_PSM_TCP               0x0005  /* Telephony Control Protocol */
#define L2CAP_PSM_TCS               0x0007  /* TCS cordless */
#define L2CAP_PSM_BNEP              0x000f  /* Bluetooth Network \
                                             * Encapsulation Protocol*/
#define L2CAP_PSM_HID_CNTL          0x0011  /* HID Control */
#define L2CAP_PSM_HID_INTR          0x0013  /* HID Interrupt */
#define L2CAP_PSM_ESDP              0x0015  /* Extended Service \
                                             * Discovery Profile */
#define L2CAP_PSM_AVCTP             0x0017  /* Audio/Video Control \
                                             * Transport Protocol */
#define L2CAP_PSM_AVDTP             0x0019  /* Audio/Video Distribution \
                                             * Transport Protocol */
#define L2CAP_PSM_AVCTP_BROWSING    0x001B  /* Audio/Video Control Transport \
                                             * Protocol Browsing */
/* 0x001C - 0x1000 - reserved for future use */

#define L2CAP_PSM_INVALID(psm)      (((psm) & 0x0101) != 0x0001)

/* L2CAP Connection response command result codes */
#define L2CAP_SUCCESS           0x0000
#define L2CAP_PENDING           0x0001
#define L2CAP_PSM_NOT_SUPPORTED     0x0002
#define L2CAP_SECURITY_BLOCK        0x0003
#define L2CAP_NO_RESOURCES      0x0004
#define L2CAP_TIMEOUT           0xeeee
#define L2CAP_UNKNOWN           0xffff
/* 0x0005 - 0xffff - reserved for future use */

/* L2CAP Connection response status codes */
#define L2CAP_NO_INFO           0x0000
#define L2CAP_AUTH_PENDING      0x0001
#define L2CAP_AUTZ_PENDING      0x0002
/* 0x0003 - 0xffff - reserved for future use */

/* L2CAP Configuration response result codes */
#define L2CAP_UNACCEPTABLE_PARAMS   0x0001
#define L2CAP_REJECT            0x0002
#define L2CAP_UNKNOWN_OPTION        0x0003
/* 0x0003 - 0xffff - reserved for future use */

/* L2CAP Configuration options */
#define L2CAP_OPT_CFLAG_BIT     0x0001
#define L2CAP_OPT_HINT_BIT      0x80
#define L2CAP_OPT_HINT_MASK     0x7f
#define L2CAP_OPT_MTU           0x01
#define L2CAP_OPT_FLUSH_TIMO        0x02
#define L2CAP_OPT_QOS           0x03
#define L2CAP_OPT_RFC           0x04
/* 0x05 - 0xff - reserved for future use */

/* L2CAP Information request type codes */
#define L2CAP_CONNLESS_MTU      0x0001
#define L2CAP_EXTENDED_FEATURES     0x0002
/* 0x0003 - 0xffff - reserved for future use */

/* L2CAP Information response codes */
#define L2CAP_NOT_SUPPORTED     0x0001
/* 0x0002 - 0xffff - reserved for future use */

/* L2CAP MTU option */
typedef uint16_t l2cap_opt_mtu_t;

/* L2CAP Flush Timeout option */
typedef uint16_t l2cap_opt_flush_timo_t;

/* L2CAP Quality of Service option */
typedef   struct {
    uint8_t  flags;         /* reserved for future use */
    uint8_t  service_type;      /* service type */
    uint32_t token_rate;        /* bytes per second */
    uint32_t token_bucket_size; /* bytes */
    uint32_t peak_bandwidth;    /* bytes per second */
    uint32_t latency;       /* microseconds */
    uint32_t delay_variation;   /* microseconds */
} __PACKED_POST__ l2cap_opt_qos_t;

/* L2CAP QoS type */
#define L2CAP_QOS_NO_TRAFFIC    0x00
#define L2CAP_QOS_BEST_EFFORT   0x01       /* (default) */
#define L2CAP_QOS_GUARANTEED    0x02
/* 0x03 - 0xff - reserved for future use */

/* L2CAP Retransmission & Flow Control option */
typedef struct __PACKED_POST__ {
    uint8_t mode;          /* RFC mode */
    uint8_t window_size;       /* bytes */
    uint8_t max_transmit;      /* max retransmissions */
    uint16_t    retransmit_timo;   /* milliseconds */
    uint16_t    monitor_timo;      /* milliseconds */
    uint16_t    max_pdu_size;      /* bytes */
} __PACKED_POST__ l2cap_opt_rfc_t;

/* L2CAP RFC mode */
#define L2CAP_RFC_BASIC     0x00       /* (default) */
#define L2CAP_RFC_RETRANSMIT    0x01
#define L2CAP_RFC_FLOW      0x02
/* 0x03 - 0xff - reserved for future use */

/* L2CAP Extended Features Bit Mask */
#define L2CAP_EXT_FEAT_FLOW_CONTROL     0x0000
#define L2CAP_EXT_FEAT_RETRANSMISSION   0x0001
#define L2CAP_EXT_FEAT_QOS              0x0002
/* 0x0003 - 0xffff - reserved for future use */

typedef uint16_t l2cap_connless_mtu_t;
typedef uint32_t l2cap_ext_features_t;

/**************************************************************************
 **************************************************************************
 **                 Link level defines, headers and types
 **************************************************************************
 **************************************************************************/

/* L2CAP header */
typedef   struct {
    uint16_t    length; /* payload size */
    uint16_t    dcid;   /* destination channel ID */
} __PACKED_POST__ l2cap_hdr_t;

/* L2CAP ConnectionLess Traffic     (dcid == L2CAP_CLT_CID) */
typedef   struct {
    uint16_t    psm; /* Protocol/Service Multiplexor */
} __PACKED_POST__ l2cap_clt_hdr_t;

#define L2CAP_CLT_MTU_MAXIMUM \
    (L2CAP_MTU_MAXIMUM - sizeof(l2cap_clt_hdr_t))

/* L2CAP Command header         (dcid == L2CAP_SIGNAL_CID) */
typedef   struct {
    uint8_t code;   /* command OpCode */
    uint8_t ident;  /* identifier to match request and response */
    uint16_t    length; /* command parameters length */
} __PACKED_POST__ l2cap_cmd_hdr_t;

/* L2CAP Command Reject */
#define L2CAP_COMMAND_REJ           0x01
typedef   struct {
    uint16_t    reason; /* reason to reject command */
    uint16_t    data[2];/* optional data */
} __PACKED_POST__ l2cap_cmd_rej_cp;

/* L2CAP Connection Request */
#define L2CAP_CONNECT_REQ           0x02
typedef   struct {
    uint16_t    psm;  /* Protocol/Service Multiplexor (PSM) */
    uint16_t    scid; /* source channel ID */
} __PACKED_POST__ l2cap_con_req_cp;

/* L2CAP Connection Response */
#define L2CAP_CONNECT_RSP           0x03
typedef   struct {
    uint16_t    dcid;   /* destination channel ID */
    uint16_t    scid;   /* source channel ID */
    uint16_t    result; /* 0x00 - success */
    uint16_t    status; /* more info if result != 0x00 */
} __PACKED_POST__ l2cap_con_rsp_cp;

/* L2CAP Configuration Request */
#define L2CAP_CONFIG_REQ            0x04
typedef   struct {
    uint16_t    dcid;  /* destination channel ID */
    uint16_t    flags; /* flags */
    /*  uint8_t options[] --  options */
} __PACKED_POST__ l2cap_cfg_req_cp;

/* L2CAP Configuration Response */
#define L2CAP_CONFIG_RSP            0x05
typedef   struct {
    uint16_t    scid;   /* source channel ID */
    uint16_t    flags;  /* flags */
    uint16_t    result; /* 0x00 - success */
    /*  uint8_t options[] -- options */
} __PACKED_POST__ l2cap_cfg_rsp_cp;

/* L2CAP configuration option */
typedef   struct {
    uint8_t type;
    uint8_t length;
    /*  uint8_t value[] -- option value (depends on type) */
} __PACKED_POST__ l2cap_cfg_opt_t;

/* L2CAP configuration option value */
typedef union {
    l2cap_opt_mtu_t             mtu;            /* L2CAP_OPT_MTU */
    l2cap_opt_flush_timo_t      flush_timo;     /* L2CAP_OPT_FLUSH_TIMO */
    l2cap_opt_qos_t             qos;            /* L2CAP_OPT_QOS */
    l2cap_opt_rfc_t             rfc;            /* L2CAP_OPT_RFC */
} l2cap_cfg_opt_val_t;

/* L2CAP Disconnect Request */
#define L2CAP_DISCONNECT_REQ            0x06
typedef   struct {
    uint16_t    dcid; /* destination channel ID */
    uint16_t    scid; /* source channel ID */
} __PACKED_POST__ l2cap_discon_req_cp;

/* L2CAP Disconnect Response */
#define L2CAP_DISCONNECT_RSP            0x07
typedef l2cap_discon_req_cp l2cap_discon_rsp_cp;

/* L2CAP Echo Request */
#define L2CAP_ECHO_REQ              0x08
/* No command parameters, only optional data */

/* L2CAP Echo Response */
#define L2CAP_ECHO_RSP              0x09
#define L2CAP_MAX_ECHO_SIZE \
    (L2CAP_MTU_MAXIMUM - sizeof(l2cap_cmd_hdr_t))
/* No command parameters, only optional data */

/* L2CAP Information Request */
#define L2CAP_INFO_REQ              0x0a
typedef   struct {
    uint16_t    type; /* requested information type */
} __PACKED_POST__ l2cap_info_req_cp;

/* L2CAP Information Response */
#define L2CAP_INFO_RSP              0x0b
typedef   struct {
    uint16_t    type;   /* requested information type */
    uint16_t    result; /* 0x00 - success */
    /*  uint8_t info[]  -- info data (depends on type)
     *
     * L2CAP_CONNLESS_MTU - 2 bytes connectionless MTU
     * L2CAP_EXTENDED_FEATURES - 4 bytes
     */
} __PACKED_POST__ l2cap_info_rsp_cp;

typedef union {
    /* L2CAP_CONNLESS_MTU */
      struct {
        uint16_t    mtu;
    } __PACKED_POST__ mtu;
} l2cap_info_rsp_data_t;

/* XXX: consider using only BT_LINK_MODE_xxx for all profiles */
/* L2CAP link mode flags */
#define L2CAP_LM_NONE       BT_LINK_MODE_NONE      /* unsecure */
#define L2CAP_LM_AUTH       BT_LINK_MODE_AUTH      /* want authentication */
#define L2CAP_LM_ENCRYPT    BT_LINK_MODE_ENCRYPT   /* want encryption */
#define L2CAP_LM_SECURE     BT_LINK_MODE_SECURE    /* want secured link */

#endif  /* _BT_L2CAP_H_ */

