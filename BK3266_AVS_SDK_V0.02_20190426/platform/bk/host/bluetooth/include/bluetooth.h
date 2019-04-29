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

#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include <include/uw_types.h>
#include <include/uw_compiler.h>
#include <bt_svc_type.h>

/* The Class of Device/Service (CoD) field - 3 octets
 *
 * 23        16 15        8 7         1     bit transmission order
 * |  octet 3  |  octet 2  |  octet 1  |    octet transmission order
 */
#define BT_CLASS_SERVICE_SHIFT  13
#define BT_CLASS_MAJOR_SHIFT    8
#define BT_CLASS_MINOR_SHIFT    2

#define BT_CLASS_SERVICE_MASK   0x7ff   /* 11 bits */
#define BT_CLASS_MAJOR_MASK     0x1f    /* 5 bits */
#define BT_CLASS_MINOR_MASK     0x3f    /* 6 bits */

/* Bluetooth Device Class */
#define BT_CLASS_SIZE 3 /* Size, in bytes */
typedef uint8_t btclass_t[BT_CLASS_SIZE];

static __INLINE__ uint32_t bt_class_get(const btclass_t uclass)
{
    return uclass[0] | ((uint32_t)uclass[1] << 8) |
        ((uint32_t)uclass[2] << 16);
}

static __INLINE__ void bt_class_set(btclass_t uclass, uint32_t btclass)
{
    uclass[0] = (uint8_t)(btclass);
    uclass[1] = (uint8_t)((btclass) >> 8);
    uclass[2] = (uint8_t)((btclass) >> 16);
}

static __INLINE__ void bt_class_set3(btclass_t uclass, uint16_t service,
    uint8_t major, uint8_t minor)
{
    uint32_t btclass = ((uint32_t)service) << BT_CLASS_SERVICE_SHIFT |
        ((uint32_t)major) << BT_CLASS_MAJOR_SHIFT |
        ((uint32_t)minor) << BT_CLASS_MINOR_SHIFT;

    bt_class_set(uclass, btclass);
}

static __INLINE__ uint16_t bt_class_get_service(const btclass_t uclass)
{
    /* Service Class, bits [23-13] */
    uint16_t service = (uint16_t)(((uint16_t)uclass[2] << (16 - BT_CLASS_SERVICE_SHIFT)) 
                        | (uclass[1] >> (BT_CLASS_SERVICE_SHIFT - 8)));

    return (service & BT_CLASS_SERVICE_MASK);
}

static __INLINE__ uint8_t bt_class_get_major(const btclass_t uclass)
{
    /* Major Device Class, bits [12-8] */
    return uclass[1] & BT_CLASS_MAJOR_MASK;
}

static __INLINE__ uint8_t bt_class_get_minor(const btclass_t uclass)
{
    /* Minor Device Class, bits [7-2] */
    return (uclass[0] >> BT_CLASS_MINOR_SHIFT) & BT_CLASS_MINOR_MASK;
}

static __INLINE__ bool_t bt_class_same(const btclass_t a, const btclass_t b)
{
    return (a[0] == b[0] && a[1] == b[1] && a[2] == b[2]);
}

const char *bt_class2str(const btclass_t uclass, char *buf, uint16_t len);


/*
 * Baseband Assigned Numbers
 *
 * https://www.bluetooth.org/Technical/AssignedNumbers/baseband.htm
 */

/* Major Service Classes, bits [13-23] (Table 2) */
#define BT_SERVICE_LIMITED_DISCOVERABLE 0x0001
#define BT_SERVICE_POSITIONING          0x0008
#define BT_SERVICE_NETWORKING           0x0010
#define BT_SERVICE_RENDERING            0x0020
#define BT_SERVICE_CAPTURING            0x0040
#define BT_SERVICE_OBJECT_TRANSFER      0x0080
#define BT_SERVICE_AUDIO                0x0100
#define BT_SERVICE_TELEPHONY            0x0200
#define BT_SERVICE_INFORMATION          0x0400

/* Major Device Class, bits [12-8] (Table 3) */
#define BT_DEVCLASS_MISCELLANEOUS  0
#define BT_DEVCLASS_COMPUTER       1    /* desktop, notebook, PDA, organizers */
#define BT_DEVCLASS_PHONE          2    /* cellular, cordless, modem, ... */
#define BT_DEVCLASS_LAN            3    /* LAN/Network Access point */
#define BT_DEVCLASS_AUDIO_VIDEO    4    /* headset, speaker, stereo,
                                           video display, vcr ... */
#define BT_DEVCLASS_PERIPHERAL     5    /* mouse, joystick, keyboard, ... */
#define BT_DEVCLASS_IMAGING        6    /* printer, scanner, camera, display */
#define BT_DEVCLASS_WEARABLE       7
#define BT_DEVCLASS_TOY            8
#define BT_DEVCLASS_HEALTH         9
#define BT_DEVCLASS_UNCATEGORIZED  0x1f

/* Minor Device Class, bits [7-2] */
#define BT_DEVSUBCLASS_UNCATEGORIZED    0

/* Minor Device Class field - Computer Major Class (Table 4) */
#define BT_DEVSUBCLASS_DESKTOP          1
#define BT_DEVSUBCLASS_SERVER           2
#define BT_DEVSUBCLASS_LAPTOP           3
#define BT_DEVSUBCLASS_HANDHELD         4
#define BT_DEVSUBCLASS_PALM_SIZED       5
#define BT_DEVSUBCLASS_WEARABLE_COMP    6

/* Minor Device Class field - Phone Major Class (Table 5) */
#define BT_DEVSUBCLASS_CELLULAR         1
#define BT_DEVSUBCLASS_CORDLESS         2
#define BT_DEVSUBCLASS_SMART_PHONE      3
#define BT_DEVSUBCLASS_WIRED_MODEM      4
#define BT_DEVSUBCLASS_ISDN             5

/* Minor Device Class field - LAN/Network Major Class (Table 6, 7) */
#define BT_DEVSUBCLASS_LAN_17           (1<<3)
#define BT_DEVSUBCLASS_LAN_33           (2<<3)
#define BT_DEVSUBCLASS_LAN_50           (3<<3)
#define BT_DEVSUBCLASS_LAN_67           (4<<3)
#define BT_DEVSUBCLASS_LAN_83           (5<<3)
#define BT_DEVSUBCLASS_LAN_99           (6<<3)

#define BT_DEVSUBCLASS_LAN_MASK         (0x7 << 3)

/* Minor Device Class field - Audio/Video Major Class (Table 8) */
#define BT_DEVSUBCLASS_WEARABLE_HEADSET 1
#define BT_DEVSUBCLASS_HANDSFREE        2
#define BT_DEVSUBCLASS_MICROPHONE       4
#define BT_DEVSUBCLASS_LOUDSPEAKER      5
#define BT_DEVSUBCLASS_HEADPHONES       6
#define BT_DEVSUBCLASS_PORTABLE_AUDIO   7
#define BT_DEVSUBCLASS_CAR_AUDIO        8
#define BT_DEVSUBCLASS_SETTOP_BOX       9
#define BT_DEVSUBCLASS_HIFI_AUDIO       10
#define BT_DEVSUBCLASS_VCR              11
#define BT_DEVSUBCLASS_VIDEO_CAMERA     12
#define BT_DEVSUBCLASS_CAMCORDER        13
#define BT_DEVSUBCLASS_VIDEO_MONITOR    14
#define BT_DEVSUBCLASS_VIDEO_DISPLAY    15
#define BT_DEVSUBCLASS_VIDEO_CONFERENCE 16
#define BT_DEVSUBCLASS_AV_GAMING        18

/* Minor Device Class field - Peripheral Major Class (Table 9,10) */
#define BT_DEVSUBCLASS_JOYSTICK    1
#define BT_DEVSUBCLASS_GAMEPAD     2
#define BT_DEVSUBCLASS_REMOTE      3
#define BT_DEVSUBCLASS_SENSING     4
#define BT_DEVSUBCLASS_DIGITIZER   5
#define BT_DEVSUBCLASS_CARD_READER 6

#define BT_DEVSUBCLASS_KEYBOARD 0x10
#define BT_DEVSUBCLASS_MOUSE    0x20

/* Minor Device Class field - Imaging Major Class (Table 11,12) */
#define BT_DEVSUBCLASS_DISPLAY          0x04
#define BT_DEVSUBCLASS_CAMERA           0x08
#define BT_DEVSUBCLASS_SCANNER          0x10
#define BT_DEVSUBCLASS_PRINTER          0x20

/* Minor Device Class field - Wearable Major Class (Table 13) */
#define BT_DEVSUBCLASS_WRIST_WATCH      1
#define BT_DEVSUBCLASS_PAGER            2
#define BT_DEVSUBCLASS_JACKET           3
#define BT_DEVSUBCLASS_HELMET           4
#define BT_DEVSUBCLASS_GLASSES          5

/* Minor Device Class field - Toy Major Class (Table 14) */
#define BT_DEVSUBCLASS_ROBOT            1
#define BT_DEVSUBCLASS_VEHICLE          2
#define BT_DEVSUBCLASS_DOLL             3
#define BT_DEVSUBCLASS_TOY_CONTROLLER   4
#define BT_DEVSUBCLASS_GAME             5
#define BT_DEVSUBCLASS_

/* Minor Device Class field - Health Major Class (Table 15) */
#define BT_DEVSUBCLASS_BLOOD_PRESSURE           1
#define BT_DEVSUBCLASS_THERMOMETER              2
#define BT_DEVSUBCLASS_WEIGHING_SCALE           3
#define BT_DEVSUBCLASS_GLUCOSE_METER            4
#define BT_DEVSUBCLASS_PULSE_OXIMETER           5
#define BT_DEVSUBCLASS_HEART_RATE               6
#define BT_DEVSUBCLASS_HEALTH_DATA_DISPLAY      7

/*
 * Bluetooth Address Family Protocol Numbers
 */
#define BTPROTO_HCI	1
#define BTPROTO_L2CAP	2
#define BTPROTO_RFCOMM	3
#define BTPROTO_SCO	4
#define BTPROTO_AVDTP	5
#define BTPROTO_AVCTP	6

/* All sizes are in bytes */
#define BLUETOOTH_BTADDR_SIZE	6

/*
 * Bluetooth Device Address
 */
typedef   struct {
    uint8_t	b[BLUETOOTH_BTADDR_SIZE];
} __PACKED_POST__ btaddr_t;

/*
 * btaddr utility functions
 */
static __INLINE__ bool_t btaddr_same(const btaddr_t *a, const btaddr_t *b)
{
    return (a->b[0] == b->b[0] && a->b[1] == b->b[1]
        && a->b[2] == b->b[2] && a->b[3] == b->b[3]
        && a->b[4] == b->b[4] && a->b[5] == b->b[5]);
}

static __INLINE__ bool_t btaddr_any(const btaddr_t *a)
{
    return (a->b[0] == 0 && a->b[1] == 0 && a->b[2] == 0
        && a->b[3] == 0 && a->b[4] == 0 && a->b[5] == 0);
}

static __INLINE__ void btaddr_copy(btaddr_t *d, const btaddr_t *s)
{
    d->b[0] = s->b[0];
    d->b[1] = s->b[1];
    d->b[2] = s->b[2];
    d->b[3] = s->b[3];
    d->b[4] = s->b[4];
    d->b[5] = s->b[5];
}

/*
 * Socket address used by Bluetooth protocols
 */
typedef struct bt_sockaddr {
    uint8_t     bt_len;
    btaddr_t    bt_btaddr;
    uint16_t    bt_psm;
    uint8_t     bt_channel;
    uint8_t     bt_zero[6];
} bt_sockaddr_t;

typedef struct {
    bt_sockaddr_t laddr;
    bt_sockaddr_t raddr;
} bt_sockaddr_pair_t;

#define BTADDR(a) \
    (a)->b[5], (a)->b[4], (a)->b[3], (a)->b[2], (a)->b[1], (a)->b[0]

#define BTADDR_FORMAT   "%02x:%02x:%02x:%02x:%02x:%02x"
#define BTADDR_LEN      6

#define btaddr_dump(a,b,c,d)            do {} while(0)
#define sockaddr_bt_dump(a,b,c,d)       do {} while(0)
#define DBG_CON(a,b)                    do {} while(0)

/* Note: this is actually 6 bytes including terminator */
#define BTADDR_ANY	((const btaddr_t *) "\000\000\000\000\000")

/* Bluetooth link mode flags */
#define BT_LINK_MODE_NONE      (0)     /* No link mode (not secure) */
#define BT_LINK_MODE_AUTH      (1<<0)  /* Require authentication */
#define BT_LINK_MODE_ENCRYPT   (1<<1)  /* Require encryption */
#define BT_LINK_MODE_SECURE    (1<<2)  /* Require secured link */

/* Bluetooth link modes mask */
typedef uint8_t bt_link_modes_mask_t;

/* Security levels, these correspond to those shown in Core 2.1, Part C,
 * 5.2.2.8 */

#define BT_SECURITY_SDP_ONLY    0
#define BT_SECURITY_LOW         1
#define BT_SECURITY_MEDIUM      2
#define BT_SECURITY_HIGH        3

#define BT_BONDING_NO_BONDING   0
#define BT_BONDING_DEDICATED    1
#define BT_BONDING_GENERAL      2

/*
 * Bluetooth Protocol API callback methods
 */
struct mbuf;

typedef void (*btproto_connecting_t)(void *);
typedef void (*btproto_connected_t)(void *);
typedef void (*btproto_disconnected_t)(void *, result_t);
typedef void (*btproto_newconn_t)(void *, bt_sockaddr_t *, bt_sockaddr_t *,
    bt_link_modes_mask_t, void *);
typedef void (*btproto_complete_t)(void *, int32_t, void *);
typedef void (*btproto_linkmode_t)(void *, bt_link_modes_mask_t);
typedef void (*btproto_input_t)(void *, struct mbuf *);
typedef void (*btproto_command_t)(void *, uint8_t, void *);

typedef struct btproto {
    btproto_connecting_t        connecting;
    btproto_connected_t         connected;
    btproto_disconnected_t      disconnected;
    btproto_newconn_t           newconn;
    btproto_complete_t          complete;
    btproto_linkmode_t          linkmode;
    btproto_input_t             input;
    btproto_command_t           command;
} btproto_t;

#define BTPROTO_INITIALIZER(connecting, connected, disconnected, newconn,   \
    complete, linkmode, input, command)                                     \
{                                                                           \
    connecting,                                                             \
    connected,                                                              \
    disconnected,                                                           \
    newconn,                                                                \
    complete,                                                               \
    linkmode,                                                               \
    input,                                                                  \
    command                                                                 \
}

#define UNKNOWN(debug_type,fname,value) \
    DBG_E(debug_type, ("%s: %s = %d unkown!\n", fname, #value, (value)));

#define BT_PROFILE_VALIDATION(cond_param, cond_already, cond_state, result) \
do {                            \
    (result) = UWE_OK;          \
    if((cond_param))           \
        (result) = UWE_PARAM;   \
    else if((cond_already))    \
        (result) = UWE_ALREADY; \
    else if((cond_state))      \
        (result) = UWE_STATE;   \
} while(0)

#endif
