/****************************************************************************
 *
 * File:
 *     $Id: ddb.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     Definitions the API used by the stack to access a system-specific
 *     device database.
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

#ifndef __DDBH
#define __DDBH


#ifdef __cplusplus
extern "C" {
#endif
#include "bttypes.h"
#include "me.h"


/*---------------------------------------------------------------------------
 * Device Database API layer
 *
 *     The Device Database is used by the Management Entity to store link
 *     keys and other information in a non-volatile way. This allows security
 *     information about a device to be used in later sessions, even if the
 *     device is switched off.
 *
 *     The database accessed by these calls must be maintained in
 *     non-volatile memory, such as flash RAM or a hard disk drive.
 */

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * DDB_Open()
 *
 *     Called by the stack to open the device database. This function is
 *     called during stack initialization, before any other DDB_ calls
 *     are made.
 *
 * Parameters:
 *     bdAddr - 48-bit address of the local radio.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if operation was successful to
 *         indicate that other device database calls may be made.
 *
 *     BT_STATUS_FAILED - Return this if the operation failed. Overall
 *         stack initialization (EVM_Init) will fail as a result.
 */
BtStatus DDB_Open(const BT_BD_ADDR *bdAddr);


/*---------------------------------------------------------------------------
 * DDB_Close()
 *
 *     Called by the stack to close the device database. This function is
 *     called during stack deinitialization.
 *
 * Returns:
 *     Ignored by the stack.
 */
BtStatus DDB_Close(void);


/*---------------------------------------------------------------------------
 * DDB_Flush()
 *
 *     Called by the stack to flush the device database to disk. This function 
 *     is called when new devices are added to the database. 
 *
 * Returns:
 *     Ignored by the stack.
 */
BtStatus DDB_Flush(void);


/*---------------------------------------------------------------------------
 * DDB_AddRecord()
 *
 *     Called by the stack to add or replace a record in the database.
 *     If the device record indicates a BD_ADDR that is already in the
 *     database, this function should replace the existing record.
 *
 * Parameters:
 *     record - Pointer to the record which is to be copied into the
 *         database. The structure should not be used by DDB after this call
 *         has returned.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this to indicate that the record
 *         was successfully written to the database.
 *
 *     BT_STATUS_FAILED - Return this to indicate that the record could
 *         not be written to the database.
 */
BtStatus DDB_AddRecord(const BtDeviceRecord* record);


/*---------------------------------------------------------------------------
 * DDB_FindRecord()
 *
 *     Called by the stack to copy a record out of the database. The record
 *     is identified by the BD_ADDR that represents the device.
 *
 * Parameters:
 *     bdAddr - 48-bit address that identifies the desired record.
 * 
 *     record - pointer to the record structure to fill with the
 *         specified record's information.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if the record was found and copied
 *         into the "record" parameter.
 *
 *     BT_STATUS_FAILED - Returns this if the record could not be found
 *         or an error occurred while accessing non-volatile memory.
 */
BtStatus DDB_FindRecord(const BT_BD_ADDR *bdAddr, BtDeviceRecord* record);


/*---------------------------------------------------------------------------
 * DDB_DeleteRecord()
 *
 *     Called by the stack to remove a record from the database. The record
 *     is identified by the BD_ADDR that represents the device.
 * 
 * Parameters:
 *     bdAddr - 48-bit address contained in the record to delete.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if the record was found and deleted.
 *
 *     BT_STATUS_FAILED - Return this if the record could not be found or
 *         deleted.
 */
BtStatus DDB_DeleteRecord(const BT_BD_ADDR *bdAddr);


/*---------------------------------------------------------------------------
 * DDB_EnumDeviceRecords()
 *
 *     Called by the stack to read a specific record from the device
 *     database. Records are stored with indexes between 0 and n-1 where n
 *     is the number of records in the database.
 *
 *     The stack calls this function to read device records from 0 to n-1
 *     in increasing sequence. The stack does not call other DDB functions
 *     during enumeration.
 *
 * Parameters:
 *     index - index of desired record. The first record is at index 0.
 *
 *     record - pointer to the record structure to fill with the
 *         specified record's information.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if the record was located and copied
 *         into the "record" parameter.
 *
 *     BT_STATUS_FAILED - Return this if the record could not be found or
 *         copied. For example, BT_STATUS_FAILED should be returned if the
 *         specified index is invalid.
 */
BtStatus DDB_EnumDeviceRecords(I16 index, BtDeviceRecord* record);

#if FPGA==0
void DDB_List_Saved_Flash(void);

void DDBlist_Erased_Flash(void);
typedef struct _DDB_List
{
    struct _DDB_List *next;
    BtDeviceRecord record;
} DDB_List;
#endif

#ifdef __cplusplus
}
#endif

#endif /* __DDBH */

