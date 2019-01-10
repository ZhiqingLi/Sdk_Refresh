/***************************************************************************
 *
 * File:
 *     $Id: sniffer.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     Definitions for the sniffer protocol decode library. This library is
 *     based on the MiniOS API to enable portability.
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of BES.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of 
 * BES. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __SNIFFER_H
#define __SNIFFER_H

#include "config.h"
#include "utils.h"
#include "me.h"
#include "sys/hci.h"
#include "sys/debug.h"

/*---------------------------------------------------------------------------
 * Sniffer layer
 *
 *     These APIs allow implementations to interface with a lightweight
 *     Bluetooth protocol decoder module built into the Blue SDK. This
 *     analyzer is not appropriate in all situations; for a more robust
 *     analyzer you may wish to use a third-party product.
 */

/****************************************************************************
 *
 * Configuration Constants
 *
 ****************************************************************************/

/* Number of channel info structures to allocate for incoming and outgoing
 * connection/endpoint contexts.
 */
#ifndef MAX_CHANNEL_INFO_STRUCTS
#define MAX_CHANNEL_INFO_STRUCTS 30
#endif


/****************************************************************************
 *
 * Sniffer Types
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * SniffDispFilter type
 *
 *     The types and display formats of decoded packets can be managed by
 *     setting an appropriate sniffer display filter. This type is used
 *     when issuing SNIFFER_GetOption or Sniffer_SetOption calls to get
 *     or set the display filter.
 */
typedef U16 SniffDispFilter;

#define SDF_NORMAL          0x0000  /* Normal display mode */
#define SDF_DISPLAY_RAW     0x0001  /* Show data packets in raw hex format */

/* End of SniffDispFilter */


/*---------------------------------------------------------------------------
 * SniffDispHandler type
 *
 *     A function of this type is provided to the sniffer for writing lines
 *     of output to the display device.
 */
typedef void (*SniffDispHandler)(const char *Message);

/* End of SniffDispHandler */


/*---------------------------------------------------------------------------
 * SnifferOption type
 *
 *     This type is used when issuing SNIFFER_GetOption and SNIFFER_SetOption
 *     calls.
 */
typedef U8 SnifferOption;

/* The current sniffer display handler function pointer. This option must
 * be set for the sniffer to decode and output packets. The OptionValue 
 * parameter is a pointer of type 'SniffDispHandler'. 
 */
#define SO_DISPLAY_HANDLER  1

/* The current sniffer output format filter. This option may be used to
 * manage display options within the sniffer decode library. The OptionValue
 * parameter is a pointer of type 'SniffDispFilter'. 
 */
#define SO_DISPLAY_FILTER   2

/* End of SnifferOption */

/*---------------------------------------------------------------------------
 * SEL2cap structure
 *
 */
typedef struct _SEL2cap{

    /* L2CAP local and remote channel Id endpoints. These fields
     * are valid when SPI_L2CAP is set in 'proto'.
     */ 

    U16                 l2cap;
    U16                 l2cap_remote;
} SEL2cap;


/*---------------------------------------------------------------------------
 * SERfcomm structure
 *
 */
typedef struct _SERfcomm{
    /* RFCOMM Channel DLCI. Valid when SPI_RFCOMM is set in 'proto'. */ 
    U16                 rfcomm;
} SERfcomm;


/*---------------------------------------------------------------------------
 * SELR union
 *
 */
typedef union _SELR{

    /* L2CAP channel identifiers */ 
    SEL2cap l;

    /* RFCOMM channel identifiers */ 
    SERfcomm r;

} SELR;


/*---------------------------------------------------------------------------
 * SniffEndpoint structure
 *
 */
typedef struct _SniffEndpoint {

    /* Type of protocol to decode (e.g. SPI_SDP, SPI_OBEX). See
     * "/inc/sys/debug.h" for a definition.
     */
    SniffProtocolId     type;

    /* Valid Protocol identifiers. This value is created by or'ing
     * together the SniffProtocolId's that correspond to the multiplexing
     * layer id's that are set (e.g. SPI_LM|SPI_RFCOMM).
    */
    SniffProtocolId     proto;

    /* Link Manager id. This corresponds to the HCI connection handle, 
     * which is shared by both sides. Valid when SPI_LM is set in 'proto'.
     */
    U16                 lmp;

    SELR                u;

} SniffEndpoint;

/*---------------------------------------------------------------------------
 * SniffDecoder structure
 * 
 * Specifies behavior for a generic plug-in decoder.
 *
 */
typedef struct _SniffDecoder {
    ListEntry node; /* Internal use only */

    /* Protocol to be decoded by this decoder */
    SniffProtocolId protocol; 
    
    /* Size that must be available for this decoder's context */
    int contextSize;          

    /* Function to call to initialize the context on first use */
    void (*init)(void *context);

    /* Function to call to parse a buffer of data. Bytes of output are
     * delivered via SNIFFER_Message.
     *
     * Returns the address of the next unencoded byte in buff after parsing
     * as much as possible.
     */
    const U8 *(*parse)(void *context, const U8 *buff, U16 len);

} SniffDecoder;


/****************************************************************************
 *
 * Public Sniffer Display Library Functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SNIFFER_Reset()
 *
 *     This function resets the state of the sniffer packet decoder and 
 *     connection tables.
 *
 * Requires:
 *     XA_DEBUG enabled.
 *
 * Parameters:
 *     Full - When TRUE the connection table is reset along with the packet
 *         decoders. When FALSE, only the packet decoders are reset.
 *         Typically FALSE is used, unless a critical sniffer error is
 *         encountered and a full reset is necessary.
 */
void SNIFFER_Reset(BOOL Full);


/*---------------------------------------------------------------------------
 * SNIFFER_GetOption()
 *
 *     This function retrieves display option settings from the sniffer library.
 *
 * Requires:
 *     XA_DEBUG enabled.
 *
 * Parameters:
 *     OptType - The option type to retrieve, taken from the list of SO_ types.
 *
 *     OptionValue - A pointer to a variable of the type used by the specific
 *         'OptType' requested.
 */
BOOL SNIFFER_GetOption(SnifferOption OptType, void *OptionValue);


/*---------------------------------------------------------------------------
 * SNIFFER_SetOption()
 *
 *     This function sets display options within the sniffer library.
 *
 * Requires:
 *     XA_DEBUG enabled.
 *
 * Parameters:
 *     OptType - The option type to set, taken from the list of SO_ types.
 *
 *     OptionValue - A pointer to a variable of the type used by the specific
 *         'OptType' being set.
 */
BOOL SNIFFER_SetOption(SnifferOption OptType, void *OptionValue);


/*---------------------------------------------------------------------------
 * SNIFFER_DisplayHciPacket()
 *
 *     This function decodes and displays a HCI transmit packet. A
 *     sniffer output function must be set via SNIFFER_SetOption()
 *     prior to calling this function.
 *
 * Requires:
 *     XA_DEBUG enabled.
 *
 * Parameters:
 *     Packet - The HciPacket structure to decode and display.
 *
 *     TimeStamp - The local time the packet was transmitted (in milliseconds).
 */
void SNIFFER_DisplayHciPacket(const HciPacket *Packet, U32 TimeStamp);


/*---------------------------------------------------------------------------
 * SNIFFER_DisplayHciBuffer()
 *
 *     This function decodes and displays a HCI receive buffer. A
 *     sniffer output function must be set via SNIFFER_SetOption()
 *     prior to calling this function.
 *
 * Requires:
 *     XA_DEBUG enabled.
 *
 * Parameters:
 *     Buffer - The HciBuffer structure to decode and display.
 *
 *     TimeStamp - The local time the buffer was received (in milliseconds).
 */
void SNIFFER_DisplayHciBuffer(const HciBuffer *Buffer, U32 TimeStamp);


/*---------------------------------------------------------------------------
 * SNIFFER_DisplayLmpPacket()
 *
 *     This function decodes and displays an INet-LMP packet. A
 *     sniffer output function must be set via SNIFFER_SetOption()
 *     prior to calling this function.
 *
 * Requires:
 *     XA_DEBUG enabled.
 *
 * Parameters:
 *     RxTx - String indicating direction of LMP packet.
 *
 *     Packet - The INet LMP packet data.
 *
 *     Len - The length of the data pointed to by 'Packet'.
 *
 *     TimeStamp - The local time the LMP command was sent or received 
 *         (in milliseconds).
 */
void SNIFFER_DisplayLmpPacket(const char *RxTx, const U8 *Packet, U16 Len, U32 TimeStamp);


/*---------------------------------------------------------------------------
 * SNIFFER_RegisterEndpoint()
 *
 *     This function is called by protocol implementations to register
 *     the protocol type for a specific endpoint. It is not necessary
 *     to deregister an endpoint.
 *
 * Requires:
 *     XA_DEBUG enabled.
 *
 * Parameters:
 *     Endpoint - A completed SniffEndpoint structure which describes the
 *         new endpoint to register. 
 */
void SNIFFER_RegisterEndpoint(SniffEndpoint *Endpoint);

/*---------------------------------------------------------------------------
 * SNIFFER_RegisterDecoder()
 *
 *     Specifies a new decoder to which the sniffer may delegate its decode.
 *
 * Requires:
 *     XA_DEBUG enabled.
 *
 * Parameters:
 *     decoder - A SniffDecoder structure with "init", "decode", and 
 *         "protocol" members filled.
 *
 */
void SNIFFER_RegisterDecoder(SniffDecoder *decoder);


/*---------------------------------------------------------------------------
 * SNIFFER_Message()
 *
 *     Delivers decoded information to the sniffer interface.
 *
 * Requires:
 *     XA_DEBUG enabled.
 *
 * Parameters:
 *     msg - NULL-terminated string to send to display, log, etc. as defined
 *         by the current sniffer UI.
 *
 */
void SNIFFER_Message(const char *msg);


/*****************************************************************************
 *
 * Internal Sniffer Data Types & Structures
 *
 ****************************************************************************/

/*****************************************************************************
 *
 * Internal Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * LINE_WIDTH constant
 *
 *     Number of hex-bytes displayed per line.
 */
#define LINE_WIDTH          16

/*---------------------------------------------------------------------------
 * LINE_LENGTH constant
 *
 *     Maximum number of chars in a sniffer output line. This does not
 *     include a <CR><LF> which may be appended by the output function.
 */
#define LINE_LENGTH         80


/*****************************************************************************
 *
 * Internal Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SniffDirection type
 *
 *     This type is used in ChannelInfo to identify the channel's data
 *     direction.
 */
typedef U8 SniffDirection;

#define SD_OUTBOUND     0x01
#define SD_INBOUND      0x02

/* End of SniffDirection */

/* Forward reference of EvmTimer. 
 */
typedef struct _ObexContext ObexContext;

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * ObexContext structure
 *
 *      A structure type used to track a particular OBEX connection. This 
 *      context tracks both the transmit and receive context simultaneously.
 */
struct _ObexContext {
    /* === Internal Use Only === */
    U16     packetLen;          /* Size of current OBEX packet */
    U16     curPacketLen;       /* Amount of current packet data received */
    U16     savedCurPacketLen;  /* Saved current packet length, in case 
                                 * a client abort is sent during a response */
    U16     savedPacketLen;     /* Saved OBEX packet length */
    U8      stageBuff[12];      /* OBEX Header stage buffer */
    U8      stageLen;           /* Num bytes to read into stageBuff */
    U8      stageOffset;        /* Num bytes in stageBuff */
    U8      rxState;            /* Receive parser state */
    U8      opcode;             /* Current Opcode */
    U8      rsp;                /* Current Response */
    BOOL    isCommand;          /* Is the packet a Command or Response */
    BOOL    inSync;             /* Is the parser synced with the data */
    BOOL    connIdHeader;       /* Do we have a Connection Identifier Header */
    BOOL    seqNumHeader;       /* Do we have a Session Sequence Number Header */
    BOOL    actionIdHeader;     /* Do we have an Action Identifier Header */
};

/* End of ObexContext */

#define MAX_TCS_BUFF 120
/*---------------------------------------------------------------------------
 * PacketInfo structure
 *
 *     This structure is used to manage the decode state of a data packet.
 */
typedef struct _PacketInfo {
    /* === Internal Use Only === */
    U16         lm_opcode;
    U16         lm_llcLen;  /* Payload after LM header  */
    U16         lm_lmLen;   /* Size of LM header  */
    BOOL        lm_inSync;  /* Synchronized and parsing */
    U16         lm_connHandle;
    U8          l2cap_hdrBuff[8];   /* L2CAP I-Frame header */
    U16         l2cap_state;
    U16         l2cap_cid;
    U16         l2cap_len;  /* Remaining L2CAP data length */
    BOOL        l2cap_inSync; /* Used by FEC channels */
    U16         rfc_state;
    U16         rfc_len;
    U8          rfc_stageBuff[5];
    U8          rfc_stageLen;
    U8          rfc_stageOffset;
    U8          rfc_cmd;
    U8          rfc_cmdRsp;
    U8          rfc_addr;

    /* SDP stuff should be moved to ChannelInfo */
    U16         sdp_state;
    U16         sdp_bufferLen;
    U16         sdp_paramLen;
    U16         sdp_offset;
    BOOL        sdp_contState;

    /* TCS stuff should be moved to Channel Info */
    U16         tcs_len;
    U16         tcs_pos;
    U8          tcs_buff[MAX_TCS_BUFF];
    U8          output[LINE_LENGTH];
} PacketInfo;

/* End of PacketInfo */

typedef union _ChannelInfoContext {
    /* For SPI_OBEX, this is an ObexContext structure. */
    ObexContext     obContext;
    /* For other SPIs they can have up to 512 bytes */
    U8              buff[512];
} ChannelInfoContext;

/*---------------------------------------------------------------------------
 * ChannelInfo structure
 *
 *     This structure is used to manage the state of channel multiplexers.
 */
typedef struct _ChannelInfo {

    /* Node so that this structure can be managed on a list */
    ListEntry           node;

    SniffEndpoint       endpoint;  

    /* Channel-specific info to identify what to do with the data
     * The meaning of this field will depend on the highest bit set in flags
     */
    ChannelInfoContext info;

} ChannelInfo;

/* End of ChannelInfo */


/*---------------------------------------------------------------------------
 * XaSnifferCore structure
 *
 *     Core context for the common sniffer component.
 */
typedef struct _XaSnifferCore {
    /* === Internal Use Only === */
    BOOL                reset;

    SniffDispFilter     filter;

    SniffDispHandler    output;

    PacketInfo          rx;
    PacketInfo          tx;

    PacketInfo         *context;

    U32                 timeStart;
    U32                 timeStamp;

    U8                  stage[LINE_WIDTH];
    U8                  stageLen;

    char                timeBuff[20];
    char                obexBuff[20];

    SniffDirection      direction;

    ListEntry           channelList;
    ChannelInfo         channelTable[MAX_CHANNEL_INFO_STRUCTS];

    ListEntry           decoders;

} XaSnifferCore;

/* End of XaSnifferCore */


/****************************************************************************
 *
 * Prototypes for external parser modules
 *
 ****************************************************************************/

/* This doesn't work because the external modules currently reference the
 * static Sniffer context. Need a registration procedure.
 *const char *SNIFFER_TCS_Parse(const U8 *Buffer, U16 BufferLen);
 */

#endif /* __SNIFFER_H */
