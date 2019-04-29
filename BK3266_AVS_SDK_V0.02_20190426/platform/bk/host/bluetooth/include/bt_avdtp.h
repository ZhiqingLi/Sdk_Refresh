/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_AVDTP_H_
#define _BT_AVDTP_H_

#include <bluetooth.h>
#include <bt_a2dp_types.h>
#include <bt_l2cap_proto.h> /* For L2CAP header size */
#include <bt_unit.h>        /* For ACL header size */

/* Signal command and response headers */
typedef   struct {
    uw_u8_t b1;
    uw_u8_t b2;
} __PACKED_POST__ avdtp_hdr_single_t;

typedef   struct {
    uw_u8_t b1;
    uw_u8_t b2;
    uw_u8_t b3;
} __PACKED_POST__ avdtp_hdr_start_t;

typedef   struct {
    uw_u8_t b;
} __PACKED_POST__ avdtp_hdr_cont_t;

#define AVDTP_MSG_COMMAND       0x00
#define AVDTP_MSG_ACCEPT        0x02
#define AVDTP_MSG_REJECT        0x03

#define AVDTP_PKT_SINGLE        0x00
#define AVDTP_PKT_START         0x01
#define AVDTP_PKT_CONT          0x02
#define AVDTP_PKT_END           0x03

/* AVDTP header - "type" byte */
#define AVDTP_MSG_GET(b)                ((b) & 0x3)
#define AVDTP_PKT_GET(b)                (((b) >> 2) & 0x3)
#define AVDTP_TRANSACTION_GET(b)        (((b) >> 4) & 0xf)

#define AVDTP_TID_MAX           0xf

/* AVDTP header - "signal id" byte */
#define AVDTP_SIGNAL_ID_GET(b)          ((b) & 0x3f)

/*
 * Allocate an mbuf capable of holding an AVDTP frame which consists of
 * AVDTP payload and AVDTP, L2CAP, ACL headers.
 * The data pointer is positioned at the beginning of the payload.
 */

#define AVDTP_OVERHEAD (\
    sizeof(avdtp_hdr_start_t) + sizeof(l2cap_hdr_t) + sizeof(hci_acldata_hdr_t))

static __INLINE__ struct mbuf *avdtp_m_get(uint32_t payload_size)
{
    struct mbuf *m;

    m = m_get_flags(MT_DATA, M_PKTHDR, payload_size + AVDTP_OVERHEAD);
    if(m)
    {
        M_ALIGN(m, payload_size);
    }
        
    return m;
}

/* AVDTP error codes */
#define AVDTP_ERR_NONE                          0x00
#define AVDTP_ERR_BAD_HEADER_FORMAT             0x01
#define AVDTP_ERR_BAD_LENGTH                    0x11
#define AVDTP_ERR_BAD_ACP_SEID                  0x12
#define AVDTP_ERR_SEP_IN_USE                    0x13
#define AVDTP_ERR_SEP_NOT_IN_USE                0x14
#define AVDTP_ERR_BAD_SERV_CATEGORY             0x17
#define AVDTP_ERR_BAD_PAYLOAD_FORMAT            0x18
#define AVDTP_ERR_NOT_SUPPORTED_COMMAND         0x19
#define AVDTP_ERR_INVALID_CAPABILITIES          0x1A
#define AVDTP_ERR_BAD_RECOVERY_TYPE             0x22
#define AVDTP_ERR_BAD_MEDIA_TRANSPORT_FORMAT    0x23
#define AVDTP_ERR_BAD_RECOVERY_FORMAT           0x25
#define AVDTP_ERR_BAD_ROHC_FORMAT               0x26
#define AVDTP_ERR_BAD_CP_FORMAT                 0x27
#define AVDTP_ERR_BAD_MULTIPLEXING_FORMAT       0x28
#define AVDTP_ERR_UNSUPPORTED_CONFIGURATION     0x29
#define AVDTP_ERR_BAD_STATE                     0x31
typedef uw_u8_t avdtp_err_t;

#define avdtp_err_str(n) (                                             \
    (n) == AVDTP_ERR_NONE ? "SUCCESS" :                                   \
    (n) == AVDTP_ERR_BAD_HEADER_FORMAT ? "BAD_HEADER_FORMAT" :            \
    (n) == AVDTP_ERR_BAD_LENGTH ? "BAD_LENGTH" :                          \
    (n) == AVDTP_ERR_BAD_ACP_SEID ? "BAD_ACP_SEID" :                      \
    (n) == AVDTP_ERR_SEP_IN_USE ? "SEP_IN_USE" :                          \
    (n) == AVDTP_ERR_SEP_NOT_IN_USE ? "SEP_NOT_IN_USE" :                  \
    (n) == AVDTP_ERR_BAD_SERV_CATEGORY ? "BAD_SERV_CATEGORY" :            \
    (n) == AVDTP_ERR_BAD_PAYLOAD_FORMAT ? "BAD_PAYLOAD_FORMAT" :          \
    (n) == AVDTP_ERR_NOT_SUPPORTED_COMMAND ? "NOT_SUPPORTED_COMMAND" :    \
    (n) == AVDTP_ERR_INVALID_CAPABILITIES ? "INVALID_CAPABILITIES" :      \
    (n) == AVDTP_ERR_BAD_RECOVERY_TYPE ? "BAD_RECOVERY_TYPE" :            \
    (n) == AVDTP_ERR_BAD_MEDIA_TRANSPORT_FORMAT ? "BAD_MEDIA_TRANSPORT_FORMAT":\
    (n) == AVDTP_ERR_BAD_RECOVERY_FORMAT ? "BAD_RECOVERY_FORMAT" :        \
    (n) == AVDTP_ERR_BAD_ROHC_FORMAT ? "BAD_ROHC_FORMAT" :                \
    (n) == AVDTP_ERR_BAD_CP_FORMAT ? "BAD_CP_FORMAT" :                    \
    (n) == AVDTP_ERR_BAD_MULTIPLEXING_FORMAT ? "BAD_MULTIPLEXING_FORMAT" :\
    (n) == AVDTP_ERR_UNSUPPORTED_CONFIGURATION ? "UNSUPPORTED_CONFIGURATION" :\
    (n) == AVDTP_ERR_BAD_STATE ? "BAD_STATE" :                            \
    "ERR UNKNOWN")

/* Signaling command set */
#define AVDTP_DISCOVER          0x01
#define AVDTP_GET_CAPABILITIES  0x02
#define AVDTP_SET_CONFIGURATION 0x03
#define AVDTP_GET_CONFIGURATION 0x04
#define AVDTP_RECONFIGURE       0x05
#define AVDTP_OPEN              0x06
#define AVDTP_START             0x07
#define AVDTP_CLOSE             0x08
#define AVDTP_SUSPEND           0x09
#define AVDTP_ABORT             0x0A
#define AVDTP_SECURITY_CONTROL  0x0B

//The following two commands for AVDTP_v1.3
#define AVDTP_GET_ALL_CAPABILITIES  0x0C
#define AVDTP_DELAY_REPORT          0x0D

/* The media type of an AVDTP stream endpoint */
#define AVDTP_MEDIA_TYPE_AUDIO                  (0x00 << 4)
#define AVDTP_MEDIA_TYPE_VIDEO                  (0x01 << 4)
#define AVDTP_MEDIA_TYPE_MULTIMEDIA             (0x02 << 4)

/* Stream endpoint type */
#define AVDTP_SEP_TYPE_SRC                      0x00
#define AVDTP_SEP_TYPE_SNK                      0x01
typedef uint8_t avdtp_sep_type_t;

/* Service capabilities */
#define AVDTP_SERVICE_CAP_MEDIA_TRANSPORT       0x01
#define AVDTP_SERVICE_CAP_REPORTING             0x02
#define AVDTP_SERVICE_CAP_RECOVERY              0x03
#define AVDTP_SERVICE_CAP_CONTENT_PROTECTION    0x04
#define AVDTP_SERVICE_CAP_HEADER_COMPRESSION    0x05
#define AVDTP_SERVICE_CAP_MULTIPLEXING          0x06
#define AVDTP_SERVICE_CAP_MEDIA_CODEC           0x07
#define AVDTP_SERVICE_CAP_DELAY_REPORTING       0x08

#define avdtp_cap_category_str(n) (                                  \
    n == AVDTP_SERVICE_CAP_MEDIA_TRANSPORT ? "MEDIA TRANSPORT" :        \
    n == AVDTP_SERVICE_CAP_REPORTING ? " REPORTING" :                   \
    n == AVDTP_SERVICE_CAP_RECOVERY ? "RECOVERY" :                      \
    n == AVDTP_SERVICE_CAP_CONTENT_PROTECTION ? "CONTENT_PROTECTION" :  \
    n == AVDTP_SERVICE_CAP_HEADER_COMPRESSION ? "HEADER_COMPRESSION" :  \
    n == AVDTP_SERVICE_CAP_MULTIPLEXING ? "MULTIPLEXING" :              \
    n == AVDTP_SERVICE_CAP_MEDIA_CODEC ? "MEDIA_CODEC" :                \
    n == AVDTP_SERVICE_CAP_DELAY_REPORTING ? "DELAY_REPORTING" :        \
    "CAP UNKNOWN")

/* Stream endpoint identifier range */
#define AVDTP_SEID_MIN                          (0x01 << 2)
#define AVDTP_SEID_MAX                          (0x3E << 2)

/* Socket options */
#define SO_AVDTP_LM                     1       /* link mode */
#define SO_AVDTP_CAPABILITY             2
#define SO_AVDTP_GET_SESSION_ID         3

/* AVDTP link mode flags */
#define AVDTP_LM_NONE           BT_LINK_MODE_NONE      /* unsecure */
#define AVDTP_LM_AUTH           BT_LINK_MODE_AUTH      /* want authentication */
#define AVDTP_LM_ENCRYPT        BT_LINK_MODE_ENCRYPT   /* want encryption */
#define AVDTP_LM_SECURE         BT_LINK_MODE_SECURE    /* want secured link */

/* AVDTP session address, size and fields are compatible with bt_sockaddr_t
 */
typedef struct {
    uint8_t       len;
    btaddr_t      btaddr;
    uint16_t      psm;
    bt_svc_type_t svc_type;
    uint8_t       unused[5];
} sockaddr_avdtp_session_t;

/* AVDTP stream address, size and fields are compatible with bt_sockaddr_t
 */
typedef struct {
    uint8_t     len;
    btaddr_t    btaddr;
    uint16_t    psm;
    uint8_t     sep_id;         /* 1 - 62 */
    uint8_t     in_use;
    uint8_t     media_type;     /* AVDTP_MEDIA_TYPE_... */
    uint8_t     sep_type;       /* AVDTP_SEP_TYPE_... */
    uint8_t     session_id;     /* 1 - 255 */
    uint8_t     unused[2];
} sockaddr_avdtp_stream_t;

/*
 * AVDTP stream endpoint information
 */

typedef uw_u16_t avdtp_sep_info_packed_t;

/* Get/set stream endpoint info */
#define AVDTP_SEP_INFO_UNPACK(packed, sep_id, in_use, media_type, tsep) do { \
    (sep_id) = (uint8_t)(packed)[0];                                         \
    (in_use) = (uint8_t)(((packed)[0] >> 1) & 0x01);                         \
    (media_type) = (uint8_t)(((packed)[1] >> 4) & 0x0f);                     \
    (tsep) = (uint8_t)(((packed)[1] >> 3) & 0x01);                           \
} while (0)

#define AVDTP_SEP_INFO_PACK(packed, sep_id, in_use, media_type, tsep) do {   \
    packed[0] = (uint8_t)((sep_id) |                                         \
        ((in_use) ? 1 << 1 : 0));                                            \
    packed[1] = (uint8_t)((((media_type) & 0x0f) << 4) |                     \
        ((tsep) ? 1 << 3 : 0));                                              \
} while (0)

/*
 * AVDTP service capabilities
 */

typedef   struct {
    uw_u8_t     category; /* AVDTP_SERVICE_CAP_... */
    uw_u8_t     length;   /* Length in bytes of the information element that
                           * follows this header, 0 if no element.
                           */
} __PACKED_POST__ avdtp_cap_generic_t;

typedef avdtp_cap_generic_t avdtp_cap_media_transport_t;
typedef avdtp_cap_generic_t avdtp_cap_reporting_t;
typedef avdtp_cap_generic_t avdtp_delay_reporting_t;

typedef   struct {
    uw_u8_t     category; /* AVDTP_SERVICE_CAP_... */
    uw_u8_t     length;
    uw_u8_t     recovery_type;
    uw_u8_t     mrws; /* Maximum Recovery Window Size for a transport packet */
    uw_u8_t     mnmp; /* The maximum number of media packets a specfic parity
                         code covers */
} __PACKED_POST__ avdtp_cap_recovery_t;

typedef   struct {
    uw_u8_t     category; /* AVDTP_SERVICE_CAP_... */
    uw_u8_t     length;
    uw_u8_t     cp_type_lsb;
    uw_u8_t     cp_type_msb;
} __PACKED_POST__ avdtp_cap_content_protection_t;

typedef   struct {
    uw_u8_t     category; /* AVDTP_SERVICE_CAP_... */
    uw_u8_t     length;
    uw_u8_t     hc;
} __PACKED_POST__ avdtp_cap_header_compression_t;

#define AVDTP_HC_BACK_CHANNEL   (1 << 7)
#define AVDTP_HC_MEDIA          (1 << 6)
#define AVDTP_HC_RECOVERY       (1 << 5)

typedef   struct {
    uw_u8_t     category; /* AVDTP_SERVICE_CAP_... */
    uw_u8_t     length;
    uw_u8_t     tsid_media;
    uw_u8_t     tcid_media;
    uw_u8_t     tsid_reporting;
    uw_u8_t     tcid_reporting;
    uw_u8_t     tsid_recovery;
    uw_u8_t     tcid_recovery;
} __PACKED_POST__ avdtp_cap_multiplexing_t;

typedef   struct {
    uw_u8_t          category;   /* AVDTP_SERVICE_CAP_... */
    uw_u8_t          length;
    uw_u8_t          media_type; /* AVDTP_MEDIA_TYPE_... */
    uw_u8_t          media_codec_type;
    union a2dp_codec u;
} __PACKED_POST__ avdtp_cap_media_codec_t;

typedef   struct {
    uw_u8_t          category;   /* Service Category = Delay Reporting */
    uw_u8_t          length;    /* Length Of Service Capability (LOSC) = 0x00 */
} __PACKED_POST__ avdtp_cap_delay_reporting_t;

typedef struct avdtp_cap {
    STAILQ_ENTRY(avdtp_cap)     cap_next;
    avdtp_cap_generic_t         hdr;
}avdtp_cap_t;

typedef STAILQ_HEAD(, avdtp_cap) avdtp_cap_list_t;

/* AVDTP Media Packet header - the mandatory part */
typedef   struct {
    uw_u8_t     octet0;         /* V, P, X, CC bits */
    uw_u8_t     octet1;         /* M, PT bits */
    uw_u16_t    sequence_number;
    uw_u32_t    time_stamp;
    uw_u32_t    ssrc;
} __PACKED_POST__ avdtp_media_packet_hdr_t;

/* octet 0 */
#define AVDTP_MEDIA_PACKET_VERSION      0xc0
#define AVDTP_MEDIA_PACKET_PADDING      0x20
#define AVDTP_MEDIA_PACKET_EXTENSION    0x10
#define AVDTP_MEDIA_PACKET_CSRC_COUNT   0x0f

/* octet 1 */
#define AVDTP_MEDIA_PACKET_MARKER       0x80
#define AVDTP_MEDIA_PACKET_PAYLOAD_TYPE 0x3f

/* AVDTP commands/responses, sent over "avdtp session"  */

typedef   struct {
    uint8_t             sig_id;
} __PACKED_POST__ avdtp_cmd_discover_t;

typedef   struct {
    uint8_t             sig_id;
    uint8_t             sep_id;
} __PACKED_POST__ avdtp_cmd_get_caps_t;

typedef   struct {
    uint8_t             sig_id;
    uint8_t             accept;
    uint8_t             acp_sep_id;
} __PACKED_POST__ avdtp_resp_hdr_t;

#ifdef CONFIG_TWS
typedef struct{
    uint8_t   tid;
    uint8_t   remote_sep_id;
    bt_a2dp_codec_t codec;
}avdtp_reconfig_t;
#endif

typedef struct {
	uint8_t rfa0:2;
	uint8_t acp_seid:6;
	uint16_t delay;
} delay_req_t;

#endif
