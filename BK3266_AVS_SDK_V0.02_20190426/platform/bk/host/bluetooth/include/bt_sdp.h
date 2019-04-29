/* Modifications by Jungo Ltd. Copyright (c) 2012 Jungo Ltd. All Rights reserved */

/*
 * Copyright (c) 2006 Itronix Inc.
 * All rights reserved.
 *
 * Written by Iain Hibbert for Itronix Inc.
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
 * Copyright (c) 2001-2003, 2008 Maksim Yevmenkin <m_evmenkin@yahoo.com>
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

#ifndef _BT_SDP_H_
#define _BT_SDP_H_

#include <uw_types.h>
#include <uw_types_util.h>
#include <bluetooth.h>
#include <bt_svc_type.h>

/*
 * Data representation (page 349)
 */

/**********************************************
 * VARIOUS SDP PROTOCOL/PROFILE VERSION DATA.
 **********************************************/
#define SDP_PROTOCOL_AVDTP_VERSION_0102                 0x0102 // v1.2
#define SDP_PROTOCOL_AVDTP_VERSION_0103                 0x0103 // v1.3

#define SDP_PROTOCOL_AVCTP_VERSION_0104                 0x0104 // v1.4

#define SDP_PROFILE_A2DP_VERSION_0102                   0x0102 // v1.2
#define SDP_PROFILE_A2DP_VERSION_0103                   0x0103 // v1.3

#define SDP_PROFILE_AVRCP_VERSION_0105                  0x0105 // v1.5 
#define SDP_PROFILE_AVRCP_VERSION_0106                  0x0106 // v1.6 

#define SDP_PROFILE_GAVDP_VERSION_0103                  0x0103 // v1.3

#define SDP_PROFILE_HFP_VERSION_0105                    0x0105 // v1.5
#define SDP_PROFILE_HFP_VERSION_0106                    0x0106 // v1.6
#define SDP_PROFILE_HFP_VERSION_0107                    0x0107 // v1.7

/* Nil, the null type */
#define SDP_DATA_NIL                                    0x00

/* Unsigned integer */
#define SDP_DATA_UINT8                                  0x08
#define SDP_DATA_UINT16                                 0x09
#define SDP_DATA_UINT32                                 0x0A
#define SDP_DATA_UINT64                                 0x0B
#define SDP_DATA_UINT128                                0x0C

/* Signed two's-complement integer */
#define SDP_DATA_INT8                                   0x10
#define SDP_DATA_INT16                                  0x11
#define SDP_DATA_INT32                                  0x12
#define SDP_DATA_INT64                                  0x13
#define SDP_DATA_INT128                                 0x14

/* UUID, a universally unique identifier */
#define SDP_DATA_UUID16                                 0x19
#define SDP_DATA_UUID32                                 0x1A
#define SDP_DATA_UUID128                                0x1C

/* Text string */
#define SDP_DATA_STR8                                   0x25
#define SDP_DATA_STR16                                  0x26
#define SDP_DATA_STR32                                  0x27

/* Boolean */
#define SDP_DATA_BOOL                                   0x28

/*
 * Data element sequence.
 * A data element whose data field is a sequence of data elements
 */
#define SDP_DATA_SEQ8                                   0x35
#define SDP_DATA_SEQ16                                  0x36
#define SDP_DATA_SEQ32                                  0x37

/*
 * Data element alternative.
 * A data element whose data field is a sequence of data elements from
 * which one data element is to be selected.
 */
#define SDP_DATA_ALT8                                   0x3D
#define SDP_DATA_ALT16                                  0x3E
#define SDP_DATA_ALT32                                  0x3F

/* URL, a uniform resource locator */
#define SDP_DATA_URL8                                   0x45
#define SDP_DATA_URL16                                  0x46
#define SDP_DATA_URL32                                  0x47

#define SDP_BASE_UUID \
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, \
    0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }

/*
 * Protocols UUID (short) http://www.bluetoothsig.org/assigned-numbers/sdp.htm
 * BASE UUID 00000000-0000-1000-8000-00805F9B34FB
 */

#define SDP_UUID_PROTOCOL_SDP                           0x0001
#define SDP_UUID_PROTOCOL_UDP                           0x0002
#define SDP_UUID_PROTOCOL_RFCOMM                        0x0003
#define SDP_UUID_PROTOCOL_TCP                           0x0004
#define SDP_UUID_PROTOCOL_TCS_BIN                       0x0005
#define SDP_UUID_PROTOCOL_TCS_AT                        0x0006
#define SDP_UUID_PROTOCOL_OBEX                          0x0008
#define SDP_UUID_PROTOCOL_IP                            0x0009
#define SDP_UUID_PROTOCOL_FTP                           0x000A
#define SDP_UUID_PROTOCOL_HTTP                          0x000C
#define SDP_UUID_PROTOCOL_WSP                           0x000E
#define SDP_UUID_PROTOCOL_BNEP                          0x000F
#define SDP_UUID_PROTOCOL_UPNP                          0x0010
#define SDP_UUID_PROTOCOL_HIDP                          0x0011
#define SDP_UUID_PROTOCOL_HARDCOPY_CONTROL_CHANNEL      0x0012
#define SDP_UUID_PROTOCOL_HARDCOPY_DATA_CHANNEL         0x0014
#define SDP_UUID_PROTOCOL_HARDCOPY_NOTIFICATION         0x0016
#define SDP_UUID_PROTOCOL_AVCTP                         0x0017
#define SDP_UUID_PROTOCOL_AVDTP                         0x0019
#define SDP_UUID_PROTOCOL_CMPT                          0x001B
#define SDP_UUID_PROTOCOL_UDI_C_PLANE                   0x001D
#define SDP_UUID_PROTOCOL_L2CAP                         0x0100

/*
 * Service class IDs
 * https://www.bluetooth.org/Technical/AssignedNumbers/service_discovery.htm
 */

#define SDP_SERVICE_CLASS_SERVICE_DISCOVERY_SERVER      0x1000
#define SDP_SERVICE_CLASS_BROWSE_GROUP_DESCRIPTOR       0x1001
#define SDP_SERVICE_CLASS_PUBLIC_BROWSE_GROUP           0x1002
#define SDP_SERVICE_CLASS_SERIAL_PORT                   0x1101
#define SDP_SERVICE_CLASS_LAN_ACCESS_USING_PPP          0x1102
#define SDP_SERVICE_CLASS_DIALUP_NETWORKING             0x1103
#define SDP_SERVICE_CLASS_IR_MC_SYNC                    0x1104
#define SDP_SERVICE_CLASS_OBEX_OBJECT_PUSH              0x1105
#define SDP_SERVICE_CLASS_OBEX_FILE_TRANSFER            0x1106
#define SDP_SERVICE_CLASS_IR_MC_SYNC_COMMAND            0x1107
#define SDP_SERVICE_CLASS_HEADSET                       0x1108
#define SDP_SERVICE_CLASS_CORDLESS_TELEPHONY            0x1109
#define SDP_SERVICE_CLASS_AUDIO_SOURCE                  0x110A
#define SDP_SERVICE_CLASS_AUDIO_SINK                    0x110B
#define SDP_SERVICE_CLASS_AV_REMOTE_CONTROL_TARGET      0x110C
#define SDP_SERVICE_CLASS_ADVANCED_AUDIO_DISTRIBUTION   0x110D
#define SDP_SERVICE_CLASS_AV_REMOTE_CONTROL             0x110E
#define SDP_SERVICE_CLASS_AV_REMOTE_CONTROL_CONTROLLER  0x110F
#define SDP_SERVICE_CLASS_INTERCOM                      0x1110
#define SDP_SERVICE_CLASS_FAX                           0x1111
#define SDP_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY         0x1112
#define SDP_SERVICE_CLASS_WAP                           0x1113
#define SDP_SERVICE_CLASS_WAP_CLIENT                    0x1114
#define SDP_SERVICE_CLASS_PANU                          0x1115
#define SDP_SERVICE_CLASS_NAP                           0x1116
#define SDP_SERVICE_CLASS_GN                            0x1117
#define SDP_SERVICE_CLASS_DIRECT_PRINTING               0x1118
#define SDP_SERVICE_CLASS_REFERENCE_PRINTING            0x1119
#define SDP_SERVICE_CLASS_IMAGING                       0x111A
#define SDP_SERVICE_CLASS_IMAGING_RESPONDER             0x111B
#define SDP_SERVICE_CLASS_IMAGING_AUTOMATIC_ARCHIVE     0x111C
#define SDP_SERVICE_CLASS_IMAGING_REFERENCED_OBJECTS    0x111D
#define SDP_SERVICE_CLASS_HANDSFREE                     0x111E
#define SDP_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY       0x111F
#define SDP_SERVICE_CLASS_DIRECT_PRINTING_REFERENCE_OBJECTS     0x1120
#define SDP_SERVICE_CLASS_REFLECTED_UI                  0x1121
#define SDP_SERVICE_CLASS_BASIC_PRINTING                0x1122
#define SDP_SERVICE_CLASS_PRINTING_STATUS               0x1123
#define SDP_SERVICE_CLASS_HUMAN_INTERFACE_DEVICE        0x1124
#define SDP_SERVICE_CLASS_HARDCOPY_CABLE_REPLACEMENT    0x1125
#define SDP_SERVICE_CLASS_HCR_PRINT                     0x1126
#define SDP_SERVICE_CLASS_HCR_SCAN                      0x1127
#define SDP_SERVICE_CLASS_COMMON_ISDN_ACCESS            0x1128
#define SDP_SERVICE_CLASS_VIDEO_CONFERENCING_GW         0x1129
#define SDP_SERVICE_CLASS_UDI_MT                        0x112A
#define SDP_SERVICE_CLASS_UDI_TA                        0x112B
#define SDP_SERVICE_CLASS_AUDIO_VIDEO                   0x112C
#define SDP_SERVICE_CLASS_SIM_ACCESS                    0x112D
#define SDP_SERVICE_CLASS_PHONEBOOK_ACCESS_PCE          0x112E
#define SDP_SERVICE_CLASS_PHONEBOOK_ACCESS_PSE          0x112F
#define SDP_SERVICE_CLASS_PHONEBOOK_ACCESS              0x1130
#define SDP_SERVICE_CLASS_HEADSET_HS                    0x1131
#define SDP_SERVICE_CLASS_MESSAGE_ACCESS_SERVER         0x1132
#define SDP_SERVICE_CLASS_MESSAGE_NOTIFICATION_SERVER   0x1133
#define SDP_SERVICE_CLASS_MESSAGE_ACCESS                0x1134
#define SDP_SERVICE_CLASS_PNP_INFORMATION               0x1200
#define SDP_SERVICE_CLASS_GENERIC_NETWORKING            0x1201
#define SDP_SERVICE_CLASS_GENERIC_FILE_TRANSFER         0x1202
#define SDP_SERVICE_CLASS_GENERIC_AUDIO                 0x1203
#define SDP_SERVICE_CLASS_GENERIC_TELEPHONY             0x1204
#define SDP_SERVICE_CLASS_UPNP                          0x1205
#define SDP_SERVICE_CLASS_UPNP_IP                       0x1206
#define SDP_SERVICE_CLASS_ESDP_UPNP_IP_PAN              0x1300
#define SDP_SERVICE_CLASS_ESDP_UPNP_IP_LAP              0x1301
#define SDP_SERVICE_CLASS_ESDP_UPNP_L2CAP               0x1302

#define SDP_SERVICE_CLASS_SCO                           0x1FFF /* internal */

#define SDP_SERVICE_CLASS_WIAP_DEVICE                        \
    { 0x00, 0x00, 0x00, 0x00, 0xDE, 0xCA, 0xFA, 0xDE,        \
      0xDE, 0xCA, 0xDE, 0xAF, 0xDE, 0xCA, 0xCA, 0xFE }

#define SDP_SERVICE_CLASS_WIAP_ACCESSORY                     \
    { 0x00, 0x00, 0x00, 0x00, 0xDE, 0xCA, 0xFA, 0xDE,        \
      0xDE, 0xCA, 0xDE, 0xAF, 0xDE, 0xCA, 0xCA, 0xFF }

/* to-string conversion for most commonly used service class values */
#define sdp_service_class_str(n) (\
    n == SDP_SERVICE_CLASS_SERVICE_DISCOVERY_SERVER ?\
        "Service Discovery Server" :\
    n == SDP_SERVICE_CLASS_SERIAL_PORT ? "Serial Port" :\
    n == SDP_SERVICE_CLASS_OBEX_OBJECT_PUSH ? "OBEX Object Push" :\
    n == SDP_SERVICE_CLASS_OBEX_FILE_TRANSFER ? "OBEX File Transfer" :\
    n == SDP_SERVICE_CLASS_PHONEBOOK_ACCESS_PSE ? "Phonebook Access PSE" : \
    n == SDP_SERVICE_CLASS_DIALUP_NETWORKING ? "Dialup Networking" :\
    n == SDP_SERVICE_CLASS_HEADSET ? "Headset" :\
    n == SDP_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY ? "Headset Audio Gateway" :\
    n == SDP_SERVICE_CLASS_HEADSET_HS ? "Headset HS" :\
    n == SDP_SERVICE_CLASS_AUDIO_SOURCE ? "A2DP Source" :\
    n == SDP_SERVICE_CLASS_AUDIO_SINK ? "A2DP Sink" :\
    n == SDP_SERVICE_CLASS_AV_REMOTE_CONTROL ? "AVRCP" :\
    n == SDP_SERVICE_CLASS_AV_REMOTE_CONTROL_CONTROLLER ? "AVRCP Controller" :\
    n == SDP_SERVICE_CLASS_AV_REMOTE_CONTROL_TARGET ? "AVRCP Target" :\
    n == SDP_SERVICE_CLASS_HANDSFREE ? "Handsfree" :\
    n == SDP_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY ? \
        "Handsfree Audio Gateway" :\
    n == SDP_SERVICE_CLASS_HUMAN_INTERFACE_DEVICE ? "HID" : \
    n == SDP_SERVICE_CLASS_MESSAGE_ACCESS_SERVER ? "Message Access Server" : \
    n == SDP_SERVICE_CLASS_NAP ? "Network Access Point" : \
    n == SDP_SERVICE_CLASS_SIM_ACCESS ? "SIM Access" : \
    n == SDP_SERVICE_CLASS_PNP_INFORMATION ? "PNP Information" : \
    n == SDP_SERVICE_CLASS_GENERIC_NETWORKING ? "Generic Networking" : \
    n == SDP_SERVICE_CLASS_GENERIC_FILE_TRANSFER ? "Generic File Transfer" : \
    n == SDP_SERVICE_CLASS_GENERIC_AUDIO ? "Generic Audio" : \
    n == SDP_SERVICE_CLASS_GENERIC_TELEPHONY ? "Generic Telephony" : \
    "")

/*
 * Universal attribute definitions (Bluetooth Specification, vol 3, part B, 5.1)
 * and http://www.bluetoothsig.org/assigned-numbers/sdp.htm
 */

//Maximum attr ID supported by BEKEN 
#define BEKEN_SDP_ATTR_MAX 0x400 

#define SDP_ATTR_RANGE(lo, hi) \
        (uint32_t)(((uint16_t)(lo) << 16) | ((uint16_t)(hi)))

#define SDP_ATTR_SERVICE_RECORD_HANDLE                  0x0000
#define SDP_ATTR_SERVICE_CLASS_ID_LIST                  0x0001
#define SDP_ATTR_SERVICE_RECORD_STATE                   0x0002
#define SDP_ATTR_SERVICE_ID                             0x0003
#define SDP_ATTR_PROTOCOL_DESCRIPTOR_LIST               0x0004
#define SDP_ATTR_BROWSE_GROUP_LIST                      0x0005
#define SDP_ATTR_LANGUAGE_BASE_ATTRIBUTE_ID_LIST        0x0006
#define SDP_ATTR_SERVICE_INFO_TIME_TO_LIVE              0x0007
#define SDP_ATTR_SERVICE_AVAILABILITY                   0x0008
#define SDP_ATTR_BLUETOOTH_PROFILE_DESCRIPTOR_LIST      0x0009
#define SDP_ATTR_DOCUMENTATION_URL                      0x000A
#define SDP_ATTR_CLIENT_EXECUTABLE_URL                  0x000B
#define SDP_ATTR_ICON_URL                               0x000C
#define SDP_ATTR_ADDITIONAL_PROTOCOL_DESCRIPTOR_LISTS   0x000D
#define SDP_ATTR_GROUP_ID                               0x0200
#define SDP_ATTR_IP_SUBNET                              0x0200
#define SDP_ATTR_VERSION_NUMBER_LIST                    0x0200
#define SDP_ATTR_SERVICE_DATABASE_STATE                 0x0201
#define SDP_ATTR_SERVICE_VERSION                        0x0300
#define SDP_ATTR_EXTERNAL_NETWORK                       0x0301
#define SDP_ATTR_NETWORK                                0x0301
#define SDP_ATTR_SUPPORTED_DATA_STORES_LIST             0x0301
#define SDP_ATTR_FAX_CLASS1_SUPPORT                     0x0302
#define SDP_ATTR_REMOTE_AUDIO_VOLUME_CONTROL            0x0302
#define SDP_ATTR_FAX_CLASS20_SUPPORT                    0x0303
#define SDP_ATTR_SUPPORTED_FORMATS_LIST                 0x0303
#define SDP_ATTR_FAX_CLASS2_SUPPORT                     0x0304
#define SDP_ATTR_AUDIO_FEEDBACK_SUPPORT                 0x0305
#define SDP_ATTR_NETWORK_ADDRESS                        0x0306
#define SDP_ATTR_WAP_GATEWAY                            0x0307
#define SDP_ATTR_HOME_PAGE_URL                          0x0308
#define SDP_ATTR_WAP_STACK_TYPE                         0x0309
#define SDP_ATTR_SECURITY_DESCRIPTION                   0x030A
#define SDP_ATTR_NET_ACCESS_TYPE                        0x030B
#define SDP_ATTR_MAX_NET_ACCESS_RATE                    0x030C
#define SDP_ATTR_IPV4_SUBNET                            0x030D
#define SDP_ATTR_IPV6_SUBNET                            0x030E
#define SDP_ATTR_SUPPORTED_CAPABALITIES                 0x0310
#define SDP_ATTR_SUPPORTED_FEATURES                     0x0311
#define SDP_ATTR_SUPPORTED_FUNCTIONS                    0x0312
#define SDP_ATTR_TOTAL_IMAGING_DATA_CAPACITY            0x0313
#define SDP_ATTR_SUPPORTED_REPOSITORIES                 0x0314
#define SDP_ATTR_MAS_INSTANCE_ID                        0x0315
#define SDP_ATTR_SUPPORTED_MESSAGE_TYPES                0x0316

/*HID Attribute ID definition -- added by wxj*/
#define SDP_ATTR_HID_PARSER_VERSION			0x0201
#define	SDP_ATTR_HID_DEVICE_SUBCLASS		0x0202
#define	SDP_ATTR_HID_COUNTRY_CODE			0x0203
#define	SDP_ATTR_HID_VIRTUAL_CABLE			0x0204
#define	SDP_ATTR_HID_RECONNECT_INITIATE		0x0205
#define	SDP_ATTR_HID_DESCRIPTOR_LIST		0x0206
#define	SDP_ATTR_HID_LANGID_BASE_LIST		0x0207
#define	SDP_ATTR_HID_BOOT_DEVICE			0x020E
/* Device ID Service (PNP information) */
#define SDP_ATTR_DIP_SPECIFICATION_ID                   0x0200
#define SDP_ATTR_DIP_VENDOR_ID                          0x0201
#define SDP_ATTR_DIP_PRODUCT_ID                         0x0202
#define SDP_ATTR_DIP_VERSION                            0x0203
#define SDP_ATTR_DIP_PRIMARY_RECORD                     0x0204
#define SDP_ATTR_DIP_VENDOR_ID_SOURCE                   0x0205

/*
 * The offset must be added to the attribute ID base (contained in the
 * LANGUAGE_BASE_ATTRIBUTE_ID_LIST attribute) in order to compute the
 * attribute ID for these attributes.
 */

#define SDP_ATTR_PRIMARY_LANGUAGE_BASE_ID               0x0100
#define SDP_ATTR_SERVICE_NAME_OFFSET                    0x0000
#define SDP_ATTR_SERVICE_DESCRIPTION_OFFSET             0x0001
#define SDP_ATTR_PROVIDER_NAME_OFFSET                   0x0002

#define SDP_ATTR_SERVICE_NAME(base)                                 \
    ((base) + SDP_ATTR_SERVICE_NAME_OFFSET)
#define SDP_ATTR_SERVICE_DESCRIPTION(base)                          \
    ((base) + SDP_ATTR_SERVICE_DESCRIPTION_OFFSET)
#define SDP_ATTR_PROVIDER_NAME(base)                                \
    ((base) + SDP_ATTR_PROVIDER_NAME_OFFSET)

#define SDP_ATTR_SERVICE_NAME_PRIMARY                               \
    SDP_ATTR_SERVICE_NAME(SDP_ATTR_PRIMARY_LANGUAGE_BASE_ID)
#define SDP_ATTR_SERVICE_DESCRIPTION_PRIMARY                        \
    SDP_ATTR_SERVICE_DESCRIPTION(SDP_ATTR_PRIMARY_LANGUAGE_BASE_ID)
#define SDP_ATTR_PROVIDER_NAME_PRIMARY                              \
    SDP_ATTR_PROVIDER_NAME(SDP_ATTR_PRIMARY_LANGUAGE_BASE_ID)

/*
 * Protocol data unit (PDU) format (page 352)
 */

#define SDP_PDU_ERROR_RESPONSE                          0x01
#define SDP_PDU_SERVICE_SEARCH_REQUEST                  0x02
#define SDP_PDU_SERVICE_SEARCH_RESPONSE                 0x03
#define SDP_PDU_SERVICE_ATTRIBUTE_REQUEST               0x04
#define SDP_PDU_SERVICE_ATTRIBUTE_RESPONSE              0x05
#define SDP_PDU_SERVICE_SEARCH_ATTRIBUTE_REQUEST        0x06
#define SDP_PDU_SERVICE_SEARCH_ATTRIBUTE_RESPONSE       0x07

typedef   struct sdp_pdu {
    uw_u8_t     pid;    /* PDU ID - SDP_PDU_xxx */
    uw_u16_t    tid;    /* transaction ID */
    uw_u16_t    len;    /* parameters length (in bytes) */
} __PACKED_POST__ sdp_pdu_t;

/*
 * Error codes for SDP_PDU_ERROR_RESPONSE
 */
#define SDP_ERROR_CODE_INVALID_SDP_VERSION              0x0001
#define SDP_ERROR_CODE_INVALID_SERVICE_RECORD_HANDLE    0x0002
#define SDP_ERROR_CODE_INVALID_REQUEST_SYNTAX           0x0003
#define SDP_ERROR_CODE_INVALID_PDU_SIZE                 0x0004
#define SDP_ERROR_CODE_INVALID_CONTINUATION_STATE       0x0005
#define SDP_ERROR_CODE_INSUFFICIENT_RESOURCES           0x0006

/*
 * SDP int128/uint128 parameter
 */

typedef struct {
    uw_u128_t   val;  /* big endian */
    uint8_t        size; /* The UUID size in bytes: 2, 4, 16 */
#ifdef SUPPORT_128BITS_UUID
    uint8_t        uuid_type; /* SDP_DATA_UUID16 / SDP_DATA_UUID32 / SDP_DATA_UUID128 */
#endif
} uuid_t;

/*
 * compares the two uuids and returns a value indicating their relationship.
 * if the two are not of the same size, the function extends the smaller one and
 * then performs the comparison
 */
bool_t uuid_same(const uuid_t *uuid1, const uuid_t *uuid2);
#ifdef SUPPORT_128BITS_UUID
bool_t uuid_same_all(const uuid_t *uuid_short, const uuid_t *uuid_long);
#endif
#ifdef J_DEBUG
const char *sdp_pdu_to_str(uint8_t pid);
const char *sdp_error_code_to_str(uint16_t err_code);
void uuid_dump(const char *file, int_t line, const uuid_t *uuid);
#define UUID_DUMP(dbg_type, dbg_level, uuid) \
    DBG_RUN(dbg_type, dbg_level, uuid_dump(__FILE__, __LINE__, uuid));
#else
#define UUID_DUMP(dbg_type, dbg_level, uuid) do {} while(0)
#endif

/*
 * uuid_t initializer macros
 */
#define UUID_INIT16(uuid16)                                               \
    {                                                                     \
        {                                                                 \
            0,                                                            \
            0,                                                            \
            (uint8_t)((uuid16) >> 8),                                     \
            (uint8_t)(uuid16)                                             \
        },                                                                \
        sizeof(uint16_t)                                                  \
    }

#define UUID_INIT32(uuid32)                                               \
    {                                                                     \
        {                                                                 \
            (uint8_t)((uuid32) >> 24),                                    \
            (uint8_t)((uuid32) >> 16),                                    \
            (uint8_t)((uuid32) >> 8),                                     \
            (uint8_t)(uuid32)                                             \
        },                                                                \
        sizeof(uint32_t)                                                  \
    }

#define UUID_INIT128(full_uuid) {full_uuid, sizeof(uw_u128_t)}

/*
 * uuid_t getter/setter macros
 */
#define uuid_set16(uuid, value) do {                                      \
    (uuid).size = sizeof(uint16_t);                                       \
    ua_set_be32((uuid).val, (uint16_t)(value));                           \
} while (0)

#define uuid_get16(uuid)                                                  \
    ((uuid).size == sizeof(uint16_t) ? (uint16_t)ua_be32toh((uuid).val) : 0)

#define uuid_set32(uuid, value) do {                                      \
    (uuid).size = sizeof(uint32_t);                                       \
    ua_set_be32((uuid).val, (value));                                     \
} while (0)

#define uuid_get32(uuid)                                                  \
    ((uuid).size == sizeof(uint32_t) ? ua_be32toh((uuid).val) : 0)

#define uuid_set128(uuid, ptr) do {                                       \
    (uuid).size = sizeof(uw_u128_t);                                      \
    j_memcpy((uuid).val, ptr, sizeof(uw_u128_t));                         \
} while (0)

#define uuid_get128(uuid)                                                 \
    ((uuid).size == sizeof(uw_u128_t) ? (uuid).val : NULL)

#define uuid_is_valid(v) ((v).size == 2 || (v).size == 4 || (v).size == 16)

#define sdp_uuid_type(uuid)                                               \
    ((uuid).size == 2 ? SDP_DATA_UUID16 :                                 \
     (uuid).size == 4 ? SDP_DATA_UUID32 : SDP_DATA_UUID128)

#define sdp_uuid_size_by_type(type)                                       \
    (type == SDP_DATA_UUID16 ? sizeof(uint16_t) :                         \
     type == SDP_DATA_UUID32 ? sizeof(uint32_t) :                         \
     type == SDP_DATA_UUID128 ? sizeof(uw_u128_t) : 0)

/*
 * SDP attribute
 */

typedef struct sdp_attr {
    uint16_t    attr;  /* SDP_ATTR_xxx */
    uint32_t    vlen;  /* length of the value[] in bytes */
    uint8_t    *value; /* base pointer */
} sdp_attr_t;

/******************************************************************************
 * User interface
 *****************************************************************************/

#define SDP_PARSE_ERROR UWE_NOENT

/* Inline versions of get/put byte/short/long. Pointer is advanced */
#define SDP_GET8(b, cp)         do {                            \
    (b) = *(const uint8_t *)(cp);                               \
    (cp) += sizeof(uint8_t);                                    \
} while (/* CONSTCOND */0)

#define SDP_GET16(s, cp)        do {                            \
    (s) = ua_get_be16(cp);                                      \
    (cp) += sizeof(uint16_t);                                   \
} while (/* CONSTCOND */0)

#define SDP_GET32(l, cp)        do {                            \
    (l) = ua_get_be32(cp);                                      \
    (cp) += sizeof(uint32_t);                                   \
} while (/* CONSTCOND */0)

#define SDP_PUT8(b, cp)         do {                            \
    *(uint8_t *)(cp) = (b);                                     \
    (cp) += sizeof(uint8_t);                                    \
} while (/* CONSTCOND */0)

#define SDP_PUT16(s, cp)        do {                            \
    ua_set_be16((cp), (s));                                     \
    (cp) += sizeof(uint16_t);                                   \
} while (/* CONSTCOND */0)

#define SDP_PUT32(s, cp)        do {                            \
    ua_set_be32((cp), (s));                                     \
    (cp) += sizeof(uint32_t);                                   \
} while (/* CONSTCOND */0)

#define SDP_GET_UUID(uuid, cp, _size)  do {                     \
    (uuid).size = (_size);                                      \
    if((_size) == sizeof(uint16_t))                            \
    {                                                           \
        j_memset((uuid).val, 0, 2);                             \
        j_memcpy((uuid).val + 2, (cp), sizeof(uint16_t));       \
    }                                                           \
    else                                                        \
        j_memcpy((uuid).val, (cp), (uuid).size);                \
    (cp) += (uuid).size;                                        \
} while (/* CONSTCOND */0)

#define SDP_PUT_UUID(uuid, cp)  do {                            \
    if((uuid).size == sizeof(uint16_t))                        \
        j_memcpy((cp), (uuid).val + 2, sizeof(uint16_t));       \
    else                                                        \
        j_memcpy((cp), (uuid).val, (uuid).size);                \
    (cp) += (uuid).size;                                        \
} while (/* CONSTCOND */0)

typedef struct sdp_session *sdp_session_h;

char const *sdp_attr2desc(uint16_t attr);
char const *sdp_uuid2desc(uint16_t uuid);
void sdp_print(uint32_t level, uint8_t *start, uint8_t const *end);

#define SDP_MAX_SERVICE_NAME    48

/*
 * SDP PDU structs
 */
typedef struct {
    uint8_t server_channel;
    uint8_t audio_feedback_support;
    uint8_t reserved[2];
} sdp_dun_profile_t;

typedef struct {
    uint8_t server_channel;
    uint8_t reserved[3];
} sdp_ftrn_profile_t;

typedef struct {
    bt_svc_type_t svc_type;  /* One of the following bt_svc_type_t values:
                              * BT_SVC_TYPE_HANDSFREE
                              * BT_SVC_TYPE_HANDSFREE_AUDIO_GATEWAY
                              * BT_SVC_TYPE_HEADSET
                              * BT_SVC_TYPE_HEADSET_AUDIO_GATEWAY
                              */
    uint8_t     server_channel;
    union {
        uint16_t hf_supported_features;    /* Hands-free features */
        uint8_t  hs_remote_volume_control; /* Headset volume control, 0 or 1 */
    } u;
    char        service_name[SDP_MAX_SERVICE_NAME];
} sdp_hset_profile_t, sdp_hfree_profile_t;

/* Keep this in sync with sdp_opush_profile */
typedef struct {
    uint8_t server_channel;
    uint8_t supported_formats_n;
    uint8_t supported_formats[30];
} sdp_irmc_profile_t;

typedef struct {
    uint8_t server_channel;
    uint8_t reserved[3];
} sdp_irmc_command_profile_t;

typedef struct {
    uint8_t     server_channel;
    uint8_t     load_factor;
    uint8_t     reserved;
    uint8_t     ip_subnet_radius;
    uint32_t    ip_subnet;
} sdp_lan_profile_t;

/* Keep this in sync with sdp_irmc_profile */

#define OPUSH_MAX_SUPPORTED_FORMATS     30

typedef struct {
    uint8_t server_channel;
    uint8_t supported_formats_n;
    uint8_t supported_formats[OPUSH_MAX_SUPPORTED_FORMATS];
    char    service_name[SDP_MAX_SERVICE_NAME];
} sdp_opush_profile_t;

typedef struct {
	bt_svc_type_t svc_type;
    uint8_t server_channel;
    char    service_name[SDP_MAX_SERVICE_NAME];
} sdp_sp_profile_t;

typedef struct {
    uint8_t  reserved;
    uint8_t  load_factor;
    uint16_t security_description;
    uint16_t net_access_type;
    uint32_t max_net_access_rate;
    uint16_t role_uuid;
} sdp_pan_profile_t;

/* Advanced Audio Distribution Profile - service record for source/sink */
typedef struct {
    bt_svc_type_t svc_type; /* audio source or sink */
    uint16_t      supported_features;
    char          service_name[SDP_MAX_SERVICE_NAME];
    /* PSM, AVDTP version and A2DP version are hard coded */
} sdp_a2dp_profile_t;

/* Audio/Video Remote Control Profile - service record for control/target */
typedef struct {
    bt_svc_type_t svc_type;
    uint16_t      supported_features;
    char          service_name[SDP_MAX_SERVICE_NAME];
    /* PSM, AVCTP version and AVRCP version are hard coded */
} sdp_avrcp_profile_t;

/* Phonebook Access Profile */
typedef struct {
    uint8_t     server_channel;
    uint16_t    supported_repositories;
    char        service_name[SDP_MAX_SERVICE_NAME];
} sdp_pse_profile_t;

/* HID Profile  */
typedef struct {
    bt_svc_type_t svc_type;
    char          service_name[SDP_MAX_SERVICE_NAME];
} sdp_hid_profile_t;


#define PBAP_REPOSITORY_LOCAL_PHONEBOOK         (1 << 0)
#define PBAP_REPOSITORY_SIM_CARD                (1 << 1)

#define PBAP_REPOSITORY_ALL \
    (PBAP_REPOSITORY_LOCAL_PHONEBOOK | PBAP_REPOSITORY_SIM_CARD)

/* Wireless iAP profile */
typedef struct {
    uint8_t     server_channel;
    char        service_name[SDP_MAX_SERVICE_NAME];
} sdp_wiap_profile_t;

/* message access profile */
typedef struct {
    uint8_t     rfcomm_channel; /* Must be first */
    uint16_t    version;
    uint8_t     instance_id;
    uint8_t     supported_msg_types;
    char        service_name[SDP_MAX_SERVICE_NAME];
} sdp_mse_profile_t;

/* Message Access Profile - message notification server */
typedef struct {
    uint8_t     server_channel;
    char        service_name[SDP_MAX_SERVICE_NAME];
} sdp_mns_profile_t;

#endif

