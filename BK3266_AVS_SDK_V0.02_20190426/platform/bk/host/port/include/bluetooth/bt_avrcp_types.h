/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_AVRCP_TYPES_H_
#define _BT_AVRCP_TYPES_H_

#include <uw_types.h>

/*
 * Table 4.5: AVRCP Specific operations
 * AV/C VENDOR DEPENDENDENT and Browsing commands
 */

/* Capabilities */
#define AVRCP_OP_GetCapabilities                            0x10

/* Player Application Settings */
#define AVRCP_OP_ListPlayerApplicationSettingAttributes     0x11
#define AVRCP_OP_ListPlayerApplicationSettingValues         0x12
#define AVRCP_OP_GetCurrentPlayerApplicationSettingValue    0x13
#define AVRCP_OP_SetPlayerApplicationSettingValue           0x14
#define AVRCP_OP_GetPlayerApplicationSettingAttributeText   0x15
#define AVRCP_OP_GetPlayerApplicationSettingValueText       0x16
#define AVRCP_OP_InformDisplayableCharacterSet              0x17
#define AVRCP_OP_InformBatteryStatusOfCT                    0x18

/* Metadata Attributes for Current Media Item */
#define AVRCP_OP_GetElementAttributes                       0x20

/* Notifications */
#define AVRCP_OP_GetPlayStatus                              0x30
#define AVRCP_OP_RegisterNotification                       0x31

/* Continuation */
#define AVRCP_OP_RequestContinuingResponse                  0x40
#define AVRCP_OP_AbortContinuingResponse                    0x41

/* Absolute Volume */
#define AVRCP_OP_SetAbsoluteVolume                          0x50

/* MediaPlayerSelection */
#define AVRCP_OP_SetAddressedPlayer                         0x60
#define AVRCP_OP_GetFolderItems_MediaPlayerList             0x71

/* Browsing */
#define AVRCP_OP_SetBrowsedPlayer                           0x70
#define AVRCP_OP_GetFolderItems_Filesystem                  0x71
#define AVRCP_OP_ChangePath                                 0x72
#define AVRCP_OP_GetItemAttributes                          0x73
#define AVRCP_OP_PlayItem_Filesystem                        0x74

/* Search */
#define AVRCP_OP_Search                                     0x80
#define AVRCP_OP_GetFolderItems_SearchResultList            0x71
#define AVRCP_OP_PlayItem_SearchResultList                  0x74

/* NowPlaying */
#define AVRCP_OP_GetFolderItems_NowPlayingList              0x71
#define AVRCP_OP_PlayItem_NowPlayingList                    0x74
#define AVRCP_OP_AddToNowPlaying                            0x90

/* Error Response */
#define AVRCP_OP_General_Reject                             0xA0

typedef uint8_t avrcp_op_t;

/*
 * Appendix H: List of defined notification events
 */
#define AVRCP_EVENT_PLAYBACK_STATUS_CHANGED                 0x01
#define AVRCP_EVENT_TRACK_CHANGED                           0x02
#define AVRCP_EVENT_TRACK_REACHED_END                       0x03
#define AVRCP_EVENT_TRACK_REACHED_START                     0x04
#define AVRCP_EVENT_PLAYBACK_POS_CHANGED                    0x05
#define AVRCP_EVENT_BATT_STATUS_CHANGED                     0x06
#define AVRCP_EVENT_SYSTEM_STATUS_CHANGED                   0x07
#define AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED      0x08
#define AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED             0x09
#define AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED               0x0A
#define AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED                0x0B
#define AVRCP_EVENT_UIDS_CHANGED                            0x0C
#define AVRCP_EVENT_VOLUME_CHANGED                          0x0D
/* 0x0e-0xFF  Reserved for future use */
typedef uint8_t avrcp_event_id_t;

/* Event ID is a running number, 1-based. */
#define AVRCP_MAX_EVENTS AVRCP_EVENT_VOLUME_CHANGED

#define avrcp_event_str(n) (                                \
    (n) == AVRCP_EVENT_PLAYBACK_STATUS_CHANGED ?            \
    "EVENT_PLAYBACK_STATUS_CHANGED" :                       \
    (n) == AVRCP_EVENT_TRACK_CHANGED ?                      \
    "EVENT_TRACK_CHANGED" :                                 \
    (n) == AVRCP_EVENT_TRACK_REACHED_END ?                  \
    "EVENT_TRACK_REACHED_END" :                             \
    (n) == AVRCP_EVENT_TRACK_REACHED_START ?                \
    "EVENT_TRACK_REACHED_START" :                           \
    (n) == AVRCP_EVENT_PLAYBACK_POS_CHANGED ?               \
    "EVENT_PLAYBACK_POS_CHANGED" :                          \
    (n) == AVRCP_EVENT_BATT_STATUS_CHANGED ?                \
    "EVENT_BATT_STATUS_CHANGED" :                           \
    (n) == AVRCP_EVENT_SYSTEM_STATUS_CHANGED ?              \
    "EVENT_SYSTEM_STATUS_CHANGED" :                         \
    (n) == AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED ? \
    "EVENT_PLAYER_APPLICATION_SETTING_CHANGED" :            \
    (n) == AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED ?        \
    "EVENT_NOW_PLAYING_CONTENT_CHANGED" :                   \
    (n) == AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED ?          \
    "EVENT_AVAILABLE_PLAYERS_CHANGED" :                     \
    (n) == AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED ?           \
    "EVENT_ADDRESSED_PLAYER_CHANGED" :                      \
    (n) == AVRCP_EVENT_UIDS_CHANGED ?                       \
    "EVENT_UIDS_CHANGED" :                                  \
    (n) == AVRCP_EVENT_VOLUME_CHANGED ?                     \
    "EVENT_VOLUME_CHANGED" :                                \
    "UNKNOWN EVENT")

/* Play status */
#define AVRCP_PLAY_STATUS_STOPPED                           0x00
#define AVRCP_PLAY_STATUS_PLAYING                           0x01
#define AVRCP_PLAY_STATUS_PAUSED                            0x02
#define AVRCP_PLAY_STATUS_FWD_SEEK                          0x03
#define AVRCP_PLAY_STATUS_REV_SEEK                          0x04
#define AVRCP_PLAY_STATUS_ERROR                             0x05

#define avrcp_play_status_str(n) (                          \
    (n) == AVRCP_PLAY_STATUS_STOPPED ? "STOPPED" :          \
    (n) == AVRCP_PLAY_STATUS_PLAYING ? "PLAYING" :          \
    (n) == AVRCP_PLAY_STATUS_PAUSED ? "PAUSED" :            \
    (n) == AVRCP_PLAY_STATUS_FWD_SEEK ? "FWD_SEEK" :        \
    (n) == AVRCP_PLAY_STATUS_REV_SEEK ? "REV_SEEK" :        \
    (n) == AVRCP_PLAY_STATUS_ERROR ? "ERROR" :              \
    "UNKNOWN")

/* Battery status */
#define AVRCP_BATTERY_STATUS_NORMAL                         0x00
#define AVRCP_BATTERY_STATUS_WARNING                        0x01
#define AVRCP_BATTERY_STATUS_CRITICAL                       0x02
#define AVRCP_BATTERY_STATUS_EXTERNAL                       0x03
#define AVRCP_BATTERY_STATUS_FULL_CHARGE                    0x04

/* System status */
#define AVRCP_SYSTEM_STATUS_POWER_ON                        0x00
#define AVRCP_SYSTEM_STATUS_POWER_OFF                       0x01
#define AVRCP_SYSTEM_STATUS_UNPLUGGED                       0x02

/* RegisterNotification command */
typedef struct {
    uw_u8_t     event_id;
    uw_u32_t    playback_interval;
} __PACKED_POST__ avrcp_register_notification_cmd_t;

/* RegisterNotification response */
typedef struct {
    uw_u8_t             event_id;

    union {
        /* EVENT_PLAYBACK_STATUS_CHANGED */
        struct {
            uw_u8_t     play_status;
        } __PACKED_POST__ playback_status_changed;

        /* EVENT_TRACK_CHANGED */
        struct {
            uw_u64_t    cur_track_index;    /* if no track currently selected,
                                             * return 0xf..ff in the INTERIM
                                             * response */
        } __PACKED_POST__ track_changed;

        /* EVENT_TRACK_REACHED_END - no params */
        /* EVENT_TRACK_REACHED_START - no params */

        /* EVENT_PLAYBACK_POS_CHANGED */
        struct {
            uw_u32_t    playback_pos;
        } __PACKED_POST__ playback_pos_changed;

        /* EVENT_BATT_STATUS_CHANGED */
        struct {
            uw_u8_t     batt_status;
        } __PACKED_POST__ batt_status_changed;

        /* EVENT_SYSTEM_STATUS_CHANGED */
        struct {
            uw_u8_t     system_status;
        } __PACKED_POST__ system_status_changed;

        /* EVENT_PLAYER_APPLICATION_SETTING_CHANGED */
        struct {
            uw_u8_t     attr_n;         /* num of attributes */

            /* TODO: consider implementing an array of id/val pairs */
            uw_u8_t     attr_id;        /* 1st attribute id */
            uw_u8_t     attr_val;       /* 1st attribute value */
            /* ... and so on for the number of attributes specified by attr_n */
        } __PACKED_POST__ player_application_setting_changed;

        /* EVENT_NOW_PLAYING_CONTENT_CHANGED - no params */
        /* EVENT_AVAILABLE_PLAYERS_CHANGED - no params */

        /* EVENT_ADDRESSED_PLAYER_CHANGED */
        struct {
            uw_u16_t    player_id;
            uw_u16_t    uid_counter;
        } __PACKED_POST__ addressed_players_changed;

        /* EVENT_UIDS_CHANGED */
        struct {
            uw_u16_t    uid_counter;
        } __PACKED_POST__ uids_changed;

        /* EVENT_VOLUME_CHANGED */
        struct {
            uw_u8_t     volume;
        } __PACKED_POST__ volume_changed;
    } params;
}__PACKED_POST__ avrcp_event_rsp_t;

/*
 * AV/C command
 */
#define AVC_SUBUNIT_PANEL               0x09
#define AVC_SUBUNIT_UNIT                0x1F

#define AVC_SUBUNIT_ID                  0x0
#define AVC_SUBUNIT_ID_IGNORE           0x7

#define AVC_SUBUNIT_INFO_PAGE           0x0
#define AVC_SUBUNIT_INFO_EXT_CODE       0x7
#define AVC_SUBUNIT_INFO_MAX_ID         0x0
#define AVC_SUBUNIT_PAGE_DATA_DEFAULT   0xFF

#define AVC_OPCODE_VENDOR_DEPENDENT     0x00
#define AVC_OPCODE_PASS_THROUGH         0x7C
#define AVC_OPCODE_UNIT_INFO            0x30
#define AVC_OPCODE_SUBUNIT_INFO         0x31
#ifdef CONFIG_PRODUCT_TEST_INF
#define AVC_OPCODE_BK_PRDT_TEST         0x5B
#endif
#ifdef CONFIG_TWS
#define AVC_OPCODE_BK_DATA         0x5A
#endif
#define avc_opcode_str(n) (                                                 \
    (n) == AVC_OPCODE_VENDOR_DEPENDENT ? "AVC_OPCODE_VENDOR_DEPENDENT" :    \
    (n) == AVC_OPCODE_PASS_THROUGH ? "AVC_OPCODE_PASS_THROUGH" :            \
    (n) == AVC_OPCODE_UNIT_INFO ? "AVC_OPCODE_UNIT_INFO" :                  \
    (n) == AVC_OPCODE_SUBUNIT_INFO ? "AVC_OPCODE_SUBUNIT_INFO" :            \
    "AVC OPCODE UNKNOWN")

#ifdef CONFIG_PRODUCT_TEST_INF
#define AVC_PRDT_TEST_TYPE_BUTTON    1
#define AVC_PRDT_TEST_TYPE_RSSI         2
#endif

/* AV/C specification - Section 5.3.2, Table 8 */
#define AVC_CTYPE_CONTROL               0x0
#define AVC_CTYPE_STATUS                0x1
#define AVC_CTYPE_SPECIFIC_INQUIRY      0x2
#define AVC_CTYPE_NOTIFY                0x3
#define AVC_CTYPE_GENERAL_INQUIRY       0x4
/* 0x5 - 0x7 reserved for future use */
typedef uint8_t avc_ctype_t;

/* AV/C specification - Section 5.3.3, Table 9 */
#define AVC_RESPONSE_NOT_IMPLEMENTED    0x8
#define AVC_RESPONSE_ACCEPTED           0x9
#define AVC_RESPONSE_REJECTED           0xA
#define AVC_RESPONSE_IN_TRANSITION      0xB
#define AVC_RESPONSE_STABLE             0xC
#define AVC_RESPONSE_CHANGED            0xD
#define AVC_RESPONSE_INTERIM            0xF
typedef uint8_t avc_response_t;

/* Octet 3 of SUBUNIT INFO response frame. See AVRCP spec section 22.2,
 * Figure 22.4 */
#define AVC_SUBUNIT_INFO_PAGE_SET                                       \
    ((AVC_SUBUNIT_INFO_PAGE << 4) & 0x7F) | (AVC_SUBUNIT_INFO_EXT_CODE & 0x07)

/* AV/C header response code */
#define AVC_RESPONSE_GET(b)             ((b) & 0x0F)
/* AV/C pass through operation id */
#define AVC_PT_OP_ID_GET(b)             ((b) & 0x7F)

#define avc_response_str(n) (                                             \
    (n) == AVC_RESPONSE_NOT_IMPLEMENTED ? "RESPONSE NOT IMPLEMENTED" :    \
    (n) == AVC_RESPONSE_ACCEPTED ? "RESPONSE ACCEPTED" :                  \
    (n) == AVC_RESPONSE_REJECTED ? "RESPONSE REJECTED" :                  \
    (n) == AVC_RESPONSE_IN_TRANSITION ? "RESPONSE IN TRANSITION" :        \
    (n) == AVC_RESPONSE_STABLE ? "RESPONSE STABLE" :                      \
    (n) == AVC_RESPONSE_CHANGED ? "RESPONSE CHANGED" :                    \
    (n) == AVC_RESPONSE_INTERIM ? "RESPONSE INTERIM" :                    \
    "UNKNOWN RESPONSE")

/* AV/C pass through control commands */
#define AVC_OP_VOLUME_UP                0x41
#define AVC_OP_VOLUME_DOWN              0x42
#define AVC_OP_MUTE                     0x43
#define AVC_OP_PLAY                     0x44
#define AVC_OP_STOP                     0x45
#define AVC_OP_PAUSE                    0x46
#define AVC_OP_RECORD                   0x47
#define AVC_OP_REWIND                   0x48
#define AVC_OP_FAST_FORWARD             0x49
#define AVC_OP_EJECT                    0x4A
#define AVC_OP_NEXT                     0x4B
#define AVC_OP_PREV                     0x4C
#ifdef CONFIG_TWS
#define AVC_OP_MATCH                     0x71
#endif
typedef uint16_t avc_op_t;

#define avc_op_str(op) (                                                  \
    (op) == AVC_OP_VOLUME_UP     ? "VOLUME UP"                           :\
    (op) == AVC_OP_VOLUME_DOWN   ? "VOLUME DOWN"                         :\
    (op) == AVC_OP_MUTE          ? "MUTE"                                :\
    (op) == AVC_OP_PLAY          ? "PLAY"                                :\
    (op) == AVC_OP_STOP          ? "STOP"                                :\
    (op) == AVC_OP_PAUSE         ? "PAUSE"                               :\
    (op) == AVC_OP_RECORD        ? "RECORD"                              :\
    (op) == AVC_OP_REWIND        ? "REWIND"                              :\
    (op) == AVC_OP_FAST_FORWARD  ? "FAST_FORWARD"                        :\
    (op) == AVC_OP_EJECT         ? "EJECT"                               :\
    (op) == AVC_OP_NEXT          ? "NEXT"                                :\
    (op) == AVC_OP_PREV          ? "PREV"                                :\
    "UNKNOWN")

/* AVRCP spec - section 4.2.1 */
#define BT_DEFAULT_COMPANY_ID           0xFFFFFF
/* AVRCP spec - section 4.3.1 */
#define BT_SIG_REGISTERED_COMPANY_ID    0x001958

#define SET_COMPANY_ID(target, val)     \
    target[2] = (uint8_t)(val);         \
    target[1] = (uint8_t)((val) >> 8);  \
    target[0] = (uint8_t)((val) >> 16);

/* GetCapabilities command */
#define AVRCP_CAP_COMPANY_ID            0x02
#define AVRCP_CAP_EVENTS_SUPPORTED      0x03
typedef uint8_t avrcp_capability_id_t;

/* GetCapabilities response */
typedef struct {
    uw_u8_t     id;
    uw_u8_t     cap_count;
    /* Followed by an array of company or event ids */
} __PACKED_POST__ avrcp_get_caps_rsp_t;

typedef uint8_t avrcp_company_id_t[3];

/* Appendix E: Media Attributes */
#define AVRCP_MEDIA_ATTR_TITLE                  0x01
#define AVRCP_MEDIA_ATTR_ARTIST                 0x02
#define AVRCP_MEDIA_ATTR_ALBUM                  0x03
#define AVRCP_MEDIA_ATTR_TRACK_NUMBER           0x04
#define AVRCP_MEDIA_ATTR_TOTAL_TRACK_NUMBER     0x05
#define AVRCP_MEDIA_ATTR_GENRE                  0x06
#define AVRCP_MEDIA_ATTR_PLAYING_TIME           0x07

/* Media Attribute ID is a running number, 1-based. */
#define AVRCP_MAX_MEDIA_ATTR AVRCP_MEDIA_ATTR_PLAYING_TIME

#define avrcp_media_attr_id_str(n) (\
    (n) == AVRCP_MEDIA_ATTR_TITLE ? "TITLE" :\
    (n) == AVRCP_MEDIA_ATTR_ARTIST ? "ARTIST" :\
    (n) == AVRCP_MEDIA_ATTR_ALBUM ? "ALBUM" :\
    (n) == AVRCP_MEDIA_ATTR_TRACK_NUMBER ? "TRACK_NUMBER" :\
    (n) == AVRCP_MEDIA_ATTR_TOTAL_TRACK_NUMBER ? "TOTAL_TRACK_NUMBER" :\
    (n) == AVRCP_MEDIA_ATTR_GENRE ? "GENRE" :\
    (n) == AVRCP_MEDIA_ATTR_PLAYING_TIME ? "PLAYING_TIME" :\
    "UNKNOWN")

typedef uint32_t        avrcp_media_attr_id_t;
typedef uint16_t        avrcp_character_set_id_t;

/* GetPlayStatus command - no parameters */

/* GetPlayStatus response */
typedef struct {
    uw_u32_t            song_length;    /* in milliseconds */
    uw_u32_t            song_position;  /* in milliseconds (elapsed time) */
    uw_u8_t             play_status;    /* AVRCP_PLAY_STATUS_... */
} __PACKED_POST__ avrcp_get_play_status_rsp_t;

/* Software context for external apps */
typedef void *avrcp_h;

#define AVC_PRESS   0
#define AVC_RELEASE 1
typedef uint8_t avc_click_t;

/* Transaction id */
typedef uint8_t avrcp_tid_t;

#define AVRCP_MAX_TRANSACTIONS 16

/* Continuation packet type */
#define AVRCP_PKT_SINGLE    0x00
#define AVRCP_PKT_START     0x01
#define AVRCP_PKT_CONT      0x02
#define AVRCP_PKT_END       0x03

typedef uint8_t avrcp_pkt_type_t;

#endif

