#ifndef __HIDCON_H__
#define  __HIDCON_H__


typedef struct _HidConn HidConn;
typedef struct _HidConnCallbackParms HidConnCallbackParms;


typedef void (*HidConnCallback)(HidConn *Conn, HidConnCallbackParms *Parms);



struct _HidConnCallbackParms {
    U8         event;
    BtStatus   status;
    U16        dataLen;
    union {
        BtRemoteDevice *remDev;
        U8             *data;
        BtPacket       *packet;
    } ptrs;
};


/* Connection State */ 
struct _HidConn {
    L2capChannelId    l2ChannelId;
    U8                state;
    HidConnCallback callback;
    

};


void HidCL2Callback(L2capChannelId L2ChannelId, L2capCallbackParms *Parms);
void HidiL2Callback(L2capChannelId L2ChannelId, L2capCallbackParms *Parms);
BOOL HidIsConnected(HidConn *Conn);
BtStatus HidConnect( BtRemoteDevice *RemDev);
BtStatus HidDisconnect(BtRemoteDevice *RemDev);

#endif
