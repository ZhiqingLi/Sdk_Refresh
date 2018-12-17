//////////////////////////////////////////////////////////////////////////////////////
//
//
//

#include <string.h>

#include "type.h"

#include "bt_trust_list.h"

static BT_TRUST_LIST		btTrustList;
static BT_TRUST_NODE * 		nodePointer = NULL;

#define GET_TRUST_LIST		&btTrustList


static bool LoadTrustListFromBtDdb(BT_TRUST_LIST * list, uint8_t count);
static bool DumpTrustListToBtDdb(void);
static bool GetFirstRecordFromTrustList(BT_DB_RECORD * record);
static bool GetNextRecordFromTrustList(BT_DB_RECORD * record);


bool InitTrustList(const uint8_t * localBdAddr, uint8_t maxCount)
{
	BT_TRUST_LIST * 	list;
	uint8_t				count = 0;

	/*
	* 1. Init trust list struct
	*/
	list = GET_TRUST_LIST;
	if(list == NULL)
	{
		return FALSE;
	}

	list->count = 0;
	list->head = NULL;
	list->tail = NULL;

	/*
	* 2. Open database
	*
	*/
	count = BtDdb_Open(localBdAddr);

	/*
	* 3. Get records and insert it to trust list
	*/
	LoadTrustListFromBtDdb(list, count);

	return TRUE;
}

bool DeinitTrustList(void)
{
	BT_TRUST_NODE * 	temp_node_pointer;
	BT_TRUST_LIST * 	list;

	list = GET_TRUST_LIST;
	if(list == NULL)
	{
		return FALSE;
	}

	temp_node_pointer = list->head;
	while(temp_node_pointer != NULL)
	{
		list->head = list->head->next;
//		mmm_free(temp_node_pointer);
		vPortFree(temp_node_pointer);
		temp_node_pointer = list->head;
	}


	list->count = 0;
	list->head = NULL;
	list->tail = NULL;

	nodePointer = NULL;

	BtDdb_Close();

	return TRUE;
}

bool InsertRecordToTrustList(const BT_DB_RECORD * record, bool writeToDdb)
{
	BT_TRUST_LIST * 	list;
	BT_TRUST_NODE * 	node;
	BT_TRUST_NODE * 	temp_node_pointer;

	/* paramters check */
	if(record == NULL)
		return FALSE;

	node = (BT_TRUST_NODE*)pvPortMalloc(sizeof(BT_TRUST_NODE));
	if(node == NULL)
	{
		return FALSE;
	}

	list = GET_TRUST_LIST;
	if(list == NULL)
	{
		return FALSE;
	}

	memcpy(&node->record , record, sizeof(BT_DB_RECORD));

	/*
	* 1. insert the new one into trust list
	*/
	if(list->count == 0)
	{
		node->pre = NULL;
		node->next = NULL;

		list->head = node;
		list->tail = node;
	}
	else
	{
		temp_node_pointer = list->head;
		while(temp_node_pointer)
		{
			if(memcmp(temp_node_pointer->record.bdAddr, node->record.bdAddr, 6) == 0)
			{
				/*
				* if the device has been listed in trust list,
				* replace the record with the new one
				*/
				memcpy(&temp_node_pointer->record, &node->record, sizeof(BT_DB_RECORD));
				goto WRITE_TO_DB;
			}
			else
			{
				temp_node_pointer = temp_node_pointer->next;
			}
		}

		/*
		* if can not find any record equal to the new one
		* insert the new one to the trust list
		*/

		list->tail->next = node;
		node->next = NULL;
		node->pre = list->tail;
		list->tail = node;
	}

	list->count++;

WRITE_TO_DB:
	/*
	* 2. Write to the bt device database
	*/
	if(writeToDdb)
	{
		DumpTrustListToBtDdb();
	}

	return TRUE;
}

bool RemoveRecordFromTrustList(const uint8_t * bdAddr)
{
	BT_TRUST_NODE *		temp;
	BT_TRUST_LIST * 	list;

	if(bdAddr == NULL)
		return FALSE;

	list = GET_TRUST_LIST;
	if(list == NULL)
	{
		return FALSE;
	}

	temp = list->head;
	while(temp != NULL)
	{
		if(memcmp(temp->record.bdAddr, bdAddr, 6) == 0)
		{
			/* remove from trust list*/
			if(temp == list->head)
			{
				list->head = temp->next;
				temp->next->pre = NULL;
			}
			else if(temp == list->tail)
			{
				list->tail = temp->pre;
				temp->pre->next = NULL;
			}
			else
			{
				temp->pre->next = temp->next;
				temp->next->pre = temp->pre;
			}

//			mmm_free(temp);
			vPortFree(temp);

			break;
		}
		temp = temp->next;
	}

	DumpTrustListToBtDdb();

	return TRUE;
}

uint8_t GetCountOfDevicesFromTrustList()
{
	BT_TRUST_LIST *			list;


	list = GET_TRUST_LIST;

	if(list == NULL)
	{
		return 0;
	}

	return list->count;
}

bool FindRecordInTrustList(const uint8_t * bdAddr, BT_DB_RECORD * record)
{
	BT_TRUST_NODE *			temp_node_pointer;
	BT_TRUST_LIST *			list;

	if(bdAddr == NULL || record == NULL)
		return FALSE;

	list = GET_TRUST_LIST;
	if(list == NULL)
	{
		return FALSE;
	}

	temp_node_pointer = list->head;
	while(temp_node_pointer != NULL)
	{
		if(memcmp(temp_node_pointer->record.bdAddr, bdAddr, 6) == 0)
		{
			memcpy(record, &temp_node_pointer->record, sizeof(BT_DB_RECORD));
			return TRUE;
		}
		temp_node_pointer = temp_node_pointer->next;
	}

	return FALSE;
}

bool FindRecordInTrustListByIndex(uint8_t index, BT_DB_RECORD * btDbRecord)
{
	uint8_t			i;

	if(index >= GetCountOfDevicesFromTrustList())
		return FALSE;

	if(!GetFirstRecordFromTrustList(btDbRecord))
		return FALSE;

	for(i = 0; i < index; i++)
	{
		if(!GetNextRecordFromTrustList(btDbRecord))
			return FALSE;
	}

	return TRUE;
}

static bool LoadTrustListFromBtDdb(BT_TRUST_LIST * list, uint8_t count)
{
	BT_DB_RECORD		record;

	if(BtDdb_GetFirstRecord(&record))
	{
		if(!InsertRecordToTrustList(&record, FALSE))
			goto END;

		count--;

		while(count > 0)
		{
			if(BtDdb_GetNextRecord(&record))
			{
				if(!InsertRecordToTrustList(&record, FALSE))
					goto END;

				count--;
			}
		}
	}

END:
	return TRUE;
}

static bool DumpTrustListToBtDdb(void)
{
	BT_DB_RECORD		record;


	if(!BtDdb_PrepareVaildFreeSpace())
		return FALSE;


	if(GetFirstRecordFromTrustList(&record))
	{
		if(!BtDdb_AddOneRecord(&record))
			return FALSE;

		while(GetNextRecordFromTrustList(&record))
		{
			if(!BtDdb_AddOneRecord(&record))
				return FALSE;
		}
	}

	return TRUE;
}

static bool GetFirstRecordFromTrustList(BT_DB_RECORD * record)
{
	BT_TRUST_LIST *		list;

	list = GET_TRUST_LIST;
	if(list == NULL)
	{
		return FALSE;
	}

	nodePointer = list->head;

	if(nodePointer == NULL)
		return FALSE;

	memcpy(record, &nodePointer->record , sizeof(BT_DB_RECORD));

	return TRUE;
}

static bool GetNextRecordFromTrustList(BT_DB_RECORD * record)
{
	if(nodePointer == NULL || record == NULL)
		return FALSE;

	nodePointer = nodePointer->next;

	if(nodePointer == NULL)
		return FALSE;

	memcpy(record, &nodePointer->record , sizeof(BT_DB_RECORD));

	return TRUE;
}
//
//static BT_TRUST_LIST		btTrustList;
//static BT_TRUST_NODE * 		nodePointer = NULL;
//
//#define GET_TRUST_LIST		&btTrustList
//
//
//static bool LoadTrustListFromBtDdb(BT_TRUST_LIST * list, uint8_t count);
//static bool DumpTrustListToBtDdb(void);
//static bool GetFirstRecordFromTrustList(BT_DB_RECORD * record);
//static bool GetNextRecordFromTrustList(BT_DB_RECORD * record);
//
//
//bool InitTrustList(const uint8_t * localBdAddr, uint8_t maxCount)
//{
//	return TRUE;
//}
//
//bool DeinitTrustList(void)
//{
//	return TRUE;
//}
//
//bool InsertRecordToTrustList(const BT_DB_RECORD * record, bool writeToDdb)
//{
//
//	return TRUE;
//}
//
//bool RemoveRecordFromTrustList(const uint8_t * bdAddr)
//{
//	return TRUE;
//}
//
//uint8_t GetCountOfDevicesFromTrustList()
//{
//	return 0;
//}
//
//bool FindRecordInTrustList(const uint8_t * bdAddr, BT_DB_RECORD * record)
//{
//	return FALSE;
//}
//
//bool FindRecordInTrustListByIndex(uint8_t index, BT_DB_RECORD * btDbRecord)
//{
//	return TRUE;
//}
//
//static bool LoadTrustListFromBtDdb(BT_TRUST_LIST * list, uint8_t count)
//{
//	return TRUE;
//}
//
//static bool DumpTrustListToBtDdb(void)
//{
//	return TRUE;
//}
//
//static bool GetFirstRecordFromTrustList(BT_DB_RECORD * record)
//{
//	return TRUE;
//}
//
//static bool GetNextRecordFromTrustList(BT_DB_RECORD * record)
//{
//	return TRUE;
//}

