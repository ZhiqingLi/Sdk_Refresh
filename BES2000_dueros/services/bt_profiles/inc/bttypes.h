/****************************************************************************
 *
 * File:
 *     $Id: bttypes.h 1411 2009-04-15 22:10:58Z suneelk $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 1411 $
 *
 * Description:
 *     General types, defines, and prototypes for Blue SDK components.
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

#ifndef __BTTYPES_H
#define __BTTYPES_H

#include "xatypes.h"
#include "btconfig.h"
#include "xastatus.h"
#include "utils.h"
#include "eventmgr.h"


/*---------------------------------------------------------------------------
 * Global Types layer
 *
 *     General types and conversion functions used by all components of
 *     the stack and applications are defined here.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BDADDR_NTOA_SIZE constant
 *
 *     Defines the minimum size for the string when using the bdaddr_ntoa
 *     function.
 */
#define BDADDR_NTOA_SIZE        18


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtStatus type
 *
 *     This type is returned from most stack APIs to indicate the success
 *     or failure of the operation. In many cases, BT_STATUS_PENDING
 *     is returned, meaning that a future callback will indicate the
 *     result of the operation.
 */
typedef XaStatus BtStatus;

#define BT_STATUS_SUCCESS           XA_STATUS_SUCCESS
#define BT_STATUS_FAILED            XA_STATUS_FAILED
#define BT_STATUS_PENDING           XA_STATUS_PENDING
#define BT_STATUS_BUSY              XA_STATUS_BUSY
#define BT_STATUS_NO_RESOURCES      XA_STATUS_NO_RESOURCES
#define BT_STATUS_NOT_FOUND         XA_STATUS_NOT_FOUND
#define BT_STATUS_DEVICE_NOT_FOUND  XA_STATUS_DEVICE_NOT_FOUND
#define BT_STATUS_CONNECTION_FAILED XA_STATUS_CONNECTION_FAILED
#define BT_STATUS_TIMEOUT           XA_STATUS_TIMEOUT
#define BT_STATUS_NO_CONNECTION     XA_STATUS_NO_CONNECTION
#define BT_STATUS_INVALID_PARM      XA_STATUS_INVALID_PARM
#define BT_STATUS_IN_PROGRESS       XA_STATUS_IN_PROGRESS
#define BT_STATUS_RESTRICTED        XA_STATUS_RESTRICTED
#define BT_STATUS_INVALID_TYPE      XA_STATUS_INVALID_TYPE
#define BT_STATUS_HCI_INIT_ERR      XA_STATUS_HCI_INIT_ERR
#define BT_STATUS_NOT_SUPPORTED     XA_STATUS_NOT_SUPPORTED
#define BT_STATUS_IN_USE            XA_STATUS_IN_USE
#define BT_STATUS_SDP_CONT_STATE    XA_STATUS_CONTINUE
#define BT_STATUS_CANCELLED         XA_STATUS_CANCELLED

/* The last defined status code */
#define BT_STATUS_LAST_CODE         XA_STATUS_LAST_CODE
/* End of BtStatus */

/*---------------------------------------------------------------------------
 * BtPacketFlags type
 *
 *     This type is used by L2CAP and protocols that use directly L2CAP
 *     to manage the status of a particular BtPacket.
 */
typedef U16 BtPacketFlags;

#define BTP_FLAG_NONE       0x0000  /* No current flags */
#define BTP_FLAG_INUSE      0x0001  /* Used only by packet owner */
#define BTP_FLAG_LSYSTEM    0x0002  /* Used only by L2CAP */
#define BTP_FLAG_TAIL       0x0004  /* Used only by L2CAP Applications */
#define BTP_FLAG_RDEV       0x0008  /* Used only by L2CAP */
#define BTP_FLAG_FCS        0x0010  /* FCS field is valid, set only by L2CAP */
#define BTP_FLAG_NON_FLUSH  0x0020  /* Used by L2CAP, HCI or packet owner */
#define BTP_FLAG_ENHANCED   0x0040  /* Used only by L2CAP */
#define BTP_FLAG_SEGMENTED  0x0080  /* Used only by L2CAP */
#define BTP_FLAG_TXDONE     0x0100  /* Used only by L2CAP */
#define BTP_FLAG_USER       0x0200  /* Used only by L2CAP */
#define BTP_FLAG_IMMEDIATE  0x0400  /* Used only by L2CAP */

/* End of BtPacketFlags */


/*---------------------------------------------------------------------------
 * BtPriority type
 *
 *     Specifies the priority of an L2CAP channel or BtPacket.
 */
typedef U8 BtPriority;

/* L2CAP 1st priority. */
#define BT_PRIORITY_HI      0x01

/* L2CAP 2nd priority. */
#define BT_PRIORITY_MED     0x02

/* L2CAP 3rd priority. */
#define BT_PRIORITY_NON     0x03

/* End of BtPriority */


/*---------------------------------------------------------------------------
 * HciTransError type
 *
 *     Describes the type of transport error encountered.
 */
typedef U8 HciTransError;

/* No transport error has occurred. */ 
#define HCI_TRANSERROR_NONE                0x00

/* An unknown transport error occurred */ 
#define HCI_TRANSERROR_UNKNOWN             0x01

/* Poorly formed data was received from the radio */ 
#define HCI_TRANSERROR_ILLEGAL_DATA        0x02

/* The Radio itself indicated a hardware failure */ 
#define HCI_TRANSERROR_HARDWARE            0x03

/* A command required by the stack failed during initialization */ 
#define HCI_TRANSERROR_REQUIRED_CMD_FAILED 0x04

/* A command was issued but no response was ever heard. See 
 * HCI_RESET_TIMEOUT. */ 
#define HCI_TRANSERROR_CMD_TIMEOUT         0x05

/* An error occurred while writing data to the transport */ 
#define HCI_TRANSERROR_WRITE_ERROR         0x06

/* An error occurred while reading data from the transport */ 
#define HCI_TRANSERROR_READ_ERROR          0x07

/* A buffer could not be allocated */ 
#define HCI_TRANSERROR_BUFFER_ERROR        0x08

/* The transport was externally shut down or no longer responds */ 
#define HCI_TRANSERROR_SHUTDOWN            0x09

/* End of HciTransError */ 


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtPacket structure
 *
 *     Represents a packet of data. These packets may be used by applications
 *     to provide data to the stack. Certain stack events may also provide
 *     data to the application enclosed in a BtPacket structure.
 */
typedef struct _BtPacket
{
    ListEntry    node;    /* Used internally by the stack. */

    U8          *data;    /* Points to a buffer of user data.  */
    U16          dataLen; /* Indicates the length of "data" in bytes. */

    U16          flags;   /* Must be initialized to BTP_FLAG_NONE by
                           * applications running on top of L2CAP. Other
                           * higher layer protocols must never modify this
                           * value.
                           */

#if L2CAP_PRIORITY == XA_ENABLED
    BtPriority   priority; /* May be used to override the L2CAP channel
                            * BtPriority. If set to 0, packet will use the
                            * default priority set for the L2CAP channel.
                            */
#endif

    /* Group: The following fields are for internal use only by the stack. */
    void        *ulpContext;
    U8          *tail;
    U16          tailLen;

#if XA_STATISTICS == XA_ENABLED
    U32          rfc_timer;
    U32          hci_timer;
    U32          l2cap_timer;
#endif /* XA_STATISTICS == XA_ENABLED */

    U16          llpContext;
    U16          remoteCid;

#if L2CAP_NUM_ENHANCED_CHANNELS > 0
    U8           segStart;
    U16          segNum;
    U16          segCount;
    U8           fcs[2];
#endif /* L2CAP_NUM_ENHANCED_CHANNELS > 0 */

    U8           hciPackets;
    U8           headerLen;
    U8           header[BT_PACKET_HEADER_LEN];
} BtPacket;


/* Indicates size of BD_ADDR array */
#define BD_ADDR_SIZE    6

/*---------------------------------------------------------------------------
 * BD_ADDR structure
 *
 *     Represents a 48-bit Bluetooth device address.
 */
typedef struct _BT_BD_ADDR {
    U8    addr[BD_ADDR_SIZE];
} BT_BD_ADDR;


/* Internal ROM constant defined in btrom.c to represent an invalid address */
extern const BT_BD_ADDR INVALID_BDADDR;


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * bdaddr_aton()
 *
 *     Converts an ASCII-formatted Bluetooth device address into a BD_ADDR
 *     structure.
 *
 * Parameters:
 *     addr - Address in the format "xx:xx:xx:xx:xx", where x is a
 *         hexadecimal digit. The most significant byte appears first.
 *
 * Returns:
 *     BD_ADDR - A structure that contains the binary version of the
 *         address.
 */
BT_BD_ADDR  bdaddr_aton(const char *addr);


/*---------------------------------------------------------------------------
 * bdaddr_ntoa()
 *
 *     Converts a BD_ADDR structure into an ASCII-formatted device address.
 *
 * Parameters:
 *     addr - Binary device address to convert.
 *
 *     addrString - Destination string space for formatted device address.
 *         The string must contain at least BDADDR_NTOA_SIZE bytes of space.
 *
 * Returns:
 *     char * - A copy of the pointer passed in as "addrString".
 */
char    *bdaddr_ntoa(const BT_BD_ADDR *addr, char *addrString);


/*---------------------------------------------------------------------------
 * AreBdAddrsEqual()
 *
 *     Compares two Bluetooth device addresses for equality.
 *
 * Parameters:
 *     addrA - First address to compare.
 *
 *     addrB - Second address to compare.
 *
 * Returns:
 *     TRUE when the addresses are the same, FALSE when they are different
 */
BOOL    AreBdAddrsEqual(const BT_BD_ADDR *addrA, const BT_BD_ADDR *addrB);
#define AreBdAddrsEqual(_ADDRA, _ADDRB)   OS_MemCmp((_ADDRA)->addr, 6, (_ADDRB)->addr, 6)


/*---------------------------------------------------------------------------
 * IsValidBdAddr()
 *
 *     Determines whether a BD_ADDR contains a valid Bluetooth device
 *     address.
 *
 * Parameters:
 *     addr - Address to examine.
 *
 * Returns:
 *     TRUE when the address is valid, FALSE if it is invalid.
 */
BOOL    IsValidBdAddr(const BT_BD_ADDR *Addr);
#define IsValidBdAddr(_BDA) (OS_MemCmp((_BDA)->addr, 6, INVALID_BDADDR.addr, 6) == FALSE)

#endif /* __BTTYPES_H */
