#ifndef _APP_DEBUG_H_
#define _APP_DEBUG_H_

#define BEKEN_OCF                   0XE0
#define TX_FIFO_THRD                0x40
#define RX_FIFO_THRD                0x40
#define HCI_EVENT_HEAD_LENGTH       0X03
#define HCI_COMMAND_HEAD_LENGTH     0X04

typedef struct {
    unsigned char code;             /**< 0x01: HCI Command Packet
                                         0x02: HCI ACL Data Packet
                                         0x03: HCI Synchronous Data Packet
                                         0x04: HCI Event Packet */
    struct {
        unsigned short ocf : 10;    /**< OpCode Command Field */
        unsigned short ogf : 6;     /**< OpCode Group Field */
    } __PACKED_POST__ opcode;
    unsigned char total;
    unsigned char param[];
} __PACKED_POST__ HCI_PACKET;

typedef struct {
    unsigned char code;             /**< 0x01: HCI Command Packet
                                         0x02: HCI ACL Data Packet
                                         0x03: HCI Synchronous Data Packet
                                         0x04: HCI Event Packet */
    struct {
        unsigned short ocf : 10;    /**< OpCode Command Field */
        unsigned short ogf : 6;     /**< OpCode Group Field */
    } __PACKED_POST__ opcode;
    unsigned char total;
    unsigned char cmd;              /**< private command */
    unsigned char param[];
} __PACKED_POST__ HCI_COMMAND_PACKET;

typedef struct {
    unsigned char code;             /**< 0x01: HCI Command Packet
                                         0x02: HCI ACL Data Packet
                                         0x03: HCI Synchronous Data Packet
                                         0x04: HCI Event Packet */
    unsigned char event;            /**< 0x00-0xFF: Each event is assigned a 1-Octet event code used to uniquely identify different types of events*/
    unsigned char total;            /**< Parameter Total Length */
    unsigned char param[];
} __PACKED_POST__ HCI_EVENT_PACKET;

typedef struct {
    unsigned int addr;
    unsigned int value;
} __PACKED_POST__ REGISTER_PARAM;

typedef struct {
    unsigned int  addr;
    unsigned char len;
    unsigned char data[];
} __PACKED_POST__ FLASH_PARAM;

//return status
#define APP_RET_STATUS_OK           "OK\r\n"                               // cmd success
#define APP_RET_STATUS_FAIL         "FAIL\r\n"                             // cmd can not execute 
#define APP_RET_STATUS_ERR          "ERR\r\n"                              // cmd execute error
#define APP_RET_STATUS_UNKNOWN      "UNKNOWN\r\n"                          // cmd unknown
#define APP_RET_STATUS_INIT         "IS\r\n"                               // initialization
#define APP_RET_STATUS_UNPAIR       "I1\r\n"                               // can't be paired
#define APP_RET_STATUS_PAIR         "IA\r\n"                               // can be paired
#define APP_RET_STATUS_CONNECTING   "IV(%s)\r\n"                           // connecting(profile)
#define APP_RET_STATUS_CONNECTION   "IB(%s)\r\n"                           // connected(profile)
#define APP_RET_STATUS_INCOMING     "IC\r\n"                               // incoming call
#define APP_RET_STATUS_OUTGOING     "ID\r\n"                               // outgoing call
#define APP_RET_STATUS_ICALLNUMBER  "IR-%s\r\n"                            // call number of incoming call
#define APP_RET_STATUS_OCALLNUMBER  "PR-%s\r\n"                            // call number of outgoing call
#define APP_RET_STATUS_CALL_EST     "IG\r\n"                               // call established
#define APP_RET_STATUS_MUSIC_PAUSE  "MA\r\n"                               // music pause
#define APP_RET_STATUS_MUSIC_PLAY   "MB\r\n"                               // music play
#define APP_RET_STATUS_DEVICE_INFO  "WZ-%02x:%02x:%02x:%02x:%02x:%02x\r\n" // show remote device btaddr

#endif
