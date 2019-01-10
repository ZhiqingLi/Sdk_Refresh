#ifndef __HID_H__
#define __HID_H__
#include "bttypes.h"
#include "conmgr.h"
#include "sys/hidcon.h"

//typedef struct _HidChannel HidChannel;
typedef struct _HidCallbackParms HidCallbackParms;
typedef struct _HidChannel   HidChannel;
typedef struct _HidFrame   HidFrame;


typedef U16 HidVersion;
#define HID_VERSION_100   0x100
#define HID_VERSION_111   0x101



typedef U16 HidEvent;



struct _HidCallbackParms{
    HidEvent    event;    /* Event associated with the callback       */

    BtStatus    status;  /* Status of the callback event             */
    BtErrorCode errCode; /* Error code (reason) on disconnect events */
    HidChannel *chnl;
    
    union {
        BtRemoteDevice *remDev;
        U8 idle_rate;

    } p;
};


struct _HidFrame {
    ListEntry   node;           /* Used internally by HID. */
    L2capChannelId channel_id;
    U8  trans_type;          /* 4 bits */
    U8          header;      
    U8          dataLen;      /* data length */
    U8          data[10];      /* data information */
    EvmTimer    timer;          /* Timer for the command */
};


/*--------------------------------------------------------------------------
 * HIDChannelStates type
 *
 *     This type enumerates the possible HID channel connection
 *     states.
 */
typedef U8 HidChannelStates;

/** The channel is currently closed.
 */
#define HID_STATE_CLOSED        0

/** An outgoing ACL data link is currently being setup.
 */
#define HID_STATE_CONN_PENDING  1

/** An incoming ACL data link is currently being setup.
 */
#define HID_STATE_CONN_INCOMING 2

/** An ACL data link has been established, and the Hands-Free Service Level
 *  Link parameters are currently being negotiated.
 */
#define HID_STATE_NEGOTIATE     3

/** A Hands-Free channel is currently open.
 */
#define HID_STATE_OPEN          4

/** Closing the link but waiting for the audio link to go down first.
 */
#define HID_STATE_DISC          5

/* End of HfChannelStates */





typedef void (*HidCallback)(HidChannel *Chan, HidCallbackParms *Info);


struct _HidChannel{

    ListEntry           node;
    HidCallback          callback;         /* Application callback*/
    CmgrHandler         cmgrHandler;
    HidConn          hidi_conn;
    HidConn          hidc_conn;
    BtRemoteDevice   *remDev;
    HidChannelStates     state;           /* Current connection state      */
    U16                 flags;           /* Current connection flags      */
    BtPacket       packet;
    ListEntry       txQueue;
    U8                 txState;
    void           *cur_ctrlframe;
    void           *cur_interframe;
    U8     capture_process;
    U8    initiator;
    /* SDP variables for client */
    SdpQueryToken       sdpQueryToken;   /* Used to query the service     */
    U8                  queryFlags;      /* Defines which SDP entries were
                                          * parsed from the gateway.
                                          */
    HidVersion    version;         /* Profile version*/
    HidFrame ctrl_frame;
    HidFrame capture_frame[4];
    U8 hid_from_phone;
};


///HID CHAN FLAGS
#define HID_CHAN_FLAG_OUTGOING        		0x0001







/* SDP Query States */ 
#define HID_SDP_QUERY_FLAG_PROTOCOL   0x01
#define HID_SDP_QUERY_FLAG_PROFILE    0x02
#define HID_SDP_QUERY_FLAG_ADDPROTOCOL    0x04


#ifndef HIDC_MAX_MTU
#define HIDC_MAX_MTU L2CAP_MTU
#endif


#ifndef HIDI_MAX_MTU
#define HIDI_MAX_MTU L2CAP_MTU
#endif


////hid channel state
#define HID_STATE_DISCONNECTED   0
#define HID_STATE_CONN_PENDING   1
#define HID_STATE_CONN_INCOMING  2
#define HID_STATE_DISC_PENDING   3
#define HID_STATE_DISC_INCOMING  4
#define HID_STATE_CONNECTED      5

BtStatus Hid_Register(HidChannel *chnl, 
                        HidCallback    callback
                        );
void HidCtrDataProc(HidChannel *chnl,uint8_t *data,uint16_t len);
BtStatus HidSendNextFrame(HidChannel *chnl);

void Hid_Send_capture(HidChannel *chnl);
BtStatus HidStartServiceQuery(HidChannel *Chan, SdpQueryMode mode);
void HidReportFailedConnect(HidChannel   *chnl, 
                              BtRemoteDevice *remDev,
                              BtStatus        status);
BtStatus Hid_Connect(HidChannel *chnl,BtRemoteDevice *RemDev);
BtStatus Hid_Disconnect(HidChannel *chnl);



#define HID_REPORT_TYPE_INPUT     1
#define HID_REPORT_TYPE_OUTPUT     2
#define HID_REPORT_TYPE_FEATURE     3



#define HID_TRANS_TYPE_HANDSHAKE    0
#define HID_TRANS_TYPE_CONTROL        1
#define HID_TRANS_TYPE_GET_REPORT        4
#define HID_TRANS_TYPE_SET_REPORT        5
#define HID_TRANS_TYPE_GET_PROTOCAL        6
#define HID_TRANS_TYPE_SET_PROTOCAL        7
#define HID_TRANS_TYPE_GET_IDLE        8
#define HID_TRANS_TYPE_SET_IDLE        9
#define HID_TRANS_TYPE_DATA        10
#define HID_TRANS_TYPE_DATAC        11


#define HID_HANDSHAKE_SUCCESS    0
#define HID_HANDSHAKE_NOT_READY    1
#define HID_HANDSHAKE_INVALID_REPORTID    2
#define HID_HANDSHAKE_UNSUPPORT_REQUEST    3
#define HID_HANDSHAKE_INVALID_PARAM   4
#define HID_HANDSHAKE_ERROR_UNKNOWN    0xe
#define HID_HANDSHAKE_ERROR_FATAL   0xF


#define HID_CONTROL_NOP        0
#define HID_CONTROL_HARD_RESET   1
#define HID_CONTROL_SOFT_RESET   2
#define HID_CONTROL_SUSPEND   3
#define HID_CONTROL_EXIT_SUSPEND   4
#define HID_CONTROL_VIRTUAL_CABLE_UNPLUG   5

#define HID_CHANNEL_TYPE_INTERRUPT     0
#define HID_CHANNEL_TYPE_CONTROL     1



#define HID_DATA_TXSTATE_IDLE            0
#define HID_DATA_TXSTATE_SEND            1




#define HID_EVENT_INTERRUPT_CONNECTED               1

#define HID_EVENT_INTERRUPT_DISCONNECTED         2

#define HID_EVENT_INTERRUPT_DATA_IND                3

#define HID_EVENT_INTERRUPT_DATA_SENT              4




#define HID_EVENT_CONTROL_CONNECTED               0x21

#define HID_EVENT_CONTROL_DISCONNECTED         0x22

#define HID_EVENT_CONTROL_DATA_IND                0x23

#define HID_EVENT_CONTROL_DATA_SENT              0x24


#define HID_EVENT_CONTROL_SET_IDLE              0x30


#endif
