/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_SDP_TYPES_H_
#define _BT_SDP_TYPES_H_

/* A2DP Source features */
#define SDP_A2DP_PLAYER         (1 << 0)
#define SDP_A2DP_MICROPHONE     (1 << 1)
#define SDP_A2DP_TUNER          (1 << 2)
#define SDP_A2DP_MIXER          (1 << 3)

/* A2DP Sink features */
#define SDP_A2DP_HEADPHONE      (1 << 0)
#define SDP_A2DP_SPEAKER        (1 << 1)
#define SDP_A2DP_RECORDER       (1 << 2)
#define SDP_A2DP_AMPLIFIER      (1 << 3)

/* AVRCP Control/Target features - bits 4,5 and 7 are Target only */
#define SDP_AVRCP_PLAYER_RECORDER               (1 << 0) /* Category 1 */
#define SDP_AVRCP_MONITOR_AMPLIFIER             (1 << 1) /* Category 2 */
#define SDP_AVRCP_TUNER                         (1 << 2) /* Category 3 */
#define SDP_AVRCP_MENU                          (1 << 3) /* Category 4 */
#define SDP_AVRCP_PLAYER_APP_SETTINGS           (1 << 4) /* Category 1 must be
                                                          * set */
#define SDP_AVRCP_GROUP_NAVIGATION              (1 << 5) /* Category 1 must be
                                                          * set */
#define SDP_AVRCP_SUPPORTS_BROWSING             (1 << 6)
#define SDP_AVRCP_SUPPORT_MULT_MEDIA_PLAYER_APP (1 << 7)

/* OPUSH Supported formats */
#define SDP_OPUSH_VCARD_2_1         (0x01) /* vCard 2.1           */
#define SDP_OPUSH_VCARD_3_0         (0x02) /* vCard 3.0           */
#define SDP_OPUSH_VCAL_1_0          (0x03) /* vCal 1.0            */
#define SDP_OPUSH_ICAL_2_0          (0x04) /* iCal 2.0            */
#define SDP_OPUSH_VNOTE             (0x05) /* vNote               */
#define SDP_OPUSH_VMESSAGE          (0x06) /* vMessage            */
#define SDP_OPUSH_ANY               (0xFF) /* Any type of object  */

/* MAP Supported message types */
#define SDP_MAP_EMAIL               (1 << 0)
#define SDP_MAP_SMS_GSM             (1 << 1)
#define SDP_MAP_SMS_CDMA            (1 << 2)
#define SDP_MAP_MMS                 (1 << 3)

#endif /* _BT_SDP_TYPES_H_ */

