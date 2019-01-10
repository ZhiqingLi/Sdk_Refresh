/***************************************************************************
 *
 * File:
 *     $Id: rxbuff.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     This file contains receive buffer structures for the HCI transport.
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

#ifndef __RXBUFF_H
#define __RXBUFF_H

#include "bttypes.h"
#include "hcitrans.h"

#if IAG_BLE_INCLUDE == XA_ENABLED
typedef struct
{
    uint16_t          event;
    uint16_t          len;
    uint16_t          offset;
    uint16_t          layer_specific;
  //  uint8_t           data[];
} BT_HDR_TMP;
#define BT_HDR_SIZE_TMP (sizeof (BT_HDR_TMP))

typedef struct _buffer_hdr_tmp
{
    struct _buffer_hdr_tmp *p_next;   /* next buffer in the queue */
	uint8_t   q_id;                 /* id of the queue */
	uint8_t   task_id;              /* task which allocated the buffer*/
	uint8_t   status;               /* FREE, UNLINKED or QUEUED */
	uint8_t   Type;
        uint32_t   iag_buffer;
} BUFFER_HDR_T_TMP;
#define BUFFER_HDR_SIZE_TMP     (sizeof(BUFFER_HDR_T_TMP))                  /* Offset past header */
#endif
/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*-------------------------------------------------------------------------
 * TRANS_MAX_ACL_BUFF_SIZE constant
 *
 *     Defines the amount of memory to be allocated by the transport layer
 *     for each ACL receive buffer.
 *
 *     This value is based on HCI_ACL_DATA_SIZE. It should not be modified
 *     directly.
 */
#if IAG_BLE_INCLUDE == XA_ENABLED
#define TRANS_MAX_ACL_BUFF_SIZE  (HCI_ACL_DATA_SIZE + 4 + BUFFER_HDR_SIZE_TMP + BT_HDR_SIZE_TMP)
#else
#define TRANS_MAX_ACL_BUFF_SIZE  (HCI_ACL_DATA_SIZE + 4)
#endif
/*-------------------------------------------------------------------------
 * TRANS_NUM_ACL_BUFFERS constant
 * 
 *     Defines the number of buffers to be allocated by the transport
 *     layer for receiving ACL data.
 *
 *     This value is based on HCI_NUM_ACL_BUFFERS and should not be modified
 *     directly.
 */
#define TRANS_NUM_ACL_BUFFERS  HCI_NUM_ACL_BUFFERS

/*-------------------------------------------------------------------------
 * TRANS_MAX_SCO_BUFF_SIZE constant
 *
 *     Defines the amount of memory to be allocated by the transport layer
 *     for each SCO receive buffer.
 *
 *     This value is based on HCI_SCO_DATA_SIZE and should not be modified
 *     directly.
 */
#define TRANS_MAX_SCO_BUFF_SIZE  (HCI_SCO_DATA_SIZE + 3)

/*-------------------------------------------------------------------------
 * TRANS_NUM_SCO_BUFFERS constant
 *
 *     Defines the number of buffers to be allocated by the transport layer
 *     for receiving SCO data.
 *
 *     This value is based on HCI_NUM_SCO_BUFFERS and should not be modified
 *     directly.
 */
#define TRANS_NUM_SCO_BUFFERS  HCI_NUM_SCO_BUFFERS

/*-------------------------------------------------------------------------
 * TRANS_EVENT_BUFF_SIZE constant
 *
 *     Defines the amount of memory to be allocated by the transport layer
 *     for each HCI event buffer.
 */
#if IAG_BLE_INCLUDE == XA_ENABLED
#define TRANS_EVENT_BUFF_SIZE  (257 + BUFFER_HDR_SIZE_TMP + BT_HDR_SIZE_TMP)
#else
#define TRANS_EVENT_BUFF_SIZE  257     /* 255 bytes of data + 2 byte header */
#endif
/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/* Receive buffer context
 * 
 * Contains receive buffers and management lists. 
 */
typedef struct _BtRxbuffContext {
    
    /* Lists and Queues */
#if NUM_SCO_CONNS != 0
    ListEntry   rxSCOFreeList;
#endif /* NUM_SCO_CONNS != 0 */
    ListEntry   rxACLFreeList;
    ListEntry   rxEvtFreeList;

    /* RX buff structures */
#if NUM_SCO_CONNS != 0
    HciBuffer rxSCOBuffers[TRANS_NUM_SCO_BUFFERS];  
#endif /* NUM_SCO_CONNS != 0 */
    HciBuffer rxACLBuffers[TRANS_NUM_ACL_BUFFERS];  
    HciBuffer rxEvtBuffers[HCI_NUM_EVENTS];  

    /* Allocated memory for Rx data */
#if NUM_SCO_CONNS != 0
    U8  rxSCOBuffer[TRANS_NUM_SCO_BUFFERS * TRANS_MAX_SCO_BUFF_SIZE]; 
#endif /* NUM_SCO_CONNS != 0 */
    U8  rxACLBuffer[TRANS_NUM_ACL_BUFFERS * TRANS_MAX_ACL_BUFF_SIZE]; 
    U8  rxEvtBuffer[HCI_NUM_EVENTS * TRANS_EVENT_BUFF_SIZE];

} BtRxbuffContext;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/* Internal references to buffer allocation routines. */
BtStatus RXBUFF_Init(void);
void RXBUFF_Deinit(void);
BtStatus RXBUFF_Alloc(RxBuffHandle *rxBuffhandle, HciConnHandle hciConnHandle, 
                      U16 len, U8 type);
void RXBUFF_Free(RxBuffHandle rxBuffhandle);
BtStatus RXBUFF_SetLength(RxBuffHandle rxBuffHandle, U16 len);

#endif /* __RXBUFF_H */


