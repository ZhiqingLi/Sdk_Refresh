/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_SVC_TYPE_H_
#define _BT_SVC_TYPE_H_

#include <uw_types.h>

#define BT_SVC_TYPE_INVALID                      0
#define BT_SVC_TYPE_SERVICE_DISCOVERY_SERVER     1
#define BT_SVC_TYPE_SCO                          2
#define BT_SVC_TYPE_SERIAL_PORT                  3
#define BT_SVC_TYPE_OBEX_OBJECT_PUSH             4
#define BT_SVC_TYPE_PHONEBOOK_ACCESS_PSE         5
#define BT_SVC_TYPE_DIALUP_NETWORKING            6
#define BT_SVC_TYPE_HEADSET                      7
#define BT_SVC_TYPE_HEADSET_HS                   8
#define BT_SVC_TYPE_HEADSET_AUDIO_GATEWAY        9
#define BT_SVC_TYPE_AUDIO_SOURCE                 10
#define BT_SVC_TYPE_AUDIO_SINK                   11
#define BT_SVC_TYPE_AV_REMOTE_CONTROL            12
#define BT_SVC_TYPE_AV_REMOTE_CONTROL_CONTROLLER 13
#define BT_SVC_TYPE_AV_REMOTE_CONTROL_TARGET     14
#define BT_SVC_TYPE_HANDSFREE                    15
#define BT_SVC_TYPE_HANDSFREE_AUDIO_GATEWAY      16
#define BT_SVC_TYPE_HUMAN_INTERFACE_DEVICE       17
#define BT_SVC_TYPE_WIAP_ACCESSORY               18
#define BT_SVC_TYPE_WIAP_DEVICE                  19
#define BT_SVC_TYPE_PUBLIC_BROWSE_GROUP          20
#define BT_SVC_TYPE_BROWSE_GROUP_DESCRIPTOR      21
#define BT_SVC_TYPE_PHONEBOOK_ACCESS_PCE         22
#define BT_SVC_TYPE_MESSAGE_ACCESS_MSE           23
#define BT_SVC_TYPE_MESSAGE_ACCESS_MNS           24
#define BT_SVC_TYPE_PNP_INFO                     25
#define BT_SVC_TYPE_NAP                          26
#define BT_SVC_TYPE_GN                           27
#define BT_SVC_TYPE_PANU                         28
typedef uint16_t bt_svc_type_t;

#define BT_SVC_TYPE_RFCOMM                       40
#define BT_SVC_TYPE_L2CAP                        50

#define bt_svc_type_str(n) (\
    (n) == BT_SVC_TYPE_SERVICE_DISCOVERY_SERVER ? "SERVICE_DISCOVERY" :\
    (n) == BT_SVC_TYPE_SCO ? "SCO" :\
    (n) == BT_SVC_TYPE_SERIAL_PORT ? "SP" :\
    (n) == BT_SVC_TYPE_OBEX_OBJECT_PUSH ? "OPP" :\
    (n) == BT_SVC_TYPE_PHONEBOOK_ACCESS_PSE ? "PSE" : \
    (n) == BT_SVC_TYPE_DIALUP_NETWORKING ? "DUN" :\
    (n) == BT_SVC_TYPE_HEADSET ? "HEADSET" :\
    (n) == BT_SVC_TYPE_HEADSET_HS ? "HEADSET HS" :\
    (n) == BT_SVC_TYPE_HEADSET_AUDIO_GATEWAY ? "HS AG" :\
    (n) == BT_SVC_TYPE_AUDIO_SOURCE ? "A2DP SRC" :\
    (n) == BT_SVC_TYPE_AUDIO_SINK ? "A2DP SNK" :\
    (n) == BT_SVC_TYPE_AV_REMOTE_CONTROL ? "AVRCP" :\
    (n) == BT_SVC_TYPE_AV_REMOTE_CONTROL_CONTROLLER ? "AVRCP CONTROLLER" :\
    (n) == BT_SVC_TYPE_AV_REMOTE_CONTROL_TARGET ? "AVRCP TARGET" :\
    (n) == BT_SVC_TYPE_HANDSFREE ? "HF" :\
    (n) == BT_SVC_TYPE_HANDSFREE_AUDIO_GATEWAY ? "HF AG" :\
    (n) == BT_SVC_TYPE_PUBLIC_BROWSE_GROUP ? "PUBLIC BROWSE" :\
    (n) == BT_SVC_TYPE_BROWSE_GROUP_DESCRIPTOR ? "BROWSE GROUP" :\
    (n) == BT_SVC_TYPE_HUMAN_INTERFACE_DEVICE ? "HID" :\
    (n) == BT_SVC_TYPE_WIAP_ACCESSORY ? "WIAP ACCESSORY" :\
    (n) == BT_SVC_TYPE_WIAP_DEVICE ? "WIAP DEVICE" :\
    (n) == BT_SVC_TYPE_RFCOMM ? "RFCOMM" :\
    (n) == BT_SVC_TYPE_L2CAP ? "L2CAP" :\
    (n) == BT_SVC_TYPE_PHONEBOOK_ACCESS_PCE ? "PCE" :\
    (n) == BT_SVC_TYPE_MESSAGE_ACCESS_MSE ? "MSE" :\
    (n) == BT_SVC_TYPE_MESSAGE_ACCESS_MNS ? "MNS" :\
    (n) == BT_SVC_TYPE_PNP_INFO ? "PNP INFO" :\
    (n) == BT_SVC_TYPE_NAP ? "NAP" :\
    (n) == BT_SVC_TYPE_GN ? "GN" :\
    (n) == BT_SVC_TYPE_PANU ? "PANU" :\
    "INVALID")

#endif

