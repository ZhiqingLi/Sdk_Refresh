#ifndef __HS_H__
#define __HS_H__

#include "hf.h"
#include "sdp.h"
#include "at_hf.h"

#include "sys/hfi.h"
#include "sys/hf_cmd.h"
#include "sys/hfalloc.h"
#include "conmgr.h"
#include "sys/hf_cmgr.h"

/* Link Flags */ 
#define HS_LINK_ACL                  0x01
#define HS_LINK_HEADSET           0x08
#define HS_LINK_REMOVE_HS            0x10

/* Internal State Machine Events */ 
#define EVENT_HS_OPEN_IND               0x80
#define EVENT_HS_OPEN                   0x81
#define EVENT_HS_AT_DATA                0x82
#define EVENT_HS_CLOSED                 0x83
#define EVENT_HS_COMMAND_TIMEOUT        0x84
#define EVENT_HS_RF_PACKET_SENT         0x85
#define EVENT_HS_COMMAND_COMPLETE       0x89
#define EVENT_HS_POLL_COMPLETE          0x9A
#define EVENT_HS_RING_TIMEOUT           0x9B

/* SDP Query States */ 
#define HS_SDP_QUERY_FLAG_PROTOCOL   0x01
#define HS_SDP_QUERY_FLAG_PROFILE    0x02
#define HS_SDP_QUERY_FLAG_HS         0x10


/*---------------------------------------------------------------------------
 * HS_SNIFF_XX constant
 *     HS_SNIFF_XX defines the ?  interval for sniff mode if
 *     enabled.  (See HS_SNIFF_TIMER.)
 */
#define HS_SNIFF_ATTEMPT	0x0160
#define HS_SNIFF_TIMEOUT	0x0160
#define HS_SNIFF_MAX_INTERVAL	0x0800
#define HS_SNIFF_MIN_INTERVAL	0x0040
#ifndef HS_SNIFF_TIMER
#define HS_SNIFF_TIMER			10000
#endif
#ifndef HS_SECURITY_SETTINGS
#define HS_SECURITY_SETTINGS	(BSL_SECURITY_L2_IN | BSL_SECURITY_L2_OUT )
#endif


#ifndef HS_RECV_BUFFER_SIZE
#define HS_RECV_BUFFER_SIZE		1024
#endif 
#ifndef HS_TX_BUFFER_SIZE
#define HS_TX_BUFFER_SIZE		1024

#endif


/*--------------------------------------------------------------------------
 * HsChannelStates type
 *
 *     This type enumerates the possible Hands-Free channel connection
 *     states.
 */
 typedef U8 HsChannelStates;
/** The channel is currently closed.
 */
#define HS_STATE_CLOSED        0

/** An outgoing ACL data link is currently being setup.
 */
#define HS_STATE_CONN_PENDING  1

/** An incoming ACL data link is currently being setup.
 */
#define HS_STATE_CONN_INCOMING 2

/** An ACL data link has been established, and the Hands-Free Service Level
 *  Link parameters are currently being negotiated.
 */
#define HS_STATE_NEGOTIATE     3

/** A Hands-Free channel is currently open.
 */
#define HS_STATE_OPEN          4

/** Closing the link but waiting for the audio link to go down first.
 */
#define HS_STATE_DISC          5
/* End of HsChannelStates */


/*---------------------------------------------------------------------------
 * HsEvent type
 *
 *     All indications and confirmations are sent through a callback
 *     function.  Depending on the event, different elements of
 *     the HsCallbackParms "HsCallbackParms.p" union will be valid.
 */
typedef U16 HsEvent;
#define HS_EVENT_SERVICE_CONNECT_REQ   0
#define HS_EVENT_SERVICE_CONNECTED      1
#define HS_EVENT_SERVICE_DISCONNECTED   2
#define HS_EVENT_AUDIO_CONNECTED        3
#define HS_EVENT_AUDIO_DISCONNECTED     4
#define HS_EVENT_AUDIO_DATA             5
#define HS_EVENT_AUDIO_DATA_SENT        6
#define HS_EVENT_GATEWAY_FEATURES       7
#define HS_EVENT_RING_IND              16
#define HS_EVENT_SPEAKER_VOLUME        28
#define HS_EVENT_MIC_VOLUME            29
#define HS_EVENT_IN_BAND_RING          30
#define HS_EVENT_COMMAND_COMPLETE      43
#define HS_EVENT_AT_RESULT_DATA        42
#define HS_EVENT_SIRI_STATUS            44
/* End of HsEvent */

 
/*---------------------------------------------------------------------------
 * HsCmdCompType type
 *
 *     This type indicates the condition that caused a command to be 
 *     completed.  
 */

typedef U8 HsCmdCompType;
#define HS_CMD_COMP_OK       0
#define HS_CMD_COMP_ERROR    1
#define HS_CMD_COMP_EXTENDED 2
#define HS_CMD_COMP_TIMEOUT  3
/* End of HsCmdCompType */ 


/* Channel Flags */
#define HS_CHAN_FLAG_TX_IN_PROGRESS  		0x0080
#define HS_CHAN_FLAG_OUTGOING				0x0001
#define HS_CHAN_FLAG_IGNORE_CLOSE    		0x0800
#define HS_CHAN_FLAG_SNIFFING        		0x0400


/*--------------------------------------------------------------------------
 * HsGatewayVersion type
 *
 *     HsGatewayVersion corresponds to the Audio Gateway Profile version
 *     discovered during the SDP query.  The service connection features
 *     will be limited to the capabilities of this profile version.
 */

typedef U16  HsGatewayVersion;
/* End of HsGatewayVersion */

typedef struct _HsAtData {
    U8         *data;
    U16         dataLen;
} HsAtData;

typedef union {
    const char *asConstCharP;
    void       *asVoidP;
} HsUPtr;

typedef AtError HsCmeError;

typedef U8 HsCallActiveState;
typedef U8 HsAudioConnectState;
typedef U32 HsGatewayFeatures;

typedef CmgrAudioData HsAudioData;
typedef struct _HsCallbackParms HsCallbackParms;
typedef struct _HsChannel HsChannel;
//typedef struct _HsCommand HsCommand;
#define HsCommand	HfCommand


typedef void (*HsCallback)(HsChannel *Chan, HsCallbackParms *Info);


/*---------------------------------------------------------------------------
 * HsCmdOverride type
 *
 * A function of this type is called when a function override is
 * registered.
 */
typedef BtStatus (*HsCmdOverride)(HsChannel *Chan, HsCommand *Command);

/* End of HsCmdOverride */


/*---------------------------------------------------------------------------
 * HsChannel structure
 *
 *     Structures of type HsChannel can be used to store information about a
 *     Head-Set channel.
 */
struct _HsChannel {

    /* === Internal use only === */

    /* Registration variables */
    ListEntry           node;
    HsCallback          callback;         /* Application callback
                                           * function */
    RfChannel           rfChannel;        /* RFCOMM channel used for
                                           * head-setclient or server
                                           * connection.
                                           */

    /* Transmit Queue */
    BOOL                processingCmdQ;   /* Used to prevent recursive calls
                                           * to HsExecuteNextCommand().
                                           */
    ListEntry           cmdQueue;         /* List of logical commands     */
    ListEntry           txQueue;          /* List of AT commands          */
    HsCommand          *nextCommand;      /* The next command to execute  */

    /* Connection State Variables */
    HsChannelStates     state;           /* Current connection state      */
    U16                 flags;           /* Current connection flags      */
    U8                  linkFlags;       /* Levels of service connected   */

    /* Gateway State Information */
    HsGatewayVersion    version;         /* Profile version parsed from
                                          * SDP
                                          */
#if 0
    HsGatewayFeatures   gwFeatures;      /* Profile features parsed from
                                          * SDP read from at+brsf by wangjianjun
                                          */
#endif    
    BOOL                ibRing;          /* State of in-band ringing      */

    
    U8                  micGain;         /* Current microphone gain       */
    U8                  speakerGain;     /* Current speaker gain          */

    /* SDP variables for client */
    SdpQueryToken       sdpQueryToken;   /* Used to query the service     */
    U8                  queryFlags;      /* Defines which SDP entries were
                                          * parsed from the gateway.
                                          */
    U8                  rfServerChannel; /* When connecting hs client     */

    /* Channel Resources */
    CmgrHandler         cmgrHandler;
    AtCommands          atCommand;
    AtCommands         *currentAtCommand;
    AtCommands         *lastAtCommand;
    XaBufferDesc        atBuffer;
    BtPacket            atTxPacket;
    U8                  atTxData[HS_TX_BUFFER_SIZE];
    U16                 bytesToSend;
    EvmTimer            atTimer;
    EvmTimer            ringTimer;
    U8                  atRxBuffer[HS_RECV_BUFFER_SIZE];
    U16                 atRxLen;
};


/*---------------------------------------------------------------------------
 * HsCommand structure
 *
 * Structures of type HsCommand can be used to store the command type and
 * parameters for sending Head-Set SDK commands.
 */

struct _HsCommand {

    /* Used Internally by the Hands-free SDK */
    ListEntry   node;

    /* The type of command */
    U8          type;

    /* The command parameters */
    U32         parms[6];

    /* The command status */
    BtStatus    status;

    /* CME Error when command fails */
    HsCmeError  cmeError;

    /* Application context */
    void       *context;

    /* === Internal use only === */

    U8          state;
#if HF_USE_CALL_MANAGER == XA_ENABLED
		U16 		callMgrFlags;
#endif /* HS_USE_CALL_MANAGER == XA_ENABLED */

};

/*---------------------------------------------------------------------------
 * HsCallbackParms structure
 *
 * A pointer to this structure is sent to the application's callback
 * function notifying the application of any state changes or important
 * events.
 */
struct _HsCallbackParms {
    HsEvent    event;    /* Event associated with the callback       */

    BtStatus    status;  /* Status of the callback event             */
    BtErrorCode errCode; /* Error code (reason) on disconnect events */

    /* For certain events, a single member of this union will be valid.
     * See HsEvent documentation for more information.
     */
    union {
        void             *ptr;
        BtRemoteDevice   *remDev;
//        HsGatewayFeatures features;
        BOOL              service;
 
//      BOOL              inBandRing;
 
        U8                battery;
        U8                signal;
        U8                gain;
        HsCommand        *command;
 

 
 
 
 
#if BT_SCO_HCI_DATA == XA_ENABLED

        HsAudioData      *audioData;     /* Only valid if BT_SCO_HCI_DATA is
                                          * set to XA_ENABLED.
                                          */
        BtPacket         *audioPacket;   /* Only valid if BT_SCO_HCI_DATA is
                                          * set to XA_ENABLED.
                                          */

#endif /* BT_SCO_HCI_DATA == XA_ENABLED */

        HsAtData         *data;
    } p;
};


/*---------------------------------------------------------------------------
 * BtHsContext structure
 *
 * The BtHfContext structure contains the global context of the Head-Set 
 * unit SDK that is shared by all its connection channels.  
 *
 */

typedef struct _BtHsContext {

    /* List of registered channels */ 
    ListEntry         channelList;

    /* RFCOMM variables */ 
    RfService         hfService;
    U8                regCount;

#if BT_SECURITY == XA_ENABLED

    BtSecurityRecord  hfSecRecIn;
    BtSecurityRecord  hfSecRecOut;

#endif /* BT_SECURITY == XA_ENABLED */ 

    /* Device Manager Variables */ 
    CmgrHandler       cmgrHandler;

    /* Hands-Free SDP Variables */ 
    U8                hfSdpProtoDescList[14];
    U8                hsSdpSupportedFeatures[3];	
    SdpAttribute      hfSdpAttribute[6];
    SdpRecord         hfSdpRecord;

    /* AT Parser info */ 
    AtContext         atContext;

    /* Command Prescan */ 
    HsCmdOverride     cmdOverride;

} BtHsContext;



#if HS_MEMORY_EXTERNAL == XA_DISABLED

#if XA_CONTEXT_PTR == XA_ENABLED
extern BtHsContext *hsContext;
#define HS(s) (hsContext->s)
//#define HS(s)	HF(s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */ 
extern BtHsContext hsContext;
#define HS(s) (hsContext.s)
//#define HS(s)	HF(s)	
#endif /* XA_CONTEXT_PTR */ 

#endif /* HS_MEMORY_EXTERNAL == XA_DISABLED */ 

BtStatus HS_RegisterSec(HsChannel       *Chan,HsCallback Callback,BtSecurityParms *secParms);

#define HS_Register(ch, cb) HS_RegisterSec(ch, cb, 0)


//typedef HF_ReportSpeakerVolume HS_ReportSpeakerVolume;


BtStatus HS_CKPD_CONTROL(HsChannel *Chan, HsCommand *Command);

void HsAppCallback(HsChannel *Chan, 
                   HsEvent    Event, 
                   BtStatus   Status, 
                   void      *data);
BtStatus HS_CreateServiceLink(HsChannel *Chan, BT_BD_ADDR *Addr);
BtStatus HsStartServiceQuery(HsChannel *Chan, SdpQueryMode mode);
void HsCloseChannel(HsChannel *Chan);
BtStatus HS_CreateAudioLink(HsChannel *Chan);
BtStatus HS_DisconnectAudioLink(HsChannel *Chan);
void HsSMHandleEvent(HsChannel *chanP, U8 smEvent, AtResults *parmsP);
BtStatus HsAtSendRfPacket(HsChannel *Chan, BtPacket *Packet);
BtStatus HS_EnableSniffMode(HsChannel *Chan, BOOL Enable);
BOOL HS_IsACLConnected(HsChannel *Chan);
BtStatus HS_ReportSpeakerVolume(HsChannel *Chan, U8 Gain, HsCommand *Command);

BtStatus HS_DisconnectServiceLink(HsChannel *Chan);


BtStatus HS_SetMasterRole(HsChannel *Chan, BOOL Flag);
#define HS_SetMasterRole(c, f) CMGR_SetMasterRole(&((c)->cmgrHandler), f)



#endif
