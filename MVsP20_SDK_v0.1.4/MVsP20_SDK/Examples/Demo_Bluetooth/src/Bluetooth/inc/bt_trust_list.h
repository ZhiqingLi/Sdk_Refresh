//////////////////////////////////////////////////////////////////////////////////////
//
//	bt_db_trust_list.h
//

#ifndef __BT_TRUST_LIST_H__
#define __BT_TRUST_LIST_H__

#include "type.h"
#include "bt_stack_types.h"

typedef struct _BT_TRUST_NODE
{
	BT_DB_RECORD	record;
	struct _BT_TRUST_NODE*	pre;
	struct _BT_TRUST_NODE*	next;
} BT_TRUST_NODE;

typedef struct _BT_TRUST_LIST
{
	uint8_t			count;
	BT_TRUST_NODE	*head;
	BT_TRUST_NODE	*tail;
} BT_TRUST_LIST;

/**************************************************************************
 * InitTrustList()
 *
 *		Initialize trust list
 *
 * Parameters:
 *		localAddr - the address of local device
 *		max_count - the max number of devices in the trust list
 *
 * Returns:
 *		TRUE - Initialize successful
 *
 *		FALSE - Initialize failed. Maybe flash error or no space to be allocated
 *
 * Comment:
 *
 */
bool InitTrustList(const uint8_t * localAddr, uint8_t max_count);

/**************************************************************************
 * DeinitTrustList()
 *
 *		Deinitialize the trust list
 *
 * Parameters:
 *		NONE
 *
 * Returns:
 *		TRUE - Deinitialize successful
 *
 *		FALSE - Deinitialize failed
 *
 * Comment:
 *
 */
bool DeinitTrustList(void);

/**************************************************************************
 * InsertRecordToTrustList()
 *
 *		Insert one record to the trust list
 *
 * Parameters:
 *		record - the record of the trusted bt device
 *		writeToDdb - TRUE for writing record to device database
 *
 * Returns:
 *		TRUE - Insert successful
 *
 *		FALSE - Insert failed
 *
 * Comment:
 *		the remote bt devices with the same bt address can only be saved once. 
 *		If the remote bt device	update a new link key, the trust list will 
 *		replace	the dirty link key.
 */
bool InsertRecordToTrustList(const BT_DB_RECORD * record, bool writeToDdb);

/**************************************************************************
 * RemoveRecordFromTrustList()
 *
 *		Remove the record from the trust list
 *
 * Parameters:
 *		bdAddr - the bt address of the trusted bt device
 *
 * Returns:
 *		TRUE - Remove successful
 *
 *		FALSE - Remove failed
 *
 * Comment:
 */
bool RemoveRecordFromTrustList(const uint8_t * bdAddr);

/**************************************************************************
 * GetCountOfDevicesFromTrustList()
 *
 *		Get the number of trusted devices in the trust list. 
 *
 * Parameters:
 *		NONE.
 *
 * Returns:
 *		Count - the number of the trusted deivce
 *
 * Comment:
 */
uint8_t GetCountOfDevicesFromTrustList(void);

/**************************************************************************
 * FindRecordInTrustList()
 *
 *		Get the record in the trust list with parameter bt address. 
 *
 * Parameters:
 *		bdAddr - the bt address of the reomte device
 *
 * Returns:
 *		TRUE - Get the record successful, and the buffer of record will be
 *			filled.
 *
 *		FALSE - Get the record failed or there is no any record matching  the 
 *			given bt address
 *
 * Comment:
 *
 */
bool FindRecordInTrustList(const uint8_t * bdAddr, BT_DB_RECORD * record);

/**************************************************************************
 * FindRecordInTrustListByIndex()
 *
 *		Get the record in the trust list by index. 
 *
 * Parameters:
 *		index - the index in trust list
 *
 * Returns:
 *		TRUE - Get the record successful, and the buffer of record will be
 *			filled.
 *
 *		FALSE - Get the record failed or there is no any record matching  the 
 *			given bt address
 *
 * Comment:
 *
 */
bool FindRecordInTrustListByIndex(uint8_t index, BT_DB_RECORD * btDbRecord);

#endif //__BT_TRUST_LIST_H__

