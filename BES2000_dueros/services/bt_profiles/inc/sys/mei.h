/****************************************************************************
 *
 * File:
 *     $Id: mei.h 860 2009-02-19 00:15:35Z dliechty $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 860 $
 *
 * Description:
 *     This file contains internal definitions for the Bluetooth
 *     Management Enity.
 *
 * Created:
 *     October 18, 1999
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
 * Use of this work is governed by a license granted by BES, 
 * This work contains confidential and proprietary information of 
 * BES. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __MEI_H
#define __MEI_H

#include "me.h"
#include "sec.h"
#include "sys/mexp.h"
#include "utils.h"
#include "hcitrans.h"
#include "sys/hci.h"
#include "sys/l2capxp.h"

#if NUM_SCO_CONNS > 0
#include "mesco.h"
#endif
#if NUM_KNOWN_DEVICES > 0
#include "medev.h"
#endif

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * BtOpMask is used to indicate which operations are in progress.
 */
typedef U16 BtOpMask;    /* Used internally */
#define BOM_INQUIRY            0x0001
#define BOM_CANCEL_INQUIRY     0x0002
#define BOM_ACCESS_MODE        0x0004
#define BOM_ACCESS_MODE_AUTO   0x0008
#define BOM_HCICOMMAND_BUSY    0x0010
#define BOM_INITRADIO_BUSY     0x0020

/*---------------------------------------------------------------------------
 *
 * BtOpType defines Bluetooth operations. The typedef is in me.h
 */
#define BOP_NOP                 0x00  /* No operation */
#define BOP_INQUIRY             0x01  /* Inquiry operation */

#define BOP_LINK_CONNECT        0x05  /* Initiate a link connection */
#define BOP_LINK_ACCEPT         0x06  /* Accept an incoming link */
#define BOP_LINK_REJECT         0x07  /* Reject an incoming link */
#define BOP_LINK_DISCONNECT     0x08  /* Disconnet a link */
#define BOP_AUTHENTICATE        0x09  /* Authenticate the link */
#define BOP_LINK_KEY_RSP        0x0a  /* Respond to link key request */
#define BOP_USER_IO_RSP         0x0b  /* Respond to user IO request */
#define BOP_AUTHORIZE           0x0c  /* Authorization  operation */
#define BOP_ENCRYPTION          0x0d  /* Encryption operation */
#define BOP_SECURITY_MODE       0x0e  /* Set security mode operation */
#define BOP_ACCESS_MODE         0x0f  /* Change accessability mode operation */
#define BOP_CANCEL_INQUIRY      0x10  /* Cancel inquiry operation */
#define BOP_REM_NAME_REQ        0x11  /* Remote name request operation */
#define BOP_LOCAL_NAME_CHG      0x12  /* Local name change */
                                
/* Mode change operations must be continguous with no other operations
 * between. Hold must be first and exit park mode must be last.
 */
#define BOP_HOLD_MODE           0x13  /* Hold mode operation */
#define BOP_SNIFF_MODE          0x14  /* Enter sniff mode operation */
#define BOP_EXIT_SNIFF_MODE     0x15  /* Exit sniff mode */
#define BOP_PARK_MODE           0x16  /* Enter park mode */
#define BOP_EXIT_PARK_MODE      0x17  /* Exit park mode */
/* end of mode change operations */

#define BOP_SWITCH_ROLE         0x18
#define BOP_WRITE_COD           0x19  /* Write class of device */
#define BOP_GEN_COMMAND         0x1a  /* General command */
#define BOP_SCO_CONNECT         0x1b  /* Initiate a SCO link connection */
#define BOP_SCO_ACCEPT          0x1c  /* Accept an incoming link */
#define BOP_SCO_REJECT          0x1d  /* Reject an incoming link */
#define BOP_SCO_DISCONNECT      0x1e  /* Disconnet a link */
#define BOP_LINK_IN_REJECT      0x1f  /* Internal link reject */
#define BOP_SCO_VOICESET        0x20  /* Set SCO voice parameters */ 
#define BOP_WRITE_LINK_POLICY   0x21  /* Set link policy */

#define BOP_WRITE_DEF_LNK_POL   0x23  /* Set default link policy */
#define BOP_SCO_SETUP           0x24  /* Initiate an SCO link setup (BT v1.2) */
#define BOP_CANCEL_CONNECT      0x25  /* Cancel a link connection */
#define BOP_CANCEL_REM_NAME     0x26  /* Cancel a remote name request */
#define BOP_CHANNEL_CLASS       0x27  /* Set channel classification for AFH */
#define BOP_DISCOVER_ROLE       0x28  /* Discover Role */
#define BOP_CANCEL_AUTHORIZE    0x29  /* Cancel Authorization request */
#define BOP_CHANGE_PKT_TYPE     0x2a  /* Change connection packet type */
#define BOP_SCO_PACKET_TYPE     0x2b  /* Change SCO connection packet type */
#define BOP_IO_CAP_RSP          0x2d  /* IO Capability Response */
#define BOP_REM_OOB_DATA_RSP    0x30  /* Respond to a remote OOB data request */
#define BOP_DENY_OOB_DATA_RSP   0x31  /* Negative reply to an OOB data request */
#define BOP_GET_LOCAL_OOB_DATA  0x32  /* Get the local OOB data */
#define BOP_READ_REM_FEATURES   0x33  /* Read the remote features */
#define BOP_READ_REM_EXT_FEAT   0x34  /* Read the remote extended features */
#define BOP_REFRESH_ENCRYPT_KEY 0x35  /* Refresh the encryption key */
#define BOP_SNIFF_SUBR_PARMS    0x36  /* Enable sniff subrating */
#define BOP_SET_INQ_MODE        0x37  /* Set the inquiry response mode */
#define BOP_SET_EXT_INQ_RESP    0x38  /* Set the extended inquiry response */
#define BOP_SET_ERR_DATA_REP    0x39  /* Set the erroneous data reporting */
#define BOP_SET_INQ_TX_PWR      0x40  /* Set the inquiry response TX power level */
#define BOP_SET_SMP_DEBUG_MODE  0x41  /* Set the simple pairing debug mode */
#if BT_BEST_SYNC_CONFIG == XA_ENABLED
#define BOP_WRITE_SYNC_CONFIG   0x42  /* Write Sync Config */
#endif

#define BOP_SLEEP_ENABLE      0x43  /* enable sleep */
#define BOP_SLEEP_DISABLE      0x44  /* enable sleep */

#define BOP_SET_QOS             0x45

#define BOP_SCO_SWITCH    0x46

#define BOP_WRITE_SCAN_ACTIVITY 0x47 /* write page/inq scan activity */

#define BOP_WRITE_INQURIYSCAN_TYPE  0x48

#define BOP_WRITE_PAGESCAN_TYPE  0x49

#define BOP_WRITE_MEMORY      0x4a

#define BOP_READ_REM_VERSION      0x4b

#define BOP_WRITE_LINK_SUPERV_TIMEOUT   0x4c


#define BOP_WRITE_ADV_DATA      0x4D
#define BOP_WRITE_ADV_PARA      0x4E
#define BOP_WRITE_ADV_EN      0x4F
#define BOP_WRITE_SCAN_PARA      0x50
#define BOP_WRITE_SCAN_EN      0x51
#define BOP_WRITE_PRIVATE_ADDR      0x52
#define BOP_ADD_DEV_TO_WHITELIST      0x53
#define BOP_CLEAR_WHITELIST      0x54
#define BOP_WRITE_ADV_DIS      0x55




#if IAG_BLE_INCLUDE == XA_ENABLED
#define BOP_SEND_BLE_CMD        0x80
#endif
#define BOP_BUSY                0xfe  /* Indicate op queue is busy */

/*---------------------------------------------------------------------------
 *
 * BtOpEvent defines events for the operation state machine.
 */
typedef U8 BtOpEvent;

#define BOE_NOP        0x00  /* No operation */
#define BOE_START      0x01  /* Start of operation */
#define BOE_CONT       0x02  /* Continue the operation */
#define BOE_TASK_END   0x03  /* Task finished successfully */
#define BOE_TASK_ERR   0x04  /* Task ended because of an error */

/*---------------------------------------------------------------------------
 *
 * Authenticate State of a Remote Device (BtAuthState). The typedef is in 
 * me.h
 */
#define BAS_NOT_AUTHENTICATED  0x00
#define BAS_START_AUTHENTICATE 0x01
#define BAS_WAITING_KEY_REQ    0x02
#define BAS_SENDING_KEY        0x03
#define BAS_WAITING_FOR_IO     0x04
#define BAS_WAITING_FOR_IO_R   0x05
#define BAS_WAITING_FOR_KEY    0x06
#define BAS_WAITING_FOR_KEY_R  0x07
#define BAS_AUTHENTICATED      0x08

/*---------------------------------------------------------------------------
 *
 * Authorize State of a Remote Device (BtAuthorizeState). The typedef is in 
 * me.h
 */
#define BAS_NOT_AUTHORIZED     0x00
#define BAS_START_AUTHORIZE    0x01
#define BAS_AUTHORIZED         0x02
#define BAS_AUTHORIZED_SERVICE 0x03

/*---------------------------------------------------------------------------
 *
 * Encyrption State of a Remote Device (BtEncryptState). The typedef is in 
 * me.h
 */
#define BES_NOT_ENCRYPTED      0x00
#define BES_START_ENCRYPT      0x01
#define BES_ENCRYPTED          0x02
#define BES_END_ENCRYPT        0x03

/*---------------------------------------------------------------------------
 *
 * Security Access State is used to keep track of the access procedure. The
 * typedef is in me.h.
 */

#define BAS_NO_ACCESS          0x00
#define BAS_AUTH_COMPLETE      0x01
#define BAS_AUTHORIZE_COMPLETE 0x02
#define BAS_ENCRYPT_COMPLETE   0x04

/*---------------------------------------------------------------------------
 *
 * Security Mode state is used to keep track of the security state. 
 */

typedef U8 BtSecurityModeState;

#define BSMS_LEVEL2       0x00
#define BSMS_START_LEVEL3 0x01
#define BSMS_LEVEL3       0x02
#define BSMS_END_LEVEL3   0x03

/*---------------------------------------------------------------------------
 *
 * Link Receive Buffer State (BtLinkRxState) is used to track the L2CAP
 * receive buffer state. The typedef is in me.h.
 */

#define BRXS_COMPLETE      0x00
#define BRXS_COPYING       0x01
#define BRXS_FORWARDING    0x02
#define BRXS_FLUSH         0x03
#define BRXS_BROADCAST     0x04
#define BRXS_DISCONNECTING 0x05

/*---------------------------------------------------------------------------
 *
 * Values used for the Scan_Enable parameter of the Write_Scan_Enable command.
 */
#define BSE_INQUIRY    0x01
#define BSE_PAGE       0x02

/*---------------------------------------------------------------------------
 *
 * Values used in flags field of MeCommandToken.
 */
#define MCTF_NEED_CHECK  0x01
#define MCTF_ASYNC       0x02

/*---------------------------------------------------------------------------
 *
 * Values used to determine the handling of simple pairing
 */
#define IO_USE_PIN_CODE 0
#define IO_USE_PASS_KEY 1
#define IO_USE_CONFIRM  2
#define IO_USE_DISPLAY  3

/*---------------------------------------------------------------------------
 * IO Capabilities State Flags in BtRemoteDevice.
 */
#define IO_CAP_RSP_RECVD 0x01
#define IO_CAP_REQ_RECVD 0x02
#define IO_CAP_REJECTED  0x04

/*---------------------------------------------------------------------------
 *
 * MeTaskHandler is a pointer to a function to handle the HCI event for a
 * given task.
 */
typedef void (*MeTaskHandler)(U8 event, U8 len, U8* data);

#if IAG_BLE_INCLUDE == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * MeBleTaskHandler is a pointer to a function to handle the HCI event for a
 * given BLE task.
 */
typedef void (*MeBleTaskHandler)(HciEvent * pEvent);
#endif
/*---------------------------------------------------------------------------
 *
 * MeOpHandler is a pointer to a function to handle events for a
 * given operation.
 */
typedef void (*MeOpHandler)(BtOpEvent event);

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * BtInquiryOp is used to hold an inquiry operation on a queue. 
 */
typedef struct {
    BtOperation op;
    BtIac       lap;
    U8          len; /* Duration of Inquiry */
    U8          max; /* Maximum number of responses */
} BtInquiryOp;

/*---------------------------------------------------------------------------
 *
 * BtScanOp is used to hold an inquiry or page scan operation on a queue. 
 */
typedef struct {
    BtOperation        op;  /* This must be the first item */
    uint8_t            nc;  /* 0 means no connection. 1 means connection 1. 2 means connection 2.*/
    BtAccessibleMode   mode;
    BtAccessModeInfo    i;
} BtScanOp;

/*---------------------------------------------------------------------------
 * BtExtInqModeOp is used to hold Inquiry Mode and Extended Inquiry
 * operations on a queue.
 */
typedef struct {
    BtOperation        op;

    /* Informatino used for setting the inquiry mode */
    BtInquiryMode inqMode;

} BtInqModeOp;

typedef struct {
    BtOperation        op;

    /* Informatino used for setting the inquiry mode */
    U8 scanMode;

} BtScanModeOp;


/*---------------------------------------------------------------------------
 * BtExtInqModeOp is used to hold Inquiry Mode and Extended Inquiry
 * operations on a queue.
 */
typedef struct {
    BtOperation op;

    /* Determines if FEC will be used on the extende inquiry
     * response.  Use 0 means disabled, and 1 means enabled.
     */ 
    U8 fec;

    /* The extended inquiry response for the local device */
    U8 inqResp[240];

} BtExtInqRespOp;

/*---------------------------------------------------------------------------
 * BtErrDataRepOp is used to hold the operation and parameters for erroneous
 * data reporting.
 */
typedef struct {
    BtOperation      op;

    /* Defines the state of erroneous data reporting */
    BtErrDataReporting errDataRep;

} BtErrDataRepOp;

/*---------------------------------------------------------------------------
 * BtSmpDbgModeOp is used to hold the operation and parameters for Secure
 * Simple Pairing debug mode.
 */
typedef struct _BtSmpDbgModeOp {
    BtOperation op;
    
    /* Defines the simple pairing debug mode */
    BOOL mode;
} BtSmpDbgModeOp;

/*---------------------------------------------------------------------------
 * BtInqTxPwrOp is use to hold the operation and parameters for setting the
 * Inquiry Response TX Power Level.
 */
typedef struct {
    BtOperation      op;

    /* Defines the Inquiry Responst TX Power Level in dBm */
    BtInqTxPower  power;
} BtInqTxPwrOp;

/*---------------------------------------------------------------------------
 *
 * BtSecurityOp is used to deal with asynchronous security requests. 
 */
typedef struct {
    BtOperation     op;
    BtRemoteDevice* remDev;
    BT_BD_ADDR         bdAddr;
    U8              data[16];
    U8              ioType;
    BOOL            rejectKey;

    /* Extra data */
    union {
        U8                  pLen;
        BtEncryptMode       eMode;
        BtSimplePairingMode pMode;
        
        struct {
            BtSecurityMode  mode;
            BOOL            encrypt;
        } secMode;
    } x;

} BtSecurityOp;

#if NUM_BT_DEVICES == 1
/* Add one more BtSecurityOp if NUM_BT_DEVICES == 1.
 * Some radios require at least two.
 */
#define NUM_SEC_OPS     2
#else
#define NUM_SEC_OPS     NUM_BT_DEVICES
#endif

/*---------------------------------------------------------------------------
 *
 * BtConRejectOp is used to reject ACL and SCO connections. 
 */
typedef struct {
    BtOperation  op;
    BT_BD_ADDR      bdAddr;
    U8           rejectReason;
} BtConRejectOp;

/*---------------------------------------------------------------------------
 *
 * BtMeContext contains all the data used used internally by the
 * Management Enity.
 */

typedef struct {
    BtOperation        op;

    /* Informatino used for setting the inquiry mode */
    U16 scohandle;

} BtScoSwitchOp;

typedef struct {
    BtOperation        op;
    U32 address;
    U32 value;
} BtWriteMemoryOp;

#if defined( __TWS_RECONNECT_USE_BLE__) || defined(__TWS_CHARGER_BOX__)
typedef struct {
    BtOperation        op;
    uint8_t len;
    uint8_t data[31];
} BtWriteAdvDataOp;


typedef struct{
    U16 interval_min;
    U16 interval_max;
    U8   adv_type;
    U8   own_addr_type;
    U8   peer_addr_type;
    BT_BD_ADDR  bd_addr;
    U8   adv_chanmap;
    U8  adv_filter_policy;
}adv_para_struct;


typedef struct {
    BtOperation        op;
    adv_para_struct  adv_para;

} BtWriteAdvParaOp;

typedef struct {
    BtOperation        op;
    U8 en;
} BtWriteAdvEnOp;

typedef struct{
    U8 scan_type;
    U16 scan_interval;
    U16 scan_window;
    U8  own_addr_type;
    U8  scan_filter_policy;
}scan_para_struct;


typedef struct {
    BtOperation        op;
    scan_para_struct scan_para;
} BtWriteScanParaOp;

typedef struct {
    BtOperation        op;
    U8 scan_en;
    U32 filter_duplicat;
} BtWriteScanEnOp;

typedef struct {
    BtOperation        op;
    BT_BD_ADDR  rand_bd_addr;
} BtWritePrivateAddrOp;

typedef struct {
    BtOperation        op;
    U8 addr_type;
    BT_BD_ADDR  bd_addr;
} BtAddDevToWhiteListOp;



#endif



/*---------------------------------------------------------------------------
 *
 * BtMeContext contains all the data used used internally by the
 * Management Enity.
 */

typedef struct {
    ListEntry     regList;     /* List of registered global handlers */
    ListEntry     opList;      /* Operation queue */
    ListEntry     secList;     /* Security record list */
    ListEntry     cmdList;     /* List of pending commands */
    ListEntry     asyncList;   /* List of pending Async commands */
    ListEntry     pendList;    /* Pending operation list */
    BtEvent       btEvent;     /* Bluetooth event */
    BtOpMask      opMask;      /* Indicates which operations are in progress */

    BtOperation*  curOp;       /* Current operation */

    HciCommand    hciGenCmd;   /* For sending general HCI commands */
#if IAG_BLE_INCLUDE == XA_ENABLED
    MeBleTaskHandler bleTaskHandler;
    HciEvent *        taskBleEvent;
#endif
    MeTaskHandler taskHandler; /* Handle HCI events */
    U8            taskEvent;   /* Event expected for taskHandler */
    MeOpHandler   opHandler;   /* Handle operation events */

    BtHandler*    authorizeHandler;
    BtHandler*    secModeHandler;

    /* Handler for the pairing operation */
    BtHandler*    pairingHandler;

    /* Incoming connection fields */
    BtHandler*       acceptHandler;
    BtConnectionRole connectionPolicy;

    U8            stackState;
    U8            opState;
    U8            taskState;

#if NUM_SCO_CONNS > 0
    BtScoConnect        scoTable[NUM_SCO_CONNS];
    BtScoAudioSettings  vSettings;
#endif    /* NUM_SCO_CONNS > 0 */

    /* Device table */
    BtRemoteDevice devTable[NUM_BT_DEVICES];
    BtSecurityOp   secOpTable[NUM_SEC_OPS];
#ifdef __ACC_FRAGMENT_COMPATIBLE__
    U8             rxBuffTable[NUM_BT_DEVICES][L2CAP_MTU_FOR_ACC+6+L2CAP_PRELUDE_SIZE];
#else
    U8             rxBuffTable[NUM_BT_DEVICES][L2CAP_MTU+6+L2CAP_PRELUDE_SIZE];
#endif

#if NUM_KNOWN_DEVICES > 0
    /* Non-Connected Device Selection list */
    ListEntry      deviceList;
    ListEntry      btDeviceFreeList;
    BtDeviceContext btDevices[NUM_KNOWN_DEVICES];

    BtHandler*      dsMonitor;
#endif

    BtConRejectOp  rejectOp;

    U8             pendCons;   /* Number of outgoing and incoming connections in process */
    U8             activeCons; /* Number of active connections */
    U8             active_slave;
    U8             inHoldCons; /* Number of incoming holds */
    U8             holdCons;   /* Number of outgoing holds */

    /* Access Mode info */
    /* Current settings */
    BOOL               isLiac;   /* TRUE if LIAC or FALSE if GIAC */
    BtAccessibleMode   accModeCur;
    BtAccessModeInfo   accInfoCur;

//#if !defined(__EARPHONE__) && !defined(__BT_ONE_BRING_TWO__)
    /* Non connected accessible info */
    BtAccessibleMode   accModeNC;
    BtAccessModeInfo   accInfoNC;

#if BT_ALLOW_SCAN_WHILE_CON == XA_ENABLED
    /* Connected accessible info */
    BtAccessibleMode    accModeC;
    BtAccessModeInfo    accInfoC;
#endif /* BT_ALLOW_SCAN_WHILE_CON */

//#else
    U8   setAccFlag;

    BtAccessibleMode   accMode;
    BtAccessModeInfo   accInfo;

    BtAccessibleMode   accModePair;
    BtAccessModeInfo   accInfoPair;

    BtAccessibleMode   accModeNCON;
    BtAccessModeInfo   accInfoNCON;
    
    BtAccessibleMode   accMode1C;
    BtAccessModeInfo   accInfo1C;
    
    BtAccessibleMode   accMode2C;
    BtAccessModeInfo   accInfo2C;
//#endif

    /* Operations */
    BtInquiryOp     inquiryOp;   /* Inquiry operation */
    BtScanOp        accOp;
    BtScanOp        accUpOp;
    BtScanOp        accDownOp;
    BtOperation     codOp;         /* Class of device operation */
    BtOperation     nameOp;        /* Write Local name operation */
#if BT_BEST_SYNC_CONFIG == XA_ENABLED
    BtOperation     syncConfigOp;        /* Write Local name operation */
#endif
    BtOperation     sleepOp;     /* Write Sleep Enable operation */
    BtOperation     policyOp;      /* Write Default Link Policy operation */
    BtOperation     channelOp;     /* Write Channel Classification (AFH) operation */
    BtScoSwitchOp     scoswitchOp;
    BtWriteMemoryOp     wrMemOp;
    BtInqModeOp     inqModeOp;     /* Write Inquiry Mode operation */
    BtExtInqRespOp  extInqRespOp;  /* Write Extended Inquiry Response operation */
    BtErrDataRepOp  errDataRepOp;  /* Write Erroneous Data Reporting operation */
    BtInqTxPwrOp    inqTxPwrOp;    /* Write the Inquiry TX Power Level */
    BtSmpDbgModeOp  smpDbgModeOp;  /* Write the simple pairing debug mode */

    BtOperation     scanActivityOp; /* Write page/inq scan activity */
    BtScanModeOp     inqscanModeOp;     /* Write Inquiry Mode operation */
    BtScanModeOp     pagescanModeOp;     /* Write Inquiry Mode operation */

    BtOperation     linkSupervTimeoutOp;   /* Write Link Supe rvision Timeout operation */

#if defined( __TWS_RECONNECT_USE_BLE__) || defined(__TWS_CHARGER_BOX__)
    BtWriteAdvDataOp  writeadvdataOp;
    BtWriteAdvParaOp   writeadvparaOp;
    BtWriteAdvEnOp    writeadvenOp;
    BtWriteAdvEnOp    writeadvdisOp;
    
    BtWriteScanParaOp  writescanparaOp;
    BtWriteScanEnOp    writescanenOp;
    BtWritePrivateAddrOp  writeparivateaddrOp;
    BtAddDevToWhiteListOp adddevtowhitelistOp;
    BtOperation     clearwhitelistOp; 
#endif


    
#if BT_SECURITY == XA_ENABLED
    /* Security mode state and encryption setting*/
    BtSecurityModeState secModeState;        /* Security mode state */
    BOOL                secModeEncrypt;      /* Encryption mode */
    BtSimplePairingMode secSimplePairMode;   /* Simple Pairing mode state */
    BtIoCapabilities    secIoCap;            /* Default global IO capabilities */
    BtBondingMode       secBondingMode;      /* The current Bonding mode */
    BtAuthRequirements  secAuthRequirements; /* Default global Authentication reqs */
    BtOobData           secOobData;          /* Received remote OOB data  */
    BtOperation         secOobOp;            /* Op to get the local OOB data */
    BtAuthorizeRequirements secAuthorizeRequirements; /* Default global authorization reqs */
#endif

    BtClassOfDevice     classOfDevice;
    BtClassOfDevice     sdpServCoD;
    BtClassOfDevice     writeCoD;

    U16             curTask;

    BtLinkPolicy    inAclPolicy;
    BtLinkPolicy    outAclPolicy;

    /* Local radio settings */
    U16             radioPageTimeout;
    BT_BD_ADDR         bdaddr;
    const U8*       localName;
    U8              localNameLen;
#if BT_BEST_SYNC_CONFIG == XA_ENABLED
    U16             sync_path;
    U8              sync_max_buffer_size;
    U8              sync_cvsd_bypass;
#endif
    U8              btVersion;
    U8              btFeatures[8];
    U8              btExtFeatures[2][8];
    BtInquiryMode   inqMode;           /* Current inquiry mode */
    BtErrDataReporting errDataRep;     /* Current state of Erroneous Data Rep */
    BtInqRspTxPower inqRspTxPwr;       /* Current value for Inq Rsp TX Power */

    EvmTimer        timer;          /* Timer for ME_RadioShutdown */
    U16             pendingEvent;   /* Pending ME_RadioShutdown event */
    BtChannelClass  channelClass;   /* Channel Classification Map for AFH */
    U8              autoMode;       /* AFH auto mode enabled */
    BOOL            commandPending;

    BtLinkSupervTimeout   LinkSupervTimeout;
} BtMeContext;


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Prototype:     void ME_AddOperation(BtOperation* op)
 *
 * Description:   Add an operation to the operation queue. 
 *
 * Parameters:    op - pointer to operation to add 
 *
 * Returns:       void 
 */
#define MeAddOperation(o) (InsertTailList(&MEC(opList), &((o)->op.node)))
#define MeAddOperationFront(o) (InsertHeadList(&MEC(opList), &((o)->op.node)))
#define MeAddOperationPend(o) (InsertTailList(&MEC(pendList), &((o)->op.node)))

/*---------------------------------------------------------------------------
 *
 * Prototype:     void MeStartOperation(void)
 *
 * Description:   Add an operation to the operation queue. 
 *
 * Parameters:    none 
 *
 * Returns:       void 
 */
void MeStartOperation(void);

void MeOperationEnd(void);
void MeStartInquiry(BtInquiryOp* op);
void MeReportResults(BtEventMask mask);
void MeHandleCompleteTask(U8 event, U8  len, U8* parm);
void MeHandleAccessModeOp(BtOpEvent event);
void MeHandleWriteScanActivityOp(BtOpEvent event);
BOOL MeIsScanValsLegal(U16 interval, U16 window);
void MeChangeAccessibleMode(BOOL firstCon);
BOOL MeIsAccessModeInfoEqual(BtAccessModeInfo* info1, BtAccessModeInfo* info2);
BtRemoteDevice* MeGetFreeRemoteDevice(void);
void MeHandleLinkConnectOp(BtOpEvent event);
void MeHandleConnectionStatusOnly(U8 event, U8  len, U8* parm);
void MeCallLinkHandlers(BtRemoteDevice* remDev);
//BOOL MeHandleEvents(U8 event, U8  len, U8* parm);
BOOL MeHandleEvents(HciEvent *pEvent);
void MeHandleLinkConnectReq(U8 len, U8* parm);
void MeHandleLinkAcceptOp(BtOpEvent event);
void MeHandleConnectComplete(U8 len, U8* parm);
void MeReportLinkUp(BtRemoteDevice* remDev, BtEventType eType);
BtRemoteDevice* MeMapHciToRemoteDevice(HciHandle hciHandle);
void MeDisconnectLink(BtRemoteDevice* remDev);
void MeDelayDisconnectLink(BtRemoteDevice* remDev);
void MeHandleDisconnectComplete(U8 len, U8* parm);
void MeHandleLinkDisconnectOp(BtOpEvent event);
void MePendingCommandHandler(EvmTimer *timer);

BtSecurityOp* SecBtGetFreeSecurityOp(void);
#if BT_SECURITY == XA_ENABLED
void SecHandleLinkKeyReq(U8 len, U8* parm);
void SecHandleUserIoReq(U8 len, U8* parm, U8 type);
void SecHandleAuthenticateOp(BtOpEvent event);
void SecHandleLinkKeyRspOp(BtOpEvent event);
void SecHandleUserIoRspOp(BtOpEvent event);
void SecHandleSimplePairingComplete(U8 len, U8 *parm);
void SecHandleAuthenticateComplete(U8  len, U8* parm);
void SecHandleLinkKeyNotify(U8 len, U8* parm);
BtStatus SecAddAuthenticateOpToQueue(BtRemoteDevice* remDev);
BtStatus SecProcessSecurityToken(BtSecurityToken* token);
#if BT_SECURITY_TIMEOUT != 0
void SecAccessTimeoutHandler(EvmTimer *timer);
#endif /* BT_SECURITY_TIMEOUT */
void SecHandleAuthorizeOp(BtOpEvent event);
void SecHandleAuthorizeRequest(BtRemoteDevice *remDev);
void SecHandleEncryptOp(BtOpEvent event);
void SecProcessAllSecurityTokens(BtRemoteDevice* remDev);
void SecHandleEncryptChange(U8 len, U8* parm, BOOL refresh);
BtStatus SecSetSecurityMode(BtHandler* handler, BtSecurityMode mode, BOOL encryption);
void SecHandleSecurityModeOp(BtOpEvent event);
void SecReportAuthenticateResult(BtRemoteDevice* remDev);
void SecReportEncryptionResult(BtRemoteDevice* remDev, U8 mode);
void SecReportSecurityModeResult(void);
void SecHandleReturnLinkKeys(U8 len, U8* parm);
void SecHandleIoCapReq(U8 len, U8* parm);
void SecHandleIoCapRspOp(BtOpEvent event);
void SecHandleGetOobDataOp(BtOpEvent event);
void SecHandleIoCapRsp(U8 len, U8* parm);
void SecHandlePassKeyReq(U8 len, U8* parm);
void SecHandleKeypress(U8 len, U8* parm);
void SecHandleUserPassKeyNotify(U8 len,U8* parm);
void SecHandleConfirmReq(U8 len, U8* parm);
void SecHandleRemOobDataReqRspOp(BtOpEvent event);
void SecHandleRemOobDataReq(U8 len, U8* parm);
void SecHandleDenyOobDataReqRspOp(BtOpEvent event);
void SecHandleSetSmpDbgModeOp(BtOpEvent event);
U8 SecGetPinLen(BtRemoteDevice* remDev);
BOOL SecIsLinkKeyGoodEnough(BtRemoteDevice* remDev, BtDeviceRecord *record);
BOOL SecIsCurrentSecurityGoodEnough(BtSecurityToken* token);
void SecSetIoCapRspRejectExt(BOOL (*ext_fn)(void*));
#else /* BT_SECURITY == XA_DISABLED */
void SecHandleDenyUserIoReq(U8 len, U8 *parm);
#endif /* BT_SECURITY */

BtRemoteDevice* MeEnumerateRemoteDevices(I8 i);
void MeReportInqCancelComplete(void);
void MeHandleRemNameReqOp(BtOpEvent event);
MeCommandToken* MeFindMatchingToken(MeCommandToken* token, ListEntry* list);
void MeReportMeCommandComplete(MeCommandToken* token);
void MeHandleRemNameReqComplete(U8 len, U8* parm);
void MeProcessToken(MeCommandToken *token);
void MeHandleLinkRejectOp(BtOpEvent event);
void MeSuspendTransmitters(void);
void MeRestartTransmitters(void);
void MeHandleHoldModeOp(BtOpEvent event);
void MeHandleModeChange(U8 len, U8* parm);
void MeHandleSniffModeOp(BtOpEvent event);
void MeHandleParkModeOp(BtOpEvent event);
void MeHandleExitParkModeOp(BtOpEvent event);
void MeHandleExitSniffModeOp(BtOpEvent event);
void MeHandleSetSniffSubratingParmsOp(BtOpEvent event);
void MeHandleSwitchRoleOp(BtOpEvent event);
void MeHandleRoleChange(U8 len, U8* parm);
void MeHandleConnPacketTypeChange(U8 len, U8* parm);
void MeHandleQosSetupComplete(U8 len, U8* parm);
void MeHandleDiscoverRole(BtOpEvent event);
void MeHandleReadRemVersion(U8 len, U8* parm);
void MeHandleReadRemFeatures(U8 len, U8* parm);
void MeHandleReadRemExtFeatures(U8 len, U8* parm);
void MeHandleRemHostSuppFeatures(U8 len, U8* parm);
void MeHandleLinkSupervTimeout(U8 len, U8* parm);
void MeHandleSniffSubrating(U8 len, U8* parm);
void MeWriteClassOfDevice(void);
void MeWriteDefaultLinkPolicy(void);
#if BT_BEST_SYNC_CONFIG == XA_ENABLED
void MeWriteSyncConfig(void);
#endif
void MeWriteLinkSupervTimeout(void);
void HandleBtQosOp(BtOpEvent event);
void MeWriteLocalName(void);
void MeChangeConnPacketTypeOp(BtOpEvent event);
void MeHandleReadRemVersionOp(BtOpEvent event);
void MeHandleReadRemFeaturesOp(BtOpEvent event);
void MeHandleReadRemExtFeaturesOp(BtOpEvent event);
void SecMeHandleRefreshEncryptKeyOp(BtOpEvent event);
BOOL MeIsValidGeneralToken(MeCommandToken* token);
void MeCheckRemDevToken(MeCommandToken *token);
BtStatus ME_SendHciCommand(MeCommandToken *token);
MeCommandToken* MeFindGeneralEvent(BtOpEvent event, U8 *parm);
void MeProcessGenToken(MeCommandToken *token);
void MeReportNMResults(void);
BtStatus MeSendHciCommand(HciCommandType opCode, U8 parmLen);
void MeHandleWriteLinkPolicyOp(BtOpEvent event);
void MeHandleCancelConnectOp(BtOpEvent event);
void MeHandleCancelRemNameOp(BtOpEvent event);
BtStatus MeCheckModeChange(BtRemoteDevice *remDev, BtOpType bop);
void MeHandleDataReqWhileNotActive(BtRemoteDevice *remDev);
U8 MeSetBits(U8 byte, U8 bit, U8 n, BOOL state);
#define MeSetBits(a, b, n, u) ((a & ((~0 << (b)) | (~(~0 << (b - n))))) |  \
                                   (((u ? 0xff : 0x00) & ~(~0 << n)) << (b - n)));


#if NUM_KNOWN_DEVICES > 0
void MeDevEventHandler(void);
void MeDevInquiryStart(void);
#if SDP_CLIENT_SUPPORT == XA_ENABLED
void MeDevSdpServiceFound(const SdpQueryToken *SdpToken);
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#else
#define MeDevEventHandler()         while (0)
#define MeDevInquiryStart()         while (0)
#define MeDevSdpServiceFound(_SDP)  while (0)
#endif /* NUM_KNOWN_DEVICES > 0 */

#if NUM_SCO_CONNS > 0
void ScoHandleLinkAcceptOp(BtOpEvent event);
BtScoConnect *ScoMapHciToConnect(HciHandle hcihandle);
void MeScoDisconnectLink(BtScoConnect *scoConnect);
void ScoHandleLinkRejectOp(BtOpEvent event);
void ScoHandleVoiceSettingsOp(BtOpEvent event);
void ScoChangeConnPacketTypeOp(BtOpEvent event);
void ScoHandleConnectComplete(U8 len, U8* parm);
void ScoHandleConnChanged(U8 len, U8* parm);
void ScoHandleLinkConnectOp(BtOpEvent event);
void ScoHandleLinkSetupOp(BtOpEvent event);
void ScoHandleLinkDisconnectOp(BtOpEvent event);
BOOL ScoHandleLinkConnectReq(U8 len, U8* parm);
BOOL ScoHandleDisconnectComplete(U8 len, U8* parm);
void ScoHandleDisconnectAcl(BtRemoteDevice *remDev);
void ScoReportScoLinkUp(BtScoConnect *scocon, BtEventType evnt);
BtScoConnect *ScoGetFreeConnect(void);

#if BT_SCO_HCI_DATA == XA_ENABLED
void SCO_Init(void);
BtStatus Sco_Send(BtScoConnect *scocon, BtPacket *Packet);
void ScoDataCallback(U8 event, HciCallbackParms *parms);
#endif /* BT_SCO_HCI_DATA == XA_ENABLED */

#endif /* NUM_SCO_CONNS */

BtStatus Me_switch_sco(uint16_t  scohandle);

BtStatus Me_WriteMemory(uint32_t addr, uint32_t val);

#if defined( __TWS_RECONNECT_USE_BLE__) || defined(__TWS_CHARGER_BOX__)

void MeHandleBleAdvReport(U8 len, U8* parm);
BtStatus ME_Ble_SetAdv_data(U8 len, U8 *data);
BtStatus ME_Ble_SetAdv_parameters(adv_para_struct *para);
BtStatus ME_Ble_SetAdv_en(U8 en);
BtStatus ME_Ble_SetAdv_disen(void);
BtStatus ME_Ble_Setscan_parameter(scan_para_struct *para);
BtStatus ME_Ble_Setscan_en(U8 scan_en,  U8 filter_duplicate);
BtStatus ME_Ble_Set_Private_Address(BT_BD_ADDR *addr);
BtStatus ME_Ble_Add_Dev_To_Whitelist(U8 addr_type,BT_BD_ADDR *addr);
BtStatus ME_Ble_Clear_Whitelist(void);
#endif


/* -- From ME tester --------------------------------------------------- */
#if JETTEST == XA_ENABLED
extern void (*Tester_MeReceive)(U8 event, HciCallbackParms *parms);
#endif /* JETTEST == XA_ENABLED */

#endif /* __MEI_H */
